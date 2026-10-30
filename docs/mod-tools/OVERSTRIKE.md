# MSM2 - Overstrike 仕様書

対象: Mod-Tools v1.1.0 / 2026-10-29。配置・設定の共通仕様は
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

## 起動前の限定自己修復

インストール済み `Start-Overstrike.cmd` はPythonの環境検査より前に、共有環境で実際に確認された次の3ケースだけを限定的に処理します。

1. `<game>/Overstrike/winmm.dll` が誤配置されている場合、SHA256で識別して `<game>/Mod-Tools/_quarantine` へ退避する。同一bytesが既に退避済みなら誤配置側だけを削除する。
2. ゲームルートの `scripts_proxy.dll` または `winmm.dll` がReadOnlyの場合、その属性だけを解除して再確認する。ファイルbytesは変更しない。
3. `<game>/.MSM2-Mod-Tools.lock` が残っていて、その中のPIDが現在存在しない場合だけstale lockとして削除する。PIDが生きている場合は保持する。

このReadOnly正規化はrootの2ファイルに限定した属性修復です。Overstrike1.8.1.0のコピー元 `scripts_proxy.dll` はprocess CWD相対なので、実際のコピー元がOverstrike配下ならそのReadOnly属性はこのroot修復では変わりません。コピー元がReadOnlyなら再作成されたwinmmへ属性が引き継がれ得ますが、root正規化だけでInstall成功や再発防止を保証しません。

自己修復は上記ローダー/lockだけに限定し、Profiles、Mods Library、SF、WS runtime、通常MOD、asset dataを変更しません。予期しない状態ではcleanup範囲を広げず失敗します。

## 処理順序

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

## 初回・ツール更新時だけ行うこと

`_install()` はゲーム/Overstrikeが終了していることと保存対象の走査を確認し、
`KIT` に列挙された自ツールだけを `<game>/Mod-Tools` へコピーします。
設定に版を保存し、Overstrike / Play / Backupの3つのショートカットをDesktopへ作ります。
既存のバックアップ先・選択プロファイル・復元確認フラグは保持します。
バックアップ先が未設定ならホームの `MSM2-Mod-Backups` を登録しますが、この段階では保存しません。

導入済みの通常起動は、全MODのバックアップ走査や再コピーを行いません。
自動アップデートはなく、GitHub上の新版は利用者が取得して初回と同様に導入します。
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

`Install mods` が、更新済みlauncherで両root loaderのReadOnlyを解除した後にも `winmm.dll` の `UnauthorizedAccessException` で失敗する場合は、同じInstallを繰り返さず、active file lockまたはWindows ACLを次に調査します。

## 改変箇所と確認項目

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

## scripts_proxy installer依存の訂正 — 2026-10-31

Overstrike1.8.1.0公開tagのSetupScriptsは `File.Copy("scripts_proxy.dll", <game>/winmm.dll)` を使用します。コピー元はprocess CWD相対で、exe隣へ絶対固定したコードではありません。この実機の例外は `<game>/Overstrike/scripts_proxy.dll` を要求していました。通常ファイル・非reparseのそのコピー元を許容し、rootの2ファイルも維持します。`Overstrike/winmm.dll` の既存拒否・限定退避は維持します。sourceとloaderの新規hash一致条件、DLLの自動取得・コピー・削除は追加しません。

共通 `environment_ready()` の同じ混同も訂正したのでStart/Open/Playに適用されます。Start CMD自体はwinmmだけを退避し、scripts_proxyを削除しないため変更しません。既存ReadOnly正規化はrootファイルだけです。実際のコピー元がexe隣なら、その属性も別に影響し得るため、root正規化だけでInstall成功を保証する説明はしません。Profile/script-support設定は変更しません。
