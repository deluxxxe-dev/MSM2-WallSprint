"""Offline b6 review. Real A/E repetitions + anchored store/source required.
Synthetic fixtures can test decoding, never promote a game reset writer.
"""
from pathlib import Path
import argparse, hashlib, json, math
from review_orientation_writers import review as decode, fields

def review(path, a_session=None, e_session=None):
    text=path.read_text(errors='replace'); lines=text.splitlines(); base=decode(path)
    actual=all(any(value in line for line in lines) for value in (
        'WallSprint v0.1b6 ', 'target_build_gate=ACCEPTED',
        'exe_file_sha256=c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9',
        'RESET_TRACE_CONFIG admitted=1 ')) and 'TEST_ONLY_SYNTHETIC' not in text
    resolutions={(e['session'],e['thread_id'],e['qpc_ms'],e['rip_after']):e for e in base['event_resolution']}
    entries={}; candidates=[]; windows=[]
    for line in lines:
        d=fields(line)
        if 'RESET_ENTRY_SOURCE ' in line: entries[(d['session'],d['entry_id'])]=d
        if 'RESET_TRACE_WINDOW ' in line: windows.append(d)
    def vector(value):return tuple(float(v) for v in value.strip('()').split(','))
    def unit(v):
        length=math.sqrt(sum(x*x for x in v))
        return tuple(x/length for x in v) if len(v)==3 and all(math.isfinite(x) for x in v) and .9<length<1.1 else None
    def dot(a,b):return sum(x*y for x,y in zip(a,b))
    for line in lines:
        if 'RESET_TRACE_EVENT ' not in line:continue
        d=fields(line); before=unit(vector(d['up_before'])); after=unit(vector(d['up_after'])); normal=unit(vector(d['N398']))
        if not(before and after and normal and dot(before,normal)>=.5 and after[1]>=.98):continue
        resolved=resolutions.get((d['session'],d['thread_id'],d['qpc_ms'],d['rip']),{})
        entry=entries.get((d['session'],d['entry_id']),{}); source_ok=False
        if entry.get('readable')=='1' and entry.get('entry_correlated')=='1':
            raw=bytes.fromhex(entry['source_0x40'])
            import struct
            up=unit(struct.unpack('<3f',raw[16:28])) if len(raw)==64 else None
            source_ok=bool(up and up[1]>=.98 and int(entry['RCX'],16)==int(d['transform'],16) and unit(vector(entry['source_up'])) and dot(up,unit(vector(entry['source_up'])))>=.999999)
        evidence=bool(int(d.get('native_update','0'))>0 and d.get('read_valid')=='1' and d.get('target_verified_by_watch')=='1' and d.get('entry_correlated')=='1' and d.get('dropped')=='0' and 0<=float(d['relative_us_from_override'])<=30000 and int(d['caller_rva'],16)>0 and resolved.get('resolved') and source_ok)
        evidence=evidence and int(d['writer_function_rva'],16)==0x2cfc580 and resolved.get('function_capture',{}).get('sha256')=='3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6'
        first=float(d['first_surface_observed_qpc_ms']); now=float(d['qpc_ms'])
        evidence=evidence and 0<first<=now
        face='A' if dot(normal,(0,0,1))>=.98 else 'E' if dot(normal,(-2**-.5,0,2**-.5))>=.98 else 'other'
        candidates.append({'session':d['session'],'face_axis':face,'writer_function_rva':d['writer_function_rva'],'caller_rva':d['caller_rva'],'thread_id':d['thread_id'],'reset_latency_ms':float(d['relative_us_from_override'])/1000,'surface_to_reset_observed_ms':now-first,'native_update':d['native_update'],'source_up':entry.get('source_up'),'source_overridden_this_entry':entry.get('source_overridden_this_entry'),'source_entry_verified':source_ok,'instruction':resolved.get('resolved'),'function':resolved.get('function_capture'),'complete_candidate_evidence':bool(evidence)})
    def face_candidates(session,face):return [c for c in candidates if str(session)==c['session'] and c['face_axis']==face and c['complete_candidate_evidence']]
    a=face_candidates(a_session,'A'); e=face_candidates(e_session,'E')
    matching=[{'writer_function_rva':x['writer_function_rva'],'caller_rva':x['caller_rva']} for x in a for y in e if (x['writer_function_rva'],x['caller_rva'])==(y['writer_function_rva'],y['caller_rva'])]
    return {'log_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'actual_game_capture':actual,'confirmed_face_sessions':{'A':a_session,'E':e_session},'windows':windows,'reset_candidates':candidates,'same_A_E_writer_caller':matching,'RESET_WRITER_VERIFIED':bool(actual and matching),'success_E':bool(actual and e),'reset_patch_permission':False,'adhesion_permission':False,'limitations':['Old Up is previous watched/arm observation, not atomic instruction-before.','Current-thread enumeration is bounded and excludes helper/new threads; no-event is missing evidence.','Deadline is 30ms from committed source write; arm latency is reported; OS scheduling can delay cleanup.','Source/RSI/caller association is limited to the authenticated known common writer; unknown writers remain candidates.','A/E session mapping must be explicitly confirmed; runtime never self-promotes from synthetic events.']}

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('--a-session',type=int);p.add_argument('--e-session',type=int);p.add_argument('--output',required=True,type=Path);args=p.parse_args()
    result=review(args.log,args.a_session,args.e_session);args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(result,indent=2,allow_nan=False)+'\n');print('PASS: reset candidates',len(result['reset_candidates']),'actual_game',result['actual_game_capture'],'RESET_WRITER_VERIFIED',result['RESET_WRITER_VERIFIED'])
