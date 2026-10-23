# Next task — WallSprint v0.1c2 Real-Game Orientation Gate

## Status

The c2 proof-repair implementation is complete and published at:

```text
branch: codex/v01c2-proof-fix
implementation commit: 2d26710034dcf695fa7bb2da763f117faf6171db
published branch head: 727fcdc1594e36e6a1ffaabf5569eee89375b871
package: WallSprint_v0.1c2_real_runtime_proof_fix.zip
version: v0.1c2 / 0.1.20
```

The real writer444/caller128 regression now passes the complete startup proof: 444/444 writer bytes, 128/128 caller bytes, nine recovered copy pairs. The old c1 failure is independently reproduced by the retained old decoder. Exact-build/fail-closed behavior and c1 lock-free dual-source ownership logic are preserved.

## Objective

Do not make another implementation change yet. The next task is to evaluate the authoritative real-game v0.1c2 log after the human A/E run.

This remains orientation-only. Do not add adhesion, movement, animation, camera, physics, gravity, parkour, swing or combat unless the real-game orientation retention gate below passes.

## Required startup gate

Before interpreting A/E behavior require:

```text
WallSprint v0.1c2 package_version=0.1.20
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1 caller_verified=1 writer_verified=1 rdx_to_rcx_map_verified=1 source_bytes=0x24 decoded_prefix_bytes=444 caller_decoded_bytes=128 pairs=9
RESET_COPY_HOOK admitted=1 installed=1
DUAL_SOURCE_ADMISSION admitted=1
PATH_B_LAYOUT_CONFIG ... path_b_override=1
```

If startup proof/admission fails, stop there and diagnose that failure. Do not weaken hashes, caller identity, decode completeness, stack relation or exact-build gates.

## Human run

A Control:
- one left-to-right WallRun, about 1.5 s.

E diagonal:
- same known diagonal face;
- one left-to-right WallRun, about 2 s.

Expected visible success is sideways Actor rotation while the camera remains world-up. Animation may be visually incorrect at this milestone.

## Runtime evidence to extract

For each active WallRun session, determine:

```text
PATH_A_OVERRIDE_COUNT
PATH_A_REJECT_COUNT
PATH_A_STATE_LOCK_CONTENTION
PATH_B_OVERRIDE_COUNT
PATH_B_REJECT_COUNT
PATH_B_COMPACT3X3_OVERRIDE
ORIENTATION_RETENTION_PASS
NEW_RESET_AFTER_DUAL_OVERRIDE
```

Also verify no crash, Transform corruption, Position corruption or metadata write occurred.

## Decision tree

### A. Startup proof fails

Treat as proof/authentication blocker. Diagnose the exact first-failure line and admission fields. Do not change orientation semantics.

### B. Path A or Path B override count stays zero

Identify the exact admission rejection from runtime evidence: lifecycle, target, thread, stack, freshness, row magnitude, caller identity or other bounded gate. Fix only the demonstrated gate/implementation defect; do not broaden writes speculatively.

### C. Both override counts are positive but Up resets

Use `NEW_RESET_AFTER_DUAL_OVERRIDE` plus timing/caller evidence to identify the next reset owner. A new writer must be characterized read-only before any new write experiment. Do not use direct Actor spam writes.

### D. Retention passes

Runtime authorization requires:

```text
PATH_A_OVERRIDE_COUNT > 0
PATH_B_OVERRIDE_COUNT > 0
ORIENTATION_RETENTION_PASS=1
```

in the same verified active session, with:

```text
dot(ActorUp,N398) >= 0.98
```

for >=300 ms or >=20 distinct native updates.

Only after this real-game gate passes may `docs/PROJECT_STATE.md` advance orientation ownership to complete and `docs/CODEX_NEXT.md` define the first adhesion milestone. Do not implement adhesion in the same handoff that merely observes retention PASS; first record the runtime evidence and define a separate bounded task.

## Safety / distribution constraints

Preserve `AGENTS.md` and `docs/DEV_RULES.md`.

- exact Spider-Man2.exe build only;
- fail closed on identity/proof/lifecycle/target/stack/thread/freshness failures;
- independent WallSprint implementation suitable for Nexus Mods;
- no copied reference-mod code/binaries/assets;
- no private game-function captures in Git or release packages;
- Path B remains limited to the first 0x24 source bytes;
- Path A preserves Position;
- no direct Actor Transform write;
- no adhesion/movement/animation/camera until the orientation gate authorizes the next milestone.

## Next input

Authoritative input is the complete `WallSprint.log` from the v0.1c2 A/E run plus the human visible-change observation. Update `docs/PROJECT_STATE.md` and replace this task only after that evidence exists.
