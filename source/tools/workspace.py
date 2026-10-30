"""Fail-closed WS-only Windows migration/update; platform-neutral core is fixture-tested."""
from pathlib import Path
import retired_probes
try:import operation_lock
except ModuleNotFoundError:
 import sys
 sys.path.insert(0,str(Path(__file__).resolve().parents[2]/"environment"))
 import operation_lock
import contextlib
import argparse,hashlib,json,os,re,shutil,stat,subprocess,sys,zipfile
REMOTE='https://github.com/deluxxxe-dev/MSM2-WallSprint.git'
WS_NAME='MSM2-WS'
ACTIVE=re.compile(r'MSM2-WS-v[0-9A-Za-z.]+\.script\Z')
ARCHIVE_SUFFIXES={'.zip','.rar','.7z'}
LEGACY_SOURCE_PATTERNS=('/*','!*.[zZ][iI][pP]','!*.[rR][aA][rR]','!*.7[zZ]')
SOURCE_PATTERNS=LEGACY_SOURCE_PATTERNS+('!*.[sS][tT][aA][gG][eE]',)
class WorkspaceError(RuntimeError):pass

@contextlib.contextmanager
def update_ownership(game):
 try:
  with operation_lock.hold(game,'update') as h:yield h
 except RuntimeError as ex:
  if isinstance(ex,WorkspaceError):raise
  raise WorkspaceError(str(ex)) from ex

def require(condition,message):
 if not condition:raise WorkspaceError(message)
def run(args,cwd=None):
 p=subprocess.run([str(x) for x in args],cwd=cwd,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True,encoding='utf-8',errors='replace',**operation_lock.child_options())
 require(p.returncode==0,'Command failed: '+str(args[0])+' '+p.stderr.strip())
 return p.stdout.strip()
def linked(path):
 st=path.lstat()
 return stat.S_ISLNK(st.st_mode) or bool(getattr(st,'st_file_attributes',0)&0x400)
def no_links(path):
 require(path.exists() or path.is_symlink(),'Missing path: '+str(path))
 require(not linked(path),'Reparse point/symlink refused: '+str(path))
 if path.is_dir():
  for parent,dirs,files in os.walk(path,followlinks=False):
   for name in dirs+files:
    item=Path(parent)/name
    require(not linked(item),'Reparse point/symlink refused: '+str(item))
def live_scripts(path):
 if not path.exists():return []
 no_links(path)
 return sorted(p for p in path.rglob('*') if p.is_file() and p.suffix.lower()=='.script')
def live_archives(path):
 if not path.exists():return []
 no_links(path)
 return sorted(p for p in path.rglob('*') if p.is_file() and p.suffix.lower() in ARCHIVE_SUFFIXES)
def git(source,*args):return run(['git','-C',source,*args])
def source_patterns(source):
 path=source/'.git/info/sparse-checkout'
 return tuple(path.read_text().splitlines()) if path.exists() else None
def prepare_source_checkout(source):
 # Git owns these clean tracked files; keep their bytes/history in Git and
 # exclude archives from the scanned working tree, without legacy links.
 require(not git(source,'status','--porcelain'),'Preserve local work before configuring the source checkout')
 require(source_patterns(source) in (None,LEGACY_SOURCE_PATTERNS,SOURCE_PATTERNS),'Custom sparse checkout refused; preserve its configuration before migrating')
 git(source,'sparse-checkout','set','--no-cone',*SOURCE_PATTERNS)
 require(source_patterns(source)==SOURCE_PATTERNS,'Archive-excluding source checkout configuration failed')
def branch_name(value):
 require(value and not value.startswith('-'),'An explicit development branch is required')
 run(['git','check-ref-format','--branch',value]);return value

def game_preflight(game):
 game=Path(os.path.abspath(game))
 require(game.is_dir() and not linked(game),'Game root must be a real directory')
 require((game/'Spider-Man2.exe').is_file(),'Game root lacks Spider-Man2.exe')
 require((game/'scripts_proxy.dll').is_file(),'Game-root scripts_proxy.dll is required')
 require((game/'winmm.dll').is_file(),'Game-root winmm.dll is required')
 shared=game/'Overstrike'
 require(shared.is_dir() and not linked(shared),'Shared game-root Overstrike directory is required')
 require((shared/'Overstrike.exe').is_file() and not linked(shared/'Overstrike.exe'),'Shared Overstrike.exe must be an existing regular file')
 require(not os.path.lexists(shared/'winmm.dll'),'Shared Overstrike must not contain winmm.dll; fix shared installation manually')
 proxy=shared/'scripts_proxy.dll'
 require(not os.path.lexists(proxy) or (not linked(proxy) and proxy.is_file()),'Overstrike scripts_proxy.dll must be a regular non-reparse installer source')
 return game.resolve()
def workspace_paths(workspace):
 p=Path(os.path.abspath(workspace))
 require(p.name=='MSM2-WS' and p.parent.name=='Mods-Developing' and p.parent.parent.name=='Mods Library','Expected exact Mods Library/Mods-Developing/MSM2-WS layout')
 for part in [p,p.parent,p.parent.parent]:
  require(part.is_dir() and not linked(part),'Missing or reparse workspace component: '+str(part))
 game=game_preflight(p.parent.parent.parent)
 require(p.resolve()==game/'Mods Library'/'Mods-Developing'/'MSM2-WS','Workspace canonical path escaped the game root')
 return p.resolve(),game

def independent_clone(source,remote=REMOTE,allow_legacy_archives=False):
 require(source.is_dir(),'Independent _source clone is missing; use Migrate-WS.cmd')
 no_links(source)
 require((source/'.git').is_dir() and not linked(source/'.git'),'_source must be a standalone clone, not a worktree/gitdir link')
 require(Path(git(source,'rev-parse','--show-toplevel')).resolve()==source.resolve(),'Git root differs from _source')
 common=Path(git(source,'rev-parse','--git-common-dir'))
 if not common.is_absolute():common=source/common
 require(common.resolve()==(source/'.git').resolve(),'External Git common-dir/worktree refused')
 alternate=source/'.git/objects/info/alternates'
 require(not alternate.exists() or not alternate.read_text().strip(),'External/shared Git object dependency refused')
 # Missing config keys are normal; do not swallow other git failures.
 p=subprocess.run(['git','-C',str(source),'config','--get','core.worktree'],capture_output=True,text=True)
 require(p.returncode==1,'Explicit core.worktree or invalid Git config refused')
 require(git(source,'remote','get-url','origin').rstrip('/').removesuffix('.git').casefold()==remote.rstrip('/').removesuffix('.git').casefold(),'Origin is not the intended WallSprint remote')
 require(not git(source,'ls-files','*.script'),'Checkout tracks a live .script; use the migration-ready branch')
 require(not live_scripts(source),'_source contains active .script files; do not unpack install bundles there')
 if allow_legacy_archives:
  require(not git(source,'status','--porcelain','--untracked-files=no'),'_source has tracked changes; preserve/commit them before updating')
  untracked=[source/p for p in git(source,'ls-files','--others','--exclude-standard','-z').split('\0') if p]
  require(all(retired_probes.recognized(p) for p in untracked),'_source has untracked work; preserve it before updating')
 else:
  require(not git(source,'status','--porcelain'),'_source has local changes; preserve/commit them before updating')
 archives=live_archives(source)
 if allow_legacy_archives:
  tracked=set(git(source,'ls-files','-z').split('\0'))
  require(all(p.relative_to(source).as_posix() in tracked or retired_probes.recognized(p) for p in archives),'Untracked archive in _source refused; preserve it outside Mods Library')
 else:
  require(not archives,'_source contains Overstrike-visible archives, including nested scripts')
  require(source_patterns(source)==SOURCE_PATTERNS,'_source must exclude archives using the managed Git sparse checkout')
  require(git(source,'config','--bool','core.sparseCheckout')=='true' and git(source,'config','--bool','core.sparseCheckoutCone')=='false','Managed archive-excluding sparse checkout must remain active')
 return source

def expected_branch(source,expected=None):
 p=subprocess.run(['git','-C',str(source),'symbolic-ref','--quiet','--short','HEAD'],capture_output=True,text=True)
 require(p.returncode==0,'Detached HEAD is not a development branch')
 current=p.stdout.strip()
 if expected is None:
  p=subprocess.run(['git','-C',str(source),'config','--get','wallsprint.expectedBranch'],capture_output=True,text=True)
  require(p.returncode==0,'Expected-branch contract missing; specify --expected-branch')
  expected=p.stdout.strip()
 branch_name(expected);require(current==expected,'Unexpected branch; explicit --expected-branch is required after an intentional branch switch')
 require(git(source,'config','--get','branch.'+current+'.remote')=='origin','Branch must track origin')
 require(git(source,'config','--get','branch.'+current+'.merge')=='refs/heads/'+current,'Branch upstream must match the intended development branch')
 return current

def version(source):
 text=(source/'source/include/version.hpp').read_text()
 label=re.search(r'versionLabel\[\]="(v[0-9A-Za-z.]+)"',text)
 package=re.search(r'packageVersion\[\]="([0-9.]+)"',text)
 require(label and package,'Authoritative version metadata missing')
 return label[1],package[1]
def validate_payload(payload,manifest,label,package_version):
 require(payload.is_file() and not linked(payload),'Build did not produce a regular disabled script artifact')
 data=payload.read_bytes()
 require(hashlib.sha256(data).hexdigest()==manifest.get('sha256'),'Script hash does not match package validation')
 require(manifest.get('version')==label and manifest.get('package_version')==package_version,'Build manifest/version mismatch')
 with zipfile.ZipFile(payload) as z:
  require(z.testzip() is None and set(z.namelist())=={'WallSprint.dll','WallSprint.ini','info.json'},'Invalid script package/CRC')
  info=json.loads(z.read('info.json'))
  require(info.get('name')=='WallSprint' and info.get('version')==package_version,'Package is not current WallSprint')
  require(hashlib.sha256(z.read('WallSprint.dll')).hexdigest()==manifest.get('dll_sha256'),'DLL hash mismatch')
 return data

def owned_script(path):
 if not ACTIVE.fullmatch(path.name) or linked(path):return False
 try:
  with zipfile.ZipFile(path) as z:return json.loads(z.read('info.json')).get('name')=='WallSprint'
 except (OSError,ValueError,KeyError,zipfile.BadZipFile):return False

def owned_build_archive(path):
 """Only positively identified outputs of the old WS build can be disabled."""
 match=re.fullmatch(r'MSM2-WS-(v[0-9A-Za-z.]+)(-source)?\.zip',path.name,re.IGNORECASE)
 if not match or linked(path):return False
 label=match[1]
 try:
  with zipfile.ZipFile(path) as z:
   if len(z.infolist())>4096 or sum(i.file_size for i in z.infolist())>512*1024*1024 or z.testzip() is not None:return False
   if match[2]:
    info=json.loads(z.read('source/mod/info.json'))
    header=z.read('source/include/version.hpp').decode('utf-8')
    return info.get('name')=='WallSprint' and 'versionLabel[]="'+label+'"' in header and 'source/tools/workspace.py' in z.namelist()
   prefix='MSM2-WS-'+label+'/'
   manifest=json.loads(z.read(prefix+'BUILD_MANIFEST.json'))
   name=prefix+'MSM2-WS-'+label+'.script'
   if [p for p in z.namelist() if p.lower().endswith('.script')]!=[name]:return False
   data=z.read(name)
   if manifest.get('version')!=label or hashlib.sha256(data).hexdigest()!=manifest.get('sha256'):return False
   import io
   with zipfile.ZipFile(io.BytesIO(data)) as script:
    info=json.loads(script.read('info.json'))
    return script.testzip() is None and info.get('name')=='WallSprint' and info.get('version')==manifest.get('package_version') and hashlib.sha256(script.read('WallSprint.dll')).hexdigest()==manifest.get('dll_sha256')
 except (OSError,ValueError,KeyError,UnicodeError,RuntimeError,zipfile.BadZipFile):return False

def disable_legacy_build_archives(workspace):
 paths=live_archives(workspace/'_archive')
 require(all(p.parent==workspace/'_archive/_build' and owned_build_archive(p) for p in paths),'Unowned archive refused; no unrelated file will be moved')
 saved=[]
 try:
  for path in paths:
   target=workspace/'_archive'/(path.name+'.disabled');n=0
   while target.exists():
    n+=1;target=workspace/'_archive'/(path.name+'-'+str(n).zfill(2)+'.disabled')
   path.rename(target);saved.append((path,target))
   print('Disabled legacy WS build archive: '+str(path.relative_to(workspace))+' -> '+str(target.relative_to(workspace)))
 except BaseException:
  for old,target in reversed(saved):target.rename(old)
  raise

def assert_layout(workspace,expected=None,allow_legacy_archives=False):
 source=workspace/'_source'
 for folder in [source,workspace/'logs',workspace/'_archive']:
  require(folder.is_dir() and not linked(folder),'Missing/reparse internal folder: '+folder.name)
  require(not live_scripts(folder),'Active .script is forbidden in '+folder.name)
 require(not any(p.is_file() and p.suffix.lower()=='.stage' for p in workspace.rglob('*')),'Scanner-visible stage refused in WS workspace; unrelated stages must be preserved manually outside Mods Library')
 scripts=live_scripts(workspace)
 tracked=set(git(source,'ls-files','-z').split('\0')) if allow_legacy_archives else set()
 for archive in live_archives(workspace):
  require(allow_legacy_archives,'Overstrike-visible archive refused: '+str(archive)+'; ZIP/RAR/7z contents are scanned too')
  source_tracked=archive.is_relative_to(source) and archive.relative_to(source).as_posix() in tracked
  old_build=archive.parent==workspace/'_archive/_build' and owned_build_archive(archive)
  require(source_tracked or old_build,'Untracked or unowned Overstrike-visible archive refused: '+str(archive))
 if expected is not None:
  require(scripts==[workspace/expected],'Exactly one root active WS script must remain')
 else:
  require(all(p.parent==workspace and owned_script(p) for p in scripts),'Unowned/nested active .script refused; no unrelated mod will be moved')
 return scripts

def shared_tools(source=None):
 import importlib.util
 if source is not None:path=source/'environment/mod_environment.py'
 else:
  bundled=Path(__file__).parent/'shared/mod_environment.py'
  path=bundled if bundled.is_file() else Path(__file__).resolve().parents[2]/'environment/mod_environment.py'
 require(path.is_file() and not linked(path),'Current shared toolkit source missing: '+str(path))
 spec=importlib.util.spec_from_file_location('ws_owned_shared_tools',path);m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
 return m

class WindowsSystem:
 def stop(self,game):
  require(os.name=='nt','Use Windows for real migration/update')
  shared_tools().Windows().quiet(game)
 def build(self,source,destination):
  require(shutil.which('wsl.exe'),'WSL2 is required; see source/BUILD.md. Build/tests are not skipped.')
  distro=os.environ.get('MSM2_WS_WSL_DISTRO') or run(['wsl.exe','--exec','printenv','WSL_DISTRO_NAME'])
  require(distro and not distro.startswith('-'),'Cannot bind the WSL distribution identity')
  prefix=['wsl.exe','--distribution',distro,'--exec']
  require('wsl2' in run(prefix+['uname','-r']).lower(),'WSL2 Linux distribution is required for the supported cross-build')
  src=run(prefix+['wslpath','-u',source])
  dest=run(prefix+['wslpath','-u',destination])
  script=src+'/source/tools/windows/workspace-build.sh'
  token=operation_lock._current.record['token']
  operation_lock.phase('building',build_token=token,distro=distro)
  p=subprocess.run(prefix+['env','MSM2_WS_OPERATION_TOKEN='+token,'bash','--noprofile','--norc',script,src,dest],**operation_lock.child_options())
  operation_lock.wsl_quiet(operation_lock._current.record)
  operation_lock.phase('validated',build_token=None)
  require(p.returncode==0,'Required WSL build/test/package pipeline failed; previous active script retained')
  result=json.loads((destination/'Build-WS-result.json').read_text())
  name=result.get('artifact','')
  require(re.fullmatch(r'MSM2-WS-v[0-9A-Za-z.]+\.script\.disabled',name),'Invalid build artifact basename')
  payload=destination/name
  return payload,json.loads((destination/'BUILD_MANIFEST.json').read_text())
 def quiet(self,game):self.stop(game)
 def refresh(self,source,game):
  m=shared_tools(source);m.environment_ready(game);m.refresh_kit(game,self)
 def shortcuts(self,game):shared_tools().Windows().shortcuts(game)
 def launch(self,game):shared_tools().start_manager(game)

class Manager:
 def __init__(self,system=None,remote=REMOTE):self.system=system or WindowsSystem();self.remote=remote
 def migrate(self,game,branch):
  require(shutil.which('git'),'Git for Windows is required; no migration action started')
  game=game_preflight(game);branch=branch_name(branch)
  with update_ownership(game):
   parent=game/'Mods Library'/'Mods-Developing'
   # Never follow a pre-existing junction into SF or a legacy checkout.
   for p in [game/'Mods Library',parent,parent/WS_NAME]:
    if p.exists() or p.is_symlink():require(p.is_dir() and not linked(p),'Reparse/non-directory migration destination refused')
    else:p.mkdir()
   workspace=parent/WS_NAME
   for name in ['logs','_archive']:
    p=workspace/name
    if not p.exists():p.mkdir()
    require(p.is_dir() and not linked(p),'Unsafe workspace folder: '+name)
   source=workspace/'_source'
   self.system.stop(game)
   if os.path.lexists(source):
    require(source.is_dir() and not linked(source),'Existing _source must not be a file or reparse dependency')
   if not source.exists():
    run(['git','-c','core.longpaths=true','clone','--no-local','--single-branch','--branch',branch,self.remote,source])
    git(source,'config','core.longpaths','true')
    git(source,'config','wallsprint.expectedBranch',branch)
   independent_clone(source,self.remote,allow_legacy_archives=True)
   # Existing independent clones must match the explicit migration branch.
   expected_branch(source,branch)
  self.update(workspace,branch)
  migration=source/'source/tools/windows/Migrate-WS.cmd'
  require(migration.is_file(),'Validated checkout lacks Migrate-WS.cmd')
  (workspace/'Migrate-WS.cmd').write_bytes(migration.read_bytes())
  print('Local migration checks passed; the old environment was not deleted. Only after confirming shared Overstrike UI recognition of ordinary mods + SF + WS and a new versioned runtime log under MSM2-WS/logs may the old WallSprint environment be manually removed.')
  return workspace
 def update(self,workspace,expected=None):
  require(shutil.which('git'),'Git for Windows is required; no update action started')
  workspace,game=workspace_paths(workspace)
  for name in ['logs','_archive']:
   p=workspace/name
   if not p.exists():p.mkdir()
  no_links(workspace)
  source=independent_clone(workspace/'_source',self.remote,allow_legacy_archives=True)
  branch=expected_branch(source,expected)
  with update_ownership(game):
   self.system.stop(game)
   tracked=set(git(source,'ls-files','-z').split('\0'))
   retired_probes.retire(workspace,game,tracked)
   prepare_source_checkout(source)
   git(source,'pull','--ff-only')
   independent_clone(source,self.remote);expected_branch(source,branch)
   disable_legacy_build_archives(workspace)
   assert_layout(workspace)
   git(source,'config','wallsprint.expectedBranch',branch)
   head=git(source,'rev-parse','HEAD');label,pkg=version(source)
   target='MSM2-WS-'+label+'.script'
   build=workspace/'_archive/_build'
   if not build.exists():build.mkdir()
   no_links(build)
   payload,manifest=self.system.build(source,build)
   require(payload.parent.resolve()==build.resolve(),'Build artifact escaped WS staging directory')
   require(payload.name=='MSM2-WS-'+label+'.script.disabled','Artifact is not the authoritative version')
   data=validate_payload(payload,manifest,label,pkg)
   independent_clone(source,self.remote)
   require(git(source,'rev-parse','HEAD')==head,'Source changed during validation')
   game_preflight(game);previous=assert_layout(workspace)
   if hasattr(self.system,'refresh'):self.system.refresh(source,game)
   helper=source/'source/tools/windows/Update-WS.cmd'
   require(helper.is_file(),'Validated checkout lacks Update-WS.cmd')
   before=(workspace/'Update-WS.cmd').read_bytes() if (workspace/'Update-WS.cmd').exists() else None
   saved=[];active=workspace/target;deployed=False
   stage=build/'Deploy-WS.script.disabled'
   with stage.open('wb') as f:f.write(data);f.flush();os.fsync(f.fileno())
   try:
    for old in previous:
     archive=workspace/'_archive'/(old.name+'.disabled');n=0
     while archive.exists():
      n+=1;archive=workspace/'_archive'/(old.stem+'-'+str(n).zfill(2)+'.script.disabled')
     old.rename(archive);saved.append((old,archive))
    os.replace(stage,active);deployed=True
    (workspace/'Update-WS.cmd').write_bytes(helper.read_bytes())
    assert_layout(workspace,target)
   except BaseException:
    if deployed and active.exists() and hashlib.sha256(active.read_bytes()).hexdigest()==hashlib.sha256(data).hexdigest():active.unlink()
    for old,archive in reversed(saved):archive.rename(old)
    if before is None:
     if (workspace/'Update-WS.cmd').exists():(workspace/'Update-WS.cmd').unlink()
    else:(workspace/'Update-WS.cmd').write_bytes(before)
    raise
  # Deployment is committed and OS ownership released before checked launch.
  try:self.system.launch(game)
  except Exception as ex:raise WorkspaceError('WS deployment committed; manager launch failed separately: '+str(ex)) from ex
  print('WS update complete: '+target+'; select ordinary mods + SF + WS, then Install mods -> Run modded game.')
  return active

def main():
 p=argparse.ArgumentParser(description=__doc__);sub=p.add_subparsers(dest='action',required=True)
 m=sub.add_parser('migrate');m.add_argument('--game-root',default=os.environ.get('MSM2_WS_GAME_ROOT'));m.add_argument('--branch',default=os.environ.get('MSM2_WS_EXPECTED_BRANCH'))
 u=sub.add_parser('update');u.add_argument('--workspace',required=True);u.add_argument('--expected-branch')
 args=p.parse_args()
 require(sys.version_info >= (3,11),'Windows Python3.11 or newer is required')
 require(os.name=='nt','Real workspace helpers require Windows Python3.11+, Git for Windows and WSL2; fixture tests are separate')
 if args.action=='migrate':
  require(args.game_root,'Specify --game-root (no personal path is embedded)')
  Manager().migrate(args.game_root,args.branch)
 else:Manager().update(args.workspace,args.expected_branch)
if __name__=='__main__':
 try:main()
 except (WorkspaceError,OSError,ValueError,zipfile.BadZipFile) as e:
  print('WS ERROR: '+str(e),file=sys.stderr);sys.exit(1)
