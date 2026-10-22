"""Static evidence from provided reference DLL; no game calls or inferred offsets."""
from pathlib import Path
import struct,json,hashlib,re,subprocess
root=Path(__file__).resolve().parents[1]
p=root/'analysis/extracted/sc_core/SwingCore.dll';data=p.read_bytes()
pe=struct.unpack_from('<I',data,0x3c)[0];count=struct.unpack_from('<H',data,pe+6)[0];h=pe+24+struct.unpack_from('<H',data,pe+20)[0]
sections=[]
for i in range(count):
 s=h+i*40;vs,rva,size,raw=struct.unpack_from('<IIII',data,s+8);sections.append((rva,vs,raw,size))
def rva(o):
 for start,vs,raw,size in sections:
  if raw<=o<raw+size:return start+o-raw
 raise ValueError(o)
base=0x180000000;entries=[]
for label,va in [('WallRunEntry',0x147d80),('WallRunUpdate',0x147de8),('WallRunInit',0x147e60),('WallRunDeinit',0x1481a8)]:
 needle=struct.pack('<Q',base+va);start=0
 while (offset:=data.find(needle,start))>=0:
  words=struct.unpack_from('<5Q',data,offset);entries.append({'label':label,'descriptor_DLL_RVA':hex(rva(offset)),'raw_words':[hex(v) for v in words],'interpretation':'label VA, old reference target RVA, AOB VA, sentinel, zero; NOT target-build function bytes or callable address'})
  start=offset+1
wrappers=[]
for start,end,name in [(0x37dd0,0x38000,'GetImpactNormal wrapper'),(0x3f6d0,0x3f9b0,'Hero surface-query wrapper'),(0x3e7a0,0x3e920,'component helper')]:
 text=subprocess.check_output(['objdump','-d','-M','intel',f'--start-address={hex(base+start)}',f'--stop-address={hex(base+end)}',str(p)],text=True)
 lines=[line.strip() for line in text.splitlines() if re.match(r'\s*[0-9a-f]+:',line)]
 wrappers.append({'name':name,'DLL_RVA':hex(start),'instructions':lines})
report={'reference_sha256':hashlib.sha256(data).hexdigest(),'scope':'ALL addresses are in reference SwingCore.dll; game exe not present','descriptors':entries,'wrappers':wrappers,'result':'No WallRun this->contact field or getter proved. Native function bodies absent. Broad object observation permitted only as bounded unverified candidates.'}
(root/'analysis/WALLRUN_OBJECT_STATIC_EVIDENCE.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS: reference descriptors and surface wrapper disassembly recorded; no guessed runtime getter')
