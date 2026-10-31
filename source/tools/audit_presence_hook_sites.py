"""Read-only admission audit of TWO accepted entries, NOT a runtime diagnostic."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import capstone as cs
from capstone import x86 as x
import resolve_motion_gravity as resolution
pe=resolution.pe
SITES={
 'RequestSimpleMotion':(18488272,18488482,'2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c'),
 'GravityOrientationOwner':(20246272,20246459,'bc9e1a75a4d45f4dc1a1baf99b704778714e83353d58688dce82bab901514a53')}
# Accepted owner audit 2026-10-31-161445 and c42 actual runtime both report this
# exact RequestSimpleMotion .pdata UnwindData RVA for the pinned executable.
EXPECTED_REQUEST_UNWIND_RVA=155621344  # 0x94697E0


def unwind(image,rva):
 header=image.read(rva,4);version=header[0]&7;flags=header[0]>>3
 prolog,count,frame=header[1],header[2],header[3]
 if version!=1 or flags&~7 or flags&4 and flags&3 or count>64:raise pe.Refusal('unsupported_unwind_layout')
 block=image.read(rva,4+((count+1)//2)*4+(12 if flags&4 else 4 if flags&3 else 0))
 codes=[];slot=0;last=256
 while slot<count:
  off,tag=block[4+2*slot:6+2*slot];op=tag&15;info=tag>>4
  if off>prolog or off>last:raise pe.Refusal('invalid_unwind_code_order')
  last=off;extra=0;value=None
  if op==0:
   if info not in (3,5,6,7,12,13,14,15):raise pe.Refusal('invalid_saved_nonvolatile')
  elif op==1:
   if info not in (0,1):raise pe.Refusal('invalid_alloc_large')
   extra=1 if info==0 else 2
  elif op==2:value=info*8+8
  elif op==3:
   if info or not frame&15:raise pe.Refusal('invalid_frame_register_op')
  elif op in (4,8):extra=1
  elif op in (5,9):extra=2
  elif op==10:
   if info>1:raise pe.Refusal('invalid_machine_frame')
  else:raise pe.Refusal('unsupported_unwind_operation')
  if slot+extra>=count:raise pe.Refusal('truncated_unwind_extra_slots')
  if extra:
   value=int.from_bytes(block[6+2*slot:6+2*slot+extra*2],'little')
   if op==1 and info==0 or op==4:value*=8
   elif op==8:value*=16
  codes.append(dict(code_offset=off,operation=op,info=info,value=value))
  slot+=1+extra
 trailer=4+((count+1)//2)*4
 if flags&3 and not 0<struct.unpack_from('<I',block,trailer)[0]<image.image_size:raise pe.Refusal('unwind_handler_outside_image')
 return dict(rva=rva,version=version,flags=flags,prologue_size=prolog,
             frame_register=frame&15,frame_offset=(frame>>4)*16,codes=codes,
             metadata_sha256=hashlib.sha256(block).hexdigest(),
             handler_rva=struct.unpack_from('<I',block,trailer)[0] if flags&3 else None,
             chained_runtime=list(struct.unpack_from('<III',block,trailer)) if flags&4 else None)


def prefix_audit(image,start,end):
 code=image.read(start,min(32,end-start),True);ins=[];size=0
 aliases={x.X86_REG_RSP:0};stores=[];supported=True
 for i in image.md.disasm(code,start):
  if size>=5:break
  size+=i.size;ops=i.operands
  row=dict(offset=i.address-start,size=i.size,operation=i.mnemonic)
  if i.id==x.X86_INS_MOV and len(ops)==2 and ops[0].type==x.X86_OP_REG and ops[1].type==x.X86_OP_REG and ops[0].reg==x.X86_REG_R11 and ops[1].reg==x.X86_REG_RSP:
   aliases[x.X86_REG_R11]=0;row['effect']='volatile_R11_copies_entry_RSP'
  elif i.id==x.X86_INS_MOV and len(ops)==2 and ops[0].type==x.X86_OP_MEM and ops[1].type==x.X86_OP_REG and ops[1].reg==x.X86_REG_RBX and ops[0].size==8 and ops[0].mem.base in aliases and ops[0].mem.index==x.X86_REG_INVALID and ops[0].mem.segment==x.X86_REG_INVALID and ops[0].mem.disp==8:
   stores.append(dict(register='rbx',entry_rsp_offset=8,instruction_end=size))
   row['effect']='original_RBX_home_slot_store_no_RSP_change'
  else:supported=False;row['effect']='UNSUPPORTED_new_trampoline_effect'
  ins.append(row)
 if size<5:raise pe.Refusal('insufficient_whole_instruction_prefix')
 return dict(stolen_bytes=size,instructions=ins,original_prefix_sha256=hashlib.sha256(code[:size]).hexdigest(),
             simple_no_stack_adjust_candidate=supported,original_save_stores=stores,
             HDE64_decoder_agreement='MATCHES_VENDORED_HDE_FIXTURE' if hashlib.sha256(code[:size]).hexdigest() in ('4409ca3689d39165729542bf9fc76b56ffe59d6e9b79d2ff1082a81be2ba2c37','0666a96c5b537b54fb43e8d983f54c516a379f17f302825c7f047c7e9ff4cab0') else 'UNAVAILABLE_unmatched_fixture',actual_MinHook_trampoline='NOT_EXECUTED',
             leaf_unwind_transparency='REQUIRES_RUNTIME_UNWIND_AND_CONTINUATION_VALIDATION')


def audit(data,*,expected_sha=pe.EXPECTED_SHA,sites=SITES):
 if len(data)>pe.MAX_FILE:raise pe.Refusal('file_cap')
 if hashlib.sha256(data).hexdigest()!=expected_sha:raise pe.Refusal('unsupported_build_sha256')
 if cs.__version__!='5.0.6':raise pe.Refusal('requires_Capstone5.0.6')
 image=resolution.Image(data);rows={}
 for name,(b,e,digest) in sites.items():
  code=image.read(b,e-b,True)
  if hashlib.sha256(code).hexdigest()!=digest:raise pe.Refusal('accepted_function_hash_changed_'+name)
  runtime=image.covering(b)
  if not runtime or tuple(runtime[:2])!=(b,e):raise pe.Refusal('exact_runtime_entry_changed_'+name)
  if name=='RequestSimpleMotion' and runtime[2]!=EXPECTED_REQUEST_UNWIND_RVA:
   raise pe.Refusal('accepted_unwind_rva_changed_RequestSimpleMotion')
  u=unwind(image,runtime[2]);p=prefix_audit(image,b,e)
  if u['prologue_size']>e-b or p['stolen_bytes']>u['prologue_size']:raise pe.Refusal('prefix_outside_declared_prologue_'+name)
  rows[name]=dict(begin_rva=b,end_rva=e,sha256=digest,unwind=u,prefix=p,
                 native_early_unwind_operations=[c for c in u['codes'] if c['code_offset']<=p['stolen_bytes']],
                 hook_admitted=False)
 result=dict(schema=1,result='HOOK_ADMISSION_AUDIT_ONLY_BLOCKED',sites=rows,
             exe_sha256=hashlib.sha256(data).hexdigest(),
             blocker='Actual MinHook relocated-prefix/native unwind/continuation transparency unvalidated for new sites; HDE prefix fixtures alone insufficient',
             C52='not_a_blanket_inline_entry_ban; no VCH/DR bridge proposed; all existing gates preserved',
             observer_implemented=False,live_reads=0,raw_code_exported=False,production_permission=False)
 if len((json.dumps(result,indent=2)+'\n').encode())>32768:raise pe.Refusal('audit_output_cap')
 return result


def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('exe',type=Path);p.add_argument('--output',required=True,type=Path);a=p.parse_args()
 try:
  with a.exe.open('rb') as f:r=audit(f.read(pe.MAX_FILE+1))
 except (OSError,ValueError,struct.error) as e:r=dict(schema=1,result='REFUSED',reason=str(e) if isinstance(e,pe.Refusal) else 'malformed_or_unavailable',observer_implemented=False)
 with a.output.open('x',encoding='utf-8') as f:json.dump(r,f,indent=2);f.write('\n')
 print(r['result'],a.output)
 return 0 if r['result']=='HOOK_ADMISSION_AUDIT_ONLY_BLOCKED' else 2
if __name__=='__main__':raise SystemExit(main())
