# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve fail-closed exact-build behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation, and no raw user logs/full private game bodies in public artifacts.

## Accepted orientation / c13 runtime evidence
Planar Surface Up is accepted on cardinal and diagonal walls through verified Deinit.

Latest human c13 combined A -> fast travel -> E log:
- A/session1: 3875 ms, verified Deinit, retention pass, final Up dot surface=1, stale0, hard kill0, Position writes0
- E/session2: 4281 ms, verified Deinit, retention pass, final Up dot surface=1, stale0, hard kill0, Position writes0
- no visible anomaly reported

C13 captured 16 true writer-entry snapshots, eight per surface. Every sampled invocation paired to all four Position stores. On all 16, `RDX = entry RSP+0x28`; guarded `[RDX]` and stack `RSP+0x28` matched executed Position XYZ bits (`mask=0x7`), with source W=0 and destination W=1. This is correlation, not a writable contract.

## Verified movement chain
```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/93C/942/945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
callsite=0x351EEAB -> 0x2CFC900
return=0x351EEB0
observed second return=0x351A020
```

## c14 implementation handoff
```text
branch: codex/v01c14-rdx-source-contract
implementation commit: 617d7b6a84f752582aebd7be8a766c5ef02e476a
delivery head: 5fd2cdff9ae6732cf0eaa53fec02253c4d6ae25c
version: v0.1c14 / package 0.1.32
```

C14 implements a separate read-only bounded decoder/contract diagnostic for the exact RDX XYZ path, independent W, caller stack ownership, and bounded producer relation. It also repairs c13 true-entry/XYZ aggregate accounting. The c12 analyzer and c13 raw entry hook/cleanup remain unchanged.

Cloud validation reported by Codex:
```text
Windows x64 build: PASS
Portable ASan/UBSan: 13 PASS
DLL smoke: 4 PASS
Full public hooks/native: 60 PASS / 5 private-input SKIP
Public-source total: 77 PASS
Separate accepted private-byte replay: 4 PASS records
Focused entry: 7 PASS
Focused contract: 4 PASS
```

Owned fixtures cover scalar/packed SSE/AVX/shuffle/double-lane XYZ/W paths, conservative refusal, stack ownership/bounds, direct/wrapper/tail/indirect producer edges, concurrent/reentrant invocation pairing, partial/unreadable records, and cleanup. Synthetic A/E orientation/Position-watch regressions remain PASS with Position writes0. These fixtures do not establish actual-game c14 causality or ownership.

## Current evidence boundary
C14 actual-game diagnostics are still pending. In particular, cloud tests do not prove the private runtime writer prefix, W behavior, RDX stack ownership, producer provenance, source lifetime, downstream conflict absence, typed semantics, or exclusive intended Hero translation effect.

```text
writable_source_proven=0
Position/source/input/movement writes=0
```

Do not infer velocity, displacement, desired direction, root motion, or writable movement semantics from c13 equality or c14 owned fixtures.

## Current stage
1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅ COMPLETE
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact `0x351EEAB -> 0x2CFC900` callsite ✅
7. true writer-entry capture ✅ c13 actual game
8. RDX stack-local XYZ correlation ✅ c13 actual game
9. c14 RDX XYZ/W/storage/producer diagnostics implementation ✅ cloud validated
10. c14 actual-game source-contract gate ▶ CURRENT
11. safe writable upstream movement source ⬜ NOT AUTHORIZED
12. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
13. adhesion/contact refinement ⏸ deferred
14. animation/camera/later traversal ⬜

## Current action
Run `WallSprint_v0.1c14_rdx_source_contract.zip` in the actual game using ordinary input: A/cardinal ~4 s, fully end WallRun, move or fast-travel to E, E/diagonal ~4 s, then return one complete startup-through-cleanup WallSprint log. Video is optional unless a visible anomaly occurs.

Adjudicate `MOVEMENT_STATIC_CONTRACT`, `MOVEMENT_RDX_XYZ_PROOF`, `MOVEMENT_POSITION_W_PROOF`, `MOVEMENT_RDX_STORAGE_PROOF`, `MOVEMENT_RDX_PRODUCER_PROOF`, and `MOVEMENT_ENTRY_CAPTURE_SUMMARY`. Refusal/partial/unresolved records are exact blockers and must not be bypassed by weakening gates.
