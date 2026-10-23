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

The current preferred test surface is a large unobstructed A-direction control wall. The successful c4 session measured essentially `N398 = (0,0,+1)` and allowed a ~3.9 s continuous WallRun.

## Orientation Path A

- writer RVA `0x2CFC580`
- WallRun caller return RVA `0x2CFC8F3`
- padded Right / Up / Forward plus Position source
- lock-free shared WallSprint snapshot active
- real runtime confirms SurfaceFrame writes
- Position preserved
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

## c2/c3 history

c2 achieved visible surface-relative rotation but showed rapid WorldUp / SurfaceUp flicker.

c3 attempted multithread writer tracing but its production expansion path never enumerated additional threads (`process_threads_seen=0`).

## v0.1c4 real-game result — first retention PASS

Implementation:

```text
branch: codex/v01c4-real-thread-enum-fix
implementation commit: cfaae52304dfa757f2098eeb63c0168151c5b755
published head: 6bf7646d2aebdfda9c285b9a3ca4108760d6baa1
version: v0.1c4 / package 0.1.22
```

### Thread-watch repair PASS

Real production enumeration now works:

```text
owner_pid_matches=129
process_threads_seen=128
eligible=127
installed=126
failed=1
cap_exceeded=1
cleanup_pending=0
```

The one failed thread was already suspended. All 126 owned watch states were restored cleanly.

No `THIRD_WRITER_EVENT` / `THIRD_WRITER_IDENTITY` occurred in this run.

### Orientation retention PASS

Human observation: prior rapid flicker was no longer visually apparent on the large A wall.

Authoritative runtime gate:

```text
ORIENTATION_RETENTION_PASS=1
held_ms=329
held_updates=20
```

The same stable sequence continued to approximately 55 distinct updates / 969 ms with Actor Up at Surface Up.

This proves the existing Path A + Path B ownership can retain surface-relative orientation under a stable continuous WallRun.

Detailed result: `analysis/V01C4_RUNTIME_RESULT.md`.

## New blocker — source lifecycle ends before verified WallRun

The orientation source exited around WallRun elapsed 1.953 s:

```text
NATIVE_SOURCE_EXIT
reason=native_lifecycle_inactive_or_stale
applied_writes=135
held_updates=55
held_ms=985
stability_observed=1
```

But the independently verified WallRun lifecycle continued to its real Deinit around 3.900 s.

After the source exit, A/B override counts stopped increasing and Actor Up returned to World Up while `automatic_wallrun=Active`.

Therefore the current blocker is an early/false native-source lifecycle stale/inactive decision, not a demonstrated unavoidable third-writer race.

Retain the third-writer diagnostics in case unexplained resets return after lifecycle continuity is repaired.

## Current blocker / stage

1. WallRun detection ✅
2. Actor position/orientation ✅
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
13. lifecycle-wide retained orientation ▶ CURRENT
14. adhesion ⬜
15. surface-relative movement ⬜
16. animation integration ⬜
17. camera and later roadmap ⬜

## Current implementation base

Use c4 as the implementation base:

```text
branch: codex/v01c4-real-thread-enum-fix
implementation commit: cfaae52304dfa757f2098eeb63c0168151c5b755
published head: 6bf7646d2aebdfda9c285b9a3ca4108760d6baa1
```

Pre-created next branch:

```text
codex/v01c5-sustained-lifecycle
```

## Current objective

Identify the exact sub-gate causing `native_lifecycle_inactive_or_stale` while the verified WallRun remains active, then repair only that demonstrated continuity defect.

Desired behavior: after the existing 0.7 s delay / 0.25 s transition, keep Path A + Path B ownership surface-relative continuously until the verified WallRun Deinit (or a real Hero/Transform/owner/identity/kill failure).

Do not add adhesion, movement, animation or camera until this lifecycle-wide orientation hold passes on the large A control wall.
