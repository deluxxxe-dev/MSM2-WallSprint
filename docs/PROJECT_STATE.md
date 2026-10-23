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

A wider wall used for the c3 test was visually the same general direction as A but measured approximately `N398 = (-0.113317, 0, +0.993559)`, so treat it as a near-+Z planar control rather than assuming exact cardinal +Z.

## Orientation path A

- writer RVA `0x2CFC580`
- WallRun caller return RVA `0x2CFC8F3`
- padded Right / Up / Forward plus Position source
- lock-free shared WallSprint snapshot is active
- real runtime confirms qualifying calls can write the SurfaceFrame basis
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

Real-game compact proof passes and both Path A and Path B can write the intended surface-relative basis.

## v0.1c2 result — visible rotation but rapid flicker

The actor visibly reaches surface-relative orientation, then repeatedly returns to World Up. Logs confirm both known paths commit valid surface-relative Up before Actor Up abruptly returns to World Up.

Late Path-A WorldUp sources are themselves converted (`LATE_COMMON_WORLDUP_OVERRIDE=1`), so they do not explain every surviving reset.

`ORIENTATION_RETENTION_PASS=1` has not been achieved.

## v0.1c3 real-game result — multithread trace did not expand

c3 was intended to identify the remaining writer by expanding the Hero Up hardware write watch from the primary native thread to other process threads.

Package/runtime:

```text
v0.1c3 / package 0.1.21
branch: codex/v01c3-third-writer-trace
implementation commit: fd105be35a12c4a4ba8b8a72c3f75a4bcf49ff82
published head: b05278093b6e0ad92db7dad8c51a986ba16ca872
```

The real-game session still had working Path A / Path B overrides, but the new process-thread watch reported:

```text
MULTITHREAD_UP_WATCH phase=stopped
process_threads_seen=0
eligible=0
installed=0
complete_thread_coverage=0
rescan_count=0
```

The primary-thread hardware watch remained functional and captured only known Path A events. No `UNKNOWN_WRITER`, `THIRD_WRITER_EVENT`, or `THIRD_WRITER_IDENTITY` was captured.

This does **not** disprove the third-writer hypothesis. It demonstrates that the c3 real-game thread-enumeration/expansion path failed before coverage was established.

Detailed result: `analysis/V01C3_RUNTIME_RESULT.md`.

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
11. multithread Up-watch real-game enumeration/installation ▶ CURRENT
12. third reset writer identification ⬜
13. retained orientation ⬜
14. adhesion ⬜
15. surface-relative movement ⬜
16. animation integration ⬜
17. camera and later roadmap ⬜

## Current implementation base

Use the published c3 implementation as the code base:

```text
branch: codex/v01c3-third-writer-trace
implementation commit: fd105be35a12c4a4ba8b8a72c3f75a4bcf49ff82
published head: b05278093b6e0ad92db7dad8c51a986ba16ca872
```

Pre-created next branch:

```text
codex/v01c4-real-thread-enum-fix
```

## Current objective

Preserve the working A/B overrides and c3 safety behavior. Repair and instrument the real-game process-thread enumeration/expansion path so the bounded Hero Up write watch actually installs on more than one eligible game thread.

Do not modify a third writer yet. Do not add adhesion, movement, animation or camera until retained orientation passes `dot(ActorUp,N398) >= 0.98` for >=300 ms or >=20 distinct native updates.
