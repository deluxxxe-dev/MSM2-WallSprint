#!/usr/bin/env python3
from pathlib import Path

root=Path(__file__).resolve().parents[1]
cpp=(root/'src/msm2/request_presence.cpp').read_text(encoding='utf-8')
asm=(root/'src/msm2/request_presence.S').read_text(encoding='utf-8')
telemetry=(root/'src/msm2/telemetry.cpp').read_text(encoding='utf-8')
build=(root/'tools/build_windows.sh').read_text(encoding='utf-8')
default_ini=(root/'mod/WallSprint.ini').read_text(encoding='utf-8')
preset=(root/'mod/presets/request_presence_read_only.ini').read_text(encoding='utf-8')
version=(root/'include/version.hpp').read_text(encoding='utf-8')

required_cpp=[
    'kRequestRva = 0x11A1BD0',
    'kRequestEndRva = 0x11A1CA2',
    'kRequestUnwindRva = 0x1154EDC',
    'kPhaseRecordCap = 8192',
    'kPhaseLimit = 1',
    'kPhaseDurationMs = 30000',
    '2d879249c65b8074bb548e6563b5e0c2955ab672c4fd3cf080adb83b48691d64',
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

lower_asm=asm.lower()
assert '\n    call ' not in lower_asm
for reg in ('xmm','ymm','zmm'):
    assert reg not in lower_asm
assert 'cmp eax, 8192' in asm
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
assert 'REQUEST_PRESENCE_DIAGNOSTIC=0' in default_ini
assert 'WALLSPRINT_ORIENTATION_WRITE=0' in default_ini
assert 'REQUEST_PRESENCE_DIAGNOSTIC=1' in preset
for needle in ('WALLSPRINT_ENABLE=0','WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0','WALLSPRINT_ORIENTATION_WRITE=0','WALLSPRINT_ADHESION_WRITE=0','WALLSPRINT_MOVEMENT_WRITE=0'):
    assert needle in preset, needle
assert 'versionLabel[]="v0.1c40"' in version
assert 'packageVersion[]="0.1.65"' in version
print('request_presence_static: PASS')
