# v0.1c20 EDI診断版 — 検証と実機手順

対象: v0.1c20 / package0.1.38。実装ブランチ:
`codex/v01c20-edi-value-provenance`。今回の基点は `b49425e`。
正確な実装/配布コミットと生成物ハッシュは `docs/CODEX_RESULT.md` と
`analysis/V01C20_RELEASE.json` に記録します。

## 今回の変更

CODEX_NEXTの再開指示に従い、9057d24に準備済みの限定的EDI値由来解析を、
実機A/Eテスト用に有効化します。新しい解析アルゴリズムや観測フックは追加しません。
`writerAlias.edi.enabled` は既存の `calleeCurrent` 認証結果を使用し、
正しい現在のcallee identityがなければ有効になりません。
既存の `MOVEMENT_CALLEE_VALUE_CONTRACT` に `enabled` を追加しました。

既存のexeハッシュ、callsite、callee開始/終了/全体ハッシュ、現在コード再検査、
局所CFG、ソース/Transformの別領域、TEB/image保護、所有権等の条件は維持します。
ローカル値の成功をwriter全体のeffect完了と見なしません。
Position/source-position/入力/移動の書込みとproducer0x351EB00追跡は許可しません。

## 今回実行したフル試験

| 検証 | 結果 |
|---|---|
| Windows x64 cross-build / warnings-as-errors | PASS |
| portable ASan/UBSan | 13 PASS |
| DLL / Windows filesystem logger smoke（Wine） | 7 PASS |
| hooks/native/c19/EDI/workspace | 119 PASS、5 private-input SKIP |
| 合計 | **139 PASS、5 SKIP** |
| 上記に含むEDI値由来 | 6 PASS |
| 上記に含むworkspace Git/ファイル操作 | 13 PASS |

実行したコマンド:

```sh
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
```

EDI試験はraw entry-register低32bitのコピー、定数/メモリ/stack退避/正確なaffine関係、
全局所分岐の一致/不一致、部分レジスター/chain上限/部分上書きの拒否、
後続CALL/読み戻し依存と値/全effect coverageの分離、前段CALL/後方分岐/未認証拒否を含みます。
owned fixtureは実ゲーム関数の再構成でも、実ゲームでの値由来の証明でもありません。
5件のSKIPは非公開の過去game-body/capture再生入力がないためで、独立試験は実行しています。
Wineの補助winedeviceに関する出力はありますが、上記の各スイートは終了0です。

配布生成と独立ソース再現の正確な結果はCODEX_RESULT/V01C20_RELEASEに追記します。
この表の試験数と、別に実行する配布検証/再現試験の数を合算して水増ししません。

## 更新と実機A/E

既に移行済みの環境ではゲームとOverstrikeを閉じ、`MSM2-WS/Update-WS.cmd` を実行します。
acceptedフル試験・検証・配置を経て起動する共有Overstrikeで通常MOD+SF+WSを確認し、
`Install mods` → `Run modded game` へ進みます。
`MSM2 - Overstrike` のデスクトップ入口は普段の起動用で、WSソースの更新はしません。
初回環境だけはREADME_TEST/LOCAL_WORKSPACEのMigrate手順を使用します。

1. A/cardinalの平らな壁で約4秒WallRun。
2. WallRunを完全に終える。
3. 移動またはファストトラベル。
4. E/diagonalの壁で約4秒WallRun。
5. 同じ起動の最新の完全な `MSM2-WS/logs/MSM2-WS-v0.1c20-YYYYMMDD-HHMMSS.log` を
   非公開で共有する。動画は見た目の異常時のみ。

更新失敗時は既存の有効scriptを保全し、表示された失敗出力を確認します。
公開ZIPはMods Library外へ保存します。`_source`/`_archive`へ通常ZIPを置きません。

## ログの確認項目と未確定事項

- VALUE_CONTRACTの `enabled=1` は診断の有効化で、value exact成功を意味しません。
- producer RVA/kind、origin kind/root/offset/constant/addend、VALUE_DEFINITION/CHAIN、
  局所経路の一致・完了、最初の未解決命令と正確な拒否理由を確認します。
- raw register bitsをABI型/ポインターの意味に昇格させません。producer RVA0は
  callee入口値を示すことがあり、ローカルproducer命令の存在を作りません。
- `all_reaching_states_examined` と `all_writer_call_states_examined` を分け、
  WRITE/ALIAS/WRITERの完全性、後続読み依存/blockerを併せて確認します。
- 真のTransform書込みに `transform_no_write_proven=1` を与えません。
- 正常Deinit、retention、Surface Up、stale/hard kill、Position writes=0、
  entry/map最終化、cleanupを従来どおり確認します。

**現在の実機EDI producer/origin/後続依存/XYZ/Wは未確定です。**
既存c19 A3844ms/E4094msと移行後の短時間smokeは受け入れ済みの過去の証拠です。
今回の診断版の約4秒A/Eや、writable/ownership/exclusive sourceの証明には置き換えません。
