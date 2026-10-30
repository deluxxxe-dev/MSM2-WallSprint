"""Fresh diagnostic works with pending ACTIVE record; no checkout/KIT/lock writes."""
import contextlib,io,json,unittest
from unittest.mock import patch
import operation_lock as o
import repair_tools as r
import test_tooling_repair as fixture
git=fixture.git
class Inspection(unittest.TestCase):
 setUp=fixture.Repair.setUp
 def pending(self,known=False):
  with o.hold(self.game,'update') as h:
   extra={'build_returncode':29,'build_linux_exit':29,'build_stage':'fixture_c48:line10'} if known else {}
   o.phase('build_returned' if known else 'building',build_token=h.record['token'],distro='Owned',linux_uid=1000,**extra)
  return json.loads((self.game/'.MSM2-Operations.guard').read_text())
 def observe(self):
  calls=[]
  def query(record,legacy,root,wait_ms=0):
   calls.append((root,wait_ms));return dict(result='ACTIVE',events=[dict(pid=42,ppid=1,start='1234',state='S',executable_name='wine64',process_name='services.exe',stage='match')])
  before=((self.game/'.MSM2-Operations.guard').read_bytes(),git(self.source,'rev-parse','HEAD'),(self.game/'Mod-Tools/mod_environment.py').read_bytes(),self.script.read_bytes())
  with patch.object(o,'_wsl_query',side_effect=query),contextlib.redirect_stdout(io.StringIO()) as output:r.diagnose(self.source,self.game)
  self.assertEqual(before,((self.game/'.MSM2-Operations.guard').read_bytes(),git(self.source,'rev-parse','HEAD'),(self.game/'Mod-Tools/mod_environment.py').read_bytes(),self.script.read_bytes()))
  self.assertEqual(calls,[(False,0),(True,0)]);self.assertIn('installed_KIT_updated=0',output.getvalue());return json.loads(output.getvalue().splitlines()[-1])
 def test_old_exit_not_invented_and_active_root_roles_visible(self):
  self.pending();r=self.observe();self.assertEqual(r['build_record']['build_returncode'],'not_recorded');self.assertEqual(r['progress']['status'],'not_recorded_by_old_runner');self.assertEqual(r['inspection']['root']['events'][0]['process_name'],'services.exe')
 def test_bound_exit_receipt_reports_primary_and_stage(self):
  record=self.pending(True);p=self.ws/'_archive/_build';p.mkdir();file=p/'Build-WS-status.json';file.write_text(json.dumps(dict(schema=1,token=record['build_token'],stage='fixture_c48:line10',state='failed',exit_status=29,last_detail='fixture_c48:kill:position')));before=file.read_bytes();r=self.observe();self.assertEqual(r['progress']['exit_status'],29);self.assertEqual(r['build_record']['build_returncode'],29);self.assertEqual(file.read_bytes(),before)
 def test_unbound_receipt_cannot_replace_primary(self):
  self.pending(True);p=self.ws/'_archive/_build';p.mkdir();(p/'Build-WS-status.json').write_text(json.dumps(dict(schema=1,token='different',stage='old_success')));r=self.observe();self.assertEqual(r['build_record']['build_returncode'],29);self.assertEqual(r['progress']['status'],'malformed_or_unbound_not_evidence')
if __name__=='__main__':unittest.main()
