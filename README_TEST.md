# WallSprint v0.1b7 — Pre-Armed Reset Writer Capture

Package **0.1.16**。姿勢を戻す writer/caller を特定する診断版です。見た目の保持はまだ完成条件ではありません。**RESET_WRITER_VERIFIED=0、実機 A/E 未確認**。

## 導入

1. ゲームを完全に終了し、古い WallSprint を無効にする。
2. **WallSprint_v0.1b7_prearmed_reset_capture.zip** を展開する。
3. 中の **WallSprint.script は解凍せず** Overstrike に追加し、Install mods。
4. これまでと同じ Script support と `-scripts` で起動する。別 SDK・参考 MOD は不要。

対象は Spider-Man2.exe **2.810.0.0**、SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`。別 build は fail closed。配布 INI の変更は不要。

## 操作・提出

録画を開始し、地上で約2秒静止。Control A で左→右の横 WallRun を1回、約1.5秒。離脱して地上で約2秒静止し、同じ既知の斜め Face E で左→右を1回、約2秒。その後ゲームを終了する。Crawl・マーカーキー・繰り返し操作は不要。A/E 間の Fast Travel は可。

`logs/WallSprint.log` と録画、A/E に対応する session 番号、見た目・クラッシュ・Position の異常有無を提出してください。失敗や0件でもログを加工しないでください。実機ログは公開 ZIP に含めません。

F8 は診断と source 変更を停止し再起動まで OFF、F9 は script 停止。再採取はゲームを完全に再起動してください。

## 確認するログ

起動時の `RESET_TRACE_SELF_CHECK` で `exact_build=1 common_writer_hook=1 reset_trace_enabled=1 reset_trace_admitted=1 primary_thread_prearm_supported=1` が必要です。`V01B7_DIAGNOSTIC_NOT_ADMITTED` があれば診断未成立です。

native Init から0.60秒以降の最初の verified Update（common-writer entry も fallback）で既知 primary native thread のみを先に監視します。source 変更開始は従来どおり0.70秒以降。監視は1.10秒まで、または reset 候補・Deinit・identity/guard loss・F8・予算上限で終了します。helper の scheduling で間に合わない場合も source 開始を遅らせず、`PREARM_STATUS first_override=1` の `trace_already_armed_before_override=0` で失敗を明示します。成功条件は同フィールドが1、かつ `RESET_TRACE_EVENT` が1件以上。

`COMMON_WRITER_TRACE` は current Hero Transform 宛ての全 caller を記録し、`source_valid=0` の RDX を transform と解釈しません。記録予算・競合による欠落は dropped として報告し、capture を停止します。

`RESET_WRITER_CANDIDATE=1` は候補、`RESET_PATH_CORRELATED=1` は既知 common writer の thread/entry time/target/caller/post-RSI/source の照合です。実際の post-store RIP、命令の書込先と関数 SHA は private log の offline decode で確認します。watch address は実効書込命令の先頭アドレスとは限りません。A/E 両方で同じ writer/caller が再現するまで VERIFIED にしません。

source48byte の既存変更だけを維持し、Position・direct Actor・reset patch/skip・adhesion/movement/camera/animation/physics は追加しません。
