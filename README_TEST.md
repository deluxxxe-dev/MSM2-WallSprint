# WallSprint v0.1c10 — Secondary Publication Race Repair

Package **0.1.28**。c9 は A 壁で姿勢を最後まで保持し、斜め E 壁でも Surface Up に到達しました。ただし E 壁のログには、同じ所有対象の公開データ更新を拒否した直後の短い WorldUp が2回ありました。c10 は同じ所有対象の更新を1回だけ再検証し、最新の姿勢から Path B の compact source を作り直します。**native の所有スレッド・Path A の制限・Position 読み取り専用は維持しています。実機でのちらつき解消は今回確認します。**

## ダウンロード・導入

配布は **WallSprint_v0.1c10_secondary_publication_race.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得してください。この手順も GitHub 上の README_TEST.md を開いて確認できます。

ゲームを完全終了し、古い WallSprint を無効にして外側の ZIP を展開します。中の **WallSprint.script は解凍せず** Overstrike へ追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 起動確認

ゲームの `logs\WallSprint.log` に以下があることを確認します。

```text
WallSprint v0.1c10 package_version=0.1.28
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1 ... pairs=9
DUAL_SOURCE_ADMISSION admitted=1
PATH_A_POSITION_PROOF admitted=0 ... position_source_write=0
NATIVE_SOURCE_GUARD_STATUS admitted=1 first_cause=none ...
```

Position proof の admitted=0 は想定どおりです。Path B proof／dual-source admission が失敗した場合は WallRun を試さず、ログを送ってください。

## 今回の試験

**前回と同じ斜め E 壁で、約4秒の連続 WallRun を1回**行ってください。意図した処理落ちは不要です。E が問題なければ、大きな A 壁も1回確認してください。

- 約0.7秒から回転し、斜めの壁に沿った Surface Up に到達するか。
- その後、WallRun 終了まで保持するか。特に前回の約2.34秒／3.53秒付近を含め、短い直立・姿勢の飛びがないか。
- クラッシュ、位置破損、めり込み、急な飛び出しがないか。

ゲームを終了し、**起動部分を含む完全な WallSprint ログを非公開のまま ChatGPT に送ってください**。E／A のどちらの試験か、見た目の結果も添えてください。前回の WorldUp は肉眼では分かりにくかったため、ログでの確認も必要です。

## 確認するログ

通常の別スレッド B 呼び出しは、従来どおり `PATH_B_SECONDARY_THREAD_EVENT ... action=override reason=secondary_verified` です。途中で公開データの更新があった場合は追加で次を記録します。

```text
PATH_B_SECONDARY_PUBLICATION_REFRESH_EVENT ... old_published_ms=... new_published_ms=... old_native_update=... new_native_update=... semantic_identity_match=1 result=secondary_publication_refresh_admitted rejection_reason=none refresh_attempt_cap=1 expected_native_thread_unchanged=1 position_source_write=0
PATH_B_SECONDARY_THREAD_SUMMARY ... secondary_publication_refresh_seen=... secondary_publication_refresh_admitted=... secondary_publication_refresh_rejected_identity_change=... secondary_publication_refresh_rejected_stale=... secondary_publication_refresh_rejected_integrity=... secondary_publication_refresh_rejected_other=...
```

自然に競合しなかった回は refresh_seen=0 でも構いません。自動試験では同じ競合を確実に起こして検証しています。更新を再検証している最中にさらに更新された場合は1回の上限で拒否し、理由を残します。所有対象・token・コード・stack の異常は引き続き書き込みを許可しません。

成功条件：

```text
ORIENTATION_RETENTION_PASS=1
ORIENTATION_LIFECYCLE_SUMMARY ... source_exit_reason=verified_native_Deinit source_active_until_deinit=1 verified_deinit=1 ... retention_pass=1 ... first_hard_kill=none hard_kill_calls=0
ADHESION_SUMMARY ... corrections_applied=0 ... source_position_writes=0 ... mode=position_read_only
```

完全な Surface Up 到達後、source lifecycle が有効な間に **`actor_up=(0,1,0)` の観測がないこと**を確認します。正当な同一所有対象の更新だけで `secondary_source_publication_changed` を拒否して WorldUp を通していないことも確認します。別スレッドの native 所有権変更は行いません。

自然な100ms超の Update 空白は、従来の `NATIVE_SOURCE_STALE_SUSPEND` → 同一対象の実 Update による `NATIVE_SOURCE_STALE_RESUME ... transition_restarted=0` で確認します。停止中の書き込みは許可しません。

旧 hardware watch の約1.50秒終了と今回の保持は独立しています。終了時は `MULTITHREAD_UP_WATCH_RESTORE ... cleanup_pending=0` も確認します。**F8 は再起動まで書き込み・監視 OFF、F9 は script 停止**。再試験は完全再起動してください。source 開始後5秒・各 path4096回の上限は維持しています。

Position 補正・movement・animation・camera・gravity は追加していません。生ログ・動画・ゲーム関数の命令列を GitHub／公開 ZIP に追加する必要はありません。
