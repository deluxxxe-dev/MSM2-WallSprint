"""Deterministic owned Git/filesystem fixtures. No real game/mod/legacy directory."""
from pathlib import Path
import contextlib,hashlib,importlib.util,json,os,shutil,subprocess,tempfile,zipfile
spec=importlib.util.spec_from_file_location('ws_workspace',Path(__file__).with_name('workspace.py'));w=importlib.util.module_from_spec(spec);spec.loader.exec_module(w)
TOOLS=Path(__file__).parent
BRANCH='codex/fixture-next-task'

def cmd(*args):return w.run(args)
def snapshot(root):
 return {str(p.relative_to(root)):('dir',p.stat().st_mtime_ns) if p.is_dir() else ('file',p.stat().st_size,p.stat().st_mtime_ns,hashlib.sha256(p.read_bytes()).hexdigest()) for p in [root,*root.rglob('*')]}
def expect_failure(call):
 try:call()
 except (w.WorkspaceError,OSError,ValueError,zipfile.BadZipFile):return
 raise AssertionError('Operation unexpectedly succeeded')
def payload(path,label,pkg):
 dll=b'MZ independent owned fixture DLL; never executed'
 with zipfile.ZipFile(path,'w') as z:
  z.writestr('info.json',json.dumps({'name':'WallSprint','version':pkg}))
  z.writestr('WallSprint.ini','[WallSprint]\nWALLSPRINT_MOVEMENT_WRITE=0\n')
  z.writestr('WallSprint.dll',dll)
 return {'version':label,'package_version':pkg,'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'dll_sha256':hashlib.sha256(dll).hexdigest()}
class FakeSystem:
 def __init__(self):self.events=[];self.fail=None
 def stop(self,game):
  self.events.append(('stop',game))
  if self.fail=='stop':raise w.WorkspaceError('owned process-stop failure')
 def build(self,source,dest):
  self.events.append(('build',source))
  if self.fail in ['build','test','package']:raise w.WorkspaceError('owned required '+self.fail+' failure')
  label,pkg=w.version(source);p=dest/('MSM2-WS-'+label+'.script.disabled');m=payload(p,label,pkg)
  if self.fail=='hash':m['sha256']='0'*64
  if self.fail=='dll':m['dll_sha256']='0'*64
  return p,m
 def launch(self,game):
  self.events.append(('launch',game/'Overstrike/Overstrike.exe',game))
  if self.fail=='launch':raise OSError('owned launch failure')
@contextlib.contextmanager
def fixture():
 with tempfile.TemporaryDirectory(prefix='msm2-ws-workspace-') as tmp:
  root=Path(tmp)/'Space 日本語';root.mkdir();game=root/'Game Root';game.mkdir()
  for name in ['Spider-Man2.exe','scripts_proxy.dll','winmm.dll']:(game/name).write_bytes(b'owned shared fixture '+name.encode())
  (game/'Overstrike').mkdir();(game/'Overstrike/Overstrike.exe').write_bytes(b'owned shared executable marker; not launched')
  sf=game/'Mods Library/Mods-Developing/MSM2-SF';sf.mkdir(parents=True);(sf/'SF.script').write_bytes(b'SF must never be changed')
  ordinary=game/'Mods Library/Ordinary Mods';ordinary.mkdir();(ordinary/'other.script').write_bytes(b'ordinary mod unchanged')
  legacy=root/'Legacy checkout';legacy.mkdir();(legacy/'keep.txt').write_bytes(b'never automatically delete this environment')
  repo=root/'upstream';repo.mkdir();cmd('git','init','-b',BRANCH,repo);cmd('git','-C',repo,'config','user.name','Owned Fixture');cmd('git','-C',repo,'config','user.email','workspace@example.invalid')
  (repo/'source/include').mkdir(parents=True);(repo/'source/tools/windows').mkdir(parents=True)
  (repo/'source/include/version.hpp').write_text('inline constexpr char versionLabel[]="v0.1c20";\ninline constexpr char packageVersion[]="0.1.38";\n')
  for name in ['Update-WS.cmd','Migrate-WS.cmd']:shutil.copy2(TOOLS/'windows'/name,repo/'source/tools/windows'/name)
  (repo/'README.md').write_text('owned independent upstream fixture')
  cmd('git','-C',repo,'add','.');cmd('git','-C',repo,'commit','-m','owned fixture')
  remote=root/'remote.git';cmd('git','clone','--bare','--no-local',repo,remote);cmd('git','-C',repo,'remote','add','origin',remote)
  system=FakeSystem();manager=w.Manager(system,str(remote))
  before=(snapshot(sf),snapshot(ordinary),snapshot(legacy))
  try:yield root,game,repo,remote,system,manager,sf,ordinary,legacy
  finally:assert before==(snapshot(sf),snapshot(ordinary),snapshot(legacy)),'SF/ordinary/legacy fixture mutated'

def migrate(f):
 root,game,repo,remote,system,manager,*_=f
 ws=manager.migrate(game,BRANCH)
 assert ws==game/'Mods Library/Mods-Developing/MSM2-WS'
 return ws

with fixture() as f:
 ws=migrate(f);source=ws/'_source'
 assert (ws/'Update-WS.cmd').is_file() and (ws/'Migrate-WS.cmd').is_file()
 assert w.live_scripts(source)==[] and w.live_scripts(ws/'logs')==[] and w.live_scripts(ws/'_archive')==[]
 assert (source/'.git').is_dir() and not (source/'.git/objects/info/alternates').exists()
 assert f[4].events[-1]==('launch',f[1]/'Overstrike/Overstrike.exe',f[1])
 for bad in ['Mods_Developing','MSM2_WS','Update_WS.cmd','logs_WS','WS_logs','log_wallsprint']:
  assert not any(p.name==bad for p in f[1].rglob('*'))
 print('PASS: workspace migration creates exact Unicode/space layout, independent non-shared Git clone, root-only active version script and shared Overstrike CWD; legacy/SF/ordinary state untouched')
with fixture() as f:
 ws=migrate(f);repo=f[2];manager=f[5]
 (repo/'source/include/version.hpp').write_text('inline constexpr char versionLabel[]="v0.1c21";\ninline constexpr char packageVersion[]="0.1.39";\n')
 cmd('git','-C',repo,'commit','-am','next owned task version');cmd('git','-C',repo,'push','origin',BRANCH)
 manager.update(ws)
 assert [p.name for p in w.live_scripts(ws)]==['MSM2-WS-v0.1c21.script']
 assert (ws/'_archive/MSM2-WS-v0.1c20.script.disabled').exists()
 manager.update(ws)
 assert (ws/'_archive/MSM2-WS-v0.1c21.script.disabled').exists()
 assert not w.live_scripts(ws/'_source') and not w.live_scripts(ws/'_archive')
 print('PASS: safe ff-only update uses authoritative version, archives only owned old versions as .script.disabled, preserves archive collisions and exactly one recursively active WS script')
with fixture() as f:
 ws=migrate(f);old=(ws/'MSM2-WS-v0.1c20.script').read_bytes()
 for bad in ['build','test','package','hash','dll','launch','stop']:
  f[4].fail=bad;expect_failure(lambda:f[5].update(ws))
  assert (ws/'MSM2-WS-v0.1c20.script').read_bytes()==old,bad
  assert len(w.live_scripts(ws))==1 and not (ws/'_archive/Update-WS.lock').exists()
 f[4].fail=None
 print('PASS: failed stop/build/test/package/hash/launch retain or roll back the exact previous active WS bytes and helpers; no partial new deployment or sibling mutations')
with fixture() as f:
 ws=migrate(f);game=f[1];manager=f[5];old=(ws/'MSM2-WS-v0.1c20.script').read_bytes()
 for name in ['winmm.dll','scripts_proxy.dll']:
  p=game/'Overstrike'/name;p.write_bytes(b'forbidden shared duplicate')
  expect_failure(lambda:manager.update(ws));assert p.read_bytes()==b'forbidden shared duplicate';p.unlink()
 p=game/'scripts_proxy.dll';data=p.read_bytes();p.unlink();expect_failure(lambda:manager.update(ws));p.write_bytes(data)
 p=ws/'foreign.script';p.write_bytes(b'not positively WS owned');expect_failure(lambda:manager.update(ws));assert p.read_bytes()==b'not positively WS owned';p.unlink()
 assert (ws/'MSM2-WS-v0.1c20.script').read_bytes()==old
 print('PASS: shared Overstrike duplicate DLL/missing game dependency and foreign active script fail closed without cleaning shared infrastructure or touching the old WS build')
with fixture() as f:
 ws=migrate(f);source=ws/'_source';manager=f[5]
 cmd('git','-C',source,'checkout','--detach');expect_failure(lambda:manager.update(ws));cmd('git','-C',source,'checkout',BRANCH)
 cmd('git','-C',source,'config','wallsprint.expectedBranch','codex/another');expect_failure(lambda:manager.update(ws));cmd('git','-C',source,'config','wallsprint.expectedBranch',BRANCH)
 (source/'dirty.txt').write_text('user work must remain');expect_failure(lambda:manager.update(ws));assert (source/'dirty.txt').read_text()=='user work must remain';(source/'dirty.txt').unlink()
 (source/'nested').mkdir();p=source/'nested/MSM2-WS-v0.1c1.script';p.write_bytes(b'active source artifact');expect_failure(lambda:manager.update(ws));assert p.exists();p.unlink();(source/'nested').rmdir()
 cmd('git','-C',source,'config','user.name','Owned Fixture');cmd('git','-C',source,'config','user.email','workspace@example.invalid')
 (source/'README.md').write_text('owned local committed change');cmd('git','-C',source,'commit','-am','local divergence')
 (f[2]/'README.md').write_text('owned remote committed change');cmd('git','-C',f[2],'commit','-am','remote divergence');cmd('git','-C',f[2],'push','origin',BRANCH)
 expect_failure(lambda:manager.update(ws))
 print('PASS: detached/unexpected branch, dirty source and nested active script refuse without discarding work, skipping validation or silently switching branches')
with fixture() as f:
 ws=migrate(f);source=ws/'_source';manager=f[5]
 p=source/'.git/objects/info/alternates';p.parent.mkdir(exist_ok=True);p.write_text(str(f[8]));expect_failure(lambda:manager.update(ws));p.unlink()
 backup=source/'.git-saved';(source/'.git').rename(backup);(source/'.git').write_text('gitdir: '+str(backup));expect_failure(lambda:manager.update(ws));(source/'.git').unlink();backup.rename(source/'.git')
 link=source/'external-link';link.symlink_to(f[8],target_is_directory=True);expect_failure(lambda:manager.update(ws));link.unlink()
 saved=ws.parent/'source-saved';source.rename(saved);source.symlink_to(saved,target_is_directory=True);expect_failure(lambda:manager.update(ws));source.unlink();saved.rename(source)
 saved=ws.parent/'source-saved';source.rename(saved);target=f[8]/'missing-source';source.symlink_to(target,target_is_directory=True)
 expect_failure(lambda:manager.migrate(f[1],BRANCH));assert not target.exists();source.unlink();saved.rename(source)
 print('PASS: alternates, worktree gitdir-file, nested symlink and source-root junction/symlink dependencies refuse; no legacy directory is copied, backed up or deleted')
with fixture() as f:
 ws=migrate(f);manager=f[5];root=f[1]/'Mods Library'
 for bad in [root/'Mods_Developing/MSM2-WS',root/'Mods-Developing/MSM2_WS',root/'other/MSM2-WS']:
  expect_failure(lambda:manager.update(bad));assert not bad.exists()
 lock=ws/'_archive/Update-WS.lock';lock.write_text('owned other update');expect_failure(lambda:manager.update(ws));assert lock.read_text()=='owned other update';lock.unlink()
 print('PASS: exact hyphenated workspace and exclusive update lock reject aliases/concurrent mutation without creating forbidden names or fallback libraries')
for name in ['workspace.py','windows/Update-WS.cmd','windows/Migrate-WS.cmd','windows/workspace-build.sh','windows/stop-processes.ps1']:
 text=(TOOLS/name).read_text()
 assert 'D:\\' not in text and 'C:\\SteamLibrary' not in text and 'AppData' not in text
pipeline=(TOOLS/'windows/workspace-build.sh').read_text()
for name in ['build_windows.sh','test.sh','smoke_windows.sh','test_hooks_windows.sh','release.py']:assert name in pipeline
assert 'pull' in (TOOLS/'workspace.py').read_text() and '--ff-only' in (TOOLS/'workspace.py').read_text()
print('PASS: generated helpers have no personal drive/AppData dependency; supported WSL pipeline retains all existing build/test/package suites and no skip-validation interface')
