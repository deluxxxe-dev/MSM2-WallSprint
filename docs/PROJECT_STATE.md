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

The current A-direction control wall has now passed full lifecycle retention with c9.

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
- c9 proves valid active-Hero Path B calls may occur on multiple secondary threads; per-call provenance is used instead of a fatal equality-to-native-thread invariant

## Orientation history

- c2: visible surface-relative rotation, rapid WorldUp/SurfaceUp flicker.
- c3: multithread writer tracing attempted; production expansion failed.
- c4: thread-watch repaired; first retention PASS, but source ownership ended early.
- c5: lifecycle continuity repaired; real full-WallRun orientation hold passed.
- c6: Position/adhesion remained read-only; a real ~250 ms native Update gap caused `fresh_update_age` to permanently end source ownership.
- c7: added transient stale suspend/resume, but real run hard-killed orientation after only two Path A writes / ~16 ms.
- c8: exact first-cause attribution succeeded; a healthy secondary-thread Path B call was falsely treated as fatal `path_b_wrong_thread`.
- c9: secondary-thread Path B per-call admission implemented and real-game full lifecycle retention passed.

## Authoritative c9 real A-wall PASS

Implementation:

```text
branch: codex/v01c9-secondary-path-b-thread-admission
implementation commit: 0c276126ccac8e3bd2a6dd1d850da63e80bc35ce
published head: b865dabfa81626248ea8a8ae56a1dd1c992d7301
version: v0.1c9 / package 0.1.27
```

Real runtime:

```text
wallrun_duration_ms=3844
source_entry_elapsed=0.719000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=239
path_b_writes=238
retention_pass=1
max_held_ms=2859
max_held_updates=221
last_actor_up_dot_surface=1
stale_suspend_count=0
stale_resume_count=0
first_hard_kill=none
hard_kill_calls=0
```

Independent WallRun lifecycle:

```text
Init=1
Entry=1
Update=300
Deinit=1
duration=3.836902 s
end_reason=verified_deinit
```

Secondary-thread Path B was heavily exercised:

```text
seen=193
admitted=193
overridden=193
rejected=0
unique_threads=16
```

A Path B override near elapsed 3.813 s still executed on a secondary thread while the expected native thread remained unchanged. The session continued cleanly to verified Deinit.

Detailed result: `analysis/V01C9_RUNTIME_RESULT.md`.

## Planar standoff status

Position remains read-only:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
```

On the current large A wall, 29 samples showed essentially exact vanilla standoff:

```text
target_distance=0.5 m
max_abs_error_before=2.98023224e-08 m
```

Therefore active normal-direction adhesion correction is not currently justified for this planar control case. Keep it deferred/optional unless movement work later introduces measurable standoff drift.

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
12. transient freshness recovery implementation ✅ synthetic; no-gap real c9 did not need recovery
13. exact hard-kill attribution ✅ c8
14. secondary-thread Path B admission ✅ c9 real
15. lifecycle-wide real orientation hold on A wall ✅ c9
16. diagonal E-wall orientation regression ▶ CURRENT
17. active planar adhesion correction ⏸ optional/deferred; current standoff already stable
18. surface-relative movement ⬜
19. animation integration ⬜
20. camera and later roadmap ⬜

## Current implementation base

Keep c9 as the runtime base:

```text
branch: codex/v01c9-secondary-path-b-thread-admission
implementation commit: 0c276126ccac8e3bd2a6dd1d850da63e80bc35ce
published head: b865dabfa81626248ea8a8ae56a1dd1c992d7301
```

## Current objective

Do not create another implementation version yet.

Use the same c9 build on a diagonal E-type planar wall. The next gate is to verify that the now-stable orientation lifecycle is surface-normal-generic rather than accidentally A-specific.

Required E-wall evidence:

```text
N398 diagonal / non-cardinal
rotation reaches the corresponding Surface Up
ORIENTATION_RETENTION_PASS=1
source_active_until_deinit=1
verified Deinit clean final close
first_hard_kill=none
Position writes=0
no visible upright flash/snap/crash
```

A shorter wall is acceptable if it still gives enough time to complete the ~0.7 s delay + 0.25 s transition and retain orientation before Deinit. Prefer a wider diagonal wall if available.

After the E regression passes, proceed to the next planar locomotion milestone. Do not add active adhesion correction merely because it was on the old roadmap; current measured A-wall standoff gives no evidence it is necessary yet.