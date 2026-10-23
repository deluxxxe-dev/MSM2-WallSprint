# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance camera until planar v0.1 movement is robust.

## Verified target
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`

## Surface / orientation
- `this+0x398` = outward normal N398 candidate
- `this+0x338` = plane reference P338
- `this+0x320` = hero-side reference P320
- Path A writer `0x2CFC580`, caller return `0x2CFC8F3`
- Path B writer `0x2CFC3C0..0x2CFC57C`, return `0x2CFC84B`
- planar Surface Up orientation is accepted on cardinal and diagonal walls through verified Deinit

## c11 implementation
```text
branch: codex/v01c11-movement-path-ownership
implementation commit: 2129e69fe003e71bf705fb67f723b447cf3e3764
published head: 9593f0230e220770f2b34faf512ca4108c1825cc
version: v0.1c11 / package 0.1.29
```

## c11 human A -> E result
A/cardinal:
```text
N398≈(+1,0,0)
wallrun_duration_ms=4125
source_active_until_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
hard_kill_calls=0
Position_writes_by_mod=0
```

E/diagonal:
```text
N398≈(-0.7071068,0,+0.7071068)
wallrun_duration_ms=4063
source_active_until_deinit=1
retention_pass=1
last_actor_up_dot_surface≈0.99999994
hard_kill_calls=0
Position_writes_by_mod=0
```

Both Position-watch windows:
```text
watch_armed=1
threads_seen=128
threads_installed=126
unique_writers=14
total_write_events=256
cleanup_restored=1
cleanup_pending=0
```

## Movement writer evidence
Main Transform Position writer family:
```text
writer_function_rva=0x2CFC900
stores=0x2CFC936/0x2CFC93C/0x2CFC942/0x2CFC945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
```

Movement-producing caller chain observed on both A and E:
```text
caller return=0x351EEB0
caller2 return≈0x351A020
consumer=0x2CFC900
```

The 0x351EEB0 path changes real Hero XYZ and correlates with surface-tangent movement.

Path A source+0x30 remains NOT ADMITTED for Position modification. It can match the Position row after its writer, but downstream Position writers are observed and exclusive effects are unproven.

No runtime candidate reached `writable_source_proven=1`.

Detailed result: `analysis/V01C11_RUNTIME_RESULT.md`.

## Deferred visual alignment
User-observed foot floating / wall standoff and body lean remain deferred. Do not solve them in c12.

## Current stage
1. WallRun lifecycle ✅
2. SurfaceFrame / normal ✅
3. Path A/B orientation ownership ✅
4. multithread/race stabilization ✅
5. cardinal + diagonal full lifecycle orientation ✅ COMPLETE
6. bounded Hero Position writer attribution ✅ c11
7. safe upstream movement-source lineage ▶ CURRENT
8. surface-relative input rewrite ⬜
9. visual alignment/contact refinement ⏸ deferred
10. animation/camera/later roadmap ⬜

## Current base
```text
base: 9593f0230e220770f2b34faf512ca4108c1825cc
branch: codex/v01c12-movement-source-lineage
```

## Current objective
Authenticate the source lineage feeding `0x351EEB0 -> 0x2CFC900`, including writer-entry XYZ/W provenance and caller-side construction. Prefer a displacement/velocity/direction/root-motion vector upstream of final Position. c12 is read-only for movement unless a future write contract is independently proven; do not rewrite input or Position in c12.
