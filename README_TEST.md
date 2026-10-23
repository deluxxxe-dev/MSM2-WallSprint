# WallSprint v0.1c3 — Third Reset Writer Multithread Trace

Package **0.1.21**。c2 で確認できた A/B の姿勢変更を維持し、姿勢を World Up に戻す書き込み元を調べる版です。ちらつきはまだ発生する見込みです。

## ダウンロード・導入

配布は **WallSprint_v0.1c3_third_writer_trace.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得してください。ゲームを完全終了して古い WallSprint を無効にし、ZIP を展開します。中の **WallSprint.script は解凍せず** Overstrike に追加して Install mods。従来の Script support と `-scripts`、配布 INI をそのまま使います。

## 起動確認

ゲームのインストール先の `logs\WallSprint.log` を確認します。

```text
WallSprint v0.1c3 package_version=0.1.21
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1 caller_verified=1 writer_verified=1 rdx_to_rcx_map_verified=1 source_bytes=0x24 decoded_prefix_bytes=444 caller_decoded_bytes=128 pairs=9
DUAL_SOURCE_ADMISSION admitted=1
RESET_TRACE_MODE ... multithread=1 ... max_threads=128
```

proof が失敗した場合は WallRun 試験を進めず、`PATH_B_PROOF_DECODE_FAILURE` と `ORIENTATION_ADMISSION` を送ってください。

## 今回の試験

**まず A Control だけ**、同じ平らな壁を左→右へ約1.5〜2秒 WallRun します。Crawl、マーカー操作は不要です。ゲームを終了し、ログを通常どおり ChatGPT に送ってください。

A のログに `THIRD_WRITER_EVENT` がない場合だけ、完全再起動して同じ既知の斜め E 壁でも1回試します。左右への姿勢の切り替わりが見えたか、クラッシュ・位置ずれがあったかも添えてください。動画は補助で、ログを優先します。

## 確認するログ

```text
PATH_A_OVERRIDE_COUNT=（1以上） ... PATH_A_STATE_LOCK_CONTENTION=0 ... PATH_B_OVERRIDE_COUNT=（1以上）
MULTITHREAD_UP_WATCH ... installed=（2以上） ... complete_thread_coverage=...
THIRD_WRITER_EVENT ... thread_id=... writer_rva=... caller_rva=... caller2_rva=...
THIRD_WRITER_IDENTITY ... function_sha256=...
MULTITHREAD_UP_WATCH_RESTORE ... cleanup_pending=0
```

primary thread は約0.60秒から先に監視し、約0.85秒でプロセス内の安全に扱えるスレッドへ一度だけ拡張、1.50秒で終了します。foreign DR、アクセス失敗、128-thread 上限、記録 drop はログへ明示します。`complete_thread_coverage` は取得したスナップショットの対象範囲についての値で、後から生まれるスレッドは含みません。

`UP_WRITE_CLASSIFICATION` は KNOWN_PATH_A / KNOWN_PATH_B / UNKNOWN_WRITER を区別します。`THIRD_WRITER_EVENT` は、A/B 両方の変更後に Surface Up から World Up 方向へ戻った最初の記録です。同じ既知関数の別 caller／thread が原因でも記録します。これは調査資料で、この版では第三の経路を書き換えません。

scalar store の途中で Up が一時的に不完全になる場合、5 ms 以内の直前 Surface Up 観測を使い、その由来を `before_semantics` に明記します。原子的な命令直前値を取得したという意味ではありません。

`ORIENTATION_RETENTION_PASS=1` は従来どおり 300 ms または20 distinct native updates の保持が必要です。短く Surface Up が見えただけでは成功扱いにしません。

**F8 は再起動まで全書き込み・監視 OFF、F9 は script 停止**。再試験は完全再起動してください。

`THIRD_WRITER_CODE ... private_do_not_distribute=1` は次の解析用の私的な命令記録です。ログは非公開のまま送ってください。生ログやこのコード列を GitHub・公開 ZIP に追加する必要はありません。
