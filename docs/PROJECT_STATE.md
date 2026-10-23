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

## Orientation source path A

- writer RVA `0x2CFC580`
- WallRun caller return RVA `0x2CFC8F3`
- source layout: padded Right/Up/Forward plus Position

v0.1c1 introduced a lock-free shared orientation snapshot so the native hot hook no longer depends on the worker-state mutex. c2 preserves this implementation. Real c2 runtime execution remains pending.

## Orientation reset path B

Verified immediate reset path:

- writer RVA `0x2CFC3C0..0x2CFC57C`, size 444
- writer SHA256 `ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196`
- caller `0x2CFC7D0..0x2CFC850`, size 128
- caller SHA256 `1665a7a329a983101673f05ccd95bbf3d738c39099469b4acfd14344ec98a79d`
- call return RVA `0x2CFC84B`
- exact runtime source provenance: `RDX = writer-entry RSP + 0x28`
- compact 3x3 payload occupies `[RDX+0x00,RDX+0x24)`

Nine direct copies are verified from the authenticated bytes:

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

## v0.1c1 runtime result

c1 failed closed before either source override. The target executable was accepted, but the compact proof stopped after 5 bytes and recovered zero pairs. Therefore the prior no-visible-change result was caused by proof admission failure, not by a demonstrated later orientation reset.

## v0.1c2 implementation handoff — proof repair PASS

Published implementation:

```text
branch: codex/v01c2-proof-fix
implementation commit: 2d26710034dcf695fa7bb2da763f117faf6171db
published branch head: 727fcdc1594e36e6a1ffaabf5569eee89375b871
version: v0.1c2
package: 0.1.20
artifact: WallSprint_v0.1c2_real_runtime_proof_fix.zip
```

Verified implementation evidence:

- the privately supplied real writer444/caller128 reproduce the exact c1 failure under the retained old decoder;
- c2 uses statically linked Capstone 5.0.6 for bounded startup proof only, with skipdata disabled;
- writer decodes 444/444 bytes, 109 instructions;
- caller decodes 128/128 bytes, 28 instructions;
- all nine copy pairs are independently recovered;
- caller construction, stack relation and direct E8 call are verified;
- writer/caller byte mutation and malformed-dataflow negatives fail closed;
- production authentication/hook installation/restoration passes on a nonexecuted owned registered image;
- c1 lock-free A/B snapshot, exact Path-A every-call override, Path-B 0x24-byte-only write, row-scale preservation, metadata preservation, lifecycle/thread/Hero/target/stack/freshness/kill gates and bounded hardware watch are retained;
- no direct Actor/Position, adhesion, movement, animation or camera writes were enabled;
- private captured game bytes are excluded from distribution.

Validation reported by c2:

```text
portable ASan/UBSan: 11 PASS
Windows smoke: 4 PASS
hook/offline/actual-byte replay: 31 PASS
historical optional private-fixture SKIP: 4
standalone source rebuild: PASS
```

This verifies the implementation/startup-proof repair only. It does not establish that c2 successfully changes or retains Actor orientation in the running game.

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
10. real-runtime compact-3x3 proof implementation ✅
11. c2 real-game dual-source retained orientation ▶ CURRENT
12. adhesion ⬜
13. surface-relative movement ⬜
14. animation integration ⬜
15. camera and later roadmap ⬜

## Current objective

Do not implement another ownership strategy yet. Run v0.1c2 and establish the real-game gate.

Startup must first report:

```text
PATH_B_COMPACT3X3_PROOF admitted=1 caller_verified=1 writer_verified=1 rdx_to_rcx_map_verified=1 source_bytes=0x24 decoded_prefix_bytes=444 caller_decoded_bytes=128 pairs=9
RESET_COPY_HOOK admitted=1 installed=1
DUAL_SOURCE_ADMISSION admitted=1
```

Then test A Control left-to-right for about 1.5 s and E diagonal left-to-right for about 2 s.

Primary runtime success requires, in the same active session:

```text
PATH_A_OVERRIDE_COUNT > 0
PATH_B_OVERRIDE_COUNT > 0
ORIENTATION_RETENTION_PASS=1
```

Retention threshold remains `dot(ActorUp,N398) >= 0.98` for >=300 ms or >=20 distinct native updates.

If both paths commit but orientation is reset, use `NEW_RESET_AFTER_DUAL_OVERRIDE` as evidence for the next writer investigation. Do not add direct Actor spam writes.

Do not authorize adhesion, movement, animation or camera until the retention gate passes in the real game.

## Human test locations

A Control: current flat unobstructed vertical control wall near Face E. Do not hard-code +Z; use measured N398.

E: same known physical diagonal wall used in prior tests.
