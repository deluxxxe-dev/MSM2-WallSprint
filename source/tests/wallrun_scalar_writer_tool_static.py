#!/usr/bin/env python3
from pathlib import Path
import ast

root = Path(__file__).resolve().parents[1]
v4 = (root / 'tools/wallrun_scalar_writer_refs.py').read_text(encoding='utf-8')
v5 = (root / 'tools/wallrun_scalar_writer_refs_v5.py').read_text(encoding='utf-8')
wrapper = (root / 'tools/windows/Inspect-WallRun250-Writers.ps1').read_text(encoding='utf-8')
version = (root / 'include/version.hpp').read_text(encoding='utf-8')
ast.parse(v4)
ast.parse(v5)

for needle in (
    "import wallrun_scalar_writer_refs as v4",
    "MAX_NO_PDATA_BYTES = 128",
    "MAX_NO_PDATA_INSTRUCTIONS = 32",
    "NONVOLATILE_OR_RSP = {'rbx','rbp','rsi','rdi','r12','r13','r14','r15','rsp'}",
    "def decode_no_pdata_scope(",
    "no_pdata_fallback_call_",
    "no_pdata_fallback_nonvolatile_or_rsp_write_",
    "no_pdata_fallback_indirect_jump_",
    "no_pdata_fallback_conditional_escape_",
    "no_pdata_fallback_cfg_cycle_",
    "DIRECT_EXTERNAL_TAIL_JUMP",
    "STRICT_NO_PDATA_BOUNDED_CFG",
    "'schema': 5",
    "'roots_only': ['0x29c06f0', '0x29c18b0']",
    "'global_executable_scan': False",
    "'adjacent_offsets_scanned': False",
    "'deeper_callee_traversal': False",
    "'gameplay_write_authorized': False",
):
    assert needle in v5, needle

for needle in (
    "'target_rva': 0x29C06F0",
    "'target_rva': 0x29C18B0",
    "'entry_owner_offset': 0x80",
    "'entry_owner_offset': 0x58",
    "'target_relative_field_offset': 0x1D0",
    "'target_relative_field_offset': 0x1F8",
    "INTERSECTION_SAME_REGISTER_SAME_OWNER_OFFSET_ONLY",
    "VOLATILE_WIN64 = {'rax','rcx','rdx','r8','r9','r10','r11'}",
):
    assert needle in v4, needle

for forbidden in (
    'WriteProcessMemory', 'VirtualProtect', 'CreateRemoteThread', 'SetThreadContext',
    'MAX_RAW_DISP_HITS', 'MAX_CANDIDATE_FUNCTIONS', 'writer_cap', 'raw_displacement_hits',
):
    assert forbidden not in v5, forbidden

for needle in (
    "wallrun_scalar_writer_refs_v5.py",
    "wallrun_scalar_writer_refs.py",
    "discover_ground_support.py",
    "/workspace/toolchains/analysis-python/bin/python3",
    "tuple(map(int,capstone.__version__.split(chr(46)))) == (5,0,6)",
    "-m py_compile $converted[0] $converted[1] $converted[2]",
    "Pinned V5 Python helper syntax preflight failed; EXE scan not started",
    "MSM2-WS-wallrun250-writers-",
    "No game launch, hook, build, package, deploy, or memory write.",
):
    assert needle in wrapper, needle

for forbidden in ('Update-WS.cmd', 'Overstrike.exe', 'taskkill', 'Stop-Process'):
    assert forbidden not in wrapper, forbidden

assert 'versionLabel[]=\"v0.1c47\"' in version
assert 'packageVersion[]=\"0.1.72\"' in version

print('wallrun_scalar_writer_tool_static: PASS (V5 strict no-pdata fallback, c47 runtime unchanged)')
