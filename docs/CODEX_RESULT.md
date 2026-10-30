# Codex result — read-only steering extraction-source investigation

2026-10-30 JST. Branch `codex/v01c20-edi-value-provenance`.
Task baseline: `afa7654549380b45b58fabadcdfc333adaa7c2d3`.

## STATUS

**partial** — tool-source review and controlled existing-tool procedure complete;
actual installed asset and runtime consumption remain unverified.

## CHANGED

- `analysis/WALLRUN_STEERING_EXTRACTION_SOURCE_REVIEW_20261030.md`
- `docs/CODEX_RESULT.md`

No code, PROJECT_STATE, CODEX_NEXT, package, game data or other repository changed.
Two pre-existing unfinished c66 experimental Python files conflicted with newly
tracked upstream names. Their exact bytes were preserved in cloud scratch
`/workspace/scratch/ws-c66/preserved-before-steering-20261030` before the normal
fast-forward to the current task. Five pre-existing untracked archives/bundle
remain untouched and are not part of this commit. No main merge/reset/force push.

## RESULT

- Modding Tool 1.4.5 resolves a selected file to its parent directory and prefers
  `toc.BAK` over `toc`, even if the file picker selected `toc`.
- MSM2 live toc is at game root. Archives are read relative to the loaded toc;
  copying only toc is insufficient. The list suppresses a leading `d\` in I29
  archive names. The inspected MSM2 stage installer uses `d\mods\mod7`;
  that is the expected copy path, not proof of the private toc string from the
  displayed name alone.
- Export uses the currently loaded toc's `(span, asset ID)` mapping, not the UI
  archive text, and has no export-time vanilla fallback.
- List size is raw payload size; exported STG can add a header/extras/padding.
  A 512-byte payload and 544-byte export are compatible, not a demonstrated
  selection mismatch. Derive wrapper length before applying byte offsets.
- The accepted ChatGPT-inspected export hash matches vanilla (Fast100/Slow75),
  not Slow5. That does not prove which toc was used in the historical session,
  installation failure, or the identity of an overriding MOD.
- Missing copied archives can produce a silent extraction failure, but the
  reported still-open Save dialog cannot be attributed to this from source
  alone: extraction runs only after the save dialog returns OK.
- Installed Standard.Slow5, actual Standard/Slow runtime selection and runtime
  adoption are three separate claims. The last two are not established here.

Full source references, wrapper/raw offsets, and **one exact owner PowerShell
block** are in the new analysis document. That block only reads game-root `toc`
and `d\mods\mod7`, verifies copies in a new TEMP directory, and prints paths
for the existing GUI. GUI menus and exact ref `0/948FA063808F7E18` were checked
against versioned source. No invented extractor CLI arguments, new extractor,
framework, game/backup rename or reinstall is proposed.

## Checks — exact commands, expectations and actual results

All source commands below were run in the temporary upstream-source clone;
WS commands were run in `/workspace/WallSprint`. These are source/manual audit
groups, not native/GUI tests or real installation verification.

| Check / exact command | Expected | Actual / classification |
| --- | --- | --- |
| `git branch --show-current`; `git rev-parse HEAD`; `git fetch origin codex/v01c20-edi-value-provenance`; `git merge --ff-only origin/codex/v01c20-edi-value-provenance` | Correct branch; update only by fast-forward; current bounded task read | Expected branch, baseline `afa7654`; required AGENTS/manual/rules/layout/state/task/evidence read. PASS |
| `git -C /tmp/ws-steering-overstrike-review rev-parse 'MT/v1.4.5^{}'`; `git -C /tmp/ws-steering-overstrike-review show MT/v1.4.5:ModdingTool/ModdingTool.csproj`; `git -C /tmp/ws-steering-overstrike-review show MT/v1.4.5:ModdingTool/MainWindow.xaml.cs \| sed -n '129,213p'` | Version matched; selection branch identified | Tag commit `98bb04f1a37d73f1b0ec2c2148d0a0b354d61292`, assembly1.4.5; toc.BAK preference explicit. PASS |
| `git -C /tmp/ws-steering-overstrike-review show MT/v1.4.5:DAT1/TOC.cs \| sed -n '84,108p'`; same file `sed -n '185,200p'` and `sed -n '460,488p'`; `git -C /tmp/ws-steering-overstrike-review show MT/v1.4.5:Overstrike/Games/GameMSM2.cs \| sed -n '19,27p'` | Loaded toc parent determines read-only archive location; MSM2 root | `File.OpenRead(Path.Combine(AssetArchivePath, fn))`; parent stored at Load; MSM2 root toc. UI leading-d suppression checked in preceding source. PASS |
| `git -C /tmp/ws-steering-overstrike-review show MT/v1.4.5:OverstrikeShared/STG/STG.cs \| sed -n '10,90p'`; same file `sed -n '100,177p'`; `git -C /tmp/ws-steering-overstrike-review show MT/v1.4.5:Overstrike/Installers/StageInstaller.cs \| sed -n '190,250p'` | Raw payload versus STG extras differentiated; no DAT1 repack on export | Selected size is raw; wrapper added on export and stripped on install; offsets conditional on actual wrapper. PASS |
| `git -C /tmp/ws-steering-overstrike-review show MT/v1.4.5:ModdingTool/MainWindow.xaml.cs \| sed -n '532,619p'` | Determine error handling and whether export falls back | Current `_toc`/span/id passed to STG; extraction/write catch empty; save dialog completes before extraction. No fallback. PASS |
| `git -C /tmp/ws-steering-overstrike-review show MT/v1.4.5:ModdingTool/MainWindow.xaml \| sed -n '18,64p'`; `git -C /tmp/ws-steering-overstrike-review show MT/v1.4.5:ModdingTool/MainWindow.xaml.cs \| sed -n '450,497p'`; `git -C /tmp/ws-steering-overstrike-review show MT/v1.4.5:ModdingTool/README.txt \| sed -n '36,61p'` | Existing GUI operations/ref syntax verified; no fabricated CLI | Load toc, Jump to path or ref, extraction supported; regex accepts `0/948FA063808F7E18`. Owner command paths conform to loader/oracle source. PASS |
| `test -f "/mnt/c/SteamLibrary/steamapps/common/Marvel's Spider-Man 2/toc"`; same test for `d/mods/mod7` | Establish availability without scanning private data | Both absent; owner live extraction/value check UNAVAILABLE |
| `command -v pwsh`; `command -v powershell` | Establish native command/UI execution capability | Neither available; no Windows GUI/display or owner ModdingTool executable. Owner PowerShell/GUI reproduction UNAVAILABLE; source review is not execution |
| `git diff --name-only afa7654549380b45b58fabadcdfc333adaa7c2d3`; `git status --short` | Only the two intended documentation paths plus original untracked artifacts | Scope checked before staging and after commit. PASS |
| `git diff --check`; `git diff --cached --check` | No whitespace errors | Exit0. PASS |

**PASS 9 audit groups (8 source/scope/whitespace + 1 publication) / SKIP 1
suite group / UNAVAILABLE 2 owner-execution groups / BLOCKED 0**. No unit/regression/native/game PASS claimed.

### Unrun tests and reasons

SKIP: no tool/runtime code changes, so no focused Fast/Slow builder test, Windows
build, portable test, Windows smoke/hooks suite or package run. These are not
required by the current documentation-only task. No Update-WS, reinstall, game
launch, native probe, c73, steering change or broader movement investigation.

UNAVAILABLE: live toc/referenced archive/control export missing in cloud; actual
owner PowerShell/ModdingTool UI behavior and Save-dialog symptom not reproduced.
The proposed block is source-reviewed, not reported as locally executed.
The supplied file hash is accepted from CODEX_NEXT, not freshly recomputed here.

## Safety and artifact

Runtime v0.1c38 unchanged. Existing C52
`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` and all closed gates unchanged.
No generated install/source ZIP. No private asset/archive/log committed.
Wisdoms read-only references consulted; synchronization intentionally not made
because current task forbids other-repository changes.

## Branch / publication

Branch: `codex/v01c20-edi-value-provenance`.
Initial documentation commit: `0a3a3c723536aa33fea6e29b47ff81102a446ab1`.
Normal `git push origin HEAD:refs/heads/codex/v01c20-edi-value-provenance`
succeeded. `git ls-remote --heads origin refs/heads/codex/v01c20-edi-value-provenance`
returned that exact SHA. Independent GitHub-only readback:

```bash
git clone --depth 1 --filter=blob:none --no-checkout --single-branch --branch codex/v01c20-edi-value-provenance https://github.com/deluxxxe-dev/MSM2-WallSprint.git /tmp/ws-steering-docs-publication
git -C /tmp/ws-steering-docs-publication rev-parse HEAD
git -C /tmp/ws-steering-docs-publication show HEAD:analysis/WALLRUN_STEERING_EXTRACTION_SOURCE_REVIEW_20261030.md
git -C /tmp/ws-steering-docs-publication show HEAD:docs/CODEX_RESULT.md
```

Expected: independent head equals the pushed local commit; both remote blobs
byte-identical to the intended local documents. Actual: all matched (exit0).
Initial remote document SHA256s, in that order:
`00c1ffb333977feda3518cefb126c5365c20ac441243fdedbd8765c1004d0729`,
`00ba86428cd546e9f7ba63bb014523ecfd71c0038b6cf18eb7804e5bea78e55d`.
These identify the first pushed revision, not the later publication record.

This final documentation follow-up records the completed verification and
clarifies that the UI-suppressed archive path is expected from public installer
source, not authenticated from the private toc. After its normal push, refresh
only the independent temporary checkout with
`git -C /tmp/ws-steering-docs-publication fetch origin codex/v01c20-edi-value-provenance`
and compare `FETCH_HEAD`/both document blobs against the final local HEAD/files.
The final metadata commit SHA is reported in the owner handoff to avoid a
self-referential hash in this file.

## NEED

Only a controlled export of the currently installed entry from the existing
Modding Tool, with copied live toc + its referenced `d/mods/mod7`, input hashes,
Span/Archive/Size and the new `installed-Standard.config`. Exact command and
existing GUI sequence are in the analysis. If the copied toc points elsewhere,
report that exact archive instead. Do not send whole private archives by default.
No additional game run is requested and no runtime-use conclusion is available.
