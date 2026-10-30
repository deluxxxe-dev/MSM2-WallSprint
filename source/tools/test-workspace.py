"""Deterministic owned Git/filesystem fixtures. No real game/mod/legacy directory."""
from pathlib import Path
import contextlib,hashlib,importlib.util,io,json,os,shutil,subprocess,tempfile,zipfile
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
  z.writestr('info.json',json.dumps({'name':'WallSprint','game':'MSM2','version':pkg}))
  z.writestr('WallSprint.ini','[WallSprint]\nWALLSPRINT_MOVEMENT_WRITE=0\n')
  z.writestr('WallSprint.dll',dll)
 return {'version':label,'package_version':pkg,'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'dll_sha256':hashlib.sha256(dll).hexdigest()}
def bundles(folder,label,pkg,script,manifest,disabled=True):
 result=[];suffix='.disabled' if disabled else '';prefix='MSM2-WS-'+label+'/'
 p=folder/('MSM2-WS-'+label+'.zip'+suffix)
 with zipfile.ZipFile(p,'w') as z:
  z.writestr(prefix+'MSM2-WS-'+label+'.script',script.read_bytes())
  z.writestr(prefix+'BUILD_MANIFEST.json',json.dumps(manifest))
 result.append(p);p=folder/('MSM2-WS-'+label+'-source.zip'+suffix)
 with zipfile.ZipFile(p,'w') as z:
  z.writestr('source/mod/info.json',json.dumps({'name':'WallSprint','game':'MSM2','version':pkg}))
  z.writestr('source/include/version.hpp','inline constexpr char versionLabel[]="'+label+'";')
  z.writestr('source/tools/workspace.py','owned fixture marker; never executed')
 result.append(p);return result
def detected_wall_sprint(root):
 # Independent model of Overstrike's extension dispatch and recursive ZIP
 # discovery, including .script members that were never extracted to disk.
 found=[]
 def inspect(data,name):
  suffix=Path(name).suffix.lower()
  if suffix=='.script':
   with zipfile.ZipFile(io.BytesIO(data)) as z:
    info=json.loads(z.read('info.json'))
    if info.get('name')=='WallSprint' and info.get('game')=='MSM2':found.append(name)
  elif suffix=='.zip':
   with zipfile.ZipFile(io.BytesIO(data)) as z:
    for member in z.namelist():
     if Path(member).suffix.lower() in {'.script','.zip'}:inspect(z.read(member),name+'||'+member)
 for p in root.rglob('*'):
  if p.is_file() and p.suffix.lower() in {'.script','.zip'}:inspect(p.read_bytes(),str(p))
 return found
class FakeSystem:
 def __init__(self):self.events=[];self.fail=None
 def stop(self,game):
  self.events.append(('stop',game))
  if self.fail=='stop':raise w.WorkspaceError('owned process-stop failure')
 def build(self,source,dest):
  self.events.append(('build',source))
  if self.fail in ['build','test','package']:raise w.WorkspaceError('owned required '+self.fail+' failure')
  label,pkg=w.version(source);p=dest/('MSM2-WS-'+label+'.script.disabled');m=payload(p,label,pkg)
  bundles(dest,label,pkg,p,m)
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
 for name in ['winmm.dll']:
  p=game/'Overstrike'/name;p.write_bytes(b'forbidden shared duplicate')
  expect_failure(lambda:manager.update(ws));assert p.read_bytes()==b'forbidden shared duplicate';p.unlink()
 p=game/'Overstrike/scripts_proxy.dll';p.write_bytes(b'owned installer source');w.game_preflight(game);assert p.read_bytes()==b'owned installer source';p.unlink()
 p.mkdir();expect_failure(lambda:w.game_preflight(game));p.rmdir()
 p.symlink_to(game/'scripts_proxy.dll');expect_failure(lambda:w.game_preflight(game));p.unlink()
 p.symlink_to(game/'missing-proxy');expect_failure(lambda:w.game_preflight(game));p.unlink()
 for name in ['Spider-Man2.exe','scripts_proxy.dll','winmm.dll']:
  required=game/name;saved=required.read_bytes();required.unlink();expect_failure(lambda:w.game_preflight(game));required.mkdir();expect_failure(lambda:w.game_preflight(game));required.rmdir();required.write_bytes(saved)
 p=game/'scripts_proxy.dll';data=p.read_bytes();p.unlink();expect_failure(lambda:manager.update(ws));p.write_bytes(data)
 p=ws/'foreign.script';p.write_bytes(b'not positively WS owned');expect_failure(lambda:manager.update(ws));assert p.read_bytes()==b'not positively WS owned';p.unlink()
 assert (ws/'MSM2-WS-v0.1c20.script').read_bytes()==old
 print('PASS: misplaced winmm/unsafe installer source/missing game dependency and foreign active script fail closed without cleaning shared infrastructure or touching the old WS build')
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

with fixture() as f:
 repo=f[2];p=repo/'owned.script.disabled';m=payload(p,'v0.1c20','0.1.38');main,src=bundles(repo,'v0.1c20','0.1.38',p,m,disabled=False)
 (repo/'nested').mkdir();nested=repo/'nested/History.ZiP';nested.write_bytes(main.read_bytes())
 cmd('git','-C',repo,'add','.');cmd('git','-C',repo,'commit','-m','owned historical ZIPs');cmd('git','-C',repo,'push','origin',BRANCH)
 ws=migrate(f);source=ws/'_source'
 assert not w.live_archives(source) and len(detected_wall_sprint(ws))==1
 for original in [main,src,nested]:
  name=original.relative_to(repo).as_posix()
  assert not (source/name).exists() and subprocess.check_output(['git','-C',str(source),'show','HEAD:'+name])==original.read_bytes()
 assert (source/'source/include/version.hpp').is_file() and not w.git(source,'status','--porcelain')
 main.write_bytes(src.read_bytes());cmd('git','-C',repo,'commit','-am','next tracked archive bytes');cmd('git','-C',repo,'push','origin',BRANCH)
 f[5].update(ws)
 assert not w.live_archives(source) and len(detected_wall_sprint(ws))==1
 assert w.source_patterns(source)==w.SOURCE_PATTERNS
 print('PASS: independent source Git sparse checkout excludes root/nested/mixed-case archives while preserving all tracked bytes/history, clean full code and safe ff-only archive updates')

with fixture() as f:
 repo=f[2];p=repo/'owned.script.disabled';m=payload(p,'v0.1c20','0.1.38');bundles(repo,'v0.1c20','0.1.38',p,m,disabled=False)
 cmd('git','-C',repo,'add','.');cmd('git','-C',repo,'commit','-m','old checkout archives');cmd('git','-C',repo,'push','origin',BRANCH)
 ws=migrate(f);source=ws/'_source';cmd('git','-C',source,'sparse-checkout','disable')
 # The active ZIP can have a different creation timestamp from the upstream
 # fixture. Its manifest must describe its actual bytes, as the real build does.
 m=dict(m,sha256=hashlib.sha256((ws/'MSM2-WS-v0.1c20.script').read_bytes()).hexdigest())
 old=bundles(ws/'_archive/_build','v0.1c20','0.1.38',ws/'MSM2-WS-v0.1c20.script',m,disabled=False)
 before={p.name:(p.read_bytes(),p.stat().st_mtime_ns) for p in old}
 collision=ws/'_archive'/(old[0].name+'.disabled');collision.write_bytes(b'owned previous disabled cache')
 assert len(detected_wall_sprint(ws))==3
 f[5].update(ws)
 assert len(detected_wall_sprint(ws))==1 and not w.live_archives(ws)
 for name,(data,mtime) in before.items():
  found=[p for p in (ws/'_archive').glob(name+'*.disabled') if p.read_bytes()==data]
  assert len(found)==1 and found[0].stat().st_mtime_ns==mtime
 assert collision.read_bytes()==b'owned previous disabled cache'
 print('PASS: upgrades preserve legacy source archives in Git and move only positively identified old WS build ZIPs to disabled archives, retain bytes/mtime/collisions, and reduce modeled Overstrike duplicates3 to1')

with fixture() as f:
 ws=migrate(f);old=(ws/'MSM2-WS-v0.1c20.script').read_bytes()
 for folder,name in [(ws,'user.zip'),(ws/'logs','user.zip'),(ws/'_source','user.zip'),(ws/'_archive','user.7z'),(ws/'_archive/_build','MSM2-WS-v0.1c20.zip'),(ws/'logs','user.RAR')]:
  p=folder/name;p.write_bytes(b'owned foreign/unverified archive; must remain')
  expect_failure(lambda:f[5].update(ws))
  assert p.read_bytes()==b'owned foreign/unverified archive; must remain'
  assert (ws/'MSM2-WS-v0.1c20.script').read_bytes()==old;p.unlink()
 with zipfile.ZipFile(ws/'MSM2-WS-v0.1c20.script') as z:dll_hash=hashlib.sha256(z.read('WallSprint.dll')).hexdigest()
 bad={'version':'v0.1c20','package_version':'0.1.38','sha256':'0'*64,'dll_sha256':dll_hash}
 inconsistent=bundles(ws/'_archive/_build','v0.1c20','0.1.38',ws/'MSM2-WS-v0.1c20.script',bad,disabled=False)
 before={p:p.read_bytes() for p in inconsistent};expect_failure(lambda:f[5].update(ws))
 assert all(p.read_bytes()==data for p,data in before.items()) and (ws/'MSM2-WS-v0.1c20.script').read_bytes()==old
 for p in inconsistent:p.unlink()
 print('PASS: raw/untracked/foreign/misplaced/corrupt ZIP-RAR-7z archives refuse without cleanup, movement of unrelated files, loss of old active build or sibling mutations')

with fixture() as f:
 ws=migrate(f);source=ws/'_source';old=(ws/'MSM2-WS-v0.1c20.script').read_bytes()
 cmd('git','-C',source,'sparse-checkout','set','--no-cone','/*')
 config=(source/'.git/info/sparse-checkout').read_bytes();expect_failure(lambda:f[5].update(ws))
 assert (source/'.git/info/sparse-checkout').read_bytes()==config and (ws/'MSM2-WS-v0.1c20.script').read_bytes()==old
 release=source/'source/tools/release.py';shutil.copy2(TOOLS/'release.py',release)
 p=subprocess.run(['python',str(release)],capture_output=True,text=True)
 assert p.returncode!=0 and 'Release ZIPs must be built outside Mods Library' in p.stderr
 assert not w.live_archives(ws)
 old_zip=source/'owned-old-checkout.zip';old_zip.write_bytes(b'owned old-bootstrap fixture')
 p=subprocess.run(['bash',str(TOOLS/'windows/workspace-build.sh'),str(source),str(ws/'_archive/_build')],capture_output=True,text=True)
 assert p.returncode!=0 and 'update the preparation checkout' in p.stderr and old_zip.read_bytes()==b'owned old-bootstrap fixture'
 old_zip.unlink()
 print('PASS: custom sparse configuration is preserved/refused; direct release inside the scanned library refuses before producing ZIPs and never bypasses the mandatory full pipeline')

with fixture() as f:
 ws=migrate(f);build=ws/'_archive/_build'
 inner=(build/'MSM2-WS-v0.1c20.zip.disabled').read_bytes();outer=build/'owned-nested.zip'
 with zipfile.ZipFile(outer,'w') as z:z.writestr('folder/inner.ZIP',inner)
 assert len(detected_wall_sprint(ws))==2
 outer.rename(build/'owned-nested.zip.disabled')
 assert len(detected_wall_sprint(ws))==1
 old=(ws/'MSM2-WS-v0.1c20.script').read_bytes();base=f[4].build
 def bad_build(source,dest):
  p,m=base(source,dest)
  (dest/'MSM2-WS-v0.1c20.zip').write_bytes((dest/'MSM2-WS-v0.1c20.zip.disabled').read_bytes())
  return p,m
 f[4].build=bad_build;expect_failure(lambda:f[5].update(ws))
 assert (ws/'MSM2-WS-v0.1c20.script').read_bytes()==old
 print('PASS: independent extension/recursive-ZIP model detects unextracted nested scripts, ignores disabled containers, and a build that reintroduces a visible ZIP cannot deploy or launch')

# Owned, non-game synthetic probe assets. Production SHA remains pinned; this
# fixture substitutes only its local expected identity, never package builders.
def probe_fixture_bytes(stem,outer=False):
 r=w.retired_probes;name,offset,value,original=r.PROBES[stem]
 vanilla=bytearray(544);vanilla[0x218]=100;vanilla[0x219]=75
 r.VANILLA_SHA=hashlib.sha256(vanilla).hexdigest();vanilla[offset]=value
 b=io.BytesIO()
 with zipfile.ZipFile(b,'w') as z:
  z.writestr(r.ASSET,vanilla);z.writestr('info.json',json.dumps({'game':'MSM2','name':name,'author':'deluxxxe-dev / WallSprint','format_version':2}))
 if not outer:return b.getvalue()
 out=io.BytesIO()
 with zipfile.ZipFile(out,'w') as z:z.writestr(stem+'.stage',b.getvalue())
 return out.getvalue()
original_probe_sha=w.retired_probes.VANILLA_SHA
assert original_probe_sha=='32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b'
try:
 with fixture() as f:
  ws=migrate(f);game=f[1];source=ws/'_source';stem='MSM2-WS-SteeringProbe-TurnSpeedSlow5'
  profile=game/'Overstrike/Profile.json';profile.write_text('{"mods":[{"name":"other","enabled":false,"order":7}]}');before=profile.read_bytes()
  p=source/(stem+'.stage');p.write_bytes(probe_fixture_bytes(stem));q=ws/'_archive'/(stem+'.zip');q.write_bytes(probe_fixture_bytes(stem,True))
  sibling=game/'Mods Library/Manual Probe Copies'/(stem+'.stage');sibling.parent.mkdir();sibling.write_bytes(p.read_bytes())
  old={x:x.read_bytes() for x in [p,q,sibling]}
  f[5].update(ws);assert not p.exists() and not q.exists() and sibling.read_bytes()==old[sibling]
  backups=list((game/'MSM2-WS-Retired-Probes').glob('retired-*'));assert len(backups)==1
  records=json.loads((backups[0]/'manifest.json').read_text());assert len(records)==2
  for row in records:assert (backups[0]/row['backup']).read_bytes()==old[ws/row['source']]
  f[5].update(ws);assert len(list((game/'MSM2-WS-Retired-Probes').glob('retired-*')))==1
  assert profile.read_bytes()==before and not list(ws.rglob('*.stage')) and not w.live_archives(ws)
  # Reintroduced copy cannot overwrite previous backup.
  p.write_bytes(old[p]);f[5].update(ws);assert len(list((game/'MSM2-WS-Retired-Probes').glob('retired-*')))==2
  print('PASS: verified duplicate probes retire reversibly outside library; Unicode paths, collision preservation, repeated update idempotence and sibling/profile byte isolation')
 with fixture() as f:
  ws=migrate(f);stem='MSM2-WS-SteeringProbe-TurnSpeedFast10';p=ws/(stem+'.stage');p.write_bytes(probe_fixture_bytes(stem))
  # Same name and valid ZIP with wrong payload does not confer ownership.
  with zipfile.ZipFile(p,'a') as z:z.writestr('foreign.bin',b'preserve')
  old=p.read_bytes();expect_failure(lambda:f[5].update(ws));assert p.read_bytes()==old
  p.unlink();u=ws/'user.stage';u.write_bytes(b'foreign stage');expect_failure(lambda:f[5].update(ws));assert u.read_bytes()==b'foreign stage'
  print('PASS: forged known name/metadata and unrelated stage refuse without deleting, moving or enabling files')
 with fixture() as f:
  ws=migrate(f);source=ws/'_source';stem='MSM2-WS-SteeringProbe-TurnSpeedSlow5';p=source/(stem+'.stage');p.write_bytes(probe_fixture_bytes(stem))
  (source/'README.md').write_text('dirty user work');expect_failure(lambda:f[5].update(ws));assert p.exists()
  cmd('git','-C',source,'checkout','--','README.md');p.unlink();target=ws/'probe-target';target.write_bytes(probe_fixture_bytes(stem));p.symlink_to(target)
  expect_failure(lambda:f[5].update(ws));assert p.is_symlink() and target.exists()
  print('PASS: dirty tracked work and reparse candidates refuse before any retirement')
 with fixture() as f:
  repo=f[2];stem='MSM2-WS-SteeringProbe-TurnSpeedSlow5';p=repo/'source/tools'/(stem+'.stage');p.write_bytes(probe_fixture_bytes(stem))
  cmd('git','-C',repo,'add','.');cmd('git','-C',repo,'commit','-m','owned tracked stage history');cmd('git','-C',repo,'push','origin',BRANCH)
  ws=migrate(f);source=ws/'_source';assert not list(source.rglob('*.stage'))
  assert cmd('git','-C',source,'show','HEAD:source/tools/'+p.name)
  cmd('git','-C',source,'sparse-checkout','set','--no-cone',*w.LEGACY_SOURCE_PATTERNS);assert (source/'source/tools'/p.name).exists()
  f[5].update(ws);assert not list(source.rglob('*.stage')) and not cmd('git','-C',source,'status','--porcelain')
  print('PASS: legacy managed sparse policy upgrades without losing tracked stage history; next update never restores manually removed probes')
 with fixture() as f:
  ws=migrate(f);old=(ws/'MSM2-WS-v0.1c20.script').read_bytes();base=f[4].build
  def stage_build(source,dest):
   result=base(source,dest);(dest/'unrelated.stage').write_bytes(b'foreign stage');return result
  f[4].build=stage_build;expect_failure(lambda:f[5].update(ws));assert (ws/'MSM2-WS-v0.1c20.script').read_bytes()==old
  print('PASS: unexpected stage generated by build refuses deployment rather than silently enabling a new mod')
finally:w.retired_probes.VANILLA_SHA=original_probe_sha
