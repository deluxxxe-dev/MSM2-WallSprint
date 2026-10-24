"""Owned Windows cmd/copy/launch fixture, never the user's C: drive or real Overstrike."""
from pathlib import Path
import os,subprocess,tempfile,shutil,time,re
root=Path(__file__).resolve().parents[1]
base=Path(tempfile.mkdtemp(prefix='wallsprint-helper-',dir='/tmp'))
box=base/'Downloads ! & spaces';box.mkdir()
game=base/'Game Root ! & spaces';game.mkdir()
(game/'Spider-Man2.exe').write_bytes(b'owned fixture game marker, not an executable')
app=base/'AppData'/'Separated';app.mkdir(parents=True)
shutil.copy2(root/'build/windows/update_for_test_host.exe',app/'Overstrike.exe')
shutil.copy2(root/'build/windows/update_helper_dependency.dll',game/'scripts_proxy.dll')
helper=box/'UpdateForTest.cmd';shutil.copy2(root/'UpdateForTest.cmd',helper)
label=re.search(r'versionLabel\[\]="([^"]+)"',(root/'include/version.hpp').read_text())[1]
package=box/('MSM2-WS-'+label+'.zip');package.write_bytes(b'owned WS install archive payload 1')
lib=game/'Mods Library';lib.mkdir()
(lib/'MSM2-SF-sentinel.zip').write_bytes(b'SF root must stay identical')
(lib/'unrelated.script').write_bytes(b'other mod untouched')
def win(p):return 'Z:'+str(p).replace('/','\\')
env=os.environ.copy();env.update({'WINEPREFIX':'/workspace/toolchains/wine-prefix','WINEDLLPATH':'/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine','WINEDEBUG':'-all','WINEDLLOVERRIDES':'mscoree,mshtml=','MSM2_WS_GAME_ROOT':win(game),'MSM2_WS_OVERSTRIKE_EXE':win(app/'Overstrike.exe'),'MSM2_WS_NO_PAUSE':'1'})
wine='/workspace/toolchains/wine/usr/lib/wine/wine64'
seq=0
def run(overrides=None,ok=True):
 global seq
 seq+=1;report=base/f'report{seq}.txt';current=env.copy();current['MSM2_WS_TEST_REPORT']=win(report)
 for key,value in (overrides or {}).items():
  if value is None:current.pop(key,None)
  else:current[key]=value
 proc=subprocess.run([wine,'cmd','/d','/c','UpdateForTest.cmd'],cwd=box,env=current,capture_output=True,text=True,errors='replace',timeout=30)
 if ok:
  assert proc.returncode==0,(proc.returncode,proc.stdout,proc.stderr)
  deadline=time.monotonic()+8
  while not report.exists() and time.monotonic()<deadline:time.sleep(.05)
  assert report.read_bytes()==b'GAME_ROOT_CWD_AND_DEPENDENCY_PASS',(proc.stdout,proc.stderr)
 else:
  assert proc.returncode!=0 and '[WS ERROR]' in proc.stdout,(proc.returncode,proc.stdout,proc.stderr)
  assert not report.exists()
 return proc
run({'MSM2_WS_OVERSTRIKE_EXE':None},False)
run({'MSM2_WS_OVERSTRIKE_EXE':'relative\\Overstrike.exe'},False)
run({'MSM2_WS_OVERSTRIKE_EXE':win(app/'missing.exe')},False)
run({'MSM2_WS_GAME_ROOT':win(base/'not a game')},False)
exe_dir=base/'invalid'/'Overstrike.exe';exe_dir.mkdir(parents=True)
run({'MSM2_WS_OVERSTRIKE_EXE':win(exe_dir)},False)
assert not (lib/'Mods-Developing').exists()
print('PASS: c20 helper rejects missing/relative/unavailable separated Overstrike and invalid game root before copying, without guessed AppData or library fallback')
run()
dev=lib/'Mods-Developing';dest=dev/package.name
assert dest.read_bytes()==package.read_bytes() and not (lib/package.name).exists()
assert (lib/'MSM2-SF-sentinel.zip').read_bytes()==b'SF root must stay identical'
assert (lib/'unrelated.script').read_bytes()==b'other mod untouched'
print('PASS: c20 actual Windows cmd creates Mods-Developing, copies current WS ZIP, and launches owned separated host with game-root CWD/dependency resolution; spaces/!/ampersand paths preserved')
(dev/'MSM2-SF-keep.zip').write_bytes(b'SF developing untouched')
(dev/'MSM2-WS-v0.1c19.zip').write_bytes(b'older WS retained')
package.write_bytes(b'owned WS install archive payload 2')
run();assert dest.read_bytes()==package.read_bytes()
run({'MSM2_WS_PACKAGE':win(dest)})
assert (dev/'MSM2-SF-keep.zip').read_bytes()==b'SF developing untouched'
assert (dev/'MSM2-WS-v0.1c19.zip').read_bytes()==b'older WS retained'
print('PASS: c20 helper only replaces matching current WS artifact, supports idempotent library source, and preserves SF/old WS/unrelated files without cleanup globs')
sf=box/'MSM2-SF-refuse.zip';sf.write_bytes(b'SF input unchanged')
run({'MSM2_WS_PACKAGE':win(sf)},False)
assert sf.read_bytes()==b'SF input unchanged'
blocked=base/'Blocked Game';blocked.mkdir();(blocked/'Spider-Man2.exe').write_bytes(b'marker');(blocked/'Mods Library').mkdir();(blocked/'Mods Library'/'Mods-Developing').write_bytes(b'not a directory')
run({'MSM2_WS_GAME_ROOT':win(blocked)},False)
assert (blocked/'Mods Library'/'Mods-Developing').read_bytes()==b'not a directory'
print('PASS: c20 wrong SF/source filename and blocked library fail clearly, without modifying SF or falling back to another Mods Library/WorkingDirectory')
