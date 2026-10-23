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

The current preferred control surface is a large unobstructed A-direction wall.

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

## Orientation history

- c2: visible surface-relative rotation, rapid WorldUp/SurfaceUp flicker.
- c3: multithread writer tracing attempted; production expansion failed.
- c4: thread-watch repaired; first retention PASS, but source ownership ended early.
- c5: lifecycle continuity repaired; real full-WallRun orientation hold passed.
- c6: Position/adhesion remained read-only; a real ~250 ms native Update gap caused `fresh_update_age` to permanently end source ownership.
- c7: added transient stale suspend/resume, but real run hard-killed orientation after only two Path A writes / ~16 ms, before the recovery logic could matter.

## Last authoritative real PASS: c5

```text
wallrun_duration_ms=3843
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
path_a_writes=219
path_b_writes=218
retention_pass=1
max_held_ms=2906
max_held_updates=205
last_actor_up_dot_surface=1
```

## c6 regression

The same verified WallRun survived a ~250 ms native Update gap, but c6 permanently closed source ownership on `fresh_update_age`. Adhesion was read-only (`corrections_applied=0`, `source_position_writes=0`).

## c7 real-game FAIL

Implementation:

```text
branch: codex/v01c7-transient-freshness-recovery
implementation commit: 93d7887e8ba8cb04e38473a0a821077ee8fc9efb
published head: 466f0ab237faccd8c769d17fedd66a42138a7829
version: v0.1c7 / package 0.1.25
```

User observation: actor stayed visually in vanilla orientation.

Runtime sequence:

```text
elapsed=0.703  Path A write #1  t=0.000
elapsed=0.719  Path A write #2  t=0.064
NATIVE_SOURCE_EXIT reason=disabled_kill_or_patch_gate
```

The verified WallRun continued after the hard close. Therefore this was not a normal WallRun end and was not the c6 `fresh_update_age` path.

Immediately after the first write, the bounded orientation/reset watch reported:

```text
stop_reason=observation_guards_lost
```

This is temporally adjacent but not yet proven to be the first kill cause.

Code review shows the worker currently hard-kills `native_source` whenever the broad `observationPlanAuthenticated()` boolean becomes false. That boolean combines verified hook availability, observation `fieldPlanEnabled`, and byte ownership of all four WallRun hook patches. Path A/B also contain separate hard-kill sites. The log does not currently identify which one fired first.

Detailed result: `analysis/V01C7_RUNTIME_RESULT.md`.

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
10. visible surface-relative rotation ✅ historical
11. lifecycle-wide hold under normal cadence ✅ c5 historical
12. transient freshness recovery implementation ✅ synthetic / real pending
13. exact c7 hard-kill attribution ▶ CURRENT
14. lifecycle-wide real orientation hold ⬜ revalidate
15. planar adhesion / safe Position-source ownership ⏸
16. surface-relative movement ⬜
17. animation integration ⬜
18. camera and later roadmap ⬜

## Current implementation base

Use published c7 so its stale suspend/resume code is preserved:

```text
branch: codex/v01c7-transient-freshness-recovery
implementation commit: 93d7887e8ba8cb04e38473a0a821077ee8fc9efb
published head: 466f0ab237faccd8c769d17fedd66a42138a7829
```

Pre-created next branch:

```text
codex/v01c8-kill-cause-attribution
```

## Current objective

Identify and record the exact first hard-kill cause from c7, then fix only the demonstrated coupling/false failure.

Do not simply remove integrity checks. If the failure is only a diagnostic/observation-plan condition that is not required for authenticated Path A/B ownership, decouple it from source hard-kill. If any actual WallRun hook patch, Path A/B caller/writer, thread/stack, Hero/Transform, epoch, rollback or build identity is corrupted, remain fail-closed.

Do not advance Position writes/adhesion until real-game full-WallRun orientation passes again.