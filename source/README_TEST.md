# WallSprint v0.1b9 — Reset-Source Layout Characterization

Package **0.1.18**。Path B は読み取り専用、Path A の姿勢補間は比較用です。**実機のソース形式・保持成功は未確認**。この版の目的はリセット元の特定で、見た目の変化は必須ではありません。

## ダウンロード・導入

配布ファイル: **WallSprint_v0.1b9_path_b_layout.zip**。チャットの直接ダウンロードリンクから取得できます。ゲームを完全終了し古い WallSprint を無効化。ZIP を展開し、中の **WallSprint.script は解凍せず** Overstrike に追加して Install mods。従来の Script support と `-scripts` で起動します。配布 INI の変更は不要です。

対象は Spider-Man2.exe **2.810.0.0**、SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`。別 build や認証不一致では停止します。

## 今回の A / E テスト

1. 地上で約2秒静止し、Control A の壁で左→右の横 WallRun を1回、約1.5秒。
2. 離脱して地上で約2秒静止し、同じ既知の斜め Face E で左→右を1回、約2秒。
3. ゲームを終了。**A/E の session 対応**、クラッシュ・Position 異常の有無を添えて、通常どおり ChatGPT にログを送ってください。Crawl・マーカー操作は不要です。

起動時に `RESET_COPY_HOOK admitted=1 installed=1`、`DUAL_SOURCE_ADMISSION admitted=1`、`PATH_B_LAYOUT_CONFIG ... path_b_override=0 ... path_b_characterization_active=1` を確認します。診断窓は約0.60〜1.50秒。未知の Path B layout で Path A 全体を停止しません。

## 確認する記録

`RESET_COPY_LAYOUT_ENTRY` の raw128 と RCX/RDX/R8/R9/RSP、stack bounds、`rdx_minus_rsp`、`rdx_inside_stack_bounds`、`expected_rsp_relation_match` を分けて確認します。通常の行列とは仮定しません。Path B の書き込みは常に0です。

Path A の `COMMON_SOURCE_OVERRIDE`、遅い未変更呼び出しの `COMMON_OVERRIDE_REJECT`（各 admission 判定付き）、`PATH_B_ENTRY_LINK` と hardware store 記録が比較対象です。prearm未成立・drop・イベント0件は成功扱いにしません。保持成功や吸着の許可は出しません。

起動時の `PATH_B_FUNCTION` / `PATH_B_CODE` は、フック前の caller128bytes / writer444bytes を解析するための私的記録です。**生ログ・完全なゲーム関数を公開 GitHub や配布 ZIP に入れないでください**。追加解析が必要な範囲はこの起動記録と、A/E の elapsed0.60〜1.50にある `RESET_COPY_LAYOUT_ENTRY` / `PATH_B_ENTRY_LINK` / `ORIENTATION_WRITER_EVENT` / `ORIENTATION_WRITER_REGS` です。ChatGPT から Codex へは派生結果を `analysis/V01B9_RUNTIME_RESULT.md` で引き継げます。

**F8 は再起動まで OFF、F9 は script 停止**。再試験はゲームを完全再起動します。direct Actor/Position、adhesion/movement/animation/camera/physics は追加していません。

## 保守者のオフライン解析

Capstone5.0.6 の Python で `tools/review_path_b_layout.py A.log --label A --output A.json`、続いて `tools/review_path_b_layout.py E.log --label E --peer A.json --output E.json`。元のゲームコードを出力へコピーしません。対応オフセット・instruction RVA・スコアは候補としてレビューし、未確定の計算やポインタは UNKNOWN のまま残します。出力から書き込み許可を自動付与する機能はありません。
