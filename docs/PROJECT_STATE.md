# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Repository / branch policy
- Canonical remote: `https://github.com/deluxxxe-dev/MSM2-WallSprint.git`
- Implementation source of truth: `codex/v01c20-edi-value-provenance`
- Owner-confirmed organized baseline: `5192f05f32313919fdf4049bd734b971655d163f`
- `main` is heavily diverged and must **not** be merged into this branch.

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

## Current research blocker — c20 EDI value provenance
C19 proved:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
RCX provenance = entry writer Hero Transform
destination = Hero Transform +0x10C
```

The write does not directly overlap Position `+0x30..+0x3F` or the Transform identity/pointer-carrier range. EDI value provenance remains unresolved.

The prepared c20 bounded local value-provenance implementation is preserved in this branch. After the real-PC workspace acceptance passed on 2026-10-24, `docs/CODEX_NEXT.md` now explicitly reauthorizes a diagnostic-only c20 test build and actual-game A/E capture.

The current question is limited to:
- the exact local reaching-definition/value relation of EDI at `0x2B2358E`;
- all-local-path coverage/conflict/refusal;
- the next precise later read/effect blocker if the local relation succeeds;
- eventual XYZ/W writer causality only if the stricter protected-effect coverage also succeeds.

No semantic ABI type is assigned to EDI or Transform+0x10C without evidence.

## Current safety gate
```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```

Producer `0x351EB00` remains deferred. No movement, adhesion, camera, animation or physics advancement is authorized.

## Local workspace implementation — accepted on real PC
Branch: `codex/v01c20-edi-value-provenance`

Cloud implementation commit: `a4ec9c0f2635e9f2594bf5972d1765332de73766`.
Real-PC acceptance evidence: `analysis/LOCAL_WORKSPACE_RUNTIME_ACCEPTANCE_20261024.md`.

The final local migration/update/logging infrastructure is accepted on the user's real Windows installation:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\
├─ MSM2-WS-v<version>.script
├─ Update-WS.cmd
├─ logs\
├─ _source\
└─ _archive\
```

Verified acceptance includes:
- final `Mods-Developing\MSM2-WS` workspace and versioned `logs` destination;
- ordinary mods + SF + WS recognized together by the shared Overstrike;
- modded game launch;
- visible WS operation;
- versioned WS and SF runtime logs from the same run;
- no evidence that the accepted WS tooling required mutation of SF or ordinary mods.

Runtime logs target:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log
```

The acceptance smoke also preserved the c19 safety baseline (verified WallRun Deinit/retention/Surface Up and Position writes by MOD=0), but those short sessions are not a substitute for the dedicated c20 A/E provenance capture.

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
15. local workspace implementation ✅
16. real-PC workspace/shared-Overstrike/log acceptance ✅
17. c20 EDI value provenance ▶ CURRENT — diagnostic build + actual A/E evidence
18. resume writer XYZ/W causal proof ⏸ after EDI/value + protected-effect contract
19. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
20. safe writable upstream movement source ⬜ NOT AUTHORIZED
21. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
22. adhesion/contact refinement ⏸ deferred
23. animation/camera/later traversal ⬜

## Current action
Follow `docs/CODEX_NEXT.md` on `codex/v01c20-edi-value-provenance`: revalidate and production-enable only the prepared c20 EDI diagnostics, run the relevant regression suite, build/package through the accepted workspace tooling, commit/push, then perform the dedicated A/cardinal ~4 s -> fully end WallRun -> move/fast-travel -> E/diagonal ~4 s runtime capture.

Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable any Position/source/input/movement writes.