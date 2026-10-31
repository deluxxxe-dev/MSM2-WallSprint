#!/usr/bin/env python3
from pathlib import Path
import hashlib
import re

root=Path(__file__).resolve().parents[1]
cpp=(root/'src/msm2/request_presence.cpp').read_text(encoding='utf-8')
asm=(root/'src/msm2/request_presence.S').read_text(encoding='utf-8')
telemetry=(root/'src/msm2/telemetry.cpp').read_text(encoding='utf-8')
build=(root/'tools/build_windows.sh').read_text(encoding='utf-8')
default_ini=(root/'mod/WallSprint.ini').read_text(encoding='utf-8')
preset=(root/'mod/presets/request_presence_read_only.ini').read_text(encoding='utf-8')
info=(root/'mod/info.json').read_text(encoding='utf-8')
version=(root/'include/version.hpp').read_text(encoding='utf-8')
source_readme=(root/'README_TEST.md').read_text(encoding='utf-8')
project_readme=(root.parent/'README_TEST.md').read_text(encoding='utf-8')
package_wrapper=(root/'tools/package.py').read_text(encoding='utf-8')
audit=(root/'tools/audit_presence_hook_sites.py').read_text(encoding='utf-8')

GRAVITY_BEGIN=0x134EF00
GRAVITY_END=0x134EFBB
GRAVITY_UNWIND=0x94A1CB8
GRAVITY_SHA='bc9e1a75a4d45f4dc1a1baf99b704778714e83353d58688dce82bab901514a53'
GRAVITY_PREFIX=bytes.fromhex('48 89 5c 24 08')
GRAVITY_PREFIX_SHA='0666a96c5b537b54fb43e8d983f54c516a379f17f302825c7f047c7e9ff4cab0'

required_cpp=[
    'kRequestRva = 0x134EF00',
    'kRequestEndRva = 0x134EFBB',
    'kRequestUnwindRva = 0x94A1CB8',
    'kBatchRecordCap = 16',
    'kDrainIntervalMs = 500',
    'kQuiesceWaitMs = 250',
    'kSessionRecordCap = 65536',
    GRAVITY_SHA,
    'kRequestPrefix[] = {0x48,0x89,0x5c,0x24,0x08}',
    'kObservedRole[] = "GravityOrientationOwner"',
    'expected_instructions=1',
    'bool spanOk=validateInstructionSpan',
    'bool pdataOk=exactRequestUnwind',
    'hash_ok=%u prefix_ok=%u span_ok=%u pdata_ok=%u',
    'whole_site_sha=verified',
    'observer_unwind=split_verified',
    'captured_pointer_dereference=0',
    'argument_writes=0',
    'native_game_calls=0',
    'capture_control=automatic_on_admission',
    'capture_lifetime=script_session',
    'periodic_worker_drain_ms=%llu',
    'batch_record_cap=%ld',
    'session_record_cap=%llu',
    'REQUEST_PRESENCE_BATCH_SUMMARY target_role=%s',
    'REQUEST_PRESENCE_RECORD target_role=%s',
    'session_offset_ms_est=',
    'shutdown_flush=best_effort_not_required_for_prior_batches',
    'prior_periodic_batches_already_persisted=1',
    'volatile LONG captureCloseRequested=0',
    'atomicRead(&captureCloseRequested)==0',
    'drainBatch("periodic_worker",true)',
    'drainBatch(reason?reason:"stop",false)',
    'F11_disabled_capture_auto_starts_on_admission',
    'F12_disabled_capture_auto_starts_on_admission',
    'trampoline_retained=1',
    'observer_module_pinned=1',
]
for needle in required_cpp:
    assert needle in cpp, needle

runtime_begin=int(re.search(r'kRequestRva = 0x([0-9A-Fa-f]+)',cpp)[1],16)
runtime_end=int(re.search(r'kRequestEndRva = 0x([0-9A-Fa-f]+)',cpp)[1],16)
runtime_unwind=int(re.search(r'kRequestUnwindRva = 0x([0-9A-Fa-f]+)',cpp)[1],16)
runtime_hash=re.search(r'kRequestSha256\[\] = "([0-9a-f]{64})"',cpp)[1]
prefix_match=re.search(r'kRequestPrefix\[\]\s*=\s*\{([^}]+)\}',cpp)
assert prefix_match, 'production prefix missing'
prefix_bytes=bytes(int(x.strip(),16) for x in prefix_match[1].split(','))
assert (runtime_begin,runtime_end,runtime_unwind,runtime_hash)==(GRAVITY_BEGIN,GRAVITY_END,GRAVITY_UNWIND,GRAVITY_SHA)
assert prefix_bytes==GRAVITY_PREFIX
assert hashlib.sha256(prefix_bytes).hexdigest()==GRAVITY_PREFIX_SHA

# Production identity must match the accepted GravityOrientationOwner audit tuple.
audit_match=re.search(r"'GravityOrientationOwner':\((\d+),(\d+),'([0-9a-f]{64})'\)",audit)
assert audit_match, 'GravityOrientationOwner audit tuple missing'
audit_begin,audit_end,audit_hash=int(audit_match[1]),int(audit_match[2]),audit_match[3]
assert (runtime_begin,runtime_end,runtime_hash)==(audit_begin,audit_end,audit_hash)
assert GRAVITY_PREFIX_SHA in audit, 'accepted Gravity prefix SHA missing from audit fixture set'

# c44 RequestSimpleMotion must no longer be the active production target.
for stale in (
    'kRequestRva = 0x11A1BD0',
    'kRequestEndRva = 0x11A1CA2',
    'kRequestUnwindRva = 0x94697E0',
    '2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c',
    '{0x4c,0x8b,0xdc,0x49,0x89,0x5b,0x08}',
    'expected_instructions=2',
    'kPhaseDurationMs = 300000',
    'kPhaseRecordCap = 131072',
):
    assert stale not in cpp, stale

lower_asm=asm.lower()
assert '\n    call ' not in lower_asm
for reg in ('xmm','ymm','zmm'):
    assert reg not in lower_asm
assert 'cmp eax, 16' in asm
assert 'cmp eax, 131072' not in asm
assert 'ws_request_presence_read_tsc:' in asm
assert 'lock cmpxchg DWORD PTR ws_request_presence_count[rip], ecx' in asm
assert 'lock inc DWORD PTR ws_request_presence_overflow[rip]' in asm
assert 'lock inc DWORD PTR ws_request_presence_contention[rip]' in asm
assert 'jmp QWORD PTR ws_request_presence_original[rip]' in asm
assert asm.count('.seh_proc') == 10
assert asm.count('.seh_endproc') == 10
for reg in ('rax','rcx','rdx','r8','r9','r10','r11'):
    assert f'push {reg}' in asm
    assert f'pop {reg}' in asm
assert 'pushfq' in asm and 'popfq' in asm

for needle in ('request_presence::start','request_presence::tick','F8_kill_switch','script_disable'):
    assert needle in telemetry, needle
for needle in ('request_presence.S','request_presence.cpp','request_presence_asm.o','request_presence.o'):
    assert needle in build, needle
for ini in (default_ini,preset):
    assert 'REQUEST_PRESENCE_DIAGNOSTIC=1' in ini
    for needle in ('WALLSPRINT_ENABLE=0','WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0','WALLSPRINT_ORIENTATION_WRITE=0','WALLSPRINT_ADHESION_WRITE=0','WALLSPRINT_MOVEMENT_WRITE=0'):
        assert needle in ini, needle
    assert 'GravityOrientationOwner' in ini
    assert 'No F11/F12 input' in ini or 'no F11/F12 input' in ini
    assert '500 ms' in ini
    assert '16 records' in ini
    assert 'waiting' in ini
assert 'v0.1c45' in default_ini

assert 'versionLabel[]="v0.1c45"' in version
assert 'packageVersion[]="0.1.70"' in version
assert '"version": "0.1.70"' in info
assert 'WallSprint v0.1c45' in info
assert 'GravityOrientationOwner' in info
assert '500 ms' in info and '16 records' in info and '65536' in info

for readme in (source_readme, project_readme):
    assert 'v0.1c45' in readme
    assert '0.1.70' in readme
    assert 'GravityOrientationOwner' in readme
    assert 'No F11/F12 input is required' in readme
    assert '500 ms' in readme
    assert '16 retained records' in readme
    assert '65536' in readme
    assert 'No extra wait is required' in readme
    assert '0x94A1CB8' in readme
    assert 'c44' in readme and 'zero-hit' in readme

for needle in (
    'package_mode": "gravity_owner_presence_read_only"',
    '"native_stack_source_write_enabled": False',
    '"path_b_override": False',
    '"request_presence_diagnostic": True',
    '"native_source": False',
    '"observed_role": "GravityOrientationOwner"',
    '"capture_control": "automatic_on_admission"',
    '"capture_lifetime": "script_session"',
    '"periodic_worker_drain_ms": 500',
    '"batch_record_cap": 16',
    '"session_record_cap": 65536',
    '"shutdown_flush_required_for_prior_batches": False',
    '"manual_F11_F12_required": False',
):
    assert needle in package_wrapper, needle
assert 'c45' in package_wrapper
assert 'RequestSimpleMotion presence package' not in package_wrapper
assert 'capture_duration_ms' not in package_wrapper
assert 'record_cap": 131072' not in package_wrapper

print('request_presence_static: PASS')
