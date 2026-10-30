"""Actual owned Git refresh/legacy recovery/KIT files, fake Windows actions only."""
from pathlib import Path
import importlib.util,json,shutil,subprocess,tempfile,unittest
import operation_lock as o
import mod_environment as m
import repair_tools as r
from test_mod_environment import System
ROOT=Path(__file__).resolve().parent.parent

def git(p,*args):return subprocess.check_output(['git','-C',str(p),*args],stderr=subprocess.PIPE,text=True).strip()
class Repair(unittest.TestCase):
 def setUp(self):
  self.tmp=tempfile.TemporaryDirectory(prefix="MSM2 修復 ' ");self.addCleanup(self.tmp.cleanup);self.root=Path(self.tmp.name);self.game=self.root/'Game';self.game.mkdir()
  for n in ['Spider-Man2.exe','scripts_proxy.dll','winmm.dll']:(self.game/n).write_bytes(('owned '+n).encode())
  (self.game/'Overstrike').mkdir();(self.game/'Overstrike/Overstrike.exe').write_bytes(b'owned manager never executed');(self.game/'Overstrike/scripts_proxy.dll').write_bytes(b'owned installer DLL')
  (self.game/'Profiles').mkdir();(self.game/'Profiles/Main.json').write_text(json.dumps({'game':'MSM2','path':str(self.game)}))
  self.ws=self.game/'Mods Library/Mods-Developing/MSM2-WS';(self.ws/'_archive').mkdir(parents=True);(self.ws/'logs').mkdir()
  self.script=self.ws/'MSM2-WS-v1.0.0.script';self.script.write_bytes(b'previous runtime must not rebuild or change')
  self.up=self.root/'upstream';self.up.mkdir();git(self.up,'init','-b',r.BRANCH);git(self.up,'config','user.name','Owned');git(self.up,'config','user.email','owned@example.invalid')
  (self.up/'.gitignore').write_text('**/__pycache__/\n')
  env=self.up/'environment';env.mkdir()
  for n in m.KIT:shutil.copy2(ROOT/'environment'/n,env/n)
  tools=self.up/'source/tools';tools.mkdir(parents=True)
  for n in ['workspace.py','retired_probes.py']:shutil.copy2(ROOT/'source/tools'/n,tools/n)
  (tools/'windows').mkdir();shutil.copy2(ROOT/'source/tools/windows/Update-WS.cmd',tools/'windows/Update-WS.cmd')
  git(self.up,'add','.');git(self.up,'commit','-m','owned baseline');self.remote=self.root/'remote.git';subprocess.run(['git','clone','--bare',str(self.up),str(self.remote)],check=True,capture_output=True)
  self.source=self.ws/'_source';subprocess.run(['git','clone',str(self.remote),str(self.source)],check=True,capture_output=True)
  self.oldremote=r.REMOTE;r.REMOTE=str(self.remote);self.addCleanup(setattr,r,'REMOTE',self.oldremote)
  git(self.source,'config','wallsprint.expectedBranch',r.BRANCH)
  import sys;sys.path.insert(0,str(ROOT/'source/tools'));import workspace as w
  self.oldwremote=w.REMOTE;w.REMOTE=str(self.remote);self.addCleanup(setattr,w,'REMOTE',self.oldwremote)
  # The repaired fresh module uses its canonical default origin constant; owned
  # fixture substitutes only that constant in its Git-authenticated source.
  p=self.up/'source/tools/workspace.py';p.write_text(p.read_text().replace("REMOTE='https://github.com/deluxxxe-dev/MSM2-WallSprint.git'",'REMOTE='+repr(str(self.remote))))
  git(self.up,'add','.');git(self.up,'commit','-m','owned current tooling');git(self.up,'remote','add','origin',str(self.remote));git(self.up,'push','origin',r.BRANCH);git(self.source,'fetch','origin',r.BRANCH)
  self.head=git(self.up,'rev-parse','HEAD')
  git(self.source,'sparse-checkout','set','--no-cone',*w.SOURCE_PATTERNS)
  target=self.game/'Mod-Tools';target.mkdir();self.settings={'format':'msm2-mod-tools-1','tool_version':'old','backup_directory':str(self.root/'private backup'),'profile':'Main','restore_requires_install':True};(target/'mod-tools.json').write_text(json.dumps(self.settings));(target/'mod_environment.py').write_text("raise AssertionError('OLD CHECKER EXECUTED')\n");(target/'Start-Overstrike.cmd').write_bytes(b'old launcher')
  self.system=System();self.checks=[]
 def repair(self):r.repair(self.source,self.game,self.head,self.system,lambda g,record:self.checks.append(record))
 def test_refresh_before_old_checker_idempotent_runtime_and_settings_preserved(self):
  (self.ws/'_archive/Update-WS.lock').touch();runtime=self.script.read_bytes();dlls=[(p,p.read_bytes()) for p in self.game.rglob('*.dll')]
  self.repair();self.assertTrue(self.checks);self.assertEqual(git(self.source,'rev-parse','HEAD'),self.head)
  for n in m.KIT:self.assertEqual((self.game/'Mod-Tools'/n).read_bytes(),(self.source/'environment'/n).read_bytes())
  current=json.loads((self.game/'Mod-Tools/mod-tools.json').read_text())
  for k,v in self.settings.items():
   if k!='tool_version':self.assertEqual(current[k],v)
  backups=list((self.game/'Mod-Tools/_toolkit_previous').iterdir());self.assertEqual(len(backups),1)
  self.assertIn('OLD CHECKER', (backups[0]/'mod_environment.py').read_text())
  self.repair();self.assertEqual(list((self.game/'Mod-Tools/_toolkit_previous').iterdir()),backups)
  self.assertEqual(self.script.read_bytes(),runtime)
  for p,data in dlls:self.assertEqual(p.read_bytes(),data)
  self.assertTrue((self.ws/'Update-WS.cmd').is_file())
 def test_readonly_diagnostic_does_not_claim_refresh_or_deploy(self):
  from unittest.mock import patch
  import contextlib,io
  with o.hold(self.game,'update'):
   o.phase('building',build_token='owned-token',distro='Owned',linux_uid=1000)
  lock=self.game/'.MSM2-Operations.guard';before=lock.read_bytes();head=git(self.source,'rev-parse','HEAD');kit=(self.game/'Mod-Tools/mod_environment.py').read_bytes();runtime=self.script.read_bytes()
  with patch.object(o,'wsl_quiet',return_value={'result':'QUIET'}) as quiet,contextlib.redirect_stdout(io.StringIO()) as output:r.diagnose(self.source,self.game)
  self.assertIn('read_only=1',output.getvalue());self.assertEqual(quiet.call_args.args[0]['linux_uid'],1000)
  self.assertEqual((lock.read_bytes(),git(self.source,'rev-parse','HEAD'),(self.game/'Mod-Tools/mod_environment.py').read_bytes(),self.script.read_bytes()),(before,head,kit,runtime))
 def test_dirty_and_divergent_preserve_checkout_runtime(self):
  p=self.source/'.gitignore';p.write_text('dirty owned file')
  with self.assertRaisesRegex(RuntimeError,'Dirty'):self.repair()
  self.assertEqual(p.read_text(),'dirty owned file')
  git(self.source,'checkout','--','.gitignore');git(self.source,'config','user.name','Owned');git(self.source,'config','user.email','owned@example.invalid');(self.source/'own.txt').write_text('keep');git(self.source,'add','.');git(self.source,'commit','-m','unpublished owner change')
  head=git(self.source,'rev-parse','HEAD')
  with self.assertRaises(RuntimeError):self.repair()
  self.assertEqual(git(self.source,'rev-parse','HEAD'),head)
 def test_active_update_blocks_all_shared_actions(self):
  with o.hold(self.game,'update'):
   for call in [lambda:m.start_manager(self.game,self.system),lambda:m.open_manager(self.game,self.system),lambda:m.install(self.game,system=self.system),lambda:m.backup(self.game,self.root/'out',system=self.system)]:
    with self.assertRaises(m.EnvironmentError):call()
  self.assertFalse(any(x[0]=='launch' for x in self.system.events))
if __name__=='__main__':unittest.main()
