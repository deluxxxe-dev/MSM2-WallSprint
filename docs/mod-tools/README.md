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


## 前回の切り分け手順（今回の再実行は不要）

ビルド成功と配置成功は別です。`Cannot verify same-user Linux process339` だけでは
消滅競合・権限・残存buildのどれか分かりません。現在のcheckerはdistro/UID/PID/
stage/category/errno/exitを出し、token方式で不要なcmdline/cwdを読みません。
生存token、権限不明、PID再利用、distro/既知UID不一致は引き続き拒否します。
`pidfd`/再確認した不存在で終了が証明されたPIDだけを除外します。

ゲーム/Overstrike/updaterを通常終了し、Windows PowerShellで以下を一括実行します。
これは最新診断コードを一時取得します。fetchはGit refs/objectsを更新しますが、
worktree/lock/KIT/runtimeを変更せず、ビルド・配置・起動はしません。診断のQUIETは
配置成功の証明ではありません。出力をChatへ返してください。

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
if ($LASTEXITCODE -ne 0) { throw 'Diagnostic bootstrap unavailable' }
$temp = Join-Path $env:TEMP ('MSM2-diagnostic-' + [guid]::NewGuid().ToString('N') + '.ps1')
[IO.File]::WriteAllText($temp, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
try { & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $temp -GameRoot $game -DiagnoseWSL }
finally { Remove-Item -LiteralPath $temp -Force }
```

診断後の通常retryは既存仕様上フルビルドをやり直します。`_archive/_build` に
payload/manifestが残っていてもexact source-HEAD/session/inactivity receiptがないため
自動再利用・手動配置は許可しません。旧pending記録のUIDは未記録なのでnullと明示します。
新buildではUIDを記録し、不一致を拒否します。WSL/PIDの一括終了やlock削除はしません。


## 現在のEACCES回復 — root読み取り確認付きtooling修復

前回の診断は受理済みです。今回は同じDiagnoseWSLを繰り返さず、以下の通常修復を
一括実行します。WSLの選択済みdistro内でrootが**正確な操作tokenだけを読み取り確認**
します。全UIDを対象にし、元UID未記録のnullを1000と推測しません。rootでビルドや配置を
実行せず、権限/セキュリティ設定やWSLを変更・停止しません。
rootでも読めない・tokenが残る・identity不一致・inventoryが安定しない場合は拒否します。

ゲーム/Overstrike/updaterを通常終了後、Windows PowerShellで一括実行し出力を返してください。
fetchはrefs/objectsを更新し、修復はclean/ff確認後だけsource/KITを更新します。
r5 runtime/packageは変更せず、コンパイル・ゲーム起動をしません。

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
if ($LASTEXITCODE -ne 0) { throw 'Repair bootstrap unavailable' }
$temp = Join-Path $env:TEMP ('MSM2-repair-' + [guid]::NewGuid().ToString('N') + '.ps1')
[IO.File]::WriteAllText($temp, ($body -join "`n"), (New-Object Text.UTF8Encoding($false)))
try { & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $temp -GameRoot $game }
finally { Remove-Item -LiteralPath $temp -Force }
```

`WSL_TOKEN_CHECK ... TOKEN_ABSENT` はtoken回収の証拠だけで、配置成功ではありません。
その後の `TOOLING_REPAIR_COMPLETE runtime_unchanged=1 no_build=1 no_game_launch=1`
がtooling修復完了です。root利用不能/拒否でもlock削除等で回避せず、その出力を返します。
現在はr5配置・実ゲーム確認は未完了です。後で通常Update-WSを行う際は再ビルドします。
