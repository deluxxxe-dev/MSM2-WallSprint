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
- c7: added transient stale suspend/resume, but real run hard-killed orientation after only two Path A writes / ~16 ms.
- c8: exact first-cause attribution succeeded. Real run reached Surface Up and retention PASS, then hard-killed at 1.750 s because a verified Path B call for the active Hero Transform arrived on a secondary thread.

## Last authoritative full-WallRun PASS: c5

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

## c8 real-game result

Implementation:

```text
branch: codex/v01c8-kill-cause-attribution
implementation commit: 98c1d45108c89262750616c7258414cf762cac55
published head: 75af734d4d32d5e3a77087cbee6b2247b9617ced
version: v0.1c8 / package 0.1.26
```

The target build and all normal source/integrity gates were accepted. Orientation reached full Surface Up and passed retention:

```text
ORIENTATION_RETENTION_PASS=1
held_ms=312
held_updates=20
```

At elapsed `1.750000`, c8 latched the exact first abnormal close:

```text
NATIVE_SOURCE_HARD_KILL
reason=path_b_wrong_thread
origin=path_b
thread_id=35948
native_thread=10680
expected_thread=10680
wallrun_active=1
wallrun_session=1
```

All other detailed guards were healthy:

```text
input_auth=1
copy_auth=1
compact_proof=1
caller_read_ok=1
caller_patch_match=1
source_failed_gate=none
observation_failed_hook=none
observation_failure_mask=0x0
```

The secondary-thread Path B entry still targeted the current Hero Transform, returned through verified caller RVA `0x2CFC84B`, and had `RDX-RSP=0x28`. The native lifecycle token was coherent and the same session/owner/generation/epoch remained active.

The source exited only because of the thread equality rule. The independently verified WallRun then continued to normal Deinit:

```text
Init=1
Entry=1
Update=279
Deinit=1
duration=4.405647 s
end_reason=verified_deinit
```

Therefore the invariant “every Path B call for active WallSprint must execute on the verified WallRun/native thread” is false in real runtime.

No stale-suspend event occurred. Position/adhesion remained read-only (`corrections_applied=0`, `source_position_writes=0`).

Detailed result: `analysis/V01C8_RUNTIME_RESULT.md`.

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
12. lifecycle-wide hold under normal cadence ✅ c5 historical
13. transient freshness recovery implementation ✅ synthetic / real no-gap run pending
14. exact hard-kill attribution ✅ c8
15. secondary-thread Path B admission ▶ CURRENT
16. lifecycle-wide real orientation hold ⬜ revalidate
17. planar adhesion / safe Position-source ownership ⏸
18. surface-relative movement ⬜
19. animation integration ⬜
20. camera and later roadmap ⬜

## Current implementation base

Use published c8 so exact first-cause attribution and c7 stale recovery remain available:

```text
branch: codex/v01c8-kill-cause-attribution
implementation commit: 98c1d45108c89262750616c7258414cf762cac55
published head: 75af734d4d32d5e3a77087cbee6b2247b9617ced
```

Pre-created next branch:

```text
codex/v01c9-secondary-path-b-thread-admission
```

## Current objective

Repair the false `path_b_wrong_thread` fatal invariant without weakening Path B provenance globally.

A secondary-thread Path B call may use the already-published WallSprint compact orientation basis only when it still targets the active Hero Transform and independently passes exact verified caller, current-thread stack relation, compact-layout, lifecycle identity/freshness and source-integrity checks. This is per-call Path B admission only; it must not migrate the expected native WallRun thread or weaken Path A ownership.

Do not advance Position writes/adhesion until real-game full-WallRun orientation passes again.