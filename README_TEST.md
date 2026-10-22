# WallSprint v0.1b6 — 姿勢を戻すwriterの短時間記録

Package **0.1.15**。v0.1b5で姿勢sourceが実Actorへ届くことは確認できました。今回は、その直後にUpをWorld Upへ戻す書き込みを調べます。姿勢が一瞬変わって戻っても、そのままログを提出してください。保持を直す版ではありません。

## 導入

1. ゲームを完全に終了する。
2. **WallSprint_v0.1b6_reset_writer_trace.zipを展開**する。
3. 古いWallSprintを無効にする。
4. 中の**WallSprint.scriptは解凍せず**Overstrikeへ追加し、Install mods。
5. これまでと同じScript supportと`-scripts`で起動する。

対象: Spider-Man2.exe **2.810.0.0** / SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`。参考MOD・WSU・別SDKは不要です。

## 操作 — AとEで各1回、Crawl不要

録画を開始。マーカーキーは不要です。

1. **地上で約2秒静止**する。
2. 前回と同じ**Control A**で、**左→右の横WallRunを1回、約1.5秒**。
3. 離脱して**地上で約2秒静止**する。
4. 従来と同じ斜めの**Face E**で、**左→右の横WallRunを1回、約2秒**。
5. ゲームを終了する。

Crawl、上方向WallRun、何度も繰り返す操作は不要です。A→E間のFast Travelは構いません。失敗や余計な入力が入っても、まずそのログを提出してください。

0.7秒後からのsource変更はv0.1b5方式のままです。十分に傾いた最初の変更から**最大30msの取得区間**だけUpへの書き込みを監視し、戻る書き込みを捕捉したら終了します。Camera、吸着、移動、アニメーションは追加しません。通常Sprint姿勢が維持されることは、まだ完成条件として要求しません。

## 停止・提出

異常があれば**F8でsource変更と監視を停止**。F9はscript停止です。再採取はゲームを完全に再起動してください。

**logs/WallSprint.logと録画**をChatGPT側へ提出してください。Codexへはログと、A/Eのsession対応・見た目の確認結果で十分です。

主なログ: `RESET_TRACE_CONFIG`、`RESET_TRACE_WINDOW`、`RESET_TRACE_EVENT`、`RESET_ENTRY_SOURCE`、`RESET_WRITER_CANDIDATE`。監視開始が間に合わない場合は`RESET_TRACE_SKIPPED`となります。0件でも加工せず提出してください。捕捉0件だけでは「戻すwriterが存在しない」と判断しません。

設定は配布済みで変更不要です。`WALLSPRINT_ENABLE=1`と`WALLSPRINT_NATIVE_SOURCE_OVERRIDE=1`で既存の限定source変更を使用。両方を0にすると停止します。direct Actor/Position・adhesion/movement設定は0です。常時監視やreset writerの改変は行いません。

**v0.1b6のreset writer特定は今回の実機テスト待ち**です。クラウドの独立テストをゲーム内成功として扱いません。
