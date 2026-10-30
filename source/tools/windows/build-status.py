"""Atomic operation-bound progress; diagnostic receipt, never deployment authority."""
from pathlib import Path
import json,os,re,sys,uuid

def record(folder,stage,state,exit_status=None):
 token=os.environ.get('MSM2_WS_OPERATION_TOKEN','')
 if not re.fullmatch(r'[a-f0-9]{32}',token):raise RuntimeError('Build status requires bound operation token')
 if not re.fullmatch(r'[A-Za-z0-9_:./-]{1,160}',stage):raise RuntimeError('Invalid authored build stage')
 if state not in ('running','failed','finished'):raise RuntimeError('Invalid build state')
 root=Path(folder);path=root/'Build-WS-status.json'
 for p in [path,*path.parents]:
  if p.is_symlink():raise RuntimeError('Status symlink refused')
 root.mkdir(parents=True,exist_ok=True)
 old={}
 if path.is_file():
  try:
   if path.stat().st_size<=32768:old=json.loads(path.read_text())
  except ValueError:pass
 if not isinstance(old,dict) or old.get('token')!=token:old={}
 detail=stage if stage.startswith('fixture_c48:') else old.get('last_detail')
 if state=='finished' and old.get('state')=='failed':stage=old.get('stage',stage)
 data=dict(schema=1,token=token,distro=os.environ.get('WSL_DISTRO_NAME'),uid=os.getuid(),stage=stage,last_detail=detail,state=('completed' if exit_status==0 else 'failed') if state=='finished' else state,exit_status=exit_status)
 pending=root/('.Build-WS-status-'+uuid.uuid4().hex+'.pending')
 try:
  with pending.open('x') as f:json.dump(data,f,sort_keys=True);f.write('\n');f.flush();os.fsync(f.fileno())
  pending.replace(path)
 finally:
  if pending.exists():pending.unlink()
if __name__=='__main__':record(sys.argv[1],sys.argv[2],sys.argv[3],int(sys.argv[4]) if len(sys.argv)>4 and sys.argv[4] else None)
