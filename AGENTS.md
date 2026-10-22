# WallSprint Codex instructions

Before starting any implementation task, read these files in order:

1. `docs/DEV_RULES.md`
2. `docs/PROJECT_STATE.md`
3. `docs/CODEX_NEXT.md`
4. Relevant files under `analysis/`

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

## Persistence is mandatory

A task is not complete merely because a ZIP/artifact was generated.

Before reporting completion:

1. ensure all intended source, analysis, test and documentation changes are present in the repository working tree;
2. update `docs/CODEX_RESULT.md`;
3. commit the implementation changes to the task's Git branch;
4. push/publish that branch to GitHub when the environment supports it;
5. if direct push is unavailable, clearly report the exact branch/commit/diff that the user must preserve and do not claim the implementation is safely handed off.

Prefer a dedicated task branch such as `codex/v01b8-dual-source` over leaving changes only in an ephemeral cloud checkout. Do not force-push or rewrite `main` history.

The next isolated Codex task must be able to reproduce the current implementation by checking out GitHub alone.

## After completing a task

Update `docs/CODEX_RESULT.md` with:

- what changed;
- files changed;
- build/test results;
- safety/admission gates;
- what still requires real in-game verification;
- exact package/output name;
- branch name and commit SHA containing the implementation;
- next recommended human test.

Do not overwrite `docs/PROJECT_STATE.md` or `docs/CODEX_NEXT.md` unless the task explicitly asks you to.