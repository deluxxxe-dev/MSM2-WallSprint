# WallSprint v0.1c17 — Callee Alias Contract

Package **0.1.35**。c16 で確認した callee `0x2B23560..0x2B235E9` の書き込み先を、writer の call 直前の引数から追跡する診断版です。source／Transform と重ならないと証明できた場合だけ XYZ／W 解析を続け、未解決なら正確な命令・書き込み先の由来を記録します。Position・source・入力・移動の書き換えはありません。見た目の変化は想定していません。

## ダウンロード・導入

配布は **WallSprint_v0.1c17_callee_alias_contract.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得し、この手順も GitHub 上で開いてください。

ゲームを完全終了し、旧 WallSprint を無効にして外側の ZIP を展開。**WallSprint.script は解凍せず** Overstrike に追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 試験

起動後、ゲームフォルダ直下の `log_wallsprint` に日時付きの新しいファイルがあることを確認してください。ファイル名の日時は PC のローカル時刻で、A→E を同じファイルに記録します。既存ファイルは上書きしません。フォルダ作成・保存に失敗した場合はデバッグ出力にパス／Win32Error を出し、共用 `logs` への代替保存はしません。

1. A/cardinal の平らな壁で、約4秒の連続 WallRun を1回。
2. WallRun を完全に終了。
3. E の場所へ移動、またはファストトラベル。
4. E/diagonal の壁でも、約4秒の連続 WallRun を1回。
5. **ゲームフォルダの `log_wallsprint` 内で、この実行で作られた最新の `WallSprint_YYYYMMDD-HHMMSS.log`（同秒の重複時は `_01` など付き）を1本、起動から終了まで省略せず非公開で ChatGPT に送ってください。**

普通の入力で試してください。特別な入力は不要です。A/E の対応と、ちらつき・位置破損・引っかかり・停止・クラッシュの有無を添えてください。動画は見た目の異常時だけ。生ログや private game body を GitHub に追加する必要はありません。

## 確認するログ

```text
WallSprint v0.1c17 package_version=0.1.35
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1
DUAL_SOURCE_ADMISSION admitted=1
MOVEMENT_ENTRY_DIAGNOSTIC_GATE admitted=1
MOVEMENT_WRITER_CALL_CONTRACT ... call_rva=0x2cfc927 call_kind=... target_rva=... target_boundary_begin=... target_boundary_end=... target_code_hash=... return_rva=0x2cfc92c current_code_verified=... relevant_to_xyz_paths=... effect=... next_callee_barrier_rva=... next_callee_instruction=... reason=...
MOVEMENT_WRITER_CALL_ARGUMENT_CONTRACT ... arg_rcx_origin=... arg_rdx_origin=... arg_r8_origin=... arg_r9_origin=... source_span_alias_mask=... transform_alias_mask=... proof_complete=...
MOVEMENT_CALLEE_ALIAS_CONTRACT ... target_rva=0x2b23560 memory_writes_seen=... source_span_no_write_proven=... transform_no_write_proven=... all_relevant_write_aliases_resolved=... next_barrier_rva=... next_barrier_instruction=... next_barrier_effect=... reason=...
MOVEMENT_WRITER_CALL_INTEGRITY ... current_code_verified=... stack_footprint_verified=... global_regions_verified=...
MOVEMENT_RDX_BRANCH_CONTRACT ... all_relevant_store_paths_proven=... next_barrier_rva=... next_barrier_instruction=... reason=...
MOVEMENT_RDX_XYZ_PROOF ... xyz_causality_proven=... entry_store_xyz_match_mask=...
MOVEMENT_POSITION_W_PROOF ... source_kind=... causality_proven=...
MOVEMENT_ENTRY_CAPTURE_SUMMARY ... true_entry_snapshots=... complete_invocation_maps=... phase=finalized summary_final=1
MOVEMENT_LINEAGE_SUMMARY ... true_entry_snapshots=... complete_invocation_maps=...
```

callee の未知／競合する書き込み先、source／Transform 書き込み、入れ子 call、境界・上限の拒否も、次の障害を正確に絞れた診断結果として扱います。値の一致だけでは call の無害性や書き込み許可を証明しません。XYZ と W は別判定です。`writable_source_proven=0` と `Position_writes_by_mod=0` を維持します。

従来の回帰確認は、A/E の通常 Deinit、retention、Surface Up 維持、hard kill／stale なし、入口診断と DR/watch の正常な解除です。起動版が c16 になっていることも確認してください。
