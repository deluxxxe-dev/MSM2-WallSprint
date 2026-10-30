"""Run from the fresh bootstrap staging directory, under the shared OS gate."""
from pathlib import Path
import argparse,hashlib,importlib.util,subprocess,sys
import operation_lock
BRANCH='codex/v01c20-edi-value-provenance'
REMOTE='https://github.com/deluxxxe-dev/MSM2-WallSprint.git'
def git(source,*args):
 p=subprocess.run(['git','-C',str(source),*args],capture_output=True,text=True,**operation_lock.child_options())
 if p.returncode:raise RuntimeError('Git refused: '+p.stderr.strip())
 return p.stdout.strip()
def repair(source,game,commit,system=None,quiescent=None):
 source=Path(source).absolute();game=Path(game).absolute();operation_lock.safe(source);operation_lock.safe(game)
 if source!=game/'Mods Library/Mods-Developing/MSM2-WS/_source':raise RuntimeError('Exact WS source layout required')
 if not (source/'.git').is_dir() or (source/'.git').is_symlink():raise RuntimeError('Independent Git directory required')
 if git(source,'branch','--show-current')!=BRANCH or git(source,'remote','get-url','origin').rstrip('/')!=REMOTE:raise RuntimeError('Wrong branch/origin; preserve checkout')
 with operation_lock.hold(game,'repair',quiescent=quiescent):
  if git(source,'status','--porcelain'):raise RuntimeError('Dirty Git checkout preserved; commit/preserve changes before repair')
  git(source,'merge-base','--is-ancestor','HEAD',commit)
  git(source,'merge','--ff-only',commit)
  if git(source,'rev-parse','HEAD')!=commit:raise RuntimeError('Checkout head changed during repair')
  canonical=source/'environment/operation_lock.py'
  if canonical.read_bytes()!=Path(operation_lock.__file__).read_bytes():raise RuntimeError('Bootstrap lock module differs from fetched commit')
  sys.path.insert(0,str(source/'source/tools'))
  spec=importlib.util.spec_from_file_location('repair_workspace',source/'source/tools/workspace.py');w=importlib.util.module_from_spec(spec);spec.loader.exec_module(w)
  w.workspace_paths(source.parent);w.independent_clone(source);w.expected_branch(source,BRANCH)
  spec=importlib.util.spec_from_file_location('current_mod_tools',source/'environment/mod_environment.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
  m.repair_locked(game,system)
def diagnose(source,game):
 source=Path(source).absolute();game=Path(game).absolute();operation_lock.safe(source);operation_lock.safe(game)
 if source!=game/'Mods Library/Mods-Developing/MSM2-WS/_source':raise RuntimeError('Exact WS source layout required')
 operation_lock.safe(source/'.git')
 if not (source/'.git').is_dir():raise RuntimeError('Independent Git directory required')
 if git(source,'branch','--show-current')!=BRANCH or git(source,'remote','get-url','origin').rstrip('/')!=REMOTE:raise RuntimeError('Wrong branch/origin')
 # No claim/refresh/merge/build/deploy. Read existing evidence without exposing token.
 for path in [game/'.MSM2-Operations.guard',game/'Mods Library/Mods-Developing/MSM2-WS/_archive/Update-WS.lock']:
  operation_lock.safe(path)
  if not path.is_file():continue
  record=operation_lock.parsed(path.read_bytes(),game)
  if record and record.get('build_token'):
   print('WSL_DIAGNOSTIC read_only=1 ownership_or_deployment_certificate=0')
   result=operation_lock.wsl_quiet(record)
   print(__import__('json').dumps(result,sort_keys=True));return
 raise RuntimeError('No authenticated pending build record; do not infer deployment/inactivity')
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--source',required=True);p.add_argument('--game-root',required=True);p.add_argument('--commit',required=True);p.add_argument('--diagnose-wsl',action='store_true');a=p.parse_args()
 try:
  if a.diagnose_wsl:diagnose(a.source,a.game_root)
  else:repair(a.source,a.game_root,a.commit)
 except Exception as ex:raise SystemExit('TOOLING ACTION REFUSED: '+str(ex))
