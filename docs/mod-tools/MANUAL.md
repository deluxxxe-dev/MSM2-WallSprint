# MSM2 Mod-Tools 総合マニュアル・仕様書

対象: v1.1.0 / 作成日: 2026-10-24（日本時間）。
用途: GitHubで運用・設定・安全条件・今後の改変箇所をまとめて参照する資料。

- [共通仕様・使い方・改変運用](#common)
- [MSM2 - Overstrike](#overstrike)
- [MSM2 - Play Modded](#play)
- [MSM2 - Backup Mods／検証・復元](#backup)

原本はリポジトリの `docs/mod-tools/README.md` と各仕様書です。
このファイルと原本を同時に更新してください。
以下の実装リンクは対象版のコミットに固定しています。

<a id="common"></a>

## MSM2 Mod-Tools 仕様書ライブラリ

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

### いつ何を使うか

| やりたいこと | 操作 |
|---|---|
| 現在のMOD構成で遊ぶ | `MSM2 - Play Modded` |
| MODを追加/切替/再適用する | `MSM2 - Overstrike` → 選択確認 → Install mods → Run modded game |
| WS本体をソースから更新する | 既存の `MSM2-WS/Update-WS.cmd`。この3つのショートカットは更新処理を兼ねない |
| SF本体を更新する | SF側の既存 `Update-SF.cmd`。開発はSFの作業系で進める |
| 更新前や健全な状態を保存する | ゲーム/Overstrike/更新処理を閉じ、`MSM2 - Backup Mods` |
| 保存したデータの破損を確認する | 保存フォルダーの `Verify-Backup.cmd` |
| 同じ場所のVanillaへ環境を戻す | 保存フォルダーの `Restore-Mods.cmd` → Install mods → Playの確認 |

### 導入・ツール更新

現在の独立Git cloneを、意図した開発ブランチ上でfast-forward更新します。
その後 `_source/environment/Start-Overstrike.cmd` をダブルクリックします。
初回/ツール更新時はゲームとOverstrikeを閉じてください。
ZIP版を使う場合はMods Library外へ一式を展開して同じCMDを実行し、ゲームルートを入力します。
Python等が導入済みなら、共有Overstrikeやローダーを再インストールする手順はありません。

ツール一式が `<game>/Mod-Tools` に保存され、3つのショートカットができます。
既存のMOD・SF/WS・ログの構造は維持されます。起動処理はGitHubの新版を自動取得しません。
ダウンロードURLが404でも既存Git cloneから取得できます。認証/アクセス権も確認し、
リンク表示だけを配布成功の証拠にしないでください。Gitの更新失敗時は古い内容を破棄せず出力を保存します。

### 共通の配置と依存

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

### ショートカットと実ファイルの違い

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

### 共通の実行経路

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

### 設定ファイル

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

### 共通の保護と境界

処理対象はゲームルートの実在EXEから判定し、リンク・ジャンクション等を検査します。
共有Overstrikeは `<game>/Overstrike/Overstrike.exe` の1つで、起動CWDは常に `<game>`。
ゲームルートの `winmm.dll` と `scripts_proxy.dll` は保持します。`Overstrike/winmm.dll` は誤配置として拒否しますが、`Overstrike/scripts_proxy.dll` はinstallerのコピー元として通常ファイル・非reparseなら許容します。両者のbytes一致を新しく要求したり、自動コピー・削除したりしません。
存在するSF/WSの `_source/.git` は独立保存であることを調べ、外部Git依存を拒否します。

Install/Start/Play/Open/Backup/Restore適用は `<game>/.MSM2-Mod-Tools.lock` の
排他的作成で調停します。残っているロックを自動削除しません。
VerifyとRestoreの確認のみは、この変更操作用ロックを取得しません。
PowerShellはゲームとOverstrikeのプロセス実行パスを照合し、同名の別パスや
実行パス不明のプロセスを拒否します。強制終了は行いません。

### 改変・保存の運用

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

---

<a id="overstrike"></a>

## MSM2 - Overstrike 仕様書

対象: Mod-Tools v1.1.0 / 2026-10-29。配置・設定の共通仕様は
[仕様書ライブラリ](README.md)を参照。

### 目的・入出力

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

### ゲームルートの決定

1. `--game-root` があれば使用。
2. エンジンの所在が `<game>/Mod-Tools` ならその親。
3. それ以外の `start` は、エンジンの祖先から `Spider-Man2.exe` のある場所を探す。
4. 見つからなければ初回入力を求める。

そのため現在の `_source/environment/Start-Overstrike.cmd` からも自動判定できます。
外へ展開したZIPからの初回は入力が必要です。個人用C:/D:パスは組み込みません。

### 起動前の限定自己修復

インストール済み `Start-Overstrike.cmd` はPythonの環境検査より前に、共有環境で実際に確認された次の3ケースだけを限定的に処理します。

1. `<game>/Overstrike/winmm.dll` が誤配置されている場合、SHA256で識別して `<game>/Mod-Tools/_quarantine` へ退避する。同一bytesが既に退避済みなら誤配置側だけを削除する。
2. ゲームルートの `scripts_proxy.dll` または `winmm.dll` がReadOnlyの場合、その属性だけを解除して再確認する。ファイルbytesは変更しない。
3. `<game>/.MSM2-Mod-Tools.lock` が残っていて、その中のPIDが現在存在しない場合だけstale lockとして削除する。PIDが生きている場合は保持する。

このReadOnly正規化はrootの2ファイルに限定した属性修復です。Overstrike1.8.1.0のコピー元 `scripts_proxy.dll` はprocess CWD相対なので、実際のコピー元がOverstrike配下ならそのReadOnly属性はこのroot修復では変わりません。コピー元がReadOnlyなら再作成されたwinmmへ属性が引き継がれ得ますが、root正規化だけでInstall成功や再発防止を保証しません。

自己修復は上記ローダー/lockだけに限定し、Profiles、Mods Library、SF、WS runtime、通常MOD、asset dataを変更しません。予期しない状態ではcleanup範囲を広げず失敗します。

### 処理順序

1. インストール済みCMDなら上記の限定自己修復を実行する。
2. ゲームルートとEXEを検証し、共通操作ロックを取得。
3. 共有Overstrike、ルートの2つのローダー、Mods Library、独立Git保存を確認。
4. このゲームを対象にしたMSM2プロファイルを選択・確認する。設定は書き換えない。
5. `ws_launch_ready()` でWSの次の条件を確認する。
   - `MSM2-WS/_source/.git` がディレクトリとして存在する。
   - Gitの `index.lock` と `_archive/Update-WS.lock` がない。
   - WS配下にリンク・ジャンクション等、走査不能の場所がない。
   - 有効 `.script` はルートに1個だけで、名前は `MSM2-WS-v<version>.script`。
   - WS配下に可視 `.zip/.rar/.7z` がない（大文字拡張子も対象）。
   - `_source/.git` の内部オブジェクトは、このscript/アーカイブ走査から除外する。
6. `Mod-Tools` が未知の他用途フォルダーなら停止する。
7. バージョン不一致・ファイル不足・元ツールとの差分があれば `_install()` を実行。
8. Windows helperの `manager` actionへ進み、共有Overstrikeを起動/再利用。
9. 成功/失敗を問わず通常の例外処理ではロックを解放する。

`.script` の確認は名前・配置の確認です。内部DLLのハッシュ検証、ゲームビルドの
ネイティブ互換性判定やGitブランチ/dirty状態の検証を行う代替アップデーターではありません。

### 初回・ツール更新時だけ行うこと

`_install()` はゲーム/Overstrikeが終了していることと保存対象の走査を確認し、
`KIT` に列挙された自ツールだけを `<game>/Mod-Tools` へコピーします。
設定に版を保存し、Overstrike / Play / Backupの3つのショートカットをDesktopへ作ります。
既存のバックアップ先・選択プロファイル・復元確認フラグは保持します。
バックアップ先が未設定ならホームの `MSM2-Mod-Backups` を登録しますが、この段階では保存しません。

導入済みの通常起動は、全MODのバックアップ走査や再コピーを行いません。
自動アップデートはなく、GitHub上の新版は利用者が取得して初回と同様に導入します。
配布一式が欠けた状態は、インターネットから補完せずエラーにします。

### 起動済みプロセス・失敗時の扱い

同じパスのOverstrikeが起動済みなら `WScript.Shell.AppActivate(PID)` を使用します。
前面化できなければエラーにして、二重起動しません。ゲームが起動中でも通常の
manager action自体はそれを強制終了しません。初回/更新の導入が必要なら終了を求めます。
同名で別パスのゲーム/Overstrikeや識別不能プロセスは、操作を止めて手動対応を求めます。

重複scriptやアーカイブを見つけても削除・移動しません。`Update-WS` の確認対象です。
通常起動でSF・通常MOD・WSソース・既存プロファイルを変更しません。
旧 `Open-Overstrike.cmd` は `manager` actionの簡易入口であり、`start` の初回セットアップや
追加のWS一意性検査を行わないことを区別してください。

`Install mods` が、更新済みlauncherで両root loaderのReadOnlyを解除した後にも `winmm.dll` の `UnauthorizedAccessException` で失敗する場合は、同じInstallを繰り返さず、active file lockまたはWindows ACLを次に調査します。

### 改変箇所と確認項目

| 変更したい内容 | 主な実装箇所 |
|---|---|
| 起動前検査/初回導入 | `Start-Overstrike.cmd`、`start_manager()`、`ws_launch_ready()`、`_install()` |
| パス自動判定/CLI | `main()` の `start` 分岐 |
| 起動・既存ウィンドウ | `Windows.launch()`、windows.ps1の `manager` |
| 名前・アイコン・リンク先 | `Set-ModShortcuts()` と `Start-Overstrike.cmd` |

変更後は新規/旧版からの導入、設定と未コミット作業の保持、通常2回目の非再導入、
誤配置loader/ReadOnly root loader/stale lockの限定修復、重複script/大文字ZIP/active lock/リンク/ローダー欠落/異なるプロファイルの拒否を確認します。
PowerShell側は正しいEXE/CWD、同一プロセス再利用、別パス拒否、3つの `.lnk` と
同名別アプリ保全を検証します。自動テストは共通READMEから参照してください。
Windows実機では、移動した `.lnk` を含め、既存の普通MOD+SF+WSが同じUIに現れること、ReadOnly正規化後に連続した `Install mods` が再び `winmm.dll` 削除エラーを起こさないことも確認します。

### scripts_proxy installer依存の訂正 — 2026-10-31

Overstrike1.8.1.0公開tagのSetupScriptsは `File.Copy("scripts_proxy.dll", <game>/winmm.dll)` を使用します。コピー元はprocess CWD相対で、exe隣へ絶対固定したコードではありません。この実機の例外は `<game>/Overstrike/scripts_proxy.dll` を要求していました。通常ファイル・非reparseのそのコピー元を許容し、rootの2ファイルも維持します。`Overstrike/winmm.dll` の既存拒否・限定退避は維持します。sourceとloaderの新規hash一致条件、DLLの自動取得・コピー・削除は追加しません。

共通 `environment_ready()` の同じ混同も訂正したのでStart/Open/Playに適用されます。Start CMD自体はwinmmだけを退避し、scripts_proxyを削除しないため変更しません。既存ReadOnly正規化はrootファイルだけです。実際のコピー元がexe隣なら、その属性も別に影響し得るため、root正規化だけでInstall成功を保証する説明はしません。Profile/script-support設定は変更しません。

---

<a id="play"></a>

## MSM2 - Play Modded 仕様書

対象: Mod-Tools v1.1.0 / 2026-10-24。配置・設定の共通仕様は
[仕様書ライブラリ](README.md)を参照。

### 目的・入出力

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

### 前提と処理順序

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

### 起動引数と起動済みの場合

`commandline.txt` があり、空白で分割した中に `-scripts` があれば引数を追加しません。
PowerShellの比較は大文字小文字を区別しないので `-ScRiPtS` も認識します。
それ以外は `-scripts` を1つ指定してゲームを開始します。
このツール自身が `commandline.txt` 全体を読み直して引数へ再追加する処理はありません。
ファイル自体の内容も保持します。

同じゲームパスの既存プロセスがあれば前面化し、新しいゲームは起動しません。
同名の別パス/パス不明は拒否します。前面化できない場合も二重起動せず失敗します。
PowerShellの共有事前検査はOverstrike側のプロセス実行パスも確認するため、
別場所のOverstrikeが動いている場合も停止します。プロセスの強制終了はありません。

### 復元後の確認フラグ

Restore適用は、ファイル復元後に `restore_requires_install=true` を設定します。
Vanillaへファイルを戻しただけではゲームアーカイブへのMOD再適用が終わらないためです。
共有Overstrikeの `Install mods` が成功してから `INSTALLED` と入力します。
自動処理からの `--confirm-installed` も、利用者がその完了を明示するための指定です。

これは人の申告であり、ツールがOverstrikeのインストール完了を解析する機能ではありません。
現在はフラグを保存してから起動要求を送るため、起動要求が後で失敗してもフラグはfalseのままです。
この順序を変更する場合、二重起動対策と失敗時の設定保全を併せて検討します。

### 改変箇所と確認項目

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

---

<a id="backup"></a>

## MSM2 - Backup Mods 仕様書

対象: Mod-Tools v1.1.0 / 2026-10-24。配置・設定の共通仕様は
[仕様書ライブラリ](README.md)を参照。

### 目的・入出力

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

### 保存範囲

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

### 保存処理の順序

1. ゲームと出力先を検証。出力先はゲーム内・ゲームの祖先フォルダーを拒否。
2. 共通ロック取得後、ゲーム/共有Overstrikeが終了していることを確認。
3. 共有環境、独立Git、導入済み設定のformatを確認し、対象の一覧/メタデータを採取。
   リンク等、外部Git依存、`index.lock` / `Update-WS.lock`、非通常ファイルを拒否。
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

### 出力形式・互換性

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

### 付属の検証・復元

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

### 改変箇所と確認項目

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
