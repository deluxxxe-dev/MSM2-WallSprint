# Next task — WallSprint v0.1c14 Actual-Game RDX Source-Contract Gate

## Base
```text
branch: codex/v01c14-rdx-source-contract
implementation commit: 617d7b6a84f752582aebd7be8a766c5ef02e476a
delivery head: 5fd2cdff9ae6732cf0eaa53fec02253c4d6ae25c
version: v0.1c14 / package 0.1.32
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `analysis/V01C13_RUNTIME_RESULT.md`, c14 `docs/CODEX_RESULT.md`, and the c14 contract analyses first.

## Status
C14 cloud implementation is complete and validated. It adds read-only decoded RDX XYZ / independent W, bounded caller-storage ownership and producer diagnostics, and fixes c13 true-entry/XYZ aggregate reporting. It does not authorize or implement movement/source/input/Position mutation.

Authoritative gate remains:
```text
writable_source_proven=0
Position/source/input/movement writes=0
```

Accepted c13 runtime evidence remains: 16 true entries across A/E, every sampled invocation paired to all four stores, `RDX=entry RSP+0x28`, XYZ exact store-bit match (`mask=0x7`), sourceW0/destinationW1. These equalities are not by themselves causal, lifetime, ownership, downstream-conflict or writable-source proof.

## Immediate task: human runtime evidence
Install `WallSprint_v0.1c14_rdx_source_contract.zip` and use ordinary input:
```text
1. A/cardinal planar WallRun ~4 s
2. fully end WallRun
3. move or fast-travel to E
4. E/diagonal planar WallRun ~4 s
5. return one complete startup-through-cleanup WallSprint log
```
Video is optional unless a visible anomaly occurs or a later targeted reproduction is requested.

## Required evidence
Preserve exact-build, lifecycle/orientation, hard-kill, Position-watch, debug-register and entry-hook cleanup gates. Require both A and E to retain verified Deinit, Surface Up retention, hard kill0 and `Position_writes_by_mod=0`.

Adjudicate at minimum:
```text
MOVEMENT_STATIC_CONTRACT
MOVEMENT_RDX_XYZ_PROOF
MOVEMENT_POSITION_W_PROOF
MOVEMENT_RDX_STORAGE_PROOF
MOVEMENT_RDX_PRODUCER_PROOF
MOVEMENT_ENTRY_CAPTURE_SUMMARY
```

Exact refusal RVA/mnemonic, partial coverage, inherited RDX, unresolved producer edge, cap, stale, unreadable event, cleanup failure or code recheck failure are evidence. Do not weaken gates to remove them.

## Decision after the human log
### A. XYZ/W/storage/producer contract advances
Record only facts directly established by actual-game diagnostics. If RDX XYZ causality, W behavior and a bounded producer/storage relation are established, define the next task around the smallest remaining lifetime/downstream/exclusive-effect edge. Keep `writable_source_proven=0` until all write-contract requirements are independently proven.

### B. Exact barrier/refusal remains
Use the emitted first barrier RVA/mnemonic, inherited-storage result, anchored parent call target/next destination, or constructor/barrier RVA to define one minimal read-only diagnostic. Do not broaden into whole-engine CFG tracing.

### C. Regression / safety failure
If c14 causes orientation regression, hitch/freeze/crash, hard cause, identity failure, Position write, DR restoration failure or entry-hook cleanup failure, stop source-lineage advancement and repair that issue first.

## Write gate
Do not set `writable_source_proven=1` unless actual-game evidence independently proves all of:
- exact XYZ/W causal feed;
- exact source storage and lifetime;
- Hero/session/thread/owner identity;
- stable producer provenance;
- absence or understood handling of relevant downstream conflicting overwrites;
- exclusive intended Hero translation effect.

Even if that gate is eventually satisfied, movement mutation must be a separate explicit implementation task. Do not silently add it during c14 adjudication.

## Safety
No Actor/Transform Position write, RDX/source mutation, Path A Position mutation, controller/input rewrite, custom velocity/gravity, active adhesion, camera/animation/traversal work, PAGE_GUARD/data-page protection mutation, guessed native ABI/game call, raw user-log publication or full private game-body distribution. Preserve exact-build/fail-closed behavior and independent Nexus-compatible implementation constraints.

## Handoff after runtime adjudication
Update:
```text
analysis/V01C14_RUNTIME_RESULT.md
docs/PROJECT_STATE.md
docs/CODEX_NEXT.md
```
Only define a c15 implementation task after the actual-game c14 log identifies a specific unresolved edge.
