# MSM2 Mod-Tools v1.2.0

今回構築した共有MOD環境の起動・バックアップ・同じ場所への復元用ツールです。
SF/WS本体の開発・更新は、それぞれの既存アップデーターで行います。

## Overstrikeをダブルクリックで開く

現在の独立Git cloneを更新した後、
`MSM2-WS\_source\environment\Start-Overstrike.cmd` をダブルクリックします。
配置からゲームフォルダーを自動判定し、初回はツールを `<game>\Mod-Tools` に導入して
デスクトップに **`MSM2 - Overstrike`** を作成し、そのまま共有Overstrikeを開きます。
初回導入時だけゲームとOverstrikeを終了してください。

以後はデスクトップの **`MSM2 - Overstrike`** をダブルクリックするだけです。
共有Overstrike・ローダーの配置、MSM2プロファイル、WSの独立ソースと
ルートの有効scriptが1つであること、WS内の重複を生むZIP等とOS上の更新所有権を
確認して、`<game>\Overstrike\Overstrike.exe` をゲームルートの作業フォルダーで開きます。
同じOverstrikeが起動済みなら既存ウィンドウを前面に出します。
起動時のGit更新・ビルド・バックアップは実行せず、通常MODやSF/WSのファイルも変更しません。
配置不整合はエラーを表示して停止し、ファイルの削除・移動による修復は行いません。

ソースをゲーム外へ展開した場合は初回のみゲームフォルダーを入力します。
明示指定も可能です。

```powershell
.\Start-Overstrike.cmd --game-root "<ゲームフォルダー>"
```

バックアップ未設定ならホームの `MSM2-Mod-Backups` を既定値として登録します。
この操作ではバックアップを作成しません。既存の保存先・プロファイル・復元後確認設定は保持します。
保存先を変更する場合は下記の `Install-Mod-Tools.cmd --backup-dir` を使用できます。

## 最初の導入

ゲームとOverstrikeを終了し、このツール一式を**Mods Libraryの外**へ展開します。
`Install-Mod-Tools.cmd` を実行し、ゲームEXEのあるフォルダーと保存先を入力します。
例のゲームフォルダーは次の場所です。

```text
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2
```

保存先には、ゲームフォルダーと別の場所を指定します。別ドライブのフォルダーを
選ぶとゲームのあるドライブが故障した場合にも使えます。

ZIPをブラウザーから保存した場合、Windowsのプロパティで信頼するZIPの「許可する」を選んでから展開してください。既存Git checkoutからの導入も使えます。

Python 3.11以降とWindows標準PowerShellが必要です。現在の環境のPythonで動きます。
このツールの起動・保存・復元自体に、Git・WSL・MSVC・ビルドは必要ありません。
それらの外部開発ツール本体・Steam・セーブデータは、このバックアップに含めません。
PC全体を再インストールした場合、開発の再開には別途その前提環境が必要です。

導入先は `<game>\Mod-Tools` です。デスクトップに以下の3つを作成します。

- `MSM2 - Overstrike`：環境とWS配置を確認して共有Overstrikeを起動。
- `MSM2 - Play Modded`：導入済みMODでゲームを起動。
- `MSM2 - Backup Mods`：現在の共有環境を保存・検証。

導入時に選択されているMSM2用Overstrikeプロファイルを保存して使用します。複数候補があり選べない場合は
`Install-Mod-Tools.cmd --profile "プロファイル名"` で指定できます。

## 普段の起動

`MSM2 - Play Modded` をダブルクリックします。Overstrikeの `Run modded game` と同じ
ゲームEXE・作業ディレクトリ・`-scripts` 指定で起動します。Steamは通常どおり使用します。
すでに起動している同じゲームは重複起動せず、既存のウィンドウを前面に出します。

MODを追加・更新して適用するときはデスクトップの `MSM2 - Overstrike` から
共有Overstrikeを開き、`Install mods` を実行します。プロジェクトのコードを更新する
`Update-SF.cmd` / `Update-WS.cmd` は従来どおり別操作です。

## バックアップ

ゲーム・Overstrike・アップデーターを終了し、編集中のファイルを保存してから
`MSM2 - Backup Mods` を実行します。ゲームやOverstrikeが残っている場合は停止し、
勝手にプロセスを強制終了しません。

次を、隠しファイル・空のフォルダーも含め、階層を維持して保存します。

| 対象 | 内容 |
|---|---|
| `Overstrike` | 共有マネージャー本体と配下のファイル |
| `Profiles` | MOD選択状態、プロファイル、Overstrike設定 |
| `Mods Library` 全体 | 通常MOD、SF/WS、両方の `_source/.git` と未コミットの作業、ログ、アーカイブ |
| `scripts` | インストール済みスクリプト・設定・付属データ |
| `scripts_proxy.dll`, `winmm.dll` | ゲームルートのローダー |
| `scripts.txt`, `commandline.txt` | 起動設定（存在するもの） |
| `Mod-Tools` | このツールとローカル設定 |

`Spider-Man2.exe` や `asset_archive` などゲーム本体は保存・上書きしません。
ゲームEXEはSHA256を記録して、復元先のビルドを照合します。
バックアップ中に一覧・サイズ・更新時刻・モードの変化を検出した場合は公開せず失敗にします。
個々のMODの動作を検証する機能ではなく、保存したファイルの完全性を検証します。

保存先には `MSM2-Mod-Environment-YYYYMMDD-HHMMSS` フォルダーができます。

```text
MSM2-Mod-Environment-YYYYMMDD-HHMMSS\
  environment.zip
  Restore-Mods.cmd
  Verify-Backup.cmd
  mod_environment.py
  windows.ps1
  ... ツール一式 ...
```

ZIP内の全ファイルをCRCとSHA256で再検証してから完成扱いにします。
同時刻の保存でも過去のバックアップを上書きしません。
このフォルダーを**丸ごと**保管してください。Git設定・ローカル作業・ログを含む個人用の
バックアップです。このツールがバックアップをGitHubなどへ送信することはありません。

## Vanillaへの復元

1. Steamで、バックアップ時と**同じゲームビルド**を、**元と同じパス**へ用意します。
2. ゲームとOverstrikeを終了します。
3. 保存してあるバックアップフォルダーの `Restore-Mods.cmd` を実行します。
4. 表示された対象を確認し、`RESTORE` と入力すると展開・復元します。
5. 自動で開いた共有Overstrikeで `Install mods` を実行し、通常MODもゲーム本体へ再適用します。
6. `Play-Modded` で一度だけ `INSTALLED` と確認し、通常の起動へ戻ります。

バックアップ検証だけなら `Verify-Backup.cmd` を実行します。
復元前に全データを検証し、別のステージへ展開・照合してから置き換えます。
既存の対象フォルダー・ローダーは、ゲームフォルダーの隣の `.MSM2-rollback-...` に
退避します。途中の置き換えに失敗したら元へ戻します。ロールバック自体が失敗したときは
両方の場所を表示して保全し、完了とは表示しません。成功時の退避も自動削除しません。

復元したのはMOD環境のファイルです。VanillaのゲームアーカイブへMODを反映する
最後の工程はOverstrikeの `Install mods` が行います。破損したゲーム本体はSteamで修復し、
同じEXEのハッシュになってから復元します。別パスへの移設はプロファイル・外部MODの
不透明な絶対パスを壊すため、この版では受け付けません。

ファイルの内容・名前・階層・ファイル更新時刻を保存します。Windows ACL、所有者、
ジャンクション/リンク/代替ストリーム、WSLディストリビューションのエクスポートは含みません。
リンクを含む環境、外部Gitオブジェクト依存、危険なZIPパスは処理を拒否します。

## コマンドライン

```powershell
.\Install-Mod-Tools.cmd --game-root "<ゲームフォルダー>" --backup-dir "<外部保存先>"
.\Backup-Mods.cmd
.\Verify-Backup.cmd --archive "<保存フォルダー>\environment.zip"
.\Restore-Mods.cmd --archive "<保存フォルダー>\environment.zip" --game-root "<元のゲームフォルダー>"
```

スクリプトから明示的に復元するときだけ `--apply` を指定します。指定しなければ
検証・対象表示後に `RESTORE` の入力が必要です。
SHA256は破損検出用で、バックアップの作者を認証する署名ではありません。
自分で作成したバックアップを使用してください。

## 検証と出典

所有する一時データで、Git履歴/未コミット作業/通常MODを含む往復復元、破損・ZIPパス攻撃・
リンク・異なるEXE/復元先・プロセス稼働・途中失敗の拒否とロールバックを検証します。
クラウドでゲームや共有Overstrikeそのものは起動していません。
実機のショートカット・起動・バックアップ読み取り・Vanillaへの復元は実機確認が必要です。

通常起動の仕様はTkachov/Overstrikeの `MainWindow.LaunchGame`、プロファイル形式は
`Profile`/`AppSettings` を参照（commit `ab61de27a4ba76d92780178f600f73f15bcddd81`）。
外部の実装・バイナリはこのツールにコピーしていません。

Shared dependency clarification (2026-10-31): preserve root scripts_proxy.dll/winmm.dll; allow regular non-reparse Overstrike/scripts_proxy.dll as installer source. Overstrike/winmm.dll remains refused; no launcher-side relocation/deletion. No automatic DLL copying/downloading/deletion or new source/loader hash-equality requirement. See docs/mod-tools/OVERSTRIKE.md.


## v1.2 OS所有権と互換lock

`environment/operation_lock.py` が正本です。`.MSM2-Operations.guard` をWindowsの
CreateFile共有排他で保持し、閉じる/プロセス終了でOSが解放します。ビルド子へ明示的に
ハンドルを継承し、PIDと開始identity、token、workspace、phase、WSL distroを監査記録に保持。
`.MSM2-Mod-Tools.lock` / `Update-WS.lock` は旧版を止める互換記録として残り得ます。
**ファイルを削除する必要はありません。存在はactiveの証拠ではありません。**

OS busyなら実所有が残っています。PID/start/action/phaseを確認し、子ビルドが保持して
いる可能性も示します。旧empty/PID lockは年齢だけで回収せず、Windows operation processと
全running WSL distroのbuild停止を確認できた場合だけ `.MSM2-lock-recovery/<token>` に
元bytesを保存して移行。PID再利用は開始identityで区別。legacy PID-onlyで現PIDが生きて
いる場合、malformed/reparse/所有者変更/WSL検証不能は保持・具体的理由で拒否します。
`index.lock` は自動削除しません。プロセスの自動kill、DLLの移動/削除/属性修正もしません。

新WSLビルドはdistroとtokenを固定。親の異常終了後、Windows子のOSハンドルが残れば
次操作は取得できません。WSLホストまで終了してLinux子が残った場合も、未解決build
記録を保持し、同tokenのプロセスが消えた証拠が得られるまで再開しません。プロセス環境を
読めない場合は成功扱いにしません。手動の外部Git/ビルドは同時実行しないでください。

Backup/Restoreも同じゲートです。互換lockはsnapshot対象外。復元時は現在のKITをstageに
保持し、古いbackupから旧ロック方式を再導入しません。mods/profile/復元確認設定の既存
保全・rollbackは維持。KIT旧bytesはbackup/退避先に残り、全体cleanupはしません。
実機で健全な環境を消す復元試験は要求しません。

修復の一括実行例: docs/mod-tools/README.md「一度だけのtooling修復」。


### r5 post-build WSL確認の切り分け

ビルド/テスト/package完了の行は配置完了ではありません。原因別PID/stage/category/
errno/exitとdistro/UIDの診断はdocs/mod-tools/README.mdの一括ブロックから実行できます。
古いcheckerを通る前に最新コードをstageするDiagnoseWSLモードです。lock/KIT/runtime/
worktreeは変更せず、process/WSLを終了しません。bootstrap fetchのみGit refs/objects更新。
実機PID339の原因は未確定。retryはフルビルド、既存stagingの手動配置はしません。


### 確認済みEACCESからの現在の回復

前回DiagnoseWSLでenviron EACCESが確定したため、同じ診断を繰り返しません。
共通mod-tools README「現在のEACCES回復」の通常修復が次操作です。
正確なtokenの不存在を選択distro/root/全UIDで読み取り確認し、既存OS所有権とGit保護の下で
KITだけを修復します。user QUIETでもescaped UIDの子をrootで確認。権限不明/root失敗は
拒否、旧UIDnullは未記録のままです。root build/配置や設定変更は行いません。
TOKEN_ABSENTは配置完了ではなく、後のUpdate-WSは再ビルドします。実機受入れ待ちです。
