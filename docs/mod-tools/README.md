# MSM2 Mod-Tools 仕様書ライブラリ

対象は現在の **Mod-Tools v1.1.0** が作成する3つのショートカットです。
作成日: 2026-10-24（日本時間）。実装基準:
[`845e33cafecae352e5194d790358500bb0066081`](https://github.com/deluxxxe-dev/MSM2-WallSprint/commit/845e33cafecae352e5194d790358500bb0066081)。
この資料追加はツールの動作・配布ZIP・WS/SFのバージョンを変更しません。

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
| `tool_version` | 導入版。現在 `1.1.0` |
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
排他的作成で調停します。残っているロックを自動削除しません。
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

既存自動検証はPython 27試験・PowerShell 16チェックです。これはv1.1.0実装の記録で、
この文書だけの追加のために再実行したという意味ではありません。
ユーザーの画面で3つのショートカットが存在することは確認済みですが、
各アイコンからの全機能の実行・実バックアップ・Vanilla復元の完了とは区別します。
