# WallSprint project state

## Goal

Convert vanilla WallRun into a surface-relative locomotion mode. The architecture is surface-relative locomotion, not an extended vanilla WallRun.

## Roadmap

- v0.0 reference analysis / instrumentation
- v0.1 locomotion MVP
- v0.2 surface-relative camera
- v0.3 wall jump / wall-to-wall
- v0.4 corners / ceilings / continuous surfaces
- v0.5 parkour
- v0.6 web swing
- v0.7+ combat

Do not advance to camera until planar v0.1 movement is robust.

## Verified build / lifecycle

Target:

- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`

Verified WallRun lifecycle RVAs:

- Init `0x14233C0`
- Entry `0x14235C0`
- Update `0x1424530`
- Deinit `0x1416660`

## Surface data

Verified on cardinal and diagonal planar vertical walls:

- `WallRun.this + 0x398` = candidate outward surface normal `N398`
- `WallRun.this + 0x338` = candidate surface-plane reference point `P338`
- `WallRun.this + 0x320` = hero-side WallRun reference point `P320`
- `P320 - P338 ≈ 0.5 * N398`
- `dot(HeroPosition - P338, N398) ≈ 0.5 m`

## Orientation ownership

Path A:

- writer RVA `0x2CFC580`
- WallRun caller return RVA `0x2CFC8F3`
- caller-local source Right / Up / Forward / Position
- orientation owns `[source+0x00, source+0x30)`; Position remains unchanged

Path B:

- writer RVA `0x2CFC3C0..0x2CFC57C`, size 444
- return RVA `0x2CFC84B`
- compact 3x3 source at `RDX = writer-entry RSP + 0x28`
- real-game compact proof passes
- valid active-Hero Path B calls may occur on multiple secondary threads; per-call provenance is used instead of a fatal equality-to-native-thread invariant
- c10 adds one bounded semantic-publication refresh/revalidation path for identity-preserving concurrent updates

## Orientation history

- c2: visible surface-relative rotation, rapid WorldUp/SurfaceUp flicker.
- c3: multithread writer tracing attempted; production expansion failed.
- c4: thread-watch repaired; first retention PASS, but source ownership ended early.
- c5: lifecycle continuity repaired; real full-WallRun orientation hold passed.
- c6: Position/adhesion remained read-only; a real ~250 ms native Update gap caused `fresh_update_age` to permanently end source ownership.
- c7: added transient stale suspend/resume, but real run hard-killed orientation after only two Path A writes / ~16 ms.
- c8: exact first-cause attribution succeeded; a healthy secondary-thread Path B call was falsely treated as fatal `path_b_wrong_thread`.
- c9 A wall: secondary-thread Path B admission passed real runtime with 193/193 secondary overrides and clean Deinit.
- c9 E wall: diagonal Surface Up passed, but two `secondary_source_publication_changed` rejects produced two ~10 ms WorldUp observations.
- c10: one bounded publication-refresh/revalidation path implemented. Human cardinal/diagonal regression produced clean full-lifecycle orientation with no WorldUp observations, no secondary rejects and no hard kill. The natural refresh path itself did not fire in that human run; deterministic c10 fixtures cover the actual race branch.

## Authoritative c10 implementation

```text
branch: codex/v01c10-secondary-publication-race
implementation commit: 609fdf935e40947d7aad29fea4bed5f2cf3d0e2e
published/delivery head: 6246fd6d2e52cf81e18e6ff8ffd4d8ee4d863aa1
version: v0.1c10 / package 0.1.28
```

## Authoritative c10 human regression

The private runtime log contains two clean WallRun sessions.

### Session 1 — diagonal planar surface

```text
N398 ≈ (-0.70710677, 0, +0.70710677)
wallrun_duration_ms=4172
source_entry_elapsed=0.719000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=221
path_b_writes=220
retention_pass=1
max_held_ms=3187
max_held_updates=204
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
secondary seen/admitted/overridden/rejected=102/102/102/0
```

### Session 2 — cardinal planar surface

```text
wallrun_duration_ms=3421
source_entry_elapsed=0.718000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=151
path_b_writes=152
retention_pass=1
max_held_ms=2453
max_held_updates=140
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
secondary seen/admitted/overridden/rejected=161/161/161/0
```

Across the human c10 log:

```text
actor_up=(0,1,0) after full transition: 0 observed
secondary_source_publication_changed: 0 observed
secondary publication refresh events: 0 observed
Position writes: 0
```

Interpretation: real planar orientation stability is accepted. The c9 failure did not recur. Do not claim that a natural real-game c10 refresh race was observed; it was not.

Detailed result: `analysis/V01C10_RUNTIME_RESULT.md`.

## Planar standoff / visual alignment

Position remains read-only:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
```

Measured root standoff is centered around the vanilla ~0.5 m relation. The user visually identified two remaining presentation issues:

1. the wall-running body appears leaned toward the apparent ground side compared with ordinary ground-running animation;
2. feet/body appear too far from the wall and therefore visually float despite the diagnostic root standoff being stable.

These are **deferred visual-alignment tasks**, not current orientation blockers. Later create a dedicated pass for root/surface offset, contact appearance and animation/root lean. Do not add active normal-direction adhesion merely from the current root-distance metric.

## Previous Position proof status

The c6 Path A Position proof remains **NOT ADMITTED**. Known source layout has a candidate Position Vec4 at `source+0x30`, but accepted-build complete causal propagation, destination W semantics and exclusive side effects were not proven. Equal source/Hero coordinates are not enough.

This unresolved proof is directly relevant to the next movement-path task and should be revisited using bounded runtime position-writer attribution rather than assumed safe.

See `analysis/PATH_A_POSITION_PROOF.md`.

## Current blocker / stage

1. WallRun detection ✅
2. Actor position/orientation observation ✅
3. surface normal ✅
4. planar surface point / standoff data ✅
5. SurfaceFrame basis ✅
6. Path A source injection ✅
7. Path B discovery / compact layout ✅
8. real-runtime Path B proof ✅
9. Path A + Path B SurfaceFrame writes ✅
10. visible surface-relative rotation ✅
11. retention threshold ✅
12. transient freshness recovery ✅ synthetic + later stable runtime
13. exact hard-kill attribution ✅ c8
14. secondary-thread Path B admission ✅ c9
15. diagonal/general Surface Up ✅ c9/c10
16. publication-race regression clean ✅ c10 human A/E; natural refresh branch not exercised
17. planar orientation-stability milestone ✅ COMPLETE
18. safe movement-path / Position-writer ownership ▶ CURRENT
19. surface-relative movement input ⬜
20. visual alignment / contact / standoff refinement ⏸ deferred
21. animation integration ⬜
22. camera and later roadmap ⬜

## Current implementation base

Use c10 delivery head exactly:

```text
branch: codex/v01c10-secondary-publication-race
implementation commit: 609fdf935e40947d7aad29fea4bed5f2cf3d0e2e
published head: 6246fd6d2e52cf81e18e6ff8ffd4d8ee4d863aa1
```

Pre-created next branch:

```text
codex/v01c11-movement-path-ownership
```

## Current objective

Discover and authenticate the native/caller-local path that actually advances Hero translation during verified WallRun, so later surface-relative input can be implemented upstream without direct Actor Position spam.

c11 is primarily diagnostic/read-only. Prefer proving a safe movement source over modifying movement. In particular:

- reuse c10 orientation behavior unchanged;
- use bounded Position-writer observation and complete static/runtime attribution;
- decompose observed translation deltas in the SurfaceFrame (`normal`, tangent-forward, tangent-right);
- revisit the Path A `source+0x30` Position candidate only with actual causal/post-writer evidence;
- identify any safer velocity/displacement/input source upstream if present;
- do not write Position or movement unless the task independently proves a safe caller-local/native source and the handoff explicitly permits a minimal bounded proof write.

No camera, animation, wall jump, corners/ceilings, swing reconnect or combat yet.
