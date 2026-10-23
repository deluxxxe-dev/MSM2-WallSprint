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

- `WallRun.this + 0x398` = candidate outward surface normal
- `WallRun.this + 0x338` = candidate surface-plane reference point
- `WallRun.this + 0x320` = hero-side WallRun reference point
- `P320 - P338 ≈ 0.5 * N398`
- `dot(HeroPosition - P338, N398) ≈ 0.5 m`

Face E repeatedly produces approximately `N398 = (-0.707107, 0, +0.707107)`.

## Orientation path A

- writer RVA `0x2CFC580`
- WallRun caller return RVA `0x2CFC8F3`
- padded Right / Up / Forward plus Position source
- lock-free shared WallSprint snapshot is active
- c2 runtime confirms qualifying calls can write the SurfaceFrame basis
- `PATH_A_STATE_LOCK_CONTENTION=0`

## Orientation path B

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

c2 startup proof passes on the real accepted build:

```text
PATH_B_COMPACT3X3_PROOF admitted=1
caller_verified=1
writer_verified=1
rdx_to_rcx_map_verified=1
decoded_prefix_bytes=444
caller_decoded_bytes=128
pairs=9
```

c2 runtime confirms Path B writes the intended SurfaceFrame Up on both A and diagonal E.

## v0.1c2 real-game result — visible rotation but rapid flicker

Human observation: visible change is now obvious. Normal vanilla WallRun orientation and the intended surface-relative orientation switch rapidly back and forth.

Authoritative log confirms the visual symptom.

Representative A-wall state:

```text
Path A applied_up=(0,0,1)
Path B applied_up=(0,0,1)
actor_up=(0,0,1) dot_surface=1
...
actor_up=(0,1,0) dot_surface=0
```

The same failure reproduces on diagonal E, where the desired Up is approximately `(-0.707107,0,+0.707107)`.

Both verified source paths therefore work, but another writer resets Actor Up to World Up shortly afterward.

Retention repeatedly resets after only a few distinct updates / a few tens of milliseconds. `ORIENTATION_RETENTION_PASS=1` is absent.

Late path-A WorldUp inputs are detected and converted (`LATE_COMMON_WORLDUP_OVERRIDE=1`), so those known calls do not fully explain the surviving WorldUp reset.

## Critical diagnostic gap

The current hardware Up watch is not process-thread complete:

```text
threads_enumerated=0
installed=1
complete_thread_coverage=0
primary_only=1
primary_native_thread=8968
```

During the armed window, captured primary-thread events are the already-known path-A writer and show Surface Up writes. The actual SurfaceUp -> WorldUp transition appears in worker observations without a corresponding hardware writer event.

Strongest current hypothesis: a third orientation writer runs on another game thread (or otherwise outside the primary-only watch coverage).

Detailed runtime result: `analysis/V01C2_RUNTIME_RESULT.md`.

## Current blocker / stage

1. WallRun detection ✅
2. Actor position/orientation ✅
3. surface normal ✅
4. planar surface point / standoff data ✅
5. SurfaceFrame basis ✅
6. path-A source injection ✅
7. path-B discovery / compact layout ✅
8. real-runtime path-B proof ✅
9. path-A + path-B SurfaceFrame writes ✅
10. visible surface-relative rotation ✅ intermittent
11. third reset writer identification ▶ CURRENT
12. retained orientation ⬜
13. adhesion ⬜
14. surface-relative movement ⬜
15. animation integration ⬜
16. camera and later roadmap ⬜

## Current implementation base

Use c2, not main's older source, as the code base:

```text
branch: codex/v01c2-proof-fix
implementation commit: 2d26710034dcf695fa7bb2da763f117faf6171db
published branch head: 727fcdc1594e36e6a1ffaabf5569eee89375b871
```

A working c3 branch has been pre-created from that base:

```text
codex/v01c3-third-writer-trace
```

## Current objective

Preserve the working c2 A/B overrides. Expand the bounded Hero Up write-watch to enough process-thread coverage to identify the first unexplained SurfaceUp -> WorldUp write after both overrides are active.

Do not modify a third path yet. First capture its exact RIP/function/caller/thread and timing.

Do not add adhesion, movement, animation or camera until retained orientation passes `dot(ActorUp,N398) >= 0.98` for >=300 ms or >=20 distinct native updates.

## Human test locations

A Control: same flat unobstructed vertical wall.

E: same known physical diagonal wall.
