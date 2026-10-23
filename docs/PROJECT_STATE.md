# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve exact-build fail-closed behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation, and no raw user logs/full private game bodies in public artifacts.

## Accepted orientation
Planar Surface Up remains accepted through verified Deinit on cardinal and diagonal walls.

Latest human c17 A -> E:
```text
A/session1: 3875 ms, verified Deinit, retention pass, Up dot surface=1, stale0, hard0, Position writes0
E/session2: 4390 ms, verified Deinit, retention pass, Up dot surface=1, stale0, hard0, Position writes0
```
Both sessions finalized 8 true entry snapshots / 8 complete maps with XYZ mask0x7, source W0 and destination W1. Cleanup restored normally.

## Logging
Actual c17 run verified dedicated logging:
```text
game-root\log_wallsprint\WallSprint_YYYYMMDD-HHMMSS.log
shared_logs_fallback=0
```
The real submitted file was `WallSprint_20261024-043843.log`. Preserve this behavior.

## Verified movement chain
```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/0x2CFC93C/0x2CFC942/0x2CFC945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
callsite=0x351EEAB -> 0x2CFC900
return=0x351EEB0
observed second return=0x351A020
```
Runtime source relation remains `RDX=entry RSP+0x28`; sampled `[RDX] XYZ` equals exact Position XYZ store bits. Still correlation until writer-side causality completes.

## c16/c17 call evidence
Authenticated writer call:
```text
0x2CFC927 -> 0x2B23560
return=0x2CFC92C
callee boundary=0x2B23560..0x2B235E9
callee hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
```

C17 actual call arguments:
```text
arg_rcx_origin=entry_writer_RCX_transform
arg_rdx_origin=entry_writer_RCX_transform
arg_r8_origin=constant_bits_NOT_pointer
arg_r9_origin=unknown_argument
source_span_alias_mask=0x0
transform_alias_mask=0x3
proof_complete=1
```

## c17 actual-game verdict
**Diagnostic PASS / narrower exact callee CFG blocker.** See `analysis/V01C17_RUNTIME_RESULT.md`.

C17 stopped before memory-write classification at:
```text
next_barrier_rva=0x2B235C1
next_barrier_instruction=je
next_barrier_effect=indirect_or_branch_cap
reason=callee_indirect_transfer_or_branch_cap_refused
memory_writes_seen=0
source_span_no_write_proven=0
transform_no_write_proven=0
all_relevant_write_aliases_resolved=0
```
`memory_writes_seen=0` is not proof of a write-free callee; local CFG analysis stopped first.

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
13. c17 dedicated timestamped logging ✅ actual game
14. c18 bounded callee CFG + write alias proof ▶ CURRENT
15. resume writer XYZ/W causal proof ⏸ after callee alias
16. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
17. safe writable upstream movement source ⬜ NOT AUTHORIZED
18. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
19. adhesion/contact refinement ⏸ deferred
20. animation/camera/later traversal ⬜

## Current implementation base / next branch
```text
c17 branch: codex/v01c17-callee-alias-contract
c17 implementation: d1867bb2de42079a46ceb547f8fccc4d477ae18d
c17 delivery head: 4f7d1590af744f449d717b8ede30400de0cef820
c17 version: v0.1c17 / package 0.1.35
c18 branch: codex/v01c18-callee-branch-alias
```

## Current action
Run Codex on `docs/CODEX_NEXT.md` for c18. Resolve only the exact local control flow of authenticated callee `0x2B23560..0x2B235E9`, beginning at `0x2B235C1: je`, derive a small function-specific bound from the actual local CFG, and feed reachable writes into the existing c17 address-provenance classifier. Do not begin producer ownership or movement/adhesion/camera work.