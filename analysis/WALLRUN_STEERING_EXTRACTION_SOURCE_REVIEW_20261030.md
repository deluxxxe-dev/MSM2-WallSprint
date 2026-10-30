# Steering extraction source review — 2026-10-30 JST

## Result and evidence limits

**PARTIAL: public source behavior established; live installation unverified.**
Task baseline: `afa7654` on `codex/v01c20-edi-value-provenance`.
Read AGENTS, the operational manual/rules/layout, PROJECT_STATE, CODEX_NEXT,
the vanilla steering review and Fast-probe A/B result. This investigation
supersedes the old Slow5 runtime-test instruction still in PROJECT_STATE.

The supplied task record says the exported file is 544 bytes with SHA256
`32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b`.
That is the authenticated **vanilla** Standard config: Fast=100, Slow=75,
not Slow5. This is accepted ChatGPT-inspected evidence, not a fresh cloud
hash/decoding of the private file. The live toc, referenced archive, and this
new export are unavailable here. No installation failure, winning MOD identity,
actual toc.BAK selection in that session, or runtime consumption is established.
Owner-observed Slow5 "no visible change" does not verify the isolation steps or
that the modified value was used.

## Confirmed public-source behavior — VERIFIED-STATIC (tool source)

Inspected `Tkachov/Overstrike` tag `MT/v1.4.5`, peeled commit
`98bb04f1a37d73f1b0ec2c2148d0a0b354d61292`. Its ModdingTool project declares
version 1.4.5.0. This is version-matched source, not authentication of the
owner's installed executable.

| Question | Source and conclusion |
| --- | --- |
| Which TOC is opened? | `ModdingTool/MainWindow.xaml.cs:StartLoadTOCThread` converts a chosen file to its directory, then chooses `toc.BAK` if present, otherwise `toc`. Explicitly selecting `toc` does **not** override this preference. `Overstrike/Games/GameMSM2.cs:GetTocPath` puts MSM2's live toc at game root, not `asset_archive/toc`. |
| Where are archives resolved? | `DAT1/TOC.cs:TOC_I29.Load` stores the loaded toc's parent directory. `OpenArchive` opens `Path.Combine(AssetArchivePath, archiveName)` read-only. Copying only toc does not copy its payload archives. |
| Why does the UI say `mods\mod7`? | `MainWindow.LoadTOC` strips a leading `d\` from displayed I29 archive names. The inspected MSM2 `StageInstaller_I29_V2.Install` writes `d\mods\mod7` into toc, so that is the expected copy path. The display alone does not authenticate the original stored path, a stable MOD name, or Slow5 package identity. |
| What do selection and export use? | The list's Size comes from `GetSizeInArchiveByAssetIndex`: payload size. `ExtractAsset` calls `STG.Load(_toc, asset.Span, asset.Id)`, which looks up that `(span,id)` in the currently loaded toc and reads its archive. There is no per-export toc.BAK/vanilla fallback. It then calls `ClearDat1()` to prevent DAT1 repacking and saves an STG-wrapped asset when extras exist. |
| What about failed Save? | `ExtractAsset` silently catches extraction/write exceptions. A missing copied archive can therefore yield no file. However the save dialog returns before extraction starts (`ExtractOneAssetDialog`); "dialog stayed open" alone cannot identify an archive error or explain that exact UI symptom. An output filename/dialog is not success evidence. |

### 512 bytes displayed versus 544 bytes exported

`OverstrikeShared/STG/STG.cs:Save/Load` separates the raw DAT1 payload from the
STG fixed header (16 bytes), asset-header bytes, texture metadata and 16-byte
alignment. Thus **512-byte payload + 32-byte prefix = 544-byte export is
compatible with normal wrapping**, not evidence of selecting another entry.
`StageInstaller_I29_V2.Helper.ProcessAsset` removes this wrapper on installation
and passes the extras separately. It does not install the whole 544 bytes as
the raw payload.

For any new export, derive its raw start from its actual STG header lengths:
`align16(align16(16 + headerSize) + textureMetaSize)`. Check version/bounds and
the DAT1 header before interpreting fixed byte positions. For the authenticated
544-byte Standard layout with raw start `0x20`, file offsets `0x218/0x219`
(Fast/Slow) correspond to raw DAT1 offsets `0x1F8/0x1F9`. Do not apply the wrapped
offsets to a raw 512-byte file, assume all STG wrappers are 32 bytes, or use byte
5 at an arbitrary offset as a config identity. This cloud review has not
measured the private export's wrapper header anew.

## Existing read-only method; one owner command

Use the existing Modding Tool 1.4.5 GUI. Its documented operations are File >
Load 'toc', Search > Jump to path or ref, and right-click extraction. The exact
ref syntax accepted by `MainWindow.JumpTo` is `0/948FA063808F7E18`.
There is no documented CLI extractor in this entrypoint; do not invent flags.

The following **owner-only** PowerShell block prepares a fresh temporary view
of the already-installed state reported as `mods\mod7`. It reads the game
inputs and writes only under a unique TEMP directory outside Mods Library;
it never copies/renames/deletes toc.BAK or changes any installation. It checks
source/copy hashes, not config semantics. Do not install mods during this check.
If an input is absent or the copied toc shows a different archive, stop and
report that exact path/entry; do not reinstall or silently substitute a file.

```powershell
& {
    $ErrorActionPreference = 'Stop'
    $gameRoot = "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2"
    if (Get-Process -Name 'Spider-Man2','Overstrike' -ErrorAction SilentlyContinue) {
        throw 'Close Spider-Man 2 and Overstrike before this read-only check.'
    }
    $relativeInputs = @('toc', 'd\mods\mod7')
    $before = @{}
    foreach ($relative in $relativeInputs) {
        $inputFile = Join-Path $gameRoot $relative
        if (-not (Test-Path -LiteralPath $inputFile -PathType Leaf)) {
            throw "Missing input: $inputFile"
        }
        $before[$relative] = (Get-FileHash -LiteralPath $inputFile -Algorithm SHA256).Hash
    }
    $checkRoot = Join-Path $env:TEMP ('MSM2-WS-SteeringCheck-' + [guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path (Join-Path $checkRoot 'd\mods') | Out-Null
    foreach ($relative in $relativeInputs) {
        $sourceFile = Join-Path $gameRoot $relative
        $copyFile = Join-Path $checkRoot $relative
        Copy-Item -LiteralPath $sourceFile -Destination $copyFile
        $sourceHash = (Get-FileHash -LiteralPath $sourceFile -Algorithm SHA256).Hash
        $copyHash = (Get-FileHash -LiteralPath $copyFile -Algorithm SHA256).Hash
        if ($before[$relative] -ne $sourceHash -or $sourceHash -ne $copyHash) {
            throw "Input changed or copy mismatch: $relative"
        }
        "SHA256 $relative = $copyHash"
    }
    "LoadToc: $(Join-Path $checkRoot 'toc')"
    'JumpTo: 0/948FA063808F7E18; verify Span=0, Archive=mods\mod7'
    "ExtractTo: $(Join-Path $checkRoot 'installed-Standard.config')"
}
```

Open the printed LoadToc path in the existing tool, jump to the exact ref,
record Span/Archive/Size, and extract to the printed **new** ExtractTo path.
Because this folder has no toc.BAK, the source-selection branch must use its
copied live toc; `d/mods/mod7` is present at the expected relative location.
If a different archive is shown, this two-input snapshot is insufficient;
request that exact referenced archive instead of claiming an extraction result.

Return only `installed-Standard.config`, the printed input hashes and the
selected Span/Archive/Size (not whole private archives). Verify an actual new
file exists; do not reuse the previous same-named vanilla export. ChatGPT can
compare the STG/DAT1 config with the authenticated vanilla and Slow5 payload.

## Three separate conclusions

1. **Export contents:** the supplied inspected export is vanilla. Its historical
   selection/source session is not authenticated by the screenshot alone.
2. **Installed replacement:** only a controlled live-toc/actual-archive export
   can show whether this installed Standard entry contains Slow=5, Slow=75, or
   another config. Exporting from the package or toc.BAK is insufficient.
3. **Runtime consumption:** even an installed Slow=5 does not prove the tested
   game state loads Standard, enters the Slow branch, or uses that value without
   override/scaling. No existing evidence here establishes this; no repeated
   game run or new instrumentation is requested by this task.

No source/tool changes, new extractor, steering changes, builds, deployment,
native probes, SF/other-repository changes or c73 work. Runtime stays v0.1c38;
all existing gates stay closed, including C52
`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`. Reusable source findings are
recorded here; Wisdoms edits are intentionally not performed because this task
forbids other-repository changes.
