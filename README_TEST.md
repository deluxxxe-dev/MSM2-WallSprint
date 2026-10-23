# WallSprint v0.1c4 — Real-Game Thread Enumeration Fix

Package **0.1.22**。c3 で追加スレッドの監視が始まらなかった問題を修正・診断する版です。A/B の姿勢変更は維持します。ちらつきの解消と姿勢保持は実機確認待ちです。

## ダウンロード・導入

配布は **WallSprint_v0.1c4_real_thread_enum_fix.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得してください。ゲームを完全終了し、古い WallSprint を無効にして ZIP を展開します。中の **WallSprint.script は解凍せず** Overstrike に追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 起動確認

ゲームのインストール先の `logs\WallSprint.log` を確認します。

```text
WallSprint v0.1c4 package_version=0.1.22
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1 caller_verified=1 writer_verified=1 rdx_to_rcx_map_verified=1 source_bytes=0x24 decoded_prefix_bytes=444 caller_decoded_bytes=128 pairs=9
DUAL_SOURCE_ADMISSION admitted=1
RESET_TRACE_MODE ... multithread=1 ... max_threads=128
```

proof が失敗した場合は WallRun 試験を進めず、そのログを送ってください。

## 今回の試験

**まず c3 と同じ広めの near-A 壁で1回だけ**、左→右へ約1.5〜2秒 WallRun します。前回の法線は約 `(-0.113317,0,+0.993559)` で、厳密な +Z 壁ではありません。Crawl、マーカー操作は不要です。

ゲームを終了し、**完全な WallSprint ログを非公開のまま通常どおり ChatGPT に送ってください**。ちらつき・クラッシュ・位置ずれの有無も添えてください。

**追加スレッドの監視が成功し、それでも未知 writer を捕捉できなかった場合だけ**、完全再起動して既知の斜め E 壁を1回試します。列挙自体に失敗した場合は、先にそのログを解析します。

## 確認するログ

最低限の c4 成功条件です。各行には他のフィールドも含まれます。

```text
PATH_A_OVERRIDE_COUNT=（1以上） ... PATH_B_OVERRIDE_COUNT=（1以上）
MULTITHREAD_EXPAND_ATTEMPT ... trigger_seen=1
MULTITHREAD_ENUM_RESULT ... entries_total=（2以上） owner_pid_matches=（2以上）
MULTITHREAD_UP_WATCH ... process_threads_seen=（2以上） eligible=（1以上） installed=（2以上） ... expand_attempted=1
MULTITHREAD_UP_WATCH_RESTORE ... cleanup_pending=0
```

`THIRD_WRITER_EVENT` と `THIRD_WRITER_IDENTITY` があれば、残るリセット元を絞れた強い成果です。今回の列挙修正の最低条件には含めません。

失敗時は `MULTITHREAD_EXPAND_STATE` の `stop_reason` と `expand_requested/attempted/completed/failed`、`MULTITHREAD_ENUM_RESULT` / `MULTITHREAD_UP_WATCH_SKIP` の失敗段階と `win32_error_code` を確認します。追加ログを切り取る必要はありません。

主スレッドを約0.60秒から監視し、0.85秒以上を観測した時点でプロセス内の対象へ一度だけ拡張、1.50秒で終了します。列挙前に記録上限へ達しても拡張処理を止めず、拡張後の記録枠を残します。foreign DR、アクセス失敗、128-thread 上限、drop は明示します。`complete_thread_coverage` は取得した対象範囲の値で、後から生まれるスレッドは含みません。

`UP_WRITE_CLASSIFICATION` は KNOWN_PATH_A / KNOWN_PATH_B / UNKNOWN_WRITER を区別します。Up の直前値は前回観測、または5ms以内の Surface 観測で、原子的な命令直前値ではありません。この版では第三の経路を書き換えません。

`ORIENTATION_RETENTION_PASS=1` は従来どおり 300ms または20 distinct native updates の保持が必要です。**F8 は再起動まで書き込み・監視 OFF、F9 は script 停止**。再試験は完全再起動してください。

`THIRD_WRITER_CODE ... private_do_not_distribute=1` は私的な命令記録です。生ログやコード列を GitHub・公開 ZIP に追加する必要はありません。
