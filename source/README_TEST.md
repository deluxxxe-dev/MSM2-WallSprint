# WallSprint v0.1c12 — Movement Source Lineage

Package **0.1.30**。c10／c11 の A／E 壁での姿勢保持は実機で成功しました。今回は**移動経路を調べる診断版**です。Position・入力・速度・重力は変更しません。見た目の移動変化は想定していません。体の傾きや足の浮きは、後の見た目・接地調整として保留しています。

## ダウンロード・導入

配布は **WallSprint_v0.1c12_movement_source_lineage.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得し、この手順も GitHub 上の README_TEST.md で確認してください。

ゲームを完全終了し、旧 WallSprint を無効にして外側の ZIP を展開します。**WallSprint.script は解凍せず** Overstrike に追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 起動確認

`logs\WallSprint.log` に以下があることを確認します。

```text
WallSprint v0.1c12 package_version=0.1.30
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1 ... pairs=9
DUAL_SOURCE_ADMISSION admitted=1
PATH_A_POSITION_PROOF admitted=0 ... position_source_write=0
PATH_A_POSITION_CAUSALITY_PROOF admitted=0 ... write_permission=0
```

Position／causality の admitted=0 は想定どおりです。安全な移動 source をまだ認証していないため、この版では書き込みを許可しません。build／Path B proof／dual-source admission に失敗した場合は走らずログを送ってください。

## c12 の追加診断

c11 で実測した `0x351EEB0 → 0x2CFC900` と `caller2≈0x351A020` を読み取り専用で調べます。起動時には次の記録が出ます。

```text
MOVEMENT_WRITER_PROOF ... function_begin=... function_end=... complete_decode=... current_code_hash=... lineage_complete=... source_map_supported_prefix=... reason=...
MOVEMENT_WRITER_SOURCE_COMPONENT ... component=... store_rva=... source=...
MOVEMENT_CALLSITE_PROOF ... direct_call_verified=... caller_function_begin=... caller_hash=... caller2_direct_relation_verified=... reason=...
MOVEMENT_CALLER_SOURCE_PROOF ... construction_kind=unknown ... writable_source_proven=0 reason=...
MOVEMENT_CALLER_SOURCE_COMPONENT ... expression=...
MOVEMENT_LINEAGE_UNSUPPORTED ... instruction_rva=... reason=...
```

`complete_decode=1` と、完全なデータフロー証明や移動の所有権は別の項目です。非対応命令・分岐・範囲上限は具体的な位置と理由を記録します。`construction_kind=unknown`／`writable_source_proven=0` は想定内で、診断のために条件を緩める必要はありません。

監視中には、証明できた経路だけで以下を採取します。

```text
MOVEMENT_WRITER_SOURCE_MAP ... source_readable=... recovered_aliases_stable=... post_store_match=... entry_snapshot_captured=0 writable_source_proven=0 reason=...
MOVEMENT_SOURCE_CORRELATION ... source_payload=... observed_position_before=... observed_position_after=... predicted_delta=... observed_delta=... delta_error=... delta_normal=... delta_forward=... delta_right=...
MOVEMENT_LINEAGE_SUMMARY ... source_capture_ready=... attempts=... source_prefix_proven=... direct_call_verified=... true_entry_snapshot=0 writable_source_proven=0 Position_writes_by_mod=0
```

既存の書き込み監視時点で残るレジスタ／ソースからの復元であり、関数入口で直接採取した値ではありません。1監視あたり最大16回、1 writer の表示は従来どおり最大4件です。途中の scalar store では行全体が一致しないことがあります。一致しても W の意味や後続 writer の排他性は証明されません。復元できない場合も、正確な理由が次の解析材料になります。

## 今回の試験

1. 大きな平らな A 方向の壁で、約4秒の連続 WallRun を1回。
2. 同じ斜め E 壁でも、約4秒の連続 WallRun を1回。

特別なスティック操作や意図した処理落ちは不要です。試験ごとにゲームを完全再起動すると、ログの対応が分かりやすくなります。

- c10 と同じく、約0.7秒から回転して Surface Up を終了まで保持するか。
- クラッシュ、停止、引っかかり、強い処理落ちがないか。
- 直立するちらつき、位置破損、急な飛び出しがないか。

ゲームを終了し、**起動から終了までの完全なログを非公開のまま ChatGPT に送ってください**。A／E のどちらか、見た目・停止や引っかかりの有無も添えてください。動画・命令列・生ログを GitHub／公開 ZIP に追加する必要はありません。

## 診断で確認する項目

元の Up writer 監視は約1.50秒までです。その全 lease の復元後、完全姿勢と retention が確認できた場合だけ、1.55〜3.0秒の間に Position の監視を1回開始し、**最大600ms**観測します。空き DR0／DR1 を使い、既存の foreign DR は保持します。監視できない場合は、条件を緩めず理由を記録します。

```text
POSITION_WATCH_ARM ... watched=Transform+[0x30,0x40) ... old_Up_leases_restored=1
POSITION_WRITE_EVENT ... writer_function_rva=... caller_rva=... slot_mask=... position_before=... position_after=... position_w_bits_after=... classification=...
POSITION_WRITER_SUMMARY ... watch_armed=1 ... unique_writers=... total_write_events=... cleanup_restored=1 cleanup_pending=0 ... Position_writes_by_mod=0
POSITION_WRITER_HITS ... writer_rva=... hits=...
MOVEMENT_DELTA_SAMPLE ... dt_ms=... delta_world=... delta_normal=... delta_forward=... delta_right=... speed_estimate=... valid=1 ... Position_writes=0
MOVEMENT_SOURCE_CANDIDATE ... writable_source_proven=0 reason=...
```

Position の第4成分は意味を決めずに読み取ります。速度表示は位置差分からの診断値で、ゲーム内部の velocity だとは扱いません。Position event の before は直前の監視観測、after は命令実行後の観測で、関数完了時の結果ではありません。記録上限・スナップショット対象外のスレッドがあるため、writer が捕まらなかったことだけで「他の writer が存在しない」とは判断しません。

最終確認：

```text
ORIENTATION_RETENTION_PASS=1
ORIENTATION_LIFECYCLE_SUMMARY ... source_exit_reason=verified_native_Deinit source_active_until_deinit=1 verified_deinit=1 ... retention_pass=1 ... first_hard_kill=none hard_kill_calls=0
ADHESION_SUMMARY ... corrections_applied=0 ... source_position_writes=0
MULTITHREAD_UP_WATCH_RESTORE ... cleanup_pending=0
POSITION_WRITER_SUMMARY ... cleanup_pending=0
```

監視が開始されなかった場合も、`POSITION_WATCH_SKIPPED`／`POSITION_WRITER_SUMMARY watch_armed=0` と理由を含めた完全ログが必要です。c12 の新しい記録で writer 全体・直接 call・ソース復元・呼び出し元のどの証明が残っているかを確認します。

F8 は再起動まで書き込み・診断監視を停止、F9 は script 停止です。c7 の自然な stale 停止／同一対象の新しい実 Update による復帰、c8 の初回異常原因、c9／c10 の別スレッド B と更新再検証は維持しています。Position 補正・movement 注入・入力 rewrite・animation・camera・gravity は追加していません。
