# Next task — Real-PC workspace acceptance gate

## Status
The cloud implementation of the final WS local workspace migration/update/logging infrastructure is complete on branch `codex/v01c20-edi-value-provenance`.

Implementation commit: `a4ec9c0f2635e9f2594bf5972d1765332de73766`.

Do not start another Codex gameplay implementation yet. The next safe task is a real Windows acceptance test of the infrastructure that cloud fixtures cannot prove.

## Preserve these constraints
Read and preserve:
- `AGENTS.md`
- `docs/DEV_RULES.md`
- `docs/LOCAL_WORKSPACE.md`
- `docs/LOGGING_CONVENTION.md`
- `docs/PROJECT_STATE.md`

Keep exact-build/fail-closed behavior and independent implementation suitable for Nexus Mods. Do not weaken any orientation/lifecycle/ownership gate.

The accepted c19 runtime evidence remains authoritative. Keep:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

No movement, adhesion, camera, animation, physics, producer `0x351EB00`, or new gameplay write work is authorized by this task.

## Human acceptance procedure
Use the produced `MSM2-WS-v0.1c20.zip`. Extract it outside Mods Library and keep its migration helper/tools together.

For the known local installation, invoke the helper with the actual game root and current development branch:

```bat
Migrate-WS.cmd --game-root "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2" --branch codex/v01c20-edi-value-provenance
```

The reusable tooling must continue to derive/validate paths rather than depend on this personal absolute path.

## Required acceptance checks
Migration is accepted only if all of the following are true:

1. Final workspace is exactly under `Mods Library\Mods-Developing\MSM2-WS`.
2. `_source` is an independent Git clone on the expected branch/upstream and is not a worktree, junction, symlink, reparse dependency or legacy `.git` copy.
3. Exactly one active `MSM2-WS-v<version>.script` exists recursively under `MSM2-WS`.
4. `_source`, `logs` and `_archive` contain zero active `*.script` files.
5. Existing `Mods-Developing\MSM2-SF` and ordinary mods remain unchanged.
6. Shared Overstrike is only `<game>\Overstrike\Overstrike.exe`, launched with working directory `<game>`.
7. `<game>\scripts_proxy.dll` and `<game>\winmm.dll` remain the game-root loader files; no duplicate `Overstrike\winmm.dll` is introduced.
8. Shared Overstrike UI can see ordinary mods + SF + WS together.
9. `Install mods` -> `Run modded game` succeeds.
10. A fresh log is created at `Mods Library\Mods-Developing\MSM2-WS\logs\MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log`.
11. No automatic deletion or mutation of the legacy D-drive workspace occurs.

If migration fails, preserve the old environment and capture the exact helper output/error. Do not manually clean up protected sibling mods to make the test pass.

## Optional gameplay regression after infrastructure acceptance
After the workspace/shared-Overstrike/log checks pass, run one normal regression session:

```text
A surface: ~4 s
fully end WallRun
move or fast-travel
E surface: ~4 s
```

Expected safety baseline remains verified Deinit/retention, Surface Up pass, stale=0, hard kill=0, Position writes by MOD=0, and no visible regression. Supply the newest complete versioned WS log if runtime analysis is needed; video only for a visible anomaly.

## Next Codex task after this gate passes
Resume the prepared c20 EDI-value-provenance research only after real-PC workspace acceptance is confirmed and `docs/CODEX_NEXT.md` is explicitly reauthorized.

The next research question remains the value provenance of:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
```

Prepared EDI code/tests may remain preserved but production-disabled until that reauthorization. Do not treat offline fixture success as proof of actual-game EDI provenance.

## Completion rule
This gate is not complete from cloud evidence alone. It requires the user's real-PC migration/Overstrike/log result. Until then, no further gameplay implementation handoff should be generated.
