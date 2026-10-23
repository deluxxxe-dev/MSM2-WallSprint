# WallSprint v0.1c7 — Transient Freshness Recovery

Package **0.1.25**。c6 で確認された、native Update の一時停止後に姿勢保持が永久終了する問題を修正しました。100msを超える間は書き込みを止め、同じ WallRun の実 Update が再開したら、保持していた姿勢から復帰します。**Position は引き続き読み取り専用です。**

## ダウンロード・導入

配布は **WallSprint_v0.1c7_transient_freshness_recovery.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得してください。試験手順は GitHub 上のこの README_TEST.md を開いてください。

ゲームを完全終了し、古い WallSprint を無効にして ZIP を展開します。中の **WallSprint.script は解凍せず** Overstrike へ追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 起動確認

ゲームのインストール先の `logs\WallSprint.log` を確認します。

```text
WallSprint v0.1c7 package_version=0.1.25
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1 ... decoded_prefix_bytes=444 caller_decoded_bytes=128 pairs=9
DUAL_SOURCE_ADMISSION admitted=1
PATH_A_POSITION_PROOF admitted=0 ... position_source_write=0
SOURCE_LIFECYCLE_CONTRACT ... native_update_freshness_ms=100 ... stale_policy=suspend_until_same_identity_real_Update
```

Position proof の admitted=0 は想定どおりです。Path B proof／dual-source admission が失敗した場合は WallRun 試験を進めずログを送ってください。

## 今回の試験

**c5/c6 と同じ大きな A 方向の平らな壁**で、左→右へ**約4秒の連続 WallRun を1回**行ってください。小さな処理落ちが自然に起きても構いませんが、意図して発生させる必要はありません。Crawl、マーカー操作、E の試験は不要です。

確認する点：

- 壁に沿った姿勢が WallRun 終了まで続くか。
- 処理落ち後に、一瞬直立する・姿勢が飛ぶ・回転をやり直す様子がないか。
- クラッシュ、位置破損、めり込み、急な飛び出しがないか。

ゲームを終了し、**起動部分を含む完全な WallSprint ログを、非公開のまま通常どおり ChatGPT に送ってください**。見た目の結果も添えてください。

## 確認するログ

100msを超える Update の空白が起きた場合：

```text
NATIVE_SOURCE_STALE_SUSPEND ... update_age_ms=... last_native_update=...
NATIVE_SOURCE_STALE_RESUME ... new_update_age_ms=... native_update=... transition_restarted=0
```

停止中は A/B 書き込みを行いません。復帰は同一 owner/session/generation/thread/Hero/Transform/epoch と整合性を満たす新しい実 Update のみです。停止前に補間が完了していればその姿勢を継続し、補間途中なら最後の進行度から続きを進めます。Deinit や対象変更、整合性喪失、F8 は永久終了のままです。

最終確認：

```text
ORIENTATION_RETENTION_PASS=1
ORIENTATION_LIFECYCLE_SUMMARY ... source_exit_reason=verified_native_Deinit source_active_until_deinit=1 verified_deinit=1 ... retention_pass=1 ... stale_suspend_count=... stale_resume_count=... max_stale_duration_ms=...
ADHESION_SUMMARY ... corrections_applied=0 ... source_position_writes=0 ... mode=position_read_only
```

空白が起きなかった回では stale の件数が0でも正常です。250〜300ms の復帰は独立した native 試験でも検証しています。`fresh_update_age` の永久終了は成功扱いにしません。read-only の距離・補正候補は適用されず、自然に約0.5mなら候補0で構いません。

既存の複数スレッド診断監視は約1.50秒で終了し、姿勢保持とは独立しています。`MULTITHREAD_UP_WATCH_RESTORE ... cleanup_pending=0` も確認します。未知 writer 捕捉は今回の必須条件ではありません。

**F8 は再起動まで書き込み・監視 OFF、F9 は script 停止**。再試験は完全再起動してください。source 開始後5秒・各 path4096回の上限は維持しています。movement／animation／camera／gravity は追加していません。

生ログ、動画、ゲーム関数の命令列を GitHub・公開 ZIP に追加する必要はありません。
