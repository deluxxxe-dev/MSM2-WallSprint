"""Read-only b8 log gate. Native proof and human A/E mapping remain required."""
from pathlib import Path
import argparse, hashlib, json, math, re
COPY_HASH='ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196'
EXE_HASH='c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9'
def fields(line):return dict(re.findall(r'(\w+)=([^ ]+)',line))
def vector(text):return tuple(float(x) for x in text.strip('()').split(','))
def layout(entry):
    try:
        rows=[vector(entry[k]) for k in ('source_right','source_up','source_forward')]
        pos=vector(entry['source_position'])
        if not all(len(v)==3 and all(math.isfinite(x) for x in v) for v in rows+[pos]):return False
        dot=lambda a,b:sum(x*y for x,y in zip(a,b))
        right,up,forward=rows
        cross=(right[1]*up[2]-right[2]*up[1],right[2]*up[0]-right[0]*up[2],right[0]*up[1]-right[1]*up[0])
        p,rsp,lo,hi=(int(entry[k],16) for k in ('RDX','RSP','stack_limit','stack_base'))
        return all(abs(dot(v,v)-1)<.01 for v in rows) and all(abs(dot(rows[i],rows[j]))<.01 for i,j in ((0,1),(0,2),(1,2))) and dot(cross,forward)>.99 and max(map(abs,pos))<1e6 and p%16==0 and lo<=rsp and rsp+0x28<=p and p+64<=min(hi,rsp+0x400) and entry.get('layout_valid')=='1' and entry.get('source_is_stack_local')=='1' and int(entry['caller_rva'],16)==0x2cfc84b and float(entry['elapsed'])>=.70
    except (ValueError,KeyError,OverflowError):return False

def review(path,a_session=None,e_session=None):
    text=path.read_text(errors='replace');lines=text.splitlines()
    actual=('WallSprint v0.1b8 ' in text and 'target_build_gate=ACCEPTED' in text and 'exe_file_sha256='+EXE_HASH in text and 'TEST_ONLY_SYNTHETIC' not in text)
    hooks=[fields(l) for l in lines if 'RESET_COPY_HOOK ' in l]
    hook_ok=any(d.get('admitted')=='1' and d.get('installed')=='1' and d.get('original_sha256')==COPY_HASH and d.get('writer_rva')=='0x2cfc3c0' and d.get('size')=='444' and d.get('direct_call_verified')=='1' for d in hooks)
    admission=any(d.get('admitted')=='1' and d.get('dual_source_required')=='1' and d.get('copy_hook_authenticated')=='1' for d in (fields(l) for l in lines if 'DUAL_SOURCE_ADMISSION ' in l))
    sessions={};entries={}
    for line in lines:
        d=fields(line);sid=d.get('session')
        if sid is None:continue
        s=sessions.setdefault(sid,{'common_overrides':0,'copy_overrides':0,'copy_layout_entries':0,'retention_pass':False,'held_ms':0,'held_updates':0,'new_resets':[],'retention_lost_after_pass':False,'targets':set(),'violations':[]})
        try:
            if 'COMMON_SOURCE_OVERRIDE ' in line:
                if int(d['caller_rva'],16)==0x2cfc8f3 and float(d['elapsed'])>=.70 and d.get('position_unchanged')=='1':
                    s['common_overrides']+=1;s['targets'].add(d['target_transform'])
                else:s['violations'].append('common_source_gate')
            if 'RESET_COPY_SOURCE_ENTRY ' in line:
                entries[(sid,d.get('RDX'),d.get('elapsed'))]=d
                if layout(d):s['copy_layout_entries']+=1
            if 'RESET_COPY_SOURCE_OVERRIDE ' in line:
                entry=entries.get((sid,d.get('source_ptr'),d.get('elapsed')), {})
                if layout(entry) and all(d.get(k)=='1' for k in ('position_unchanged','caller_verified','target_verified','source_stack_verified')):
                    s['copy_overrides']+=1;s['targets'].add(entry['RCX'])
                else:s['violations'].append('copy_source_gate_or_entry')
            if 'ORIENTATION_RETENTION_PASS=1 ' in line:
                s['held_ms']=max(s['held_ms'],int(d['held_ms']));s['held_updates']=max(s['held_updates'],int(d['held_updates']))
                s['retention_pass']|=int(d['held_ms'])>=300 or int(d['held_updates'])>=20
            if 'NATIVE_SOURCE_ACTOR_OBSERVATION ' in line and s['retention_pass'] and d.get('observation_phase')=='next_matching_native_PRE' and float(d.get('actor_up_dot_surface','0'))<.98:s['retention_lost_after_pass']=True
            if 'NEW_RESET_AFTER_DUAL_OVERRIDE ' in line:s['new_resets'].append(d)
        except (ValueError,KeyError,OverflowError):s['violations'].append('malformed_evidence')
    for s in sessions.values():
        s['targets']=sorted(s['targets']);s['minimum_source_evidence']=bool(s['common_overrides'] and s['copy_overrides'] and s['copy_layout_entries'] and len(s['targets'])==1 and not s['violations'])
        s['orientation_retained']=bool(s['minimum_source_evidence'] and s['retention_pass'])
        s['clean_observed_window']=s['orientation_retained'] and not s['new_resets'] and not s['retention_lost_after_pass']
    actual=bool(actual and hook_ok and admission)
    mapped=a_session is not None and e_session is not None and a_session!=e_session
    strong=bool(actual and mapped and all(sessions.get(str(s),{}).get('clean_observed_window') for s in (a_session,e_session)))
    return {'log_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'actual_game_capture':actual,'copy_hook_authenticated':hook_ok,'dual_source_admitted':admission,'sessions':sessions,'confirmed_A_E_sessions':{'A':a_session,'E':e_session},'ORIENTATION_OWNERSHIP_VERIFIED':bool(actual and any(s['orientation_retained'] for s in sessions.values())),'STRONG_A_E_VERIFIED':strong,'adhesion_permission':False,'direct_actor_write_permission':False,'limitations':['Runtime PASS must come from active lifecycle observations after shared transition completion.','Raw-entry caller-local proof is required; a stale common-writer entry cannot prove copy source.','Hardware watch is bounded and primary-only; no-event does not prove absence of other writers.','Review real A/E mapping, no-crash and Position behavior before any later milestone.']}
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('--a-session',type=int);p.add_argument('--e-session',type=int);p.add_argument('--output',required=True,type=Path);a=p.parse_args();r=review(a.log,a.a_session,a.e_session);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(r,indent=2,allow_nan=False)+'\n');print('PASS: review completed actual_game=',r['actual_game_capture'],'ownership_verified=',r['ORIENTATION_OWNERSHIP_VERIFIED'],'strong_A_E=',r['STRONG_A_E_VERIFIED'])
