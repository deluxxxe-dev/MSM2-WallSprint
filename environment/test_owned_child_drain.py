"""Actual owned Linux pidfd drain: no name whitelist, sleeps, or termination."""
import os,select,subprocess,sys,unittest
from unittest.mock import patch
import operation_lock as o
from test_wsl_process_probe import Proc
class Drain(unittest.TestCase):
 def children(self):
  self.tag='owned-drain-token';self.child=subprocess.Popen([sys.executable,'-c','import sys;sys.stdin.buffer.read()'],stdin=subprocess.PIPE,env={**os.environ,'MSM2_WS_OPERATION_TOKEN':self.tag})
  self.unrelated=subprocess.Popen([sys.executable,'-c','import sys;sys.stdin.buffer.read()'],stdin=subprocess.PIPE,env={k:v for k,v in os.environ.items() if k!='MSM2_WS_OPERATION_TOKEN'})
  for p in [self.child,self.unrelated]:self.addCleanup(lambda p=p:(p.stdin.close() if not p.stdin.closed else None,p.wait(timeout=5)))
 def probe(self,release,wait_ms):
  ns={'__name__':'fixture'};exec(o.WSL_PROBE,ns);real=select.poll;child=self.child
  class Poll:
   def __init__(self):self.q=real()
   def register(self,*a):return self.q.register(*a)
   def unregister(self,*a):return self.q.unregister(*a)
   def poll(self,t):
    if release and t>0 and not child.stdin.closed:child.stdin.close()
    return self.q.poll(t)
  with patch.dict(os.environ,{'WSL_DISTRO_NAME':'Owned'}),patch('os.listdir',return_value=[str(self.child.pid),str(self.unrelated.pid)]),patch('select.poll',Poll):return ns['probe'](self.tag,False,'Owned',os.getuid(),wait_ms=wait_ms)
 def test_owned_service_natural_exit_waits_and_unrelated_is_untouched(self):
  self.children();r=self.probe(True,1000);self.assertEqual(r['result'],'QUIET');self.assertEqual(r['counts']['owned_exits_waited'],1);self.assertIsNone(self.unrelated.poll())
 def test_live_child_deadline_keeps_active_and_identity_metadata(self):
  self.children();r=self.probe(False,20);self.assertEqual(r['result'],'ACTIVE');self.assertIn('owned_exit_deadline',r['counts']);e=next(e for e in r['events'] if e['category']=='active_build');self.assertEqual(e['pid'],self.child.pid);self.assertEqual(e['ppid'],os.getpid());self.assertTrue(e['start'].isdigit());self.assertTrue(e['executable_name']);self.assertIsNone(self.child.poll());self.assertIsNone(self.unrelated.poll())
 def test_stale_pidfd_cannot_wait_for_live_reused_pid(self):
  p=Proc();p.exited=True;p.live_after=True;p.env=b'MSM2_WS_OPERATION_TOKEN=token\0';r=p.run(wait_ms=20);self.assertNotEqual(r['result'],'QUIET');self.assertIn('pid_reused',r['counts'])
 def test_diagnostic_active_inspects_root_without_claim_or_wait(self):
  calls=[]
  def query(record,legacy,root,wait_ms=0):calls.append((root,wait_ms));return {'result':'ACTIVE','events':[{'pid':42,'executable_name':'owned-service'}]}
  with patch.object(o,'_wsl_query',side_effect=query):r=o.wsl_inspect({'build_token':'token','token':'token'})
  self.assertEqual(r['authority'],'diagnostic_only_no_recovery');self.assertEqual(calls,[(False,0),(True,0)]);self.assertEqual(r['root']['result'],'ACTIVE')
 def test_failed_completion_with_token_is_persistently_interrupted(self):
  import tempfile,json
  from pathlib import Path
  with tempfile.TemporaryDirectory() as d:
   game=Path(d)
   with o.hold(game,'update') as h:o.phase('build_failed',build_token=h.record['token'],build_returncode=37,distro='Owned')
   r=json.loads((game/'.MSM2-Operations.guard').read_text());self.assertEqual(r['state'],'interrupted');self.assertEqual(r['build_token'],r['token']);self.assertEqual(r['build_returncode'],37)
 def test_active_events_not_hidden_by_departed_process_noise(self):
  import errno,io,types
  ns={'__name__':'fixture'};exec(o.WSL_PROBE,ns)
  def file(path,mode='r'):
   pid=int(path.split('/')[2]);field=path.rsplit('/',1)[-1]
   if field=='stat':return io.StringIO(str(pid)+' (owned) '+('Z' if pid<150 else 'S')+' '+' '.join(['0']*18+['1234']))
   if field=='environ':
    if pid<150:raise PermissionError(errno.EACCES,'owned')
    return io.BytesIO(b'MSM2_WS_OPERATION_TOKEN=token\0')
   if field=='comm':return io.StringIO('owned-service')
   raise AssertionError(path)
  class Poll:
   def register(self,fd,*a):self.fd=fd
   def poll(self,t):return [(self.fd,select.POLLIN)] if self.fd<150 else []
  with patch.dict(os.environ,{'WSL_DISTRO_NAME':'Owned'}),patch('os.listdir',return_value=[str(n) for n in range(100,150)]+['444']),patch('os.stat',return_value=types.SimpleNamespace(st_uid=os.getuid())),patch('builtins.open',side_effect=file),patch('os.pidfd_open',side_effect=lambda pid,flags:pid),patch('os.close'),patch('select.poll',Poll),patch('os.readlink',return_value='/owned/service'):
   r=ns['probe']('token',False,'Owned',os.getuid())
  self.assertEqual(r['counts']['exited_confirmed'],50);self.assertTrue(any(e['category']=='active_build' and e['pid']==444 for e in r['events']));self.assertLessEqual(len(r['events']),32)
if __name__=='__main__':unittest.main()
