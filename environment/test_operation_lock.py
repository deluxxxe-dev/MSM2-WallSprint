"""Owned cross-process lock/recovery fixtures; Linux backend is not Windows proof."""
from pathlib import Path
import json,os,subprocess,sys,tempfile,time,unittest
import operation_lock as o
class Locks(unittest.TestCase):
 def setUp(self):
  self.temp=tempfile.TemporaryDirectory(prefix="MSM2 锁 ' owned ");self.addCleanup(self.temp.cleanup);self.game=Path(self.temp.name)
  (self.game/'Mods Library/Mods-Developing/MSM2-WS/_archive').mkdir(parents=True)
 def record(self):return json.loads((self.game/'.MSM2-Operations.guard').read_text())
 def script(self,body):
  code="import sys,os,subprocess,time;sys.path.insert(0,"+repr(str(Path(o.__file__).parent))+ ");import operation_lock as o;from pathlib import Path;g=Path(sys.argv[1]);"+body
  p=subprocess.Popen([sys.executable,'-c',code,str(self.game)],stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
  self.addCleanup(p.stdout.close);self.addCleanup(p.stderr.close)
  return p
 def test_concurrent_claim_and_release(self):
  with o.hold(self.game):
   p=self.script("\nwith o.hold(g):print('WRONG')")
   out,err=p.communicate(timeout=5);self.assertNotEqual(p.returncode,0);self.assertNotIn('WRONG',out);self.assertIn('busy',err)
  with o.hold(self.game):pass
  self.assertEqual(self.record()['state'],'released')
 def test_crash_auto_recovers_os_released_owned_record(self):
  p=self.script("\nwith o.hold(g):\n print('READY',flush=True);os._exit(21)")
  self.assertEqual(p.stdout.readline().strip(),'READY');p.wait(timeout=5)
  old=self.record()['token']
  with o.hold(self.game):self.assertNotEqual(o._current.record['token'],old)
  self.assertTrue(list((self.game/'.MSM2-lock-recovery').rglob('.MSM2-Operations.guard')))
 def test_inherited_child_retains_ownership_after_parent_crash(self):
  p=self.script("\nwith o.hold(g):\n child=subprocess.Popen([sys.executable,'-c','import time;time.sleep(1.2)'],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL,**o.child_options());print('READY',flush=True);os._exit(21)")
  self.assertEqual(p.stdout.readline().strip(),'READY');p.wait(timeout=5)
  with self.assertRaises(o.OperationError):
   with o.hold(self.game):pass
  time.sleep(1.4)
  with o.hold(self.game):pass
 def test_empty_legacy_requires_inactivity_evidence_and_preserves_copy(self):
  legacy=self.game/'Mods Library/Mods-Developing/MSM2-WS/_archive/Update-WS.lock';legacy.touch()
  with self.assertRaises(o.OperationError):
   with o.hold(self.game):pass
  self.assertEqual(legacy.read_bytes(),b'')
  checks=[]
  with o.hold(self.game,quiescent=lambda g,r: checks.append(g)):pass
  self.assertTrue(checks);copies=list((self.game/'.MSM2-lock-recovery').rglob('Update-WS.lock'));self.assertEqual(len(copies),1);self.assertEqual(copies[0].read_bytes(),b'')
 def test_malformed_preserved(self):
  path=self.game/'.MSM2-Mod-Tools.lock';path.write_text('not an owned operation')
  with self.assertRaises(o.OperationError):
   with o.hold(self.game):pass
  self.assertEqual(path.read_text(),'not an owned operation')
 def test_pid_reuse_uses_start_identity_not_pid_alone(self):
  with o.hold(self.game):pass
  path=self.game/'.MSM2-Operations.guard';r=self.record();r.update(state='active',start='different-start',pid=os.getpid());path.write_text(json.dumps(r))
  with o.hold(self.game):pass
 def test_live_matching_owner_is_not_reclaimed(self):
  with o.hold(self.game):pass
  path=self.game/'.MSM2-Operations.guard';r=self.record();r['state']='active';path.write_text(json.dumps(r));old=path.read_bytes()
  with self.assertRaisesRegex(o.OperationError,'Active operation metadata owner'):
   with o.hold(self.game):pass
  self.assertEqual(path.read_bytes(),old)
 def test_unresolved_build_blocks_even_after_parent_death(self):
  p=self.script("\nwith o.hold(g):\n o.phase('building',build_token=o._current.record['token'],distro='not-real');print('READY',flush=True);os._exit(21)")
  p.stdout.readline();p.wait(timeout=5)
  with self.assertRaisesRegex(o.OperationError,'WSL probe unavailable'):
   with o.hold(self.game):pass
  with o.hold(self.game,quiescent=lambda g,r:None):pass # Injected QUIET evidence only.
 def test_reparse_refuses(self):
  path=self.game/'.MSM2-Operations.guard';other=self.game/'untouched';other.write_text('unchanged');path.symlink_to(other)
  with self.assertRaises(o.OperationError):
   with o.hold(self.game):pass
  self.assertEqual(other.read_text(),'unchanged')
 def test_replacement_race_never_releases_foreign_file(self):
  if os.name=='nt':self.skipTest('Windows share denial tested by Win32 fixture')
  path=self.game/'.MSM2-Mod-Tools.lock'
  with self.assertRaisesRegex(o.OperationError,'replaced'):
   with o.hold(self.game):
    path.rename(self.game/'previous');path.write_text('foreign replacement')
  self.assertEqual(path.read_text(),'foreign replacement')
 def test_wrong_workspace_metadata_refuses(self):
  with o.hold(self.game):pass
  path=self.game/'.MSM2-Mod-Tools.lock';r=json.loads(path.read_text());r['workspace']='elsewhere';path.write_text(json.dumps(r));old=path.read_bytes()
  with self.assertRaises(o.OperationError):
   with o.hold(self.game):pass
  self.assertEqual(path.read_bytes(),old)
 def test_legacy_creation_race_is_not_mistaken_for_our_new_file(self):
  from unittest.mock import patch
  path=self.game/'Mods Library/Mods-Developing/MSM2-WS/_archive/Update-WS.lock';original=o.open_gate
  def racing(p):
   if p==path and not p.exists():p.touch()
   return original(p)
  with patch.object(o,'open_gate',racing):
   with self.assertRaises(o.OperationError):
    with o.hold(self.game,'update'):pass
  self.assertEqual(path.read_bytes(),b'')
 def test_legacy_checks_all_running_distros_not_default(self):
  from unittest.mock import patch
  seen=[]
  def run(args,**kw):
   if '--list' in args:return subprocess.CompletedProcess(args,0,'First\nSecond\n'.encode('utf-16-le'),b'')
   seen.append(args[args.index('--distribution')+1]);return subprocess.CompletedProcess(args,0,json.dumps(dict(schema=1,result='QUIET',distro_expected=seen[-1],distro_observed=seen[-1],uid_expected=None,uid_observed=os.getuid(),inspection='user',events=[],counts={})), '')
  with patch.object(o.subprocess,'run',run):o.wsl_quiet({},legacy=True)
  self.assertEqual(seen,['First','Second'])
 def test_exact_wsl_token_probe_runs_on_owned_linux_process(self):
  from unittest.mock import patch
  real_run=subprocess.run;token='owned-wsl-token-'+str(os.getpid())
  p=subprocess.Popen([sys.executable,'-c','import time;time.sleep(5)'],env={**os.environ,'MSM2_WS_OPERATION_TOKEN':token})
  self.addCleanup(lambda:p.poll() is None and p.kill())
  def run(args,**kw):
   at=args.index('-c');return real_run([sys.executable,*args[at:]],capture_output=True,text=True,env={**os.environ,'WSL_DISTRO_NAME':'owned'})
  with patch.object(o.subprocess,'run',run):
   try:o.wsl_quiet({'build_token':token,'distro':'owned'})
   except o.OperationError as ex:
    if 'access_denied' in str(ex):
     p.terminate();p.wait(timeout=5);self.skipTest('Full /proc environment inspection unavailable in sandbox; production probe failed closed')
    self.assertIn('active_build',str(ex))
   else:self.fail('Live token not detected')
   p.terminate();p.wait(timeout=5)
   o.wsl_quiet({'build_token':token,'distro':'owned'})
 def test_token_probe_with_isolated_owned_proc_inventory(self):
  from unittest.mock import patch
  real_run=subprocess.run;token='owned-isolated-'+str(os.getpid())
  p=subprocess.Popen([sys.executable,'-c','import time;time.sleep(5)'],env={**os.environ,'MSM2_WS_OPERATION_TOKEN':token})
  def run(args,**kw):
   at=args.index('-c');script=args[at+1]
   prefix="import os;_ls=os.listdir;os.listdir=lambda p: ["+repr(str(p.pid))+"] if p=='/proc' else _ls(p);\n"
   return real_run([sys.executable,'-c',prefix+script,*args[at+2:]],capture_output=True,text=True,env={**os.environ,'WSL_DISTRO_NAME':'owned'})
  try:
   with patch.object(o.subprocess,'run',run):
    with self.assertRaisesRegex(o.OperationError,'active_build'):o.wsl_quiet({'build_token':token,'distro':'owned'})
    p.terminate();p.wait(timeout=5);record={'build_token':token,'distro':'owned'}
    self.assertEqual(o._wsl_query(record,False,False)['result'],'QUIET')
    if os.getuid()==0:o.wsl_quiet(record)
    else:
     with self.assertRaisesRegex(o.OperationError,'inspection=root'):o.wsl_quiet(record)
  finally:
   if p.poll() is None:p.kill();p.wait()
if __name__=='__main__':unittest.main()
