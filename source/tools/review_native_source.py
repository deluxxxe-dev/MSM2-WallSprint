"""Read logs only. A native source test pass requires observed retained Actor Up,
never just an applied matrix. Synthetic tests cannot become game evidence.
"""
from pathlib import Path
import argparse,re,json,hashlib,math
from collections import defaultdict
def fields(line):return dict(re.findall(r'(\w+)=([^ ]+)',line))
def vec(s):return tuple(map(float,s.strip('()').split(',')))
def dot(a,b):return sum(x*y for x,y in zip(a,b))
def proper(rows):
 if any(not all(math.isfinite(x) for x in v) for v in rows):return False
 if any(abs(dot(v,v)-1)>.02 for v in rows):return False
 if any(abs(dot(rows[i],rows[j]))>.02 for i,j in [(0,1),(0,2),(1,2)]):return False
 a,b,c=rows;cross=(a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]);return dot(cross,c)>.98

def review(path):
 lines=path.read_text(errors='replace').splitlines();sessions=defaultdict(lambda:{'writes':0,'completed':False,'held_updates':0,'held_ms':0,'max_held_updates':0,'max_held_ms':0,'last_update':None,'first_good_ms':None,'violations':[],'exits':[],'other_writers':[],'entry_rejects':[],'retention_observed':False})
 for line in lines:
  if not any(x in line for x in ['NATIVE_SOURCE_OVERRIDE ','NATIVE_SOURCE_ACTOR_OBSERVATION ','NATIVE_SOURCE_EXIT ','NATIVE_SOURCE_OTHER_WRITER ','NATIVE_SOURCE_ENTRY_REJECT ']):continue
  d=fields(line);s=sessions[d['session']]
  if 'NATIVE_SOURCE_OVERRIDE ' in line:
   s['writes']+=1
   good=d.get('position_unchanged')=='1' and d.get('source_is_stack_local')=='1' and d.get('direct_actor_transform_write')=='0' and int(d.get('caller_rva','0'),16)==0x2cfc8f3 and float(d['elapsed'])>=.7
   rows=[vec(d[k]) for k in ['applied_right','applied_up','applied_forward']];target=[vec(d[k]) for k in ['target_right','target_up','target_forward']]
   if not good or not proper(rows) or not proper(target):s['violations'].append('write_admission_or_matrix_evidence_failed')
   t=float(d['t']);
   if t>=1:s['completed']=True
   if t<0 or t>1:s['violations'].append('bad_transition_parameter')
  if 'NATIVE_SOURCE_ACTOR_OBSERVATION ' in line and d.get('observation_phase') in ('next_matching_native_PRE','verified_native_update_worker') and s['completed']:
   update=int(d['native_update']);now=int(d['native_ms']);up=vec(d['actor_up']);length=math.sqrt(dot(up,up));measured=float(d['actor_up_dot_surface'])
   if update==s['last_update']:continue
   if s['last_update'] is not None and update<s['last_update']:s['violations'].append('native_update_reversed')
   s['last_update']=update
   if d.get('lifecycle_active')=='1' and .95<=length<=1.05 and math.isfinite(measured) and measured>=.98:
    if s['first_good_ms'] is None:s['first_good_ms']=now
    s['held_updates']+=1;s['held_ms']=now-s['first_good_ms']
    s['max_held_updates']=max(s['max_held_updates'],s['held_updates']);s['max_held_ms']=max(s['max_held_ms'],s['held_ms'])
   else:s['held_updates']=0;s['held_ms']=0;s['first_good_ms']=None
   s['retention_observed']|=s['max_held_updates']>=20 or s['max_held_ms']>=300
  if 'NATIVE_SOURCE_EXIT ' in line:s['exits'].append(d)
  if 'NATIVE_SOURCE_OTHER_WRITER ' in line:s['other_writers'].append(d)
  if 'NATIVE_SOURCE_ENTRY_REJECT ' in line:s['entry_rejects'].append(d)
 target='c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9'
 actual=any('WallSprint v0.1b5 ' in x for x in lines) and any('target_build_gate=ACCEPTED' in x for x in lines) and any('exe_file_sha256='+target in x for x in lines) and any('native_source_config ' in x and 'admitted=1' in x for x in lines)
 clean_exit=any('WallSprint exited reason=TelemetryStopped' in x for x in lines)
 for s in sessions.values():
  s['failure_exits']=[x['reason'] for x in s['exits'] if any(bad in x.get('reason','') for bad in ['A_or_B','B_returned','invalid','identity_changed','ownership_lost','write_failed','admission_lost'])]
  s['scope_retention_pass']=s['writes']>0 and s['completed'] and s['retention_observed'] and not s['violations'] and not s['failure_exits']
  s['game_retention_observed']=actual and s['scope_retention_pass']
 return {'log_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'actual_game_capture':actual,'clean_telemetry_exit':clean_exit,'sessions':dict(sessions),'v01b5_orientation_retention_observed':actual and sum(s['game_retention_observed'] for s in sessions.values())>=2,'v01b5_game_pass_observed':False,'video_no_crash_and_position_review_pending':True,'adhesion_permission':False,'camera_permission':False,'direct_actor_write_permission':False,'limitations':['Readback of source alone does not prove orientation retention.','Only distinct next-matching-native-PRE samples count; worker snapshots are supplementary.','Game/video review must confirm A/E mapping, no crash/corruption and sane native position; normal process exit may omit script-disable footer.','No automatic next-phase implementation.']}
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=review(a.log);a.output.write_text(json.dumps(r,indent=2,allow_nan=False)+'\n');print('native source sessions',len(r['sessions']),'actual_game',r['actual_game_capture'],'game retention',r['v01b5_game_pass_observed'])
