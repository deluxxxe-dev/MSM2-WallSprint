"""Reference DLL evidence only; none of these RVAs are game addresses."""
from pathlib import Path
import re,struct,hashlib,json,argparse
p=argparse.ArgumentParser();p.add_argument('dll',type=Path);p.add_argument('--disasm',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();data=a.dll.read_bytes();pe=struct.unpack_from('<I',data,0x3c)[0];opt=pe+24;size=struct.unpack_from('<H',data,pe+20)[0];base=struct.unpack_from('<Q',data,opt+24)[0];sections=[]
for i in range(struct.unpack_from('<H',data,pe+6)[0]):
 o=opt+size+40*i;vsize,rva,n,raw=struct.unpack_from('<IIII',data,o+8);sections.append((rva,vsize,raw,n))
def rva(offset):
 for v,_,raw,n in sections:
  if raw<=offset<raw+n:return v+offset-raw
 return None
labels=['PoseInit','PoseParentWitness','native:PoseInit','native:PoseParentWitness','completed pose unavailable','resolve-native-wall-orientation','synchronize-pose-and-native-rope']
result=[]
for label in labels:
 offset=data.find(label.encode()+b'\0')
 if offset<0:continue
 va=base+rva(offset);entry={'label':label,'file_offset':hex(offset),'DLL_RVA':hex(va-base),'descriptors':[],'static_xrefs':[]};needle=struct.pack('<Q',va);cursor=0
 while (at:=data.find(needle,cursor))>=0:
  if at+40<=len(data):entry['descriptors'].append({'DLL_RVA':hex(rva(at)) if rva(at) is not None else None,'raw5Q':[hex(x) for x in struct.unpack_from('<5Q',data,at)]})
  cursor=at+1
 result.append(entry)
if a.disasm:
 selected={base+int(x['DLL_RVA'],16):x for x in result}
 for line in a.disasm.read_text().splitlines():
  m=re.search(r'^\s*([0-9a-f]+):.*#\s*(?:0x)?([0-9a-f]+)',line)
  if m and int(m[2],16) in selected:selected[int(m[2],16)]['static_xrefs'].append(hex(int(m[1],16)-base))
report={'reference_sha256':hashlib.sha256(data).hexdigest(),'scope':'ALL addresses are reference DLL only, NOT Spider-Man2.exe','evidence':result,'engine_orientation_setter_identified':False,'target_build_ABI_verified':False,'runtime_hook_added':False,'game_call':0}
a.output.write_text(json.dumps(report,indent=2)+'\n');print('PASS: reference-only Pose/late-pipeline evidence,',len(result),'labels; no game setter/ABI asserted')
