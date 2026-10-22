from pathlib import Path
import tempfile
from review_static_fields import review,AXES
mapping={'sessions':{'1':'A','2':'B','3':'C','4':'D','6':'A','7':'A'},'primary_sessions':[1,2,3,4],'vertical_sessions':[6,7],'excluded':{'5':'too_short_transition_or_failed_attach'}}
def synthetic(kind):
    out=[]
    for s,f in [(1,'A'),(2,'B'),(3,'C'),(4,'D'),(5,'A'),(6,'A'),(7,'A')]:
        axis=AXES[f];vs=axis if kind!='constant' else (0,1,0)
        if kind=='inverted':vs=tuple(-x for x in vs)
        vec=lambda v:'('+','.join(map(str,v))+')'
        if kind=='missing' and s==4:continue
        out.append(f'session_timeline session_id={s} duration={.013 if s==5 else 2} candidate={"WallRunUp" if s>=6 else "WallRunSide"}')
        out.append(f'static_field_calibration_pair wallrun_session={s} crawl_axis={vec(axis)}')
        for i in range(3):
            v=(0,1,0) if kind=='outlier' and s==2 and i==1 else vs
            out.append(f'static_field_sample session={s} scope=this offset=0x298 value={vec(v)}')
        out.append(f'static_field_summary session={s} scope=this offset=0x298 attempts=3 child_pointer_changed=0')
    return '\n'.join(out)
with tempfile.TemporaryDirectory() as td:
    p=Path(td)/'test.log'
    for k in ('correct','inverted','constant','missing','outlier'):
        p.write_text(synthetic(k));r=review(p,mapping);s=r['field_scores']['this+0x298'];assert r['excluded_sessions']['5']=='too_short_transition_or_failed_attach' and not r['surface_axis_verified'] and not r['contact_verified']
        if k in ('correct','inverted'):assert s['strong_axis_candidate'] and s['opposite_face_consistent'] and s['subtype_invariant_on_A'] and s['subtype_invariance_score']==1
        if k=='constant':assert not s['strong_axis_candidate'] and not s['subtype_invariant_on_A'] and s['constant_world_axis_status']=='CONSTANT_WORLD_AXIS_SUSPECT'
        if k=='missing':assert not s['complete_four_faces']
        if k=='outlier':assert not s['strong_axis_candidate'] and s['min_abs_dot']==0
print('PASS: targeted field A/B/C/D whole-sample correlation, opposite signs/global inversion, Side/Up invariance, constant/missing/outlier and short-session rejection; gate never promoted')
