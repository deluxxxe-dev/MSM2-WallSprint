"""Read provided reference binaries. Emits DLL-relative evidence, never game addresses."""
from pathlib import Path
import re,struct,json,hashlib
root=Path(__file__).resolve().parents[1]
dll=root/'analysis/extracted/sc_core/SwingCore.dll'
data=dll.read_bytes();pe=struct.unpack_from('<I',data,0x3c)[0]
count=struct.unpack_from('<H',data,pe+6)[0];opt=pe+24;opt_size=struct.unpack_from('<H',data,pe+20)[0];imagebase=struct.unpack_from('<Q',data,opt+24)[0]
sections=[]
for i in range(count):
 off=opt+opt_size+i*40;name=data[off:off+8].split(b'\0')[0].decode();vsize,rva,raw_size,raw=struct.unpack_from('<IIII',data,off+8)
 sections.append((name,rva,vsize,raw,raw_size))
def va(file_offset):
 for _,rva,_,raw,size in sections:
  if raw<=file_offset<raw+size:return imagebase+rva+file_offset-raw
 raise ValueError(file_offset)
spans=[]
for name,rva,vsize,raw,size in sections:
 if name=='.pdata':
  for off in range(raw,raw+vsize-11,12):
   start,end,_=struct.unpack_from('<III',data,off)
   if start<end:spans.append((imagebase+start,imagebase+end))
strings=[(m.start(),m.group().decode('ascii')) for m in re.finditer(rb'[\x20-\x7e]{5,}',data)]
terms=('TransitionWallRun','WallRunEntry','WallRunUpdate','WallRunInit','WallRunDeinit','WallCrawlDeinit','HeroTransitionManager','HeroStateWall','HolyRoller','GetImpactNormal','surface-query','ActorComponent','ComponentRegistry','ImmediateAlloc','RequestCtor','ShapeImpl','WorldQuery','RayCast','Raycast','Sweep','wwr:ImpactNormal')
selected=[(o,t) for o,t in strings if any(term in t for term in terms)]
xrefs={va(o):[] for o,_ in selected}
with (root/'analysis/SwingCore.disasm.txt').open() as f:
 for line in f:
  match=re.search(r'^\s*([0-9a-f]+):.*# 0x([0-9a-f]+)',line)
  if not match:continue
  instruction,target=int(match[1],16),int(match[2],16)
  if target in xrefs:
   owner=next((start for start,end in spans if start<=instruction<end),None)
   xrefs[target].append({'instruction_DLL_RVA':hex(instruction-imagebase),'containing_function_DLL_RVA':hex(owner-imagebase) if owner else None})
patterns=[]
for i,(offset,text) in enumerate(strings):
 if re.fullmatch(r'(?:[0-9A-F?*]{2} )+[0-9A-F?*]{2}',text) and i:
  prev=strings[i-1][1]
  if prev in ['WallRunEntry','WallRunUpdate','WallRunInit','WallRunDeinit','ActorComponentSmall','ActorComponentLarge','WallCrawlIdleTransition','WallCrawlMoveTransition']:
   patterns.append({'label':prev,'AOB':text,'file_offset':hex(offset),'proven_on_game_2_810':False})
report={'source':'provided sc_core.script / SwingCore.dll','sha256':hashlib.sha256(data).hexdigest(),'address_scope':'ALL RVAs BELOW REFER TO REFERENCE DLL, NOT Spider-Man2.exe','evidence':[{'text':t,'file_offset':hex(o),'DLL_RVA':hex(va(o)-imagebase),'xrefs':xrefs[va(o)]} for o,t in selected],'embedded_patterns':patterns}
(root/'analysis/STATE_SURFACE_XREFS.json').write_text(json.dumps(report,indent=2)+'\n')
print('Wrote',len(selected),'string evidence entries and',len(patterns),'AOB candidates; no game functions invoked.')
