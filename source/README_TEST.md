# WallSprint v0.1c1 — Compact Path-B Override + Lock-Free Path-A

Package **0.1.19**。この版は exact build `Spider-Man2.exe 2.810.0.0` の姿勢ソースだけを変更します。Path A と Path B は同じ lock-free snapshot を使い、Path B は compact 3x3 の先頭 0x24 bytes だけを書き換えます。direct Actor / Position / adhesion / movement / animation / camera / physics は変更しません。

## ダウンロードと導入

配布ファイルは **WallSprint_v0.1c1_compact_path_b_override.zip** です。Codex の完了メッセージにある GitHub の直接ダウンロードリンクを使ってください。

1. ゲームを完全終了し、古い WallSprint を無効にします。
2. 配布 ZIP を展開します。
3. 中の **WallSprint.script は解凍せず** Overstrike に追加し、Install mods を実行します。
4. 従来どおり Script support と `-scripts` を有効にして起動します。配布 INI の変更は不要です。

## 起動時の確認

`Documents\Marvel's Spider-Man 2\scripts\WallSprint.log` で次を確認します。

```text
WallSprint v0.1c1 package_version=0.1.19
BUILD_IDENTITY accepted=1
PATH_B_COMPACT3X3_PROOF admitted=1 caller_verified=1 writer_verified=1 rdx_to_rcx_map_verified=1 source_bytes=0x24
RESET_COPY_HOOK admitted=1 installed=1
DUAL_SOURCE_ADMISSION admitted=1
PATH_B_LAYOUT_CONFIG ... path_b_override=1
```

どれかが `0` の場合は fail closed です。書き込みは行われないため、そのままログを送ってください。

## 実機テスト

起動後、次を各1回だけ行います。

1. **A Control**: 通常の垂直壁で左から右へ約1.5秒 WallRun。
2. **E diagonal**: 以前と同じ斜め壁で左から右へ約2秒 WallRun。

成功すれば、カメラは world-up のまま Spider-Man の Actor Up が壁面法線へ近づくため、画面上では明確に横向きへ回転します。アニメーションの不自然さはこの段階では許容します。動画は見た目の補助になり、判定はログを優先します。

## ログの判定

以下が最低条件です。

```text
PATH_A_LOCKFREE_SNAPSHOT_READ ... coherent=1
PATH_A_OVERRIDE_COUNT=（1以上）
PATH_A_STATE_LOCK_CONTENTION=0
PATH_B_COMPACT3X3_OVERRIDE ... bytes_written=0x24 metadata_untouched=1 caller_verified=1 target_verified=1 stack_verified=1
```

保持成功は次の行です。

```text
ORIENTATION_RETENTION_PASS=1 held_ms=... held_updates=...
```

`dot(ActorUp, N398) >= 0.98` を 300 ms または 20 native updates 保持すると出ます。A または E の少なくとも一方で出れば今回の runtime gate は PASS です。

`NEW_RESET_AFTER_DUAL_OVERRIDE` が出た場合は、その行を含むログが次の writer 調査資料になります。F8 を押すと全書き込みを即時停止し、再起動まで無効のままです。

## 返してほしいもの

- `WallSprint.log` 全体
- A / E のどちらで横向きが見えたか
- クラッシュ、位置ずれ、transform corruption の有無
- 可能なら短い動画

元ゲームの code dump や capture ファイルは不要です。
