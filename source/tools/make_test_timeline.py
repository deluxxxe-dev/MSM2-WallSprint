"""Video alignment aid. UTC is not video-relative time; wall labels require video."""
from pathlib import Path
import argparse,re
p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('--output',type=Path,default=Path('TEST_TIMELINE.txt'));a=p.parse_args()
rows=[];fallback={}
for line in a.log.read_text(errors='replace').splitlines():
 utc=line.split(' ',1)[0];d=dict(re.findall(r'(\w+)=([^ ]+)',line))
 if 'WALLRUN_HOOK Init ' in line:fallback[int(d['wallrun_session'])]={'start':utc}
 if 'WALLRUN_HOOK Deinit ' in line:fallback.setdefault(int(d['wallrun_session']),{})['end']=utc
 if 'WallRun verified session #' in line:
  session=int(re.search(r'session #(\d+)',line)[1]);fallback.setdefault(session,{}).update(duration=d['duration'],owner=d['this'])
 if 'session_timeline session_id=' in line:
  session=int(d['session_id']);fallback.pop(session,None)
  rows.append((d['start_utc'],f"Session {session}: {d['start_utc']} - {d['end_utc']}\n  duration={d['duration']}s candidate={d['candidate']} owner={d['owner']}\n  start={d['start_position']} mean={d['mean_position']} end={d['end_position']}\n  event UTC=estimated; positions=worker snapshots; source=position_difference_heuristic"))
 if 'session_timeline_axes ' in line:
  rows.append((utc,f"Session {d['session_id']} {d['phase']} axes: right={d['actor_right']} up={d['actor_up']} forward={d['actor_forward']} valid={d['valid']}"))
 if 'surface_calibration_pair ' in line:rows.append((utc,f"Crawl calibration paired to session {d['wallrun_session']}: {utc}\n  delay={d['deinit_to_crawl_ms']}ms axis={d['crawl_axis']} (temporal pairing, verify same wall in video)"))
 if 'surface_calibration_crawl_position ' in line:rows.append((utc,f"Crawl position for session {d['wallrun_session']}: {d['crawl_detected_position']}"))
 if 'crawl_like_orientation_sample ' in line:rows.append((utc,f"Crawl-like orientation: {utc} axis={d['crawl_axis']} position={d['position']} (unpaired proxy; native crawl state unknown)"))
 if 'idle_like_interval ' in line:rows.append((utc,f"Idle-like interval: {utc} duration>={d['duration_ms']}ms position={d['position']} (Ground state unverified)"))
for session,d in fallback.items():
 if 'start' in d:rows.append((d['start'],f"Session {session}: {d['start']} - {d.get('end','unavailable')}\n  duration={d.get('duration','unavailable')} owner={d.get('owner','unavailable')} (v0.1a5 fallback: worker delivery UTC; no session mean position)"))
rows.sort(key=lambda x:x[0]);text='WallSprint TEST TIMELINE\nUTC alignment aid; video starts on its own clock. Match initial idle and actions.\nWall A/B/C and same-wall identity are NOT inferred automatically.\n\n'+'\n\n'.join(t for _,t in rows)+'\n'
a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(text);print(f'Wrote {a.output} ({len(rows)} timeline items)')
