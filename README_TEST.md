# WallSprint v0.1c6 — Planar Adhesion Diagnostics

Package **0.1.24**。c5 で Deinit までの姿勢保持が成功したため、壁からの距離を調べる段階へ進みました。**この c6 は Position 読み取り専用です。** 安全な Position 経路の証明が未完了のため、実際の吸着補正は行いません。姿勢変更は c5 を維持しています。

## ダウンロード・導入

配布は **WallSprint_v0.1c6_planar_adhesion.zip**。完了メッセージの GitHub 直接ダウンロードリンクから取得してください。ゲームを完全終了し、古い WallSprint を無効にして ZIP を展開します。中の **WallSprint.script は解凍せず** Overstrike へ追加して Install mods。従来の Script support、`-scripts` と配布 INI を使います。

## 起動確認

ゲームのインストール先の `logs\WallSprint.log` を確認します。

```text
WallSprint v0.1c6 package_version=0.1.24
target_build_gate=ACCEPTED
PATH_B_COMPACT3X3_PROOF admitted=1 ... decoded_prefix_bytes=444 caller_decoded_bytes=128 pairs=9
DUAL_SOURCE_ADMISSION admitted=1
PATH_A_POSITION_PROOF admitted=0 ... reason=... position_source_write=0
```

**Position proof の admitted=0 は今回の想定です。** reason、元関数の認証、解析結果と残る未証明部分を記録します。続く `PATH_A_POSITION_ACCESS` / `PATH_A_POSITION_CONTROL` は候補情報で、書き込み許可ではありません。既存の Path B proof / dual-source admission が失敗した場合は WallRun 試験を進めずログを送ってください。

## 今回の試験

**c5 と同じ大きな A 方向の平らな壁**で、左→右へ約3〜4秒の連続 WallRun を1回行ってください。Crawl、マーカー操作、E の試験は不要です。

次を確認してください。

- Surface Up への回転と WallRun 終了までの保持が維持されるか。
- 新しいスナップ、揺れ、壁へのめり込み、急な外向きの飛び出しがないか。
- クラッシュや位置破損がないか。

ゲームを終了し、**起動部分を含む完全な WallSprint ログを、非公開のまま通常どおり ChatGPT に送ってください**。見た目の結果も添えてください。

## 確認するログ

```text
PATH_A_POSITION_RUNTIME ... current_thread_stack_verified=1 ... position_row_preserved=1 position_source_write=0
ADHESION_SAMPLE ... valid=1 ... distance=... target_distance=0.5 error=... correction_requested=... correction_applied=0 position_source_write=0
ORIENTATION_LIFECYCLE_SUMMARY ... source_active_until_deinit=1 verified_deinit=1 ... retention_pass=1
ADHESION_SUMMARY ... corrections_applied=0 ... source_position_writes=0 ... end_reason=verified_native_Deinit
```

`correction_requested` は計算上の候補で、実際には適用しません。transition 完了後、誤差0.01m以内は0、誤差0.35m超も0、その他は gain0.25・上限0.02mです。自然に約0.5mを保っている壁では、ほとんど候補が出なくても正常です。無理に壁から離れたり、位置を動かして見た目の差を作る必要はありません。

距離は HeroPosition / P338 / N398 の読み取り値です。summary の error_after は補正していない同じ観測値で、改善を示す値ではありません。Position/W の意味と後続処理が証明されるまでは、INI を変更しても Position 書き込みは有効になりません。

c4/c5 の複数スレッド監視は約1.50秒で終了し、姿勢保持とは独立しています。`MULTITHREAD_UP_WATCH_RESTORE ... cleanup_pending=0` も確認します。未知 writer の捕捉は今回の必須条件ではありません。

**F8 は再起動まで書き込み・監視 OFF、F9 は script 停止**。再試験は完全再起動してください。既存の source 開始後5秒・各 path4096回の上限は維持しています。

生ログ、動画、`THIRD_WRITER_CODE ... private_do_not_distribute=1` の命令列を GitHub・公開 ZIP に追加する必要はありません。movement / animation / camera / gravity は追加していません。
