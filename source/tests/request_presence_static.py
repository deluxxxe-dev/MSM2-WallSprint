#!/usr/bin/env python3
from pathlib import Path
import hashlib
import re

root=Path(__file__).resolve().parents[1]
cpp=(root/'src/msm2/request_presence.cpp').read_text(encoding='utf-8')
asm=(root/'src/msm2/request_presence.S').read_text(encoding='utf-8')
surface=(root/'include/msm2/surface_source.hpp').read_text(encoding='utf-8')
scalar=(root/'include/wallrun_scalar_observation.hpp').read_text(encoding='utf-8')
plan=(root/'include/static_observation_plan.hpp').read_text(encoding='utf-8')
default_ini=(root/'mod/WallSprint.ini').read_text(encoding='utf-8')
preset=(root/'mod/presets/request_presence_read_only.ini').read_text(encoding='utf-8')
info=(root/'mod/info.json').read_text(encoding='utf-8')
version=(root/'include/version.hpp').read_text(encoding='utf-8')
source_readme=(root/'README_TEST.md').read_text(encoding='utf-8')
project_readme=(root.parent/'README_TEST.md').read_text(encoding='utf-8')
package_wrapper=(root/'tools/package.py').read_text(encoding='utf-8')
audit=(root/'tools/audit_presence_hook_sites.py').read_text(encoding='utf-8')

# Dormant c45 Gravity observer remains compiled but must keep its exact proven identity/safety.
GRAVITY_BEGIN=0x134EF00; GRAVITY_END=0x134EFBB; GRAVITY_UNWIND=0x94A1CB8
GRAVITY_SHA='bc9e1a75a4d45f4dc1a1baf99b704778714e83353d58688dce82bab901514a53'
GRAVITY_PREFIX=bytes.fromhex('48 89 5c 24 08')
for needle in ('kRequestRva = 0x134EF00','kRequestEndRva = 0x134EFBB','kRequestUnwindRva = 0x94A1CB8',GRAVITY_SHA,'kRequestPrefix[] = {0x48,0x89,0x5c,0x24,0x08}','kObservedRole[] = "GravityOrientationOwner"','expected_instructions=1','captured_pointer_dereference=0','argument_writes=0','native_game_calls=0'):
    assert needle in cpp, needle
runtime_begin=int(re.search(r'kRequestRva = 0x([0-9A-Fa-f]+)',cpp)[1],16)
runtime_end=int(re.search(r'kRequestEndRva = 0x([0-9A-Fa-f]+)',cpp)[1],16)
runtime_unwind=int(re.search(r'kRequestUnwindRva = 0x([0-9A-Fa-f]+)',cpp)[1],16)
runtime_hash=re.search(r'kRequestSha256\[\] = "([0-9a-f]{64})"',cpp)[1]
prefix_match=re.search(r'kRequestPrefix\[\]\s*=\s*\{([^}]+)\}',cpp); assert prefix_match
prefix_bytes=bytes(int(x.strip(),16) for x in prefix_match[1].split(','))
assert (runtime_begin,runtime_end,runtime_unwind,runtime_hash)==(GRAVITY_BEGIN,GRAVITY_END,GRAVITY_UNWIND,GRAVITY_SHA)
assert prefix_bytes==GRAVITY_PREFIX
assert hashlib.sha256(prefix_bytes).hexdigest()=='0666a96c5b537b54fb43e8d983f54c516a379f17f302825c7f047c7e9ff4cab0'
audit_match=re.search(r"'GravityOrientationOwner':\((\d+),(\d+),'([0-9a-f]{64})'\)",audit); assert audit_match
audit_unwind=int(re.search(r'EXPECTED_GRAVITY_UNWIND_RVA\s*=\s*(\d+)',audit)[1])
assert (runtime_begin,runtime_end,runtime_unwind,runtime_hash)==(int(audit_match[1]),int(audit_match[2]),audit_unwind,audit_match[3])

lower_asm=asm.lower()
assert '\n    call ' not in lower_asm
for reg in ('xmm','ymm','zmm'): assert reg not in lower_asm
assert 'cmp eax, 16' in asm and 'cmp eax, 131072' not in asm
assert asm.count('.seh_proc')==10 and asm.count('.seh_endproc')==10
assert 'jmp QWORD PTR ws_request_presence_original[rip]' in asm

# c46 active contract: no Gravity presence hook; reuse verified WallRun observation only.
for ini in (default_ini,preset):
    assert 'REQUEST_PRESENCE_DIAGNOSTIC=0' in ini
    assert 'WALLRUN_HOOK_DIAGNOSTIC=1' in ini
    assert 'WALLRUN_OBJECT_DIAGNOSTIC=1' in ini
    for needle in ('WALLSPRINT_ENABLE=0','WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0','WALLSPRINT_ORIENTATION_WRITE=0','WALLSPRINT_ADHESION_WRITE=0','WALLSPRINT_MOVEMENT_WRITE=0'): assert needle in ini
assert 'v0.1c46' in default_ini

UPDATE_SHA='e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f'
assert UPDATE_SHA in scalar
assert f'{{"Update","{UPDATE_SHA}",422,89}}' in plan
for needle in ('0x208','0x220','0x238','0x250','0x40c','0x3ecccccdU','0x41400000U','0x42960000U','0x42c80000U','0x42480000U','wallrun_update_scalar_sample','wallrun_update_scalar_summary','semantic_mapping=UNPROVEN','game_write=0','pointer_dereference=0'):
    assert needle in scalar, needle
for needle in ('#include "wallrun_scalar_observation.hpp"','ws::scalar_fields::Observer scalarObserver','scalarObserver.reset()','scalarObserver.begin','scalarObserver.observe','scalarObserver.finish','wallrun_update_scalar_plan','pointer_dereference=0 game_write=0'):
    assert needle in surface, needle
assert 'writeMemory' not in scalar

assert 'versionLabel[]="v0.1c46"' in version
assert 'packageVersion[]="0.1.71"' in version
assert '"version": "0.1.71"' in info and 'WallSprint v0.1c46' in info
for readme in (source_readme,project_readme):
    for needle in ('v0.1c46','0.1.71','0x208','0x220','0x238','0x250','0x40C','REQUEST_PRESENCE_DIAGNOSTIC=0','wallrun_update_scalar_summary'):
        assert needle in readme, needle
for needle in ('package_mode": "wallrun_update_scalar_read_only"','"request_presence_diagnostic": False','"status": "c45_bounded_zero_hit_retired"','"extra_native_hooks": False','"captured_pointer_dereference": False','"semantic_mapping_authenticated": False','"offsets_hex": ["0x208", "0x220", "0x238", "0x250", "0x40C"]'):
    assert needle in package_wrapper, needle
assert 'c46' in package_wrapper

print('request_presence_static: PASS (c46 dormant presence + WallRun scalar contract)')
