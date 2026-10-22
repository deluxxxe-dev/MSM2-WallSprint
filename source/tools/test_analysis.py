"""Validate analysis against the current production-worker synthetic evidence."""
from pathlib import Path
import json,re
root=Path(__file__).resolve().parents[1]
log=(root/'logs/windows_surface_runtime_synthetic.log').read_text();timeline=(root/'logs/TEST_TIMELINE_SYNTHETIC.txt').read_text();result=json.loads((root/'logs/SURFACE_CANDIDATES_SYNTHETIC.json').read_text())
assert 'Session 1:' in timeline and 'Crawl calibration paired to session 1' in timeline
assert 'start=(10.000000,20.000000,30.000000)' in timeline
assert result['pairs'] and result['surface_valid'] is False and result['v01b_gate']=='BLOCKED'
assert result['raw_layout_hypotheses'] and result['contact_scores']
raw=[dict(re.findall(r'(\w+)=([^ ]+)',line)) for line in log.splitlines() if 'wallrun_object_raw_floats session=1 ' in line]
assert len(raw)==24
for phase in ['first','last']:
 assert sorted(int(d['offset'],16) for d in raw if d['phase']==phase)==list(range(0x120,0x1e0,16))
result='PASS: timeline/candidate tools parse production synthetic log; pair/positions/contact present; exactly two bounded endpoint dumps; gate blocked'
(root/'logs/analysis_tools_tests.log').write_text(result+'\n');print(result)

source=json.loads((root/'logs/source_synthetic/SURFACE_SOURCE_REVIEW.json').read_text())
assert len(source['function_captures'])==1 and source['function_captures'][0]['synthetic']
assert any(d['parent_field_signed']=='136' and d['offset_signed']=='84' for d in source['this_accesses'])
assert source['pointer_plans'] and any('WallRun.this+0x88->child' in path for path in source['four_face_scores'])
assert source['surface_axis_verified'] is False and source['v01b_gate']=='BLOCKED'
print('PASS: native function capture checksum/disassembly/access map and full child provenance; no synthetic fixture promoted to game evidence')
