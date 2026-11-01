#!/usr/bin/env python3
from pathlib import Path
import ast

root = Path(__file__).resolve().parents[1]
v4 = (root / 'tools/wallrun_scalar_writer_refs.py').read_text(encoding='utf-8')
v5 = (root / 'tools/wallrun_scalar_writer_refs_v5.py').read_text(encoding='utf-8')
v6 = (root / 'tools/wallrun_scalar_writer_refs_v6.py').read_text(encoding='utf-8')
v7 = (root / 'tools/wallrun_scalar_writer_refs_v7.py').read_text(encoding='utf-8')
wrapper = (root / 'tools/windows/Inspect-WallRun250-Writers.ps1').read_text(encoding='utf-8')
version = (root / 'include/version.hpp').read_text(encoding='utf-8')
ast.parse(v4)
ast.parse(v5)
ast.parse(v6)
ast.parse(v7)

for needle in (
    "import wallrun_scalar_writer_refs as v4",
    "import wallrun_scalar_writer_refs_v6 as v6",
    "PATH_B_TARGET = 0x29C18B0",
    "PATH_B_ENTRY_OWNER_OFFSET = 0x58",
    "HANDOFF_CALL_SITE = 0x29C18D4",
    "HANDOFF_CALL_BYTES = bytes.fromhex('e817886c00')",
    "HANDOFF_TARGET = 0x308A0F0",
    "HANDOFF_ENTRY_OWNER_OFFSET = 0x58",
    "v6_handoff_call_bytes_mismatch",
    "v6_handoff_call_reachability_mismatch",
    "v6_handoff_rcx_owner_offset_mismatch",
    "'schema': 7",
    "'source_path': 'B_only'",
    "'single_handoff_target': '0x308a0f0'",
    "'path_a_reopened': False",
    "'global_executable_scan': False",
    "'adjacent_offsets_scanned': False",
    "'deeper_callee_traversal': False",
    "'gameplay_write_authorized': False",
):
    assert needle in v7, needle

for needle in (
    "MAX_NO_PDATA_TRACE_VISITS = 2048",
    "STRICT_NO_PDATA_BOUNDED_CFG_V6",
    "BOUNDED_CFG_CYCLES_ALLOWED_MONOTONE_INTERSECTION",
):
    assert needle in v6, needle

for needle in (
    "MAX_NO_PDATA_BYTES = 128",
    "MAX_NO_PDATA_INSTRUCTIONS = 32",
    "NONVOLATILE_OR_RSP = {'rbx','rbp','rsi','rdi','r12','r13','r14','r15','rsp'}",
    "no_pdata_fallback_call_",
    "no_pdata_fallback_nonvolatile_or_rsp_write_",
    "no_pdata_fallback_indirect_jump_",
    "no_pdata_fallback_conditional_escape_",
):
    assert needle in v5, needle

for needle in (
    "'target_rva': 0x29C06F0",
    "'target_rva': 0x29C18B0",
    "INTERSECTION_SAME_REGISTER_SAME_OWNER_OFFSET_ONLY",
    "VOLATILE_WIN64 = {'rax','rcx','rdx','r8','r9','r10','r11'}",
):
    assert needle in v4, needle

for forbidden in (
    'WriteProcessMemory', 'VirtualProtect', 'CreateRemoteThread', 'SetThreadContext',
    'MAX_RAW_DISP_HITS', 'MAX_CANDIDATE_FUNCTIONS', 'writer_cap', 'raw_displacement_hits',
):
    assert forbidden not in v7, forbidden

for needle in (
    "wallrun_scalar_writer_refs_v7.py",
    "wallrun_scalar_writer_refs_v6.py",
    "wallrun_scalar_writer_refs_v5.py",
    "wallrun_scalar_writer_refs.py",
    "discover_ground_support.py",
    "/workspace/toolchains/analysis-python/bin/python3",
    "tuple(map(int,capstone.__version__.split(chr(46)))) == (5,0,6)",
    "-m py_compile $converted[0] $converted[1] $converted[2] $converted[3] $converted[4]",
    "Pinned V7 Python helper syntax preflight failed; EXE scan not started",
    "MSM2-WS-wallrun250-writers-",
    "No game launch, hook, build, package, deploy, or memory write.",
):
    assert needle in wrapper, needle

for forbidden in ('Update-WS.cmd', 'Overstrike.exe', 'taskkill', 'Stop-Process'):
    assert forbidden not in wrapper, forbidden

assert 'versionLabel[]=\"v0.1c48\"' in version
assert 'packageVersion[]=\"0.1.73\"' in version

print('wallrun_scalar_writer_tool_static: PASS (V7 offline tool retained; c48 runtime active)')
