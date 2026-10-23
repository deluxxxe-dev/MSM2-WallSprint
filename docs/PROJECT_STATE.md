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

- `WallRun.this + 0x398` = outward surface normal candidate `N398`
- `WallRun.this + 0x338` = surface-plane point candidate `P338`
- `WallRun.this + 0x320` = hero-side WallRun reference candidate `P320`
- `P320 - P338 ≈ 0.5 * N398`
- `dot(HeroPosition - P338, N398) ≈ 0.5 m`

## Orientation ownership

Path A:

- writer RVA `0x2CFC580`
- WallRun caller return RVA `0x2CFC8F3`
- caller-local source Right / Up / Forward / Position candidate
- orientation owns `[source+0x00, source+0x30)`; Position remains unchanged by the mod

Path B:

- writer RVA `0x2CFC3C0..0x2CFC57C`, size 444
- return RVA `0x2CFC84B`
- compact 3x3 source at `RDX = writer-entry RSP + 0x28`
- valid active-Hero calls can occur on multiple secondary threads
- c10 supports one bounded identity-preserving publication refresh/revalidation

## Orientation status

Planar orientation stability is accepted.

History:

- c2: visible surface-relative rotation, rapid WorldUp/SurfaceUp flicker.
- c3/c4: multithread writer tracing and first retention pass.
- c5: lifecycle-wide orientation hold through verified Deinit.
- c6/c7: transient native Update gap / over-broad kill regressions exposed.
- c8: exact first-cause attribution.
- c9: valid secondary-thread Path B calls admitted; A wall full hold passed.
- c9 E: diagonal Surface Up passed but two short publication-race WorldUp flashes remained.
- c10: bounded publication refresh/revalidation implemented; human A/E regression had zero observed WorldUp flashes, zero hard kill, clean Deinit.
- c11: Position-writer diagnostics did not regress orientation; A and E both held through Deinit.

## Authoritative c11 implementation

```text
branch: codex/v01c11-movement-path-ownership
implementation commit: 2129e69fe003e71bf705fb67f723b447cf3e3764
published/delivery head: 9593f0230e220770f2b34faf512ca4108c1825cc
version: v0.1c11 / package 0.1.29
```

## Authoritative c11 human A -> E result

### Session 1 — A/cardinal

```text
N398 ≈ (+1,0,0)
wallrun_duration_ms=4125
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
max_held_ms=3157
max_held_updates=185
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
Position_writes_by_mod=0
```

### Session 2 — E/diagonal

```text
N398 ≈ (-0.7071068,0,+0.7071068)
wallrun_duration_ms=4063
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
max_held_ms=3093
max_held_updates=189
last_actor_up_dot_surface≈0.99999994
first_hard_kill=none
hard_kill_calls=0
Position_writes_by_mod=0
```

Detailed result: `analysis/V01C11_RUNTIME_RESULT.md`.

## c11 Position writer attribution

Both Position-watch windows armed and restored cleanly:

```text
watch_armed=1
threads_seen=128
threads_installed=126
unique_writers=14
total_write_events=256
cleanup_restored=1
cleanup_pending=0
Position_writes_by_mod=0
```

The main Transform Position writer family observed during the movement window is:

```text
writer_function_rva=0x2CFC900
store RVAs=0x2CFC936 / 0x2CFC93C / 0x2CFC942 / 0x2CFC945
Transform destinations=+0x30 / +0x34 / +0x38 / +0x3C
```

The call chain that produced real translation on both A and E is centered on:

```text
caller return RVA=0x351EEB0
caller2 return RVA=0x351A020
consumer writer=0x2CFC900
```

Runtime examples show this path changing actual Hero XYZ and correlating with surface-tangent translation. Finite-difference movement samples on both A and E remain predominantly tangent to the wall, with small normal drift on E.

## Path A Position status

Path A `source+0x30` is now stronger than a mere layout guess: runtime observed Path A touching the Position row, and some events produced full-row post-writer equality with the caller-local candidate.

However **Path A Position is still NOT ADMITTED for modification**:

```text
writer_verified=1
caller_verified=1
stack_relation_verified=1
post_writer_match=1
position_w_observed=1
exclusive_effects_proven=0
write_permission=0
reason=whole_function_CFG_caller_construction_W_and_exclusive_effects_not_proven
```

Downstream Position writes are observed after Path A. Therefore Path A Position is not yet proven to be the correct locomotion ownership point.

## Safe movement source status

No real c11 candidate reached:

```text
writable_source_proven=1
```

Current strongest runtime target for lineage work:

```text
0x351EEB0 -> 0x2CFC900
caller2 ≈ 0x351A020
```

c11 proves writer identity and translation correlation only. It does not yet prove whether the upstream payload is target Position, displacement, velocity, desired direction, root motion or another movement vector.

## Planar standoff / visual alignment

Deferred presentation issues remain:

1. apparent wall standoff / floating feet;
2. body/root lean relative to the apparent surface.

These are not current movement-ownership blockers. Do not solve them in the next lineage task.

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
11. retention / lifecycle stability ✅
12. secondary-thread Path B admission ✅
13. diagonal/general Surface Up ✅
14. publication-race repair/regression ✅
15. planar orientation milestone ✅ COMPLETE
16. bounded Hero Position writer attribution ✅ c11
17. SurfaceFrame translation decomposition ✅ c11
18. safe upstream movement-source lineage ▶ CURRENT
19. surface-relative input rewrite ⬜
20. visual alignment / contact refinement ⏸ deferred
21. animation integration ⬜
22. camera and later roadmap ⬜

## Current implementation base

Use c11 delivery head exactly:

```text
branch: codex/v01c11-movement-path-ownership
implementation commit: 2129e69fe003e71bf705fb67f723b447cf3e3764
published head: 9593f0230e220770f2b34faf512ca4108c1825cc
```

Pre-created next branch:

```text
codex/v01c12-movement-source-lineage
```

## Current objective

Authenticate the real movement-producing caller/source chain centered on `0x351EEB0 -> 0x2CFC900`.

The next revision remains read-only with respect to movement unless it independently proves an upstream source contract strong enough for a future bounded source rewrite. Determine exact writer-entry/caller-local provenance, full XYZ/W semantics, caller construction, upstream producer and whether a tangent displacement/velocity/direction/root-motion vector exists before final Transform Position.

Do not begin direct Position writes, input rewriting, custom gravity, camera, animation, wall jump, corners/ceilings, swing reconnect or combat yet.
