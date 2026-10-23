# Next task — WallSprint v0.1c16 Actual-Game Writer Call Gate

## Status
```text
implementation branch: codex/v01c16-writer-call-effect
implementation commit: 0a00bdeee05ffdae593d6963abd5c24424a2d8ef
validated/package head: 8f66b15a109e503cdfc115eb5a48ebadd7bc4113
version: v0.1c16 / package 0.1.34
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `analysis/V01C15_RUNTIME_RESULT.md`, `analysis/V01C16_WRITER_CALL_EFFECT.md`, `analysis/V01C16_GATE.md`, and c16 `docs/CODEX_RESULT.md` before interpreting the next log.

## Accepted evidence
C15 actual A/E is accepted: orientation retained through verified Deinit, hard0/stale0/Position writes0, clean cleanup, 8 true entry snapshots and 8 complete maps per session. The writer branch proof crossed `0x2CFC91F jne` and stopped exactly at `0x2CFC927 call`.

C16 implementation is complete and cloud regressions pass, but the cloud does not contain the current private 183-byte writer/callee. Therefore the actual call target, runtime-function boundary/hash, effect classification, resumed writer proof and any later exact blocker remain unknown until the real-game c16 log is supplied.

## Current task: human runtime gate only
Do not implement c17 yet.

Install:
```text
WallSprint_v0.1c16_writer_call_effect.zip
```

Run one private log:
```text
A/cardinal ~4 s
-> fully end WallRun
-> move or fast-travel to E
-> E/diagonal ~4 s
-> stop and provide the complete WallSprint.log
```
No special input. Video is needed only if a visible anomaly occurs.

## Required runtime checks
Confirm startup:
```text
v0.1c16
package 0.1.34
exact Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
exact-build admission=1
```

For `0x2CFC927`, extract and evaluate `MOVEMENT_WRITER_CALL_CONTRACT` / integrity fields:
```text
call_rva=0x2cfc927
return_rva=0x2cfc92c
call_kind
target_rva
target boundary
target code identity/hash result
callee effect classification
source-span effect
clobber/preservation result
exact callee blocker RVA/mnemonic/class if refused
```

Then inspect the resumed `MOVEMENT_RDX_BRANCH_CONTRACT` / XYZ proof:
```text
all_relevant_store_paths_proven
proven_component_mask
x_path / y_path / z_path
next_barrier_rva / mnemonic / reason
xyz_causality_proven
W classification independently
```

Require per-session final capture/lineage accounting and clean lifecycle/watch cleanup. A/E orientation must remain retained and `Position_writes_by_mod=0`.

## Decision after the log
Outcome A — actual call effect is authenticated and compatible, and the writer proof reaches all XYZ stores:
- record the actual c16 runtime result;
- only then define a separate next task for caller/storage/producer ownership around `0x351EB00`;
- do not mutate the producer merely because the writer certificate passes.

Outcome B — c16 emits a narrower exact call/callee or later writer blocker:
- record that exact blocker;
- next task must address only that smallest unresolved scope;
- do not expand to general tracing or producer traversal.

Outcome C — build/hash/integrity/lifecycle/orientation/cleanup regression:
- stop and repair the regression before any lineage work.

## Unchanged safety gate
```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```

No Actor/Transform Position write, RDX/stack-source mutation, Path A Position mutation, input rewrite, custom movement/velocity/gravity, active adhesion, producer hook/traversal at `0x351EB00`, camera/animation/traversal work, PAGE_GUARD/data-page protection mutation, guessed native game calls, raw-log publication, or full private game-body distribution.

Preserve exact-build fail-closed behavior, `AGENTS.md`, `docs/DEV_RULES.md`, c7-c15 lifecycle/orientation/refresh/DR-watch/direct-call/entry-pairing/accounting/branch gates, and independent implementation suitable for Nexus Mods.