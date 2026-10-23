# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve fail-closed exact-build behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation suitable for Nexus Mods, and no raw user logs/full private game bodies in public artifacts.

## Accepted orientation
Planar Surface Up remains accepted on cardinal and diagonal walls through verified Deinit.

Latest accepted human c15 A/E:
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
This remains correlation until the writer-side causal contract completes.

## c15 actual-game verdict
Diagnostic PASS. The bounded branch proof crossed `0x2CFC91F jne` and stopped at the exact next local blocker `0x2CFC927 call`. Four Position store sites were authenticated, but XYZ causality remains unproven because the call effect is unresolved. Final capture summaries are correct at 8 true entries / 8 complete maps per A/E session.

## c16 implementation handoff
**Implementation complete; actual-game c16 result pending.**

```text
branch: codex/v01c16-writer-call-effect
implementation commit: 0a00bdeee05ffdae593d6963abd5c24424a2d8ef
validated/package head: 8f66b15a109e503cdfc115eb5a48ebadd7bc4113
version: v0.1c16 / package 0.1.34
```

C16 adds a bounded writer-local call-effect contract for the single authenticated call at `0x2CFC927`, return `0x2CFC92C`. It authenticates direct target/runtime-function boundary/current code identity, refuses indirect/thunk/out-of-image/overlap cases, and analyzes only a bounded <=1024-byte linear leaf callee. Unknown memory effects, nested calls, branches, unsupported effects, stack-boundary violations, or uncertain provenance fail closed.

The effect model permits only explicitly proven private callee-stack writes and tracks register/stack provenance. Win64 volatile GPRs and XMM0..5 are invalidated conservatively; writer XYZ may continue only through independently preserved/reloaded provenance. Per-invocation TEB/source/destination/code-integrity checks remain required before any causal flag. W remains independently classified.

No new game-call invocation or general interprocedural traversal was introduced. `0x351EB00` remains deferred.

Cloud validation reported:
```text
Portable ASan/UBSan: 13 PASS
DLL smoke: 4 PASS
Full public hooks/native: 75 PASS / 5 private-input SKIP
Total public PASS records: 92
C16 exact call/effect focused records: 7 PASS
```
These owned-fixture results validate the mechanism only; they do not establish the actual game's call target/effect.

## Current gate
```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
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
11. c16 writer-call effect mechanism ✅ IMPLEMENTED / ACTUAL GAME PENDING
12. actual c16 call target/effect + resumed writer proof ▶ CURRENT RUNTIME GATE
13. caller/storage/producer ownership (`0x351EB00`) ⏸ only after writer contract
14. safe writable upstream movement source ⬜ NOT AUTHORIZED
15. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
16. adhesion/contact refinement ⏸ deferred
17. animation/camera/later traversal ⬜

## Current action
Install `WallSprint_v0.1c16_writer_call_effect.zip` and collect one complete private A/cardinal ~4 s -> fully end WallRun -> move/fast-travel -> E/diagonal ~4 s log. Confirm c16/package 0.1.34 and exact-build admission. The log must determine the actual `0x2CFC927` target/boundary/hash/effect, the next writer blocker or XYZ proof, independent W classification, finalized capture/lineage summaries, and clean A/E orientation/watch cleanup. Do not begin producer ownership or any movement/adhesion/camera implementation from cloud-only evidence.