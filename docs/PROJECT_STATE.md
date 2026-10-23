# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until the runtime ownership gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve fail-closed exact-build behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation, and no raw user logs/full private game bodies in public artifacts.

## Accepted orientation
Planar Surface Up is accepted on cardinal and diagonal walls through verified Deinit.

Latest human c13 combined A -> fast travel -> E log:
- A/session1: 3875 ms, verified Deinit, retention pass, final Up dot surface=1, stale0, hard kill0, Position writes0
- E/session2: 4281 ms, verified Deinit, retention pass, final Up dot surface=1, stale0, hard kill0, Position writes0
- no visible anomaly reported

Fast travel did not merge/corrupt sessions; the combined-log workflow is accepted.

## Verified movement chain
```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/93C/942/945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
callsite=0x351EEAB -> 0x2CFC900
return=0x351EEB0
observed second return=0x351A020
```

## c13 actual-game verdict
Diagnostic PASS. See `analysis/V01C13_RUNTIME_RESULT.md`.

Actual game captured 16 true writer-entry snapshots, 8 on A and 8 on E. Every sampled invocation paired to all four Position stores.

Repeated source relation on all 16 invocations:
```text
RDX = entry RSP + 0x28
candidate20=[RDX]
candidate23=stack RSP+0x28
XYZ == exact executed Position XYZ stores
source W=0
Position W=1
exact_store_match_mask=0x7
observed_store_mask=0xF
```
This is strong evidence that a caller-local stack payload feeds Position XYZ. It is not yet a writable contract.

C13 aggregate telemetry has a reporting defect: final summary says `true_entry_snapshot=0/source_capture_ready=0` despite real captured snapshots, and stability fields remain 0 because the current aggregation requires W to match too.

## Current gate
```text
writable_source_proven=0
Position/source/input/movement writes=0
```
Still unproven: exact RDX->XYZ causal dataflow, W source/meaning, stack-object lifetime, producer provenance through the observed caller2 path, downstream conflict behavior and exclusive intended translation effect.

## Current stage
1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅ COMPLETE
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact `0x351EEAB -> 0x2CFC900` callsite ✅
7. true writer-entry capture ✅ c13 actual game
8. RDX stack-local XYZ correlation ✅ c13 actual game
9. RDX source construction / W / lifetime / producer contract ▶ CURRENT c14
10. safe writable upstream movement source ⬜ NOT AUTHORIZED
11. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
12. adhesion/contact refinement ⏸ deferred
13. animation/camera/later traversal ⬜

## Current branch
```text
codex/v01c14-rdx-source-contract
```

## Current objective
Authenticate the RDX stack-local XYZ contract, prove W independently, trace who constructs the `RSP+0x28` payload before the writer call, and repair c13 telemetry aggregation. Keep movement read-only until a later explicit write gate.
