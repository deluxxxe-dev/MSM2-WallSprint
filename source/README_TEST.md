# WallSprint v0.1c20 — ローカル開発環境の移行

Package **0.1.38**。今回は配置・更新・ログ保存先の変更です。c19 のゲーム動作と安全ゲートを維持し、準備済みの EDI 研究はゲーム側で無効のまま保留します。Position・source・入力・移動の書き換えはありません。

## 最初の移行

配布 **MSM2-WS-v0.1c20.zip** を **Mods Library の外**へ展開してください。展開したフォルダ内の `Migrate-WS.cmd` と `tools` を一緒に使います。ソース ZIP は別の再現用資料で、正本 `_source` はヘルパーが GitHub から独立 clone します。

Windows 側に Python 3.11+、Git for Windows、PowerShell、設定済み WSL2 が必要です。WSL 側には既存の MinGW/Clang/Wine/解析 Python のフルビルド環境が必要です。**前提が足りない場合、ビルドや試験を省略して導入することはありません。** 手順は [source/BUILD.md](source/BUILD.md) を参照してください。

共有の実行ファイルと依存 DLL が次の位置にあることを確認してください。AppData の別コピーは使いません。ヘルパーは共有バイナリをコピー・移動・修復しません。

```text
<ゲームルート>\Spider-Man2.exe
<ゲームルート>\scripts_proxy.dll
<ゲームルート>\winmm.dll
<ゲームルート>\Overstrike\Overstrike.exe
```

`Overstrike` フォルダ内には `winmm.dll`／`scripts_proxy.dll` を置かないでください。別の場所の Overstrike が実行中なら手動で閉じてください。

展開先でコマンドプロンプトを開き、`<実際のゲームルート>` を実在する場所に置き換えて実行します。

```bat
Migrate-WS.cmd --game-root "<実際のゲームルート>" --branch codex/v01c20-edi-value-provenance
```

ヘルパーはゲーム／共有 Overstrike を停止し、次の場所を作成・検証して、独立 clone、fast-forward 更新、既存の全ビルド／試験、パッケージ検証、有効版の入れ替えを実行します。失敗した場合、以前の有効な WS ビルドを可能な限り維持・復元します。旧 D ドライブ環境、SF、通常 MOD は削除・整理しません。

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\
├─ MSM2-WS-v0.1c20.script
├─ Update-WS.cmd
├─ Migrate-WS.cmd        （移行用）
├─ logs\
├─ _source\             （独立 Git clone）
└─ _archive\            （旧版は .script.disabled）
```

`_source` にインストール ZIP を展開しないでください。Git checkout、`logs`、`_archive` の下に有効な `.script` を置いてはいけません。WS 全体で有効な `.script` はルートの1本だけです。

## 通常の更新

以後は、上の **`MSM2-WS\Update-WS.cmd` をダブルクリック**します。個人の C/D パスや旧 clone へのリンクには依存しません。ブランチを意図的に変えるときは、正しい upstream を設定したうえで明示的な `--expected-branch <新しい開発ブランチ>` を使います。dirty tree、detached HEAD、分岐した履歴、worktree／junction／symlink／外部 Git オブジェクト依存はエラーで止まります。

起動するのは **`<game>\Overstrike\Overstrike.exe`** だけで、WorkingDirectory は必ずゲームルートです。共有 UI で通常 MOD＋SF＋WS を必要に応じて選び、**Install mods → Run modded game** を実行してください。

クラウドでは実 PC の移行完了や実 Overstrike UI の認識を確認していません。ローカル検証と共有 UI の認識、新しい実行ログを確認するまでは旧環境を残してください。旧環境を自動削除する処理はありません。

## 実機ログと回帰試験

起動後、新しいログが次に作られることを確認してください。

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs\
MSM2-WS-v0.1c20-YYYYMMDD-HHMMSS.log
```

時刻は PC のローカル時刻です。同秒の衝突は `-01` などを付け、既存ファイルを上書きしません。同じ起動内の A→E は1本に記録します。旧 `log_wallsprint` や共有 `logs` への代替保存はありません。

1. A/cardinal の平らな壁で約4秒 WallRun。
2. WallRun を完全に終了。
3. 移動またはファストトラベル。
4. E/diagonal で約4秒 WallRun。
5. 新しい WS `logs` の最新の完全な実行ログを、非公開で ChatGPT に送ってください。

通常の入力で試してください。A/E の対応、ちらつき・停止・クラッシュなどがあれば添えてください。動画は見た目の異常時だけです。生ログや private game body を GitHub へ追加しないでください。

確認項目は、v0.1c20/package0.1.38、正確なビルドの admission、Deinit／retention／Surface Up、stale／hard kill なし、Position writes0、最終8entry／8map、診断と DR/watch の正常な解除です。今回は EDI 研究を進めていないため、その値の由来は c19 の未解決状態のままです。`writable_source_proven=0` を維持します。
