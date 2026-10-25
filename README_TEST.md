# WallSprint v0.1c31 — exact VMOVUPSの形式・アドレス診断

Package **0.1.49**。c30実機でqwordの不明依存上限0x7を維持した解析が進み、`0x2CF7FF0: VMOVUPS`で停止したことを確認済みです。今回はこの命令の幅・operand/access・VEX/EVEX・メモリの読書き方向・親状態のアドレス/alias/tags・上位レーン効果を分類します。実機の正確な形式はまだ不明です。ストアは拒否し、XMMのみの現モデルで表せないYMM/上位レーンも拒否します。ベクトルload規則やtag0は追加しません。c30のRAX上限0x7と、値を読まない規則・全安全条件を維持します。

## 今回の更新・テスト（移行済み環境）

1. ゲームと共有Overstrikeを閉じ、`MSM2-WS\Update-WS.cmd` を実行します。Git更新・既存フル試験・検証・配置を行います。
2. 起動した共有Overstrikeで通常MOD＋SF＋WSを確認し、`Install mods` → `Run modded game`。
3. A/cardinalの平らな壁で約3秒 → WallRunを完全終了 → 移動/ファストトラベル → E/diagonalで約3秒。各回は既存の5秒上限に達する前に終了します。
4. `MSM2-WS\logs` の最新の完全な `MSM2-WS-v0.1c31-YYYYMMDD-HHMMSS.log` を非公開で共有してください。動画は見た目の異常時だけです。

新しい `MOVEMENT_CALLEE_READ_VECTOR` / `READ_VECTOR_OPERAND` / `READ_VECTOR_ADDRESS` / `READ_VECTOR_ARCH` と既存のUNKNOWN_LOAD/停止点/CFG/safety記録を確認します。decoderのaccessとエンコーディングで照合したaccessを分け、ストアを誤ってload扱いしません。GPR・アドレス・XMM下位128bitのtagsと、表現されていない上位レーンを区別します。値そのものは取得・公開・数値解釈・再参照しません。1経路の診断から全target・所有権・書込み許可を導きません。

下の移行手順は未移行環境向けの参照です。デスクトップの `MSM2 - Overstrike` は普段の起動用で、WSの更新・ビルドは実行しません。

## 最初の移行

配布 **MSM2-WS-v0.1c31.zip** を **Mods Library の外**へ展開してください。展開したフォルダ内の `Migrate-WS.cmd` と `tools` を一緒に使います。ソース ZIP は別の再現用資料で、正本 `_source` はヘルパーが GitHub から独立 clone します。コードはすべて取得し、配布アーカイブだけを Git の sparse checkout 設定で作業フォルダから除外します。ZIP本体と履歴はGitに残ります。

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
├─ MSM2-WS-v0.1c31.script
├─ Update-WS.cmd
├─ Migrate-WS.cmd        （移行用）
├─ logs\
├─ _source\             （独立 Git clone）
└─ _archive\            （旧版は .script.disabled）
```

`_source` にインストール ZIP を展開しないでください。Git checkout、`logs`、`_archive` の下に有効な `.script` を置いてはいけません。WS 全体で有効な `.script` はルートの1本だけです。**OverstrikeはZIP/RAR/7zの中身も読み込むため、ZIPを展開しなくても重複します。** 保管用ZIPは `.zip.disabled` で保持し、配布用の通常ZIPは Mods Library の外に置きます。

## WallSprintが複数表示される場合

旧版ZIPが `_source` や `_archive/_build` にあると、ZIP内のWallSprintも一覧に並びます。修正版はGitの履歴を保持したままソースのZIPをチェックアウト対象から除外し、確認済みの旧WSビルドZIPだけを無効な拡張子へ移します。旧D環境・SF・通常MODは整理対象にしません。

準備用checkoutを更新し、そこにある `Migrate-WS.cmd` を同じゲームルート／開発ブランチで再実行してください。修正前のインストール済みヘルパーではなく、更新済みの準備用ヘルパーを使います。終了後、共有Overstrikeの一覧にWallSprintが**1件だけ**あることを確認してから `Install mods` へ進みます。SFと通常MODの同時表示、新しい実機ログも引き続き確認が必要です。

## 移行中のreset試験で停止した場合

`immediate_reset_smoke.cpp` のassertionで停止したら、フル移行を繰り返す前に、更新済みの準備用checkoutの `source/tools/windows/reset-diagnostic.sh` をWSLで実行します。Linuxの一時フォルダでビルドと関連試験、短い監視試験の5回反復だけを行い、ゲームには配置しません。

`RESET_SMOKE_ARM_FAILURE` は開始失敗、`RESET_SMOKE_FAILURE` は開始後の捕捉失敗を示します。失敗行から後の出力を共有してください。標準出力・エラーは表示された診断フォルダの `logs/reset-diagnostic-console.log` に残ります。30msの制限や既存のassertionは維持します。今回の実機記録では、スレッド一覧取得の段階で約44.7msかかり、30msの期限に間に合っていませんでした。修正版の正常系試験は実OSの一覧取得を計測前に済ませ、期限後の開始を拒否する試験も別途必須にしています。診断成功だけで移行完了にはなりません。

## 通常の更新

以後は、上の **`MSM2-WS\Update-WS.cmd` をダブルクリック**します。個人の C/D パスや旧 clone へのリンクには依存しません。ブランチを意図的に変えるときは、正しい upstream を設定したうえで明示的な `--expected-branch <新しい開発ブランチ>` を使います。dirty tree、detached HEAD、分岐した履歴、worktree／junction／symlink／外部 Git オブジェクト依存はエラーで止まります。

起動するのは **`<game>\Overstrike\Overstrike.exe`** だけで、WorkingDirectory は必ずゲームルートです。共有 UI で通常 MOD＋SF＋WS を必要に応じて選び、**Install mods → Run modded game** を実行してください。

既存環境の実機移行・共有UI・起動・新ログは受け入れ済みです（analysis/LOCAL_WORKSPACE_RUNTIME_ACCEPTANCE_20261024.md）。今回のVMOVUPS診断版の実機A/Eは別途未確認です。新規の環境で移行する場合はその環境の確認が必要で、旧環境を自動削除する処理はありません。

## 実機ログと回帰試験

起動後、新しいログが次に作られることを確認してください。

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs\
MSM2-WS-v0.1c31-YYYYMMDD-HHMMSS.log
```

時刻は PC のローカル時刻です。同秒の衝突は `-01` などを付け、既存ファイルを上書きしません。同じ起動内の A→E は1本に記録します。旧 `log_wallsprint` や共有 `logs` への代替保存はありません。

1. A/cardinal の平らな壁で約3秒 WallRun。
2. WallRun を完全に終了。
3. 移動またはファストトラベル。
4. E/diagonal で約3秒 WallRun。
5. 新しい WS `logs` の最新の完全な実行ログを、非公開で ChatGPT に送ってください。

通常の入力で試してください。A/E の対応、ちらつき・停止・クラッシュなどがあれば添えてください。動画は見た目の異常時だけです。生ログや private game body を GitHub へ追加しないでください。

確認項目は、v0.1c31/package0.1.49、exact-build admission、VMOVUPSの正確な形式/幅/方向/上位レーンと親状態alias、拒否理由、c30 RAX上限0x7/値読取り0、JA両経路・Deinit/retention/Position0・watch解除です。A/E各約3秒、既存5秒上限未満で終了します。生コード・ベクトル値・qwordは不要です。`writable_source_proven=0` を維持します。
