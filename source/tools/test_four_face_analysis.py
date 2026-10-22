"""Regression: de-duplicated equality groups must not hide a rotating child field."""
from pathlib import Path
import tempfile,subprocess,sys,json
from four_face_score import AXES
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='wallsprint-four-face-') as folder:
 d=Path(folder)
 for sign in [1,-1]:
  rows=[]
  for i,(face,axis) in enumerate(AXES.items(),1):
   v=tuple(sign*x for x in axis)
   rows.append(f'surface_calibration_pair wallrun_session={i} crawl_axis=({axis[0]},{axis[1]},{axis[2]})')
   rows.append(f'source_path=WallRun.this+0x88->child object_repeated_basis_group session={i} representative=0x40 members=0x40,0x54 candidate=({v[0]},{v[1]},{v[2]}) stable=1')
   rows.append(f'source_path=WallRun.this+0x90->child object_repeated_basis_group session={i} representative=0x40 members=0x40,0x54 candidate=(1,0,0) stable=1')
  log=d/'groups.log';log.write_text('\n'.join(rows)+'\n');out=d/str(sign)
  subprocess.run([sys.executable,str(root/'tools/review_surface_source.py'),str(log),'--output-dir',str(out)],check=True,stdout=subprocess.DEVNULL)
  result=json.loads((out/'SURFACE_SOURCE_REVIEW.json').read_text());good=result['four_face_scores']['WallRun.this+0x88->child+0x54'];bad=result['four_face_scores']['WallRun.this+0x90->child+0x54']
  assert good['strong_axis_candidate'] and good['strong_signed_candidate'] and good['min_abs_dot']==1
  assert not bad['strong_axis_candidate'] and bad['min_abs_dot']==0 and not result['surface_axis_verified']
  mapping=d/'map.json';mapping.write_text(json.dumps({'sessions':{'1':'A','2':'B','3':'C'}}))
  subprocess.run([sys.executable,str(root/'tools/review_surface_source.py'),str(log),'--face-map',str(mapping),'--output-dir',str(out)],check=True,stdout=subprocess.DEVNULL)
  result=json.loads((out/'SURFACE_SOURCE_REVIEW.json').read_text());assert not result['four_face_scores']['WallRun.this+0x88->child+0x54']['complete_primary_set']
message='PASS: grouped child offset identity across four faces/global sign inversion; constant axis and incomplete confirmed mapping rejected; no Verified promotion'
(root/'logs/four_face_analysis_tests.log').write_text(message+'\n');print(message)
