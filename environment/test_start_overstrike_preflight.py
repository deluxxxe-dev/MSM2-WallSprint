"""Launcher behavior: refuse unsafe input under ownership, never relocate loaders."""
from test_mod_environment import EnvironmentTests,tree
import mod_environment as m

def main():
 t=EnvironmentTests();t.setUp()
 try:
  t.project('MSM2-WS')
  proxy=t.game/'Overstrike/scripts_proxy.dll';proxy.write_bytes(b'owned valid installer source')
  before=tree(t.game/'Overstrike');m.start_manager(t.game,t.system);assert tree(t.game/'Overstrike')==before
  bad=t.game/'Overstrike/winmm.dll';bad.write_bytes(b'owned misplaced loader; never auto-delete')
  before=tree(t.game/'Overstrike')
  try:m.start_manager(t.game,t.system)
  except m.EnvironmentError:pass
  else:raise AssertionError('Misplaced winmm accepted')
  assert tree(t.game/'Overstrike')==before
  print('PASS: current checked launcher accepts installer proxy and refuses misplaced loader without relocation/deletion')
 finally:t.doCleanups()
if __name__=='__main__':main()
