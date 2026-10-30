# MSM2 Mod-Tools 総合マニュアル・仕様書

対象v1.2.0 / 更新2026-10-31 JST。個別原本と同時更新。

<a id="common"></a>

# MSM2 Mod-Tools 仕様書ライブラリ

対象は現在の **Mod-Tools v1.2.0** が作成する3つのショートカットです。
作成日: 2026-10-24（日本時間）。実装基準:
現行開発ブランチ `codex/v01c20-edi-value-provenance`（公開SHAはCODEX_RESULT参照）。
今回のtooling変更はロック・KIT更新のみです。WS r4/SFのruntime/package bytesは変更しません。

| ショートカット | 目的 | 仕様書 |
|---|---|---|
| MSM2 - Overstrike | 配置確認・初回セットアップ後に共有Overstrikeを開く | [OVERSTRIKE.md](OVERSTRIKE.md) |
| MSM2 - Play Modded | インストール済みMODでゲームを直接起動する | [PLAY_MODDED.md](PLAY_MODDED.md) |
| MSM2 - Backup Mods | 共有MOD環境を保存し、完成前に全ファイルを検証する | [BACKUP_MODS.md](BACKUP_MODS.md) |

日常操作は [利用ガイド](../../environment/README.md)、既存の検証記録は
[TEST_REPORT.md](../../environment/TEST_REPORT.md) を参照してください。
各仕様書の「改変箇所」と「確認項目」を、次回の変更依頼・実装レビューの出発点にします。
GitHubでまとめて読む場合は、全仕様を含む [一冊版 MANUAL.md](MANUAL.md) を使用できます。
一冊版はこの索引/共通仕様と3つの仕様書を統合したスナップショットです。改訂時は両方を更新します。

## いつ何を使うか

| やりたいこと | 操作 |
|---|---|
| 現在のMOD構成で遊ぶ | `MSM2 - Play Modded` |
| MODを追加/切替/再適用する | `MSM2 - Overstrike` → 選択確認 → Install mods → Run modded game |
| WS本体をソースから更新する | 既存の `MSM2-WS/Update-WS.cmd`。この3つのショートカットは更新処理を兼ねない |
| SF本体を更新する | SF側の既存 `Update-SF.cmd`。開発はSFの作業系で進める |
| 更新前や健全な状態を保存する | ゲーム/Overstrike/更新処理を閉じ、`MSM2 - Backup Mods` |
| 保存したデータの破損を確認する | 保存フォルダーの `Verify-Backup.cmd` |
| 同じ場所のVanillaへ環境を戻す | 保存フォルダーの `Restore-Mods.cmd` → Install mods → Playの確認 |

## 導入・ツール更新

現在の独立Git cloneを、意図した開発ブランチ上でfast-forward更新します。
その後 `_source/environment/Start-Overstrike.cmd` をダブルクリックします。
初回/ツール更新時はゲームとOverstrikeを閉じてください。
ZIP版を使う場合はMods Library外へ一式を展開して同じCMDを実行し、ゲームルートを入力します。
Python等が導入済みなら、共有Overstrikeやローダーを再インストールする手順はありません。

ツール一式が `<game>/Mod-Tools` に保存され、3つのショートカットができます。
既存のMOD・SF/WS・ログの構造は維持されます。起動処理はGitHubの新版を自動取得しません。
ダウンロードURLが404でも既存Git cloneから取得できます。認証/アクセス権も確認し、
リンク表示だけを配布成功の証拠にしないでください。Gitの更新失敗時は古い内容を破棄せず出力を保存します。

## 共通の配置と依存

`<game>` は `Spider-Man2.exe` が存在するゲームルートです。

```text
<game>/
  Spider-Man2.exe
  scripts_proxy.dll
  winmm.dll
  Overstrike/Overstrike.exe
  Profiles/
  scripts/
  scripts.txt
  commandline.txt                 # 存在する場合
  Mod-Tools/
    Start-Overstrike.cmd
    Play-Modded.cmd
    Backup-Mods.cmd
    Install-Mod-Tools.cmd
    Open-Overstrike.cmd
    Restore-Mods.cmd
    Verify-Backup.cmd
    mod_environment.py
    windows.ps1
    mod-tools.json
  Mods Library/Mods-Developing/
    MSM2-WS/{_source,logs,_archive,...}
    MSM2-SF/{_source,logs,...}
```

Windows、Python 3.11以降、Windows標準PowerShellを使用します。
通常起動・保存・復元にはGit更新、WSL、コンパイラー、ネット接続を使用しません。
WS/SFの開発更新はそれぞれの既存アップデーターで行います。

## ショートカットと実ファイルの違い

`.lnk` のリンク先は `<game>/Mod-Tools/<対応するCMD>`、作業フォルダーは `<game>`。
Overstrike/Backupのアイコンは共有Overstrike、PlayはゲームEXEを使用します。
**`.lnk` だけならデスクトップからゲームルートなどへ移動しても動作します。**
`Mod-Tools` 本体やゲームルートの移設は別です。リンク先の絶対パスや設定が変わるので、
ショートカットの移動と同一扱いにしないでください。CMDだけの単独移動も不可です。
CMDは隣にある `mod_environment.py` を起動し、同じツール一式を必要とします。

導入処理はWindowsのDesktop既定フォルダーへ3つの `.lnk` を作ります。
移動済みショートカットは探索せず、再導入時にデスクトップへ同名のものを再作成できます。
既存の同名 `.lnk` が別アプリを指す場合は保存前に停止します。移動した `.lnk` の
内容は自動で更新されません。同じリンク先を維持する限り、その場所で引き続き使えます。

## 共通の実行経路

```text
.lnk -> <game>/Mod-Tools/*.cmd
     -> py -3（なければpython）mod_environment.py <action>
     -> Pythonの検査・設定・ファイル処理
     -> windows.ps1（必要な場合だけ）
```

CMDは遅延展開を無効化し、エンジンを `%~dp0` から絶対パスで呼び出します。
作業場所を `%TEMP%` に変え、引数を引き継ぎます。起動時の元のカレントディレクトリに
依存しません。Pythonの終了コードを返し、エラー時は画面を閉じず `pause` します。
Backup/Restore/VerifyのCMDは成功時にも `pause` します。

PythonからPowerShellへは一時JSONでaction/gameを渡し、`powershell.exe` の
`-NoProfile -NonInteractive -ExecutionPolicy RemoteSigned -File` を使用します。
実行ポリシーを恒久変更しません。ダウンロード由来のブロック等で失敗した場合は、
失敗出力を確認し、既存Git cloneからの導入も利用できます。

## 設定ファイル

保存場所: `<game>/Mod-Tools/mod-tools.json`。ユーザーごとのローカル設定です。

| キー | 内容 |
|---|---|
| `format` | `msm2-mod-tools-1`。既存フォルダーを自ツールとして認識する識別子 |
| `tool_version` | 導入版。現在 `1.2.0` |
| `backup_directory` | ゲーム外の保存先。初回の既定値はホームの `MSM2-Mod-Backups` |
| `profile` | 使用するMSM2プロファイルのJSONファイル名から拡張子を除いた名前 |
| `restore_requires_install` | 復元後のMOD再適用確認が必要ならtrue。未設定はfalse相当 |

`save_settings()` は一時 `.pending` へ書いた後に `os.replace()` で反映します。
設定形式を変える際は既存の保存先・プロファイル・復元確認状態を移行してください。

## 共通の保護と境界

処理対象はゲームルートの実在EXEから判定し、リンク・ジャンクション等を検査します。
共有Overstrikeは `<game>/Overstrike/Overstrike.exe` の1つで、起動CWDは常に `<game>`。
ゲームルートの `winmm.dll` と `scripts_proxy.dll` は保持します。`Overstrike/winmm.dll` は誤配置として拒否しますが、`Overstrike/scripts_proxy.dll` はinstallerのコピー元として通常ファイル・非reparseなら許容します。両者のbytes一致を新しく要求したり、自動コピー・削除したりしません。
存在するSF/WSの `_source/.git` は独立保存であることを調べ、外部Git依存を拒否します。

Install/Start/Play/Open/Backup/Restore適用は `<game>/.MSM2-Mod-Tools.lock` の
存在だけでは判定しません。全更新・起動・保存・復元を `.MSM2-Operations.guard` のOS所有権で直列化します。監査・互換用lockファイルは解放後も残り得ます。
VerifyとRestoreの確認のみは、この変更操作用ロックを取得しません。
PowerShellはゲームとOverstrikeのプロセス実行パスを照合し、同名の別パスや
実行パス不明のプロセスを拒否します。強制終了は行いません。

## 改変・保存の運用

実装は [mod_environment.py](../../environment/mod_environment.py)、Windows部分は
[windows.ps1](../../environment/windows.ps1)、入口は各CMDです。
コードの正本はOrganizationの `codex/v01c20-edi-value-provenance` ブランチ。
`main` は取り込みません。SF本体や通常MODの変更は、このツールの通常起動の範囲外です。

動作を変えたら対応する仕様書・利用ガイド・テスト・`CODEX_RESULT.md` を更新します。
入口を追加したら `KIT`、CMD、ショートカット生成、配布allowlistも確認します。
配布版更新では `VERSION` とリリースメタデータを整合させ、
`build_distribution.py` でソースのみのZIPを生成して内容を検証します。
原データを含むユーザーバックアップを、仕様書や公開ZIPへ添付しないでください。

既存自動検証はPython 27試験・PowerShell 16チェックです。これはv1.2.0実装の記録で、
この文書だけの追加のために再実行したという意味ではありません。
ユーザーの画面で3つのショートカットが存在することは確認済みですが、
各アイコンからの全機能の実行・実バックアップ・Vanilla復元の完了とは区別します。


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

## 一度だけのtooling修復（PowerShell）

ゲーム・Overstrike・そのupdaterを閉じて実行します。旧launcher/checkerを通さず、
現在のGit branch/remote/dirty/ff-onlyを確認してKITとUpdate-WS.cmdだけを更新します。
WS build/配置、DLL移動、profile resetは実行しません。個人のゲームパスはこの実行例のみです。

```powershell
$game = "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2"
$repo = Join-Path $game 'Mods Library\Mods-Developing\MSM2-WS\_source'
$branch = 'codex/v01c20-edi-value-provenance'
if ((git -C $repo branch --show-current) -ne $branch) { throw 'Wrong WS branch' }
if ((git -C $repo remote get-url origin) -ne 'https://github.com/deluxxxe-dev/MSM2-WallSprint.git') { throw 'Wrong WS origin' }
git -C $repo fetch origin $branch
if ($LASTEXITCODE -ne 0) { throw 'Fetch failed' }
$commit = git -C $repo rev-parse "origin/$branch"
if ($LASTEXITCODE -ne 0) { throw 'Head unavailable' }
$body = @(git -C $repo show ($commit + ':environment/Repair-Mod-Tools.ps1'))
if ($LASTEXITCODE -ne 0) { throw 'Repair source unavailable' }
$temp = Join-Path $env:TEMP ('MSM2-repair-' + [guid]::NewGuid().ToString('N') + '.ps1')
[IO.File]::WriteAllText($temp, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
try { & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $temp -GameRoot $game }
finally { Remove-Item -LiteralPath $temp -Force }
```

成功は `TOOLING_REPAIR_COMPLETE runtime_unchanged=1 no_build=1 no_game_launch=1`。
拒否時は表示されたPID/phase/WSL/パスを保全し、lockを手で削除しないでください。


<a id="overstrike"></a>

# MSM2 - Overstrike 仕様書

対象: Mod-Tools v1.2.0 / 2026-10-29。配置・設定の共通仕様は
[仕様書ライブラリ](README.md)を参照。

## 目的・入出力

ダブルクリックで共有Overstrikeを開くための入口です。WSの配置を確認し、初回は
ツールとショートカットを導入します。通常起動の中でMODの更新・ビルド・インストールや
ゲーム開始は行いません。必要な操作は起動したOverstrikeで行います。

| 項目 | 仕様 |
|---|---|
| ショートカット | `MSM2 - Overstrike.lnk` |
| リンク先 | `<game>/Mod-Tools/Start-Overstrike.cmd` |
| Python action | `start` |
| 起動対象 / CWD | `<game>/Overstrike/Overstrike.exe` / `<game>` |
| 対応CLI | `Start-Overstrike.cmd --game-root "<game>"` |
| 成功 | 検査結果を表示し、起動要求を送るか既存ウィンドウを前面に出す。終了0 |
| 失敗 | `MOD ENVIRONMENT ERROR:` 等の出力、終了非0、CMDで画面を保持 |

成功は起動要求・既存ウィンドウ再利用の成功であり、Overstrikeの起動後の正常動作や
MODインストール完了まで監視する仕様ではありません。

## ゲームルートの決定

1. `--game-root` があれば使用。
2. エンジンの所在が `<game>/Mod-Tools` ならその親。
3. それ以外の `start` は、エンジンの祖先から `Spider-Man2.exe` のある場所を探す。
4. 見つからなければ初回入力を求める。

そのため現在の `_source/environment/Start-Overstrike.cmd` からも自動判定できます。
外へ展開したZIPからの初回は入力が必要です。個人用C:/D:パスは組み込みません。

## v1.2 の起動・修復順序

旧版のDLL退避、ReadOnly属性変更、PIDだけでのlock削除は行いません。
`Start-Overstrike.cmd` はPythonの現行 `start_manager()` を呼びます。

1. ゲームルートを検証し、共有OSゲートを取得。既存ファイルの存在だけで更新中としない。
2. 導入済みと認識できるKITは、現行ソースから検証・stageして旧checker実行前に更新。
   変更が必要な場合はゲーム/manager/updaterの終了を確認し、強制終了しない。
3. 現行の環境/DLL/profile/独立Git/単一WS script/可視archiveなしを検査。
   Git `index.lock` は削除せず拒否。解放済み監査lockは起動を妨げない。
4. 初回導入は現行検査を通してから行う。既存設定、保存先、profile、復元確認状態を保持。
5. ゲート保持中にWindows helperで共有managerを起動/再利用し、その後OSハンドルを閉じる。

正常Update-WSは同じゲートで検証/配置を確定し、所有権を解放してからこのchecked
launcherへ進みます。起動だけが失敗しても確定済みWSは巻き戻しません。
旧入口で検査前に止まる場合は共通READMEの「一度だけのtooling修復」を使います。

`.script` の確認は名前・配置の確認です。内部DLLのハッシュ検証、ゲームビルドの
ネイティブ互換性判定やGitブランチ/dirty状態の検証を行う代替アップデーターではありません。

## 初回・ツール更新時だけ行うこと

`_install()` はゲーム/Overstrikeが終了していることと保存対象の走査を確認し、
`KIT` に列挙された自ツールだけを `<game>/Mod-Tools` へコピーします。
設定に版を保存し、Overstrike / Play / Backupの3つのショートカットをDesktopへ作ります。
既存のバックアップ先・選択プロファイル・復元確認フラグは保持します。
バックアップ先が未設定ならホームの `MSM2-Mod-Backups` を登録しますが、この段階では保存しません。

導入済みの通常起動は、全MODのバックアップ走査や再コピーを行いません。
正常Update-WS後も現行KITを自動refreshします。`refresh_kit()` は同一hashなら何も変更せず、旧コードは `_toolkit_previous` に可逆保存します。
配布一式が欠けた状態は、インターネットから補完せずエラーにします。

## 起動済みプロセス・失敗時の扱い

同じパスのOverstrikeが起動済みなら `WScript.Shell.AppActivate(PID)` を使用します。
前面化できなければエラーにして、二重起動しません。ゲームが起動中でも通常の
manager action自体はそれを強制終了しません。初回/更新の導入が必要なら終了を求めます。
同名で別パスのゲーム/Overstrikeや識別不能プロセスは、操作を止めて手動対応を求めます。

重複scriptやアーカイブを見つけても削除・移動しません。`Update-WS` の確認対象です。
通常起動でSF・通常MOD・WSソース・既存プロファイルを変更しません。
旧 `Open-Overstrike.cmd` は `manager` actionの簡易入口であり、`start` の初回セットアップや
追加のWS一意性検査を行わないことを区別してください。

`Install mods` のloader/ACLエラーは別のinstaller問題として扱います。今回のlauncher/repairはDLLや属性を変更せず、誤配置winmmも自動移動しません。

## 改変箇所と確認項目

| 変更したい内容 | 主な実装箇所 |
|---|---|
| 起動前検査/初回導入 | `Start-Overstrike.cmd`、`start_manager()`、`ws_launch_ready()`、`_install()` |
| パス自動判定/CLI | `main()` の `start` 分岐 |
| 起動・既存ウィンドウ | `Windows.launch()`、windows.ps1の `manager` |
| 名前・アイコン・リンク先 | `Set-ModShortcuts()` と `Start-Overstrike.cmd` |

変更後は新規/旧版からの導入、設定と未コミット作業の保持、通常2回目の非再導入、
OS所有権・旧lockの証明付き回復、KIT refreshの反復、loader非変更、重複script/大文字ZIP/active lock/リンク/ローダー欠落/異なるプロファイルの拒否を確認します。
PowerShell側は正しいEXE/CWD、同一プロセス再利用、別パス拒否、3つの `.lnk` と
同名別アプリ保全を検証します。自動テストは共通READMEから参照してください。
Windows実機では、移動した `.lnk` を含め、既存の普通MOD+SF+WSが同じUIに現れること、DLL属性を自動変更しないことも確認します。Install modsのloader/ACLエラーは別途診断します。

## scripts_proxy installer依存の訂正 — 2026-10-31

Overstrike1.8.1.0公開tagのSetupScriptsは `File.Copy("scripts_proxy.dll", <game>/winmm.dll)` を使用します。コピー元はprocess CWD相対で、exe隣へ絶対固定したコードではありません。この実機の例外は `<game>/Overstrike/scripts_proxy.dll` を要求していました。通常ファイル・非reparseのそのコピー元を許容し、rootの2ファイルも維持します。`Overstrike/winmm.dll` は拒否し、今回のlauncherは移動・削除しません。sourceとloaderの新規hash一致条件、DLLの自動取得・コピー・削除は追加しません。

共通 `environment_ready()` の同じ混同も訂正したのでStart/Open/Playに適用されます。v1.2.0のStart CMDは現在のPython checkerへ委譲し、DLL退避・削除・ReadOnly正規化を行いません。コピー元属性がInstallに影響する可能性と、launcher起動確認は別です。Profile/script-support設定は変更しません。


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


<a id="play"></a>

# MSM2 - Play Modded 仕様書

対象: Mod-Tools v1.2.0 / 2026-10-24。配置・設定の共通仕様は
[仕様書ライブラリ](README.md)を参照。

## 目的・入出力

Overstrikeで既に適用したMOD構成のまま、ゲームを直接開く入口です。
実装は参照時点のOverstrike `LaunchGame` と同じEXE/CWD/`-scripts` の方針を使用します。
起動時にOverstrikeを開く、MODを再適用する、コードを更新する機能はありません。

| 項目 | 仕様 |
|---|---|
| ショートカット | `MSM2 - Play Modded.lnk` |
| リンク先 | `<game>/Mod-Tools/Play-Modded.cmd` |
| Python action | `play` |
| 起動対象 / CWD | `<game>/Spider-Man2.exe` / `<game>` |
| 対応CLI | `Play-Modded.cmd --game-root "<game>"` |
| 復元後の明示確認 | `--confirm-installed`、または対話で `INSTALLED` |
| 成功 | ゲーム起動要求か同じゲームの前面化。終了0、成功時のpauseなし |
| 失敗 | エラー出力・非0終了、CMDで画面を保持 |

起動要求後のプロセス継続・ゲーム画面・各MODの動作までは検証しません。
Steamを別途インストール/起動/ログインさせる処理もありません。

## 前提と処理順序

1. `--game-root` またはエンジンがある `Mod-Tools` の親からゲームを決定する。
   それ以外の場所からの実行は入力を求める（`start` 専用の祖先探索は行わない）。
2. 復元確認フラグがあり、明示オプションがなければ `INSTALLED` の入力を求める。
3. `play()` がゲームルートを検証し、共通操作ロックを取得する。
4. 共有Overstrike、rootの2つの依存ファイル、Mods Library、独立Git保存を確認する。通常ファイル・非reparseの `Overstrike/scripts_proxy.dll` は許容し、誤配置 `Overstrike/winmm.dll` は拒否する。
5. 保存された名前、または現在選択中/唯一の候補からMSM2プロファイルを選ぶ。
   このゲームルートを指し、`settings.scripts.enabled` がtrueであることを要求する。
6. `scripts.txt` と `scripts/` の実在、非空のDLL一覧を確認する。
7. 各非空行が `/`、`\`、`:` を含まない `.dll` 名で、`scripts/` に実ファイルが
   存在し、リンク等ではないことを確認する。
8. 復元確認が必要なら、明示確認があることを要求しフラグをfalseに保存する。
9. Windows helperの `play` actionで同名プロセスの実行パスを照合して起動/前面化する。

プロファイルを新規作成・書き換えたり、`scripts.txt` やDLLを修復したりしません。
インストール不足は共有Overstrikeで `Install mods` を実行して解決します。
`start` のWS配下script/ZIP一意性検査は、直接Playの処理には含まれません。

## 起動引数と起動済みの場合

`commandline.txt` があり、空白で分割した中に `-scripts` があれば引数を追加しません。
PowerShellの比較は大文字小文字を区別しないので `-ScRiPtS` も認識します。
それ以外は `-scripts` を1つ指定してゲームを開始します。
このツール自身が `commandline.txt` 全体を読み直して引数へ再追加する処理はありません。
ファイル自体の内容も保持します。

同じゲームパスの既存プロセスがあれば前面化し、新しいゲームは起動しません。
同名の別パス/パス不明は拒否します。前面化できない場合も二重起動せず失敗します。
PowerShellの共有事前検査はOverstrike側のプロセス実行パスも確認するため、
別場所のOverstrikeが動いている場合も停止します。プロセスの強制終了はありません。

## 復元後の確認フラグ

Restore適用は、ファイル復元後に `restore_requires_install=true` を設定します。
Vanillaへファイルを戻しただけではゲームアーカイブへのMOD再適用が終わらないためです。
共有Overstrikeの `Install mods` が成功してから `INSTALLED` と入力します。
自動処理からの `--confirm-installed` も、利用者がその完了を明示するための指定です。

これは人の申告であり、ツールがOverstrikeのインストール完了を解析する機能ではありません。
現在はフラグを保存してから起動要求を送るため、起動要求が後で失敗してもフラグはfalseのままです。
この順序を変更する場合、二重起動対策と失敗時の設定保全を併せて検討します。

## 改変箇所と確認項目

| 変更したい内容 | 主な実装箇所 |
|---|---|
| プロファイル/DLLの前提 | `_play()`、`profile()`、`environment_ready()` |
| 復元後の対話/フラグ | `main()` のplay側、`_play()`、`restore()` |
| 引数・起動・前面化 | windows.ps1の `play` action |
| アイコン・リンク先 | `Set-ModShortcuts()`、`Play-Modded.cmd` |

変更後はscripts未導入/空一覧/欠落DLL/危険なファイル名/無効プロファイルの拒否、
復元フラグの確認前拒否と確認後解除、commandline有無・大小文字での `-scripts` 非重複、
同じゲームの再利用・別ゲームパスの拒否・正しいCWDを検証します。
改変でSF/通常MODの設定を書き換えたり、ゲーム内のPosition等の書込みを追加したりしません。
Windows実機では通常のSteam環境でゲームが開き、導入済みMODが作用することを確認します。

共有OS所有権・KIT refresh・legacy修復の現行仕様は共通READMEのv1.2節です。
active update中のPlayはOS所有権で拒否し、存在だけのsentinel判定はしません。


<a id="backup"></a>

# MSM2 - Backup Mods 仕様書

対象: Mod-Tools v1.2.0 / 2026-10-24。配置・設定の共通仕様は
[仕様書ライブラリ](README.md)を参照。

## 目的・入出力

通常MOD、SF/WSのソース・Git履歴・未コミット作業・ログを含む共有MOD環境を、
フォルダー階層ごと私用バックアップとして保存します。保存後に全ファイルを検証します。
この明示的な共有バックアップ/復元は、WSアップデーターのWS専用操作とは別の許可範囲です。

| 項目 | 仕様 |
|---|---|
| ショートカット / リンク先 | `MSM2 - Backup Mods.lnk` / `<game>/Mod-Tools/Backup-Mods.cmd` |
| Python action / 主関数 | `backup` / `backup()` |
| 保存先の優先順 | `--backup-dir` → `mod-tools.json` → 入力（既定はホームのMSM2-Mod-Backups） |
| ゲームルート | `--game-root` → Mod-Toolsの親 → 入力 |
| 成功 | `VERIFIED BACKUP: <保存フォルダー>` を表示、終了0 |
| 失敗 | エラーを表示、終了非0。通常の捕捉可能な失敗では自分のpendingだけを除去 |
| CMDの画面 | 成功/失敗の両方でpause |

指定例: `Backup-Mods.cmd --backup-dir "<ゲーム外の保存先>"`。
この指定は今回の出力先です。既定の保存先を変更するには
`Install-Mod-Tools.cmd --backup-dir "<保存先>"` を使用します。

## 保存範囲

| ルート | 保存内容 |
|---|---|
| `Overstrike/` | 共有マネージャー本体と配下 |
| `Profiles/` | 選択状態・プロファイル・設定 |
| `Mods Library/` 全体 | 通常MOD、SF/WS本体、ソース、隠し.git、未追跡/編集中の保存済みファイル、ログ |
| `scripts/` | インストール済みDLL・付属データ・設定 |
| `Mod-Tools/` | ツールとローカル設定 |
| ルートの個別ファイル | `scripts_proxy.dll`、`winmm.dll`、存在する `scripts.txt` / `commandline.txt` |

空ディレクトリも記録します。ゲームEXEは照合用SHA256だけを記録し、本体を格納しません。
`asset_archive` 等のゲーム本体、セーブ、外部のSteam/Python/Git/WSL/コンパイラー本体や
資格情報ストアは対象外です。Gitローカル設定やログ等、対象ルート内にある個人情報は含みます。
アップロードはせず、公開配布ZIPにも私用バックアップを入れません。

## 保存処理の順序

1. ゲームと出力先を検証。出力先はゲーム内・ゲームの祖先フォルダーを拒否。
2. 共通ロック取得後、ゲーム/共有Overstrikeが終了していることを確認。
3. 共有環境、独立Git、導入済み設定のformatを確認し、対象の一覧/メタデータを採取。
   リンク等、外部Git依存、`index.lock`、未検証のlegacy lock、非通常ファイルを拒否。現行の認証済み互換lockは保存対象から除外。
4. 出力先を作成し、未圧縮合計+64MiBより多い空き容量を要求。
5. `.pending-<識別子>` 内にZIP64対応・DEFLATEレベル1で `environment.zip` を作成。
6. ファイルごとに1MiBずつ読み書きし、SHA256を算出。200ファイルごとに進捗を表示。
7. 一覧/サイズ/更新時刻/モード/空ディレクトリを再採取し、開始時との差分があれば停止。
8. ZIPを再読込し、全payloadのサイズ・CRC・SHA256とマニフェストを検証。
9. ゲームEXEのハッシュが変わっていないことを確認し、独立復元用 `KIT` 一式を同梱。
10. プロセス停止・対象一覧・ゲームハッシュをもう一度確認し、pendingを完成名へ変更。

これはVSS等による原子的なライブスナップショットではありません。エディターや更新を
止めて保存します。走査前後のメタデータ比較は、変更の完全検出を保証するものではありません。
クラッシュ/電源断でのpending自動回収やバックアップ世代の自動削除はありません。

## 出力形式・互換性

完成フォルダー名はローカル時刻の `MSM2-Mod-Environment-YYYYMMDD-HHMMSS`。
同名がある場合は `-01` 等を追加し、過去の保存を上書きしません。フォルダーを丸ごと保管します。

```text
MSM2-Mod-Environment-YYYYMMDD-HHMMSS/
  environment.zip
    manifest.json
    payload/<ルートからの相対パス...>
  Restore-Mods.cmd
  Verify-Backup.cmd
  mod_environment.py
  windows.ps1
  ...KITの残り...
```

manifestの `format` は `msm2-mod-environment-1`。
`tool_version`、`created_local`、`original_game_root`、`game_sha256`、`roots`、
`directories`、`files` を持ちます。filesは各 `path/size/mtime_ns/mode/sha256` を持ちます。
検証時はformatとルート一覧を厳密に照合し、tool_versionは文字列として検証します。
許可する上限はファイル50万・ディレクトリ50万、manifest 64MiB、未圧縮合計1TiBです。

相対パスの逸脱、Windows予約名、大小文字別名、重複/未登録エントリ、親の欠落、
リンク、暗号化エントリ、サイズ/ハッシュ不一致を拒否します。
ファイル内容・ファイル更新時刻・基本modeは保持しますが、ACL/所有者/代替ストリームや
ディレクトリ時刻を完全に保存する仕様ではありません。SHA256は破損検出用で、発行者署名ではありません。

## 付属の検証・復元

`Verify-Backup.cmd` は隣の `environment.zip` または `--archive` を全件検証します。
対象ゲームが失われていても検証でき、元リポジトリには依存しません。
ただしPython/PowerShellなどの外部実行環境は別途必要です。

`Restore-Mods.cmd` は元と同じ絶対ゲームパス・同じEXEのSHA256を要求します。
確認/一覧表示後、`RESTORE` または明示 `--apply` で適用します。
ゲーム/Overstrikeを閉じた状態で検証・ステージ展開し、現在の対象ルートを
ゲームの隣の `.MSM2-rollback-...` へ退避してからルート単位で置き換えます。
ゲーム本体は戻しません。バックアップで存在しなかった対象ルートは、現在の分を退避します。

途中失敗時は逆順に元へ戻し、ロールバック自体の失敗は両方の保全場所を示して失敗とします。
成功時も元のデータの退避先は自動削除しません。電源断をまたぐ自動ロールバック機能ではありません。
成功後は `restore_requires_install=true` にし、共有Overstrikeを開きます。
Vanillaゲームアーカイブへの再適用は `Install mods`、その後Playの `INSTALLED` 確認で進めます。
別パスへの移設・破損した元データの意味的な修復・各MODの正常動作保証は行いません。

## 改変箇所と確認項目

| 変更したい内容 | 主な実装箇所 |
|---|---|
| 保存範囲/形式 | `TREES`、`FILES`、`ROOTS`、`FORMAT`、`scan()`、`validate_manifest()` |
| 圧縮・分割・進捗・出力名 | `backup()`、`unique()`、`BUFFER` |
| 出力先/空き容量/競合 | `external_folder()`、`backup()`、`lock()`、Windowsのcheck action |
| 復元/退避/互換性 | `verify()`、`plan_restore()`、`restore()`、`save_settings()` |

範囲やformatの変更は過去バックアップを読めるか、移行が必要かを先に決めます。
Git履歴/dirty/未追跡/通常MOD/空フォルダー/読み取り専用と時刻の往復、元ゲーム消失後の
独立復元、ZIP破損と危険パス拒否、更新中拒否、途中置換失敗とロールバック失敗の保全を検証します。
Windows実機では最初の保存と再検証を優先し、健全な環境を消して復元試験をしないでください。

現行restoreはOSゲートをROOTS外で保持し、Mods Library内に開いたlock handleを残しません。
復元stageには現行KIT/互換markerを用意してから切替し、旧checker復活を防ぎます。
詳細は共通READMEのv1.2 OS所有権を参照。
