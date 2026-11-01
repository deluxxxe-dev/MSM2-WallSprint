#!/usr/bin/env python3
from pathlib import Path
import ast

root = Path(__file__).resolve().parents[1]
script = (root / 'tools/wallrun_scalar_writer_refs.py').read_text(encoding='utf-8')
wrapper = (root / 'tools/windows/Inspect-WallRun250-Writers.ps1').read_text(encoding='utf-8')
version = (root / 'include/version.hpp').read_text(encoding='utf-8')
ast.parse(script)

for needle in (
    "EXPECTED_SHA = peutil.EXPECTED_SHA",
    "UPDATE_BEGIN = 0x1424530",
    "SOURCE_BEGIN = 0x367DD10",
    "SOURCE_END = 0x367DD5F",
    "f437f092ce0b2315103302d06cdefb5ded88126302b3d276bb1a6ebcbf774378",
    "'target_rva': 0x29C06F0",
    "'target_rva': 0x29C18B0",
    "'entry_owner_offset': 0x80",
    "'entry_owner_offset': 0x58",
    "'target_relative_field_offset': 0x1D0",
    "'target_relative_field_offset': 0x1F8",
    "def merge_intersection(a, b):",
    "INTERSECTION_SAME_REGISTER_SAME_OWNER_OFFSET_ONLY",
    "unsupported_or_partial_writes': 'KILL_DESTINATION_PROVENANCE'",
    "memory_load_destination': 'KILL_DESTINATION_PROVENANCE'",
    "VOLATILE_WIN64 = {'rax','rcx','rdx','r8','r9','r10','r11'}",
    "call_return_volatile_gprs_invalidated",
    "entry_reachable_indirect_jump",
    "downstream_target_not_instruction_boundary",
    "owner_relative_0x250_accesses",
    "proven_rcx_owner_offsets",
    "global_executable_scan': False",
    "deeper_callee_traversal': False",
    "gameplay_write_authorized': False",
    "'schema': 4",
):
    assert needle in script, needle

for forbidden in (
    'WriteProcessMemory', 'VirtualProtect', 'CreateRemoteThread', 'SetThreadContext',
    'MAX_RAW_DISP_HITS', 'MAX_CANDIDATE_FUNCTIONS', 'writer_cap', 'raw_displacement_hits',
):
    assert forbidden not in script, forbidden

for needle in (
    "codex/v01c20-edi-value-provenance",
    "https://github.com/deluxxxe-dev/MSM2-WallSprint.git",
    "wallrun_scalar_writer_refs.py",
    "discover_ground_support.py",
    "/workspace/toolchains/analysis-python/bin/python3",
    "tuple(map(int,capstone.__version__.split(chr(46)))) == (5,0,6)",
    "-m py_compile $converted[0] $converted[1]",
    "Pinned V4 Python helper syntax preflight failed; EXE scan not started",
    "MSM2-WS-wallrun250-writers-",
    "No game launch, hook, build, package, deploy, or memory write.",
):
    assert needle in wrapper, needle

for forbidden in ('Update-WS.cmd', 'Overstrike.exe', 'taskkill', 'Stop-Process'):
    assert forbidden not in wrapper, forbidden

assert 'versionLabel[]="v0.1c47"' in version
assert 'packageVersion[]="0.1.72"' in version

print('wallrun_scalar_writer_tool_static: PASS (V4 owner-relative CFG provenance, c47 runtime unchanged)')
