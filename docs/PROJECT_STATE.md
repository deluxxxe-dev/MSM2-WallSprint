# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until the required runtime gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve `AGENTS.md` / `docs/DEV_RULES.md`, exact-build fail-closed behavior, independent implementation, and no private full game bodies/raw user logs in public artifacts.

## Accepted orientation state
Planar Surface Up/orientation remains accepted on cardinal and diagonal walls through verified Deinit.

Latest human c14 A/E log:
- session 1 / A: 4453 ms, verified Deinit, retention pass, final Up dot surface=1, hard kill 0, Position writes 0
- session 2 / E: 4172 ms, verified Deinit, retention pass, final Up dot surface=1.00000012, hard kill 0, Position writes 0
- no stale episode in either session
- entry hook / Position watch cleanup restored normally

## Verified movement chain
```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/0x2CFC93C/0x2CFC942/0x2CFC945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
exact callsite=0x351EEAB -> 0x2CFC900
return=0x351EEB0
observed second return=0x351A020
```
The direct writer call is authenticated and correlated with real Hero translation. Path A Position remains denied.

## c13 actual-game result
Diagnostic PASS. Actual game captured 16 true writer-entry snapshots, 8 per A/E session, all paired to all four Position stores.

Repeated runtime relation:
```text
RDX = entry RSP + 0x28
candidate 20 = [RDX]
candidate 23 = entry stack bytes at RSP+0x28
XYZ == exact executed Position XYZ store values
source W = 0
destination Position W = 1
exact_store_match_mask = 0x7
observed_store_mask = 0xF
```

## c14 actual-game result
**Diagnostic PASS / exact writer-side blocker identified.** See `analysis/V01C14_RUNTIME_RESULT.md`.

C14 repaired the final c13 `MOVEMENT_LINEAGE_SUMMARY`: both real sessions now report `source_capture_ready=1`, `true_entry_snapshot=1`, `true_entry_snapshots=8`, `complete_invocation_maps=8`.

The independent static RDX contract refuses before the known Position stores:
```text
writer_complete_decode=1
entry_store_prefix=0
xyz_RDX_0_4_8_paths=0
W_kind=unknown
first_barrier_rva=0x2CFC91F
first_barrier_instruction=jne
exact_next_producer_rva=0x351EB00
writable_source_proven=0
reason=exact_position_store_sites_mismatch_or_prefix_incomplete
```

Actual A/E entry records still show `entry_store_xyz_match_mask=0x7`, but `xyz_causality_proven=0` because the branch-aware writer dataflow is not yet proven. W source classification is likewise blocked before `0x2CFC945`.

Storage is narrowed to a 16-byte TEB-bounded current-invocation stack object with `RDX == entry RSP+0x28 == precall RSP+0x20`, but caller ownership/exclusivity is still unproven. The observed parent around `0x351A020` has an anchored call at `0x351A01B` to exact next edge `0x351EB00`; construction semantics remain unknown.

A remaining telemetry issue is that `MOVEMENT_ENTRY_CAPTURE_SUMMARY` is emitted before capture activation and reports zeros. The final `MOVEMENT_LINEAGE_SUMMARY` and actual entry records are authoritative. Re-emit/fix the capture summary at finalization in the next diagnostic revision.

## Current gate
```text
writable_source_proven=0
Position/source/input/movement writes=0
```
No surface-relative movement rewrite is authorized yet.

## Current stage
1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅ COMPLETE
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact `0x351EEAB -> 0x2CFC900` callsite ✅
7. true writer-entry capture ✅ c13 real
8. RDX stack-local XYZ bit correlation ✅ c13/c14 real
9. c14 final entry accounting repair ✅
10. writer branch-aware RDX->Position causal proof ▶ CURRENT c15
11. caller/storage/producer ownership (`0x351EB00` next edge) ⏸ after writer contract
12. safe writable upstream movement source ⬜ NOT AUTHORIZED
13. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
14. adhesion/contact refinement ⏸ deferred
15. animation/camera/later traversal ⬜

## Current implementation base
```text
completed branch: codex/v01c14-rdx-source-contract
implementation commit: 617d7b6a84f752582aebd7be8a766c5ef02e476a
published delivery head before human result: 5fd2cdff9ae6732cf0eaa53fec02253c4d6ae25c
version: v0.1c14 / package 0.1.32
```

## Current objective
Resolve only the authenticated writer control-flow barrier beginning at `0x2CFC91F` and prove/reject the RDX XYZ path through the known stores. Do not jump ahead to producer mutation or movement. Keep W separate and keep all Position/source/input writes at zero.
