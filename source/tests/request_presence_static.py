#!/usr/bin/env python3
from pathlib import Path
import re

root=Path(__file__).resolve().parents[1]
cpp=(root/'src/msm2/request_presence.cpp').read_text(encoding='utf-8')
asm=(root/'src/msm2/request_presence.S').read_text(encoding='utf-8')
telemetry=(root/'src/msm2/telemetry.cpp').read_text(encoding='utf-8')
build=(root/'tools/build_windows.sh').read_text(encoding='utf-8')
default_ini=(root/'mod/WallSprint.ini').read_text(encoding='utf-8')
preset=(root/'mod/presets/request_presence_read_only.ini').read_text(encoding='utf-8')
version=(root/'include/version.hpp').read_text(encoding='utf-8')
source_readme=(root/'README_TEST.md').read_text(encoding='utf-8')
project_readme=(root.parent/'README_TEST.md').read_text(encoding='utf-8')
package_wrapper=(root/'tools/package.py').read_text(encoding='utf-8')
audit=(root/'tools/audit_presence_hook_sites.py').read_text(encoding='utf-8')

required_cpp=[
    'kRequestRva = 0x11A1BD0',
    'kRequestEndRva = 0x11A1CA2',
    'kRequestUnwindRva = 0x1154EDC',
    'kPhaseRecordCap = 16384',
    'kPhaseLimit = 1',
    'kPhaseDurationMs = 45000',
    '2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c',
    'bool spanOk=validateInstructionSpan',
    'bool pdataOk=exactRequestUnwind',
    'hash_ok=%u prefix_ok=%u span_ok=%u pdata_ok=%u',
    'whole_site_sha=verified',
    'observer_unwind=split_verified',
    'captured_pointer_dereference=0',
    'argument_writes=0',
    'request_suppression=0',
    'label_auth=video_posthoc_not_native_authenticated',
    'correlation=non_atomic',
    'capture_control=F11_once',
    'legacy_F12=disabled',
    'offset_ms_est=',
    'offset_clock=tsc_linearized_to_GetTickCount64',
    'trampoline_retained=1',
    'observer_module_pinned=1',
]
for needle in required_cpp:
    assert needle in cpp, needle

# Prevent another runtime/audit identity drift: the production admission tuple must
# exactly match the already accepted RequestSimpleMotion audit tuple.
runtime_begin=int(re.search(r'kRequestRva = 0x([0-9A-Fa-f]+)',cpp)[1],16)
runtime_end=int(re.search(r'kRequestEndRva = 0x([0-9A-Fa-f]+)',cpp)[1],16)
runtime_hash=re.search(r'kRequestSha256\[\] = "([0-9a-f]{64})"',cpp)[1]
audit_match=re.search(r"'RequestSimpleMotion':\((\d+),(\d+),'([0-9a-f]{64})'\)",audit)
assert audit_match, 'RequestSimpleMotion audit tuple missing'
audit_begin,audit_end,audit_hash=int(audit_match[1]),int(audit_match[2]),audit_match[3]
assert (runtime_begin,runtime_end,runtime_hash)==(audit_begin,audit_end,audit_hash), (
    (runtime_begin,runtime_end,runtime_hash),(audit_begin,audit_end,audit_hash)
)
assert '2d879249c65b8074bb548e6563b5e0c2955ab672c4fd3cf080adb83b48691d64' not in cpp

lower_asm=asm.lower()
assert '\n    call ' not in lower_asm
for reg in ('xmm','ymm','zmm'):
    assert reg not in lower_asm
assert 'cmp eax, 16384' in asm
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

for needle in ('request_presence::start','request_presence::tick','VK_F11','VK_F12','F8_kill_switch','script_disable'):
    assert needle in telemetry, needle
for needle in ('request_presence.S','request_presence.cpp','request_presence_asm.o','request_presence.o'):
    assert needle in build, needle
for ini in (default_ini,preset):
    assert 'REQUEST_PRESENCE_DIAGNOSTIC=1' in ini
    for needle in ('WALLSPRINT_ENABLE=0','WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0','WALLSPRINT_ORIENTATION_WRITE=0','WALLSPRINT_ADHESION_WRITE=0','WALLSPRINT_MOVEMENT_WRITE=0'):
        assert needle in ini, needle

assert 'versionLabel[]="v0.1c41"' in version
assert 'packageVersion[]="0.1.66"' in version

for readme in (source_readme, project_readme):
    assert 'v0.1c41' in readme
    assert '0.1.66' in readme
    assert 'F11 once' in readme
    assert '16384' in readme
    assert '45-second' in readme or '45 seconds' in readme

for needle in (
    'package_mode": "request_presence_read_only"',
    '"native_stack_source_write_enabled": False',
    '"path_b_override": False',
    '"request_presence_diagnostic": True',
    '"native_source": False',
    '"capture_duration_ms": 45000',
    '"record_cap": 16384',
):
    assert needle in package_wrapper, needle

print('request_presence_static: PASS')
