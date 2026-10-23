# WallSprint v0.1c18 — Callee Branch-Aware Alias Contract

Package **0.1.36**。c17 の実機で止まった callee 内 `0x2B235C1: je` の分岐解析を進める診断版です。認証済み137バイトの全分岐を数えて解析上限を計算し、両方の経路で書き込み先を確認します。source／Transform に書かないと証明できた場合だけ writer の XYZ／W 解析を再開し、未解決なら正確な障害を記録します。Position・source・入力・移動の書き換えはありません。見た目の変化は想定していません。実機の分岐数・書き込み先・次の障害は今回の試験で確認します。

## ダウンロード・導入

配布は **WallSprint_v0.1c18_callee_branch_alias.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得し、この手順も GitHub 上で開いてください。

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
WallSprint v0.1c18 package_version=0.1.36
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1
DUAL_SOURCE_ADMISSION admitted=1
MOVEMENT_ENTRY_DIAGNOSTIC_GATE admitted=1
MOVEMENT_WRITER_CALL_CONTRACT ... call_rva=0x2cfc927 call_kind=... target_rva=... target_boundary_begin=... target_boundary_end=... target_code_hash=... return_rva=0x2cfc92c current_code_verified=... relevant_to_xyz_paths=... effect=... next_callee_barrier_rva=... next_callee_instruction=... reason=...
MOVEMENT_WRITER_CALL_ARGUMENT_CONTRACT ... arg_rcx_origin=... arg_rdx_origin=... arg_r8_origin=... arg_r9_origin=... source_span_alias_mask=... transform_alias_mask=... proof_complete=...
MOVEMENT_CALLEE_CFG_CONTRACT ... local_branch_sites=... conditional_branch_sites=... branch_cap=... path_cap_per_call=... state_cap_per_call=... blocks_total=... edges_total=... blocks_visited=... edges_visited=... paths_examined=... backedges_seen=... indirect_transfers_seen=... nested_calls_seen=... first_unresolved_rva=... first_unresolved_instruction=... reason=...
MOVEMENT_CALLEE_WRITE_ALIAS ... site_rva=... width=... address_class=... address_root=... address_offset=... may_alias_source=... may_alias_transform=... coverage=...
MOVEMENT_CALLEE_ALIAS_CONTRACT ... target_rva=0x2b23560 memory_writes_seen=... source_span_no_write_proven=... transform_no_write_proven=... all_relevant_write_aliases_resolved=... next_barrier_rva=... next_barrier_instruction=... next_barrier_effect=... reason=...
MOVEMENT_WRITER_CALL_INTEGRITY ... current_code_verified=... stack_footprint_verified=... global_regions_verified=...
MOVEMENT_RDX_BRANCH_CONTRACT ... all_relevant_store_paths_proven=... next_barrier_rva=... next_barrier_instruction=... reason=...
MOVEMENT_RDX_XYZ_PROOF ... xyz_causality_proven=... entry_store_xyz_match_mask=...
MOVEMENT_POSITION_W_PROOF ... source_kind=... causality_proven=...
MOVEMENT_ENTRY_CAPTURE_SUMMARY ... true_entry_snapshots=... complete_invocation_maps=... phase=finalized summary_final=1
MOVEMENT_LINEAGE_SUMMARY ... true_entry_snapshots=... complete_invocation_maps=...
```

callee の未知／競合する書き込み先、source／Transform 書き込み、入れ子 call、境界・上限の拒否も、次の障害を正確に絞れた診断結果として扱います。値の一致だけでは call の無害性や書き込み許可を証明しません。XYZ と W は別判定です。`writable_source_proven=0` と `Position_writes_by_mod=0` を維持します。

従来の回帰確認は、A/E の通常 Deinit、retention、Surface Up 維持、hard kill／stale なし、入口診断と DR/watch の正常な解除です。起動版が c18 になっていることも確認してください。
