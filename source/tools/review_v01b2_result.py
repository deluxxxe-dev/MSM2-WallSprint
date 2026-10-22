import re,json,math,hashlib,collections,argparse
from pathlib import Path
ap=argparse.ArgumentParser();ap.add_argument('log',type=Path);ap.add_argument('--output',type=Path,required=True);args=ap.parse_args();p=args.log;rows=p.read_text().splitlines(); pre={};post={};postUp={};writes={};surv=[];sessions=[];exits=[];entry={}
vec=lambda t:tuple(map(float,t.split(',')))
def angle(a,b):
 scale=math.sqrt(sum(x*x for x in a)*sum(x*x for x in b));return math.degrees(math.acos(max(-1,min(1,sum(x*y for x,y in zip(a,b))/scale))))
for line in rows:
 m=re.search(r'WALLRUN_UPDATE_PRE sequence=(\d+) wallrun_session=(\d+).*qpc_ms=([\d.]+).*actor_basis_R_U_F=\([^)]*\)/\(([^)]*)\)/.*N398=\(([^)]*)\)',line)
 if m:pre[int(m[1])]={'session':int(m[2]),'ms':float(m[3]),'up':vec(m[4]),'normal':vec(m[5])}
 m=re.search(r'WALLRUN_UPDATE_POST_NATIVE sequence=(\d+).*basis_changed_by_native=(\d+)',line)
 if m:
  post[int(m[1])]=int(m[2]);u=re.search(r'actor_basis_R_U_F=\([^)]*\)/\(([^)]*)\)/',line)
  if u:postUp[int(m[1])]=vec(u[1])
 m=re.search(r'wallsprint_enter=1 sequence=(\d+)',line)
 if m:entry[pre[int(m[1])]['session']]=pre[int(m[1])]['up']
 m=re.search(r'WALLSPRINT_POST_WRITE sequence=(\d+) qpc_ms=([\d.]+).*target_basis_R_U_F=\([^)]*\)/\(([^)]*)\)/.*write_verified_immediate=(\d+) orientation_dot_after_write=([\d.]+).*actorUp_dot_N398=([-\d.]+)',line)
 if m:
  k=int(m[1]);u=vec(m[3]);e=entry[pre[k]['session']];a=angle(u,e);writes[k]={'session':pre[k]['session'],'ms':float(m[2]),'verified':int(m[4]),'dot':float(m[5]),'normal_dot':float(m[6]),'requested_degrees':a}
 m=re.search(r'WALLSPRINT_WRITE_SURVIVAL sequence=(\d+) previous_sequence=(\d+).*survived_until_next_update=(\d+).*target_dot=([-\d.]+)',line)
 if m:
  k,j=int(m[1]),int(m[2]);w=writes.get(j);surv.append({'session':pre[k]['session'],'sequence':k,'previous':j,'survived':int(m[3]),'dot':float(m[4]),'requested_degrees':w['requested_degrees'] if w else None,'gap_ms':pre[k]['ms']-w['ms'] if w else None})
 m=re.search(r'WallRun verified session #(\d+).*duration=([\d.]+)s.*end_reason=(\S+)',line)
 if m:sessions.append({'id':int(m[1]),'duration':float(m[2]),'end_reason':m[3]})
 m=re.search(r'wallsprint_exit reason=(\S+) duration=([\d.]+)',line)
 if m:exits.append({'reason':m[1],'duration':float(m[2])})
upAngles=[angle(pre[k]['up'],u) for k,u in postUp.items()]
summary={'native_Up_change_over_0_1deg':sum(a>.1 for a in upAngles),'max_native_Up_angle_degrees':max(upAngles,default=None),'input_sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'sessions':sessions,'pre_count':len(pre),'post_count':len(post),'post_basis_changed':sum(post.values()),'writes':len(writes),'immediate_verified':sum(w['verified'] for w in writes.values()),'survival_count':len(surv),'survival_success_raw':sum(s['survived'] for s in surv),'ownership_eligible_count':sum(s['requested_degrees'] is not None and s['requested_degrees']>=5 for s in surv),'eligible_survival_success':sum(s['survived'] for s in surv if s['requested_degrees'] is not None and s['requested_degrees']>=5),'excluded_small_rotation_count':sum(s['requested_degrees'] is not None and s['requested_degrees']<5 for s in surv),'exits':exits,'per_session':[]}
for s in sessions:
 k=s['id']; ss=[v for v in surv if v['session']==k and v['requested_degrees']>=5]; pp=[v for v in pre.values() if v['session']==k]; ww=[v for v in writes.values() if v['session']==k]
 summary['per_session'].append({**s,'normal_first':pp[0]['normal'] if pp else None,'writes':len(ww),'eligible':len(ss),'survived':sum(v['survived'] for v in ss),'gap_ms_min':min([v['gap_ms'] for v in ss],default=None),'gap_ms_max':max([v['gap_ms'] for v in ss],default=None),'max_requested_degrees':max([v['requested_degrees'] for v in ww],default=None)})
args.output.write_text(json.dumps(summary,indent=2)+'\n');print("PASS: reviewed",len(sessions),"sessions,",len(writes),"writes; eligible survival",summary["eligible_survival_success"],"/",summary["ownership_eligible_count"],"; Up changes",summary["native_Up_change_over_0_1deg"])
