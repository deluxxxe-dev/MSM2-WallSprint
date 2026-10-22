from pathlib import Path
import hashlib,math,re,struct,tempfile
from review_orientation_source import review,inspect,slice_arguments
from capstone import Cs,CS_ARCH_X86,CS_MODE_64
ROOT=Path(__file__).resolve().parents[1]
raw=b'\x90'*63+b'\xc3' # Independent synthetic body, no game bytes.
lines=[]
def function(label,start,code):
 lines.append(f'orientation_source_function label={label} available=1 begin_rva={start:#x} end_rva={start+len(code):#x} size={len(code)} sha256={hashlib.sha256(code).hexdigest()}')
 for off in range(0,len(code),64):lines.append(f'orientation_source_code label={label} offset={off:#x} bytes={code[off:off+64].hex()}')
function('writer',0x2cfc580,raw)
# Synthetic caller is NOT a game body. It provides a real direct call/slice fixture.
start=0x2cfc8e7;prep=bytes.fromhex('488b9380000000');call=start+len(prep);target=0x2cfc580
code=prep+b'\xe8'+struct.pack('<i',target-call-5)+b'\xc3';function('caller_fixture',start,code)
idx=0
for face,session,normal in [('A',1,(0.,0.,1.)),('E',2,(-math.sqrt(.5),0.,math.sqrt(.5)))]:
 for state in ['WallRun','CrawlCandidate']:
  up=(0.,1.,0.) if state=='WallRun' else normal
  forward=(0.,0.,1.) if state=='WallRun' else (0.,1.,0.)
  right=(up[1]*forward[2]-up[2]*forward[1],up[2]*forward[0]-up[0]*forward[2],up[0]*forward[1]-up[1]*forward[0])
  floats=[0.]*32;floats[0:3]=right;floats[4:7]=up;floats[8:11]=forward;floats[12:16]=[1000.,100.,1000.,1.];blob=struct.pack('<32f',*floats)
  for n in range(4):
   idx+=1;caller='WALLRUN_CALLER' if state=='WallRun' else 'CRAWL_SURFACE_CALLER';rva=0x2cfc8f3 if state=='WallRun' else 0x1021c9e
   lines.append(f'ORIENTATION_WRITER_ENTRY event={idx} window=1 state={state} session={session} thread_id=1 writer_rva=0x2cfc580 caller_rva={rva:#x} caller_class={caller} RCX=0x100000 RDX=0x200000 R8=0x0 R9=0x0 target_transform=0x100000 RCX_matches_target=1 N398=({normal[0]},{normal[1]},{normal[2]})')
   lines.append(f'ORIENTATION_SOURCE_SNAPSHOT event={idx} argument=RDX pointer=0x200000 readable=1 size=128 bytes={blob.hex()} one_level=1')
text='\n'.join(lines)+'\n'
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'source.log';p.write_text(text);r=review(p)
 assert r['entries']==16 and r['source_candidate_identified'] and not r['actual_game_capture'] and not r['v01b4_pass'] and not r['actor_write_permission']
 row=next(x for x in r['source_candidates'] if x['argument']=='RDX' and x['offset']=='0x10');assert row['source_candidate_identified'];assert all(x['mean_signed_dot']>.9999 for x in row['metrics'].values())
 assert r['functions']['writer']['decoded_coverage']==1 and r['functions']['writer']['hash_verified']
 trace=r['caller_slices'][0]['arguments']['rdx'];assert 'rdx' in trace['backward_local_trace'][0]['defines'] and trace['unresolved_dependencies']==['rbx']
 for bad in [text.replace('RCX_matches_target=1','RCX_matches_target=0'),text.replace('caller_class=CRAWL_SURFACE_CALLER','caller_class=OTHER_CALLER'),text.replace('readable=1 size=128','readable=0 size=128'),text.replace('pointer=0x200000','pointer=0x300000')]:
  p.write_text(bad);b=review(p);assert not b['source_candidate_identified'] and not b['v01b4_pass']
 # Fixed identity basis must fail the cross-state/diagonal test.
 identity=[0.]*32;identity[0]=identity[5]=identity[10]=identity[15]=1.;hexed=struct.pack('<32f',*identity).hex()
 p.write_text(re.sub(r'(ORIENTATION_SOURCE_SNAPSHOT [^\n]* bytes=)[a-f0-9]+',lambda m:m[1]+hexed,text));assert not review(p)['source_candidate_identified']
 p.write_text('\n'.join(x for x in lines if 'label=writer offset=0x0 ' not in x));assert not review(p)['functions']['writer']['hash_verified']
 p.write_text(text.replace(hashlib.sha256(raw).hexdigest(),'0'*64));assert not review(p)['functions']['writer']['hash_verified']
 # Overlapping windows reported as clusters; layout evidence is required for promotion.
 vectors,matrices,quats,clusters=inspect(struct.pack('<32f',*identity));assert matrices and clusters and quats
 assert all(x['meaning'].startswith('unit_Vec4') for x in quats)
 (ROOT/'logs/orientation_source_review_fixture.log').write_text(text)
print('PASS: same RDX+0x10 across A/E/WallRun/Crawl basis, signed/absolute normalized scores, overlap/matrix/quaternion grouping; constant basis/wrong target/caller/pointer/unreadable/missing code/hash rejected; local RDX slice; synthetic never game PASS/write permission')
