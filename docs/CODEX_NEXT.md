# Next task — WallSprint v0.1c12 Actual-Game Movement-Lineage Gate

## Base
Use the completed c12 delivery exactly:

```text
branch: codex/v01c12-movement-source-lineage
implementation commit: 4c84d634df4d629b237d4cc311d5e5cd6de4398c
published head: c966d233787504dda88f65d1825c5c1c408a2d67
version: v0.1c12 / package 0.1.30
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, c12 `docs/CODEX_RESULT.md`, and `analysis/V01C12_GATE.md` before any follow-up implementation.

## Status
c12 cloud implementation is complete and validated, but the actual-game lineage proof is not. `writable_source_proven=0` remains authoritative. Do not implement c13 movement/input/adhesion/camera work before the human c12 log is adjudicated.

## Immediate task: human runtime evidence only
Install `WallSprint_v0.1c12_movement_source_lineage.zip` and run ordinary input:

```text
1. A/cardinal planar WallRun continuously for ~4 s
2. E/diagonal planar WallRun continuously for ~4 s
```

No special stick pattern and no visible movement change are expected.

Return the complete private WallSprint startup-through-cleanup log plus whether posture/orientation remained stable and whether any hitch/freeze/crash occurred.

## Required startup evidence
Preserve exact reasons; do not loosen a failed gate. Collect:

```text
WallSprint v0.1c12 package_version=0.1.30
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1 ... pairs=9
DUAL_SOURCE_ADMISSION admitted=1
MOVEMENT_WRITER_PROOF ...
MOVEMENT_WRITER_SOURCE_COMPONENT ...
MOVEMENT_CALLSITE_PROOF ... direct_call_verified=...
MOVEMENT_CALLER_SOURCE_PROOF ... writable_source_proven=0|1 ...
MOVEMENT_CALLER_SOURCE_COMPONENT ...
MOVEMENT_LINEAGE_UNSUPPORTED ... (if any)
```

## Required per-session evidence
For both A and E, require the old orientation/lifecycle gate to remain clean:

```text
verified_deinit=1
source_active_until_deinit=1
retention_pass=1
first_hard_kill=none
Position_writes_by_mod=0
cleanup_pending=0
```

Also collect and adjudicate:

```text
MOVEMENT_WRITER_SOURCE_MAP ...
MOVEMENT_SOURCE_CORRELATION ...
MOVEMENT_LINEAGE_SUMMARY ...
POSITION_WRITER_SUMMARY ...
```

The c12 reconstruction is post-store/trap-time evidence, not a true function-entry snapshot. `entry_snapshot_captured=0`, a partial scalar-store mismatch, or an unavailable overwritten alias must remain explicit rather than being inferred away.

## Gate decision after the human log
Classify the result into exactly one of these outcomes.

### A. Full lineage proof
Only if actual-game evidence independently establishes all required source identity, caller relation, XYZ/W behavior, ownership/lifetime and exclusive intended translation effects may a candidate reach:

```text
writable_source_proven=1
```

Even then, do not silently implement movement. First update `docs/PROJECT_STATE.md` with the exact admitted write contract and create a separate minimal next task for a bounded SurfaceFrame source proof. Direct Transform Position spam remains forbidden.

### B. Diagnostic PASS / exact missing edge
If writer/callsite/source correlation works but one or more semantic/entry/CFG/ownership edges remain unknown, record the exact blocker and design the smallest read-only diagnostic needed to resolve only that edge. Do not broaden instrumentation or infer typed semantics from arithmetic shape.

### C. Regression / safety failure
If orientation, exact-build admission, hardware-watch cleanup, target identity or process stability regresses, stop lineage advancement and repair that regression first. No movement/adhesion/input/camera work is authorized.

## Safety constraints
Preserve all existing exact-build/fail-closed behavior and independent implementation constraints. No reference-mod code/assets and no private full game-function body/raw user log in the repository or package.

Until the actual-game gate authorizes otherwise, no:
- direct Actor/Transform Position write;
- Path A Position or c12 source write;
- controller/keyboard input hook or rewrite;
- custom velocity/gravity;
- active adhesion correction;
- PAGE_GUARD/protection mutation;
- animation/camera/wall jump/corners/ceilings/parkour/swing/combat.

Deferred body lean and wall-standoff/foot-floating remain visual/contact work for later.

## Required handoff after runtime adjudication
Update:

```text
analysis/V01C12_RUNTIME_RESULT.md
docs/PROJECT_STATE.md
docs/CODEX_NEXT.md
```

`docs/CODEX_NEXT.md` must be based only on evidence from the actual c12 log. If no safe source is admitted, the next task must remain diagnostic and target the smallest identified missing edge.
