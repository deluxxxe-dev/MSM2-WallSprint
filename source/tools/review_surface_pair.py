"""Targeted N398/P338/P320 validation. No game access; explicit new-log labels.
All-sample metrics remain distinct from endpoint/cardinal proxy metrics.
"""
from pathlib import Path
import argparse,collections,hashlib,itertools,json,math,re,statistics
AXES={'A':(0.,0.,1.),'B':(1.,0.,0.),'C':(0.,0.,-1.),'D':(-1.,0.,0.)}
NORMALS=[0x350,0x35c,0x368,0x374,0x380,0x38c,0x398,0x3a4,0x3e0,0x3ec]
POINTS=[0x320,0x32c,0x338,0x3c8,0x3d4]
def vec(s):return tuple(map(float,s.strip('()').split(',')))
def add(a,b):return tuple(x+y for x,y in zip(a,b))
def sub(a,b):return tuple(x-y for x,y in zip(a,b))
def scale(a,s):return tuple(x*s for x in a)
def dot(a,b):return sum(x*y for x,y in zip(a,b))
def length(a):return math.sqrt(dot(a,a))
def finite(a):return all(math.isfinite(x) for x in a)
def unit(a):
 n=length(a)
 return scale(a,1/n) if math.isfinite(n) and n>1e-8 else None

def stats(xs):
 xs=list(xs)
 if not xs:return None
 ys=sorted(xs);return {'n':len(xs),'min':ys[0],'mean':statistics.mean(xs),'median':statistics.median(xs),'p95':ys[min(len(ys)-1,math.ceil(.95*len(ys))-1)],'max':ys[-1]}

def read_log(path):
 groups=collections.defaultdict(dict);timelines={};pairs={};lifecycle={};hashes={};summaries={};plan=False
 for l in path.read_text(errors='replace').splitlines():
  d=dict(re.findall(r'(\w+)=([^ ]+)',l))
  if 'static_field_sample ' in l and d['scope']=='this':
   key=(int(d['session']),int(d['sample_ms']));off=int(d['offset'],16)
   if off in groups[key]:raise ValueError('duplicate session/time/offset sample; do not silently combine restarts')
   groups[key][off]={'value':vec(d['value']),'hero':vec(d['hero_position'])}
  if 'session_timeline ' in l:timelines[int(d['session_id'])]=d
  if 'static_field_calibration_pair ' in l:pairs[int(d['wallrun_session'])]=unit(vec(d['crawl_axis']))
  if 'WallRun verified session #' in l:lifecycle[int(re.search(r'session #(\d+)',l)[1])]=d
  if 'static_access_v2 ' in l:hashes[d['function']]=d.get('authenticated_plan')=='1'
  if 'static_field_plan ' in l:plan=d.get('enabled')=='1'
  if 'static_field_summary ' in l and d['scope']=='this':summaries[(int(d['session']),int(d['offset'],16))]=d
 return groups,timelines,pairs,lifecycle,hashes,plan,summaries

def review(path,mapping,baseline=None):
 groups,timelines,pairs,lifecycle,hashes,plan,summaries=read_log(path);labels={int(s):f for s,f in mapping['sessions'].items()};excluded={int(s):v for s,v in mapping.get('excluded',{}).items()}
 for s,t in timelines.items():
  if float(t['duration'])<.1:excluded[s]='too_short_transition_or_failed_attach'
 sessions={};all_distances=[];duplicate=[];sample_rows=[]
 for s in sorted(labels):
  if s in excluded:continue
  rows=[];missing=0;previous=None;positive=negative=0;normal_stability=[];first_n=None
  for (sid,ms),fields in sorted(groups.items()):
   if sid!=s:continue
   if not all(o in fields for o in (0x320,0x338,0x398)):missing+=1;previous=None;continue
   near=fields[0x320]['value'];point=fields[0x338]['value'];raw=fields[0x398]['value'];hero=fields[0x398]['hero']
   # Grouped by worker time; reject Hero identity/value mismatch across the3 logs.
   if any(fields[o]['hero']!=hero for o in (0x320,0x338)):missing+=1;previous=None;continue
   if not all(finite(v) for v in (near,point,raw,hero)) or not .9<=length(raw)<=1.1 or max(abs(x) for v in (near,point,hero) for x in v)>=1e6:missing+=1;previous=None;continue
   n=unit(raw)
   if abs(n[1])>.35:missing+=1;previous=None;continue
   delta=sub(near,point);separation=dot(delta,n);residual=sub(delta,scale(n,separation));distance=dot(sub(hero,point),n)
   row={'session':s,'sample_ms':ms,'face':labels[s],'N398':n,'P320':near,'P338':point,'hero':hero,'normal_separation':separation,'tangent_error':length(residual),'plane_distance':distance,'dryrun_adhesion_correction':scale(n,.5-distance)}
   if first_n is None:first_n=n
   normal_stability.append(dot(first_n,n));row['pair_sample_pass']=.4<=separation<=.6 and row['tangent_error']<=.1
   if previous:
    dt=(ms-previous[0])/1000
    if .02<=dt<=.5:
     velocity=scale(sub(hero,previous[1]),1/dt);projected=sub(velocity,scale(n,dot(velocity,n)));forward=unit(projected) if length(projected)>=.1 else None
     row['velocity_source']='position_difference_NOT_physics_velocity';row['position_difference_velocity']=velocity
     row['surface_frame_valid']=bool(forward and row['pair_sample_pass'])
     if row['surface_frame_valid']:
      # cross(up,forward), evaluated independently of game code.
      right=unit((n[1]*forward[2]-n[2]*forward[1],n[2]*forward[0]-n[0]*forward[2],n[0]*forward[1]-n[1]*forward[0]));row['forward']=forward;row['right']=right
     tangent=unit((n[2],0.,-n[0]));speed=dot(velocity,tangent)
     positive+=speed>1;negative+=speed<-1
   previous=(ms,hero);rows.append(row);sample_rows.append(row)
  if not rows:continue
  normals=[r['N398'] for r in rows];crawl=pairs.get(s);face=labels[s];ideal=AXES.get(face,crawl);tl=timelines.get(s,{})
  life=lifecycle.get(s,{});life_ok=life.get('Init')=='1' and life.get('Entry')=='1' and life.get('Deinit')=='1' and int(life.get('Update','0'))>0 and life.get('end_reason')=='verified_deinit'
  # Missing root reads may have no sample line; check the per-field attempt counts.
  complete=not missing and all(summaries.get((s,o),{}).get('attempts')==str(len(rows)) for o in (0x320,0x338,0x398))
  separation_stats=stats(r['normal_separation'] for r in rows);tangent_stats=stats(r['tangent_error'] for r in rows);distance_stats=stats(r['plane_distance'] for r in rows)
  stable=min(normal_stability)>.98
  pair_ok=len(rows)>=3 and complete and stable and separation_stats['min']>=.4 and separation_stats['max']<=.6 and tangent_stats['mean']<=.1
  sign_ok=pair_ok and distance_stats['min']>0
  # A plane reference need not be a contact directly beneath the Hero.
  plane_ok=sign_ok and .4<=distance_stats['median']<=.6 and stats(abs(d-.5) for d in [r['plane_distance'] for r in rows])['p95']<=.1
  summary={'face':face,'subtype':tl.get('candidate','unknown'),'duration':float(tl.get('duration','0')),'lifecycle_verified':life_ok,'worker_samples':len(rows),'missing_or_invalid_batches':missing,'all_primary_attempts_accounted':complete,'first_normal':normals[0],'last_normal':normals[-1],'mean_normal':unit(tuple(sum(v[i] for v in normals) for i in range(3))),'min_dot_to_first':min(normal_stability),'normal_separation':separation_stats,'tangent_error':tangent_stats,'hero_plane_distance':distance_stats,'pair_sample_transients':sum(not r['pair_sample_pass'] for r in rows),'surface_pair_candidate_pass':pair_ok,'surface_plane_reference_candidate':plane_ok,'surface_sign_candidate':'OUTWARD_FROM_SURFACE' if sign_ok else 'UNRESOLVED','forward_samples':positive,'reverse_samples':negative,'movement_direction_invariant':bool(pair_ok and positive>=2 and negative>=2),'crawl_axis':crawl,'min_signed_dot_to_crawl':min(dot(n,crawl) for n in normals) if crawl else None,'mean_signed_dot_to_crawl':statistics.mean(dot(n,crawl) for n in normals) if crawl else None,'endpoint_signed_dot_to_declared_axis':dot(normals[-1],ideal) if ideal else None,'all_sample_min_signed_dot_to_declared_axis':min(dot(n,ideal) for n in normals) if ideal else None,'dryrun_frame_valid_count':sum(r.get('surface_frame_valid',False) for r in rows),'max_dryrun_correction':max(length(r['dryrun_adhesion_correction']) for r in rows),'physics_contact_verified':False}
  sessions[str(s)]=summary;all_distances.extend(r['plane_distance'] for r in rows)
 # Pairwise same-worker equality + bounded lag hypotheses, no across-session matching.
 for kind,offsets in [('normal_like',NORMALS),('point_like',POINTS)]:
  for a,b in itertools.combinations(offsets,2):
   differences=[];lag_values={k:[] for k in range(-2,3)};variation=[]
   for s in sorted(labels):
    series=[(f[a]['value'],f[b]['value']) for (sid,t),f in sorted(groups.items()) if sid==s and sid not in excluded and a in f and b in f and finite(f[a]['value']) and finite(f[b]['value'])]
    if not series:continue
    differences.extend(length(sub(x,y)) for x,y in series);variation.extend(length(sub(series[i][0],series[i-1][0])) for i in range(1,len(series)))
    for lag in lag_values:
     lag_values[lag].extend(length(sub(series[i][0],series[i+lag][1])) for i in range(len(series)) if 0<=i+lag<len(series))
   if not differences:continue
   scores={k:statistics.mean(v) for k,v in lag_values.items() if len(v)>=3};best=min(scores,key=scores.get) if scores else 0;informative=bool(variation and max(variation)>1e-3 and best!=0 and scores[best]<scores[0]*.8)
   duplicate.append({'kind':kind,'a':hex(a),'b':hex(b),'same_worker_difference':stats(differences),'numeric_duplicate_fraction_1e_5':sum(x<1e-5 for x in differences)/len(differences),'lag_mean_differences':scores,'best_lag_worker_samples':best,'lag_informative':informative,'interpretation':'numerical evidence only; copied/previous/current/filtered meaning needs producer evidence; flat signals do not establish lag'})
 primary=[int(x) for x in mapping.get('primary_sessions',[])];up=[int(x) for x in mapping.get('vertical_sessions',[])];control=[int(x) for x in mapping.get('control_sessions',[])];diagonal=[int(x) for x in mapping.get('diagonal_sessions',[])]
 def eligible(s):
  q=sessions.get(str(s),{});return q.get('lifecycle_verified') and q.get('surface_pair_candidate_pass') and q.get('surface_plane_reference_candidate') and q.get('min_signed_dot_to_crawl') is not None and q['min_signed_dot_to_crawl']>=.97 and q['mean_signed_dot_to_crawl']>=.985
 cardinal=[sessions.get(str(s),{}) for s in primary];byface={q.get('face'):q for q in cardinal}
 four=all(f in byface for f in AXES) and len(primary)>=4 and all(eligible(s) for s in primary)
 if four:four=statistics.mean(byface[f]['min_signed_dot_to_crawl'] for f in AXES)>=.985
 ac=dot(byface['A']['last_normal'],byface['C']['last_normal']) if 'A' in byface and 'C' in byface else None;bd=dot(byface['B']['last_normal'],byface['D']['last_normal']) if 'B' in byface and 'D' in byface else None
 opposite=bool(four and ac<=-.97 and bd<=-.97)
 side_a=[q for q in sessions.values() if q['face']=='A' and q['subtype']=='WallRunSide'];up_a=[sessions[str(s)] for s in up if str(s) in sessions and sessions[str(s)]['face']=='A' and sessions[str(s)]['subtype']=='WallRunUp']
 subtype_dots=[dot(x['last_normal'],y['last_normal']) for x in side_a for y in up_a];subtype=bool(len(up_a)>=2 and subtype_dots and min(subtype_dots)>=.97 and all(eligible(s) for s in up if str(s) in sessions and sessions[str(s)]['face']=='A'))
 baseline_evidence=bool(baseline and baseline.get('cardinal_evidence_pass'))
 e_rows=[sessions.get(str(s),{}) for s in diagonal];e_side=[q for q in e_rows if q.get('subtype')=='WallRunSide'];e_up=[q for q in e_rows if q.get('subtype')=='WallRunUp']
 diagonal_geometry=bool(mapping.get('diagonal_geometry_confirmed') and mapping.get('same_face_E_confirmed') and e_rows and all(q.get('face')=='E' and q.get('crawl_axis') and abs(q['crawl_axis'][0])>=.2 and abs(q['crawl_axis'][2])>=.2 and all(abs(n[0])>=.2 and abs(n[2])>=.2 for n in [q['first_normal'],q['last_normal']]) for q in e_rows))
 e_dots=[dot(x['last_normal'],y['last_normal']) for x in e_side for y in e_up];diagonal_ok=bool(diagonal_geometry and e_side and e_up and min(e_dots)>=.97 and all(eligible(s) for s in diagonal) and any(q['movement_direction_invariant'] for q in e_side))
 controls_ok=bool(control and all(eligible(s) and sessions[str(s)]['face']=='A' and sessions[str(s)]['subtype']=='WallRunSide' for s in control))
 authenticated=all(hashes.get(f) for f in ('Init','Entry','Update','Deinit')) and plan
 current_cardinal=bool(authenticated and four and opposite and subtype and any(q['movement_direction_invariant'] for q in side_a))
 ready=bool(authenticated and baseline_evidence and controls_ok and diagonal_ok)
 return {'input_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'ground_truth_source':mapping.get('source','explicit user mapping; paired R1 proxy, not engine geometry'),'excluded_sessions':excluded,'sessions':sessions,'cardinal_evidence_pass':current_cardinal,'endpoint_cardinal_mean_dot':statistics.mean(byface[f]['endpoint_signed_dot_to_declared_axis'] for f in AXES) if all(f in byface for f in AXES) else None,'endpoint_cardinal_min_dot':min(byface[f]['endpoint_signed_dot_to_declared_axis'] for f in AXES) if all(f in byface for f in AXES) else None,'opposite_A_C_endpoint_dot':ac,'opposite_B_D_endpoint_dot':bd,'subtype_invariance_A_signed_dots':subtype_dots,'measured_target_standoff':stats(all_distances),'runtime_dryrun_target_standoff':.5,'duplicate_fields':duplicate,'sample_rows':sample_rows,'gate':{'lifecycle_verified':bool(authenticated and sessions and all(q['lifecycle_verified'] for q in sessions.values())),'four_face_evidence':bool(current_cardinal or baseline_evidence),'surface_axis_verified':ready,'surface_sign_verified':ready,'plane_reference_verified':ready,'movement_direction_invariant':bool(current_cardinal or ready),'wallrun_subtype_invariant':bool(current_cardinal or ready),'diagonal_surface_verified':diagonal_ok,'physics_contact_verified':False,'v01b_gate':'READY_FOR_PLANAR_WALL_MVP' if ready else 'BLOCKED_PENDING_DIAGONAL_PAIR_VALIDATION','actor_write':False,'scope':'planar vertical wall only; no corner/curve/ceiling; no write is enabled even when offline gate is ready'}}
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('--face-map',type=Path,required=True);p.add_argument('--baseline',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=review(a.log,json.loads(a.face_map.read_text()),json.loads(a.baseline.read_text()) if a.baseline else None);a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS:',len(r['sessions']),'sessions;',r['gate']['v01b_gate'],'median standoff=',r['measured_target_standoff']['median'] if r['measured_target_standoff'] else None)
