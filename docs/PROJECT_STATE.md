# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve exact-build fail-closed behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation, and no raw user logs/full private game bodies in public artifacts.

## Accepted orientation / logging
Latest actual c18 combined A -> E:
```text
A/session1: 2984 ms, verified Deinit, retention pass, Up dot surface=1, stale0, hard0, Position writes0
E/session2: 4171 ms, verified Deinit, retention pass, Up dot surface=1.00000012, stale0, hard0, Position writes0
```
Both sessions finalized 8 true entry snapshots / 8 complete maps, XYZ mask0x7, source W0 and destination W1. Entry diagnostic / Position-watch cleanup restored normally.

Dedicated logging remains actual-game verified:
```text
game-root\log_wallsprint\WallSprint_YYYYMMDD-HHMMSS.log
shared_logs_fallback=0
```
Latest actual file: `WallSprint_20261024-051436.log`.

## Verified movement chain
```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/0x2CFC93C/0x2CFC942/0x2CFC945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
exact callsite=0x351EEAB -> 0x2CFC900
return=0x351EEB0
observed second return=0x351A020
```
Runtime source relation remains `RDX=entry RSP+0x28`; sampled `[RDX] XYZ` equals exact executed Position XYZ bits. Correlation only until writer-side causality completes.

## c18 actual-game result
C18 is Diagnostic PASS. The authenticated callee local CFG is complete (5 local branches, 4 Jcc, 8 blocks, 10 edges). Three reachable stack/home writes are disjoint. The next exact blocker is the 4-byte MOV at `0x2B2358E`, previously classified as possible Hero Transform alias at root1 +0x10C.

## c19 cloud implementation
Branch `codex/v01c19-transform-write-contract`, implementation `b3185cd240a16446a2424ecab9fa0c2605cb00e8`, version v0.1c19/package0.1.37.

C19 implements exact destination/value/reaching-state metadata for `0x2B2358E`, a narrowly admitted model only for an exact scalar dword write at original writer RCX/Transform +0x10C, and conservative read-carrier dependency checks. A real Transform write continues to report literal `transform_no_write_proven=0`; writer continuation instead requires the narrower Position-protected-range and relevant-effect certificates. Unknown/conflicting/computed effects fail closed. Public cloud validation reports 117 PASS / 5 private-input SKIP; no Position/source/input/movement writes or new runtime hooks were added.

Cloud fixtures do not establish the actual game instruction operands/value/all-reaching effects. Actual c19 A/E evidence is required before any later writer/producer task.

## Current gate
```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```
Producer `0x351EB00` remains deferred.

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
10. c15 writer branch crossing ✅
11. c16 exact call target/boundary/hash ✅
12. c17 call argument provenance ✅
13. c17 isolated timestamped logging ✅ actual game
14. c18 actual callee CFG / first real writes ✅
15. c19 exact `0x2B2358E` Transform-write contract ✅ CLOUD; actual A/E PENDING
16. resume writer XYZ/W causal proof ⏸ pending c19 runtime
17. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
18. safe writable upstream movement source ⬜ NOT AUTHORIZED
19. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
20. adhesion/contact refinement ⏸ deferred
21. animation/camera/later traversal ⬜

## Current action
Install `WallSprint_v0.1c19_transform_write_contract.zip` and run normal A/cardinal ~4 s, fully end WallRun, then E/diagonal ~4 s in one run. Submit the newest complete timestamped `log_wallsprint` file. Inspect exact MOV operands/value/root/offset/all-reaching effects and the next blocker or XYZ/W proof. Do not begin producer ownership, movement, adhesion or camera work until this runtime gate is evaluated.