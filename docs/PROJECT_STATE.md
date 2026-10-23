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

For relevant Hero calls its RDX source layout is verified as:

- `+0x00` Right
- `+0x10` Up
- `+0x20` Forward
- `+0x30` Position

WallSprint can override this caller-local source and briefly change the real Hero Actor Up toward Surface Up without direct Actor writes.

Current remaining path-A issue: v0.1b9 logged thousands of `COMMON_OVERRIDE_REJECT reason=state_lock_contention` events. The hot hook must stop depending on a contended mutex/try-lock before orientation ownership can be considered complete.

## Orientation reset path B

Verified immediate reset path:

- writer RVA `0x2CFC3C0`
- range `0x2CFC3C0..0x2CFC57C`
- size 444 bytes
- captured SHA256 `ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196`
- caller return RVA `0x2CFC84B`
- caller function `0x2CFC7D0..0x2CFC850`
- caller2 RVA `0x35184A5`

It acts roughly 0.8–1.4 ms after representative path-A source commits and restores Hero orientation toward World Up.

## v0.1b9 result — path-B compact 3x3 layout characterized

User reported no visible change. That is expected because v0.1b9 explicitly ran with:

```text
path_b_override=0
path_b_characterization_active=1
```

The diagnostic milestone succeeded.

### Proven caller-local provenance

On both A and E, exact path-B entries repeatedly reported:

```text
RDX = RSP + 0x28
rdx_inside_stack_bounds=1
source_span_inside_stack_bounds=1
expected_rsp_relation_match=1
source_readable=1
```

So the old v0.1b8 `source_is_stack_local=0` result was a misleading diagnostic, not evidence against stack-local provenance.

### Compact 3x3 source layout

Authenticated caller bytes show nine scalar products written into a local stack buffer before the call to `0x2CFC3C0`.

Authenticated writer bytes show direct copies from those nine floats into Hero orientation slots:

```text
RDX + 0x00 -> RCX + 0x00
RDX + 0x04 -> RCX + 0x04
RDX + 0x08 -> RCX + 0x08

RDX + 0x0C -> RCX + 0x10
RDX + 0x10 -> RCX + 0x14
RDX + 0x14 -> RCX + 0x18

RDX + 0x18 -> RCX + 0x20
RDX + 0x1C -> RCX + 0x24
RDX + 0x20 -> RCX + 0x28
```

Interpretation: path B uses a compact 3x3 basis payload in `[RDX+0x00, RDX+0x24)`, unlike path A's padded affine layout.

Runtime values support the same interpretation on A and E:

- A path-B source basis stays world-relative with Up ≈ `(0,1,0)` while path A has already tilted Hero Up toward the wall.
- E path-B source basis has diagonal horizontal Right/Forward rows but still Up ≈ `(0,1,0)`, while path A drives Hero Up toward `(-0.707,0,+0.707)`.

Path B therefore remains the immediate World-Up reset owner.

Detailed evidence: `analysis/V01B9_RUNTIME_RESULT.md`.

## Current blocker / stage

1. WallRun detection ✅
2. Actor position/orientation ✅
3. surface normal ✅
4. planar surface point / standoff data ✅
5. SurfaceFrame basis ✅
6. path-A native orientation source injection ✅
7. reset writer discovery ✅
8. path-B hook/authentication ✅
9. path-B source provenance / compact 3x3 layout ✅
10. dual-source retained orientation ▶ CURRENT
11. adhesion ⬜
12. surface-relative movement ⬜
13. animation integration ⬜
14. camera and later roadmap ⬜

## Current implementation objective

Next package should attempt orientation ownership only:

1. Formalize an anchored startup decode/self-check for the path-B compact 3x3 direct-copy map.
2. For exact verified `0x2CFC84B -> 0x2CFC3C0` Hero/WallRun calls after the transition delay, replace only the first nine source floats with the shared current WallSprint basis.
3. Preserve original row magnitudes/per-axis scale; modify orientation direction only.
4. Do not touch anything at or after `RDX+0x24`.
5. Replace path-A lock-dependent admission with a lock-free published orientation snapshot so qualifying `0x2CFC8F3 -> 0x2CFC580` calls do not fail because of `state_lock_contention`.
6. Require retained `dot(ActorUp,N398) >= 0.98` for >=0.30 s or >=20 distinct native updates.
7. Do not add adhesion until that retention gate passes.

## Human test locations

A Control: current flat unobstructed vertical control wall near Face E. Do not hard-code +Z; use measured N398.

E: same known physical diagonal wall used in prior tests.
