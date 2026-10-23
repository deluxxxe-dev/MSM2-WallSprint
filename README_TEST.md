# WallSprint v0.1c13 — Writer Entry Snapshot

Package **0.1.31**。c12 の A／E 壁での姿勢保持と直接 call の実機結果を維持し、今回は **writer が値を変更する前の入口記録**を採取します。Position・source・入力・移動・重力の書き換えはありません。見た目の移動変化は想定していません。足の浮き・壁との距離・体の傾きは保留しています。

## ダウンロード・導入

配布：**WallSprint_v0.1c13_writer_entry_snapshot.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得し、この手順も GitHub 上で確認してください。

ゲームを完全終了し、旧 WallSprint を無効にして外側の ZIP を展開します。**WallSprint.script は解凍せず** Overstrike に追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 試験

1. 平らな A/cardinal 壁で、約4秒の連続 WallRun を1回。
2. 斜め E/diagonal 壁でも、約4秒の連続 WallRun を1回。

普通の入力で試してください。特別なスティック操作・意図した処理落ちは不要です。約0.7秒からの Surface Up が終了まで保たれるか、ちらつき・位置破損・クラッシュ・停止・引っかかりがないかを確認します。

終了後、**起動から終了までの完全な WallSprint.log を非公開のまま ChatGPT に送ってください**。A／E の対応と見た目・hitch/freeze/crash の有無を添えてください。動画は任意で、今回の判定はログが中心です。生ログ・動画・private game body を GitHub や配布 ZIP に追加する必要はありません。

## 起動確認

```text
WallSprint v0.1c13 package_version=0.1.31
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1
DUAL_SOURCE_ADMISSION admitted=1
MOVEMENT_WRITER_PROOF ... function_begin=0x2cfc900 function_end=0x2cfc9b7 ... first_unsupported_offset=0xa
MOVEMENT_CALLSITE_PROOF ... direct_call_verified=1
MOVEMENT_ENTRY_DIAGNOSTIC_GATE admitted=1 ... original_sha256=... reason=...
```

c12 の `source_prefix_proven=0`／`true_entry_snapshot=0` は、c12 の再構成診断として残ります。c13 入口診断の結果とは別の記録です。Position/source の書き換え許可は常に0です。build／Path B／dual-source の admission が失敗したら走らずログを共有してください。

## c13 の採取

元の Up 監視が復元され、完全姿勢と retention が確認できた場合だけ、既存の1.55〜3.0秒開始・最大600msの Position 監視に合わせて入口診断を有効にします。**1セッション最大8回**、入口レジスタ・64 bytes の stack 引数・3個の小さな pointer 候補を採取します。別対象の generic Transform 呼び出しは除外します。

```text
MOVEMENT_ENTRY_DIAGNOSTIC_START ... installed=1 ... eligible_cap=8
MOVEMENT_ENTRY_DIAGNOSTIC_ACTIVE ... capture_ready=1 hardware_watch_published=1
MOVEMENT_WRITER_ENTRY_SNAPSHOT ... session=... invocation_id=... thread_id=... return_rva=... caller2_observed_rva=... true_entry_snapshot=1
POSITION_WRITE_EVENT invocation_id=... ... writer_function_rva=0x2cfc900
MOVEMENT_WRITER_INVOCATION_MAP ... invocation_id=... store_30_seen=... store_34_seen=... store_38_seen=... store_3c_seen=... entry_source_match_mask=... entry_pointer_candidate_mask=... source_stable=0 ... reason=...
MOVEMENT_ENTRY_SOURCE_CANDIDATE ... origin=... raw_bits=... relationship=... exact_store_match_mask=... typed_semantics=unknown writable_source_proven=0
MOVEMENT_CALLER_CHAIN_RUNTIME ... relation=direct|indirect|wrapper|unresolved ... runtime_stack_verified=...
MOVEMENT_ENTRY_DIAGNOSTIC_STOP restored=1 code_restored=1 ... trampoline_retained=1
```

`final_position`／`delta_like` は値の関係を示す診断ラベルで、ゲーム内部の velocity・displacement・root motion と認定するものではありません。全4成分が一致しても、W の意味・書き込み後の競合・移動の所有権は未証明です。Position 行の観測は命令実行後で、関数終了やエンジン全体の原子的状態ではありません。入れ子・並行呼び出しは入口 RSP と ID で区別し、不明なイベントは一致扱いにしません。

admitted/installed=0、部分イベント、candidate=none／unresolved も、理由を含めてそのまま送ってください。条件を緩める必要はありません。caller2 の以前の direct mismatch を、根拠なく direct/wrapper に変えることもありません。

## 終了時の確認

```text
ORIENTATION_LIFECYCLE_SUMMARY ... source_exit_reason=verified_native_Deinit source_active_until_deinit=1 verified_deinit=1 ... retention_pass=1 ... hard_kill_calls=0
ADHESION_SUMMARY ... corrections_applied=0 ... source_position_writes=0
POSITION_WRITER_SUMMARY ... cleanup_restored=1 cleanup_pending=0 ... Position_writes_by_mod=0
MOVEMENT_ENTRY_DIAGNOSTIC_STOP restored=1 code_restored=1
```

入口診断が install されなかった場合は STOP が無いことがあります。START/GATE の拒否理由と完全ログを共有してください。F8 は再起動まで書き込み・監視を停止、F9 は script 停止です。実機の入口→store→candidate を判定してから次のタスクを決めます。c13 から移動・入力 rewrite へ自動では進みません。
