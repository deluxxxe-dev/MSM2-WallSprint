# Codex result — one flat-wall lateral movement route research

2026-10-31 JST. STATUS: partial — bounded investigation completed, movement route
not admitted; exact missing original continuation identified. No runtime change.
Branch codex/v01c20-edi-value-provenance; baseline
 d7cb968297478aa751633abda75d56d6ff359a95 (latest fetched/fast-forwarded).
No main merge. Changed only analysis/FLAT_WALL_LATERAL_ROUTE_REVIEW.md,
PROJECT_STATE current handoff and this result. Publication recorded below.

## Result / evidence / precise next target

Accepted supplied r4 runtime: session3 eleven completed turns,27.031s source
through native Deinit,1819 A/1822 B writes, no hard kill; one40ms stale recovery.
This accepts normal5s-stop removal in that test, not unlimited lifetime. Facing
START remains explicitly deferred, not solved; no repeat of input inventory.
Current runtime remains v0.1c38r4/package0.1.60 and existing output/log convention.

Selected ONE original route: verified WallRun_Update0x1424530 -> shared direct
conditional-branch continuation0x14248f9 -> unknown original steering/motion
consumer. Reference RequestSimpleMotion/UpdateMotion names remain hints. Four
actual helper CALL targets are untyped; no guessed input/coordinate/contact ABI.
State/owner identity is supported by existing lifecycle; requested vector/frame,
contact handling and motion submission/restoration contract are not proven.
A/B orientation overrides all orientation rows but preserves Position/W/compact
metadata/scales; possible coupling to a future motion consumer's frame remains
UNKNOWN. No whole-engine ownership proof imposed, no API admitted by name alone.

Previously available c20 attached log authenticates current intended EXE build/
SHA and captures ORIGINAL pre-MinHook Update422bytes. SHA matches compiled static
plan. Offline decoder confirms422/422bytes,89 instructions. Four branches leave
the runtime-function span for0x14248f9; terminal fallthrough also missing, no RET.
Thus entire captured runtime-function RANGE is not entire logical CFG/function.
This concrete first blocker was found in existing data, not a speculative input
API search. No recursive callee scan or reference-offset transplant.

ONE next acquisition: up to128 original file-backed executable bytes starting
RVA0x14248f9, plus current-build/file/block SHA and available section/runtime/
unwind metadata. Stop at relevant control boundary/cap. Existing offline
slot_publication.parse_pe() supplies checked file_range; use its metadata/range
primitive ONLY, not analyze_image/CLI scans. Authenticated on-disk EXE not available
in selected evidence, so successor extraction not executed. Current runtime
analyzeNative only captures four lifecycle STARTS' single lookup spans; repeating
the same normal startup would not acquire this successor. No invented successor
capture command/new hook/helper or full EXE upload/full-log request. Chat supplies
any owner acquisition procedure; this handoff contains no new game test.

## Exact executed inspection / expected / actual

Read AGENTS required manuals, current state/result/NEXT, owner runtime analysis,
AFTER_TURN_PLAN, input inventory and historical movement/facing source review.
Read-only Wisdoms ENGINE_INTERNALS lifecycle and WALLSPRINT_REFERENCE_MODS movement
sections: authenticated Update RVA matches; labelled motion APIs unverified.
Targeted source reads: wallrun_hooks.cpp, sdk_bridge.cpp, surface_source.hpp,
static_observation_plan.hpp, existing static_access_v2.py/review_surface_source.py
and slot_publication.py::parse_pe. Historical pre-r1 facing review is not treated
as current “no reversal branch” behavior. No broad repository/binary code sweep.

Update-only private capture extraction actually executed:

```bash
python3 - <<'PY'
from pathlib import Path
p=Path('/workspace/attachments/e22e1300-b10a-4907-b0c7-d10c37f6dcfb/MSM2-WS-v0.1c20-20261024-180229.log')
lines=[x for x in p.read_text().splitlines() if 'native_function_capture function=Update ' in x or 'native_function_code function=Update ' in x]
assert len(lines)>1
Path('/tmp/ws-lateral-update-only.log').write_text('\n'.join(lines)+'\n')
PY
python3 source/tools/review_surface_source.py /tmp/ws-lateral-update-only.log --output-dir /tmp/ws-lateral-update-review
```

Expected/actual:15 selected lines (metadata+14 chunks), no other bodies; one complete
checksum-verified capture, no overlapping/missing chunk; exit0/PASS. Existing tool
reports gate BLOCKED,0 access/face candidates (those schemas deliberately omitted);
this is code integrity, NOT motion/layout/ownership admission. Hash:
e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f.

Exact focused decoder validation executed with pinned offline Python:

```bash
/workspace/toolchains/analysis-python/bin/python - <<'PY'
from pathlib import Path
import sys,capstone
sys.path.insert(0,'source/tools')
import static_access_v2
raw=Path('/tmp/ws-lateral-update-review/Update.bin').read_bytes()
r=static_access_v2.analyze(raw,0x1424530,'Update')
xs=list(capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_64).disasm(raw,0x1424530))
assert r['sha256']=='e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f'
assert r['decoded_bytes']==422 and r['decoded_instructions']==89
assert r['outside_span_branches']==[0x14248f9]
assert xs[-1].address+xs[-1].size==0x14246d6 and xs[-1].mnemonic=='xor'
assert not any(x.mnemonic.startswith('ret') for x in xs)
PY
```

Equivalent assertion checks and summary emission ran in the focused command;
expected/actual exit0,89 decoded/reachable instructions, outside target0x14248f9,
nonterminal exact-end fallthrough verified. No byte skip or whole-CFG claim. No
raw game bytes/full disassembly committed; private files/summary are /tmp only.

`git diff --check`:expected/actual exit0/PASS.
`git diff --exit-code d7cb968 -- source environment BUILD_MANIFEST.json
MSM2-WS-v0.1c38r4.zip MSM2-WS-v0.1c38r4-source.zip MSM2-WS-v0.1c38r4.script.disabled`:
expected/actual empty diff/exit0/PASS (no runtime/helper/config/package change).

Read-only analysis/doc checks4 PASS; SKIP7 unchanged binary pipeline commands
(focused reversal, portable, Windows build/smoke/native/hooks/release), not required
for docs-only research with unchanged tools. No old901 PASS run represented as
current tests. UNAVAILABLE1: missing authenticated successor code/metadata;
BLOCKED1: lateral route admission pending that concrete continuation and consumer
contract. No motion adapter implemented, so no fabricated lateral experiment test.
No tool/source change requiring new offline regression suite.

## Safety / persistence / owner

All C52/4096/stale/build/identity/cleanup gates unchanged. No new hooks, pointer
chase, input/Position/velocity/gravity/native/gameplay mutation or c73. No SF/shared
environment or other-repository edits. Reusable single-span/CFG distinction kept
in local analysis; Wisdoms synchronization deferred under local-docs-only scope.
No new runtime version/log/artifact; existing r4 files stay byte-identical, older
packaged documentation stays historical. Owner next: no movement experiment yet;
one offline successor-window acquisition to be arranged by Chat, no command
invented here. Normal commit/push and independent readback recorded below.
