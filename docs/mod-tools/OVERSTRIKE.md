# MSM2 - Overstrike 仕様書

対象: Mod-Tools v1.1.0 / 2026-10-24。配置・設定の共通仕様は
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

## 処理順序

1. ゲームルートとEXEを検証し、共通操作ロックを取得。
2. 共有Overstrike、ルートの2つのローダー、Mods Library、独立Git保存を確認。
3. このゲームを対象にしたMSM2プロファイルを選択・確認する。設定は書き換えない。
4. `ws_launch_ready()` でWSの次の条件を確認する。
   - `MSM2-WS/_source/.git` がディレクトリとして存在する。
   - Gitの `index.lock` と `_archive/Update-WS.lock` がない。
   - WS配下にリンク・ジャンクション等、走査不能の場所がない。
   - 有効 `.script` はルートに1個だけで、名前は `MSM2-WS-v<version>.script`。
   - WS配下に可視 `.zip/.rar/.7z` がない（大文字拡張子も対象）。
   - `_source/.git` の内部オブジェクトは、このscript/アーカイブ走査から除外する。
5. `Mod-Tools` が未知の他用途フォルダーなら停止する。
6. バージョン不一致・ファイル不足・元ツールとの差分があれば `_install()` を実行。
7. Windows helperの `manager` actionへ進み、共有Overstrikeを起動/再利用。
8. 成功/失敗を問わず通常の例外処理ではロックを解放する。

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

## 改変箇所と確認項目

| 変更したい内容 | 主な実装箇所 |
|---|---|
| 起動前検査/初回導入 | `start_manager()`、`ws_launch_ready()`、`_install()` |
| パス自動判定/CLI | `main()` の `start` 分岐 |
| 起動・既存ウィンドウ | `Windows.launch()`、windows.ps1の `manager` |
| 名前・アイコン・リンク先 | `Set-ModShortcuts()` と `Start-Overstrike.cmd` |

変更後は新規/旧版からの導入、設定と未コミット作業の保持、通常2回目の非再導入、
重複script/大文字ZIP/各ロック/リンク/ローダー欠落/異なるプロファイルの拒否を確認します。
PowerShell側は正しいEXE/CWD、同一プロセス再利用、別パス拒否、3つの `.lnk` と
同名別アプリ保全を検証します。自動テストは共通READMEから参照してください。
Windows実機では、移動した `.lnk` を含め、既存の普通MOD+SF+WSが同じUIに現れることも確認します。
