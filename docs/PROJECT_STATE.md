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

## Verified game/build gate

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

- `WallRun.this + 0x398` = candidate outward surface normal.
- `WallRun.this + 0x338` = candidate surface-plane reference point.
- `WallRun.this + 0x320` = hero-side WallRun reference point.
- `P320 - P338 ≈ 0.5 * N398`.
- `dot(HeroPosition - P338, N398) ≈ 0.5 m`.

Face E diagonal wall repeatedly produces approximately:

`N398 = (-0.707107, 0, +0.707107)`.

## Orientation source path 1 — common writer

Known writer:

- function RVA `0x2CFC580`
- accepted-build function size 584 bytes.

Important caller paths:

- WallRun caller return RVA `0x2CFC8F3`
- Crawl surface caller return RVA `0x1021C9E`
- secondary Crawl candidate `0x1022C15`

Observed RDX source layout for the relevant Hero calls:

- `RDX + 0x00` Right
- `RDX + 0x10` Up
- `RDX + 0x20` Forward
- `RDX + 0x30` Position

WallRun normally supplies World Up. Crawl supplies surface-relative Up matching N398.

The WallRun caller-local source override is valid: modifying this source before `0x2CFC580` briefly changes the real Hero Actor Transform toward Surface Up.

## v0.1b5 / b6

v0.1b5 established that source injection reaches the Actor but is later reset to World Up.

v0.1b6 failed only as a diagnostic: its hardware watch was disabled/late. This did not invalidate source injection.

## v0.1b7 result — reset ownership discovered

v0.1b7 successfully pre-armed the primary WallRun thread before source override and captured the real reset path on both A and E.

### Immediate reset path

Strong A/E repeated evidence:

- writer function RVA `0x2CFC3C0`
- runtime range `0x2CFC3C0..0x2CFC57C`
- size 444 bytes
- captured runtime SHA256 `ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196`
- caller return RVA `0x2CFC84B`
- caller2 RVA `0x35184A5`
- same primary native WallRun thread.

Timing:

- commonly about 0.8–1.0 ms after the WallSprint source commit.

Static/runtime interpretation:

- the beginning of `0x2CFC3C0` directly copies transform dwords from RDX to RCX;
- this includes Up components at `+0x10`, `+0x14`, `+0x18`;
- RCX is the current Hero Transform in the captured reset events;
- RDX is a nearby thread-stack address in the captured events (approximately post-prologue `RSP + 0x60` in representative samples).

On E, a Surface-like Up such as approximately `(-0.5, 0.707, +0.5)` is observed being driven toward `(0,1,0)` through sequential writes from this function.

This is the immediate visible orientation blocker.

### Later common-writer reset

A second path also exists:

- writer `0x2CFC580`
- caller return `0x2CFC8F3`
- current Hero target
- source Up = World Up
- some such calls report `source_overridden_this_entry=0` even after WallSprint source injection has begun.

A strong A sample reset Surface Up back to World Up about 14.9 ms after the preceding source commit.

Therefore the current `0x2CFC580` override admission does not cover every relevant WallRun invocation.

## Current architecture hypothesis

```text
WallRun builds World-relative native sources
        ↓
WallSprint currently overrides one 0x2CFC8F3 / 0x2CFC580 source
        ↓
Hero briefly becomes Surface-relative
        ↓ ~0.8–1.0 ms
0x2CFC84B → 0x2CFC3C0 copies another World-relative source into Hero
        ↓
Hero returns toward World-relative orientation
        ↓ later
0x2CFC8F3 → 0x2CFC580 may also write another unmodified World-Up source
```

Do not globally patch either writer. The preferred architecture remains upstream/source ownership: feed the same SurfaceFrame orientation into each exact verified WallRun source path that owns Hero orientation.

## Current blocker / stage

1. WallRun detection ✅
2. Actor position/orientation ✅
3. surface normal ✅
4. planar surface point / standoff data ✅
5. SurfaceFrame basis ✅
6. first native orientation source injection ✅
7. reset writer discovery ✅
8. dual-source orientation ownership ▶ CURRENT
9. retained WallSprint orientation ⬜
10. adhesion ⬜
11. surface-relative movement ⬜
12. animation integration ⬜
13. camera and later roadmap ⬜

## Current implementation objective

v0.1b8 should:

- preserve the existing `0x2CFC580` source-injection architecture;
- cover the late unmodified `0x2CFC8F3` World-Up calls during active WallSprint;
- independently authenticate `0x2CFC3C0` + caller `0x2CFC84B`;
- inspect its actual RDX source at entry;
- only if exact guards pass, replace orientation rows of that caller-local source with the same interpolated SurfaceFrame basis;
- never modify Position;
- keep direct Actor write, adhesion, movement, animation, camera and physics disabled;
- require retained `dot(ActorUp,N398) >= 0.98` for >=0.30 s or 20 distinct updates before moving to adhesion.

## Repository continuity warning

The GitHub `main` branch may not contain the previous Codex cloud task's v0.1b7 implementation because that task produced/tested a package without committing its working-tree changes back to this repository.

Any new isolated Codex task must first inspect the repo and, if b7 changes are absent, reconstruct the required pre-arm/trace support from `docs/CODEX_NEXT.md`, `analysis/V01B7_RUNTIME_RESULT.md`, and existing b6 code. Do not assume a previous cloud working tree persists.

## Human test locations

A Control:

- current flat unobstructed vertical control wall near Face E;
- do not hard-code +Z; use measured N398.

E:

- same known physical diagonal wall used in previous tests.

For the next orientation ownership test, one left-to-right WallRun on A and one on E are sufficient. Crawl is not required.