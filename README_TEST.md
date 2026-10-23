# WallSprint v0.1c14 — RDX Stack-Local Source Contract

Package **0.1.32**。c13 の実機 A/E で採れた「RDX／stack の XYZ が Position の書き込み値と一致、source W=0／出力 W=1」を、実際の命令の流れで確認する診断版です。入口記録と XYZ 一致の集計も修正しました。Position・source・入力・移動・重力の書き換えはありません。見た目の変化は想定していません。足の浮き・壁との距離・体の傾きは引き続き保留です。

## ダウンロード・導入

**WallSprint_v0.1c14_rdx_source_contract.zip** を完了メッセージの GitHub 直接ダウンロードリンクから取得してください。このテスト手順も GitHub 上で開けます。

ゲームを完全終了し、旧 WallSprint を無効にして外側の ZIP を展開します。**WallSprint.script は解凍せず** Overstrike に追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 試験

1. A/cardinal の平らな壁で、約4秒の連続 WallRun を1回。
2. WallRun を完全に終了。
3. E の場所へ移動、またはファストトラベル。
4. E/diagonal の壁でも、約4秒の連続 WallRun を1回。
5. **起動から終了までの完全な WallSprint.log を1本、非公開のまま ChatGPT に送ってください。**

普通の入力で試してください。特別な操作や意図した処理落ちは不要です。A/E の対応と、ちらつき・位置破損・引っかかり・停止・クラッシュの有無を添えてください。動画は見た目の異常が出た場合に使ってください。生ログや private game body を GitHub に追加する必要はありません。

## 確認するログ

```text
WallSprint v0.1c14 package_version=0.1.32
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1
DUAL_SOURCE_ADMISSION admitted=1
MOVEMENT_ENTRY_DIAGNOSTIC_GATE admitted=1
MOVEMENT_RDX_STATIC_CONTRACT ... xyz_RDX_0_4_8_paths=... W_kind=... first_barrier_rva=... first_barrier_instruction=...
MOVEMENT_ENTRY_DIAGNOSTIC_ACTIVE ... capture_ready=1
MOVEMENT_RDX_XYZ_PROOF ... xyz_causality_proven=... entry_store_xyz_match_mask=...
MOVEMENT_POSITION_W_PROOF ... source_kind=... source_value_bits=... destination_value_bits=... causality_proven=...
MOVEMENT_RDX_STORAGE_PROOF ... rdx_relation=... current_thread_TEB_bounded=... caller_owned=...
MOVEMENT_RDX_PRODUCER_PROOF ... producer_relation_to_0x351A020=... next_edge_rva=... reason=...
MOVEMENT_ENTRY_CAPTURE_SUMMARY ... true_entry_snapshots=... complete_invocation_maps=... xyz_source_candidate=... xyz_match_count=... source_w_pattern=... destination_w_pattern=...
MOVEMENT_LINEAGE_SUMMARY ... source_capture_ready=... static_reconstruction_ready=... true_entry_snapshot=... true_entry_snapshots=...
```

既存の ENTRY_SNAPSHOT／INVOCATION_MAP／ENTRY_SOURCE_CANDIDATE も残ります。最大8入口／セッション、最大600msの既存 Position 監視、対象 Hero の照合、入口 RSP と ID による入れ子・別スレッドの分離は維持しています。

XYZ の mask=0x7 は W と別に集計します。`source_stable`／`xyz_stable_across_invocations` は観測した XYZ 一致パターンの反復であり、書き換え可能な保存領域やエンジン内部の型を認定するものではありません。`constant_one` も W の命令上の生成を示すだけで、座標型の意味までは認定しません。拒否・部分採取・unknown/unresolved は理由ごとそのまま送ってください。gate を緩める必要はありません。

## 終了確認

```text
ORIENTATION_LIFECYCLE_SUMMARY ... source_active_until_deinit=1 verified_deinit=1 retention_pass=1 hard_kill_calls=0
ADHESION_SUMMARY ... corrections_applied=0 source_position_writes=0
POSITION_WRITER_SUMMARY ... cleanup_restored=1 cleanup_pending=0 Position_writes_by_mod=0
MOVEMENT_ENTRY_DIAGNOSTIC_STOP restored=1 code_restored=1
```

入口診断の install が拒否された場合は STOP がないことがあります。build／Path B／dual-source の admission が失敗したら走らずログを共有してください。F8 は再起動まで書き込み・監視を停止、F9 は script 停止です。

c14 の実機因果証明・producer/lifetime/downstream の判定は今回のログ待ちです。`writable_source_proven=0` を維持し、移動・入力 rewrite へ自動では進みません。
