"""One stable OS-held gate for updater and shared tools; never unlink a live lock.
Windows uses CreateFile sharing exclusion, including inherited build-child handles.
Persistent compatibility records intentionally block obsolete sentinel-only engines.
"""
from pathlib import Path
import contextlib,ctypes,json,os,stat,subprocess,sys,time,uuid
FORMAT='msm2-operation-2'
_current=None
class OperationError(RuntimeError):pass

def safe(path):
 for p in [path,*path.parents]:
  if os.path.lexists(p):
   s=p.lstat()
   if stat.S_ISLNK(s.st_mode) or getattr(s,'st_file_attributes',0)&0x400:raise OperationError('Reparse operation path refused: '+str(p))

def identity(pid):
 if os.name=='nt':
  from ctypes import wintypes as w
  k=ctypes.WinDLL('kernel32',use_last_error=True);k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE
  h=k.OpenProcess(0x1000,False,pid)
  if not h:
   if ctypes.get_last_error() in (87,1168):return None
   raise OperationError('Cannot verify process identity PID='+str(pid))
  try:
   code=w.DWORD();k.GetExitCodeProcess.argtypes=[w.HANDLE,ctypes.POINTER(w.DWORD)]
   if not k.GetExitCodeProcess(h,ctypes.byref(code)):raise OperationError('Cannot verify process exit state')
   if code.value!=259:return None
   times=[w.FILETIME() for _ in range(4)]
   k.GetProcessTimes.argtypes=[w.HANDLE,*([ctypes.POINTER(w.FILETIME)]*4)]
   if not k.GetProcessTimes(h,*[ctypes.byref(t) for t in times]):raise OperationError('GetProcessTimes failed')
   return str((times[0].dwHighDateTime<<32)|times[0].dwLowDateTime)
  finally:k.CloseHandle.argtypes=[w.HANDLE];k.CloseHandle(h)
 try:return Path('/proc',str(pid),'stat').read_text().rsplit(')',1)[1].split()[19]
 except FileNotFoundError:return None

def describe(path):
 try:
  with path.open('rb') as f:r=json.loads(f.read(65536))
  return ' owner_pid='+str(r.get('pid'))+' start='+str(r.get('start'))+' action='+str(r.get('action'))+' phase='+str(r.get('phase'))+' workspace='+str(r.get('workspace'))+' (an inherited child may retain ownership)'
 except (OSError,ValueError,AttributeError):return ' owner metadata unavailable/malformed; OS ownership still authoritative'

def open_gate(path):
 safe(path)
 if os.name=='nt':
  from ctypes import wintypes as w
  import msvcrt
  k=ctypes.WinDLL('kernel32',use_last_error=True)
  k.CreateFileW.argtypes=[w.LPCWSTR,w.DWORD,w.DWORD,w.LPVOID,w.DWORD,w.DWORD,w.HANDLE];k.CreateFileW.restype=w.HANDLE
  ctypes.set_last_error(0)
  h=k.CreateFileW(str(path),0xc0000000,1,None,4,0x00200000,None)
  new=ctypes.get_last_error()!=183
  if h==w.HANDLE(-1).value:raise OperationError('OS operation ownership busy or inaccessible: '+str(path)+' Win32='+str(ctypes.get_last_error())+describe(path))
  class Tag(ctypes.Structure):_fields_=[('attributes',w.DWORD),('tag',w.DWORD)]
  tag=Tag();k.GetFileInformationByHandleEx.argtypes=[w.HANDLE,ctypes.c_int,w.LPVOID,w.DWORD]
  if not k.GetFileInformationByHandleEx(h,9,ctypes.byref(tag),ctypes.sizeof(tag)) or tag.attributes&0x400:
   k.CloseHandle.argtypes=[w.HANDLE];k.CloseHandle(h);raise OperationError('Reparse/raced gate refused: '+str(path))
  fd=msvcrt.open_osfhandle(h,os.O_RDWR)
 else:
  import fcntl
  try:fd=os.open(path,os.O_RDWR|os.O_CREAT|os.O_EXCL|os.O_NOFOLLOW,0o600);new=True
  except FileExistsError:fd=os.open(path,os.O_RDWR|os.O_NOFOLLOW);new=False
  try:fcntl.flock(fd,fcntl.LOCK_EX|fcntl.LOCK_NB)
  except OSError:os.close(fd);raise OperationError('OS operation ownership busy: '+str(path)+describe(path))
 os.set_inheritable(fd,True)
 if not stat.S_ISREG(os.fstat(fd).st_mode):os.close(fd);raise OperationError('Operation gate is not regular')
 return fd,new

def read(fd):
 os.lseek(fd,0,0);data=os.read(fd,65537)
 if len(data)>65536:raise OperationError('Oversized operation record')
 return data

def write(fd,record):
 data=(json.dumps(record,sort_keys=True)+'\n').encode();os.lseek(fd,0,0);os.write(fd,data);os.ftruncate(fd,len(data));os.fsync(fd)

def parsed(data,game):
 try:r=json.loads(data)
 except (ValueError,UnicodeError):return None
 if not isinstance(r,dict) or r.get('format')!=FORMAT or r.get('game')!=str(game):return None
 if r.get('workspace')!=str(game/'Mods Library/Mods-Developing/MSM2-WS'):return None
 if not isinstance(r.get('pid'),int) or r['pid']<=0 or not isinstance(r.get('start'),str) or not isinstance(r.get('token'),str) or len(r['token'])!=32:return None
 if r.get('state') not in ('active','released','interrupted'):return None
 return r

WSL_PROBE = r"""import os,sys,json,errno,select

def probe(token,legacy,distro,expected_uid=None,elevated=False,attempt=0):
 uid=os.getuid();observed=os.environ.get('WSL_DISTRO_NAME','')
 out=dict(schema=1,result='UNKNOWN',distro_expected=distro,distro_observed=observed,uid_expected=expected_uid,uid_observed=uid,inspection='root_token_all_uids' if elevated else 'user',events=[],counts={})
 def event(pid,stage,category,err=None):
  out['counts'][category]=out['counts'].get(category,0)+1
  if len(out['events'])<32:out['events'].append(dict(pid=pid,stage=stage,category=category,errno=err))
 if not distro or observed!=distro:event(None,'identity','distro_mismatch');return out
 if os.geteuid()!=uid or (expected_uid is not None and uid!=expected_uid):event(None,'identity','uid_mismatch');return out
 if elevated and (uid!=0 or expected_uid!=0 or legacy or not token):event(None,'scope','invalid_root_scope');return out
 def start(pid):
  with open('/proc/'+str(pid)+'/stat') as f:s=f.read()
  tail=s.rsplit(')',1)[1].split()
  if len(tail)<20 or not tail[19].isdigit():raise ValueError('invalid stat')
  return tail[19],tail[0]
 def departed(pid,fd,before):
  # pidfd reports this exact process's exit, including zombies; never infer from
  # an unreadable environ/cwd or an old numeric PID alone.
  if fd is not None:
   poll=select.poll();poll.register(fd,select.POLLIN);exited=any(bits&(select.POLLIN|select.POLLHUP) for _,bits in poll.poll(0))
  else:
   try:os.kill(pid,0);exited=False
   except ProcessLookupError:exited=True
  try:after=start(pid)
  except FileNotFoundError:
   if exited:return 'exited_confirmed'
   # Revalidate actual PID absence when pidfd support is unavailable.
   try:os.kill(pid,0)
   except ProcessLookupError:return 'absent_confirmed'
   return None
  if before is not None and after[0]!=before[0]:return 'pid_reused'
  if exited and after[1] not in ('Z','X','x'):return 'pid_reused'
  return 'exited_confirmed' if exited and before is not None else None
 unknown=False;active=False;seen_ids={}
 try:names=os.listdir('/proc')
 except OSError as ex:event(None,'enumerate','enumeration_error',ex.errno);return out
 for n in names:
  if not n.isdigit():continue
  pid=int(n)
  if pid==os.getpid():continue # Exact self probe only, never a broad same-user exemption.
  fd=None;before=None;stage='pidfd'
  try:
   try:fd=os.pidfd_open(pid,0)
   except AttributeError:pass # signal0/identity fallback below remains fail closed.
   except OSError as ex:
    if ex.errno not in (errno.ENOSYS,errno.EINVAL):raise
   stage='uid';owner=os.stat('/proc/'+n).st_uid
   if owner!=uid and not elevated:continue
   stage='identity_before';before=start(pid)
   stage='environ'
   with open('/proc/'+n+'/environ','rb') as f:env=f.read()
   matched=bool(token and ('MSM2_WS_OPERATION_TOKEN='+token).encode() in env.split(b'\0'))
   if legacy:
    stage='cmdline'
    with open('/proc/'+n+'/cmdline','rb') as f:cmd=f.read()
    stage='cwd';cmd+=b' '+os.fsencode(os.readlink('/proc/'+n+'/cwd'))
    matched=matched or any(x in cmd for x in [b'/msm2-ws-validation.',b'workspace-build.sh',b'Mods-Developing/MSM2-WS'])
   stage='identity_after';after=start(pid)
   if after[0]!=before[0]:event(pid,stage,'pid_reused');unknown=True;continue
   seen_ids[pid]=after[0]
   # Matching live token is enough to block even if its process exits next.
   if matched:event(pid,'match','active_build');active=True
  except (OSError,ValueError,IndexError) as ex:
   try:gone=departed(pid,fd,before)
   except (OSError,ValueError,IndexError):gone=None
   if gone in ('absent_confirmed','exited_confirmed'):event(pid,stage,gone,getattr(ex,'errno',None))
   else:
    category=gone or ('access_denied' if isinstance(ex,PermissionError) else 'malformed_identity' if isinstance(ex,(ValueError,IndexError)) else 'process_read_error')
    event(pid,stage,category,getattr(ex,'errno',None));unknown=True
  finally:
   if fd is not None:os.close(fd)
 if elevated and not unknown and not active:
  final_ids={}
  try:
   for n in os.listdir('/proc'):
    if n.isdigit() and int(n)!=os.getpid():
     try:final_ids[int(n)]=start(int(n))[0]
     except FileNotFoundError:continue
   if final_ids!=seen_ids:
    if attempt<2:return probe(token,legacy,distro,expected_uid,True,attempt+1)
    event(None,'inventory','inventory_unstable');unknown=True
  except (OSError,ValueError,IndexError) as ex:event(None,'inventory','inventory_error',getattr(ex,'errno',None));unknown=True
 out['result']='ACTIVE' if active else 'UNKNOWN' if unknown else 'QUIET'
 return out

if __name__=='__main__':
 try:
  result=probe(sys.argv[1],sys.argv[2]=='1',sys.argv[3],None if sys.argv[4]=='unknown' else int(sys.argv[4]),len(sys.argv)>5 and sys.argv[5]=='root_token')
 except Exception as ex:
  result=dict(schema=1,result='UNKNOWN',events=[dict(pid=None,stage='probe',category=type(ex).__name__,errno=getattr(ex,'errno',None))])
 print(json.dumps(result,sort_keys=True))
 raise SystemExit(0 if result['result']=='QUIET' else 2)
"""

def wsl_quiet(record,legacy=False):
 # No game API/process calls. Read only same-user Linux process environments.
 token=record.get('build_token');distro=record.get('distro')
 if legacy and not record.get('_selected_distro'):
  try:p=subprocess.run(['wsl.exe','--list','--running','--quiet'],capture_output=True,timeout=25)
  except (OSError,subprocess.TimeoutExpired) as ex:raise OperationError('Cannot enumerate running WSL distributions; legacy artifact preserved') from ex
  if p.returncode:raise OperationError('Running WSL distribution query failed; legacy artifact preserved')
  raw=p.stdout;text=raw.decode('utf-16-le' if b'\0' in raw else 'utf-8-sig',errors='strict')
  for name in text.splitlines():
   name=name.strip().strip('\ufeff')
   if name:wsl_quiet({'distro':name,'_selected_distro':True},legacy=True)
  return
 if not token and not legacy:return
 # The user check binds the selected distro/default UID. Only EACCES can be
 # resolved by privileged reading; identity/reuse/malformed evidence is not washed.
 user=_wsl_query(record,legacy,False)
 allowed={'access_denied','absent_confirmed','exited_confirmed'}
 if user['result']=='ACTIVE' or (user['result']=='UNKNOWN' and (not user['counts'] or not set(user['counts'])<=allowed)):
  raise OperationError('WSL build not proved inactive: '+json.dumps(user,sort_keys=True))
 if legacy:
  if user['result']!='QUIET':raise OperationError('Legacy lock lacks exact token; privileged recovery refused: '+json.dumps(user,sort_keys=True))
  return user
 # Every token retirement requires all-UID inspection, even after a user QUIET:
 # surviving/reparented or UID-changed descendants must not escape the inventory.
 root=_wsl_query(record,False,True)
 if root['result']!='QUIET':raise OperationError('Read-only root token absence NOT proved: '+json.dumps(root,sort_keys=True))
 print('WSL_TOKEN_CHECK '+json.dumps(dict(result='TOKEN_ABSENT',inspection='root_token_all_uids',inspector_uid=root['uid_observed'],build_uid_expected=record.get('linux_uid'),user_uid_observed=user['uid_observed'],distro=distro,authority='token_retirement_only_not_deployment'),sort_keys=True))
 return {**root,'build_uid_expected':record.get('linux_uid'),'user_uid_observed':user['uid_observed'],'recovery_authority':'all_uid_exact_token_absence'}

def _wsl_query(record,legacy,root):
 token=record.get('build_token');distro=record.get('distro');uid=0 if root else record.get('linux_uid')
 if not isinstance(distro,str) or not distro or distro.startswith('-') or len(distro)>256 or '\0' in distro:raise OperationError('WSL missing/invalid selected distro identity; no inspection/recovery')
 if uid is not None and (type(uid) is not int or not 0<=uid<=0xffffffff):raise OperationError('WSL malformed inspector UID identity; no inspection/recovery')
 if token is not None and (not isinstance(token,str) or not 0<len(token)<=128 or '\0' in token):raise OperationError('WSL malformed exact token; no inspection/recovery')
 if root and (legacy or not isinstance(token,str) or not 0<len(token)<=128):raise OperationError('Read-only root probe requires a bounded exact operation token')
 args=['wsl.exe','--distribution',distro]+(['--user','root'] if root else [])+['--exec','python3','-c',WSL_PROBE,token or '',str(int(legacy)),distro or '',str(uid) if uid is not None else 'unknown','root_token' if root else 'user']
 try:p=subprocess.run(args,capture_output=True,text=True,timeout=25)
 except (OSError,subprocess.TimeoutExpired) as ex:raise OperationError('WSL probe unavailable: distro='+str(distro)+' inspection='+('root_token_all_uids' if root else 'user')+' stage=invoke category='+type(ex).__name__) from ex
 try:
  result=json.loads(p.stdout)
  if not isinstance(result,dict) or set(result)!={'schema','result','distro_expected','distro_observed','uid_expected','uid_observed','events','counts','inspection'}:raise ValueError('fields')
  if result['schema']!=1 or result['result'] not in ('QUIET','ACTIVE','UNKNOWN') or not isinstance(result['events'],list) or len(result['events'])>32 or not isinstance(result['counts'],dict) or type(result['uid_observed']) is not int:raise ValueError('schema')
  if result['distro_expected']!=distro or result['uid_expected']!=uid or result['inspection']!=('root_token_all_uids' if root else 'user'):raise ValueError('identity binding')
  if result['distro_observed']!=distro or (uid is not None and result['uid_observed']!=uid):raise ValueError('inspector identity')
 except (ValueError,TypeError):raise OperationError('WSL probe invalid output: distro='+str(distro)+' inspection='+('root' if root else 'user')+' stage=output category=malformed_or_identity exit='+str(p.returncode))
 if (p.returncode==0)!=(result['result']=='QUIET'):raise OperationError('WSL probe inconsistent exit/result: '+str(p.returncode))
 return {**result,'exit_status':p.returncode}

def legacy_quiet(game,record):
 if os.name!='nt':raise OperationError('Legacy recovery needs Windows process/WSL evidence; artifact preserved')
 script=r"""$ErrorActionPreference='Stop';$all=@(Get-CimInstance Win32_Process);$ignore=@([int]$args[0],[int]$PID);$id=[int]$args[0];while($id){$p=$all|Where-Object ProcessId -eq $id|Select-Object -First 1;if(-not $p){break};$id=[int]$p.ParentProcessId;$ignore+= $id};foreach($p in $all){if($ignore -contains [int]$p.ProcessId){continue};if($p.Name -match '^(python|pythonw|py|powershell|pwsh|cmd|wsl)(\.exe)?$'){if(-not $p.CommandLine){throw ('Cannot inspect candidate process PID='+$p.ProcessId)};if($p.CommandLine -match 'workspace\.py|workspace-build\.sh|Update-WS\.cmd|mod_environment\.py|Repair-Mod-Tools'){throw ('Possible active mod operation PID='+$p.ProcessId+' Name='+$p.Name+' Started='+$p.CreationDate)}}};'QUIET'"""
 script=script.replace('$args[0]',str(os.getpid()))
 p=subprocess.run(['powershell.exe','-NoProfile','-NonInteractive','-Command',script],capture_output=True,text=True,timeout=25)
 if p.returncode or p.stdout.strip()!='QUIET':raise OperationError('Legacy operation inactivity not proved: '+p.stderr.strip())
 if (game/'Mods Library/Mods-Developing/MSM2-WS/_source').exists():wsl_quiet(record,legacy=True)

def child_options():
 if not _current:return {}
 fds=[fd for _,fd,_ in _current.files]
 if os.name=='nt':
  import msvcrt
  si=subprocess.STARTUPINFO();si.lpAttributeList={'handle_list':[msvcrt.get_osfhandle(f) for f in fds]}
  return {'startupinfo':si,'close_fds':True}
 return {'pass_fds':tuple(fds)}

def phase(name,**extra):
 if _current:
  _current.record.update(phase=name,**extra)
  for _,fd,_ in _current.files:write(fd,_current.record)

def owned_artifact(path,game):
 try:return parsed(path.read_bytes(),game) is not None
 except OSError:return False

@contextlib.contextmanager
def hold(game,action='environment',quiescent=None):
 global _current
 game=Path(game).absolute();safe(game)
 if _current:raise OperationError('Nested operation ownership refused')
 class Held:pass
 h=Held();h.files=[];h.record=dict(format=FORMAT,game=str(game),workspace=str(game/'Mods Library/Mods-Developing/MSM2-WS'),pid=os.getpid(),start=identity(os.getpid()),token=uuid.uuid4().hex,action=action,state='active',phase='preflight')
 created=[]
 paths=[game/'.MSM2-Operations.guard',game/'.MSM2-Mod-Tools.lock']
 ws=game/'Mods Library/Mods-Developing/MSM2-WS/_archive'
 if ws.is_dir() and (action in ('update','repair') or os.path.lexists(ws/'Update-WS.lock')):paths.append(ws/'Update-WS.lock')
 try:
  for path in paths:
   fd,new=open_gate(path);h.files.append((path,fd,os.fstat(fd)))
   if new:created.append(fd)
   empty_legacy=not new and not read(fd) and path.name!='.MSM2-Operations.guard'
   data=read(fd);old=parsed(data,game)
   if old:
    if old['state']=='active' and identity(old['pid'])==old['start']:raise OperationError('Active operation metadata owner PID='+str(old['pid'])+' start='+old['start']+' action='+str(old.get('action')))
    if old.get('build_token'):(quiescent or (lambda g,r: wsl_quiet(r)))(game,old)
   elif data:
    if not data.strip().isdigit():raise OperationError('Malformed/unowned lock preserved: '+str(path)+'; inspect artifact before repair')
    pid=int(data.strip())
    if pid>0 and identity(pid) is not None:raise OperationError('Legacy PID-only lock is ambiguous/live, PID='+str(pid)+' path='+str(path))
    (quiescent or legacy_quiet)(game,{})
   if empty_legacy:(quiescent or legacy_quiet)(game,{})
   if (data and not old) or empty_legacy or (old and old['state']!='released'):
    folder=game/'.MSM2-lock-recovery'/h.record['token'];safe(folder);folder.mkdir(parents=True,exist_ok=True)
    with (folder/path.name).open('xb') as out:out.write(data)
  # Creation status comes from OPEN_ALWAYS/O_EXCL, not a racy existence snapshot.
  for _,fd,_ in h.files:write(fd,h.record)
  # Shared backup/restore may rename Mods Library; do not pin a file inside it.
  # Its compatibility marker remains present (old presence-only updaters refuse).
  if action not in ('update','repair'):
   for item in list(h.files):
    path,fd,st=item
    if path.name=='Update-WS.lock':
     write(fd,{**h.record,'state':'released'});os.close(fd);h.files.remove(item)
     if fd in created:created.remove(fd)
  _current=h
  try:yield h
  finally:
   h.record['state']='interrupted' if h.record.get('phase')=='building' else 'released'
   for path,fd,st in h.files:
    current=path.lstat()
    if (current.st_dev,current.st_ino)!=(st.st_dev,st.st_ino) or (parsed(read(fd),game) or {}).get('token')!=h.record['token']:raise OperationError('Operation artifact replaced; foreign state preserved: '+str(path))
    write(fd,h.record)
 finally:
  try:
   if _current is None:
    for fd in created:
     h.record['state']='released';write(fd,h.record)
  finally:
   _current=None
   for _,fd,_ in reversed(h.files):os.close(fd)
