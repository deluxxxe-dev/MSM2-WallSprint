# MSM2 - Backup Mods 仕様書

対象: Mod-Tools v1.2.0 / 2026-10-24。配置・設定の共通仕様は
[仕様書ライブラリ](README.md)を参照。

## 目的・入出力

通常MOD、SF/WSのソース・Git履歴・未コミット作業・ログを含む共有MOD環境を、
フォルダー階層ごと私用バックアップとして保存します。保存後に全ファイルを検証します。
この明示的な共有バックアップ/復元は、WSアップデーターのWS専用操作とは別の許可範囲です。

| 項目 | 仕様 |
|---|---|
| ショートカット / リンク先 | `MSM2 - Backup Mods.lnk` / `<game>/Mod-Tools/Backup-Mods.cmd` |
| Python action / 主関数 | `backup` / `backup()` |
| 保存先の優先順 | `--backup-dir` → `mod-tools.json` → 入力（既定はホームのMSM2-Mod-Backups） |
| ゲームルート | `--game-root` → Mod-Toolsの親 → 入力 |
| 成功 | `VERIFIED BACKUP: <保存フォルダー>` を表示、終了0 |
| 失敗 | エラーを表示、終了非0。通常の捕捉可能な失敗では自分のpendingだけを除去 |
| CMDの画面 | 成功/失敗の両方でpause |

指定例: `Backup-Mods.cmd --backup-dir "<ゲーム外の保存先>"`。
この指定は今回の出力先です。既定の保存先を変更するには
`Install-Mod-Tools.cmd --backup-dir "<保存先>"` を使用します。

## 保存範囲

| ルート | 保存内容 |
|---|---|
| `Overstrike/` | 共有マネージャー本体と配下 |
| `Profiles/` | 選択状態・プロファイル・設定 |
| `Mods Library/` 全体 | 通常MOD、SF/WS本体、ソース、隠し.git、未追跡/編集中の保存済みファイル、ログ |
| `scripts/` | インストール済みDLL・付属データ・設定 |
| `Mod-Tools/` | ツールとローカル設定 |
| ルートの個別ファイル | `scripts_proxy.dll`、`winmm.dll`、存在する `scripts.txt` / `commandline.txt` |

空ディレクトリも記録します。ゲームEXEは照合用SHA256だけを記録し、本体を格納しません。
`asset_archive` 等のゲーム本体、セーブ、外部のSteam/Python/Git/WSL/コンパイラー本体や
資格情報ストアは対象外です。Gitローカル設定やログ等、対象ルート内にある個人情報は含みます。
アップロードはせず、公開配布ZIPにも私用バックアップを入れません。

## 保存処理の順序

1. ゲームと出力先を検証。出力先はゲーム内・ゲームの祖先フォルダーを拒否。
2. 共通ロック取得後、ゲーム/共有Overstrikeが終了していることを確認。
3. 共有環境、独立Git、導入済み設定のformatを確認し、対象の一覧/メタデータを採取。
   リンク等、外部Git依存、`index.lock`、未検証のlegacy lock、非通常ファイルを拒否。現行の認証済み互換lockは保存対象から除外。
4. 出力先を作成し、未圧縮合計+64MiBより多い空き容量を要求。
5. `.pending-<識別子>` 内にZIP64対応・DEFLATEレベル1で `environment.zip` を作成。
6. ファイルごとに1MiBずつ読み書きし、SHA256を算出。200ファイルごとに進捗を表示。
7. 一覧/サイズ/更新時刻/モード/空ディレクトリを再採取し、開始時との差分があれば停止。
8. ZIPを再読込し、全payloadのサイズ・CRC・SHA256とマニフェストを検証。
9. ゲームEXEのハッシュが変わっていないことを確認し、独立復元用 `KIT` 一式を同梱。
10. プロセス停止・対象一覧・ゲームハッシュをもう一度確認し、pendingを完成名へ変更。

これはVSS等による原子的なライブスナップショットではありません。エディターや更新を
止めて保存します。走査前後のメタデータ比較は、変更の完全検出を保証するものではありません。
クラッシュ/電源断でのpending自動回収やバックアップ世代の自動削除はありません。

## 出力形式・互換性

完成フォルダー名はローカル時刻の `MSM2-Mod-Environment-YYYYMMDD-HHMMSS`。
同名がある場合は `-01` 等を追加し、過去の保存を上書きしません。フォルダーを丸ごと保管します。

```text
MSM2-Mod-Environment-YYYYMMDD-HHMMSS/
  environment.zip
    manifest.json
    payload/<ルートからの相対パス...>
  Restore-Mods.cmd
  Verify-Backup.cmd
  mod_environment.py
  windows.ps1
  ...KITの残り...
```

manifestの `format` は `msm2-mod-environment-1`。
`tool_version`、`created_local`、`original_game_root`、`game_sha256`、`roots`、
`directories`、`files` を持ちます。filesは各 `path/size/mtime_ns/mode/sha256` を持ちます。
検証時はformatとルート一覧を厳密に照合し、tool_versionは文字列として検証します。
許可する上限はファイル50万・ディレクトリ50万、manifest 64MiB、未圧縮合計1TiBです。

相対パスの逸脱、Windows予約名、大小文字別名、重複/未登録エントリ、親の欠落、
リンク、暗号化エントリ、サイズ/ハッシュ不一致を拒否します。
ファイル内容・ファイル更新時刻・基本modeは保持しますが、ACL/所有者/代替ストリームや
ディレクトリ時刻を完全に保存する仕様ではありません。SHA256は破損検出用で、発行者署名ではありません。

## 付属の検証・復元

`Verify-Backup.cmd` は隣の `environment.zip` または `--archive` を全件検証します。
対象ゲームが失われていても検証でき、元リポジトリには依存しません。
ただしPython/PowerShellなどの外部実行環境は別途必要です。

`Restore-Mods.cmd` は元と同じ絶対ゲームパス・同じEXEのSHA256を要求します。
確認/一覧表示後、`RESTORE` または明示 `--apply` で適用します。
ゲーム/Overstrikeを閉じた状態で検証・ステージ展開し、現在の対象ルートを
ゲームの隣の `.MSM2-rollback-...` へ退避してからルート単位で置き換えます。
ゲーム本体は戻しません。バックアップで存在しなかった対象ルートは、現在の分を退避します。

途中失敗時は逆順に元へ戻し、ロールバック自体の失敗は両方の保全場所を示して失敗とします。
成功時も元のデータの退避先は自動削除しません。電源断をまたぐ自動ロールバック機能ではありません。
成功後は `restore_requires_install=true` にし、共有Overstrikeを開きます。
Vanillaゲームアーカイブへの再適用は `Install mods`、その後Playの `INSTALLED` 確認で進めます。
別パスへの移設・破損した元データの意味的な修復・各MODの正常動作保証は行いません。

## 改変箇所と確認項目

| 変更したい内容 | 主な実装箇所 |
|---|---|
| 保存範囲/形式 | `TREES`、`FILES`、`ROOTS`、`FORMAT`、`scan()`、`validate_manifest()` |
| 圧縮・分割・進捗・出力名 | `backup()`、`unique()`、`BUFFER` |
| 出力先/空き容量/競合 | `external_folder()`、`backup()`、`lock()`、Windowsのcheck action |
| 復元/退避/互換性 | `verify()`、`plan_restore()`、`restore()`、`save_settings()` |

範囲やformatの変更は過去バックアップを読めるか、移行が必要かを先に決めます。
Git履歴/dirty/未追跡/通常MOD/空フォルダー/読み取り専用と時刻の往復、元ゲーム消失後の
独立復元、ZIP破損と危険パス拒否、更新中拒否、途中置換失敗とロールバック失敗の保全を検証します。
Windows実機では最初の保存と再検証を優先し、健全な環境を消して復元試験をしないでください。

現行restoreはOSゲートをROOTS外で保持し、Mods Library内に開いたlock handleを残しません。
復元stageには現行KIT/互換markerを用意してから切替し、旧checker復活を防ぎます。
詳細は共通READMEのv1.2 OS所有権を参照。
