from pathlib import Path
import copy,json,tempfile
from review_surface_pair import review,AXES,add,scale
root=Path(__file__).resolve().parents[1]
mapping={'sessions':{'1':'A','2':'E','3':'E'},'control_sessions':[1],'diagonal_sessions':[2,3],'vertical_sessions':[3],'source':'SYNTHETIC_TEST_ONLY','same_face_E_confirmed':True,'diagonal_geometry_confirmed':True}
baseline_file=root/'analysis/SURFACE_PAIR_V01A8_STATS.json'
if not baseline_file.exists():baseline_file=root/'tools/fixtures/cardinal_baseline_summary.json'
baseline=json.loads(baseline_file.read_text());assert baseline['cardinal_evidence_pass'] and not baseline['gate']['surface_axis_verified']
def synthetic(case):
 lines=['static_field_plan enabled=1']+[f'static_access_v2 function={f} authenticated_plan={0 if case=="bad_hash" and f=="Entry" else 1}' for f in ['Init','Entry','Update','Deinit']]
 fmt=lambda v:'('+','.join(f'{x:.9g}' for x in v)+')'
 for s in (1,2,3):
  n=(0.,0.,1.) if s==1 else (.6,0.,.8);raw=n if case!='quantized' or s==1 else (0.,0.,1.);raw=tuple(-x for x in n) if case=='wrong_sign' and s>=2 else raw
  lines.append(f'WallRun verified session #{s} Init=1 Entry=1 Update=70 Deinit=1 end_reason=verified_deinit')
  lines.append(f'session_timeline session_id={s} duration=.7 candidate={"WallRunUp" if s==3 else "WallRunSide"}')
  lines.append(f'static_field_calibration_pair wallrun_session={s} crawl_axis={fmt(n)}')
  for i,k in enumerate([0,.2,.4,.2,0,-.2,-.4]):
   p=(100.,20.,200.);near=add(p,scale(n,.5 if case!='bad_separation' or s==1 else .8));t=(n[2],0,-n[0]) if s!=3 else (0,1,0)
   h=add(add(p,scale(n,.5)),scale(t,0 if case=='no_reversal' and s==2 else k));value_n=(0.,0.,1.) if case=='bad_middle' and s==2 and i==2 else raw
   for off,v in [(0x320,near),(0x338,p),(0x398,value_n)]:
    if case=='missing' and s==2 and i==2 and off==0x338:continue
    lines.append(f'static_field_sample session={s} scope=this offset={hex(off)} sample_ms={1000*s+100*i} value={fmt(v)} hero_position={fmt(h)}')
  for off in (0x320,0x338,0x398):lines.append(f'static_field_summary session={s} scope=this offset={hex(off)} attempts=7')
 return '\n'.join(lines)
with tempfile.TemporaryDirectory() as td:
 p=Path(td)/'surface.log'
 for case in ['good','bad_hash','quantized','wrong_sign','bad_separation','bad_middle','missing','no_reversal','unconfirmed_diagonal','no_baseline']:
  p.write_text(synthetic(case));m=copy.deepcopy(mapping)
  if case=='unconfirmed_diagonal':m['same_face_E_confirmed']=False
  r=review(p,m,None if case=='no_baseline' else baseline)
  ready=r['gate']['v01b_gate']=='READY_FOR_PLANAR_WALL_MVP'
  assert ready==(case=='good'),(case,r['gate'])
  assert not r['gate']['actor_write'] and not r['gate']['physics_contact_verified']
print('PASS: actual v0.1a8 baseline plus TEST-ONLY diagonal/control/Side-Up gate; quantization/sign/pair/missing/outlier/no-reversal/hash/unconfirmed/no-baseline cases rejected; never enables writes')
