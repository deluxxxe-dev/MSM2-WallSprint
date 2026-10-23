# WallSprint v0.1c8 — Exact Hard-Kill Attribution

Package **0.1.26**。c7 は実機で姿勢変更が2回の書き込み後に止まりました。c8 は**最初に止めた条件を記録**し、診断用 field plan だけの停止では姿勢保持を終了しないようにしています。実際の hook・コード・対象の不一致は引き続き停止します。**Position は読み取り専用です。** 実機での修正確認はこれからです。

## ダウンロード・導入

配布は **WallSprint_v0.1c8_kill_cause_attribution.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得し、試験手順は GitHub 上のこの README_TEST.md を開いてください。

ゲームを完全終了し、古い WallSprint を無効にして ZIP を展開します。中の **WallSprint.script は解凍せず** Overstrike へ追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 起動確認

ゲームのインストール先の `logs\WallSprint.log` を確認します。

```text
WallSprint v0.1c8 package_version=0.1.26
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1 ... pairs=9
DUAL_SOURCE_ADMISSION admitted=1
PATH_A_POSITION_PROOF admitted=0 ... position_source_write=0
OBSERVATION_PLAN_AUTH_STATUS ... verified_available=1 field_plan_enabled=1 ... source_integrity=1 diagnostic_viable=1
NATIVE_SOURCE_GUARD_STATUS admitted=1 first_cause=none ...
```

Position proof の admitted=0 は想定どおりです。Path B proof／dual-source admission が失敗した場合は WallRun 試験を進めずログを送ってください。

## 今回の試験

**同じ大きな A 方向の平らな壁**で、左→右へ**約4秒の連続 WallRun を1回**行ってください。処理落ちを意図して起こす必要はありません。Crawl、マーカー操作、E の試験は不要です。

確認する点：

- 約0.7秒から姿勢が回転し始めるか。
- 壁に沿った Surface Up まで回転し、WallRun 終了まで続くか。
- 一瞬の直立、姿勢の飛び、回転のやり直し、クラッシュや位置破損がないか。

ゲームを終了し、**起動部分を含む完全な WallSprint ログを、非公開のまま通常どおり ChatGPT に送ってください**。見た目の結果も添えてください。回転しない場合も、今回の原因記録が必要なのでログを残してください。

## 確認するログ

停止条件が発生した場合：

```text
OBSERVATION_PLAN_AUTH_FAILURE ... verified_available=... field_plan_enabled=... failed_hook=... ..._read_ok=... ..._patch_match=... ..._first_difference=...
NATIVE_SOURCE_HARD_KILL ... reason=... origin=... thread_id=... ... source_failed_gate=...
```

`NATIVE_SOURCE_HARD_KILL` はプロセス内の最初の原因だけを保持します。F8・script 終了はそれぞれ明示的な理由で記録し、先行する異常原因を上書きしません。hook ごとの差分位置は32が「差分なし／読み取り不可時の未判定」を表し、read_ok／patch_match と合わせて確認します。完全な関数バイト列は追加しません。

正常に保持できた場合：

```text
ORIENTATION_RETENTION_PASS=1
NATIVE_SOURCE_CLOSE ... reason=verified_Deinit normal_session_close=1 source_active_until_deinit=1
ORIENTATION_LIFECYCLE_SUMMARY ... source_exit_reason=verified_native_Deinit source_active_until_deinit=1 verified_deinit=1 ... retention_pass=1 ... first_hard_kill=none
ADHESION_SUMMARY ... corrections_applied=0 ... source_position_writes=0 ... mode=position_read_only
```

100msを超える Update の空白が自然に起きた場合は、c7 と同じ `NATIVE_SOURCE_STALE_SUSPEND` → `NATIVE_SOURCE_STALE_RESUME ... transition_restarted=0` を確認します。空白なし・件数0でも正常です。停止中は A/B 書き込みをせず、同じ対象の新しい実 Update で姿勢を引き継ぎます。

既存の複数スレッド診断監視は約1.50秒で終了し、姿勢保持とは独立しています。`MULTITHREAD_UP_WATCH_RESTORE ... cleanup_pending=0` も確認します。**F8 は再起動まで書き込み・監視 OFF、F9 は script 停止**。再試験は完全再起動してください。source 開始後5秒・各 path4096回の上限は維持しています。

Position 補正、movement／animation／camera／gravity は追加していません。生ログ、動画、ゲーム関数の命令列を GitHub・公開 ZIP に追加する必要はありません。
