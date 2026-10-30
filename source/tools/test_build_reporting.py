"""Primary build status precedes completion evidence; no product/compiler invoked."""
import contextlib,importlib.util,io,json,os,subprocess,tempfile,types,unittest,uuid
from pathlib import Path
from unittest.mock import patch
import workspace as w
import operation_lock as o
class Reporting(unittest.TestCase):
 def run_build(self,rc=0,nonquiet=False,interrupt=False,bad_receipt=False,incomplete=False,linux_exit=None):
  self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup);dest=Path(self.tmp.name)
  linux_exit=rc if linux_exit is None else linux_exit
  token=uuid.uuid4().hex;self.record={'token':token};held=types.SimpleNamespace(record=self.record,files=[])
  (dest/'Build-WS-status.json').write_text('[]' if bad_receipt else json.dumps({'schema':1,'token':token,'stage':'stage_payload' if rc==0 and not nonquiet and not incomplete else 'fixture_c48:kill:position','state':'completed' if linux_exit==0 and not incomplete else 'failed','exit_status':linux_exit}))
  (dest/'Build-WS-result.json').write_text(json.dumps({'artifact':'MSM2-WS-v0.1c38r5.script.disabled'}));(dest/'BUILD_MANIFEST.json').write_text('{}')
  def run(args):
   if args[-2:]==['uname','-r']:return '6.1-WSL2'
   if args[-2:]==['id','-u']:return '1000'
   if 'wslpath' in args:return '/owned'
   raise AssertionError(args)
  def quiet(record):
   self.assertIn('primary_returncode='+str(rc),self.output.getvalue())
   self.assertEqual(record['build_returncode'],rc)
   if nonquiet:raise o.OperationError('ACTIVE owned descendants')
  self.output=io.StringIO()
  with patch.dict(os.environ,{'MSM2_WS_WSL_DISTRO':'Owned'}),patch.object(w.shutil,'which',return_value='wsl'),patch.object(w,'run',side_effect=run),patch.object(o,'_current',held),patch.object(o,'child_options',return_value={}),patch.object(o,'wsl_quiet',side_effect=quiet),patch.object(w.subprocess,'run',side_effect=KeyboardInterrupt if interrupt else None,return_value=subprocess.CompletedProcess([],rc)),contextlib.redirect_stdout(self.output):
   return w.WindowsSystem().build(Path('/owned'),dest)
 def test_failure_plus_live_children_preserves_primary_exit(self):
  with self.assertRaisesRegex(w.WorkspaceError,'primary_returncode=37.*ACTIVE'):self.run_build(37,True)
  self.assertEqual(self.record['phase'],'build_failed');self.assertEqual(self.record['build_token'],self.record['token'])
 def test_transport_zero_linux_failure_keeps_both_original_codes(self):
  with self.assertRaisesRegex(w.WorkspaceError,'primary_returncode=0 linux_pipeline_exit=31.*ACTIVE'):self.run_build(0,True,linux_exit=31)
  self.assertEqual(self.record['phase'],'build_failed');self.assertEqual(self.record['build_linux_exit'],31)
 def test_success_plus_live_children_is_not_validated(self):
  with self.assertRaisesRegex(w.WorkspaceError,'primary_returncode=0.*ACTIVE'):self.run_build(0,True)
  self.assertEqual(self.record['phase'],'build_nonquiet');self.assertIn('build_token',self.record)
 def test_failure_plus_quiet_never_deploys_or_validates(self):
  with self.assertRaisesRegex(w.WorkspaceError,'primary_returncode=23'):self.run_build(23)
  self.assertEqual(self.record['phase'],'build_failed');self.assertIsNone(self.record['build_token'])
 def test_success_complete_still_requires_manager_payload_validation(self):
  self.run_build();self.assertEqual(self.record['phase'],'build_complete');self.assertIsNone(self.record['build_token'])
 def test_zero_exit_mid_suite_cannot_reuse_stale_artifact(self):
  with self.assertRaisesRegex(w.WorkspaceError,'no authenticated completion receipt'):self.run_build(incomplete=True)
  self.assertEqual(self.record['phase'],'build_validation_failed');self.assertEqual(self.record['build_returncode'],0)
 def test_interrupt_keeps_pending_with_unknown_exit(self):
  with self.assertRaises(KeyboardInterrupt):self.run_build(interrupt=True)
  self.assertEqual(self.record['phase'],'build_interrupted');self.assertIsNone(self.record['build_returncode']);self.assertEqual(self.record['build_token'],self.record['token'])
 def test_malformed_progress_cannot_mask_primary(self):
  with self.assertRaisesRegex(w.WorkspaceError,'primary_returncode=41'):self.run_build(41,True,bad_receipt=True)
  self.assertIn('unknown_unbound_receipt',self.output.getvalue())
 def test_progress_token_mismatch_is_not_reused(self):
  with tempfile.TemporaryDirectory() as d:
   Path(d,'Build-WS-status.json').write_text(json.dumps({'schema':1,'token':'old','stage':'old:success'}));self.assertEqual(w.build_progress(d,'new')['stage'],'unknown_unbound_receipt')
   Path(d,'Build-WS-status.json').write_text(json.dumps({'schema':1,'token':'new','stage':'stage_payload','state':'completed','exit_status':False}));self.assertEqual(w.build_progress(d,'new')['stage'],'unknown_unbound_receipt')
 def test_stage_status_preserves_primary_and_specific_fixture(self):
  spec=importlib.util.spec_from_file_location('status',Path(__file__).parent/'windows/build-status.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
  with tempfile.TemporaryDirectory() as d,patch.dict(os.environ,{'MSM2_WS_OPERATION_TOKEN':uuid.uuid4().hex,'WSL_DISTRO_NAME':'Owned'}):
   m.record(d,'hooks','running');m.record(d,'fixture_c48:line10','failed',17);m.record(d,'hooks:line150','failed',17);m.record(d,'hooks','finished',17)
   r=json.loads(Path(d,'Build-WS-status.json').read_text());self.assertEqual((r['stage'],r['last_detail'],r['exit_status'],r['state']),('hooks:line150','fixture_c48:line10',17,'failed'))
class ShellStatus(unittest.TestCase):
 def fixture(self,hooks):
  import shutil
  tmp=tempfile.TemporaryDirectory(prefix='owned stage β ');self.addCleanup(tmp.cleanup);root=Path(tmp.name);repo=root/'repo';dest=root/'result';repo.mkdir()
  for n in ['docs','analysis','environment','source/tools/windows']:(repo/n).mkdir(parents=True,exist_ok=True)
  for n in ['AGENTS.md','README.md','README_TEST.md','Update-WS.cmd','Migrate-WS.cmd','.gitattributes']:(repo/n).write_text('owned fixture')
  helper=Path(__file__).parent/'windows/build-status.py';shutil.copy2(helper,repo/'source/tools/windows/build-status.py')
  for n in ['build_windows.sh','test.sh','smoke_windows.sh']:(repo/'source/tools'/n).write_text('#!/bin/bash\nexit 0\n')
  (repo/'source/tools/test_hooks_windows.sh').write_text(hooks)
  (repo/'source/tools/release.py').write_text("import json,zipfile;from pathlib import Path;p=Path('MSM2-WS-v1.0.script.disabled');z=zipfile.ZipFile(p,'w');z.writestr('owned','fixture');z.close();Path('BUILD_MANIFEST.json').write_text(json.dumps({'version':'v1.0'}))")
  token=uuid.uuid4().hex
  p=subprocess.run(['bash',str(Path(__file__).parent/'windows/workspace-build.sh'),str(repo),str(dest)],env={**os.environ,'MSM2_WS_OPERATION_TOKEN':token,'WSL_DISTRO_NAME':'Owned'},capture_output=True,text=True)
  return p,json.loads((dest/'Build-WS-status.json').read_text())
 def test_real_shell_failure_keeps_specific_stage_and_exit(self):
  p,r=self.fixture('cd source\npython3 tools/windows/build-status.py "$MSM2_WS_STATUS_DIR" fixture_c48:kill:position failed 37\nexit 37\n');self.assertEqual(p.returncode,37,p.stderr);self.assertEqual((r['state'],r['exit_status'],r['stage']),('failed',37,'fixture_c48:kill:position'));self.assertNotIn('required full WSL',p.stdout)
 def test_real_shell_interrupt_records143_without_success(self):
  p,r=self.fixture('kill -TERM "$PPID"\nexit 0\n');self.assertEqual(p.returncode,143,p.stderr);self.assertEqual((r['state'],r['exit_status']),('failed',143));self.assertNotIn('required full WSL',p.stdout)
 def test_real_shell_success_records_completion_not_deployment(self):
  p,r=self.fixture('exit 0\n');self.assertEqual(p.returncode,0,p.stderr);self.assertEqual((r['state'],r['exit_status'],r['stage']),('completed',0,'stage_payload'));self.assertIn('required full WSL',p.stdout)
if __name__=='__main__':unittest.main()
