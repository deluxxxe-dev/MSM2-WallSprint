# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Architecture is surface-relative locomotion, not an extended vanilla WallRun. Do not advance to camera until planar v0.1 locomotion is robust.

## Exact target / fail-closed gate
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve `AGENTS.md` and `docs/DEV_RULES.md`; no private game bodies/raw user logs in public source/package; independent implementation only.

## Verified orientation state
Planar Surface Up/orientation is accepted. c12 did not regress it.

Human c12 A/cardinal:
```text
wallrun_duration_ms=4266
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
Position_writes_by_mod=0
```

Human c12 E/diagonal:
```text
N398≈(-0.7071068,0,+0.7071068)
wallrun_duration_ms=4297
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface=1.00000012
first_hard_kill=none
hard_kill_calls=0
Position_writes_by_mod=0
```

E had one late native Update gap (`native_gap_ms=172`) producing a bounded 72 ms stale suspend/resume. Surface Up was already held; identity/generation/thread remained coherent; it resumed normally and is not a regression.

## Verified movement-path evidence
c11/c12 actual-game evidence establishes the measured translation chain:

```text
Hero Transform Position stores:
0x2CFC936 -> +0x30
0x2CFC93C -> +0x34
0x2CFC942 -> +0x38
0x2CFC945 -> +0x3C
writer function: 0x2CFC900..0x2CFC9B7
observed/direct return: 0x351EEB0
exact callsite: 0x351EEAB -> 0x2CFC900
observed second return: 0x351A020
```

The exact direct `0x351EEAB -> 0x2CFC900` relationship is authenticated in c12. On both A and E, this chain correlates with real Hero translation. On E, the measured X/Z Position component writes decompose into the expected diagonal SurfaceFrame relation.

Path A `source+0x30` remains rejected as a movement write source because downstream overwrite/exclusive semantics are not proven.

## c12 implementation
```text
branch: codex/v01c12-movement-source-lineage
implementation commit: 4c84d634df4d629b237d4cc311d5e5cd6de4398c
published head: c966d233787504dda88f65d1825c5c1c408a2d67
version: v0.1c12 / package 0.1.30
```

## c12 actual-game verdict
**Diagnostic PASS / exact missing edge.**

A and E reproduced the same blocker:

```text
MOVEMENT_WRITER_PROOF
writer_rva=0x2cfc900
function_size=183
complete_decode=1
lineage_complete=0
source_map_supported_prefix=0
first_unsupported_offset=0xa
unsupported=35
writable_source_proven=0

MOVEMENT_LINEAGE_SUMMARY
source_capture_ready=0
eligible_hits=0
attempts=0
complete_writer_CFG=0
source_prefix_proven=0
direct_call_verified=1
caller2_direct_relation_verified=0
true_entry_snapshot=0
writable_source_proven=0
reason=entry_snapshot_typed_caller_W_and_exclusive_effects_unproven
```

Caller2 static direct relation is not proven (`direct_target_mismatch`). This does not invalidate the observed runtime second return; it simply cannot be promoted to a direct static producer relation.

The blocker is therefore diagnostic/semantic, not surface-specific. `writable_source_proven=0` remains authoritative.

Detailed real result: `analysis/V01C12_RUNTIME_RESULT.md`.

## Deferred visual alignment
The user observed body/root lean toward apparent ground and excessive apparent wall standoff/floating feet. These remain later visual/contact tasks. Do not infer an active adhesion requirement from the current root-distance metric.

## Current stage
1. WallRun lifecycle / surface data / SurfaceFrame ✅
2. Path A/B orientation ownership and stabilization ✅
3. planar A/E orientation milestone ✅ COMPLETE
4. Hero Position writer attribution ✅ c11
5. SurfaceFrame translation decomposition ✅ c11/c12 real
6. exact `0x351EEAB -> 0x2CFC900` direct callsite ✅ c12 real
7. c12 true writer source lineage ❌ exact blocker identified
8. c13 true writer-entry snapshot / invocation correlation ▶ CURRENT
9. safe upstream writable movement source ⬜ NOT AUTHORIZED
10. surface-relative input rewrite ⬜ NOT AUTHORIZED
11. adhesion / visual contact refinement ⏸ deferred
12. animation / camera / later traversal ⬜

## Current implementation base
Use c12 delivery head exactly:

```text
base branch: codex/v01c12-movement-source-lineage
implementation commit: 4c84d634df4d629b237d4cc311d5e5cd6de4398c
published head: c966d233787504dda88f65d1825c5c1c408a2d67
package: 0.1.30
```

Pre-created next branch:
```text
codex/v01c13-writer-entry-snapshot
```

## Current blocker / objective
The smallest missing edge is the c12-reported `true_entry_snapshot=0` for writer `0x2CFC900`.

c13 must capture the actual writer-entry state for active verified Hero Transform calls and correlate each bounded invocation with the existing Position write events. The purpose is to identify which entry register/pointer/stack value feeds the four Position stores and whether that source has stable provenance suitable for one more upstream ownership proof.

Do not implement movement, input rewriting, Position injection, active adhesion, custom gravity, camera, animation or later traversal in c13. Do not loosen c12 proof gates merely to obtain `writable_source_proven=1`.
