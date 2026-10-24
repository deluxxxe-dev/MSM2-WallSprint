# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve exact-build fail-closed behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation, and no raw user logs/full private game bodies in public artifacts.

## Accepted orientation / logging
Latest actual c19 combined A -> E:
```text
A/session1: 3844 ms, verified Deinit, retention pass, Up dot surface=1, stale0, hard0, Position writes0
E/session2: 4094 ms, verified Deinit, retention pass, Up dot surface=1, stale0, hard0, Position writes0
```
Both sessions finalized 8 true entry snapshots / 8 complete maps, XYZ mask0x7, source W0 and destination W1. Cleanup restored normally.

Dedicated logging remains:
```text
game-root\log_wallsprint\WallSprint_YYYYMMDD-HHMMSS.log
shared_logs_fallback=0
```

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

## c19 actual-game result
C19 is **Diagnostic PASS / exact value-provenance blocker identified**.

The former c18 possible alias is now exact:
```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
RCX provenance = entry writer Hero Transform
destination = Hero Transform +0x10C
```
This write does not directly overlap Position `+0x30..+0x3F` or the Transform identity/pointer-carrier range.

The exact current blocker is the value source `EDI`:
```text
value_source_kind=unknown
value_source_provenance_exact=0
all_reaching_states_same_value_origin=0
all_reaching_states_examined=0
write_affects_writer_reload_provenance=unknown
write_effect_relevant_to_xyz=unknown
write_effect_relevant_to_w=unknown
reason=exact_write_value_unknown_computed_or_reaching_origins_conflict
```
Thus the +0x10C write cannot yet be modeled as irrelevant and writer XYZ/W causality cannot advance past the call.

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
13. c17 isolated timestamped logging ✅
14. c18 actual callee CFG / first real writes ✅
15. c19 exact Transform+0x10C destination ✅ actual game
16. c20 EDI value provenance implementation ✅ CLOUD; actual A/E PENDING
17. resume writer XYZ/W causal proof ⏸ after c20 value contract
18. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
19. safe writable upstream movement source ⬜ NOT AUTHORIZED
20. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
21. adhesion/contact refinement ⏸ deferred
22. animation/camera/later traversal ⬜

## Current implementation / next branch
```text
c19 branch: codex/v01c19-transform-write-contract
c19 implementation: b3185cd240a16446a2424ecab9fa0c2605cb00e8
c19 version: v0.1c19 / package 0.1.37
c20 branch: codex/v01c20-edi-value-provenance
```

## Release naming
User-facing install ZIPs use only the clear version name:
```text
MSM2-WS-v<version>.zip
```
No descriptive suffix such as `transform_write_contract` is allowed. See `docs/DEV_RULES.md`.

## Current action
Run Codex on `docs/CODEX_NEXT.md` for c20. Resolve only EDI reaching definitions and the dependency of the exact Transform+0x10C write. Do not begin producer ownership or movement/adhesion/camera work.
## c20 cloud implementation

Version v0.1c20/package0.1.38 on codex/v01c20-edi-value-provenance. Local EDI reaching definitions and low32 copy/constant/memory/affine relations are implemented independently of pointer types. Local value coverage cannot replace the existing protected-effect/whole writer gates. Current real-game producers/value/dependency/XYZ/W or next blocker remain PENDING A/E. New version-only install/source ZIPs and WS-only UpdateForTest.cmd target Mods-Developing and explicit separated Overstrike with game-root WD; no SF or unrelated cleanup. See V01C20_EDI_VALUE_PROVENANCE.md/CODEX_RESULT.md.
