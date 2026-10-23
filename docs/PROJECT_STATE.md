# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance camera until planar v0.1 movement is robust.

## Exact target
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`

## Orientation milestone
Planar Surface Up/orientation is accepted. c12 real A/E did not regress it.

A/cardinal:
```text
wallrun_duration_ms=4266
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
hard_kill_calls=0
Position_writes_by_mod=0
```

E/diagonal:
```text
N398≈(-0.7071068,0,+0.7071068)
wallrun_duration_ms=4297
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface≈1
hard_kill_calls=0
Position_writes_by_mod=0
```

E had one late 172 ms native Update gap and 72 ms stale suspend/resume; it recovered normally with coherent identity and is not a blocker.

## Verified movement chain
```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/0x2CFC93C/0x2CFC942/0x2CFC945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
return=0x351EEB0
exact callsite=0x351EEAB -> 0x2CFC900
observed second return=0x351A020
```

The direct 0x351EEAB call is authenticated and the chain correlates with real Hero translation on A and E. Path A Position remains denied.

## c12 implementation
```text
branch: codex/v01c12-movement-source-lineage
implementation commit: 4c84d634df4d629b237d4cc311d5e5cd6de4398c
published head: c966d233787504dda88f65d1825c5c1c408a2d67
version: v0.1c12 / package 0.1.30
```

## c12 actual-game verdict
**Diagnostic PASS / exact missing edge.**

Both A and E produced:
```text
first_unsupported_offset=0xA
complete_writer_CFG=0
source_prefix_proven=0
source_capture_ready=0
eligible_hits=0
attempts=0
direct_call_verified=1
caller2_direct_relation_verified=0
true_entry_snapshot=0
writable_source_proven=0
```

The blocker is diagnostic/semantic, not surface-specific. Detailed result: `analysis/V01C12_RUNTIME_RESULT.md`.

## Deferred visual alignment
User-observed wall standoff/foot floating and root/body lean remain deferred. No active adhesion yet.

## Current stage
1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅ COMPLETE
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact 0x351EEAB -> 0x2CFC900 callsite ✅
7. c12 complete source lineage ❌ exact blocker identified
8. c13 true writer-entry snapshot / invocation correlation ▶ CURRENT
9. safe writable upstream movement source ⬜
10. surface-relative input rewrite ⬜
11. visual alignment/contact ⏸ deferred
12. animation/camera/later traversal ⬜

## c13 base
```text
base: c966d233787504dda88f65d1825c5c1c408a2d67
branch: codex/v01c13-writer-entry-snapshot
```

## c13 objective
Capture actual entry-time state for verified Hero-targeted writer `0x2CFC900` invocations, pair each bounded invocation to the existing Position stores, and identify the exact entry register/pointer/stack source feeding Hero Position. Keep movement and Position read-only and do not weaken c12 gates.

## c13 implementation status

v0.1c13 / package0.1.31 implements the true-entry diagnostic on codex/v01c13-writer-entry-snapshot. Exact implementation SHA and final build/test/artifact evidence are in docs/CODEX_RESULT.md / analysis/V01C13_RELEASE.json. Native A/B source admission, c7-c10 lifecycle policy and c12 static proof stay unchanged; the new MinHook entry is diagnostic only, during the existing Position watch. Maximum8 entries/session, raw ABI preserved, explicit entry-RSP/thread/invocation linkage, guarded bounded raw origins and source-register/immediate store bits.

Cloud owned tests validate the mechanism, including concurrent/reentrant calls and transparent cleanup. **Actual c13 A/E installation/entry/store/candidate evidence remains PENDING**, not a completed game source contract. writable_source_proven=0 and Position/input/movement/source writes=0. Next action is ordinary~4s A/cardinal then E/diagonal human run and private log adjudication; no automatic movement rewrite.
