"""Offline Capstone only. Data-breakpoint RIP is AFTER store; resolve, don't guess.
No native calls, hooks, memory changes or automatic upstream/write permission.
"""
from pathlib import Path
import argparse,re,json,hashlib,struct,math
from capstone import Cs,CS_ARCH_X86,CS_MODE_64,CS_AC_WRITE
from capstone.x86 import X86_OP_MEM,X86_OP_REG,X86_OP_IMM
REGS='rax rcx rdx rbx rsp rbp rsi rdi r8 r9 r10 r11 r12 r13 r14 r15'.split()
def fields(line):return dict(re.findall(r'(\w+)=([^ ]+)',line))
def canonical(name):
 for root,aliases in {'rax':['rax','eax','ax','al','ah'],'rbx':['rbx','ebx','bx','bl','bh'],'rcx':['rcx','ecx','cx','cl','ch'],'rdx':['rdx','edx','dx','dl','dh'],'rsp':['rsp','esp','sp','spl'],'rbp':['rbp','ebp','bp','bpl'],'rsi':['rsi','esi','si','sil'],'rdi':['rdi','edi','di','dil']}.items():
  if name in aliases:return root
 m=re.fullmatch(r'(r(?:8|9|1[0-5]))[dwb]?',name);return m[1] if m else name

def review(log):
 lines=log.read_text(errors='replace').splitlines();functions={};events=[];registers={};xmms={};arms=[]
 for line in lines:
  d=fields(line)
  if 'orientation_trace_arm ' in line:arms.append(d)
  if 'ORIENTATION_WRITER_EVENT ' in line:events.append(d)
  if 'ORIENTATION_WRITER_REGS ' in line:
   values=[int(x,16) for x in d['post_gpr_RAX_RCX_RDX_RBX_RSP_RBP_RSI_RDI_R8_R9_R10_R11_R12_R13_R14_R15'].split(',')]
   if len(values)!=16:raise ValueError('invalid GPR count')
   registers[(d['writer_id'],d['qpc_ms'])]=dict(zip(REGS,values))
  if 'orientation_writer_xmm ' in line:
   raw=bytes.fromhex(d['XMM0_15_hex']);assert len(raw)==256
   if d.get('XMM_valid')=='1':xmms[(d['writer_id'],d['qpc_ms'])]=raw
  if 'orientation_writer_function ' in line:
   size=int(d['size']);assert 0<size<=0x4000
   functions[d['writer_id']]={'begin':int(d['begin'],16),'size':size,'chunks':{}}
  if 'orientation_writer_code ' in line:
   f=functions.get(d['writer_id'])
   if not f:raise ValueError('code without function metadata')
   offset=int(d['offset'],16);chunk=bytes.fromhex(d['bytes']);assert 0<=offset and offset+len(chunk)<=f['size']
   if offset in f['chunks']:raise ValueError('duplicate code chunk')
   f['chunks'][offset]=chunk
 md=Cs(CS_ARCH_X86,CS_MODE_64);md.detail=True;instructions={};function_info={};event_resolution=[]
 for writer,f in functions.items():
  raw=bytearray(f['size']);covered=bytearray(f['size'])
  for offset,chunk in f['chunks'].items():
   if any(covered[offset:offset+len(chunk)]):raise ValueError('overlapping code capture')
   raw[offset:offset+len(chunk)]=chunk;covered[offset:offset+len(chunk)]=b'\1'*len(chunk)
  if not all(covered):function_info[writer]={'complete':False};continue
  captured_sha=hashlib.sha256(raw).hexdigest();normalized=False
  # b6 known writer is owned-hook patched at entry. Normalize only its known
  # prefix, and accept reconstruction ONLY when the full original SHA matches.
  if any('WallSprint v0.1b6 ' in line for line in lines) and f['size']==584:
   ev=next((e for e in events if e['writer_id']==writer),None)
   if ev and f['begin']-int(ev['module_base'],16)==0x2cfc580 and any('orientation_source_hook installed=1 ' in line and 'exact_hash=3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6' in line for line in lines):
    restored=bytearray(raw);restored[:32]=bytes.fromhex('488bc448895808488970104889781841564883ec6080795f00498bf8c5f82970')
    if hashlib.sha256(restored).hexdigest()=='3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6':raw=restored;normalized=True
  ins=list(md.disasm(bytes(raw),f['begin']));instructions[writer]={i.address+i.size:i for i in ins};decoded=sum(i.size for i in ins)
  function_info[writer]={'complete':True,'sha256':hashlib.sha256(raw).hexdigest(),'captured_sha256':captured_sha,'owned_entry_prefix_normalized':normalized,'begin':hex(f['begin']),'size':f['size'],'decoded_bytes':decoded,'decoded_coverage':decoded/f['size'],'direct_call_targets':[hex(i.operands[0].imm) for i in ins if i.mnemonic=='call' and i.operands and i.operands[0].type==X86_OP_IMM][:32]}
 writers={}
 for e in events:
  wid=e['writer_id'];w=writers.setdefault(wid,{'writer_id':int(wid),'module':e['module'],'module_base':e['module_base'],'rip_after':e['rip'],'events':0,'WallRun_hits':0,'CrawlCandidate_hits':0,'resolved_hits':0,'caller_addresses':set(),'threads':set(),'writes':[],'WallRun_up_world_count':0,'Crawl_axis_aligned_count':0})
  w['events']+=1;state=e['state'];w[state+'_hits']+=1;w['threads'].add(int(e['thread_id']));
  if e['caller_valid']=='1':w['caller_addresses'].add(e['caller'])
  vector=lambda key:tuple(map(float,e[key].strip('()').split(',')))
  up=vector('new_observed_up');normal=vector('N398');length=math.sqrt(sum(x*x for x in up));nl=math.sqrt(sum(x*x for x in normal))
  if e['read_valid']=='1' and length>.9 and length<1.1:
   if state=='WallRun' and up[1]/length>=.98:w['WallRun_up_world_count']+=1
   if state=='CrawlCandidate' and nl>.9 and abs(sum(a*b for a,b in zip(up,normal))/(length*nl))>=.98:w['Crawl_axis_aligned_count']+=1
  ins=instructions.get(wid,{}).get(int(e['rip'],16));regs=registers.get((wid,e['qpc_ms']));reason='missing_or_unanchored_instruction';resolved=None
  if ins and regs:
   _,written=ins.regs_access();written={canonical(ins.reg_name(r)) for r in written}
   for opindex,op in enumerate(ins.operands):
    if op.type!=X86_OP_MEM:continue
    is_store=bool(op.access&CS_AC_WRITE) or (opindex==0 and ins.mnemonic.startswith(('mov','vmov')))
    if not is_store:continue
    base=ins.reg_name(op.mem.base);index=ins.reg_name(op.mem.index)
    if op.mem.segment or base in written or index in written:reason='post_register_side_effect_or_segment';continue
    if base and base!='rip' and base not in regs or index and index not in regs:reason='unavailable_address_register';continue
    address=(int(e['rip'],16) if base=='rip' else regs.get(base,0))+regs.get(index,0)*op.mem.scale+op.mem.disp
    target=int(e['transform'],16)+0x10
    if op.size<=0 or not address<target+12 or not target<address+op.size:reason='decoded_store_does_not_overlap_watched_Up';continue
    resolved={'rip':hex(ins.address),'module_offset':hex(ins.address-int(e['module_base'],16)),'instruction':ins.mnemonic+' '+ins.op_str,'write_address':hex(address),'transform_offset':address-int(e['transform'],16),'width':op.size,'linear_context_NOT_CFG_proven':[hex(i.address)+': '+i.mnemonic+' '+i.op_str for i in instructions.get(wid,{}).values() if ins.address-48<=i.address<ins.address][-8:]}
    raw=xmms.get((wid,e['qpc_ms']))
    if raw and opindex==0 and len(ins.operands)>1 and ins.operands[1].type==X86_OP_REG and ins.mnemonic.startswith(('mov','vmov')):
     src=ins.reg_name(ins.operands[1].reg);m=re.fullmatch(r'[xy]mm(\d+)',src)
     if m and op.size<=16:
      data=raw[16*int(m[1]):16*int(m[1])+op.size];resolved['source_register']=src;resolved['source_bytes']=data.hex()
      if len(data)%4==0:resolved['source_float_values']=[v if math.isfinite(v) else str(v) for v in struct.unpack('<'+'f'*(len(data)//4),data)]
    reason='resolved_from_runtime_function_boundary_and_post_registers';break
  event_resolution.append({'session':e['session'],'thread_id':e['thread_id'],'qpc_ms':e['qpc_ms'],'rip_after':e['rip'],'resolved':resolved,'function_capture':function_info.get(wid,{'complete':False})})
  if resolved:
   w['resolved_hits']+=1
   if resolved not in w['writes']:w['writes'].append(resolved)
  w.setdefault('resolution_reasons',{}).setdefault(reason,0);w['resolution_reasons'][reason]+=1
 game=any('WallSprint v0.1b3 ' in x for x in lines) and any('target_build_gate=ACCEPTED' in x for x in lines) and any('exe_file_sha256=c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9' in x for x in lines)
 for wid,w in writers.items():
  w['threads']=sorted(w['threads']);w['caller_addresses']=sorted(w['caller_addresses']);w['function_capture']=function_info.get(wid,{'complete':False});w['same_store_observed_in_both_states']=bool(w['WallRun_hits'] and w['CrawlCandidate_hits']);w['rank']=w['resolved_hits']+100*w['same_store_observed_in_both_states']+20*bool(w['caller_addresses'])
 groups={}
 for w in writers.values():
  capture=w['function_capture']
  if not capture.get('complete'):continue
  key=(w['module'],capture['begin']);g=groups.setdefault(key,{'module':w['module'],'begin':capture['begin'],'module_offset':hex(int(capture['begin'],16)-int(w['module_base'],16)),'writer_ids':[],'WallRun_hits':0,'CrawlCandidate_hits':0,'resolved_hits':0,'direct_call_targets':capture.get('direct_call_targets',[])})
  g['writer_ids'].append(w['writer_id'])
  for field in ['WallRun_hits','CrawlCandidate_hits','resolved_hits']:g[field]+=w[field]
 for g in groups.values():g['same_function_in_both_states']=bool(g['WallRun_hits'] and g['CrawlCandidate_hits'])
 result={'event_resolution':event_resolution,'function_groups':list(groups.values()),'log_sha256':hashlib.sha256(log.read_bytes()).hexdigest(),'actual_game_capture':game,'windows':arms,'writers':sorted(writers.values(),key=lambda x:-x['rank']),'success_A':game and any(w['resolved_hits'] and w['WallRun_up_world_count'] for w in writers.values()),'success_B_candidate':game and any(w['resolved_hits'] and w['Crawl_axis_aligned_count'] for w in writers.values()),'success_C':False,'upstream_orientation_source_verified':False,'actor_write_permission':False,'limitations':['CrawlCandidate is an orientation proxy, not verified engine WallCrawl state.','Old/new are observed snapshots; incomplete thread coverage/concurrent stores can hide intermediate values.','Hardware RIP is after instruction; only anchored decode plus effective watched address resolves actual store.','Source XMM capture is lower128 only, no YMM upper data.','Same writer in both states does not prove its desired-up context/ABI.']}
 return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=review(a.log);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(r,indent=2,allow_nan=False)+'\n');print('PASS: writer events',sum(w['events'] for w in r['writers']),'resolved',sum(w['resolved_hits'] for w in r['writers']),'game_capture',r['actual_game_capture'],'upstream_verified',r['success_C'])
