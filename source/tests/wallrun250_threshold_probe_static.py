#!/usr/bin/env python3
from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
source = (root / 'src/msm2/actor_writer.cpp').read_text(encoding='utf-8')
header = (root / 'include/msm2/actor_writer.hpp').read_text(encoding='utf-8')
asm = (root / 'src/msm2/hook_thunks.S').read_text(encoding='utf-8')
version = (root / 'include/version.hpp').read_text(encoding='utf-8')

for needle in (
    'kWallRun250Offset=0x250',
    'kConsumerThreshold0Rva=0x5303DE8',
    'kConsumerThreshold1Rva=0x5337098',
    'kV8Threshold0Bits=0x3F000000u',
    'kV8Threshold1Bits=0x3F35C28Fu',
    'readLiveThresholds',
    'thresholdProbeEnabled',
    'consumer0True',
    'consumer1Fallthrough',
    'std::isnan(value)',
    'unordered0=fieldNan||std::isnan(t0)',
    'unordered1=fieldNan||std::isnan(t1)',
    'wallrun_0x250_threshold_probe_admission',
    'wallrun_0x250_threshold_probe phase=',
    'direct_update_thunk_callback=1',
    'bridge_unwind_or_argument_clone=0',
    'pointer_dereference=0 game_call=0 game_write=0',
    'std::atomic<unsigned> ws_update_probe_enabled{0}',
):
    assert needle in source, needle

for needle in (
    'extern std::atomic<unsigned> ws_update_probe_enabled;',
    'void ws_update_probe_sample(std::uintptr_t owner) noexcept;',
):
    assert needle in header, needle

match = re.search(
    r'extern "C" void ws_update_probe_sample\(std::uintptr_t owner\) noexcept\{(.*?)\n\}',
    source,
    re.S,
)
assert match, 'ws_update_probe_sample body'
probe = match.group(1)
for needle in (
    'msm2::readMemory(owner+kWallRun250Offset,&bits,sizeof(bits))',
    'consumer0True.fetch_add',
    'consumer1Fallthrough.fetch_add',
):
    assert needle in probe, needle
for forbidden in (
    'writeLog', 'WriteProcessMemory', 'VirtualProtect', 'memcpy(owner',
    'ws_write_dispatch(', 'ws_write_update_pre(', 'ws_write_update_post(',
):
    assert forbidden not in probe, forbidden

assert 'extern "C" std::uint64_t ws_write_update_pre(std::uintptr_t) noexcept{return 0;}' in source
assert 'void ws_write_dispatch(unsigned,std::uintptr_t) noexcept{}' in source
assert 'void ws_write_update_post(std::uintptr_t,std::uint64_t) noexcept{}' in source
assert 'bool active(){return false;}' in source
assert 'ws_write_dispatch_enabled.store(1' not in source

# Probe is Update-only. Generic Init/Entry/Deinit thunk macro must not reference it.
macro, update = asm.split('.globl ws_hook_update', 1)
assert 'ws_update_probe_enabled' not in macro
assert update.count('or eax,DWORD PTR ws_update_probe_enabled[rip]') == 1
assert update.count('cmp DWORD PTR ws_update_probe_enabled[rip],0') == 1
assert update.count('call ws_update_probe_sample') == 1
assert update.index('or eax,DWORD PTR ws_update_probe_enabled[rip]') < update.index('call ws_update_probe_sample')
assert update.index('call ws_update_probe_sample') < update.index('cmp DWORD PTR ws_write_dispatch_enabled[rip],0')
assert update.index('cmp DWORD PTR ws_write_dispatch_enabled[rip],0') < update.index('call ws_update_prepare')

for forbidden in (
    'WriteProcessMemory', 'CreateRemoteThread', 'SetThreadContext',
):
    assert forbidden not in source, forbidden

assert 'versionLabel[]=\"v0.1c48\"' in version
assert 'packageVersion[]=\"0.1.73\"' in version

print('wallrun250_threshold_probe_static: PASS (c48 dedicated Update-thunk read-only threshold counters; writer gate stays off)')
