"""Four-face evidence scoring; no surface/ABI verification is inferred."""
import math
AXES={'A':(0.,0.,1.),'B':(1.,0.,0.),'C':(0.,0.,-1.),'D':(-1.,0.,0.)}
def dot(a,b):return sum(x*y for x,y in zip(a,b))
def unit(v):
 n=math.hypot(*v)
 return tuple(x/n for x in v) if n>1e-8 and all(math.isfinite(x) for x in v) else None
def score(records):
 faces={f:[r for r in records if r['face']==f] for f in AXES};cols={};means={};signed=[];allstable=True
 for f,rs in faces.items():
  vals=[]
  for r in rs:
   v=unit(r['vector'])
   if v is None:continue
   d=dot(v,AXES[f]);vals.append(abs(d));signed.append(d);allstable=allstable and r.get('stable',False)
  cols[f]=min(vals) if vals else None
  means[f]=unit(tuple(sum(r['vector'][i] for r in rs)/len(rs) for i in range(3))) if rs else None
 complete=all(v is not None for v in cols.values());minimum=min(cols.values()) if complete else None;mean=sum(cols.values())/4 if complete else None
 ac=dot(means['A'],means['C']) if means['A'] and means['C'] else None;bd=dot(means['B'],means['D']) if means['B'] and means['D'] else None
 ab=abs(dot(means['A'],means['B'])) if means['A'] and means['B'] else None
 signed_pattern='same_as_crawl_axes' if signed and all(x>=.97 for x in signed) else 'opposite_to_crawl_axes' if signed and all(x<=-.97 for x in signed) else 'mixed_or_mismatch'
 rotation=complete and ab is not None and ab<.3 and ac is not None and bd is not None and abs(ac)>=.97 and abs(bd)>=.97
 strong=complete and allstable and minimum>=.97 and mean>=.985 and rotation
 return {'face_abs_dot_min_over_replicates':cols,'mean_abs_dot_face_balanced':mean,'min_abs_dot':minimum,'signed_pattern':signed_pattern,'opposite_A_C_dot':ac,'opposite_B_D_dot':bd,'between_face_rotation':bool(rotation),'complete_primary_set':complete,'strong_axis_candidate':bool(strong),'strong_signed_candidate':bool(strong and signed_pattern!='mixed_or_mismatch' and ac<=-.97 and bd<=-.97),'surface_verified':False}
