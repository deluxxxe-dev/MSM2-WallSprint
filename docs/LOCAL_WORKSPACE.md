# WallSprint local development workspace

This document is the authoritative local Windows layout for WallSprint (`MSM2-WS`). It supersedes earlier project-specific/AppData Overstrike and `log_wallsprint` layouts.

## Core invariant

Spider-Man 2 uses exactly one shared game root, one `Mods Library`, and one shared Overstrike instance for ordinary mods, SocialFeed (`MSM2-SF`), and WallSprint (`MSM2-WS`).

Reusable scripts must not hard-code a personal absolute path. They derive the game root from the WallSprint workspace location.

## Final layout

```text
<game>\
├─ Spider-Man2.exe
├─ scripts_proxy.dll
├─ winmm.dll
├─ Overstrike\
│  └─ Overstrike.exe
└─ Mods Library\
   ├─ <ordinary existing mods...>
   └─ Mods-Developing\
      ├─ MSM2-SF\
      │  └─ <SocialFeed-owned files; WallSprint must never modify them>
      └─ MSM2-WS\
         ├─ MSM2-WS-v<version>.script
         ├─ Update-WS.cmd
         ├─ logs\
         ├─ _source\
         └─ _archive\
```

A one-time `Migrate-WS.cmd` may also exist at the `MSM2-WS` top level until migration is complete.

For a script located at `<game>\Mods Library\Mods-Developing\MSM2-WS`, the game root is three parent directories above the workspace. Resolve/canonicalize this relation at runtime; do not embed the user's `C:` path in reusable code.

## Naming convention

Use hyphens for user-visible/project names:

```text
Mods-Developing
MSM2-WS
Update-WS.cmd
Migrate-WS.cmd
MSM2-WS-v0.1c20.script
MSM2-WS-v0.1c20-20261024-120500.log
```

Leading underscore is reserved for internal folders only:

```text
_source
_archive
```

Do not create underscore-separated alternatives such as:

```text
Mods_Developing
MSM2_WS
Update_WS.cmd
logs_WS
WS_logs
```

The runtime log directory is simply `logs` because the parent directory already identifies the project.

## Shared Overstrike

The only supported Overstrike executable for this local development workflow is:

```text
<game>\Overstrike\Overstrike.exe
```

Launch it with:

```text
Executable:       <game>\Overstrike\Overstrike.exe
WorkingDirectory: <game>
```

Do not create project-specific Overstrike copies, AppData copies, or `MSM2-WS\_tools\Overstrike`.

`<game>\Overstrike\winmm.dll` must not exist. The game-wide `winmm.dll` and `scripts_proxy.dll` remain in the game root and must not be duplicated into the Overstrike directory.

The shared Overstrike must continue to see ordinary mods, MSM2-SF, and MSM2-WS in the same UI so the user can perform one `Install mods` operation and then `Run modded game`.

## `_source`

`MSM2-WS\_source` is the authoritative local Git checkout.

It must be an independent clone of the WallSprint repository, not a worktree, junction, symlink, or proxy to the old D-drive checkout. Normal development/update/build/test operations must not depend on `D:\ajita\制作\自主\MSM2_Mods` or any other old checkout.

Because Overstrike recursively scans `Mods Library`, no active `*.script` may exist anywhere inside `_source`. The repository itself must therefore not require a tracked/generated live `.script` in its checkout. Build/package intermediates must use a non-`.script` suffix or a temporary location outside `Mods Library`; only final deployment places the live `.script` in the workspace root.

## Active script and archive

Exactly one active WallSprint script may exist recursively beneath `MSM2-WS`:

```text
MSM2-WS\MSM2-WS-v<version>.script
```

Do not put commit hashes, GUIDs, random hex, or task-description suffixes in the user-facing filename.

Older versions may be retained only with a non-active extension, for example:

```text
_archive\MSM2-WS-v0.1c19.script.disabled
```

No file below `_source`, `logs`, or `_archive` may end in `.script` after a successful update.

## Logs

Runtime logs are written only to:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs
```

Filename:

```text
MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log
```

Same-second collision suffixes may be added without introducing hashes. Do not create `log_wallsprint`, `logs_WS`, or shared project log directories as the new destination.

## `Update-WS.cmd`

Normal user operation is a double-click on `MSM2-WS\Update-WS.cmd`.

It must fail closed and perform, at minimum:

1. stop Spider-Man 2 / shared Overstrike if running;
2. verify `_source` is an independent valid Git checkout and not a reparse-point dependency on the legacy workspace;
3. verify a non-detached intended development branch with an upstream and use a safe fast-forward pull policy;
4. update from GitHub;
5. build using the repository-supported Windows toolchain/workflow;
6. run the existing required WallSprint tests plus workspace-layout tests;
7. validate the generated package/script;
8. deploy exactly one `MSM2-WS-v<version>.script` to the workspace root;
9. archive/disable only old `MSM2-WS-*` scripts;
10. recursively verify exactly one active WS `.script` and none under `_source`, `logs`, or `_archive`;
11. verify `<game>\Overstrike\Overstrike.exe`;
12. verify `<game>\Overstrike\winmm.dll` is absent;
13. verify `<game>\scripts_proxy.dll` exists;
14. launch shared Overstrike with WorkingDirectory=`<game>`;
15. instruct the user to use `Install mods` then `Run modded game`.

All path handling must support spaces and Unicode. The updater must never clean, rename, move, back up, or overwrite `MSM2-SF` or ordinary mods.

## One-time migration

If required, `Migrate-WS.cmd` creates/validates the final workspace and independent `_source` clone without deleting the old workspace.

It may report that the old WallSprint environment can be manually deleted only after all of these pass:

- exact `Mods-Developing` / `MSM2-WS` layout;
- no forbidden underscore-name alternatives created;
- independent `_source` Git clone;
- no legacy D-drive dependency, worktree, junction, or symlink;
- `Update-WS.cmd` self-contained from the new workspace;
- Windows build success;
- existing automated tests success;
- package validation success;
- exactly one active WS `.script`;
- new `logs` path active;
- shared Overstrike path/WorkingDirectory checks pass;
- MSM2-SF unchanged;
- ordinary mods unchanged.

Automatic deletion of the legacy D-drive environment is forbidden.

## Scope

This workspace migration is infrastructure-only. Preserve current WallSprint runtime behavior, reverse-engineering evidence, exact-build safety gates, and current c19/c20 research state unless a change is strictly required to implement the logging/deployment path.