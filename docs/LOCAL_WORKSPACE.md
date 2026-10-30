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

`<game>\Overstrike\winmm.dll` must not exist. Keep game-root `winmm.dll` and `scripts_proxy.dll` intact. A regular non-reparse `Overstrike/scripts_proxy.dll` is permitted as an installer source; only misplaced `Overstrike/winmm.dll` remains forbidden.

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

No file below `_source`, `logs`, or `_archive` may end in `.script` after a successful update. ZIP/RAR/7z contents are also recursively scanned by Overstrike: no visible archive with those extensions may remain anywhere under WS. Returned install/source archives use `.zip.disabled`. This hides the entire container from extension-based scanning while preserving its bytes.

The independent `_source` clone has a managed Git sparse checkout: every code/documentation file is included, while ZIP/RAR/7z paths are excluded from the working tree, including nested and mixed-case extensions. The tracked archives and commit history remain in the independent Git object store and on GitHub. This is a regular clone with its own `.git`, not a linked Git worktree or junction. Fast-forward pulls retain this policy. Custom sparse patterns and untracked archives refuse rather than being silently changed/deleted.

During upgrade, clean tracked source archives are excluded by Git. Only validated legacy `MSM2-WS-v<version>[‑source].zip` outputs directly in `_archive/_build` may be moved to collision-safe `.disabled` names under `_archive`, preserving bytes and timestamps. Unrelated files, shared Overstrike cache/profiles, SF and ordinary mods are not cleanup targets.

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

## Implemented entry points and tool prerequisites

The release's complete bootstrap is `Migrate-WS.cmd` plus `tools/workspace.py` and its process-stop helper. Extract it outside Mods Library, then run:

```bat
Migrate-WS.cmd --game-root "<actual game root>" --branch codex/v01c20-edi-value-provenance
```

Windows Python3.11+, Git for Windows, PowerShell and a configured WSL2 cross-build/test toolchain are required. The optional `MSM2_WS_WSL_DISTRO` selects a configured distro. No missing compiler/test tool is silently skipped. Full commands and tool setup are in `source/BUILD.md`.

`Update-WS.cmd` loads only its installed `_source` engine. Migration clones with no local/shared-object optimization and stores the explicit expected branch contract. Both update and migration reject dirty/detached/diverged/linked checkouts. An intentional future branch switch needs `--expected-branch` plus a matching upstream. Build/tests/package run in a temporary WSL source copy outside Mods Library; `_source` remains clean and contains no generated active script. The current script is not archived until package validation succeeds; a failed deployment/launch attempts to restore the exact prior active build and helper.

Shared process stopping is limited to the validated game executable and shared Overstrike path; a different running Overstrike must be closed manually. The updater neither installs nor repairs shared Overstrike/DLLs. Disabled build artifacts and archives may be retained under `_archive/_build`, using `.script.disabled` and `.zip.disabled` respectively. A normal ZIP is not safe there: Overstrike opens it and discovers nested scripts. A release invoked directly anywhere inside Mods Library refuses before producing ZIPs; use the mandatory external staging pipeline.

Cloud tests use owned Git/filesystem and Windows logger fixtures. Real Windows process/WSL/toolchain behavior, shared Overstrike UI recognition and actual new runtime logs must be confirmed locally before considering manual removal of the old environment.


## Optional user-operated shared Mod-Tools

The user separately requested one-click modded launch and a complete private
backup/restore of the shared environment, including ordinary mods, SF/WS sources,
Git history and logs. The independent toolkit lives at `<game>/Mod-Tools`, outside
both project subtrees and Mods Library. See environment/README.md and
`docs/MOD_ENVIRONMENT_BACKUP.md`. Its installer modifies only that toolkit and
its own desktop shortcuts. Normal SF/WS update/build behavior remains unchanged.

Explicit shared snapshots read the documented Overstrike/Profiles/Mods Library/
scripts/toolkit roots and loader/settings files. They are stored outside the game
and never published by the tool. Explicit restore verifies bytes and game build,
stages before replacement, preserves old roots outside the scanned library and
rolls back replacement failures. This is the user's authorized shared recovery
operation, not WS cleanup or SF development. Same-path recovery preserves opaque
absolute paths. Game data, external WSL/toolchains and saves are not in the archive.
After Vanilla restoration, complete Install mods before direct modded launch.


## Retired WS Steering Probes — 2026-10-31

Normal Update-WS deploys only the current WS script. Direct `.stage` files are
scanner inputs too; ZIP/RAR/7z can expose nested stages. Managed sparse policy now
excludes stages as well as archives (old managed policy upgrades; custom patterns
still refuse). Builders/research and all Git blobs/history remain available.
The build checks for unexpected stages and distribution excludes asset/archive
files from source copies; contaminated stale package output refuses.

Only exact `MSM2-WS-SteeringProbe-TurnSpeedFast10` / `TurnSpeedSlow5` `.stage` or
single-stage `.zip` artifacts inside MSM2-WS are eligible for retirement. Names
alone are insufficient: exact metadata/member set and544byte asset restored to
its pinned vanilla SHA must match. Untracked owned probes are retired before
sparse preparation; clean tracked stages are hidden by Git, never removed from
history. Dirty tracked work, unknown stages/archives and reparse points refuse.

Retirement moves verified artifacts into a unique
`<game>/MSM2-WS-Retired-Probes/retired-*/` directory OUTSIDE Mods Library, with
source-relative paths and SHA256 in manifest.json. Existing backups never
overwrite. Restore only manually with manager/game closed and original path
absent; moving them back intentionally makes them discoverable again. Profiles,
selections/order, SF and other mods are not edited. Never put this backup inside
Mods Library, including under _archive. A `.zip.disabled` is ignored by the
reviewed extension dispatch; an active `.stage` beside it is still scanned.

Copies elsewhere in Mods Library are outside automatic scope. Use the entry's
Overstrike tooltip path (`archive||member` identifies the actual container),
close Overstrike and move only the verified obsolete probe file/container outside
Mods Library. A mixed container containing unrelated mods must NOT be moved as
probe cleanup; inspect/report its exact path first. Actual owner duplicate paths
are unknown; no cloud claim that both installed entries were retired.

If an old engine refuses before fetch, close game/Overstrike and use the guarded
Git refresh from PowerShell (this only updates the existing source checkout):

```powershell
$ws = Join-Path "<actual game root>" "Mods Library\Mods-Developing\MSM2-WS"
$src = Join-Path $ws "_source"
$branch = "codex/v01c20-edi-value-provenance"
if ((git -C $src branch --show-current) -ne $branch) { throw "Wrong branch" }
if ((git -C $src remote get-url origin) -ne "https://github.com/deluxxxe-dev/MSM2-WallSprint.git") { throw "Wrong origin" }
$dirty = git -C $src status --porcelain --untracked-files=no
if ($LASTEXITCODE -ne 0 -or $dirty) { throw "Preserve tracked changes first" }
git -C $src fetch origin $branch
if ($LASTEXITCODE -ne 0) { throw "Fetch failed" }
git -C $src merge --ff-only "origin/$branch"
if ($LASTEXITCODE -ne 0) { throw "Fast-forward failed; preserve checkout" }
& (Join-Path $ws "Update-WS.cmd")
```

Untracked/unknown files are not deleted by the refresh. The current updater still
checks them before preparation/build; this is not a validation bypass. No install,
loader repair or real-PC acceptance is performed by this cloud task.


## v1.2 OS所有権と互換lock

`environment/operation_lock.py` が正本です。`.MSM2-Operations.guard` をWindowsの
CreateFile共有排他で保持し、閉じる/プロセス終了でOSが解放します。ビルド子へ明示的に
ハンドルを継承し、PIDと開始identity、token、workspace、phase、WSL distroを監査記録に保持。
`.MSM2-Mod-Tools.lock` / `Update-WS.lock` は旧版を止める互換記録として残り得ます。
**ファイルを削除する必要はありません。存在はactiveの証拠ではありません。**

OS busyなら実所有が残っています。PID/start/action/phaseを確認し、子ビルドが保持して
いる可能性も示します。旧empty/PID lockは年齢だけで回収せず、Windows operation processと
全running WSL distroのbuild停止を確認できた場合だけ `.MSM2-lock-recovery/<token>` に
元bytesを保存して移行。PID再利用は開始identityで区別。legacy PID-onlyで現PIDが生きて
いる場合、malformed/reparse/所有者変更/WSL検証不能は保持・具体的理由で拒否します。
`index.lock` は自動削除しません。プロセスの自動kill、DLLの移動/削除/属性修正もしません。

新WSLビルドはdistroとtokenを固定。親の異常終了後、Windows子のOSハンドルが残れば
次操作は取得できません。WSLホストまで終了してLinux子が残った場合も、未解決build
記録を保持し、同tokenのプロセスが消えた証拠が得られるまで再開しません。プロセス環境を
読めない場合は成功扱いにしません。手動の外部Git/ビルドは同時実行しないでください。

Backup/Restoreも同じゲートです。互換lockはsnapshot対象外。復元時は現在のKITをstageに
保持し、古いbackupから旧ロック方式を再導入しません。mods/profile/復元確認設定の既存
保全・rollbackは維持。KIT旧bytesはbackup/退避先に残り、全体cleanupはしません。
実機で健全な環境を消す復元試験は要求しません。

修復の一括実行例: docs/mod-tools/README.md「一度だけのtooling修復」。


### r5 post-build WSL確認の切り分け

ビルド/テスト/package完了の行は配置完了ではありません。原因別PID/stage/category/
errno/exitとdistro/UIDの診断はdocs/mod-tools/README.mdの一括ブロックから実行できます。
古いcheckerを通る前に最新コードをstageするDiagnoseWSLモードです。lock/KIT/runtime/
worktreeは変更せず、process/WSLを終了しません。bootstrap fetchのみGit refs/objects更新。
実機PID339の原因は未確定。retryはフルビルド、既存stagingの手動配置はしません。
