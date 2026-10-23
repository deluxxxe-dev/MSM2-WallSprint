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

Face E repeatedly produces approximately `N398 = (-0.707107, 0, +0.707107)`.

The current preferred control surface is a large unobstructed A-direction wall. The successful c5 run measured effectively `N398 = (0,0,+1)` and sustained a ~3.85 s WallRun.

## Orientation Path A

- writer RVA `0x2CFC580`
- WallRun caller return RVA `0x2CFC8F3`
- caller-local source contains padded Right / Up / Forward plus Position
- orientation source range currently owned is `[source+0x00, source+0x30)`
- Position has intentionally remained unchanged so far
- lock-free shared WallSprint snapshot active
- real runtime confirms SurfaceFrame orientation writes
- `PATH_A_STATE_LOCK_CONTENTION=0`

## Orientation Path B

- writer RVA `0x2CFC3C0..0x2CFC57C`, size 444
- writer SHA256 `ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196`
- caller `0x2CFC7D0..0x2CFC850`, size 128
- caller SHA256 `1665a7a329a983101673f05ccd95bbf3d738c39099469b4acfd14344ec98a79d`
- return RVA `0x2CFC84B`
- runtime source provenance: `RDX = writer-entry RSP + 0x28`
- compact 3x3 payload `[RDX+0x00,RDX+0x24)`

Direct map:

```text
RDX+00 -> RCX+00
RDX+04 -> RCX+04
RDX+08 -> RCX+08
RDX+0C -> RCX+10
RDX+10 -> RCX+14
RDX+14 -> RCX+18
RDX+18 -> RCX+20
RDX+1C -> RCX+24
RDX+20 -> RCX+28
```

Real-game compact proof passes and Path B writes the intended surface-relative basis.

## Orientation history

- c2: visible surface-relative rotation, but rapid WorldUp/SurfaceUp flicker.
- c3: attempted multithread writer tracing; production thread expansion failed.
- c4: production multithread watch repaired; first real orientation retention PASS, but source ownership ended early around 1.95 s while WallRun continued.
- c5: native lifecycle continuity repaired and real full-WallRun hold passed.

## v0.1c5 real-game result — lifecycle-wide orientation PASS

Implementation:

```text
branch: codex/v01c5-sustained-lifecycle
implementation commit: 1cfe3c76a5e09183757f891c5025def860ef4a88
published head: 4917a3af9afb683db7f7b130626f5b25e20aebca
version: v0.1c5 / package 0.1.23
```

Authoritative runtime summary:

```text
ORIENTATION_LIFECYCLE_SUMMARY
wallrun_duration_ms=3843
source_entry_elapsed=0.687000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=219
path_b_writes=218
retention_pass=1
max_held_ms=2906
max_held_updates=205
last_actor_up_dot_surface=1
```

The retained threshold passed at 20 updates / 297 ms and the source then remained Surface Up through the verified native Deinit.

No premature `native_lifecycle_inactive_or_stale` exit occurred.

Detailed result: `analysis/V01C5_RUNTIME_RESULT.md`.

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
11. multithread watch real-game enumeration ✅
12. retention threshold (>=300 ms or >=20 updates) ✅
13. lifecycle-wide retained orientation through verified Deinit ✅
14. planar adhesion / safe Position-source ownership ▶ CURRENT
15. surface-relative movement ⬜
16. animation integration ⬜
17. camera and later roadmap ⬜

## Current implementation base

Use c5 as the implementation base:

```text
branch: codex/v01c5-sustained-lifecycle
implementation commit: 1cfe3c76a5e09183757f891c5025def860ef4a88
published head: 4917a3af9afb683db7f7b130626f5b25e20aebca
```

Pre-created next branch:

```text
codex/v01c6-planar-adhesion
```

## Current objective

Implement the first planar adhesion milestone while preserving c5 orientation behavior.

The adhesion model is based on the verified plane data:

```text
distance = dot(HeroPosition - P338, N398)
target_distance ~= 0.5 m
error = target_distance - distance
```

Before any Position write, authenticate the exact caller-local Position source/data flow and prove that changing it reaches only the intended Hero Position for the current verified target.

Preferred implementation is a bounded source-level wall-normal correction. Do not fall back to direct Actor Transform teleports or per-frame transform spam.

For c6:

- correct only along `N398`;
- preserve tangent position components;
- remain bounded to the verified active WallRun lifecycle;
- keep Path A/B orientation ownership unchanged;
- fail closed on owner/Hero/Transform/session/generation/stack/source identity failures;
- no surface-relative input movement yet;
- no animation, camera, gravity replacement, parkour, swing or combat yet.

If safe Position-source ownership cannot be independently proven, c6 must remain Position read-only and produce the exact proof/diagnostic needed for the next step.
