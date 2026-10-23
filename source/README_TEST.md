# WallSprint v0.1c15 — Writer Branch-Aware RDX Contract

Package **0.1.33**。c14 で止まった writer 内の分岐を、上限を設けて両側とも解析する診断版です。入口の集計が採取開始前に0で確定する問題も修正しました。Position・RDX/source・入力・移動の書き換えはありません。見た目の変化は想定していません。

## ダウンロード・導入

配布は **WallSprint_v0.1c15_writer_branch_contract.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得し、この手順も GitHub 上で開いてください。

ゲームを完全終了し、旧 WallSprint を無効にして外側の ZIP を展開。**WallSprint.script は解凍せず** Overstrike に追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 試験

1. A/cardinal の平らな壁で、約4秒の連続 WallRun を1回。
2. WallRun を完全に終了。
3. E の場所へ移動、またはファストトラベル。
4. E/diagonal の壁でも、約4秒の連続 WallRun を1回。
5. **起動から終了までの完全な WallSprint.log を1本、非公開のまま ChatGPT に送ってください。**

普通の入力で試してください。特別な入力パターンは不要です。A/E の対応と、ちらつき・位置破損・引っかかり・停止・クラッシュの有無を添えてください。動画は見た目の異常が出た場合だけ使ってください。生ログや private game body を GitHub に追加する必要はありません。

## 確認するログ

```text
WallSprint v0.1c15 package_version=0.1.33
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1
DUAL_SOURCE_ADMISSION admitted=1
MOVEMENT_ENTRY_DIAGNOSTIC_GATE admitted=1
MOVEMENT_RDX_BRANCH_CONTRACT ... first_branch_rva=... all_relevant_store_paths_proven=... x_path=... y_path=... z_path=... next_barrier_rva=... next_barrier_instruction=... reason=...
MOVEMENT_RDX_XYZ_PROOF ... static_prefix_proven=... xyz_causality_proven=... entry_store_xyz_match_mask=...
MOVEMENT_POSITION_W_PROOF ... store_rva=0x2cfc945 source_kind=... causality_proven=... homogeneous_coordinate_semantics=unknown
MOVEMENT_ENTRY_CAPTURE_SUMMARY ... true_entry_snapshots=... complete_invocation_maps=... xyz_match_count=... phase=finalized summary_final=1
MOVEMENT_LINEAGE_SUMMARY ... source_capture_ready=... true_entry_snapshot=... true_entry_snapshots=... complete_invocation_maps=...
```

XYZ と W は別々に判定します。実行した値の一致だけでは因果証明にしません。分岐の片側が異なる値を使う場合や、次の未対応命令・ループ・上限超過がある場合は、その場所と理由を出して拒否します。unknown／unresolved／部分採取も、そのまま完全ログで共有してください。

今回の対象は writer 内だけです。producer `0x351EB00` の書き換えや移動の実装には進みません。`writable_source_proven=0` を維持します。足の浮き・壁との距離・体の傾きも引き続き保留です。

## 終了確認

```text
ORIENTATION_LIFECYCLE_SUMMARY ... source_active_until_deinit=1 verified_deinit=1 retention_pass=1 hard_kill_calls=0
POSITION_WRITER_SUMMARY ... cleanup_restored=1 cleanup_pending=0 Position_writes_by_mod=0
MOVEMENT_ENTRY_DIAGNOSTIC_STOP restored=1 code_restored=1
MOVEMENT_ENTRY_CAPTURE_SUMMARY ... phase=finalized summary_final=1
```

最終集計は採取の停止と処理待ちの解消後に出ます。install／activate が拒否された場合や部分採取では、0や未完了でも理由を含めて判定します。条件を緩める必要はありません。build／Path B／dual-source の admission が失敗したら走らずログを共有してください。F8 は再起動まで書き込み・監視を停止、F9 は script 停止です。
