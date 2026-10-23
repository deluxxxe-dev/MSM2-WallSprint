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

A wall c9 full lifecycle retention passed. E-wall c9 confirmed generic diagonal Surface Up but exposed a small remaining publication race.

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

## Orientation history

- c2: visible surface-relative rotation, rapid WorldUp/SurfaceUp flicker.
- c3: multithread writer tracing attempted; production expansion failed.
- c4: thread-watch repaired; first retention PASS, but source ownership ended early.
- c5: lifecycle continuity repaired; real full-WallRun orientation hold passed.
- c6: Position/adhesion remained read-only; a real ~250 ms native Update gap caused `fresh_update_age` to permanently end source ownership.
- c7: added transient stale suspend/resume, but real run hard-killed orientation after only two Path A writes / ~16 ms.
- c8: exact first-cause attribution succeeded; a healthy secondary-thread Path B call was falsely treated as fatal `path_b_wrong_thread`.
- c9 A wall: secondary-thread Path B admission passed real runtime with 193/193 secondary overrides and clean Deinit.
- c9 E wall: diagonal Surface Up and Deinit lifecycle passed, but two secondary calls were rejected on `secondary_source_publication_changed`; each rejection correlated with a one-observation WorldUp flash.

## Authoritative c9 A-wall PASS

Implementation:

```text
branch: codex/v01c9-secondary-path-b-thread-admission
implementation commit: 0c276126ccac8e3bd2a6dd1d850da63e80bc35ce
published head: b865dabfa81626248ea8a8ae56a1dd1c992d7301
version: v0.1c9 / package 0.1.27
```

Real A runtime:

```text
wallrun_duration_ms=3844
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
path_a_writes=239
path_b_writes=238
retention_pass=1
max_held_ms=2859
max_held_updates=221
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
secondary seen/admitted/overridden/rejected=193/193/193/0
```

Detailed result: `analysis/V01C9_RUNTIME_RESULT.md`.

## c9 diagonal E-wall result

The tested surface was genuinely diagonal:

```text
N398 ≈ (-0.70710677, 0, +0.70710677)
```

The target and actor reached the matching Surface Up:

```text
target_up ≈ (-0.70710671, 0, +0.70710683)
actor_up_dot_surface=1
```

Lifecycle summary:

```text
wallrun_duration_ms=4157
source_entry_elapsed=0.703000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=231
path_b_writes=228
retention_pass=1
max_held_ms=1391
max_held_updates=90
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
```

However, exactly two short WorldUp observations occurred while source ownership remained active. Both directly followed the only two secondary-thread Path B rejects:

```text
action=reject
reason=secondary_source_publication_changed
```

Secondary summary:

```text
seen=112
admitted=110
overridden=110
rejected=2
unique_threads=16
```

At each rejection, the vanilla Path B payload had WorldUp. About 10 ms later Actor Up was observed as `(0,1,0)`, then the next common Path A/source call detected `late_worldup_source_detected=1` and restored diagonal Surface Up. The human tester did not notice these visually.

This is strong evidence that c9's exact `publishedMs` snapshot equality is too strict for a concurrent identity-preserving publication refresh. It should not be relaxed across real identity/lifecycle changes.

Detailed result: `analysis/V01C9_E_RUNTIME_RESULT.md`.

## Planar standoff status

Position remains read-only:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
```

A wall showed essentially exact 0.5 m standoff. E wall showed small natural variation, roughly 0.479–0.511 m in sampled examples. Therefore active adhesion correction remains deferred/observational; do not add it before movement unless later evidence requires it.

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
12. transient freshness recovery implementation ✅ synthetic
13. exact hard-kill attribution ✅ c8
14. secondary-thread Path B admission ✅ c9
15. lifecycle-wide real orientation hold on A wall ✅ c9
16. diagonal E-wall Surface Up generality ✅ c9
17. eliminate secondary publication-race WorldUp flash ▶ CURRENT
18. active planar adhesion correction ⏸ optional/deferred
19. surface-relative movement ⬜
20. animation integration ⬜
21. camera and later roadmap ⬜

## Current implementation base

Use published c9 exactly:

```text
branch: codex/v01c9-secondary-path-b-thread-admission
implementation commit: 0c276126ccac8e3bd2a6dd1d850da63e80bc35ce
published head: b865dabfa81626248ea8a8ae56a1dd1c992d7301
```

Pre-created next branch:

```text
codex/v01c10-secondary-publication-race
```

## Current objective

Repair only the demonstrated c9 secondary Path B publication race.

A secondary call must not be rejected merely because the lock-free source publication advanced to a newer snapshot during its validation when the newer snapshot is still the same active WallRun ownership identity. Use a bounded refresh/revalidation approach, recomputing the secondary applied orientation from the latest admitted basis before store.

Real changes to session, owner, Hero, Transform, generation, lifecycle epoch, native token identity/thread, accepted/collision epoch, source integrity, caller, stack provenance, compact proof, stale/close/F8/shutdown must remain fail-closed or rejected as appropriate.

Do not begin surface-relative movement until the same E-wall regression produces no WorldUp actor observations after full Surface Up is reached.