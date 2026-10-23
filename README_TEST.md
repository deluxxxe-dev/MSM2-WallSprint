# WallSprint v0.1c9 — Secondary-Thread Path B Admission

Package **0.1.27**。c8 では壁に沿った姿勢へ回転した後、約1.75秒に別スレッドの Path B が来て姿勢保持を終了していました。c9 は、その呼び出しも対象・caller・現在のスレッドの stack・compact proof・鮮度・コード整合性を確認してから許可します。**native WallRun の所有スレッドと Path A の制限は維持し、Position は読み取り専用です。** 実機での保持確認はこれからです。

## ダウンロード・導入

配布は **WallSprint_v0.1c9_secondary_path_b_thread.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得し、試験手順は GitHub 上のこの README_TEST.md を開いてください。

ゲームを完全終了し、古い WallSprint を無効にして ZIP を展開します。中の **WallSprint.script は解凍せず** Overstrike へ追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 起動確認

ゲームのインストール先の `logs\WallSprint.log` を確認します。

```text
WallSprint v0.1c9 package_version=0.1.27
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1 ... pairs=9
DUAL_SOURCE_ADMISSION admitted=1
PATH_A_POSITION_PROOF admitted=0 ... position_source_write=0
OBSERVATION_PLAN_AUTH_STATUS ... source_integrity=1 diagnostic_viable=1
NATIVE_SOURCE_GUARD_STATUS admitted=1 first_cause=none ...
```

Position proof の admitted=0 は想定どおりです。Path B proof／dual-source admission が失敗した場合は WallRun 試験を進めずログを送ってください。

## 今回の試験

**同じ大きな A 方向の平らな壁**で、左→右へ**約4秒の連続 WallRun を1回**行ってください。処理落ちを意図して起こす必要はありません。Crawl、マーカー操作、E の試験は不要です。

確認する点：

- 約0.7秒から回転し始め、壁に沿った Surface Up に到達するか。
- WallRun 終了まで保持し、特に約1.5〜2.0秒で直立する・姿勢が飛ぶ・回転をやり直す様子がないか。
- クラッシュ、位置破損、めり込み、急な飛び出しがないか。

ゲームを終了し、**起動部分を含む完全な WallSprint ログを、非公開のまま通常どおり ChatGPT に送ってください**。見た目の結果も添えてください。

## 確認するログ

別スレッドの Path B が来た場合：

```text
PATH_B_SECONDARY_THREAD_EVENT ... thread_id=... expected_thread=... caller_rva=0x2cfc84b ... rdx_minus_rsp=40 stack_verified=1 compact_verified=1 token_status=coherent_active identity_verified=1 ... action=override reason=secondary_verified expected_native_thread_unchanged=1
PATH_B_SECONDARY_THREAD_SUMMARY ... seen=... admitted=... overridden=... rejected=... unique_threads=...
```

拒否には理由を残します。無関係な target／caller／stack／layout は姿勢 payload を受け取りません。実際のコード・proof・patch・所有対象・epoch の異常は引き続き永久停止です。`NATIVE_SOURCE_HARD_KILL` の初回原因と `OBSERVATION_PLAN_AUTH_FAILURE` は c8 から維持しています。

最終確認：

```text
ORIENTATION_RETENTION_PASS=1
NATIVE_SOURCE_CLOSE ... reason=verified_Deinit ... source_active_until_deinit=1
ORIENTATION_LIFECYCLE_SUMMARY ... source_exit_reason=verified_native_Deinit source_active_until_deinit=1 verified_deinit=1 ... retention_pass=1 ... first_hard_kill=none
ADHESION_SUMMARY ... corrections_applied=0 ... source_position_writes=0 ... mode=position_read_only
```

別スレッドの呼び出しがなかった回では seen=0 でも構いません。その場合も WallRun 全体の姿勢保持が必要です。別スレッドの再現ケースは独立 native 試験で確認しています。**thread の違いだけで `path_b_wrong_thread` の永久終了を起こすことは成功扱いにしません。**

100msを超える Update の空白が自然に起きた場合は、従来の `NATIVE_SOURCE_STALE_SUSPEND` → `NATIVE_SOURCE_STALE_RESUME ... transition_restarted=0` を確認します。停止中はどのスレッドからも書き込まず、新しい同一対象の実 Update で復帰します。

既存の複数スレッド診断監視は約1.50秒で終了し、姿勢保持とは独立しています。新しい Path B 専用記録はその後も source が有効な間に取得します。`MULTITHREAD_UP_WATCH_RESTORE ... cleanup_pending=0` も確認します。**F8 は再起動まで書き込み・監視 OFF、F9 は script 停止**。再試験は完全再起動してください。source 開始後5秒・各 path4096回の上限は維持しています。

Position 補正、movement／animation／camera／gravity は追加していません。生ログ、動画、ゲーム関数の命令列を GitHub・公開 ZIP に追加する必要はありません。
