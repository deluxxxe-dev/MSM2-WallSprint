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
 script="""import os,sys,json
found=[];uid=os.getuid()
for n in os.listdir('/proc'):
 if not n.isdigit() or int(n)==os.getpid():continue
 p='/proc/'+n
 try:
  if os.stat(p).st_uid!=uid:continue
  env=open(p+'/environ','rb').read();cmd=open(p+'/cmdline','rb').read()+b' '+os.fsencode(os.readlink(p+'/cwd'))
 except FileNotFoundError:continue
 except PermissionError:raise SystemExit('Cannot verify same-user Linux process '+n)
 if (sys.argv[1] and ('MSM2_WS_OPERATION_TOKEN='+sys.argv[1]).encode() in env.split(b'\\0')) or (sys.argv[2]=='1' and any(x in cmd for x in [b'/msm2-ws-validation.',b'workspace-build.sh',b'Mods-Developing/MSM2-WS'])):found.append(n)
if found:raise SystemExit('Remaining WSL build PID(s): '+','.join(found))
print('QUIET')
"""
 args=['wsl.exe']+(['--distribution',distro] if distro else [])+['--exec','python3','-c',script,token or '',str(int(legacy))]
 try:p=subprocess.run(args,capture_output=True,text=True,timeout=25)
 except (OSError,subprocess.TimeoutExpired) as ex:raise OperationError('WSL build inactivity unavailable; preserve lock. Distro='+str(distro or 'default')) from ex
 if p.returncode or p.stdout.strip()!='QUIET':raise OperationError('WSL build not proved inactive: '+p.stderr.strip()+' '+p.stdout.strip())

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
