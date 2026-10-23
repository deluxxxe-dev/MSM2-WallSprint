# WallSprint v0.1c5 — Sustained Native Lifecycle

Package **0.1.23**。c4 では姿勢保持が成功しましたが、WallRun 中の約2秒で姿勢変更が終了していました。今回は native 更新の publication と鮮度判定を修正し、実際の Deinit まで続くかを確認します。

## ダウンロード・導入

配布は **WallSprint_v0.1c5_sustained_lifecycle.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得してください。ゲームを完全終了し、古い WallSprint を無効にして ZIP を展開します。中の **WallSprint.script は解凍せず** Overstrike へ追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 起動確認

ゲームのインストール先の `logs\WallSprint.log` を確認します。

```text
WallSprint v0.1c5 package_version=0.1.23
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1 caller_verified=1 writer_verified=1 rdx_to_rcx_map_verified=1 source_bytes=0x24 decoded_prefix_bytes=444 caller_decoded_bytes=128 pairs=9
DUAL_SOURCE_ADMISSION admitted=1
```

proof / admission が失敗した場合は WallRun 試験を進めず、そのログを送ってください。

## 今回の試験

**c4 で約3.9秒走れた、同じ大きな A 方向の平らな壁**で、左→右へ約3〜4秒、連続で WallRun を1回行ってください。前回の法線はほぼ `(0,0,+1)` でした。Crawl、マーカー操作は不要です。

確認するのは次の3点です。

- Surface Up へ姿勢が回転するか。
- 約2秒で直立へ戻らず、WallRun の終わりまで回転を保つか。
- クラッシュ・位置ずれ・破損がないか。

ゲームを終了し、**完全な WallSprint ログを非公開のまま通常どおり ChatGPT に送ってください**。上の見た目の結果も添えてください。今回は E の試験は不要です。

## 成功条件

```text
NATIVE_SOURCE_OVERRIDE ... elapsed=（2秒より後にも記録）
PATH_B_COMPACT3X3_OVERRIDE ... elapsed=（2秒より後にも記録）
ORIENTATION_RETENTION_PASS=1
ORIENTATION_LIFECYCLE_SUMMARY ... source_exit_reason=verified_native_Deinit source_active_until_deinit=1 verified_deinit=1 ... retention_pass=1
```

summary には WallRun 時間、source 開始時刻、A/B 回数、最長保持時間・更新数、最後の active-source Actor Up dot と観測時刻も出ます。単に一度 retention PASS が出ただけでは今回の成功にはしません。

途中で終了した場合は `NATIVE_LIFECYCLE_EXIT_DIAG` が、失敗した sub-gate、token/context、鮮度、owner/Hero/Transform、native collision epoch を1回記録します。その後に本当の Deinit が来ても、早期終了なら summary の `source_active_until_deinit=0` が残ります。ログを切り取る必要はありません。

c4 の複数スレッド監視は約1.50秒で終了します。姿勢変更はその監視期間とは独立しています。126件を復元した c4 と同様、今回も `MULTITHREAD_UP_WATCH_RESTORE ... cleanup_pending=0` を確認します。監視の cap や skip はあり得ます。未知 writer の捕捉は今回の必須条件ではありません。

**F8 は再起動まで書き込み・監視 OFF、F9 は script 停止**。再試験は完全再起動してください。既存の source 開始後5秒・各 path4096回の上限は維持しています。

`THIRD_WRITER_CODE ... private_do_not_distribute=1` は私的な命令記録です。生ログやコード列を GitHub・公開 ZIP に追加する必要はありません。この版では adhesion / movement / animation / camera は追加していません。
