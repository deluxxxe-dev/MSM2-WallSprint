"""v0.1a7 source provenance/code capture/four-face review. Never sets Verified."""
from pathlib import Path
import argparse,re,json,hashlib,subprocess,shutil
from four_face_score import score,AXES,dot
p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('--face-map',type=Path);p.add_argument('--output-dir',type=Path,default=Path('surface_source_review'));a=p.parse_args();a.output_dir.mkdir(parents=True,exist_ok=True)
face_map={}
if a.face_map:face_map={int(k):v for k,v in json.loads(a.face_map.read_text())['sessions'].items()}
code={};access=[];plans=[];children=[];records={};contacts={};pairs={};gates=[];basis_groups=[]
for line in a.log.read_text(errors='replace').splitlines():
 d=dict(re.findall(r'(\w+)=([^ ]+)',line))
 if 'native_function_capture ' in line:code[d['function']]={'metadata':d,'chunks':{}}
 if 'native_function_code ' in line and d['function'] in code:code[d['function']]['chunks'][int(d['offset'],16)]=bytes.fromhex(d['hex'])
 if 'wallrun_this_access ' in line:access.append(d)
 if 'surface_pointer_plan ' in line:plans.append(d)
 if 'child_object_observation ' in line:children.append(d)
 if 'object_repeated_basis_group ' in line:basis_groups.append(d)
 if 'surface_calibration_pair ' in line:pairs[int(d['wallrun_session'])]=d
 if 'surface_candidate_score ' in line:
  session=int(d['session']);axis=tuple(map(float,d['crawl_axis'].strip('()').split(',')))
  if face_map:face=face_map.get(session)
  else:
   face=max(AXES,key=lambda f:dot(axis,AXES[f]));face=face if dot(axis,AXES[face])>.9 else None
  if not face:continue
  path=d.get('source_path','WallRun.this_direct')
  if path=='WallRun.this_direct':path+=f"+{d['offset']}"
  records.setdefault(path,[]).append({'session':session,'face':face,'vector':tuple(map(float,d['candidate'].strip('()').split(','))),'stable':d['within_session_stable']=='1','child_object_id':d.get('child_object_id'),'within_session_basis_suspect':d['identity_or_basis_suspect'],'overlapping':d['overlapping']})
 if 'surface_contact_candidate_score ' in line:contacts.setdefault(d.get('source_path','WallRun.this_direct')+('' if 'source_path' in d else '+'+d['offset']),[]).append(d)
 if 'v01b_gate=' in line:gates.append(d['v01b_gate'])
# Expand only recorded equality groups, never invent missing fields. This preserves
# source offset identity while avoiding treating duplicates as independent evidence.
for d in basis_groups:
 session=int(d['session']);pair=pairs.get(session)
 if not pair:continue
 if face_map:face=face_map.get(session)
 else:
  axis=tuple(map(float,pair['crawl_axis'].strip('()').split(',')));face=max(AXES,key=lambda f:dot(axis,AXES[f]));face=face if dot(axis,AXES[face])>.9 else None
 if not face:continue
 parent=d.get('source_path','WallRun.this_direct')
 if '->child+' in parent:parent=parent.split('->child+',1)[0]+'->child'
 for offset in d['members'].split(','):
  path=parent+'+'+offset
  if any(r['session']==session for r in records.get(path,[])):continue
  records.setdefault(path,[]).append({'session':session,'face':face,'vector':tuple(map(float,d['candidate'].strip('()').split(','))),'stable':True,'child_object_id':d.get('child_object_id'),'overlapping':'basis_group_member','record_source':'recorded_numeric_equality_group_NOT_independent_evidence'})
functions=[];instructions={}
for name,item in code.items():
 meta=item['metadata'];size=int(meta['size']);assert size<=0x4000 and re.fullmatch(r'[A-Za-z0-9_]{1,40}',name)
 raw=bytearray(size);covered=bytearray(size)
 for off,chunk in item['chunks'].items():
  assert 0<=off<=size and off+len(chunk)<=size and not any(covered[off:off+len(chunk)]),'invalid/overlapping code capture'
  raw[off:off+len(chunk)]=chunk;covered[off:off+len(chunk)]=b'\1'*len(chunk)
 assert all(covered),'incomplete code capture'
 digest=hashlib.sha256(raw).hexdigest();assert meta['sha256']==digest,'code SHA256 mismatch'
 file=a.output_dir/(name+'.bin');file.write_bytes(raw)
 if shutil.which('objdump'):
  asm=subprocess.check_output(['objdump','-D','-b','binary','-m','i386:x86-64',f"--adjust-vma={int(meta['rva'],16)}",str(file)],text=True)
  (a.output_dir/(name+'.asm.txt')).write_text(asm)
  for line in asm.splitlines():
   columns=line.split('\t');m=re.match(r'\s*([0-9a-f]+):',columns[0])
   if m and len(columns)>=3:instructions[(name,int(m[1],16))]=' '.join(columns[2:]).strip()
 functions.append({'name':name,'rva':meta['rva'],'size':size,'sha256':digest,'scope':'bounded captured function only; native entry patched afterward','synthetic':name.startswith('Synthetic')})
scores={path:score(rs) for path,rs in records.items()}
report={'function_captures':functions,'this_accesses':access,'pointer_plans':plans,'child_observations':children,'four_face_label_source':'user-provided confirmed session mapping' if face_map else 'R1 orientation heuristic, NOT video-confirmed','four_face_records':records,'four_face_scores':scores,'contact_candidates':contacts,'surface_axis_verified':False,'surface_sign_verified':False,'contact_verified':False,'source_path_verified':False,'v01b_gate':'BLOCKED','notes':['Strong numeric support is not a verified native layout/getter or contact.','Primary map excludes unassigned transitions; no session-order auto assignment.','A/C and B/D opposite signs and same path are required; no actor writes are enabled.']}
(a.output_dir/'SURFACE_SOURCE_REVIEW.json').write_text(json.dumps(report,indent=2)+'\n')
text='# WallRun native this-access map\n\nNative function bytes and SHA256 checked from bounded runtime capture. HDE uses RCX-derived aliases, conservative CFG intersection and unknown-write invalidation. Indexed/unsupported instructions are incomplete; no complete C++ type recovery is claimed.\n\n'
if not functions:text+='No native function bodies captured. No native this-offset map can be fabricated from object floats or 16-byte prefixes.\n'
text+='|Function|Instruction RVA|Base|Parent field|Offset|Operation|Width|Possible type|Instruction|\n|---|---|---|---|---|---|---:|---|---|\n'
for d in access:text+='|'+'|'.join(d[k] for k in ['function','instruction_rva','scope','parent_field_signed','offset_signed','operation','width','possible_type'])+'|'+instructions.get((d['function'],int(d['instruction_rva'],16)),'see captured bytes/disassembly')+'|\n'
text+='\n## Source paths\n\n'
for d in plans:text+=f"- {d['source_path']}: native dereference evidence, child bound {d['observation_bytes']}, max referenced end {d['max_native_child_access']}.\n"
text+='\nNo extra Surface getter hook is installed without authenticated identity/AOB/prefix/boundary evidence.\n'
(a.output_dir/'WALLRUN_THIS_ACCESS_MAP.md').write_text(text)
print(f'PASS: {len(functions)} checksum-verified function captures, {len(access)} accesses, {len(plans)} one-level source paths, {len(scores)} four-face candidates; gate BLOCKED')
