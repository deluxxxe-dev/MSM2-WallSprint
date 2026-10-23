# Next task — WallSprint v0.1c13 Actual-Game Writer-Entry Gate

## Base
```text
branch: codex/v01c13-writer-entry-snapshot
implementation: eb257e340be347d6ac17358efeb61eaa6a83066e
published delivery: 77c8a072c5401ced0dd427e764c142ab0780bf76
version: v0.1c13 / package 0.1.31
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, `analysis/V01C12_RUNTIME_RESULT.md`, `analysis/MOVEMENT_WRITER_ENTRY_SNAPSHOT.md`, `analysis/MOVEMENT_ENTRY_SOURCE_CANDIDATES.md`, and `analysis/V01C13_GATE.md`.

## Status
C13 cloud implementation is complete and validated. It captures true pre-body entry state for the verified `0x2CFC900` writer and pairs bounded invocations to the existing Position hardware-store events. It remains diagnostic only.

Authoritative gate remains:
```text
writable_source_proven=0
Position/source/input/movement writes=0
```
No c14 movement/input/adhesion/camera implementation is authorized before actual-game c13 evidence is adjudicated.

## Immediate task: human runtime evidence
Install `WallSprint_v0.1c13_writer_entry_snapshot.zip` and use ordinary input:
```text
1. A/cardinal planar WallRun ~4 s
2. fully end WallRun
3. move to E
4. E/diagonal planar WallRun ~4 s
5. return the single complete startup-through-cleanup WallSprint log
```
Video is optional and only needed for a visible anomaly or a later targeted reproduction request.

## Required evidence
Preserve the old exact-build, dual-source, orientation/lifecycle, Position-write and debug-restoration gates. Require both A and E to retain verified Deinit, source active until Deinit, retention pass, hard kill 0, Position writes by mod 0, and clean cleanup.

Collect and adjudicate the new c13 records, including:
```text
MOVEMENT_WRITER_ENTRY_GATE / START / ACTIVE
MOVEMENT_WRITER_ENTRY_SNAPSHOT
MOVEMENT_WRITER_INVOCATION_MAP
MOVEMENT_ENTRY_SOURCE_CANDIDATE
MOVEMENT_CALLER_CHAIN_RUNTIME
MOVEMENT_WRITER_ENTRY_STOP
```
Exact refusal, partial-map, cap, stale, unresolved-chain, or cleanup reasons are evidence; do not weaken gates to remove them.

## Decision after the human log
### A. Stable entry origin identified
If the same bounded origin supplies the executed Position-store bits across A and E, record the exact origin and correlation. Keep `writable_source_proven=0` unless storage lifetime, XYZ/W causality, producer provenance, downstream conflicts, and exclusive intended translation effect are also independently proven. The next task should target only the smallest missing producer/storage edge.

### B. No stable origin / partial map
Use the entry lane masks and anchored store operands to identify the smallest missing computation around the known writer edge. Add only the minimal read-only diagnostic needed for that edge. Do not broaden into general CFG tracing.

### C. Regression / safety failure
If c13 introduces orientation regression, hitch/freeze/crash, hard cause, identity failure, or cleanup/debug-slot failure, stop lineage advancement and repair that issue first.

## Safety
Preserve exact-build/fail-closed behavior and independent implementation constraints. No direct Actor/Transform Position write, source mutation, controller/input rewrite, custom movement/velocity/gravity, active adhesion, PAGE_GUARD/data-page protection mutation, animation, camera, corners, ceilings, parkour, swing, or combat work.

Do not classify a candidate as velocity, displacement, desired direction, or root motion without independent evidence. Do not publish raw user logs or complete private game-function bodies.

## Handoff after runtime adjudication
Update:
```text
analysis/V01C13_RUNTIME_RESULT.md
docs/PROJECT_STATE.md
docs/CODEX_NEXT.md
```
Only define a c14 implementation task after the actual-game c13 log identifies a specific unresolved edge. A writable movement rewrite remains a separate later gate.