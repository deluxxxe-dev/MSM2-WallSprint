"""v0.1a8 targeted runtime fields; explicit user session map, never auto-verify."""
from pathlib import Path
import argparse, json, math, re
AXES={'A':(0.,0.,1.),'B':(1.,0.,0.),'C':(0.,0.,-1.),'D':(-1.,0.,0.)}
def vector(s):return tuple(float(x) for x in s.strip('()').split(','))
def dot(a,b):return sum(x*y for x,y in zip(a,b))
def unit(v):
    n=math.sqrt(dot(v,v))
    return tuple(x/n for x in v) if math.isfinite(n) and n>1e-8 else None

def review(log,mapping):
    records={};pairs={};summaries={};timelines={};contacts=[];excluded=dict(mapping.get('excluded',{}));labels={int(k):v for k,v in mapping['sessions'].items()}
    for line in log.read_text(errors='replace').splitlines():
        d=dict(re.findall(r'(\w+)=([^ ]+)',line))
        if 'session_timeline ' in line:
            s=int(d['session_id']);timelines[s]=d
            if float(d['duration'])<.1:excluded[str(s)]='too_short_transition_or_failed_attach'
        if 'static_field_sample ' in line:records.setdefault((int(d['session']),d['scope'],d['offset']),[]).append(d)
        if 'static_field_summary ' in line:summaries[(int(d['session']),d['scope'],d['offset'])]=d
        if 'static_field_calibration_pair ' in line:pairs[int(d['wallrun_session'])]=vector(d['crawl_axis'])
        if 'static_field_point_score ' in line:contacts.append(d)
        if 'static_field_session_excluded ' in line:excluded[d['session']]=d['excluded_reason']
    primary=set(mapping.get('primary_sessions',[s for s in labels if s not in mapping.get('vertical_sessions',[])]));vertical=set(mapping.get('vertical_sessions',[]))
    fields={};child_neighborhoods={}
    for (s,scope,off),rs in records.items():
        if scope=='child128':
            child_neighborhoods.setdefault(off,[]).extend(rs);continue
        if s not in labels or str(s) in excluded:continue
        vectors=[vector(r['value']) for r in rs];norms=[math.sqrt(dot(v,v)) for v in vectors];vs=[unit(v) for v,n in zip(vectors,norms) if .9<=n<=1.1]
        summary=summaries.get((s,scope,off),{});all_valid=len(vs)==len(vectors) and summary.get('attempts')==str(len(vectors))
        if scope=='child128' and summary.get('child_pointer_changed')!='0':all_valid=False
        stable=len(vs)>=2 and all_valid and min(dot(v,vs[0]) for v in vs)>.98
        axis=pairs.get(s);ideal=AXES[labels[s]]
        entry={'session':s,'face':labels[s],'subtype':timelines.get(s,{}).get('candidate','unconfirmed'),'observations':len(rs),'stable':stable,'crawl_axis':axis,'mean_vector':unit(tuple(sum(v[i] for v in vs) for i in range(3))) if vs else None,'min_abs_dot_to_crawl':min(abs(dot(v,axis)) for v in vs) if vs and axis else None,'min_abs_dot_to_declared_axis':min(abs(dot(v,ideal)) for v in vs) if vs else None,'signed_dot_to_crawl':dot(vs[-1],axis) if vs and axis else None,'all_signed_positive':bool(vs and axis and all(dot(v,axis)>=.97 for v in vs)),'all_signed_negative':bool(vs and axis and all(dot(v,axis)<=-.97 for v in vs))}
        fields.setdefault(f'{scope}+{off}',[]).append(entry)
    scores={}
    for path,rows in fields.items():
        fs={f:[r for r in rows if r['session'] in primary and r['face']==f] for f in AXES}
        present=all(fs.values());complete=present and all(r['min_abs_dot_to_crawl'] is not None and r['mean_vector'] is not None for rs in fs.values() for r in rs)
        all_means=[r['mean_vector'] for r in rows if r['session'] in primary and r['mean_vector']]
        constant=bool(len(all_means)>=2 and max(math.sqrt(sum((x-y)**2 for x,y in zip(a,b))) for a in all_means for b in all_means)<.0001)
        score={'constant_world_axis_status':'CONSTANT_WORLD_AXIS_SUSPECT' if constant else 'UNRESOLVED','complete_four_faces':complete,'surface_verified':False,'contact_verified':False}
        if complete:
            mins={f:min(r['min_abs_dot_to_crawl'] for r in rs) for f,rs in fs.items()};means={f:unit(tuple(sum(r['mean_vector'][i] for r in rs) for i in range(3))) for f,rs in fs.items()}
            if all(means.values()):
                ac=dot(means['A'],means['C']);bd=dot(means['B'],means['D']);ab=abs(dot(means['A'],means['B']));m=sum(mins.values())/4;mi=min(mins.values());stable=all(r['stable'] for rs in fs.values() for r in rs)
                signed=all(r['all_signed_positive'] for rs in fs.values() for r in rs) or all(r['all_signed_negative'] for rs in fs.values() for r in rs)
                strong=stable and m>=.985 and mi>=.97 and abs(ac)>=.97 and abs(bd)>=.97 and ab<.3
                score.update(face_min_abs_dot=mins,mean_abs_dot=m,min_abs_dot=mi,opposite_A_C_dot=ac,opposite_B_D_dot=bd,between_wall_rotation=ab<.3,within_session_stable=stable,strong_axis_candidate=strong,opposite_face_consistent=strong and ac<=-.97 and bd<=-.97 and signed,signed_pattern='same_as_crawl_axes' if all(r['all_signed_positive'] for rs in fs.values() for r in rs) else 'opposite_to_crawl_axes' if all(r['all_signed_negative'] for rs in fs.values() for r in rs) else 'mixed_or_mismatch')
        a=[r for r in rows if r['session'] in primary and r['face']=='A' and r['mean_vector']];ups=[r for r in rows if r['session'] in vertical and r['face']=='A' and r['mean_vector']]
        if a and len(ups)>=2:
            signed_dots=[dot(x['mean_vector'],y['mean_vector']) for x in a for y in ups];invariant=min(abs(d) for d in signed_dots)
            # Require stable + actual ground-truth agreement for both subtypes;
            # a fixed unrelated vector must not pass merely by being invariant.
            supported=all(r['stable'] and r['min_abs_dot_to_crawl'] is not None and r['min_abs_dot_to_crawl']>=.97 for r in a+ups)
            score.update(subtype_invariance_score=invariant,subtype_signed_dots=signed_dots,subtype_invariant_on_A=supported and invariant>=.97,vertical_sessions=[r['session'] for r in ups])
        else:score.update(subtype_invariance_score=None,subtype_invariant_on_A=False)
        scores[path]=score
    return {'primary_label_source':'explicit user mapping; post-Deinit R1 crawl-like proxy NOT engine geometry','excluded_sessions':excluded,'primary_sessions':sorted(primary),'vertical_sessions':sorted(vertical),'child_pointer_neighborhood_records_NOT_vector_fields':child_neighborhoods,'field_records':fields,'field_scores':scores,'point_candidate_records':contacts,'v01b_gate':'BLOCKED: numeric candidates require native semantics/sign and verified contact/equivalent distance','surface_axis_verified':False,'contact_verified':False,'actor_write':False}
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('--face-map',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=review(a.log,json.loads(a.face_map.read_text()));a.output.write_text(json.dumps(r,indent=2)+'\n');print(f"PASS: {len(r['field_scores'])} statically selected field paths reviewed; gate BLOCKED")
