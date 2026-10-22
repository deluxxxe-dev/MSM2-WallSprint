from pathlib import Path
import tempfile,struct
from unittest.mock import patch
from review_reset_writers import review
root=Path(__file__).resolve().parents[1]
r=review(root/'logs/windows_reset_trace_synthetic.log',1,2)
assert len(r['reset_candidates'])>=2 and not r['RESET_WRITER_VERIFIED'] and not r['actual_game_capture']
assert any(c['instruction'] for c in r['reset_candidates'])
assert all(not c['complete_candidate_evidence'] for c in r['reset_candidates'])
# Pure parser fixture uses own data, never any captured native function bytes.
raw=struct.pack('<16f',1,0,0,0,0,1,0,0,0,0,1,0,10,20,30,1).hex()
rows=['TEST_ONLY_SYNTHETIC']
resolved=[]
for session,n in [(1,'(0,0,1)'),(2,'(-0.707106781,0,0.707106781)')]:
 rows += [f'RESET_TRACE_EVENT session={session} native_update=77 relative_us_from_override=11000 qpc_ms=111 rip=0x1000 writer_function_rva=0x2cfc580 caller_rva=0x1234 thread_id=9 up_before={n} up_after=(0,1,0) N398={n} transform=0x8000 read_valid=1 target_verified_by_watch=1 entry_id={session} entry_correlated=1 first_surface_observed_qpc_ms=100 dropped=0',f'RESET_ENTRY_SOURCE session={session} entry_id={session} readable=1 entry_correlated=1 RCX=0x8000 source_0x40={raw} source_up=(0,1,0) source_overridden_this_entry=0']
 resolved.append({'session':str(session),'thread_id':'9','qpc_ms':'111','rip_after':'0x1000','resolved':{'instruction':'own parser stub store'},'function_capture':{'sha256':'3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6'}})
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp)/'test.log';text='\n'.join(rows);p.write_text(text)
 with patch('review_reset_writers.decode',return_value={'event_resolution':resolved}):
  good=review(p,1,2);assert all(c['complete_candidate_evidence'] for c in good['reset_candidates']) and good['same_A_E_writer_caller'] and not good['RESET_WRITER_VERIFIED']
  assert not review(p)['same_A_E_writer_caller']
  for bad in [text.replace('native_update=77','native_update=0'),text.replace('entry_correlated=1','entry_correlated=0'),text.replace('readable=1','readable=0'),text.replace('dropped=0','dropped=1'),text.replace('relative_us_from_override=11000','relative_us_from_override=31000'),text.replace('RCX=0x8000','RCX=0x9000'),text.replace('read_valid=1','read_valid=0'),text.replace('source_up=(0,1,0)','source_up=(0,0,1)')]:
   p.write_text(bad);b=review(p,1,2);assert not b['same_A_E_writer_caller'] and not b['RESET_WRITER_VERIFIED']
print('PASS: anchored hardware store decode, raw source association, explicit A/E mapping, timing/drop/target/source negative gates; synthetic cannot promote reset writer')
# Optional private original-writer replay only. Never included in release source.
private=root/'tools/fixtures/orientation_v01b3/writer.hex'
if private.exists():
 from review_orientation_writers import review as decode
 original=bytes.fromhex(private.read_text().strip());assert len(original)==584
 base=0x140000000;begin=base+0x2cfc580;after=begin+0x66
 body=bytearray(original);body[:14]=bytes.fromhex('ff25000000000000000000000000')
 def capture(body):
  gpr=[0]*16;gpr[3]=0x8000
  rows=['WallSprint v0.1b6 package_version=0.1.15','orientation_source_hook installed=1 exact_hash=3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6',f'ORIENTATION_WRITER_EVENT writer_id=1 state=WallRun session=1 thread_id=9 qpc_ms=1 rip=0x{after:x} module=Spider-Man2.exe module_base=0x{base:x} caller_valid=0 read_valid=1 transform=0x8000 new_observed_up=(0,1,0) N398=(0,0,1)',f'ORIENTATION_WRITER_REGS writer_id=1 qpc_ms=1 post_gpr_RAX_RCX_RDX_RBX_RSP_RBP_RSI_RDI_R8_R9_R10_R11_R12_R13_R14_R15='+','.join(f'{v:x}' for v in gpr),f'orientation_writer_function writer_id=1 begin=0x{begin:x} size=584']
  rows += [f'orientation_writer_code writer_id=1 offset=0x{i:x} bytes={body[i:i+64].hex()}' for i in range(0,584,64)]
  return '\n'.join(rows)
 with tempfile.TemporaryDirectory() as tmp:
  p=Path(tmp)/'private.log';p.write_text(capture(body));b=decode(p);w=b['writers'][0]
  assert w['function_capture']['owned_entry_prefix_normalized'] and w['resolved_hits']==1
  assert w['writes'][0]['width']==32 and w['writes'][0]['transform_offset']==0
  body[50]^=1;p.write_text(capture(body));bad=decode(p);assert not bad['writers'][0]['function_capture']['owned_entry_prefix_normalized']
 print('PASS: optional private known-writer owned-prefix restoration only after full584byte original SHA; tamper rejected')
else:print('SKIP: optional private584byte game-writer normalization replay; independent synthetic decoder/source tests ran')

# b7 review must gate the first override and explicit session mapping; A need not be +Z.
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp)/'b7.log'
 header=['TEST_ONLY_SYNTHETIC','WallSprint v0.1b7 package_version=0.1.16','RESET_TRACE_SELF_CHECK exact_build=1 common_writer_hook=1 reset_trace_enabled=1 reset_trace_admitted=1 primary_thread_prearm_supported=1']
 statuses=[f'PREARM_STATUS session={session} trace_already_armed_before_override=1 first_override=1' for session in (1,2)]
 b7='\n'.join(header+statuses+rows[1:])
 with patch('review_reset_writers.decode',return_value={'event_resolution':resolved}):
  p.write_text(b7);good=review(p,1,2);assert good['same_A_E_writer_caller'] and not good['RESET_WRITER_VERIFIED']
  for text in [b7.replace('trace_already_armed_before_override=1','trace_already_armed_before_override=0'),b7.replace(' first_override=1',''),b7.replace('dropped=0','dropped=1')]:
   p.write_text(text);bad=review(p,1,2);assert not bad['same_A_E_writer_caller'] and not bad['RESET_WRITER_VERIFIED']
  p.write_text(b7);assert not review(p,1,1)['same_A_E_writer_caller']
print('PASS: b7 first-override prearm/missing/false/drop/session-mapping gates; synthetic never verified')
