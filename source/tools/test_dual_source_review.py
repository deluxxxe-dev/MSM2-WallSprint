from pathlib import Path
import tempfile
from review_dual_source import review,COPY_HASH,EXE_HASH
rows=['TEST_ONLY_SYNTHETIC','WallSprint v0.1b8 package_version=0.1.17','target_build_gate=ACCEPTED','exe_file_sha256='+EXE_HASH,f'RESET_COPY_HOOK admitted=1 installed=1 original_sha256={COPY_HASH} writer_rva=0x2cfc3c0 size=444 direct_call_verified=1','DUAL_SOURCE_ADMISSION admitted=1 dual_source_required=1 copy_hook_authenticated=1']
for s in (1,2):
 rows += [f'COMMON_SOURCE_OVERRIDE session={s} elapsed=1 caller_rva=0x2cfc8f3 target_transform=0x8000 position_unchanged=1',f'RESET_COPY_SOURCE_ENTRY session={s} elapsed=1 caller_rva=0x2cfc84b RCX=0x8000 RDX=0x1040 RSP=0x1008 stack_limit=0x1000 stack_base=0x2000 source_is_stack_local=1 layout_valid=1 source_right=(1,0,0) source_up=(0,1,0) source_forward=(0,0,1) source_position=(10,20,30)',f'RESET_COPY_SOURCE_OVERRIDE session={s} elapsed=1 source_ptr=0x1040 position_unchanged=1 caller_verified=1 target_verified=1 source_stack_verified=1',f'ORIENTATION_RETENTION_PASS=1 session={s} held_ms=0 held_updates=20']
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp)/'test.log';text='\n'.join(rows);p.write_text(text)
 good=review(p,1,2);assert all(s['orientation_retained'] for s in good['sessions'].values()) and not good['ORIENTATION_OWNERSHIP_VERIFIED'] and not good['STRONG_A_E_VERIFIED']
 # Same independent parser data without synthetic tag exercises the header gate.
 real=text.replace('TEST_ONLY_SYNTHETIC\n','');p.write_text(real);assert review(p,1,2)['STRONG_A_E_VERIFIED'];assert not review(p,1,1)['STRONG_A_E_VERIFIED']
 for bad in [real.replace(COPY_HASH,'0'*64),real.replace('dual_source_required=1','dual_source_required=0'),real.replace('caller_verified=1','caller_verified=0'),real.replace('position_unchanged=1','position_unchanged=0'),real.replace('source_is_stack_local=1','source_is_stack_local=0'),real.replace('caller_rva=0x2cfc84b','caller_rva=0x2cfc84c'),real.replace('source_up=(0,1,0)','source_up=(0,0,1)'),real.replace('held_updates=20','held_updates=19'),real.replace('RCX=0x8000','RCX=0x9000')]:
  p.write_text(bad);assert not review(p,1,2)['STRONG_A_E_VERIFIED']
 p.write_text(real+'\nNEW_RESET_AFTER_DUAL_OVERRIDE session=2 writer_rva=0x1234');r=review(p,1,2);assert not r['STRONG_A_E_VERIFIED'] and r['sessions']['2']['new_resets']
 p.write_text(real+'\nNATIVE_SOURCE_ACTOR_OBSERVATION session=2 observation_phase=next_matching_native_PRE actor_up_dot_surface=0');r=review(p,1,2);assert r['ORIENTATION_OWNERSHIP_VERIFIED'] and not r['STRONG_A_E_VERIFIED']
print('PASS: dual source layout/caller/stack/target/Position/hash/admission/retention/unknown-reset/A-E gates; synthetic cannot verify game ownership')
