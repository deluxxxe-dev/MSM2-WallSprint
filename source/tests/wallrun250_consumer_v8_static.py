#!/usr/bin/env python3
from pathlib import Path
import ast

root = Path(__file__).resolve().parents[1]
script = (root / 'tools/wallrun250_consumer_v8.py').read_text(encoding='utf-8')
wrapper = (root / 'tools/windows/Inspect-WallRun250-Consumers.ps1').read_text(encoding='utf-8')
version = (root / 'include/version.hpp').read_text(encoding='utf-8')
ast.parse(script)

for needle in (
    "SCHEMA = 8",
    "READ_SITES = v4.KNOWN_UPDATE_READS",
    "MAX_PRE_CONTEXT = 6",
    "MAX_REACHABLE_INSTRUCTIONS = 32",
    "MAX_WORKLIST_VISITS = 96",
    "consumer_site_not_unique_disp250_read_",
    "WALLRUN_0X250_AUTHENTICATED_UPDATE_CONSUMER_SLICES",
    "'truncated_by_instruction_cap': truncated",
    "'unvisited_frontier_rvas': frontier",
    "TRUNCATE_AND_REPORT_FRONTIER_NOT_REFUSE",
    "'update_only': True",
    "'direct_callee_traversal': False",
    "'global_executable_scan': False",
    "'adjacent_offsets_scanned': False",
    "'live_reads_or_writes': 0",
    "'game_process_operation': False",
    "'gameplay_write_authorized': False",
    "INITIAL_FILE_BYTES_ONLY_NOT_RUNTIME_VALUES",
    "'field_semantics': 'UNKNOWN'",
):
    assert needle in script, needle

for forbidden in (
    'WriteProcessMemory', 'VirtualProtect', 'CreateRemoteThread', 'SetThreadContext',
    'MAX_RAW_DISP_HITS', 'writer_cap', 'raw_displacement_hits',
    'consumer_reachable_instruction_cap_',
):
    assert forbidden not in script, forbidden

for needle in (
    "wallrun250_consumer_v8.py",
    "wallrun_scalar_writer_refs.py",
    "discover_ground_support.py",
    "/workspace/toolchains/analysis-python/bin/python3",
    "tuple(map(int,capstone.__version__.split(chr(46)))) == (5,0,6)",
    "-m py_compile $converted[0] $converted[1] $converted[2]",
    "Pinned V8 Python helper syntax preflight failed; EXE scan not started",
    "MSM2-WS-wallrun250-consumers-",
    "No game launch, hook, build, package, deploy, or memory write.",
):
    assert needle in wrapper, needle

for forbidden in ('Update-WS.cmd', 'Overstrike.exe', 'taskkill', 'Stop-Process'):
    assert forbidden not in wrapper, forbidden

assert 'versionLabel[]=\"v0.1c47\"' in version
assert 'packageVersion[]=\"0.1.72\"' in version

print('wallrun250_consumer_v8_static: PASS (bounded Update consumer slices, cap truncates, c47 runtime unchanged)')
