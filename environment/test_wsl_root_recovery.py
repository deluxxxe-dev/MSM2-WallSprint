"""Narrow read-only elevation and legacy token recovery; owned models, not WSL proof."""
import json,os,subprocess,sys,unittest
from unittest.mock import patch
import operation_lock as o
from test_wsl_process_probe import Proc
class Recovery(unittest.TestCase):
 def result(self,result='QUIET',root=False,uid=None,counts=None):
  return dict(schema=1,result=result,distro_expected='Owned',distro_observed='Owned',uid_expected=0 if root else uid,uid_observed=0 if root else 1000,inspection='root_token_all_uids' if root else 'user',events=[],counts=counts or {})
 def invoke(self,user,root=None,record=None):
  self.calls=[]
  def run(args,**kw):
   self.calls.append(args)
   privileged='--user' in args
   if privileged and isinstance(root,Exception):raise root
   data=root if privileged else user
   return subprocess.CompletedProcess(args,0 if data['result']=='QUIET' else 2,json.dumps(data),'')
  with patch.object(o.subprocess,'run',run):return o.wsl_quiet(record or {'distro':'Owned','build_token':'exact-owned-token','linux_uid':1000})
 def test_protected_unrelated_can_resolve_only_with_root_absence(self):
  user=self.result('UNKNOWN',uid=1000,counts={'access_denied':2});r=self.invoke(user,self.result(root=True));self.assertEqual(r['recovery_authority'],'all_uid_exact_token_absence')
  args=self.calls[-1];self.assertEqual(args[args.index('--user')+1],'root');self.assertEqual(args[args.index('--exec')+1],'python3');self.assertIn('--distribution',args);self.assertNotIn('bash',args)
 def test_unknown_protected_relevance_without_root_proof_stays_blocked(self):
  with self.assertRaisesRegex(o.OperationError,'NOT proved'):self.invoke(self.result('UNKNOWN',uid=1000,counts={'access_denied':2}),self.result('UNKNOWN',True,counts={'access_denied':1}))
 def test_protected_matching_root_token_refuses(self):
  with self.assertRaisesRegex(o.OperationError,'NOT proved'):self.invoke(self.result('UNKNOWN',uid=1000,counts={'access_denied':1}),self.result('ACTIVE',True,counts={'active_build':1}))
 def test_user_quiet_still_checks_other_uid_escaped_child(self):
  with self.assertRaisesRegex(o.OperationError,'NOT proved'):self.invoke(self.result(uid=1000),self.result('ACTIVE',True,counts={'active_build':1}))
 def test_legacy_null_uid_has_all_uid_token_authority_without_fabrication(self):
  r=self.invoke(self.result(),self.result(root=True),{'distro':'Owned','build_token':'legacy-exact-token'});self.assertIsNone(r['build_uid_expected']);self.assertEqual(r['user_uid_observed'],1000);self.assertEqual(r['uid_observed'],0)
 def test_live_user_build_never_attempts_root(self):
  with self.assertRaises(o.OperationError):self.invoke(self.result('ACTIVE',uid=1000,counts={'active_build':1}))
  self.assertEqual(len(self.calls),1)
 def test_wrong_user_distro_or_uid_not_washed_by_root(self):
  for key,value in [('distro_observed','Other'),('uid_observed',2000)]:
   data=self.result(uid=1000);data[key]=value
   with self.assertRaises(o.OperationError):self.invoke(data,self.result(root=True))
   self.assertEqual(len(self.calls),1)
 def test_reused_or_malformed_identity_not_washed_by_elevation(self):
  for category in ['pid_reused','malformed_identity','process_read_error']:
   with self.assertRaises(o.OperationError):self.invoke(self.result('UNKNOWN',uid=1000,counts={category:1}),self.result(root=True))
   self.assertEqual(len(self.calls),1)
 def test_root_wrong_distro_or_not_actually_root_refuses(self):
  for key,value in [('distro_observed','Other'),('uid_observed',1000)]:
   data=self.result(root=True);data[key]=value
   with self.assertRaises(o.OperationError):self.invoke(self.result(uid=1000),data)
 def test_root_unavailable_or_timeout_stays_specific_unknown(self):
  for ex in [FileNotFoundError(),subprocess.TimeoutExpired('owned',25)]:
   with self.assertRaisesRegex(o.OperationError,'inspection=root_token_all_uids'):self.invoke(self.result(uid=1000),ex)
 def test_unavailable_root_or_incomplete_protocol_nonzero_refuses_without_stderr_dump(self):
  for raw in ['', '{"schema":1,"result":"QUIET"}']:
   user=subprocess.CompletedProcess([],0,json.dumps(self.result(uid=1000)),'')
   failed=subprocess.CompletedProcess([],1,raw,'private root availability detail')
   with patch.object(o.subprocess,'run',side_effect=[user,failed]):
    with self.assertRaisesRegex(o.OperationError,'inspection=root') as ex:o.wsl_quiet({'distro':'Owned','build_token':'token','linux_uid':1000})
   self.assertNotIn('private',str(ex.exception))
 def test_missing_or_option_shaped_distro_never_invokes_process(self):
  for distro in [None,'','--user',12]:
   with patch.object(o.subprocess,'run') as run:
    with self.assertRaises(o.OperationError):o._wsl_query({'distro':distro,'build_token':'token'},False,True)
    run.assert_not_called()
 def test_malformed_uid_or_token_never_invokes_process(self):
  for values in [{'linux_uid':'1000'},{'linux_uid':True},{'build_token':{'bad':1}},{'build_token':'bad\0token'}]:
   with patch.object(o.subprocess,'run') as run:
    with self.assertRaises(o.OperationError):o._wsl_query({'distro':'Owned','build_token':'token',**values},False,False)
    run.assert_not_called()
 def test_operation_token_mismatch_cannot_be_recovered_by_root(self):
  with patch.object(o,'_wsl_query') as query:
   with self.assertRaisesRegex(o.OperationError,'mismatches recorded operation'):o.wsl_quiet({'distro':'Owned','token':'operation-A','build_token':'operation-B'})
   query.assert_not_called()
 def test_raw_root_legacy_or_missing_token_refused(self):
  for record,legacy in [({'distro':'Owned'},False),({'distro':'Owned','build_token':'token'},True)]:
   with self.assertRaises(o.OperationError):o._wsl_query(record,legacy,True)
 def test_real_probe_model_reads_cross_uid_environment(self):
  p=Proc();p.uid=os.getuid()+1;p.env=b'MSM2_WS_OPERATION_TOKEN=token\0'
  with patch('os.getuid',return_value=0),patch('os.geteuid',return_value=0):r=p.run(elevated=True,expected_uid=0)
  self.assertEqual(r['result'],'ACTIVE');self.assertEqual(r['inspection'],'root_token_all_uids')
 def test_protected_unrelated_model_stays_unknown_without_permission(self):
  p=Proc();p.error='environ'
  with patch('os.getuid',return_value=0),patch('os.geteuid',return_value=0):r=p.run(elevated=True,expected_uid=0)
  self.assertEqual(r['result'],'UNKNOWN')
 def test_completed_owned_model_root_absence_is_quiet(self):
  p=Proc()
  with patch('os.getuid',return_value=0),patch('os.geteuid',return_value=0):r=p.run(elevated=True,expected_uid=0)
  self.assertEqual(r['result'],'QUIET')
 def test_changed_inventory_is_bounded_unknown(self):
  ns={'__name__':'fixture'};exec(o.WSL_PROBE,ns)
  import io,types
  counter=[0]
  def ls(path):counter[0]+=1;return [str(400+counter[0])]
  text='444 (owned) S '+' '.join(['0']*18+['1'])
  def file(path,mode='r'):return io.BytesIO(b'OTHER=private\0') if mode=='rb' else io.StringIO(text)
  with patch.dict(os.environ,{'WSL_DISTRO_NAME':'Owned'}),patch('os.getuid',return_value=0),patch('os.geteuid',return_value=0),patch('os.listdir',side_effect=ls),patch('os.stat',return_value=types.SimpleNamespace(st_uid=1000)),patch('builtins.open',side_effect=file),patch('os.pidfd_open',return_value=42),patch('os.close'):
   r=ns['probe']('token',False,'Owned',0,True)
  self.assertEqual(r['result'],'UNKNOWN');self.assertIn('inventory_unstable',r['counts']);self.assertEqual(counter[0],6)
 def test_actual_reparented_child_after_top_level_exit_still_blocks(self):
  # Separate harness becomes subreaper solely to clean up all its owned descendants.
  script="""import ctypes,os,subprocess,sys,json,signal
if ctypes.CDLL(None).prctl(36,1,0,0,0)!=0:raise SystemExit(77)
token='owned-reparented-token'
childcode='import time;time.sleep(30)'
parentcode='import subprocess,sys; p=subprocess.Popen([sys.executable,"-c",'+repr(childcode)+'],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL,start_new_session=True); print(p.pid,flush=True)'
p=subprocess.run([sys.executable,'-c',parentcode],capture_output=True,text=True,env={**os.environ,'MSM2_WS_OPERATION_TOKEN':token})
pid=int(p.stdout.strip())
try:
 ns={'__name__':'owned_harness'};exec(PROBE,ns)
 original=os.listdir;os.listdir=lambda path:[str(pid)] if path=='/proc' else original(path)
 os.environ['WSL_DISTRO_NAME']='Owned'
 r=ns['probe'](token,False,'Owned',os.getuid(),os.getuid()==0)
 print(json.dumps({'top_exit':p.returncode,'result':r['result']}))
finally:
 os.kill(pid,signal.SIGTERM);os.waitpid(pid,0)
""".replace('exec(PROBE,ns)','exec('+repr(o.WSL_PROBE)+',ns)')
  p=subprocess.run([sys.executable,'-c',script],capture_output=True,text=True,timeout=10)
  if p.returncode==77:self.skipTest('Owned test harness cannot become Linux subreaper')
  self.assertEqual(p.returncode,0,p.stderr);self.assertEqual(json.loads(p.stdout),{'top_exit':0,'result':'ACTIVE'})
if __name__=='__main__':unittest.main()
