"""Owned /proc model for races, plus real token/pidfd integration (no real WSL claim)."""
import errno,json,os,select,subprocess,sys,types,unittest
from unittest.mock import patch
import operation_lock as o

class Proc:
 def __init__(self):self.uid=os.getuid();self.pid=444;self.selfpid=999;self.before='1234';self.after=self.before;self.error=None;self.exited=False;self.absent=False;self.env=b'OTHER=private-secret\0';self.reads=[];self.closed=[];self.cwd_error=False;self.live_after=False
 def stat(self,path):
  if self.absent:raise FileNotFoundError(errno.ENOENT,'owned')
  return types.SimpleNamespace(st_uid=self.uid)
 def start_text(self,value):return '444 (owned process) '+('Z' if self.exited and not self.live_after and self.reads.count('stat')>1 else 'S')+' '+' '.join(['0']*18+[value])
 def open(self,path,mode='r'):
  import io
  field=path.rsplit('/',1)[-1];self.reads.append(field)
  if self.error==field:raise PermissionError(errno.EACCES,'owned secret not for output')
  if self.absent:raise FileNotFoundError(errno.ENOENT,'owned')
  if field=='stat':return io.StringIO(self.start_text(self.before if self.reads.count('stat')==1 else self.after))
  if field=='environ':return io.BytesIO(self.env)
  if field=='cmdline':return io.BytesIO(b'unrelated private command')
  if field=='comm':return io.StringIO('owned-fixture')
  raise AssertionError(field)
 def kill(self,pid,signal):
  assert signal==0
  if self.absent:raise ProcessLookupError(errno.ESRCH,'owned')
 def readlink(self,path):
  if self.cwd_error:raise PermissionError(errno.EACCES,'owned')
  return '/unrelated'
 def run(self,token='token',legacy=False,distro='Owned',expected_uid=None,elevated=False,wait_ms=0):
  ns={'__name__':'owned_probe'};exec(o.WSL_PROBE,ns)
  me=self
  class Poll:
   def register(self,*a):pass
   def poll(self,t):return [(42,select.POLLIN)] if me.exited else []
  with patch.dict(os.environ,{'WSL_DISTRO_NAME':'Owned'}),patch('os.listdir',return_value=[str(self.pid),str(self.selfpid)]),patch('os.getpid',return_value=self.selfpid),patch('os.stat',side_effect=self.stat),patch('builtins.open',side_effect=self.open),patch('os.pidfd_open',return_value=42),patch('os.close',side_effect=self.closed.append),patch('os.kill',side_effect=self.kill),patch('select.poll',Poll),patch('os.readlink',side_effect=self.readlink):
   return ns['probe'](token,legacy,distro,expected_uid,elevated,wait_ms=wait_ms)
class ProbeTests(unittest.TestCase):
 def test_unrelated_reads_only_environment_not_cwd_or_cmd(self):
  p=Proc();r=p.run();self.assertEqual(r['result'],'QUIET');self.assertNotIn('cmdline',p.reads);self.assertEqual(p.closed,[42]);self.assertNotIn('private',json.dumps(r))
 def test_permission_live_is_unknown_stage_errno(self):
  p=Proc();p.error='environ';r=p.run();self.assertEqual(r['result'],'UNKNOWN');e=r['events'][0];self.assertEqual((e['pid'],e['stage'],e['category'],e['errno']),(444,'environ','access_denied',errno.EACCES));self.assertNotIn('secret',json.dumps(r))
 def test_exits_during_read_permission_error_pidfd_confirms_exit(self):
  p=Proc();p.error='environ';p.exited=True;r=p.run();self.assertEqual(r['result'],'QUIET');self.assertEqual(r['events'][0]['category'],'exited_confirmed')
 def test_pid_absent_between_enumeration_and_read(self):
  p=Proc();p.absent=True;r=p.run();self.assertEqual(r['result'],'QUIET');self.assertEqual(r['events'][0]['category'],'absent_confirmed')
 def test_reused_pid_cannot_be_excused_by_old_pidfd_exit(self):
  p=Proc();p.error='environ';p.exited=True;p.after='9999';r=p.run();self.assertEqual(r['result'],'UNKNOWN');self.assertEqual(r['events'][0]['category'],'pid_reused')
 def test_exited_handle_cannot_excuse_live_reused_pid_even_same_start_tick(self):
  p=Proc();p.error='environ';p.exited=True;p.live_after=True;r=p.run();self.assertEqual(r['result'],'UNKNOWN');self.assertEqual(r['events'][0]['category'],'pid_reused')
 def test_readable_reuse_unknown(self):
  p=Proc();p.after='9999';self.assertEqual(p.run()['result'],'UNKNOWN')
 def test_live_exact_token_blocks(self):
  p=Proc();p.env=b'MSM2_WS_OPERATION_TOKEN=token\0';r=p.run();self.assertEqual(r['result'],'ACTIVE');self.assertEqual(r['events'][0]['category'],'active_build')
 def test_similar_token_unrelated(self):
  p=Proc();p.env=b'MSM2_WS_OPERATION_TOKEN=token-other\0';self.assertEqual(p.run()['result'],'QUIET')
 def test_self_probe_not_inspected(self):
  p=Proc();p.pid=p.selfpid;r=p.run();self.assertEqual(r['result'],'QUIET');self.assertEqual(p.reads,[])
 def test_distro_mismatch(self):self.assertEqual(Proc().run(distro='Different')['events'][0]['category'],'distro_mismatch')
 def test_uid_mismatch(self):self.assertEqual(Proc().run(expected_uid=os.getuid()+1)['events'][0]['category'],'uid_mismatch')
 def test_other_uid_not_read(self):
  p=Proc();p.uid+=1;self.assertEqual(p.run()['result'],'QUIET');self.assertEqual(p.reads,[])
 def test_malformed_identity_unknown(self):
  p=Proc();p.before='invalid';self.assertEqual(p.run()['result'],'UNKNOWN')
 def test_legacy_cwd_permission_remains_unknown(self):
  p=Proc();p.cwd_error=True;r=p.run(legacy=True)
  self.assertEqual(r['result'],'UNKNOWN');self.assertEqual(r['events'][0]['stage'],'cwd')
 def test_token_nonmatch_does_not_need_inaccessible_cwd(self):
  p=Proc();p.cwd_error=True;self.assertEqual(p.run()['result'],'QUIET')
 def test_exit_status_and_identity_binding_refuse(self):
  payload=Proc().run()
  for code,change in [(7,{}),(0,{'distro_observed':'Different'}),(0,{'uid_expected':-1})]:
   data={**payload,**change}
   with patch.object(o.subprocess,'run',return_value=subprocess.CompletedProcess([],code,json.dumps(data),'private stderr')):
    with self.assertRaises(o.OperationError):o.wsl_quiet({'build_token':'token','distro':'Owned'})
 def test_windows_caller_pins_uid_before_build_and_checks_after(self):
  from pathlib import Path
  sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'source/tools'))
  import workspace as w
  record={'token':'owned-token'};seen=[]
  def run(args):
   if args[-2:]==['uname','-r']:return '6.1-WSL2'
   if args[-2:]==['id','-u']:return '1000'
   if 'wslpath' in args:return '/owned-path'
   raise AssertionError(args)
  def phase(name,**extra):record.update(phase=name,**extra)
  def quiet(r):seen.append(dict(r));raise RuntimeError('owned-postcheck')
  with patch.dict(os.environ,{'MSM2_WS_WSL_DISTRO':'Owned'}),patch.object(w.shutil,'which',return_value='owned-wsl'),patch.object(w,'run',side_effect=run),patch.object(o,'_current',types.SimpleNamespace(record=record)),patch.object(o,'phase',side_effect=phase),patch.object(o,'child_options',return_value={}),patch.object(o,'wsl_quiet',side_effect=quiet),patch.object(w.subprocess,'run',return_value=subprocess.CompletedProcess([],0)):
   with self.assertRaisesRegex(RuntimeError,'owned-postcheck'):w.WindowsSystem().build(Path('/owned'),Path('/not-deployed'))
  self.assertEqual((seen[0]['linux_uid'],seen[0]['distro'],seen[0]['build_token'],seen[0]['phase']),(1000,'Owned','owned-token','build_returned'))
 def test_bound_quiet_output_reports_successful_exit(self):
  data=Proc().run()
  with patch.object(o.subprocess,'run',return_value=subprocess.CompletedProcess([],0,json.dumps(data),'')):
   self.assertEqual(o._wsl_query({'build_token':'token','distro':'Owned'},False,False)['exit_status'],0)
 def test_output_schema_incomplete_or_noise_refused(self):
  for output in ['QUIET\n','{}',json.dumps({'schema':1,'result':'QUIET','events':[]})]:
   with patch.object(o.subprocess,'run',return_value=subprocess.CompletedProcess([],0,output,'secret stderr')):
    with self.assertRaisesRegex(o.OperationError,'malformed'):o.wsl_quiet({'build_token':'token','distro':'Owned'})
 def test_real_child_token_and_pidfd(self):
  token='owned-test-token';p=subprocess.Popen([sys.executable,'-c','import time;time.sleep(10)'],env={**os.environ,'MSM2_WS_OPERATION_TOKEN':token})
  try:
   # Restrict only inventory to this owned child; use real proc files/pidfd/poll.
   prefix='import os;_list=os.listdir;os.listdir=lambda path: ['+repr(str(p.pid))+'] if path=="/proc" else _list(path);\n'
   env={**os.environ,'WSL_DISTRO_NAME':'Owned'}
   args=[sys.executable,'-c',prefix+o.WSL_PROBE,token,'0','Owned',str(os.getuid())]
   result=subprocess.run(args,capture_output=True,text=True,env=env);self.assertEqual(json.loads(result.stdout)['result'],'ACTIVE')
   p.terminate();p.wait(timeout=5);result=subprocess.run(args,capture_output=True,text=True,env=env);self.assertEqual(json.loads(result.stdout)['result'],'QUIET')
  finally:
   if p.poll() is None:p.kill();p.wait()
if __name__=='__main__':unittest.main()
