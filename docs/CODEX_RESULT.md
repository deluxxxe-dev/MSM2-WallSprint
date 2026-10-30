# Codex result — durable update ownership / stale KIT repair

2026-10-31 JST. STATUS: implementation complete; owner Windows acceptance pending.
Branch `codex/v01c20-edi-value-provenance`; baseline `6eeef17`.
No main merge. Toolkit `MSM2-Mod-Tools-v1.2.0.zip` only; runtime/package r4 unchanged.
Implementation commit and independent publication readback recorded below after push.

## Changes and result

OS-backed shared gate replaces empty presence-only ownership. Explicit inherited
handles protect surviving build children; PID creation identity/token/workspace
metadata, strict inactivity evidence and reversible legacy preservation fail closed
on ambiguity. Update commits/releases ownership before checked launch; launch
failure cannot roll back validated deployment. No process killing, DLL relocation,
attribute normalization, index.lock deletion or SF/ordinary-mod cleanup.

Fresh fetch/bootstrap repairs old installed KIT without executing old checker or
building/deploying runtime. Current successful updater refreshes owned KIT; backup
settings/profile/shortcuts and previous toolkit are retained. Source/distributed
bootstrap dependencies updated for future packaging; r4 package not regenerated.
Actual previous launcher was raw Popen, NOT checked launcher; do not infer previous
self-lock checker failure. Exact chain and constraints:
`analysis/UPDATE_LOCK_TOOLKIT_RECOVERY.md`.

Changed files: environment ownership/repair/launcher/distribution/tests; workspace
engine and Windows updater/launch/process helpers; future package/bootstrap tool
dependencies; individual+combined manuals; PROJECT_STATE; analysis/release record.
No native code or INI/runtime/package change. Original five untracked historical
archives/bundle retained unstaged.

## Validation


2026-10-31 JST. Owned temporary fixtures only. Expected all admitted operations
pass, unsafe/concurrent/unknown cases refuse without losing runtime/user bytes.
Actual final results:

| Exact command | Expected / actual | PASS | SKIP | UNAVAILABLE |
|---|---|---:|---:|---:|
| `python3 environment/test_operation_lock.py` | cross-process lifetime, legacy/identity/race/replacement refusal; matches | 14 | 1 | 0 |
| `python3 source/tools/test-workspace.py` | update failure preserves old runtime; launch failure retains committed new runtime; concurrent update denied; matches | 20 groups | 0 | 0 |
| `python3 environment/test_mod_environment.py` | backup/restore, refresh/settings/data safety; matches | 28 | 0 | 0 |
| `python3 environment/test_tooling_repair.py` | real owned Git ff/old checker repair/idempotence/dirty/divergent/live blocks; matches | 3 | 0 | 0 |
| `python3 environment/test_start_overstrike_preflight.py` | installer proxy accepted, misplaced loader refused unchanged; matches | 1 group | 0 | 0 |
| `bash environment/test_operation_lock_windows.sh` | Win32 sharing/crash/child lifetime under Wine; matches | 4 groups | 0 | 1 |
| PowerShell command below | mocked Windows launch contract; matches | 16 checks | 0 | 0 |

```
XDG_CACHE_HOME=/tmp/ws-lock-xdg-cache XDG_CONFIG_HOME=/tmp/ws-lock-xdg-config XDG_DATA_HOME=/tmp/ws-lock-xdg-data DOTNET_CLI_HOME=/tmp/ws-lock-dotnet /tmp/ws-proxy-pwsh/pwsh -NoLogo -NoProfile -File environment/test_windows_contract.ps1
```

SKIP: full sandbox /proc environment inventory cannot inspect process1; actual
probe refused. Isolated real owned process token inventory passed, not WSL proof.
UNAVAILABLE: Wine reparse API did not materialize a reparse node; Linux symlink
refusal passed. Actual owner Windows CPython/CIM/WSL/shortcuts/deployment are
separately UNAVAILABLE (one acceptance group); no actual-PC PASS claim.
PowerShell Core on Linux uses owned mocks, not actual COM shortcuts. Native API
fixture is Win32 C++ under Wine, not the Python ctypes integration on Windows.

Initial diagnostic runs encountered read-only default PowerShell cache (fixed by
private XDG dirs), Wine reparse nonmaterialization (reported UNAVAILABLE), and an
aborted fixture path collision (unique tick suffix fixes fixture). Official Windows
embeddable Python download returned403; no Windows Python integration claimed.

`python3 environment/build_distribution.py`: toolkit-only ZIP CRC/member bytes/
SHA checks; no game, external binary, user backup or r4 rebuild. Existing r4 runtime
and packages must match baseline6eeef17. Native/game suites intentionally NOT RUN:
CODEX_NEXT forbids native suite rerun for tooling-only work. No gameplay changes.

Additional verification, expected/actual all PASS:

- `python3 -m py_compile environment/operation_lock.py environment/repair_tools.py environment/mod_environment.py source/tools/workspace.py source/tools/package.py`
- `bash -n source/tools/windows/workspace-build.sh environment/test_operation_lock_windows.sh`
- `git diff --check`
- `git diff --exit-code 6eeef17 -- source/src source/include source/mod BUILD_MANIFEST.json 'MSM2-WS-v0.1c38r4*'` (no differences)
- PowerShell Parser.ParseFile on five changed .ps1 files: Repair-Mod-Tools,
  launch-overstrike, stop-processes, stop-overstrike-updater, windows.ps1; zero errors.
  Run with the same private XDG variables above and `-File /tmp/ws-lock-parse.ps1`.
  Parser body: for each file, call `[System.Management.Automation.Language.Parser]::ParseFile`
  with token/error refs and throw if errors.Count; no script operations executed.
- `python3 environment/build_distribution.py` verifies ZIP CRC and exact member
  bytes. Artifact58258bytes / SHA256
  `beb58b90e172eb456cada2169dd92751e5bc0818398d082ab4e28575cd93ea08`.

No BLOCKED implementation subtask. Actual-PC group UNAVAILABLE, not PASS.
Wisdoms read access confirmed; synchronization deferred to connected shared-manual
handoff; reusable detailed proof retained in project analysis. No shared claim
of owner deployment or actual Windows Python/WSL acceptance.

## ONE owner repair action

Close game/Overstrike/updater normally, then paste the single bootstrap block in
`docs/mod-tools/README.md` into Windows PowerShell. It fetches exact canonical
branch, then runs freshly staged repair under OS ownership, refuses dirty/divergent
Git or ambiguous/live legacy state. Expected marker:
`TOOLING_REPAIR_COMPLETE runtime_unchanged=1 no_build=1 no_game_launch=1`.
No Update-WS rebuild or game test requested for this toolkit repair. Return exact
refusal/output if inactivity cannot be proved. Never manually delete a lock.

---
Previous lateral-route result (historical):

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

## Verified publication

Research commit c57f44399db043d23bc0c328bba7a8f84b3cdc2e, normal push
d7cb968->c57f443. `python3 /tmp/ws-lateral-publication.py` exit0: independent
GitHub fetch into /tmp/ws-steering-docs-publication; FETCH_HEAD/ls-remote/local
head equal; all3 changed blobs against d7cb968 equal local files. The script
fetches the named branch and compares `git show FETCH_HEAD:<path>` bytes for
all `git diff --name-only d7cb968 HEAD` paths. Same check repeated after this
record-only commit; final full SHA reported in chat, no self-reference.
No binary rebuild/repackage, no raw body/log or legacy untracked artifacts staged.

Publication: implementation commit `551515d` plus whitespace/package-metadata follow-up.
Trailing blank line caught by cached diff check after initial source checks; corrected.
No behavioral changes after final suite run, so full suites were not repeated.
