# Next task — Final local development workspace migration

## Priority / scope

This task **supersedes the previous local-package/AppData-Overstrike instructions** and temporarily takes priority over the prepared c20 EDI-value-provenance research task.

Current branch:

```text
codex/v01c20-edi-value-provenance
```

Do not discard or regress the accepted c19 runtime evidence or the prepared c20 research work. The exact EDI blocker remains pending and should resume only after this infrastructure migration is implemented and validated.

This task is infrastructure/layout/update/logging work. Do not traverse producer `0x351EB00`, implement movement/adhesion/camera/animation/traversal, or change gameplay behavior except the narrow runtime log destination/name required below.

Read first:

- `AGENTS.md`
- `docs/DEV_RULES.md`
- `docs/LOCAL_WORKSPACE.md`
- `analysis/LOCAL_WORKSPACE_AUDIT.md`
- `docs/PROJECT_STATE.md`
- `docs/LOGGING_CONVENTION.md`
- `source/BUILD.md`
- current `source/tools/build_windows.sh`, `test.sh`, `smoke_windows.sh`, `test_hooks_windows.sh`, `package.py`, and `release.py`

## Final invariant

There is exactly one shared game root, one `Mods Library`, and one shared Overstrike for ordinary mods + SocialFeed + WallSprint.

Authoritative shape:

```text
<game>\
├─ Spider-Man2.exe
├─ scripts_proxy.dll
├─ winmm.dll
├─ Overstrike\
│  └─ Overstrike.exe
└─ Mods Library\
   ├─ <existing ordinary mods...>
   └─ Mods-Developing\
      ├─ MSM2-SF\
      │  └─ <never modify from WS tooling>
      └─ MSM2-WS\
         ├─ MSM2-WS-v<version>.script
         ├─ Update-WS.cmd
         ├─ logs\
         ├─ _source\
         └─ _archive\
```

A one-time `Migrate-WS.cmd` may also exist at `MSM2-WS` root until migration is complete.

The local example game path supplied by the user is:

```text
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2
```

Reusable implementation must **not** hard-code that personal absolute path. `Update-WS.cmd`, once installed under `MSM2-WS`, must derive/canonicalize `<game>` from its own location (`MSM2-WS` -> `Mods-Developing` -> `Mods Library` -> game root) and validate `Spider-Man2.exe` before doing anything destructive.

## Naming rules

User-facing workspace names use hyphens:

```text
Mods-Developing
MSM2-WS
Update-WS.cmd
Migrate-WS.cmd
MSM2-WS-v<version>.script
MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log
```

Leading `_` is reserved for internal workspace folders:

```text
_source
_archive
```

The log directory is exactly:

```text
logs
```

Do not newly create workspace variants such as:

```text
Mods_Developing
MSM2_WS
Update_WS.cmd
logs_WS
WS_logs
log_wallsprint
```

Do not mass-rename established C/C++ source identifiers/files merely because the repository historically uses underscores. This naming rule governs the local development workspace and newly created user-facing/local-management helpers; avoid unrelated source churn.

## 1. Audit and preserve the existing build/test/release chain

Before changing scripts, inspect and document how the current repository produces the Windows DLL/script/package. Preserve the existing validation suites rather than replacing them with a weaker shortcut.

Current known helpers include:

```text
source/tools/build_windows.sh
source/tools/test.sh
source/tools/smoke_windows.sh
source/tools/test_hooks_windows.sh
source/tools/package.py
source/tools/release.py
```

Determine the supported Windows-local prerequisites/wrappers for these tools. `Update-WS.cmd` may call a repository-owned PowerShell/helper script if useful, but the user entry point remains exactly `Update-WS.cmd`. Tool discovery must fail clearly; do not silently skip build/tests when Bash/Python/CMake/compiler/toolchain prerequisites are missing.

Support spaces and Unicode paths throughout.

## 2. Make `_source` a safe independent clone

Final local checkout:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\_source
```

It must be an independent Git clone, not a worktree, junction, symlink, or reparse-point link to the old D-drive checkout.

The current repository audit found a tracked root artifact:

```text
WallSprint.script
```

This is incompatible with an Overstrike-recursively-scanned `_source` checkout. Refactor repository/release behavior so a normal checkout no longer contains any active `*.script` file. Preserve Git history; do not rewrite history. Remove/stop tracking generated live script artifacts as needed and add appropriate ignore/output discipline so future builds do not leave a live `.script` anywhere under `_source`.

Historical ZIPs need not be deleted merely for this task, but no packaging/test operation may unpack or leave an active `.script` under `_source`.

Required postcondition for the local workspace:

```text
MSM2-WS\MSM2-WS-v<version>.script     <- exactly one active script
MSM2-WS\_source\**\*.script           <- zero
MSM2-WS\logs\**\*.script              <- zero
MSM2-WS\_archive\**\*.script          <- zero
```

## 3. Implement `Migrate-WS.cmd`

Add a source-controlled migration/bootstrap implementation that produces the top-level local `Migrate-WS.cmd` when used on Windows.

The one-time migration must be safe and fail closed. It must:

- resolve/accept the intended MSM2 game root without embedding the user's D-drive or C-drive personal path in reusable code;
- validate the game root and exact `Mods Library\Mods-Developing\MSM2-WS` destination;
- create only the required WS workspace folders (`logs`, `_source`, `_archive`) and top-level WS helpers;
- create `_source` as a fresh independent clone from the WallSprint GitHub remote/current development branch, or validate an already-correct independent clone;
- reject worktree/junction/symlink/reparse-point dependence on the legacy checkout;
- never copy the legacy `.git` directory as a substitute for a clone;
- never delete the old D-drive environment;
- never alter `Mods-Developing\MSM2-SF` or ordinary mods;
- run the same workspace preflight/build/test/package/deploy checks required by `Update-WS.cmd` before declaring migration successful;
- only after all local checks pass, print that the old WallSprint environment may be manually deleted.

If initial bootstrap cannot infer `<game>` because the migration script is not yet placed in the target workspace, accept a validated explicit game-root argument/environment value or prompt for it. Once installed, normal `Update-WS.cmd` must derive `<game>` from its own location with no personal absolute path.

Automatic deletion of `D:\ajita\制作\自主\MSM2_Mods` or any legacy path is forbidden.

## 4. Implement `Update-WS.cmd`

The regular user entry point must be:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\Update-WS.cmd
```

Double-click flow, fail closed:

1. stop `Spider-Man2.exe` and the shared Overstrike process if running;
2. canonicalize workspace/game paths from `%~dp0` and verify this is the exact `Mods Library\Mods-Developing\MSM2-WS` shape;
3. verify `_source` is a valid independent non-reparse-point Git checkout;
4. reject detached HEAD; verify current development branch/upstream and use safe `git pull --ff-only` semantics (do not hard-code a branch that will make future task branches impossible to use; allow an explicit expected-branch contract if needed);
5. build with the existing supported WallSprint Windows workflow;
6. run required existing tests plus new workspace tests;
7. run package validation;
8. obtain the generated WallSprint script without leaving a live `.script` inside `_source`;
9. archive/disable only old `MSM2-WS-*` active scripts as `.script.disabled` under `_archive`;
10. deploy exactly one active `MSM2-WS-v<version>.script` at workspace root; version must come from the authoritative build/version metadata, not a commit hash/GUID/random suffix;
11. recursively assert exactly one active WS `.script` beneath `MSM2-WS` and zero under `_source`, `logs`, `_archive`;
12. verify `<game>\Overstrike\Overstrike.exe` exists;
13. verify `<game>\Overstrike\winmm.dll` does **not** exist;
14. verify `<game>\scripts_proxy.dll` exists;
15. launch only `<game>\Overstrike\Overstrike.exe` with WorkingDirectory=`<game>`;
16. print a concise instruction: select ordinary mods + SF + WS as desired, then `Install mods` -> `Run modded game`.

Do not create/copy/regenerate a project-specific Overstrike. Do not create AppData Overstrike copies. Do not create `MSM2-WS\_tools\Overstrike`.

## 5. Shared Overstrike isolation

WallSprint tooling may read/validate shared infrastructure but must not treat it as WS-owned cleanup state.

Allowed shared paths:

```text
<game>
<game>\Mods Library
<game>\Mods Library\Mods-Developing
<game>\Overstrike\Overstrike.exe
```

Forbidden WS side effects:

- deleting/moving/renaming/overwriting/backing up anything under `Mods-Developing\MSM2-SF`;
- deleting/moving/renaming/overwriting/backing up ordinary mods;
- copying `winmm.dll` or `scripts_proxy.dll` into `Overstrike`;
- maintaining a separate WS Mods Library/profile;
- modifying shared Overstrike binaries as part of normal WS update.

Where practical, migration/update tests should snapshot names/metadata for sibling SF/ordinary-mod fixtures before and after operations and assert they remain untouched.

## 6. Change runtime log destination/name

Replace the old game-root `log_wallsprint` destination with:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs
```

Runtime filename:

```text
MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log
```

Keep the existing local-time filename behavior, atomic collision handling, bounded suffixing, diagnostics on failure, and no silent fallback. Include the authoritative build version; do not include commit hashes/GUID/random IDs.

Update `docs/LOGGING_CONVENTION.md`, runtime logger code, and logger smoke/integration tests accordingly.

## 7. Required workspace tests

Add deterministic tests for at least:

- exact `Mods-Developing`, never create `Mods_Developing`;
- exact `MSM2-WS`, `Update-WS.cmd`, `logs`, `_source`, `_archive`;
- never create `logs_WS`, `WS_logs`, `MSM2_WS`, `Update_WS.cmd`;
- paths with spaces and Unicode;
- no D-drive/legacy absolute dependency in generated local helpers/config;
- `_source` is independent and not a worktree/junction/symlink/reparse-point dependency;
- exactly one active WS `.script` recursively after deployment;
- no active `.script` in `_source`, `logs`, or `_archive`;
- old WS versions archived as `.script.disabled` only;
- no SF mutations;
- no ordinary-mod mutations;
- exact shared Overstrike path `<game>\Overstrike\Overstrike.exe`;
- no `<game>\Overstrike\winmm.dll`;
- Overstrike WorkingDirectory=`<game>`;
- game-root `scripts_proxy.dll` preflight;
- runtime log destination/name;
- failed preflight/build/test/package leaves the previous active WS build intact when possible and does not partially deploy a new one;
- all existing WallSprint regression suites remain PASS.

Use owned temporary filesystem fixtures for destructive/rename/archive tests. Never run cleanup tests against the user's real Mods Library in cloud/offline test suites.

## 8. Documentation / repository hygiene

Update as part of implementation:

```text
AGENTS.md
docs/DEV_RULES.md
docs/LOCAL_WORKSPACE.md
docs/LOGGING_CONVENTION.md
docs/PROJECT_STATE.md
README_TEST.md
source/BUILD.md
docs/CODEX_RESULT.md
```

Add the reusable Windows migration/update helpers and their tests in an appropriate source-controlled location. The installed top-level names remain exactly `Migrate-WS.cmd` and `Update-WS.cmd`.

Do not package raw user logs or private game bodies.

## 9. Preserve WallSprint research state

Accepted c19 actual result remains:

```text
A=3844 ms, E=4094 ms
verified Deinit / retention PASS
Surface Up PASS
stale=0 / hard kill=0
Position writes by MOD=0
8 true entries / 8 complete maps each
XYZ match mask=0x7
```

Current research blocker remains:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
EDI value provenance unresolved
```

Keep:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Do not consume this infrastructure task as authorization to advance EDI/producer/movement research. After migration implementation, state that c20 EDI work remains the next gameplay-research task.

## Cloud vs real-PC validation

Cloud Codex cannot mutate the user's `C:` or old `D:` environment. Implement and test the migration/update logic with owned fixtures and produce the exact local steps/artifacts needed.

Do **not** claim the real migration is complete until the user runs the helper locally and reports the Windows results. In particular, shared Overstrike UI recognition of ordinary mods + SF + WS is a real-PC acceptance check.

## Completion / persistence

Before reporting implementation complete:

1. run all applicable build/test/workspace fixture suites;
2. update `docs/CODEX_RESULT.md` with exact results and any real-PC checks still pending;
3. commit every intended change to the **current branch `codex/v01c20-edi-value-provenance`**;
4. push/publish that branch to GitHub;
5. report the final branch head SHA and the exact first local migration command/action for the user.

Do not force-push, rewrite history, delete the old D-drive workspace, modify `main`, or modify MSM2-SF.