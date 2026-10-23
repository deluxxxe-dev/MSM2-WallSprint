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

The current preferred control surface is a large unobstructed A-direction wall.

## Orientation Path A

- writer RVA `0x2CFC580`
- WallRun caller return RVA `0x2CFC8F3`
- caller-local source contains padded Right / Up / Forward plus Position
- orientation source range currently owned is `[source+0x00, source+0x30)`
- Position remains unchanged
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

Real-game compact proof passes and Path B writes the intended surface-relative basis.

## Orientation history

- c2: visible surface-relative rotation, rapid WorldUp/SurfaceUp flicker.
- c3: multithread writer tracing attempted; production expansion failed.
- c4: thread-watch repaired; first retention PASS, but source ownership ended early.
- c5: lifecycle continuity repaired; real full-WallRun orientation hold passed.
- c6: Position/adhesion remained read-only, but a real ~250 ms native Update gap caused `fresh_update_age` to permanently end source ownership while the same verified WallRun later continued.

## v0.1c5 authoritative full-WallRun PASS

```text
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

## v0.1c6 real-game regression

Implementation:

```text
branch: codex/v01c6-planar-adhesion
implementation commit: 6ed21d411b8b6868185143d275c2e40486b57781
published head: db874c74cc14f361a2ceb5e15647910e06ebfb84
version: v0.1c6 / package 0.1.24
```

Adhesion was read-only as intended:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
mode=position_read_only
```

Therefore the visible mid-run release was not caused by Position correction.

Exact failing gate:

```text
failing_sub_gate=fresh_update_age
update_age_ms=250
fresh_update_age_ok=0
ctx_wallrun_active=1
owner_matches_ctx=1
hero_matches=1
transform_matches=1
accepted_event_epoch=0
native_collision_epoch=0
```

Final session summary:

```text
wallrun_duration_ms=4078
source_exit_reason=native_lifecycle_inactive_or_stale
source_active_until_deinit=0
verified_deinit=1
path_a_writes=132
path_b_writes=130
retention_pass=1
max_held_ms=1422
max_held_updates=114
last_actor_up_dot_surface=1
```

The same verified WallRun continued for about 1.4 s after source ownership was permanently closed.

Detailed result: `analysis/V01C6_RUNTIME_RESULT.md`.

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
12. retention threshold ✅
13. lifecycle-wide hold under normal cadence ✅ c5
14. transient native-update hitch recovery ▶ CURRENT
15. planar adhesion / safe Position-source ownership ⏸ resume after 14
16. surface-relative movement ⬜
17. animation integration ⬜
18. camera and later roadmap ⬜

## Current implementation base

Use the published c6 implementation as the base so the Position diagnostics are retained but still read-only:

```text
branch: codex/v01c6-planar-adhesion
implementation commit: 6ed21d411b8b6868185143d275c2e40486b57781
published head: db874c74cc14f361a2ceb5e15647910e06ebfb84
```

Pre-created next branch:

```text
codex/v01c7-transient-freshness-recovery
```

## Current objective

Repair lifecycle semantics for a transient stale-update interval without weakening hard identity/integrity gates.

The existing ~100 ms freshness condition must continue to prevent source writes while the token is stale. However, `fresh_update_age` alone must not permanently destroy an otherwise coherent matching WallRun session. When a fresh verified native Update resumes for the same session/owner/generation/native thread and identities remain valid, source ownership should resume using the existing SurfaceFrame target without restarting the WallRun session or interpolation.

Permanent close must remain fail-closed for verified Deinit, owner/session/generation change, Hero/Transform identity change, native collision/epoch failure, wrong thread/caller/stack/integrity state, kill/F8/shutdown, or other hard invalidation.

Do not advance adhesion Position writes until real-game full-WallRun orientation passes again with this recovery behavior.