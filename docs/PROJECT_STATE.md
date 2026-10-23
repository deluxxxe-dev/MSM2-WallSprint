# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance camera until planar v0.1 movement is robust.

## Verified target
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`

## Surface data
- `this+0x398` = outward normal candidate N398
- `this+0x338` = plane reference P338
- `this+0x320` = hero-side reference P320
- `P320-P338 ≈ 0.5*N398`
- root standoff is centered around ~0.5 m in current planar WallRun

## Orientation ownership
Path A:
- writer `0x2CFC580`, return `0x2CFC8F3`
- source Right/Up/Forward/Position
- orientation owns only `[source+0x00, source+0x30)`

Path B:
- writer `0x2CFC3C0..0x2CFC57C`
- return `0x2CFC84B`
- compact 3x3 at `RDX=entryRSP+0x28`
- valid owned calls may occur on multiple threads
- c10 adds one bounded identity-preserving publication refresh/revalidation

## c10 implementation
```text
branch: codex/v01c10-secondary-publication-race
implementation commit: 609fdf935e40947d7aad29fea4bed5f2cf3d0e2e
published head: 6246fd6d2e52cf81e18e6ff8ffd4d8ee4d863aa1
version: v0.1c10 / package 0.1.28
```

## c10 human A/E regression — PASS
Two real sessions reached verified Deinit with full Surface Up retention.

Diagonal session:
```text
N398≈(-0.70710677,0,+0.70710677)
wallrun_duration_ms=4172
source_active_until_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
hard_kill_calls=0
secondary=102/102/102/0 seen/admitted/overridden/rejected
```

Cardinal session:
```text
wallrun_duration_ms=3421
source_active_until_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
hard_kill_calls=0
secondary=161/161/161/0
```

Across the real c10 log:
```text
post-transition actor_up=(0,1,0): 0 observed
secondary_source_publication_changed: 0 observed
Position writes by mod: 0
```
The new c10 refresh path did not naturally fire in this real run; deterministic fixtures remain the evidence for the actual race branch.

## Deferred visual alignment
The user visually observed:
1. body/root lean toward the apparent ground side compared with normal ground running;
2. feet/body appear too far from the wall and visually float.

These are later visual-alignment/contact tasks. Do not solve them in c11 and do not infer an active adhesion requirement from the current root-distance metric.

## Position proof status
c6 `source+0x30` Position remains NOT ADMITTED. Source/Hero equality before the writer did not prove complete causality, destination W semantics, downstream overwrite behavior or exclusive effects. See `analysis/PATH_A_POSITION_PROOF.md`.

## Current stage
1. WallRun lifecycle ✅
2. SurfaceFrame / normal ✅
3. Path A/B orientation ownership ✅
4. multi-thread/reset/race stabilization ✅
5. cardinal + diagonal full lifecycle orientation ✅ COMPLETE
6. safe movement-path / Position-writer ownership ▶ CURRENT
7. surface-relative input movement ⬜
8. visual alignment/contact refinement ⏸ deferred
9. animation ⬜
10. camera/later roadmap ⬜

## Current base
```text
base: 6246fd6d2e52cf81e18e6ff8ffd4d8ee4d863aa1
branch: codex/v01c11-movement-path-ownership
```

## Current objective
Identify the native/caller-local path that actually advances Hero translation during verified WallRun. Prefer an upstream displacement/velocity/direction/root-motion source over direct Transform Position mutation. c11 is diagnostic/read-only for movement and Position; preserve c10 orientation unchanged.
