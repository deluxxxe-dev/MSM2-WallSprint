# Codex result — bounded WS movement/facing source review

2026-10-30 JST. Branch `codex/v01c20-edi-value-provenance`.
Task/source baseline: `28c5fa6721818e3c5c8e3bdc7c190db7de7c85b3`.

## STATUS

**success** — completed the requested read-only source review and stale-state
correction. This is not a runtime explanation/certification of visible facing.

## CHANGED

- `analysis/WALLRUN_MOVEMENT_FACING_SOURCE_REVIEW_20261030.md`
- `docs/PROJECT_STATE.md`
- `docs/CODEX_RESULT.md`

CODEX_NEXT, all source/tools/configs and historical evidence remain unchanged.
No other repository modified. Five pre-existing untracked archives/bundle remain
untouched and excluded. No main merge, reset or force push.

## RESULT

Accepted the task's ChatGPT-inspected second export as **VERIFIED-STATIC
(supplied export)**: 544 bytes, SHA256
`d83095b5e68cc6fa20c95bc044345f7f46fd6c0a35a682df25374b140dfcec2d`, exactly
one vanilla byte changed (0x219,75->5); Fast remains100. No new cloud hash of the
private file is claimed. Workflow evidence supports later installed replacement
but does not certify the earlier game session. Slow5's owner-observed no visible
change remains narrow; runtime Standard/Slow consumption and isolation remain
unproven. Do not repeat extraction or serial steering probes by default.

The current reachable orientation path is `telemetry.cpp:run` ->
`native_source_override.cpp:tick` -> `Math/source_orientation.hpp:Transition`.
Entry Transform Forward projected onto the wall determines a saved target;
Up is the surface normal, Right is rebuilt, Forward re-orthogonalized. Sampling
interpolates entry-to-target, then keeps this entry-derived basis during the
same active transition. Displacement reversal supplies no argument/branch to
this calculation and does not negate Forward/Right or rebuild the target.

Existing Path A `capturePath`/`writeStackOrientation` and Path B
`captureCopy`/`compactReplacement`/`writeStackCompact` substitute all orientation
axes from that published basis, not just Up. Path A preserves the Position/W
row; B preserves row scales/metadata. They do not select movement direction.
The legacy Actor writer is hard disabled. Direction-aware `ws::Controller`
and `ws::planar::Controller` exist as helpers instantiated in tests, not this
runtime call path. No input/movement rewrite or new ownership is inferred.

Existing logs separate read-only displacement/finite-difference velocity from
sampled Transform Forward when corresponding valid records exist. Path A source
Forward is not actual final-Actor readback. Retention checks Up only. Sampling
is bounded/conditional, not an atomic reversal measurement. No independent
visible-animation facing channel or numeric three-way comparison was found in
the inspected paths. Thus entry-heading retention is a **hypothesis relevant
to the symptom**, not a proven cause of the visible animation mismatch.

PROJECT_STATE now closes Slow5-pending/serial steering testing, records accepted
export limits, and makes the bounded source result and remaining fact explicit.
Full source file/function/line references and diagnostic limits are in analysis.

## Validation — exact procedures and expected/actual results

Commands ran in `/workspace/WallSprint`, except the explicit read-only Wisdoms
reference. These are source/documentation checks, not executed gameplay tests.

| Procedure / exact command | Expected | Actual |
| --- | --- | --- |
| `git branch --show-current`; `git fetch origin codex/v01c20-edi-value-provenance`; `git merge --ff-only origin/codex/v01c20-edi-value-provenance`; `git rev-parse HEAD` | Correct branch/current bounded task; no main merge | Expected branch; ff-only to `28c5fa6721818e3c5c8e3bdc7c190db7de7c85b3`. AGENTS, operational manual/rules/layout/state/task, Fast A/B and extraction review read. Relevant Wisdoms reference read only. PASS |
| `nl -ba source/src/msm2/telemetry.cpp \| sed -n '66,172p'`; `nl -ba source/src/NativeHooks/native_source_override.cpp \| sed -n '395,535p'`; `nl -ba source/include/Math/source_orientation.hpp` | Identify reachable direction/orientation data flow and reversal dependency | Worker native-source tick publishes saved-entry-derived interpolated basis, without movement direction/input in target calculation. Admission and active-window limits preserved. PASS |
| `nl -ba source/src/NativeHooks/native_source_override.cpp \| sed -n '550,607p'`; `nl -ba source/include/Math/compact_basis.hpp`; `nl -ba source/src/GameInterop/stack_source.cpp \| sed -n '1,110p'`; `nl -ba source/src/msm2/orientation_source.cpp \| sed -n '84,98p'`; `nl -ba source/src/NativeHooks/reset_copy_source.cpp \| sed -n '55,76p'` | Verify immediate source-write callers and preserved bytes/axes | A/B source-only stores use shared basis; Position/W or compact scales/metadata preserved; no reversal selector. PASS |
| `rg -n 'ws::Controller\|planar::Controller\|ws::Host' source --glob '*.{cpp,hpp,c,h}' --glob '!vendor/**'`; `nl -ba source/include/planar_mvp.hpp \| sed -n '25,84p'`; `nl -ba source/include/wallsprint.hpp \| sed -n '44,58p'`; `cat source/src/msm2/actor_writer.cpp`; `rg -n 'telemetry\|planar\|actor_writer\|native_source_override\|cpp\|src/' source/tools/build_windows.sh` | Distinguish helper direction-selection code from current runtime | Instantiations found only in core/planar tests; runtime link/call path agrees; Actor writer disabled. Presence of legacy INI parsing does not activate it. PASS |
| `nl -ba source/src/NativeHooks/native_source_override.cpp \| sed -n '620,695p'`; same file `sed -n '250,335p'`; `nl -ba source/src/msm2/orientation_log.cpp \| sed -n '1,145p'`; `nl -ba source/src/msm2/sdk_bridge.cpp \| sed -n '18,49p'`; `nl -ba source/include/diagnostics.hpp \| sed -n '33,60p'`; `nl -ba source/include/Math/movement_delta.hpp \| sed -n '1,75p'` | Determine actual log quantities, origins and gaps | Displacement versus Transform Forward available conditionally; source basis != final readback; Up-only retention; no independent animation-facing channel in inspected paths. PASS |
| Source-reference/documentation scope audit (Python below); `git diff --name-only 28c5fa6721818e3c5c8e3bdc7c190db7de7c85b3`; `git status --short` | Existing line references valid; exactly three allowed documentation paths; stale Slow5-pending text gone | Verified against unchanged source and scope before staging/commit. PASS |
| `git diff --check`; `git diff --cached --check` | Whitespace clean | Exit0. PASS |
| Publication verification below | Pushed branch SHA and independently fetched documentation equal local | Pending initial documentation push; finalize before handoff |

Expected source-review groups all PASS. No unit/native/game test executions.
Exact source-reference audit command executed before commit:

```bash
python3 - <<'PY_AUDIT'
from pathlib import Path
import re, subprocess
base = '28c5fa6721818e3c5c8e3bdc7c190db7de7c85b3'
report = Path('analysis/WALLRUN_MOVEMENT_FACING_SOURCE_REVIEW_20261030.md')
refs = re.findall(r'(source/[A-Za-z0-9_./-]+):(\d+)', report.read_text())
assert refs
for name, line in refs:
    assert 1 <= int(line) <= len(Path(name).read_text().splitlines()), (name, line)
assert not subprocess.check_output(['git','diff',base,'--','source','environment','AGENTS.md','docs/CODEX_NEXT.md'])
state = Path('docs/PROJECT_STATE.md').read_text()
assert '## Next minimal steering discrimination' not in state
assert 'd83095b5e68cc6fa20c95bc044345f7f46fd6c0a35a682df25374b140dfcec2d' in state
print('PASS: source references, unchanged code/task and closed Slow5-pending state')
PY_AUDIT
```

### Not executed / unavailable

- **SKIP (one suite group):** no build, portable/smoke/hooks tests, steering
  builder tests or packaging. Current task explicitly requires source/diff
  review only, and no code is changed. No runtime rebuild/artifact generated.
- **UNAVAILABLE (one evidence group):** event-aligned displacement, final Hero
  Transform Forward and independent visible-facing relation for the observed
  active-source reversal. No new private runtime record/animation measurement
  was supplied. Schema/source availability is not event evidence. No new capture
  or game run is requested to fill it in this task.
- **BLOCKED 0:** no authorized subtask left unimplemented; the missing runtime
  fact is reported as the bounded handoff, not replaced with guessed semantics.

## Safety / next action

Runtime v0.1c38 unchanged. Existing C52
`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` and all proof/write gates remain
binding. No config changes, hooks/watches, instrumentation, new design,
engine/binary/CFG scan, c73, build/deploy/reinstall, game/toc/loader/profile changes,
SF work or other-repository changes. No new package or private asset/log committed.

ChatGPT decides how to resolve only this exact fact: in the same observed
reversal, was the existing override admitted/active, and did Transform Forward
stay entry-derived as displacement reversed, or turn while visible animation
kept its old facing? This review does not explain the engine/animation layer.
No owner runtime operation is requested.

## Branch / publication

Branch `codex/v01c20-edi-value-provenance`.
Initial documentation commit/push and independent remote-content comparison are
pending; record actual SHA and verification before final handoff.
