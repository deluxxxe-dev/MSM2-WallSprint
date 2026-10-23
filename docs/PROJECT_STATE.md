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

Latest human c16 combined A -> E result:
```text
A/session1: 3875 ms, verified Deinit, retention pass, Up dot surface=1, stale0, hard0, Position writes0
E/session2: 4203 ms, verified Deinit, retention pass, Up dot surface=0.99999994, stale0, hard0, Position writes0
```
Session1 also naturally exercised c10 publication refresh once and admitted it. Entry diagnostic / Position-watch cleanup restored normally.

Each session finalized 8 true entry snapshots / 8 complete invocation maps with XYZ mask0x7, source W0, destination W1.

## Verified movement chain
```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/0x2CFC93C/0x2CFC942/0x2CFC945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
exact callsite=0x351EEAB -> 0x2CFC900
return=0x351EEB0
observed second return=0x351A020
```
Runtime source relation remains `RDX = entry RSP+0x28`, with `[RDX] XYZ` equal to exact executed Position XYZ bits on sampled A/E invocations. This is still correlation, not mutation permission.

## c15 result
Branch-aware writer proof crossed `0x2CFC91F jne` and narrowed the writer-local blocker to `0x2CFC927 call`.

## c16 actual-game result
**Diagnostic PASS / exact callee alias blocker identified.** See `analysis/V01C16_RUNTIME_RESULT.md`.

Authenticated call contract:
```text
writer call=0x2CFC927
return=0x2CFC92C
kind=direct_call
target=0x2B23560
boundary=0x2B23560..0x2B235E9
hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
target_complete_decode=1
```

Exact unresolved effect:
```text
effect=source_span_effect_unknown
source_span_no_write_proven=0
source_span_may_write=0
source_span_effect_unknown=1
next_callee_barrier_rva=0x2B23560
next_callee_instruction=mov
reason=callee_memory_write_alias_to_source_stack_or_destination_unresolved
```
C16 cannot yet prove that every callee memory write is disjoint from the 16-byte entry-RDX source object and the Hero Transform destination. Therefore `xyz_causality_proven=0` and W causality remain unproven.

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
10. c15 branch crossing ✅
11. c16 exact call target/boundary/hash ✅
12. c17 callee memory-write alias contract ▶ CURRENT
13. resume writer XYZ/W causal proof ⏸ after callee alias
14. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
15. safe writable upstream movement source ⬜ NOT AUTHORIZED
16. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
17. adhesion/contact refinement ⏸ deferred
18. animation/camera/later traversal ⬜

## Current implementation base / next branch
```text
c16 branch: codex/v01c16-writer-call-effect
c16 implementation: 0a00bdeee05ffdae593d6963abd5c24424a2d8ef
c16 delivery head: 8f66b15a109e503cdfc115eb5a48ebadd7bc4113
c16 version: v0.1c16 / package 0.1.34
c17 branch: codex/v01c17-callee-alias-contract
```

## Current objective
Resolve only callee `0x2B23560..0x2B235E9` memory-write address provenance relative to the entry-RDX 16-byte source object and Hero Transform destination. If all relevant writes are proven disjoint, resume the existing writer proof from `0x2CFC92C`; otherwise emit one exact narrower callee blocker. No producer traversal and no movement/source/input mutation in c17.

## c17 cloud implementation

v0.1c17/package0.1.35 pins the measured137byte callee identity and analyzes write-address origins using every relevant writer pre-call state. Source/Transform writes, unknown/conflicting addresses and nested/unsupported effects refuse exactly. Private-stack/home/global separation requires per-invocation TEB/source/nonstack Transform/image checks. Only both protected no-write certificates plus all-resolved can resume writer exact XYZ/W stores. Actual c17 alias result or next blocker is pending A/E. Producer0x351EB00 and all upstream mutation/ownership/lifetime/exclusive/downstream gates remain0/deferred; no new hook or movement/source/input writes.
