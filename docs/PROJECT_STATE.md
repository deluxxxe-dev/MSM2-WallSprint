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

## Orientation source path A — common writer

Known writer:

- writer RVA `0x2CFC580`
- WallRun caller return RVA `0x2CFC8F3`

For the relevant Hero calls its RDX source layout is verified as:

- `+0x00` Right
- `+0x10` Up
- `+0x20` Forward
- `+0x30` Position

WallSprint can override this caller-local source and briefly change the real Hero Actor Up toward Surface Up without direct Actor writes.

## Reset path discovered in v0.1b7

Immediate reset path reproduced on A and E:

- writer RVA `0x2CFC3C0`
- range `0x2CFC3C0..0x2CFC57C`
- size 444 bytes
- captured SHA256 `ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196`
- caller return RVA `0x2CFC84B`
- caller2 RVA `0x35184A5`

It acts roughly 0.8–1.4 ms after a representative WallSprint source commit and drives Hero Up back toward World Up.

A later reset path also exists through `0x2CFC580 / 0x2CFC8F3` when a World-Up source invocation is allowed through unmodified.

## v0.1b8 result — dual-source attempt failed closed

User reported no visible orientation change. Runtime behavior matches that report.

Startup/authentication succeeded:

- exact build accepted
- `RESET_COPY_HOOK admitted=1 installed=1`
- `DUAL_SOURCE_ADMISSION admitted=1`
- prearmed verification active

Path A continued to work: `0x2CFC8F3 / 0x2CFC580` source overrides generated interpolated Surface-relative Up values.

Path B did **not** get modified. On both A and E, actual `0x2CFC84B -> 0x2CFC3C0` entry capture reported:

```text
source_readable=1
source_is_stack_local=0
layout_valid=0
override=0
reason=caller_local_provenance_or_source_sanity_failed
```

Representative captured relation:

```text
RDX = RSP + 0x28
RDX numerically lies inside the reported current-thread stack bounds
```

but the first 0x40 bytes at RDX do not validate as the ordinary affine matrix layout used by path A.

Therefore the old hypothesis that path B uses the same RDX matrix format as `0x2CFC580` is **not proven and must not be used for writes**.

The hook correctly failed closed. Retention remained:

```text
held_updates=0
held_ms=0
stability_observed=0
```

The immediate `0x2CFC3C0` reset still occurs, and later common-writer World-Up calls may also remain after the session aborts.

Detailed runtime result: `analysis/V01B8_RUNTIME_RESULT.md`.

## Current blocker / stage

1. WallRun detection ✅
2. Actor position/orientation ✅
3. surface normal ✅
4. planar surface point / standoff data ✅
5. SurfaceFrame basis ✅
6. path-A native orientation source injection ✅
7. reset writer discovery ✅
8. path-B hook/authentication ✅
9. path-B source layout / provenance characterization ▶ CURRENT
10. retained WallSprint orientation ⬜
11. adhesion ⬜
12. surface-relative movement ⬜
13. animation integration ⬜
14. camera and later roadmap ⬜

## Current implementation objective

The next task is diagnostic-first. For exact caller `0x2CFC84B -> 0x2CFC3C0`:

- statically decode the writer and its caller `0x2CFC7D0..0x2CFC850`;
- identify exactly how RDX is constructed and which RDX fields are copied/used to determine RCX orientation;
- correlate those fields against A and E runtime observations;
- distinguish ordinary matrix data from packed/mixed structure fields or embedded pointers;
- do not modify path-B data until same-offset semantics are proven across A/E;
- preserve path-A source injection and bounded write-watch for correlation;
- direct Actor writes, adhesion, movement, animation, camera and physics remain disabled.

Orientation retention must still reach `dot(ActorUp,N398) >= 0.98` for >=0.30 s or >=20 distinct native updates before adhesion is authorized.

## Human test locations

A Control: current flat unobstructed vertical control wall near Face E. Do not hard-code +Z; use measured N398.

E: same known physical diagonal wall used in prior tests.
