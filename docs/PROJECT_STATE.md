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

v0.1b9 exposed frequent `COMMON_OVERRIDE_REJECT reason=state_lock_contention`. v0.1c1 implemented a lock-free shared orientation snapshot to remove that hot-hook dependency, but the real runtime never reached the override stage because the new Path-B proof failed during startup admission.

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

The b9 diagnostic milestone succeeded.

On A and E, exact path-B entries repeatedly proved:

```text
RDX = RSP + 0x28
rdx_inside_stack_bounds=1
source_span_inside_stack_bounds=1
expected_rsp_relation_match=1
source_readable=1
```

Authenticated caller/writer bytes and runtime values support a compact 3x3 basis in `[RDX+0x00,RDX+0x24)` with direct copies into Hero orientation slots:

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

Detailed evidence: `analysis/V01B9_RUNTIME_RESULT.md`.

## v0.1c1 runtime result — startup proof failed closed

User again reported no visible orientation change. This time the package was the expected c1 build:

```text
WallSprint v0.1c1 package_version=0.1.19
path_b_override=1
```

The executable itself still passed the normal target build gate for Spider-Man2.exe 2.810.0.0 and the expected SHA256.

However the new c1 compact Path-B startup proof failed before either source path was admitted:

```text
PATH_B_COMPACT3X3_PROOF
admitted=0
caller_verified=0
writer_verified=1
rdx_to_rcx_map_verified=0
source_bytes=0x24
decoded_prefix_bytes=5
pairs=0
reason=instruction_or_dataflow_not_proven
```

Consequently:

```text
native_source_config admitted=0
DUAL_SOURCE_ADMISSION admitted=0
PATH_B_LAYOUT_CONFIG enabled=0 path_b_override=0
PATH_A_OVERRIDE_COUNT=0
PATH_B_OVERRIDE_COUNT=0
```

Therefore c1 produced no visible change because no orientation override executed at all.

Important: this is not evidence of a changed game build or a disproven compact-3x3 runtime model. The existing path-A common writer still authenticated/installed. The blocker is the new c1 proof decoder/self-check, which decoded only 5 bytes of the real accepted-build instruction stream and recovered zero copy pairs.

Detailed result: `analysis/V01C1_RUNTIME_RESULT.md`.

c1 implementation provenance:

```text
branch: codex/v01c1-compact-path-b
commit: 71437fac6c0640af01725a46e1b49c5627c739bb
```

The next task must use that branch/commit as the implementation base; main currently carries handoff state but is not guaranteed to contain the c1 implementation source.

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
10. real-runtime compact-3x3 proof admission ▶ CURRENT
11. dual-source retained orientation ⬜
12. adhesion ⬜
13. surface-relative movement ⬜
14. animation integration ⬜
15. camera and later roadmap ⬜

## Current implementation objective

Repair the c1 proof implementation without weakening safety:

1. Start from `codex/v01c1-compact-path-b` commit `71437fac6c0640af01725a46e1b49c5627c739bb`.
2. Make the anchored decoder consume the complete authenticated real-game Path-B caller/writer instruction streams rather than stopping after 5 bytes.
3. Independently recover and verify all nine RDX-to-RCX compact basis copy pairs.
4. Log the first failed decode offset/opcode/reason if proof cannot complete.
5. Keep the lock-free path-A/path-B shared orientation implementation from c1.
6. Keep exact-build, target, stack, lifecycle and metadata-preservation fail-closed gates.
7. Re-run the same A/E test only after startup logs `PATH_B_COMPACT3X3_PROOF admitted=1`.
8. Do not authorize adhesion until retained orientation passes `dot(ActorUp,N398) >= 0.98` for >=0.30 s or >=20 distinct native updates.

## Human test locations

A Control: current flat unobstructed vertical control wall near Face E. Do not hard-code +Z; use measured N398.

E: same known physical diagonal wall used in prior tests.
