# WallSprint v0.1c16 — Writer Call-Effect Contract

Package **0.1.34**。c15 で判明した writer 内 `0x2CFC927` の call 対象と影響を絞る診断版です。確認できた場合だけ後続の XYZ／W 解析を続け、未解決なら正確な callee 命令と理由を記録します。Position・RDX/source・入力・移動の書き換えはありません。見た目の変化は想定していません。

## ダウンロード・導入

配布は **WallSprint_v0.1c16_writer_call_effect.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得し、この手順も GitHub 上で開いてください。

ゲームを完全終了し、旧 WallSprint を無効にして外側の ZIP を展開。**WallSprint.script は解凍せず** Overstrike に追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 試験

1. A/cardinal の平らな壁で、約4秒の連続 WallRun を1回。
2. WallRun を完全に終了。
3. E の場所へ移動、またはファストトラベル。
4. E/diagonal の壁でも、約4秒の連続 WallRun を1回。
5. **起動から終了までの完全な WallSprint.log を1本、非公開のまま ChatGPT に送ってください。**

普通の入力で試してください。特別な入力は不要です。A/E の対応と、ちらつき・位置破損・引っかかり・停止・クラッシュの有無を添えてください。動画は見た目の異常時だけ。生ログや private game body を GitHub に追加する必要はありません。

## 確認するログ

```text
WallSprint v0.1c16 package_version=0.1.34
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1
DUAL_SOURCE_ADMISSION admitted=1
MOVEMENT_ENTRY_DIAGNOSTIC_GATE admitted=1
MOVEMENT_WRITER_CALL_CONTRACT ... call_rva=0x2cfc927 call_kind=... target_rva=... target_boundary_begin=... target_boundary_end=... target_code_hash=... return_rva=0x2cfc92c current_code_verified=... relevant_to_xyz_paths=... effect=... next_callee_barrier_rva=... next_callee_instruction=... reason=...
MOVEMENT_WRITER_CALL_INTEGRITY ... current_code_verified=...
MOVEMENT_RDX_BRANCH_CONTRACT ... all_relevant_store_paths_proven=... next_barrier_rva=... next_barrier_instruction=... reason=...
MOVEMENT_RDX_XYZ_PROOF ... xyz_causality_proven=... entry_store_xyz_match_mask=...
MOVEMENT_POSITION_W_PROOF ... source_kind=... causality_proven=...
MOVEMENT_ENTRY_CAPTURE_SUMMARY ... true_entry_snapshots=... complete_invocation_maps=... phase=finalized summary_final=1
MOVEMENT_LINEAGE_SUMMARY ... true_entry_snapshots=... complete_invocation_maps=...
```

callee の未対応命令／書き込み／境界・上限の拒否も、次の障害を正確に絞れた診断結果として扱います。値の一致だけでは call の無害性や書き込み許可を証明しません。XYZ と W は別判定です。`writable_source_proven=0` と `Position_writes_by_mod=0` を維持します。

従来の回帰確認は、A/E の通常 Deinit、retention、Surface Up 維持、hard kill／stale なし、入口診断と DR/watch の正常な解除です。起動版が c16 になっていることも確認してください。
