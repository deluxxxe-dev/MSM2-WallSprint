# Next task — bounded read-only review of existing WS movement/facing separation

Branch: `codex/v01c20-edi-value-provenance`. Do not merge main.
This supersedes the extraction investigation and prior Slow5 test request.

## Accepted new evidence — 2026-10-30 JST
ChatGPT directly inspected the owner's second export, supplied after the requested fresh live-toc/archive copy procedure:
- filename hero_wallrunsteeringsetup_standard(1).config
- size 544 bytes
- SHA256 d83095b5e68cc6fa20c95bc044345f7f46fd6c0a35a682df25374b140dfcec2d
- comparison against authenticated vanilla SHA256 32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b: exactly one byte differs, file offset 0x219, 75 -> 5; Fast at 0x218 remains 100.

VERIFIED-STATIC (supplied export): exact intended Slow5 payload is present.
Owner-workflow evidence supports installed replacement at extraction time; source/copy hashes and full GUI trace were not supplied. Do not retroactively certify the earlier game session's installed state.
Owner-observed runtime: Slow5 test had no visible change. Runtime selection/consumption of Standard/Slow remains UNKNOWN.
Earlier vanilla export does not prove installation failure. Extraction issue is sufficiently resolved to stop repeating it.

## Purpose / scope
Apply the existing stopping rule: no serial steering-field changes, no c73.
Read only the current WS implementation that selects movement direction and orientation in WallRun, to locate the smallest existing separation relevant to reversal with old visible facing.
ChatGPT retains design and runtime-test decisions; do not invent a new movement design.

## Read / inspect
Follow AGENTS required operational reading, docs/PROJECT_STATE.md, analysis/WALLRUN_STEERING_PROBE_AB_RESULT_20261029.md and analysis/WALLRUN_STEERING_EXTRACTION_SOURCE_REVIEW_20261030.md.
Use the existing WS source and targeted symbol searches only. Inspect functions implementing current WallRun orientation/reversal, their immediate callers, and existing diagnostics.
Return:
1. exact file/function locations for current direction selection and orientation writes;
2. what existing code does when direction reverses, including which orientation axes it preserves;
3. whether existing logs already distinguish movement direction, actor/Transform forward and visible animation facing. If absent, state that, without implementing instrumentation.
Separate static code behavior from runtime facts and hypotheses. Labels from Scripted Flight are hints only; no third-party offsets/API assumptions.

## Allowed / forbidden
Allowed: targeted read-only source inspection, compact analysis documenting the new evidence and source findings, docs/CODEX_RESULT.md and docs/PROJECT_STATE.md updates to remove the stale Slow5-pending state.
Do not overwrite this task file.
Forbidden: runtime/source/tool changes, new hooks/watches, config changes, build/deploy/reinstall requests, game/toc/loader/profile changes, SF/other-repository changes, broad binary/CFG scans, c73, producer traversal, native gameplay writes or new framework/design work.
If the question requires new engine evidence, stop at the exact missing fact.
Runtime remains v0.1c38. C52 PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL and all existing proof/write gates remain closed.

## Completion / validation
Record the compact evidence and exact source references in a WS analysis document; update CODEX_RESULT and the relevant PROJECT_STATE sections. Commit/publish on the current branch and verify remote contents.
Documentation only: verify statements against cited source and review diff; no build or game test required.
Do not claim that installed Slow5 proves runtime use or that static orientation code alone explains animation.

## Short reply
STATUS: success / partial / blocked
CHANGED:
- documentation paths
RESULT:
- direction/facing source locations and narrow findings
NEED:
- exact unresolved fact requiring Chat-side judgment, if any

No user-facing runtime-test prose or long alternative designs.
