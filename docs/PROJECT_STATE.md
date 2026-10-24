# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve exact-build fail-closed behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation, and no raw user logs/full private game bodies in public artifacts.

## Accepted c19 runtime result
Latest accepted A -> E:

```text
A/session1: 3844 ms, verified Deinit, retention pass, Up dot surface=1, stale0, hard0, Position writes0
E/session2: 4094 ms, verified Deinit, retention pass, Up dot surface=1, stale0, hard0, Position writes0
```

Both sessions finalized 8 true entry snapshots / 8 complete maps, XYZ mask0x7, source W0 and destination W1. Cleanup restored normally.

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

## Current research blocker
C19 proved:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
RCX provenance = entry writer Hero Transform
destination = Hero Transform +0x10C
```

The write does not directly overlap Position `+0x30..+0x3F` or the Transform identity/pointer-carrier range. EDI value provenance remains unresolved. Prepared c20 EDI research is preserved but production-disabled and is not authorized until the local workspace acceptance gate below passes.

## Current gate
```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```

Producer `0x351EB00` remains deferred. No movement, adhesion, camera, animation or physics advancement is authorized.

## Local workspace implementation — cloud complete, real-PC acceptance pending
Branch: `codex/v01c20-edi-value-provenance`

Implementation commit: `a4ec9c0f2635e9f2594bf5972d1765332de73766`.
Current delivery head: `b191708c07bf8013f7a3e401a3dc2beaa09aee74`.

Cloud implementation completed the final local migration/update/logging infrastructure described by `docs/LOCAL_WORKSPACE.md`:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\
├─ MSM2-WS-v<version>.script
├─ Update-WS.cmd
├─ logs\
├─ _source\
└─ _archive\
```

A one-time `Migrate-WS.cmd` is included. `_source` is required to be an independent clone. Repository/release behavior no longer requires a live tracked `WallSprint.script` inside the recursively scanned source tree. Deployment is transactional and asserts one active WS script recursively. WS tooling protects SF and ordinary mods and only validates shared Overstrike infrastructure rather than owning it.

Runtime logs now target:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log
```

Cloud validation reported 131 public PASS records plus separate historical replay, including workspace filesystem/Git fixtures and actual Windows logger smoke. Prepared EDI offline tests are included but production EDI remains disabled. These tests do not prove real Windows migration, WSL prerequisites, process stopping, shared Overstrike UI recognition, or actual runtime logging on the user's machine.

## Current blocker / required human acceptance
Before any further gameplay-research implementation, run the produced migration on the real Windows installation and verify:

1. migration completes without touching SF or ordinary mods;
2. exactly one active `MSM2-WS-v<version>.script` exists under the WS workspace;
3. the shared `<game>\Overstrike\Overstrike.exe` sees ordinary mods + SF + WS;
4. `Install mods` -> `Run modded game` succeeds;
5. a new versioned log appears under `MSM2-WS\logs`;
6. optional regression run A ~4 s -> fully end WallRun -> move/fast-travel -> E ~4 s retains the accepted c19 safety/orientation behavior.

Until this gate passes, do not resume EDI provenance or advance to producer/movement work.

## Current stages
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
13. c18 actual callee CFG / first real writes ✅
14. c19 exact Transform+0x10C destination ✅ actual game
15. local workspace implementation ✅ cloud
16. real-PC workspace/shared-Overstrike/log acceptance ▶ CURRENT GATE
17. c20 EDI value provenance ⏸ production-disabled until gate passes
18. resume writer XYZ/W causal proof ⏸ after EDI contract
19. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
20. safe writable upstream movement source ⬜ NOT AUTHORIZED
21. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
22. adhesion/contact refinement ⏸ deferred
23. animation/camera/later traversal ⬜

## Current action
Use `MSM2-WS-v0.1c20.zip` outside Mods Library and run the migration helper against the real game root as documented in `docs/CODEX_NEXT.md`. Do not delete the legacy environment until real-PC acceptance passes.
