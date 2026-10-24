# WallSprint Codex instructions

## Repository and development baseline

- Canonical remote: `https://github.com/deluxxxe-dev/MSM2-WallSprint.git`.
- Current implementation source of truth: `codex/v01c20-edi-value-provenance`.
- The owner confirmed the pushed/local baseline as `5192f05f32313919fdf4049bd734b971655d163f` on 2026-10-24. Continue from this commit and subsequent commits on that branch; do not reset later work to the baseline.
- `main` has diverged substantially. Do not merge `main` into the current development branch. Its independent documentation/analysis changes do not supersede this branch's implementation or task instructions.

Before starting any implementation task, read these files in order:

1. `docs/DEV_RULES.md`
2. `docs/LOCAL_WORKSPACE.md`
3. `docs/PROJECT_STATE.md`
4. `docs/CODEX_NEXT.md`
5. Relevant files under `analysis/`

`docs/CODEX_NEXT.md` is the current highest-priority task. Do not continue to a later milestone unless it explicitly authorizes that step.

## Working rules

- Preserve exact-build guards and fail-closed behavior.
- Do not invent native function signatures or game ABI types without evidence.
- Prefer runtime evidence, exact RVA/AOB verification, and bounded diagnostics over blind patches.
- Do not add adhesion, movement, animation, camera, parkour, swing, or combat work unless `docs/CODEX_NEXT.md` explicitly requests it.
- Keep reference MODs as research hints only. Do not copy their code, binaries, assets, or proprietary data into production source.
- Keep WallSprint independently implemented and suitable for eventual Nexus Mods distribution.
- Do not add new vendored copies of `SM2ScriptTemplate` files. New helpers should live in WallSprint-owned code such as `GameInterop`, `NativeHooks`, or math utilities.
- Do not package raw user logs, full captured game functions, or unnecessary game binary fragments in release ZIPs.
- Treat each cloud task as isolated. Never assume an uncommitted working tree from a previous task still exists.

## Final local workspace invariant

The authoritative Windows development/test layout is `docs/LOCAL_WORKSPACE.md`.

Steady-state WallSprint root:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\
├─ MSM2-WS-v<version>.script
├─ Update-WS.cmd
├─ logs\
├─ _source\
└─ _archive\
```

A one-time `Migrate-WS.cmd` may exist during migration.

Important invariants:

- `_source` is the independent authoritative Git clone; no worktree/junction/symlink/reparse-point dependency on the old D-drive checkout.
- Exactly one active WallSprint `.script` may exist recursively beneath `MSM2-WS`, at workspace root only.
- No active `.script` may remain in `_source`, `logs`, or `_archive`.
- Overstrike also recursively scans ZIP/RAR/7z contents. `_source` uses an independent Git clone with the managed sparse checkout excluding archive extensions while retaining all source code and archive blobs/history in Git. Never disable that policy inside Mods Library.
- No visible `.zip`, `.rar` or `.7z` may remain anywhere under `MSM2-WS`. Store build ZIPs only as `.zip.disabled`; the updater may disable only positively identified legacy WS build outputs. Public ZIP downloads stay outside Mods Library.
- Shared Overstrike is only `<game>\Overstrike\Overstrike.exe`, launched with WorkingDirectory=`<game>`.
- Do not create WS-specific/AppData Overstrike copies or a separate WS Mods Library/profile.
- Never mutate `Mods-Developing\MSM2-SF` or ordinary existing mods from WallSprint tooling.
- Explicit user-authorized shared `environment/` Mod-Tools are a separate owner-operated utility: the current request authorizes whole-environment snapshots/restores including SF and ordinary mods. This exception applies only to an explicitly invoked shared backup/restore, never to WS updater cleanup, SF development, or publication of private backups. Install only the toolkit/shortcuts; restore only the documented mod roots with verified staging and preserved previous data.
- Reusable scripts must not hard-code the user's personal C-/D-drive paths and must support spaces/Unicode.
- The old D-drive WallSprint environment must never be automatically deleted.

The current infrastructure task temporarily takes priority over the prepared c20 EDI research. Preserve that research state; do not advance it unless `docs/CODEX_NEXT.md` explicitly re-authorizes it.

## Runtime-log handoff

The user normally sends game runtime logs to ChatGPT, not directly to Codex.

ChatGPT will analyze those logs and write implementation-relevant evidence into repository analysis files. `docs/CODEX_NEXT.md` will reference the latest relevant runtime-result file.

When starting a task:

1. read the runtime-result files referenced by `docs/CODEX_NEXT.md`;
2. treat their measured RVAs, timings, register values, admission failures, A/E comparisons and reproduced behaviors as the runtime evidence supplied for that task;
3. preserve the distinction between measured facts, static-analysis conclusions and hypotheses;
4. do not require the full raw `.log` file unless the task explicitly says a missing detail cannot be resolved from the extracted evidence;
5. if more raw evidence is required, state the exact event, field, timestamp window or log key needed instead of asking for an undirected full re-analysis.

Do not commit full raw runtime logs to GitHub by default. Keep the repository clean for eventual public/Nexus distribution. Compact, task-specific runtime excerpts and derived analysis belong under `analysis/`.

Current local runtime logs, after the workspace migration is implemented, belong under:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs
```

with the versioned timestamp naming documented in `docs/LOGGING_CONVENTION.md`.

## Persistence is mandatory

A task is not complete merely because a ZIP/artifact was generated.

Before reporting completion:

1. ensure all intended source, analysis, test and documentation changes are present in the repository working tree;
2. update `docs/CODEX_RESULT.md`;
3. commit the implementation changes to the task's Git branch;
4. push/publish that branch to GitHub when the environment supports it;
5. if direct push is unavailable, clearly report the exact branch/commit/diff that the user must preserve and do not claim the implementation is safely handed off.

Prefer the existing dedicated task branch over leaving changes only in an ephemeral cloud checkout. Do not force-push or rewrite `main` history.

The next isolated Codex task must be able to reproduce the current implementation by checking out GitHub alone.

## After completing a task

Update `docs/CODEX_RESULT.md` with:

- what changed;
- files changed;
- build/test results;
- safety/admission gates;
- what still requires real in-game/Windows verification;
- exact package/output name;
- branch name and commit SHA containing the implementation;
- next recommended human action/test.

Do not overwrite `docs/PROJECT_STATE.md` or `docs/CODEX_NEXT.md` unless the task explicitly asks you to. The current workspace migration task explicitly authorizes the documentation updates listed in `docs/CODEX_NEXT.md`.

Implementation note: current Windows entry points are `Migrate-WS.cmd` / `Update-WS.cmd`; the reviewed engine is `source/tools/workspace.py`. Build intermediates use `.script.disabled` and only the install ZIP virtualizes its versioned active script member. Prepared EDI research commit `9057d24` remains production-disabled until separately authorized.

## Shared Mod-Tools manual

For shared launch/backup/recovery changes, read `docs/mod-tools/README.md` and the
relevant `OVERSTRIKE.md`, `PLAY_MODDED.md` or `BACKUP_MODS.md` in that directory.
They map the three shortcuts to entrypoints, settings, functions, safety limits
and regression checks. `docs/mod-tools/MANUAL.md` combines all four for browsing.
Update the individual specification and combined manual together when behavior
changes. Keep runtime evidence distinct from authored specifications; a manual
does not authorize gameplay/SF changes or claim an unperformed real-PC test.
