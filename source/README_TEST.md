# WallSprint v0.1b8 — Dual-Source Orientation Ownership

Package **0.1.17**。二つの確認済み WallRun source 経路へ同じ補間姿勢を渡す、orientation-only 版です。**b8 の実機保持は未確認**。adhesion / movement / animation / camera / physics は追加しません。

## 導入

ゲームを完全に終了し、古い WallSprint を無効化。**WallSprint_v0.1b8_dual_source_ownership.zip** を展開し、中の **WallSprint.script は解凍せず** Overstrike へ追加、Install mods。従来の Script support と `-scripts` で起動します。別 SDK・参考 MOD は不要です。

対象は Spider-Man2.exe **2.810.0.0**、SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`。別 build、未認証 writer/caller、未知 source layout は fail closed。配布 INI の変更は不要。

## A / E テスト

地上で約2秒静止後、現在の Control A の平らな壁で左→右の横 WallRun を1回、約1.5秒。離脱して地上で約2秒静止し、同じ既知の斜め Face E で左→右を1回、約2秒。その後ゲームを終了。壁の長さに合わせて構いません。Crawl・マーカーキーは不要です。

録画は可能なら添付し、**未加工の logs/WallSprint.log と明示した A/E の session 対応**を提出してください。runtime log が保持判定の基準です。クラッシュ、Position 異常、姿勢が戻るタイミングも報告してください。

成功すれば壁に対して姿勢が保持され、world-up のカメラに対して横向きに見える可能性があります。Animation が不自然でもこの段階では許容します。見た目だけで保持成功と判断しません。

## ログと停止

起動時に `RESET_COPY_HOOK admitted=1 installed=1` と `DUAL_SOURCE_ADMISSION admitted=1 dual_source_required=1 copy_hook_authenticated=1` が必要です。第二 writer は元444bytesの SHA / runtime boundary / exact caller を認証してから hook します。

`COMMON_SOURCE_OVERRIDE`、`LATE_COMMON_WORLDUP_OVERRIDE=1`、`RESET_COPY_SOURCE_ENTRY`、`RESET_COPY_SOURCE_OVERRIDE` を確認。第二 source はその関数自身の raw entry と stack/layout を検証し、orientation48bytesだけ変更します。`position_unchanged=1` が必要です。拒否された entry は raw args / stack bounds と理由を残し、書き込みません。

補間完了後の `ORIENTATION_RETENTION_PASS=1`（held_ms>=300 または held_updates>=20）が主ゲートです。同一 update の複数呼び出しを20回と数えません。A/E 両方で保持し、未知 reset・クラッシュ・Position 異常がないことを確認します。後の `NEW_RESET_AFTER_DUAL_OVERRIDE` や sampled retention loss は未解決として提出してください。

primary thread の監視は約0.60〜1.50秒で、同じ Up の繰り返しを省略します。`PREARM_STATUS first_override=1` の prearm=0、dropped、0件も加工せず提出。watch の期限/coverage外を「未知 writer なし」とは判断しません。

**F8 は source と診断を再起動まで OFF、F9 は script 停止**。再試験はゲームを完全に再起動してください。reset writer の NOP/skip、direct Actor/Position 書き込み、吸着・移動の修正は行いません。
