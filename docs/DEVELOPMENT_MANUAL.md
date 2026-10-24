# MSM2 WallSprint 開発・運用マニュアル

最終更新基準: 2026-10-24

この文書は WallSprint の実装、逆解析、ローカル workspace、GitHub 運用、バックアップ、エラー対応、GPT/Codex への引き継ぎを一つにまとめた運用中枢です。

目的は、将来 GPT / Codex が別セッションから参加しても、古い `main`、古い ZIP、旧 D-drive workspace、過去の runtime 仮説などへ誤って戻らず、現在の正しい開発系列から安全に継続できるようにすることです。

---

## 1. 最重要ルール

### 1.1 GitHub 上の正本

- Repository: `deluxxxe-dev/MSM2-WallSprint`
- Default branch: `main`
- 現在の実装 source of truth: `codex/v01c20-edi-value-provenance`
- この文書作成時の基準コミット: `5192f05`

`5192f05` は作成時点の基準であり、将来この branch に正常な後続 commit が追加された場合は最新 HEAD を優先します。

### 1.2 `main` を現在 branch へ merge しない

2026-10-24 の点検では、現在 branch と `origin/main` は大きく分岐していました。

確認時点:

```text
current branch only: 162 commits
main only:           83 commits
```

さらに `merge-tree` で以下などに多数の conflict を確認済み:

- `AGENTS.md`
- `docs/CODEX_NEXT.md`
- `docs/DEV_RULES.md`
- `docs/PROJECT_STATE.md`
- `docs/LOGGING_CONVENTION.md`
- `analysis/V01C10_RUNTIME_RESULT.md` ～複数世代

したがって、通常作業として `main` を `codex/v01c20-edi-value-provenance` へ merge しないでください。

重要な補足:

共通祖先以降の `main` 側変更を調べたところ、`source/` への独自変更は確認されず、主に docs / analysis / AGENTS の進捗記録でした。一方、現在 branch の `source/` には大量の実装・テスト・解析コードが積み上がっています。

つまり、現在の実装はこの Codex branch を正とし、`main` の文書履歴だけを理由に実装を巻き戻さないこと。

---

## 2. 作業開始時の必読順

最低限:

1. `docs/DEVELOPMENT_MANUAL.md`（この文書）
2. `AGENTS.md`
3. `docs/DEV_RULES.md`
4. `docs/LOCAL_WORKSPACE.md`
5. `docs/PROJECT_STATE.md`
6. `docs/CODEX_NEXT.md`
7. `docs/CODEX_RESULT.md`
8. `docs/LOGGING_CONVENTION.md`
9. `docs/CODEX_NEXT.md` が参照する `analysis/` files

現在タスクの境界は `docs/CODEX_NEXT.md` を正とします。

runtime evidence は関連する `analysis/*RUNTIME_RESULT*.md` を読み、測定値、静的解析、仮説を混同しないこと。

---

## 3. 役割分担

### ChatGPT

- runtime log 解釈
- reverse-engineering 方針
- evidence の整理
- 安全な次タスクの設計
- docs/handoff の整備
- Codex への bounded instruction
- merge / Git / workspace / backup の判断支援

### Codex

- 実装
- code-level investigation
- build / test
- smoke / fixture
- analysis note
- `docs/CODEX_RESULT.md`
- commit / push

### User

- 実ゲームテスト
- Overstrike 操作
- runtime log 提供
- gameplay / feel の判断
- 最終製品判断

raw log を毎回 Codex に直接読ませる必要はありません。通常は ChatGPT が runtime evidence を抽出し、analysis file と次タスクへ落とします。

---

## 4. 現在のローカル Windows workspace

現在の実機参照パス:

```text
Game root:
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2

WallSprint workspace:
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\Mods Library\Mods-Developing\MSM2-WS

Git source clone:
C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2\Mods Library\Mods-Developing\MSM2-WS\_source
```

個人の絶対パスは「現在機の参照」であり、再利用可能 script へハードコードしないこと。

最終レイアウト:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\
├─ MSM2-WS-v<version>.script
├─ Update-WS.cmd
├─ logs\
├─ _source\
└─ _archive\
```

一回限りの migration entry:

```text
Migrate-WS.cmd
```

通常更新:

```text
Update-WS.cmd
```

詳細な authoritative layout は `docs/LOCAL_WORKSPACE.md`。

---

## 5. Workspace 不変条件

- `_source` は独立 Git clone
- old D-drive checkout への worktree/junction/symlink/reparse dependency を作らない
- old environment を自動削除しない
- WS 配下の active `.script` は再帰的にちょうど 1 個
- active `.script` は workspace root のみ
- `_source`, `logs`, `_archive` に active `.script` を残さない
- build intermediates は `.script.disabled`
- archive は `.zip.disabled` 等で scanner から隠す
- visible `.zip/.rar/.7z` を WS workspace 内へ残さない
- SF (`MSM2-SF`) や ordinary mods を WS updater が変更しない

Overstrike は archive 内まで再帰 scan するため、「ZIP の中だから安全」と考えないこと。

---

## 6. Shared Overstrike

唯一の shared executable:

```text
<game>\Overstrike\Overstrike.exe
```

WorkingDirectory:

```text
<game>
```

禁止:

- WS 専用 Overstrike
- AppData 正本化
- `<game>\Overstrike\winmm.dll`
- WS だけ別 Mods Library/profile を作る

SF / WS / ordinary mods は同一 UI で扱う前提です。

---

## 7. Git 日常運用

### 7.1 正しい remote

```text
https://github.com/deluxxxe-dev/MSM2-WallSprint.git
```

旧 personal owner URL を再導入しないこと。

`source/tools/workspace.py` の remote URL は Organization へ修正済みです。

### 7.2 local fetch policy

2026-10-24 に local `_source` の `remote.origin.fetch` が特定 Codex branch だけを取得する設定になっていることが判明し、通常の全 branch fetch へ修正済みです。

正常:

```text
+refs/heads/*:refs/remotes/origin/*
```

再発確認:

```powershell
git config --get-all remote.origin.fetch
```

修正:

```powershell
git config remote.origin.fetch "+refs/heads/*:refs/remotes/origin/*"
git fetch origin --prune
```

これは local Git config であり、GitHub 上には保存されない点に注意。

### 7.3 作業開始

```powershell
git switch codex/v01c20-edi-value-provenance
git fetch origin --prune
git status -sb
git branch -vv
```

期待:

```text
## codex/v01c20-edi-value-provenance...origin/codex/v01c20-edi-value-provenance
```

### 7.4 `main` との差を見たい場合

非破壊確認のみ:

```powershell
git rev-list --left-right --count HEAD...origin/main
```

merge 可否確認:

```powershell
git merge-tree --write-tree HEAD origin/main
```

ただし現在は conflict 多数が確認済みなので、通常作業として merge しない。

---

## 8. Current branch の実装を守る理由

`source/` 比較では現在 branch 側に多数の追加/変更が存在します。

例:

- movement lineage
- writer entry snapshot
- RDX source contract
- path A/B proof
- source lifecycle
- native source override
- smoke tests
- workspace tools
- packaging / release helpers
- c1～c20 analysis support

2026-10-24 の比較で `source/` は 226 files 規模の差がありました。

このため、「main が default branch だから最新実装」とみなすのは誤りです。

---

## 9. Reverse engineering / runtime evidence のルール

WallSprint は blind patch を避け、証拠ベースで進めます。

必須原則:

- exact-build guard
- fail-closed
- measured RVA/AOB
- runtime evidence
- bounded diagnostics
- ABI/signature を証拠無しに invent しない

以下を分離して記録:

1. measured fact
2. static-analysis conclusion
3. hypothesis

仮説を「確認済み offset」に昇格させないこと。

---

## 10. Runtime log の handoff

ユーザーからの full raw log は通常 ChatGPT が読む。

ChatGPT は必要な evidence を:

```text
analysis/
```

に task-specific な形で残します。

Codex は `docs/CODEX_NEXT.md` が参照する analysis file を読む。

raw log が追加で必要な場合は、Codex は:

- exact event
- field
- timestamp window
- log key

を指定し、無方向に「全部もう一度ください」としない。

runtime log directory:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\logs
```

filename:

```text
MSM2-WS-v<version>-YYYYMMDD-HHMMSS.log
```

---

## 11. Build / update / migration

通常ユーザー操作は `Update-WS.cmd`。

重要な流れ:

1. running game / shared Overstrike 確認
2. `_source` integrity
3. intended branch/upstream
4. safe fast-forward update
5. build
6. tests
7. package validation
8. exactly one active script deployment
9. old WS scripts disable/archive
10. recursive active script verification
11. shared Overstrike preflight
12. launch shared Overstrike
13. user `Install mods`
14. user `Run modded game`

失敗時は prior working package を不必要に破壊しないこと。

Migration は old workspace を削除せず、新 workspace が完全に成立した後だけ manual removal readiness を報告します。

### 11.1 配布リンクの案内方針（2026-10-25）

バージョンの引き渡しでは、GitHubの閲覧ページを経由せず、クリックでZIP本体を
取得する直リンクを優先します。コミット/リリースに固定し、テスト手順は別途
GitHub上で開けるリンクを添えます。公開後にHTTP GET・ZIP CRC・記録したSHA256を
照合し、Git clone/push成功だけでブラウザーでの取得成功を主張しません。
クラウドでの取得成功と利用者ブラウザーのアクセス権は区別します。

直リンクが利用できない場合はその状態を明示し、既存Update-WS/認証済みGit経路で
取得できるように案内します。プレビューや404を直ダウンロードとして扱わず、
公開範囲の変更や私用バックアップの公開によって解決しません。

---

## 12. GitHub Actions

2026-10-24 時点で WallSprint の current/default tree には `.github/workflows` による継続 CI は設定されていません。

したがって SF と同じ self-hosted runner workflow が WS にあると仮定しないこと。

将来 CI を追加する場合:

- current branch policy を明示
- headless test と real-game test を分離
- game runner を push CI の必須条件にしない
- exact Windows/WSL dependencies を文書化

---

## 13. ローカル GitHub バックアップ

WS も SF と同じ独立 backup infrastructure に含まれます。

Base:

```text
D:\ajita\Backup\Github
```

Mirror:

```text
D:\ajita\Backup\Github\MSM2-WallSprint.git
```

Snapshots:

```text
D:\ajita\Backup\Github\Snapshots\MSM2-WallSprint\
```

State:

```text
D:\ajita\Backup\Github\State\MSM2-WallSprint.refs
```

Log:

```text
D:\ajita\Backup\Github\backup.log
```

### Backup behavior

- mirror update
- last successful State refs と current refs を比較
- changed 時のみ bundle 作成
- `git bundle verify`
- success 後のみ State 更新
- latest 3 generations を保持

mirror update 後に snapshot 作成失敗しても、次回再試行できる State 方式を維持すること。

---

## 14. Scheduled Task

Task name:

```text
GitHub Backup
```

Action:

```text
powershell.exe -NoProfile -ExecutionPolicy Bypass -File D:\ajita\Backup\Github\GitHubBackup.ps1
```

周期:

```text
6 時間ごと
```

確認済み:

- `LastTaskResult = 0`
- `StartWhenAvailable = True`
- `Enabled = True`
- `RunOnlyIfIdle = False`
- `WakeToRun = False`

変更が無い場合は:

```text
No changes: MSM2-WallSprint
```

となるのが正常。

---

## 15. PowerShell / Git backup の既知注意点

Windows PowerShell 5.1 は native stderr を `NativeCommandError` 化することがあります。

特に `git bundle verify` の正常メッセージを error と誤認しないこと。

backup script は `$LASTEXITCODE` を実判定に使います。

---

## 16. Backup からの復旧

current workspace を先に消さないこと。

原則:

1. remote GitHub が正常なら remote から fresh clone
2. remote loss/corruption 時だけ mirror/bundle を利用
3. bundle verify
4. temporary location に clone
5. branch/history/files を確認
6. intended branch を確認
7. workspace へ復旧

例:

```powershell
git clone "D:\ajita\Backup\Github\Snapshots\MSM2-WallSprint\<timestamp>.bundle" <temporary-folder>
```

復旧後も current development branch は `codex/v01c20-edi-value-provenance` 系列を確認すること。

---

## 17. よくある問題

### `git fetch origin` で current branch しか見えない

`remote.origin.fetch` が branch 限定に戻っていないか確認。

### `HEAD...origin/main` が大量 divergence

既知状態。即 merge しない。

### `merge-tree` で docs/analysis に conflict

既知状態。現在 branch を維持。

### Overstrike が archive 内 script を拾う

visible ZIP/RAR/7z が workspace 内に無いか確認。`.zip.disabled` policy を守る。

### active script が複数

再帰的に WS subtree を確認し、root 以外の active `.script` を残さない。

### Update が dirty/detached/diverged で止まる

fail-closed が正常。無理に reset/force せず、まず `git status -sb`, branch, upstream, local changes を確認。

### runtime behavior が変わらない

package version、active script、Overstrike selection、Install mods 実施、fresh runtime log を順に確認。

---

## 18. GPT / Codex への標準引き継ぎ

新しいセッションには最低限:

```text
Repository: deluxxxe-dev/MSM2-WallSprint
Development branch: codex/v01c20-edi-value-provenance

まず AGENTS.md と docs/DEVELOPMENT_MANUAL.md を読み、
docs/DEV_RULES.md / docs/LOCAL_WORKSPACE.md / docs/PROJECT_STATE.md / docs/CODEX_NEXT.md を確認すること。
main は大きく分岐し、merge conflict 多数確認済みなので current branch へ merge しないこと。
GitHub 上の current development branch 最新 HEAD を source of truth とする。
```

Codex は実装開始前に:

```text
git status
git branch -vv
git remote -v
```

相当を確認し、自分が intended branch にいることを証明する。

---

## 19. Codex 完了条件

タスクは artifact/ZIP を生成しただけでは完了しません。

最低限:

1. intended source changes
2. tests/build result
3. `docs/CODEX_RESULT.md`
4. branch commit
5. push
6. real-game pending items
7. exact package/output name
8. user-side next test

次の isolated Codex session が GitHub だけから再現できることを完了条件とします。

---

## 20. 禁止事項

- current branch を `main` で安易に merge/overwrite
- force-push で current history を書き換える
- old D-drive environment を自動削除
- SF subtree を WS tooling で変更
- ordinary mods を cleanup
- visible archive を WS workspace 内へ残す
- third-party code/assets/binaries を無断コピー
- runtime evidence 無しで ABI/offset を決め打ち
- raw full game function / logs を不用意に public-ready repo へ蓄積
- dirty/diverged checkout を updater が勝手に reset
- backup success 前に State 更新

---

## 21. この文書を更新すべき変更

以下が変わったら更新します。

- canonical repo / org
- implementation source-of-truth branch
- main/current branch relationship
- local workspace layout
- remote/fetch policy
- Overstrike placement
- package/archive policy
- log location/naming
- build/update/migration entry points
- CI introduction
- backup design
- handoff discipline

細かい c20/c21 実装進捗は `PROJECT_STATE` / `CODEX_NEXT` / analysis を更新し、この文書を daily log 化しないこと。

---

## 22. 2026-10-24 に確認済みの健全性

- repo は `deluxxxe-dev/MSM2-WallSprint`
- local fetch/push remote は Organization URL
- current branch と origin tracking branch は同期
- workspace tool remote URL は Organization へ修正済み
- local fetch policy は全 branch fetch へ修正済み
- `main` と current branch の大規模 divergence を確認
- `main` の current merge は conflict 多数のため禁止判断
- 共通祖先以降 `main` 側に `source/` 独自変更無しを確認
- current branch の implementation を source of truth と確認
- GitHub mirror backup 正常
- WS snapshot bundle 作成/verify 正常
- latest 3 generations policy 正常
- Scheduled Task backup 正常

将来障害が起きた場合は、この既知正常状態との差分を最初に探します。
