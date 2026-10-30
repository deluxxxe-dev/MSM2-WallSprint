"""Identity-checked retirement of two obsolete WS asset probes; no profile writes."""
from pathlib import Path
import hashlib,io,json,os,tempfile,zipfile
ASSET='0/configs/hero/hero_WallRunSteeringSetup_Standard.config'
VANILLA_SHA='32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b'
PROBES={'MSM2-WS-SteeringProbe-TurnSpeedFast10':('WS Steering Probe - TurnSpeedFast 10',0x218,10,100),
        'MSM2-WS-SteeringProbe-TurnSpeedSlow5':('WS Steering Probe - TurnSpeedSlow 5',0x219,5,75)}

def _zip(data):
 z=zipfile.ZipFile(io.BytesIO(data))
 if len(z.infolist())>4 or sum(i.file_size for i in z.infolist())>16384:raise ValueError('Probe size limit')
 if z.testzip() is not None:raise ValueError('Probe CRC')
 return z

def recognized(path):
 """Exact basename, metadata, member set and reversible original-asset SHA."""
 stem=path.stem
 if stem not in PROBES or path.suffix.lower() not in {'.stage','.zip'}:return False
 try:
  if path.stat().st_size>32768:return False
  data=path.read_bytes()
  if path.suffix.lower()=='.zip':
   with _zip(data) as z:
    name=stem+'.stage'
    if z.namelist()!=[name]:return False
    data=z.read(name)
  with _zip(data) as z:
   if len(z.namelist())!=2 or set(z.namelist())!={ASSET,'info.json'}:return False
   info=json.loads(z.read('info.json'));name,offset,value,original=PROBES[stem]
   if info!={'game':'MSM2','name':name,'author':'deluxxxe-dev / WallSprint','format_version':2}:return False
   asset=bytearray(z.read(ASSET))
   if len(asset)!=544 or asset[offset]!=value:return False
   asset[offset]=original
   return hashlib.sha256(asset).hexdigest()==VANILLA_SHA
 except (OSError,ValueError,KeyError,RuntimeError,zipfile.BadZipFile):return False

def candidates(workspace):
 # Caller must validate no reparse points before this traversal.
 return sorted(p for p in workspace.rglob('*') if p.is_file() and p.stem in PROBES and p.suffix.lower() in {'.stage','.zip'})

def retire(workspace,game,tracked=()):
 """Only WS-owned scope; new backup directory outside Mods Library, never overwrite."""
 paths=[p for p in candidates(workspace) if not (p.is_relative_to(workspace/'_source') and p.relative_to(workspace/'_source').as_posix() in tracked)]
 if not paths:return None
 if not all(recognized(p) for p in paths):raise ValueError('Unverified retired probe refused; preserve it manually outside Mods Library')
 parent=game/'MSM2-WS-Retired-Probes'
 if os.path.lexists(parent) and (not parent.is_dir() or parent.is_symlink() or getattr(parent.lstat(),'st_file_attributes',0)&0x400):raise ValueError('Unsafe probe retirement directory')
 parent.mkdir(exist_ok=True)
 backup=Path(tempfile.mkdtemp(prefix='retired-',dir=parent));saved=[]
 records=[{'source':p.relative_to(workspace).as_posix(),'backup':str(i)+'-'+p.name,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} for i,p in enumerate(paths)]
 (backup/'manifest.json').write_text(json.dumps(records,indent=2)+'\n')
 try:
  for p,rec in zip(paths,records):
   if hashlib.sha256(p.read_bytes()).hexdigest()!=rec['sha256']:raise ValueError('Probe changed during retirement')
   target=backup/rec['backup'];p.rename(target);saved.append((p,target))
 except BaseException:
  for old,new in reversed(saved):
   if not os.path.lexists(old):new.rename(old)
  raise
 print('Retired verified WS probes outside Mods Library: '+str(backup))
 return backup
