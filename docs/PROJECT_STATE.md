# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

The immediate task is an infrastructure-only finalization of the local development/test workspace. It must preserve the accepted gameplay/reverse-engineering state below.

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

The write does not directly overlap Position `+0x30..+0x3F` or the Transform identity/pointer-carrier range.

The unresolved value source is:

```text
value_source_register_or_memory=EDI
value_source_kind=unknown
value_source_provenance_exact=0
all_reaching_states_same_value_origin=0
all_reaching_states_examined=0
write_affects_writer_reload_provenance=unknown
write_effect_relevant_to_xyz=unknown
write_effect_relevant_to_w=unknown
reason=exact_write_value_unknown_computed_or_reaching_origins_conflict
```

The prepared c20 EDI-value-provenance research task is **deferred, not cancelled** until the local workspace migration is implemented and locally validated.

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

## Final local development workspace

Authoritative specification: `docs/LOCAL_WORKSPACE.md`.

Steady-state local project root:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\
├─ MSM2-WS-v<version>.script
├─ Update-WS.cmd
├─ logs\
├─ _source\
└─ _archive\
```

One-time `Migrate-WS.cmd` is allowed during migration.

Shared Overstrike invariant:

```text
Executable:       <game>\Overstrike\Overstrike.exe
WorkingDirectory: <game>
```

There is no project-specific/AppData WS Overstrike and no separate WS Mods Library/profile. Ordinary mods, MSM2-SF, and MSM2-WS remain visible to the same shared Overstrike.

WallSprint tooling must not modify `Mods-Developing\MSM2-SF` or ordinary mods.

Runtime logs are moving from the legacy `game-root\log_wallsprint` layout to:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log
```

This path change is part of the infrastructure migration and does not authorize gameplay changes.

## Repository migration blocker already identified

The current repository tracks a root-level `WallSprint.script`. A direct clone into recursively scanned `_source` would therefore expose a second active script to Overstrike. Infrastructure implementation must remove the checkout dependency on tracked/generated live `.script` output so the final workspace can enforce exactly one active script at its root.

See `analysis/LOCAL_WORKSPACE_AUDIT.md`.

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
15. local workspace finalization ▶ CURRENT INFRASTRUCTURE TASK
16. c20 EDI value provenance ⏸ DEFERRED UNTIL INFRA VALIDATED
17. resume writer XYZ/W causal proof ⏸ after EDI contract
18. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
19. safe writable upstream movement source ⬜ NOT AUTHORIZED
20. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
21. adhesion/contact refinement ⏸ deferred
22. animation/camera/later traversal ⬜

## Current branch

```text
codex/v01c20-edi-value-provenance
```

Infrastructure changes are to be committed/pushed to this same current development branch. Do not modify `main` as part of this task.

## Current action

Run Codex on `docs/CODEX_NEXT.md` to implement the final local workspace migration/update tooling, logging destination update, repository live-script hygiene, and deterministic workspace tests. After cloud implementation, real Windows migration remains pending until the user runs the generated helper locally and confirms build/test/Overstrike/log acceptance. Then resume c20 EDI research.