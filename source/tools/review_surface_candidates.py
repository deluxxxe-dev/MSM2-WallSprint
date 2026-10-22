"""Review v0.1a5/a6 bounded candidates. No automatic Verified promotion."""
import argparse,re,json,math,collections
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('log',type=Path);a=p.parse_args()
fields=collections.defaultdict(list);scores=collections.defaultdict(list);points=collections.defaultdict(list);raw=collections.defaultdict(dict);pairs=[]
for line in a.log.read_text(errors='replace').splitlines():
 d=dict(re.findall(r'(\w+)=([^ ]+)',line))
 if 'object_vec3_candidate ' in line:fields[d['offset']].append(d)
 if 'surface_candidate_score ' in line:scores[d['offset']].append(d)
 if 'surface_contact_candidate_score ' in line:points[d['offset']].append(d)
 if 'surface_calibration_pair ' in line:pairs.append(d)
 if 'wallrun_object_raw_floats ' in line:
  values=[float(x) for x in d['values'].strip('()').split(',')];start=int(d['offset'],16)
  for i,x in enumerate(values):raw[(d['session'],d['phase'])][start+i*4]=x
layout=[]
for (session,phase),floats in raw.items():
 vectors={}
 for offset in sorted(floats):
  if not all(offset+i*4 in floats for i in range(3)):continue
  v=[floats[offset+i*4] for i in range(3)];norm=math.hypot(*v)
  if all(math.isfinite(x) and abs(x)<=1.25 for x in v) and .9<=norm<=1.1:vectors[offset]=v
 clusters=[]
 for offset,v in vectors.items():
  if not clusters or offset>=clusters[-1]['end_exclusive']:clusters.append({'start':offset,'end_exclusive':offset+12,'offsets':[]})
  clusters[-1]['end_exclusive']=offset+12;clusters[-1]['offsets'].append(hex(offset))
 matrices=[];quaternions=[]
 for offset in sorted(floats):
  if all(offset+i*4 in floats for i in range(4)):
   f=[floats[offset+i*4] for i in range(4)];q=sum(x*x for x in f)
   if all(math.isfinite(x) for x in f) and .98<q<1.02:quaternions.append(hex(offset))
  for stride in [12,16]:
   if not all(offset+row*stride+i*4 in floats for row in range(3) for i in range(3)):continue
   rows=[[floats[offset+row*stride+i*4] for i in range(3)] for row in range(3)]
   if all(all(math.isfinite(x) for x in v) and abs(sum(x*x for x in v)-1)<.02 for v in rows) and all(abs(sum(rows[i][k]*rows[j][k] for k in range(3)))<.02 for i in range(3) for j in range(i+1,3)):matrices.append({'offset':hex(offset),'stride':stride,'classification':'matrix-like_numeric_hypothesis'})
 layout.append({'session':session,'phase':phase,'raw_range':'0x120..0x1e0 exclusive','overlap_clusters':clusters,'matrix_hypotheses':matrices,'quaternion_like_offsets_ambiguous_with_vector_padding':quaternions})
rows=[]
for offset,values in fields.items():
 vectors=[tuple(map(float,d['last'].strip('()').split(','))) for d in values];spread=max((math.dist(x,y) for x in vectors for y in vectors),default=0)
 last=scores[offset][-1] if scores[offset] else None
 rows.append({'offset':offset,'sessions':sorted({int(d['session']) for d in values}),'classification':'CONSTANT_WORLD_AXIS_SUSPECT' if len(values)>=2 and spread<.0001 else 'VARIABLE_DIRECTION_CANDIDATE','between_session_variability':spread,'overlapping_offsets':[o for o in fields if o!=offset and abs(int(o,16)-int(offset,16))<12],'last_calibration_score':last,'candidate_samples':values})
rows.sort(key=lambda d:(float((d['last_calibration_score'] or {}).get('discovery_score',0)),d['between_session_variability']),reverse=True)
print(json.dumps({'status':'UNVERIFIED_CANDIDATES_ONLY','surface_valid':False,'v01b_gate':'BLOCKED','requirements':['>=3 pairwise nonparallel same-wall video-confirmed calibrations, abs dot >=.98, stable within session and rotates between walls','Resolve overlapping fields/structure and outward sign; verify contact point or equivalent adhesion reference','Opposite normals count as one unoriented axis; crawl-like is an orientation proxy, not native state.'],'pairs':pairs,'offsets':rows,'contact_scores':dict(points),'raw_layout_hypotheses':layout},indent=2,allow_nan=False))
