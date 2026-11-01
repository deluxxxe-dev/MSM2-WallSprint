#!/usr/bin/env python3
from pathlib import Path
import ast

root = Path(__file__).resolve().parents[1]
script = (root / 'tools/wallrun_scalar_writer_refs.py').read_text(encoding='utf-8')
wrapper = (root / 'tools/windows/Inspect-WallRun250-Writers.ps1').read_text(encoding='utf-8')

# Syntax-only validation does not import Capstone on generic test hosts.
ast.parse(script)

for needle in (
    "EXPECTED_SHA = peutil.EXPECTED_SHA",
    "UPDATE_BEGIN = 0x1424530",
    "UPDATE_END = 0x14246D6",
    "e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f",
    "FIELD_DISP = 0x250",
    "KNOWN_UPDATE_READS = (0x1424585, 0x142460E)",
    "0x14245C3: 0x367DD10",
    "0x1424630: 0x29BD5F0",
    "MAX_CANDIDATE_FUNCTIONS = 16_384",
    "MAX_TOTAL_DECODE_BYTES = 96 * 1024 * 1024",
    "authenticated_Update_not_in_disp250_prefilter",
    "known_Update_disp250_site_selfcheck_failed",
    "complete_PE_runtime_function_linear_decode",
    "executable_file_bytes_prefilter_to_containing_PE_runtime_functions",
    "wallrun_owner_proven_at_instruction': False",
    "computed_address_writes': 'NOT_COVERED'",
    "game_process_operation': False",
):
    assert needle in script, needle

# The rejected v1 byte-backtracking candidate explosion must not return.
for forbidden in (
    "decoded_candidate_start_cap",
    "possible_starts = set()",
    "for back in range(1, 15)",
):
    assert forbidden not in script, forbidden

for forbidden in ('WriteProcessMemory', 'VirtualProtect', 'CreateRemoteThread', 'SetThreadContext'):
    assert forbidden not in script, forbidden

for needle in (
    "codex/v01c20-edi-value-provenance",
    "https://github.com/deluxxxe-dev/MSM2-WallSprint.git",
    "wallrun_scalar_writer_refs.py",
    "discover_ground_support.py",
    "/workspace/toolchains/analysis-python/bin/python3",
    "tuple(map(int,capstone.__version__.split(chr(46)))) == (5,0,6)",
    "MSM2-WS-wallrun250-writers-",
    "No game launch, hook, build, package, deploy, or memory write.",
):
    assert needle in wrapper, needle

for forbidden in ('Update-WS.cmd', 'Overstrike.exe', 'Spider-Man2.exe -', 'taskkill', 'Stop-Process'):
    assert forbidden not in wrapper, forbidden

print('wallrun_scalar_writer_tool_static: PASS')
