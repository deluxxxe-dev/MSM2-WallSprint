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

## Writer-local call/callee
Authenticated writer call:
```text
0x2CFC927 -> 0x2B23560
return=0x2CFC92C
callee=0x2B23560..0x2B235E9
hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
```
Actual call arguments from c17:
```text
RCX = Transform-derived
RDX = Transform-derived
R8 = constant bits, not pointer
R9 = unknown argument
source_span_alias_mask=0x0
transform_alias_mask=0x3
```
Writer-entry RDX Position source remains distinct from call-time RDX.

## c18 actual-game result
**Diagnostic PASS / exact write blocker identified.** See `analysis/V01C18_RUNTIME_RESULT.md`.

C18 resolved the former `0x2B235C1: je` branch-cap blocker and built the actual bounded local callee CFG:
```text
local_branch_sites=5
conditional_branch_sites=4
branch_cap=5
blocks_total=8
edges_total=10
complete_local_cfg=1
backedges_seen=0
indirect_transfers_seen=0
nested_calls_seen=2
```

Reachable writes classified before the new blocker:
```text
0x2B23560 width8 caller_shadow_or_return_area
0x2B23565 width8 caller_shadow_or_return_area
0x2B2356A width8 callee_private_stack
0x2B2358E width4 hero_transform_possible_alias address_root=1 offset=268/0x10C
```
The first three are disjoint from source/Transform. The fourth is the exact current blocker:
```text
next_barrier_rva=0x2B2358E
next_barrier_instruction=mov
next_barrier_effect=hero_transform_possible_alias
may_alias_source=0
may_alias_transform=1
writes_transform_possible_alias=1
source_span_no_write_proven=0
transform_no_write_proven=0
all_relevant_write_aliases_resolved=0
```
Thus CFG coverage is no longer the problem. The next task must resolve only the exact destination/value/provenance and protected-range relevance of this 4-byte write.

## Current gate
```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```
Upstream producer `0x351EB00` remains deferred.

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
15. exact `0x2B2358E` Transform-alias write contract ▶ CURRENT
16. resume writer XYZ/W causal proof ⏸ after write contract
17. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
18. safe writable upstream movement source ⬜ NOT AUTHORIZED
19. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
20. adhesion/contact refinement ⏸ deferred
21. animation/camera/later traversal ⬜

## Current implementation base / next branch
```text
c18 branch: codex/v01c18-callee-branch-alias
c18 implementation: 19aa3012959bf7538df3917278dd4a97551123dd
c18 delivery head before human result: 3ffc19b5b3b3f42c0fcbf62679fe6d9e44f92dfb
c18 version: v0.1c18 / package 0.1.36
next: c19 exact 0x2B2358E write contract
```
