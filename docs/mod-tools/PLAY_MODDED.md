# MSM2 - Play Modded 仕様書

対象: Mod-Tools v1.1.0 / 2026-10-24。配置・設定の共通仕様は
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
4. 共有Overstrike、2つのローダーとその配置、Mods Library、独立Git保存を確認する。
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
