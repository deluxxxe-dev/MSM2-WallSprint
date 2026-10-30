# Next task — determine steering extraction source; read-only investigation

Branch: `codex/v01c20-edi-value-provenance`. Do not merge main.
This task supersedes the previous request to run the Slow5 game test.

## Purpose
Determine whether the supplied extraction represents the installed Slow5 asset or vanilla data. Do not infer runtime config usage from extraction alone.

## Supplied evidence — 2026-10-30 JST
- Owner reports Slow5 produced no visible behavior change. Actual adoption of the changed value remains unproven; do not assume all isolation steps were independently verified.
- Modding Tool displayed `hero_wallrunsteeringsetup_standard.config` with Archive `mods\mod7` and size 512 B.
- Extract from the test copy of toc failed: Save left the dialog open.
- Extract from the game installation's toc succeeded.
- ChatGPT inspected that supplied file: STG-wrapped, 544 bytes, SHA256 `32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b`.
- It exactly matches the previously authenticated vanilla file: TurnSpeedFast=100 at file offset 0x218, TurnSpeedSlow=75 at 0x219, not Slow5.
- These observations do not prove installation failure, winning-mod identity, toc.BAK fallback, or runtime use. The selected entry and exported bytes differ in apparent size; inspect wrapper/export behavior before drawing a conclusion.

## Bounded investigation
Read AGENTS.md and required operational documents, docs/PROJECT_STATE.md, analysis/VANILLA_WALLRUN_STEERING_CONFIG_20261029.md, and analysis/WALLRUN_STEERING_PROBE_AB_RESULT_20261029.md.
Inspect only existing relevant tools and the applicable Modding Tool/Overstrike source as available:
1. Establish toc vs toc.BAK selection, archive path resolution relative to a copied toc, and selected-entry vs exported-byte behavior.
2. Identify an existing read-only extraction/check method for the live installed Standard asset (asset ID 948FA063808F7E18). Account for STG wrapper offsets vs raw payload offsets.
3. If the local game data is available, verify actual inputs and extract/check without changing them. Otherwise prepare one exact minimal local command or state the precise missing tool/input. Do not claim local verification from cloud.
4. Distinguish installed replacement, extraction source, and actual runtime consumption.

Do not rescan the whole repository or historical logs. Do not build a new extractor/framework if existing tools cannot establish the answer; report the exact blocker.

## Allowed / forbidden
Allowed: read-only inspection; temporary extraction outside Mods Library; concise WS analysis and docs/CODEX_RESULT.md updates.
Forbidden: game/toc/toc.BAK/archive/loader/profile/MOD changes, renames or deletions; SF/other-repository changes; new steering values; native hooks/watches; gameplay writes; c73 or broad movement investigation.
Runtime stays v0.1c38. C52 remains binding: PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL. All existing proof/write gates remain closed.

## Completion and checks
Record confirmed source behavior and remaining uncertainty in a compact WS analysis file and docs/CODEX_RESULT.md; commit/publish documentation on the current branch.
If no code changes occur, no runtime rebuild/game test is needed. For any read-only command proposed, verify its arguments and input/output paths against actual tool source/help; use relevant existing tests only if tool code is changed under a separately authorized task.
Do not require Update-WS, reinstall, or another identical game run for this investigation.

## Reply
STATUS: success / partial / blocked
CHANGED:
- changed documentation paths
RESULT:
- confirmed extraction source behavior and local check result, or exact minimal command
NEED:
- only a precise missing input or Chat-side decision

Keep the reply short. ChatGPT supplies any user-facing test explanation.
