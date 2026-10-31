"""Accepted current function identities -> bounded static MAY argument/object contract.
No discovery, live access, callable ABI, object lifetime or movement permission.
"""
import argparse
from collections import deque
from dataclasses import dataclass
import hashlib
import json
import struct
from pathlib import Path
import capstone as cs
from capstone import x86 as x
import resolve_motion_gravity as resolution

pe=resolution.pe
ROLES={
 'UpdateMotion':(21248208,21249549,'a8e09473b8389327a54be533a178aee62766f540a1d84670d0e50c1bd1a7a6e5'),
 'RequestSimpleMotion':(18488272,18488482,'2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c'),
 'GravityOrientationOwner':(20246272,20246459,'bc9e1a75a4d45f4dc1a1baf99b704778714e83353d58688dce82bab901514a53'),
 'SetGravityVector':(55653024,55653103,'7aaa6fefc1776035cffb9c3f291ac59c892444826dbd8b191092d71a910631aa')}
CALLS=(('UpdateMotion',21249462,'RequestSimpleMotion'),('GravityOrientationOwner',20246375,'SetGravityVector'))
MAX_EVAL=8192
MAX_EVENTS=512
MAX_STACK=128
MAX_REGUSES=128
MAX_RETURNS=64
MAX_ORIGINS=8
MAX_CHAIN=3
MAX_OUTPUT=192*1024
BANKS=[['RAX','EAX','AX','AL','AH'],['RCX','ECX','CX','CL','CH'],['RDX','EDX','DX','DL','DH'],
 ['RBX','EBX','BX','BL','BH'],['RSP','ESP','SP','SPL'],['RBP','EBP','BP','BPL'],
 ['RSI','ESI','SI','SIL'],['RDI','EDI','DI','DIL']]+[[f'R{i}',f'R{i}D',f'R{i}W',f'R{i}B'] for i in range(8,16)]
REG={}
for names in BANKS:
 for name in names:REG[getattr(x,'X86_REG_'+name)]=names[0].lower()
for i in range(16):
 for prefix in ('XMM','YMM','ZMM'):REG[getattr(x,f'X86_REG_{prefix}{i}')]='v'+str(i)
ALL=set(REG.values())
VOLATILE={'rax','rcx','rdx','r8','r9','r10','r11'}|{'v'+str(i) for i in range(6)}

@dataclass(frozen=True)
class Value:
 origins:frozenset=frozenset({'UNKNOWN'})
 address:tuple|None=None
 constant:int|None=None
 loads:frozenset=frozenset()

UNKNOWN=Value()
def bounded(s):return frozenset(s) if len(s)<=MAX_ORIGINS and 'TOP_origin_cap' not in s else frozenset({'TOP_origin_cap'})
def join(a,b):return Value(bounded(a.origins|b.origins),a.address if a.address==b.address else None,
                          a.constant if a.constant==b.constant else None,bounded(a.loads|b.loads))
def shifted(a,n):
 return Value(a.origins,(a.address[0],a.address[1]+n) if a.address else None,
              a.constant+n if a.constant is not None else None,a.loads)
def initial():
 regs={k:UNKNOWN for k in ALL}
 for k in ('rcx','rdx','r8','r9'):
  regs[k]=Value(frozenset({'entry_'+k}),('entry_'+k,0))
 for i in range(4):regs['v'+str(i)]=Value(frozenset({'entry_xmm'+str(i)}))
 regs['rsp']=Value(frozenset({'entry_stack'}),('entry_stack',0))
 return regs,{}
def view(v):return dict(origins=sorted(v.origins),address=list(v.address) if v.address else None,
                         constant=v.constant,loaded_value_origins=sorted(v.loads))
def merge(a,b):
 ra,ma=a;rb,mb=b
 return ({k:join(ra[k],rb[k]) for k in ALL},
         {k:join(ma[k],mb[k]) for k in ma.keys()&mb.keys()})


def analyze_graph(instructions,start,end,roles):
 """Bounded may-flow, exact-address candidates only until unknown/merge/clobber.
 Branches are explored both ways; UNKNOWN is never a no-alias/no-retention proof.
 """
 states={start:initial()};todo=deque([start]);events={};calls={};returns={};reguses={};barriers={};evaluations=0
 def event(row):
  # Repeated loop/path evaluations merge into one identical semantic witness.
  key=json.dumps(row,sort_keys=True)
  events[key]=row
  if len(events)>MAX_EVENTS:raise pe.Refusal('contract_event_cap')
 def barrier(site,reason):barriers[site,reason]=dict(site_rva=site,reason=reason)
 def addr(op,regs,site,size):
  m=op.mem
  if i.addr_size!=8:return None
  if m.segment!=x.X86_REG_INVALID:return None
  if m.base==x.X86_REG_RIP:return ('image',site+size+m.disp)
  bank=REG.get(m.base);a=regs.get(bank,UNKNOWN)
  index=0
  if m.index!=x.X86_REG_INVALID:
   value=regs.get(REG.get(m.index),UNKNOWN)
   if value.constant is None:return None
   index=value.constant*m.scale
  return (a.address[0],a.address[1]+m.disp+index) if a.address else None
 def load(address,width,stack):
  if address and address[0]=='entry_stack' and (address[0],address[1],width) in stack:return stack[address[0],address[1],width]
  label=(f'{address[0]}{address[1]:+d}:{width}' if address else 'unknown_address')
  # A full64 load may later be used as an address; this is an opaque static chain,
  # NOT proof of an allocated pointer, typed object or permission to dereference.
  root='load64('+label+')'
  pointer=(root,0) if width==8 and root.count('load64(')<=MAX_CHAIN else None
  return Value(frozenset({'memory_value'}),pointer,None,frozenset({label}))
 while todo:
  site=todo.popleft();evaluations+=1
  if evaluations>MAX_EVAL:raise pe.Refusal('contract_state_evaluation_cap')
  if site not in instructions:barrier(site,'CFG_edge_or_decode_unavailable');continue
  i=instructions[site];regs,stack=states[site];regs=dict(regs);stack=dict(stack);ops=i.operands
  move=i.mnemonic.startswith(('mov','vmov')) and len(ops)==2 and i.mnemonic not in ('movs','movsb','movsq','movsd')
  # MOVSD SIMD is a move when Capstone exposes operands, not string-op implicit form.
  if i.mnemonic in ('movsd','vmovsd') and len(ops)==2:move=True
  def read(op):
   if op.type==x.X86_OP_IMM:return Value(frozenset({'constant'}),None,op.imm)
   if op.type==x.X86_OP_REG:return regs.get(REG.get(op.reg),UNKNOWN)
   if op.type==x.X86_OP_MEM:return load(addr(op,regs,site,i.size),op.size,stack)
   return UNKNOWN
  values=[read(o) for o in ops]
  reads,_=i.regs_access()
  for reg in reads:
   bank=REG.get(reg);value=regs.get(bank,UNKNOWN)
   origins=[o for o in value.origins if o.startswith(('entry_rcx','entry_rdx','entry_r8','entry_r9','entry_xmm'))]
   if origins:
    name=i.reg_name(reg);width=(64 if name.startswith('zmm') else 32 if name.startswith('ymm') else 16 if name.startswith('xmm') else 8 if name==bank else 4 if name.startswith('e') or name.endswith('d') else 1 if name.endswith(('l','h','b')) else 2)
    key=(tuple(origins),name,width)
    reguses.setdefault(key,set()).add(site)
    if len(reguses)>MAX_REGUSES:raise pe.Refusal('contract_register_use_cap')
  if i.group(cs.CS_GRP_RET):
   row=dict(site_rva=site,rax=view(regs['rax']),xmm0_may=view(regs['v0']))
   returns[json.dumps(row,sort_keys=True)]=row
   if len(returns)>MAX_RETURNS:raise pe.Refusal('contract_return_cap')
  for index,o in enumerate(ops):
   if o.type!=x.X86_OP_MEM or i.id==x.X86_INS_LEA:continue
   address=addr(o,regs,site,i.size)
   access=o.access
   if move:access=cs.CS_AC_WRITE if index==0 else cs.CS_AC_READ
   row=dict(site_rva=site,operation=i.mnemonic,width=o.size,
            address=list(address) if address else None,
            access='read_write' if access==3 else 'write' if access==2 else 'read' if access==1 else 'UNKNOWN')
   if access&cs.CS_AC_WRITE:
    source=values[1] if move and index==0 else UNKNOWN
    row['source']=view(source)
    row['full64_entry_argument_value_store_candidate']=bool(move and o.size==8 and source.address and source.address[0] in ('entry_rcx','entry_rdx','entry_r8','entry_r9'))
    row['memory_value_copy_candidate']=bool(move and source.loads)
    if address and address[0]=='entry_stack':
     stack={k:v for k,v in stack.items() if k[0]!='entry_stack' or k[1]+k[2]<=address[1] or address[1]+o.size<=k[1]}
     stack[address[0],address[1],o.size]=source
     if len(stack)>MAX_STACK:raise pe.Refusal('contract_stack_cap')
    else:
     # No argument-vs-stack disjointness is proved: object writes may alias spills.
     stack.clear()
   event(row)
  if i.group(cs.CS_GRP_CALL):
   if ops[0].type==x.X86_OP_IMM:
    target=ops[0].imm;role=next((k for k,(b,e,h) in roles.items() if b==target),None)
   else:target=None;role=None
   call=dict(site_rva=site,direct_target_rva=target,accepted_role=role,
             arguments={k:view(regs[k]) for k in ('rcx','rdx','r8','r9','v0','v1','v2','v3')},
             outgoing_stack_candidates=[],callee_retention='UNKNOWN',callee_effects='UNKNOWN_NOT_FOLLOWED',
             indirect_target_origin=view(values[0]) if target is None else None)
   sp=regs['rsp'].address
   if sp and sp[0]=='entry_stack':
    for off in (0x20,0x28,0x30,0x38):
     for width in (8,4):
      value=stack.get(('entry_stack',sp[1]+off,width))
      if value:call['outgoing_stack_candidates'].append(dict(rsp_offset=off,width=width,value=view(value)))
   key=json.dumps(call,sort_keys=True);calls[key]=call
   if len(calls)>64:raise pe.Refusal('contract_call_variant_cap')
   for bank in VOLATILE:regs[bank]=UNKNOWN
   regs['rax']=Value(frozenset({'unknown_call_return'}),('return_at_'+hex(site),0))
   stack.clear();barrier(site,'unknown_callee_may_capture_arguments_or_mutate_memory')
  else:
   try:_,writes=i.regs_access()
   except cs.CsError:raise pe.Refusal('unknown_register_effects')
   before=dict(regs)
   for reg in writes:
    if reg in REG:regs[REG[reg]]=UNKNOWN
   if i.id==x.X86_INS_PUSH and ops:
    sp=shifted(before['rsp'],-8);regs['rsp']=sp
    if sp.address:stack[sp.address[0],sp.address[1],8]=values[0]
   elif i.id==x.X86_INS_POP and ops:
    sp=before['rsp'];value=load(sp.address,8,stack)
    if ops[0].type==x.X86_OP_REG:regs[REG[ops[0].reg]]=value
    regs['rsp']=shifted(sp,8)
   elif ops and ops[0].type==x.X86_OP_REG:
    dst=REG.get(ops[0].reg)
    value=None
    if move:
     value=values[1]
     if ops[1].type!=x.X86_OP_IMM and (ops[1].size!=ops[0].size or i.mnemonic.startswith(('movsx','movzx'))):
      value=Value(value.origins,None,None,value.loads)
    elif i.id==x.X86_INS_LEA:
     address=addr(ops[1],before,site,i.size)
     value=Value(frozenset({address[0]}) if address else frozenset({'UNKNOWN'}),address)
    elif i.id in (x.X86_INS_ADD,x.X86_INS_SUB) and len(ops)==2 and values[1].constant is not None:
     value=shifted(values[0],values[1].constant*(1 if i.id==x.X86_INS_ADD else -1))
    elif i.id in (x.X86_INS_XOR,x.X86_INS_PXOR,x.X86_INS_XORPS,x.X86_INS_XORPD,x.X86_INS_VXORPS,x.X86_INS_VXORPD) and len(ops)>=2 and ops[-1].type==ops[-2].type==x.X86_OP_REG and ops[-1].reg==ops[-2].reg:
     value=Value(frozenset({'constant'}),None,0)
    if dst and value is not None:
     if dst.startswith('v'):
      # Lanes/upper state not represented: merge, never claim full vector equality.
      regs[dst]=join(before[dst],value)
      barrier(site,'SIMD_lane_copy_candidate_not_full_vector_semantics')
     elif ops[0].size==8:regs[dst]=value
     elif ops[0].size==4:regs[dst]=Value(value.origins,None,value.constant&0xffffffff if value.constant is not None else None,value.loads)
     else:regs[dst]=Value(bounded(before[dst].origins|value.origins),None,None,bounded(before[dst].loads|value.loads))
    elif writes:barrier(site,'unmodeled_register_transform_invalidates_exact_origin')
  if len(stack)>MAX_STACK:raise pe.Refusal('contract_stack_cap')
  successors=[]
  if i.group(cs.CS_GRP_RET):pass
  elif i.group(cs.CS_GRP_JUMP):
   if ops and ops[0].type==x.X86_OP_IMM:successors.append(ops[0].imm)
   else:barrier(site,'indirect_control_not_followed')
   if i.mnemonic!='jmp':successors.append(site+i.size)
  elif i.mnemonic not in ('int3','ud2','syscall','sysret'):successors.append(site+i.size)
  for nxt in successors:
   if not start<=nxt<end:barrier(site,'outside_local_function_edge_not_followed');continue
   proposed=(dict(regs),dict(stack))
   combined=merge(states[nxt],proposed) if nxt in states else proposed
   if nxt not in states or states[nxt]!=combined:states[nxt]=combined;todo.append(nxt)
 return dict(access_candidates=list(events.values()),call_candidates=list(calls.values()),
             unknown_barriers=list(barriers.values()),state_evaluations=evaluations,
             incoming_register_uses=[dict(origins=list(k[0]),register=k[1],width=k[2],sites=sorted(v)[:8],distinct_sites=len(v)) for k,v in reguses.items()],
             return_candidates=list(returns.values()),
             register_only_leaf_candidate=not events and not calls and not barriers and all(i.mnemonic in ('mov','lea','xor','add','sub','cmp','test','ret','nop') for i in instructions.values()),
             no_pointer_retention_proven=False,object_allocation_lifetime_proven=False,
             interpretation='bounded_static_MAY_candidates_not_typed_ABI_or_lane_equivalence')


def report(data,*,expected_sha=pe.EXPECTED_SHA,roles=ROLES,required_calls=CALLS):
 if len(data)>pe.MAX_FILE:raise pe.Refusal('file_cap')
 if hashlib.sha256(data).hexdigest()!=expected_sha:raise pe.Refusal('unsupported_build_sha256')
 if cs.__version__!='5.0.6':raise pe.Refusal('requires_Capstone5.0.6')
 image=resolution.Image(data);out={};graphs={}
 for role,(b,e,digest) in roles.items():
  body=image.read(b,e-b,True)
  if hashlib.sha256(body).hexdigest()!=digest:raise pe.Refusal('accepted_function_hash_mismatch_'+role)
  entry=image.covering(b)
  if role!='SetGravityVector':
   if not entry or tuple(entry[:2])!=(b,e):raise pe.Refusal('accepted_runtime_range_mismatch_'+role)
  elif entry or any(b<start<e for start in image.starts):raise pe.Refusal('setter_no_pdata_status_changed')
  graph,unknown=image.graph(b,e)
  if unknown:raise pe.Refusal('accepted_CFG_now_unresolved_'+role)
  if role=='SetGravityVector' and (e-1 not in graph or not graph[e-1].group(cs.CS_GRP_RET) or any(i.group(cs.CS_GRP_CALL) for i in graph.values())):raise pe.Refusal('setter_leaf_shape_changed')
  graphs[role]=graph
  out[role]=dict(begin_rva=b,end_rva=e,sha256=digest,boundary='PARTIAL_no_pdata_bounded_leaf' if role=='SetGravityVector' else 'accepted_PE_range_CFG',
                 reachable_instructions=len(graph),**analyze_graph(graph,b,e,roles))
 for row in out.values():
  for call in row['call_candidates']:
   target=call['direct_target_rva']
   entry=image.covering(target) if target is not None and 0<target<image.image_size else None
   call['direct_target_runtime']=list(entry) if entry else None
   call['direct_target_exact_entry']=bool(entry and entry[0]==target)
   # Metadata identity only; never follow or summarize another native function.
 for owner,site,target in required_calls:
  i=graphs[owner].get(site)
  if not i or i.id!=x.X86_INS_CALL or i.operands[0].type!=x.X86_OP_IMM or i.operands[0].imm!=roles[target][0]:raise pe.Refusal('accepted_reachable_CALL_changed_'+owner)
 result=dict(schema=1,result='CURRENT_STATIC_CONTRACT_CANDIDATES',exe_sha256=hashlib.sha256(data).hexdigest(),
             roles=out,selection='accepted_exact_roles_hashes_no_discovery',
             assumptions='Windows_x64_register_clobbers_only_not_game_signature; every branch possible; unknown callees not traversed',
             motion_vs_facing='inspect_argument_provenance_not_reference_label_only',frame='UNKNOWN',units='UNKNOWN',
             object_owner_thread_lifetime='UNKNOWN',callable_API_proven=False,production_permission=False,live_reads=0,raw_code_exported=False)
 if len((json.dumps(result,indent=2)+'\n').encode())>MAX_OUTPUT:raise pe.Refusal('contract_output_cap')
 return result


def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('exe',type=Path);p.add_argument('--output',required=True,type=Path);a=p.parse_args()
 try:
  with a.exe.open('rb') as f:r=report(f.read(pe.MAX_FILE+1))
 except (OSError,ValueError,struct.error) as e:r=dict(schema=1,result='REFUSED',reason=str(e) if isinstance(e,pe.Refusal) else 'malformed_or_unavailable',production_permission=False)
 with a.output.open('x',encoding='utf-8') as f:json.dump(r,f,indent=2);f.write('\n')
 print(r['result'],a.output)
 return 0 if r['result']=='CURRENT_STATIC_CONTRACT_CANDIDATES' else 2
if __name__=='__main__':raise SystemExit(main())
