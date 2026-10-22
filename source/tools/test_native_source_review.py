from pathlib import Path
import tempfile,re
from review_native_source import review
r=Path(__file__).resolve().parents[1];source=r/'logs/windows_native_source_synthetic.log';result=review(source)
assert sum(s['scope_retention_pass'] for s in result['sessions'].values())>=2
assert not result['actual_game_capture'] and not result['v01b5_game_pass_observed'] and not result['adhesion_permission'] and not result['direct_actor_write_permission']
text=source.read_text()
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'bad.log'
 for bad in [text.replace('position_unchanged=1','position_unchanged=0'),text.replace('source_is_stack_local=1','source_is_stack_local=0'),re.sub(r'actor_up_dot_surface=\S+','actor_up_dot_surface=0',text),text.replace('observation_phase=next_matching_native_PRE','observation_phase=worker')]:
  p.write_text(bad);b=review(p);assert not any(s['scope_retention_pass'] for s in b['sessions'].values())
 # Losing orientation after the minimum good period still reports an abort.
 p.write_text(text+'\nNATIVE_SOURCE_EXIT session=1 reason=B_returned_from_surface_orientation\n');b=review(p);assert not b['sessions']['1']['scope_retention_pass']
print('PASS: real independent native fixture logs show A/E retained orientation; source writes alone/position mismatch/nonstack/WorldUp/worker-only fail; synthetic never game PASS or next phase permission')
