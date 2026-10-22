"""Offline pinned Capstone. Entry snapshots are read once, never dereference pointers.
Caller slices are conservative local dependency traces, not complete type recovery.
"""
from pathlib import Path
import argparse,hashlib,json,math,re,struct
from collections import defaultdict
from capstone import Cs,CS_ARCH_X86,CS_MODE_64,CS_GRP_JUMP,CS_GRP_RET
from capstone.x86 import X86_OP_REG,X86_OP_MEM,X86_OP_IMM
from review_orientation_writers import fields,canonical
EXE='c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9'
WRITER=0x2cfc580
WHASH='3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6'
def unit(v):
 if not all(math.isfinite(x) and abs(x)<=1.25 for x in v):return None
 n=math.sqrt(sum(x*x for x in v));return tuple(x/n for x in v) if .85<=n<=1.15 else None
def dot(a,b):return sum(x*y for x,y in zip(a,b))
def vector(text):return tuple(map(float,text.strip('()').split(',')))
def inspect(raw):
 floats=struct.unpack('<32f',raw)
 vectors={off:unit(floats[off//4:off//4+3]) for off in range(0,117,4)}
 vectors={k:v for k,v in vectors.items() if v is not None}
 matrices=[];quats=[]
 for stride in (12,16):
  for start in range(0,128-2*stride-11,4):
   if start%stride:continue
   offsets=[start+i*stride for i in range(3)]
   if all(o in vectors for o in offsets):
    rows=[vectors[o] for o in offsets]
    if max(abs(dot(rows[a],rows[b])) for a,b in [(0,1),(0,2),(1,2)])<.08:
     cross=(rows[0][1]*rows[1][2]-rows[0][2]*rows[1][1],rows[0][2]*rows[1][0]-rows[0][0]*rows[1][2],rows[0][0]*rows[1][1]-rows[0][1]*rows[1][0])
     matrices.append({'start':start,'stride':stride,'rows':offsets,'determinant':dot(cross,rows[2]),'type':'3x4_or_4x4_like' if stride==16 and start+64<=128 else '3xVec3_basis_candidate','meaning':'layout_candidate_NOT_engine_type'})
 for off in range(0,113,4):
  q=floats[off//4:off//4+4]
  if off%12 and off%16:continue
  if all(math.isfinite(x) for x in q) and abs(sum(x*x for x in q)-1)<.04:quats.append({'offset':off,'meaning':'unit_Vec4_quaternion_like_ambiguous_NOT_decoded_orientation'})
 clusters=[]
 for offset in vectors:
  if clusters and offset<clusters[-1]['end']:clusters[-1]['offsets'].append(offset);clusters[-1]['end']=offset+12
  else:clusters.append({'begin':offset,'end':offset+12,'offsets':[offset]})
 return vectors,matrices,quats,clusters

def slice_arguments(ins,call):
 index=ins.index(call);out={}
 for initial in ['rcx','rdx','r8','r9']:
  needed={initial};trace=[];reason='200byte_local_limit_or_function_entry';start=call.address-200
  for i in reversed(ins[:index]):
   if i.address<start:break
   if i.group(CS_GRP_JUMP) or i.group(CS_GRP_RET):reason='control_flow_barrier';break
   if i.mnemonic=='call':
    if needed & {'rax','rcx','rdx','r8','r9','r10','r11'}:reason='unknown_call_clobbers_volatile_input';break
    continue
   _,writes=i.regs_access();defined={canonical(i.reg_name(r)) for r in writes}
   hit=needed&defined
   if not hit:continue
   trace.append({'rva':hex(i.address),'instruction':i.mnemonic+' '+i.op_str,'defines':sorted(hit)})
   # Only full-width MOV/LEA are propagated; partial writes and arithmetic stop.
   if i.mnemonic not in ['mov','lea'] or not i.operands or i.operands[0].type!=X86_OP_REG or i.operands[0].size!=8:
    reason='nontrivial_or_partial_definition';break
   needed-=hit
   for op in i.operands[1:]:
    if op.type==X86_OP_REG:needed.add(canonical(i.reg_name(op.reg)))
    elif op.type==X86_OP_MEM:
     for reg in [op.mem.base,op.mem.index]:
      name=canonical(i.reg_name(reg)) if reg else ''
      if name and name!='rip':needed.add(name)
   if not needed:reason='immediate_or_RIP_relative_origin';break
  out[initial]={'backward_local_trace':trace,'unresolved_dependencies':sorted(needed),'stop':reason,'complete_CFG_proof':False}
 return out

def review(log,faces=None):
 faces=faces or {'1':'A','2':'E'};lines=log.read_text(errors='replace').splitlines();functions={};entries={};snapshots=[];md=Cs(CS_ARCH_X86,CS_MODE_64);md.detail=True
 for l in lines:
  d=fields(l)
  if 'orientation_source_function ' in l and d.get('available')=='1':
   size=int(d['size']);assert 0<size<=65536
   if d['label'] in functions:raise ValueError('duplicate function capture')
   functions[d['label']]={'begin':int(d['begin_rva'],16),'size':size,'hash':d['sha256'],'chunks':{}}
  if 'orientation_source_code ' in l:
   f=functions[d['label']];o=int(d['offset'],16);b=bytes.fromhex(d['bytes']);assert 0<=o and o+len(b)<=f['size']
   if o in f['chunks']:raise ValueError('duplicate chunk')
   f['chunks'][o]=b
  if 'ORIENTATION_WRITER_ENTRY ' in l:
   if d['event'] in entries:raise ValueError('duplicate entry')
   entries[d['event']]=d
  if 'ORIENTATION_SOURCE_SNAPSHOT ' in l:snapshots.append(d)
 decoded={};caller_slices=[];function_result={}
 for label,f in functions.items():
  b=bytearray(f['size']);cover=bytearray(f['size'])
  for o,c in f['chunks'].items():
   if any(cover[o:o+len(c)]):raise ValueError('overlap code')
   b[o:o+len(c)]=c;cover[o:o+len(c)]=b'\1'*len(c)
  complete=all(cover);hash_ok=complete and hashlib.sha256(b).hexdigest()==f['hash'];ins=list(md.disasm(bytes(b),f['begin'])) if hash_ok else [];count=sum(i.size for i in ins)
  function_result[label]={'begin_rva':hex(f['begin']),'end_rva':hex(f['begin']+f['size']),'size':f['size'],'sha256':f['hash'],'complete':complete,'hash_verified':hash_ok,'decoded_bytes':count,'decoded_coverage':count/f['size'],'full_decode':count==f['size'],'direct_writer_calls':[]}
  if count==f['size']:
   decoded[label]=ins
   for i in ins:
    if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==WRITER:
     function_result[label]['direct_writer_calls'].append(hex(i.address))
     caller_slices.append({'function':label,'call_rva':hex(i.address),'return_rva':hex(i.address+i.size),'arguments':slice_arguments(ins,i),'preceding_200_bytes':[hex(x.address)+': '+x.mnemonic+' '+x.op_str for x in ins if i.address-200<=x.address<i.address]})
 counters=defaultdict(int);groups=defaultdict(list);layouts=[];bad=0;seen=set();caller_fields=defaultdict(list);source_events=set();event_matrix={}
 for s in snapshots:
  e=entries.get(s.get('event'));key=(s.get('event'),s.get('argument'))
  if key in seen:raise ValueError('duplicate source snapshot')
  seen.add(key)
  if not e or s.get('readable')!='1':continue
  if int(s.get('size','0'))!=128:bad+=1;continue
  raw=bytes.fromhex(s['bytes'])
  if len(raw)!=128 or s.get('pointer')!=e.get(s['argument']):bad+=1;continue
  if e.get('RCX_matches_target')!='1' or e.get('RCX')!=e.get('target_transform'):continue
  # Retain caller/state distinctions, never equate a proxy window with native state.
  state=e['state'];caller=e['caller_class'];face=faces.get(e['session'],'Unassigned')
  rva=int(e['caller_rva'],16);derived='WALLRUN_CALLER' if rva==0x2cfc8f3 else 'CRAWL_SURFACE_CALLER' if rva in [0x1021c9e,0x1022c15] else 'OTHER_CALLER'
  if caller!=derived:continue
  expected_caller='WALLRUN_CALLER' if state=='WallRun' else 'CRAWL_SURFACE_CALLER'
  all_vecs,_,_,_=inspect(raw)
  for off,v in all_vecs.items():caller_fields[(s['argument'],off,state,e['caller_rva'])].append(v)
  n=unit(vector(e['N398']))
  if not n or caller!=expected_caller or state not in ['WallRun','CrawlCandidate'] or face not in ['A','E']:continue
  expected=(0.,1.,0.) if state=='WallRun' else n
  # User-confirmed session mapping must agree with captured A/E field axes.
  ground=(0.,0.,1.) if face=='A' else (-math.sqrt(.5),0.,math.sqrt(.5))
  if dot(n,ground)<.98:continue
  vecs,mats,quats,clusters=inspect(raw);source_events.add(e['event'])
  event_matrix[key]={o for m in mats for o in m['rows']}
  layouts.append({'event':e['event'],'argument':s['argument'],'matrices':mats,'quaternions':quats,'overlap_clusters':clusters})
  for o,v in vecs.items():groups[(s['argument'],o)].append({'event':e['event'],'group':face+'_'+state,'signed_dot':dot(v,expected),'abs_dot':abs(dot(v,expected)),'unit':v,'matrix_row':o in event_matrix[key]})
 scores=[]
 required=['A_WallRun','E_WallRun','A_CrawlCandidate','E_CrawlCandidate']
 for (arg,o),rows in groups.items():
  by={g:[x for x in rows if x['group']==g] for g in required};metrics={}
  for g,v in by.items():
   if not v:continue
   mean=tuple(sum(x['unit'][a] for x in v)/len(v) for a in range(3));ml=math.sqrt(dot(mean,mean))
   metrics[g]={'samples':len(v),'mean_abs_dot':sum(x['abs_dot'] for x in v)/len(v),'min_abs_dot':min(x['abs_dot'] for x in v),'mean_signed_dot':sum(x['signed_dot'] for x in v)/len(v),'within_group_resultant':ml,'mean_unit':mean,'matrix_row_fraction':sum(x['matrix_row'] for x in v)/len(v)}
  qualified=all(g in metrics and metrics[g]['samples']>=3 and metrics[g]['min_abs_dot']>=.98 and metrics[g]['within_group_resultant']>=.98 and metrics[g]['matrix_row_fraction']>=.9 for g in required)
  world=[x['abs_dot'] for x in rows if x['group'].endswith('_WallRun')]
  scores.append({'argument':arg,'offset':hex(o),'wallrun_worldup_dot':sum(world)/len(world) if world else None,'A_crawl_surface_dot':metrics.get('A_CrawlCandidate',{}).get('mean_abs_dot'),'E_crawl_surface_dot':metrics.get('E_CrawlCandidate',{}).get('mean_abs_dot'),'metrics':metrics,'source_candidate_identified':qualified,'status':'VERIFIED_DESIRED_UP_SOURCE_CANDIDATE' if qualified else 'INSUFFICIENT_OR_FAILED_CROSS_STATE_EVIDENCE','overlap_handling':'matrix_row_required; overlapping_triplets_NOT_independent_field_proof'})
 scores.sort(key=lambda x:(not x['source_candidate_identified'],-len(x['metrics']),int(x['offset'],16)))
 auth=any('WallSprint v0.1b4 ' in l for l in lines) and any('target_build_gate=ACCEPTED' in l for l in lines) and any('exe_file_sha256='+EXE in l for l in lines) and any('orientation_source_hook installed=1' in l for l in lines)
 fw=function_result.get('writer',{});auth=bool(auth and fw.get('sha256')==WHASH and fw.get('full_decode') and fw.get('hash_verified') and fw.get('begin_rva')==hex(WRITER))
 classified=defaultdict(int)
 for e in entries.values():classified[e['state']+'/'+e['caller_class']+'/'+e['RCX_matches_target']]+=1
 identified=any(x['source_candidate_identified'] for x in scores)
 verified_returns={x['return_rva'] for x in caller_slices}
 caller_coverage=all(hex(r) in verified_returns for r in [0x2cfc8f3,0x1021c9e,0x1022c15])
 return {'log_sha256':hashlib.sha256(log.read_bytes()).hexdigest(),'actual_game_capture':auth,'entries':len(entries),'classified_entries':dict(classified),'valid_target_source_events':len(source_events),'malformed_snapshots':bad,'functions':function_result,'caller_slices':caller_slices,'source_candidates':scores,'layout_observations':layouts,'caller_field_groups':[{'argument':key[0],'offset':hex(key[1]),'state':key[2],'caller_rva':key[3],'samples':len(vs),'mean_unit':[sum(v[a] for v in vs)/len(vs) for a in range(3)]} for key,vs in caller_fields.items()],'test_mapping':faces,'source_candidate_identified':identified,'caller_three_sites_verified':caller_coverage,'v01b4_pass':auth and identified and caller_coverage,'actor_write_permission':False,'typed_ABI_verified':False,'engine_WallCrawl_accessor_verified':False,'limitations':['Entry registers/snapshots only, no return-state intervention.','State is a worker-window proxy; caller/path and user ground truth must corroborate.','Upstream slice is local and stops at branches/call clobbers; no complete CFG/type recovery.','Quaternion-like blocks are ambiguous, never promoted as Up fields.','Source candidate validation grants no Actor write permission.']}
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--faces',type=Path);a=p.parse_args();r=review(a.log,json.loads(a.faces.read_text()) if a.faces else None);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(r,indent=2,allow_nan=False)+'\n');print('Entry events',r['entries'],'candidate',r['source_candidate_identified'],'actual_game',r['actual_game_capture'],'PASS gate',r['v01b4_pass'])
