# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve fail-closed exact-build behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation, and no raw user logs/full private game bodies in public artifacts.

## Accepted orientation
Planar Surface Up remains accepted on cardinal and diagonal walls through verified Deinit.

Latest human c15 A/E log:
```text
A/session1: 3860 ms, verified Deinit, retention pass, Up dot surface=1, stale0, hard0, Position writes0
E/session2: 4297 ms, verified Deinit, retention pass, Up dot surface≈1.00000012, stale0, hard0, Position writes0
```
Entry-hook / Position-watch cleanup restored normally.

## Verified movement chain
```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/93C/942/945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
callsite=0x351EEAB -> 0x2CFC900
return=0x351EEB0
observed second return=0x351A020
```

Runtime A/E source relation remains:
```text
RDX = entry RSP+0x28
[RDX] XYZ == exact executed Position XYZ store bits
entry_store_xyz_match_mask=0x7
source W observed 0
destination W observed 1
```
Correlation is strong but is not yet writable-source proof.

## c15 actual-game verdict
**Diagnostic PASS / exact next local writer blocker.** See `analysis/V01C15_RUNTIME_RESULT.md`.

C15 crosses the c14 `0x2CFC91F jne` barrier. The next exact blocker is:
```text
0x2CFC927: call
```

Actual branch contract:
```text
writer_complete_decode=1
first_branch_rva=0x2CFC91F
blocks_visited=2
edges_visited=2
branches_seen=4
exact_store_sites_verified=1
all_relevant_store_paths_proven=0
proven_component_mask=0x0
next_barrier_rva=0x2CFC927
next_barrier_instruction=call
W_independent=1
W_kind=unknown
writable_source_proven=0
reason=unsupported_relevant_writer_instruction_or_memory_effect
```

Per invocation, XYZ still match (`mask=0x7`) but `xyz_causality_proven=0` remains correct because the call effect is unresolved.

## Capture-summary repair
C15 fixes the early-zero summary. Both A/E finalize with:
```text
capture_activated=1
true_entry_snapshots=8
complete_invocation_maps=8
xyz_source_candidate=20
xyz_match_count=8
xyz_match_mask=0x7
xyz_stable_across_invocations=1
source_w_bits=0x0
destination_w_bits=0x3f800000
phase=finalized
summary_final=1
```

## Producer/storage boundary
Still unpromoted:
```text
RDX == entry RSP+0x28 == precall RSP+0x20
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
exact_next_producer_rva=0x351EB00
```
Do not chase/mutate `0x351EB00` until the writer-local call effect is resolved.

## Current gate
```text
writable_source_proven=0
Position/source/input/movement writes=0
```

## Current stage
1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅ COMPLETE
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact writer callsite ✅
7. true writer-entry capture ✅
8. RDX stack-local XYZ correlation ✅
9. c14 JNE blocker identified ✅
10. c15 branch-aware crossing of `0x2CFC91F` ✅
11. writer call `0x2CFC927` effect contract ▶ CURRENT c16
12. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
13. safe writable upstream movement source ⬜ NOT AUTHORIZED
14. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
15. adhesion/contact refinement ⏸ deferred
16. animation/camera/later traversal ⬜

## Current implementation base / next branch
```text
c15 branch: codex/v01c15-writer-branch-contract
c15 implementation: 0efd13924426b1c96247ff35577f7f323c934b97
c15 published delivery: 8e500b2e5f4967457f8652f46e18eb2c6d4fcb7d
c16 branch: codex/v01c16-writer-call-effect
```

## Current action
Run Codex on `docs/CODEX_NEXT.md`. C16 remains diagnostic/read-only and resolves only the exact writer-local call at `0x2CFC927`; no movement/source/input mutation is authorized.
