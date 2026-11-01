#!/usr/bin/env python3
from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
source = (root / 'src/msm2/actor_writer.cpp').read_text(encoding='utf-8')
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
    'wallrun_0x250_threshold_probe_admission',
    'wallrun_0x250_threshold_probe phase=',
    'exact_update_pre_original=1',
    'post_bridge_admitted_only=1',
    'pointer_dereference=0 game_call=0 game_write=0',
):
    assert needle in source, needle

match = re.search(
    r'extern "C" std::uint64_t ws_write_update_pre\(std::uintptr_t owner\) noexcept\{(.*?)\n\}',
    source,
    re.S,
)
assert match, 'ws_write_update_pre body'
pre = match.group(1)
for needle in (
    'msm2::readMemory(owner+kWallRun250Offset,&bits,sizeof(bits))',
    'consumer0True.fetch_add',
    'consumer1Fallthrough.fetch_add',
    'return 0;',
):
    assert needle in pre, needle
for forbidden in (
    'writeLog', 'WriteProcessMemory', 'VirtualProtect', 'memcpy(owner',
    'ws_write_dispatch(', 'ws_write_update_post(',
):
    assert forbidden not in pre, forbidden

for forbidden in (
    'WriteProcessMemory', 'CreateRemoteThread', 'SetThreadContext',
):
    assert forbidden not in source, forbidden

assert 'void ws_write_dispatch(unsigned,std::uintptr_t) noexcept{}' in source
assert 'void ws_write_update_post(std::uintptr_t,std::uint64_t) noexcept{}' in source
assert 'bool active(){return false;}' in source
assert 'versionLabel[]=\"v0.1c48\"' in version
assert 'packageVersion[]=\"0.1.73\"' in version

print('wallrun250_threshold_probe_static: PASS (c48 exact Update-entry read-only threshold counters)')
