# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until the required runtime gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve `AGENTS.md` / `docs/DEV_RULES.md`, exact-build fail-closed behavior, independent implementation, and no private full game bodies/raw user logs in public artifacts.

## Accepted orientation state
Planar Surface Up/orientation is accepted on cardinal and diagonal walls through verified Deinit.

Latest human c13 combined A -> fast travel -> E log:
- session 1 / A: 3875 ms, verified Deinit, retention pass, final Up dot surface=1, hard kill 0, Position writes 0
- session 2 / E: 4281 ms, verified Deinit, retention pass, final Up dot surface=1, hard kill 0, Position writes 0
- no stale episode in either session
- user reported no visible anomaly

Fast travel did not merge or corrupt WallRun sessions; A and E remained independently adjudicable.

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
Diagnostic PASS. See `analysis/V01C13_RUNTIME_RESULT.md`.

Actual game captured 16 true writer-entry snapshots:
- A/session1 invocation 1..8
- E/session2 invocation 9..16

Every snapshot verified the active Hero Transform, exact writer code, return `0x351EEB0`, observed caller2 `0x351A020`, and `true_entry_snapshot=1`.

Every sampled invocation was linked to all four Position stores in the same invocation.

### Strong RDX source evidence
Across all 16 samples:
```text
RDX = entry RSP + 0x28
candidate 20 = pointer read at RDX
candidate 23 = stack snapshot at RSP+0x28
XYZ == exact executed Position XYZ store values
source W = 0
destination Position W = 1
exact_store_match_mask = 0x7
observed_store_mask = 0xF
```
This is strong evidence for a caller-local stack payload feeding Position XYZ. It is not yet a writable contract. Typed semantics, lifetime, producer ownership, W causality, downstream conflicts and exclusive intended translation effect remain unproven.

C13 aggregate telemetry underreports this evidence: final `MOVEMENT_LINEAGE_SUMMARY true_entry_snapshot=0/source_capture_ready=0` contradicts the actual captured records, and candidate stability fields stay 0 because the source matches XYZ but not W. Treat this as a reporting defect to repair, not a runtime capture failure.

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
7. true writer-entry capture ✅ c13 actual game
8. RDX stack-local XYZ source correlation ✅ c13 actual game
9. RDX source construction / W / lifetime / producer contract ▶ CURRENT c14
10. safe writable upstream movement source ⬜ NOT AUTHORIZED
11. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
12. adhesion/contact refinement ⏸ deferred
13. animation/camera/later traversal ⬜

## Current implementation base
Use c13 branch head used for the runtime gate as the code base for c14:
```text
branch: codex/v01c13-writer-entry-snapshot
c14 branch: codex/v01c14-rdx-source-contract
```

## Current objective
Authenticate exactly how the caller constructs the stack-local payload at entry `RSP+0x28` / `RDX`, prove its XYZ path into Transform Position, separately prove destination W behavior, and determine producer/lifetime/downstream ownership. Repair the c13 summary/candidate aggregation defects while preserving all read-only movement constraints.
