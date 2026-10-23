# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve fail-closed exact-build behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation, and no raw user logs/full private game bodies in public artifacts.

## Accepted orientation
Planar Surface Up is accepted on cardinal and diagonal walls through verified Deinit.

Latest human c14 A/E log:
- A/session1: 4453 ms, verified Deinit, retention pass, final Up dot surface=1, stale0, hard kill0, Position writes0
- E/session2: 4172 ms, verified Deinit, retention pass, final Up dot surface=1.00000012, stale0, hard kill0, Position writes0
- entry-hook / Position-watch cleanup restored normally

## Verified movement chain
```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/93C/942/945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
callsite=0x351EEAB -> 0x2CFC900
return=0x351EEB0
observed second return=0x351A020
```

## c13/c14 runtime source evidence
C13 captured 16 true writer-entry snapshots, eight per surface. C14 preserves the same real relation:
```text
RDX = entry RSP+0x28
[RDX] XYZ == executed Position XYZ store bits
entry_store_xyz_match_mask=0x7
source W observed 0
destination W observed 1
```
This is still correlation until writer-side decoded causality is proven.

C14 also repairs the c13 final aggregate: both sessions report `source_capture_ready=1`, `true_entry_snapshot=1`, `true_entry_snapshots=8`, `complete_invocation_maps=8`.

## c14 actual-game verdict
**Diagnostic PASS / exact local blocker identified.** See `analysis/V01C14_RUNTIME_RESULT.md`.

Independent static contract result:
```text
writer_complete_decode=1
entry_store_prefix=0
xyz_RDX_0_4_8_paths=0
W_kind=unknown
W_classified=0
first_barrier_rva=0x2CFC91F
first_barrier_instruction=jne
caller_prefix=1
caller_RDX_local=0
caller_allocation_owned=0
caller2_relation=unresolved
exact_next_producer_rva=0x351EB00
writable_source_proven=0
reason=exact_position_store_sites_mismatch_or_prefix_incomplete
```

Thus actual entries still match XYZ bits, but `xyz_causality_proven=0`. W remains unclassified because the same writer-prefix proof stops first.

Storage is narrowed to a 16-byte TEB-bounded current-invocation stack object with `RDX == entry RSP+0x28 == precall RSP+0x20`, but caller ownership/exclusivity is unproven. Parent anchor `0x351A01B` points to exact next producer edge `0x351EB00`; do not chase/mutate it until writer causality is proven.

`MOVEMENT_ENTRY_CAPTURE_SUMMARY` is still emitted before capture activation and reports zero; final `MOVEMENT_LINEAGE_SUMMARY` and actual entry records are authoritative. C15 should re-emit the capture summary at finalization.

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
6. exact `0x351EEAB -> 0x2CFC900` callsite ✅
7. true writer-entry capture ✅ c13 real
8. RDX stack-local XYZ bit correlation ✅ c13/c14 real
9. c14 final entry accounting repair ✅
10. writer branch-aware RDX->Position causal proof ▶ CURRENT c15
11. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
12. safe writable upstream movement source ⬜ NOT AUTHORIZED
13. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
14. adhesion/contact refinement ⏸ deferred
15. animation/camera/later traversal ⬜

## Current implementation base / next branch
```text
c14 branch: codex/v01c14-rdx-source-contract
c14 implementation: 617d7b6a84f752582aebd7be8a766c5ef02e476a
c14 published delivery before human result: 5fd2cdff9ae6732cf0eaa53fec02253c4d6ae25c
c15 branch: codex/v01c15-writer-branch-contract
```

## Current action
Run Codex on `docs/CODEX_NEXT.md`. C15 is read-only and limited to the authenticated writer control-flow/dataflow barrier beginning at `0x2CFC91F`; no movement/source mutation is authorized.
