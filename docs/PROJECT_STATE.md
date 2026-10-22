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

Face E diagonal wall has repeatedly produced approximately:

`N398 = (-0.707107, 0, +0.707107)`.

## Orientation writer discovery

A common native orientation/transform writer was independently discovered:

- function RVA `0x2CFC580`
- function size 584 bytes on the accepted build.

It is common infrastructure and must not be globally patched.

Important caller paths observed:

- WallRun common caller return RVA `0x2CFC8F3`
- Crawl surface-relative caller return RVA `0x1021C9E`
- secondary Crawl candidate `0x1022C15`

## Source transform layout

At the common writer, the relevant RDX source block is verified for the observed Hero Transform calls as an affine transform-like layout:

- `RDX + 0x00` Right
- `RDX + 0x10` Up
- `RDX + 0x20` Forward
- `RDX + 0x30` Position

WallRun supplies World Up. Crawl supplies surface-relative Up matching N398 on A and E.

## v0.1b5 result

WallRun caller-local source override works far enough to change the actual Actor Transform.

Verified behavior:

- modify the WallRun stack-local source orientation before the common writer;
- Actor Up briefly moves toward the requested surface-relative Up;
- roughly 10–20 ms later Actor Up returns to World Up.

Therefore the source injection path is valid. The remaining orientation blocker is a later reset/write path.

Do not discard the source-override architecture.

## v0.1b6 result

v0.1b6 attempted to capture the immediate reset writer but the diagnostic trace was not actually admitted/armed in time.

Observed startup/runtime failure:

- `orientation_trace_config enabled=0 admitted=0`
- attempted windows reported `installed=0`
- `armed=0`
- `captured=0`
- arm delay was roughly 68–88 ms after the relevant commit, later than the reset itself.

The Actor still briefly became surface-relative, confirming the b5 result.

Conclusion:

**v0.1b6 did not disprove the reset-writer hypothesis. Its watch was late/inactive.**

## Current blocker

Identify the native write/caller that resets Hero Actor Up from surface-relative orientation back to World Up immediately after the successful WallRun source override.

Current stage:

1. WallRun detection ✅
2. Actor position/orientation ✅
3. surface normal ✅
4. planar surface point / standoff data ✅
5. SurfaceFrame basis ✅
6. native orientation source injection ✅ briefly reaches Actor
7. immediate reset writer/caller ▶ CURRENT
8. retained WallSprint orientation ⬜
9. adhesion ⬜
10. surface-relative movement ⬜
11. animation integration ⬜
12. camera and later roadmap ⬜

## Human test locations

A Control:

- use the current flat unobstructed vertical control wall near Face E;
- do not assume +Z in code; use measured N398.

E:

- use the same known physical diagonal wall used in previous tests.

For the next diagnostic, one left-to-right WallRun on A and one on E are sufficient. Crawl is not required.