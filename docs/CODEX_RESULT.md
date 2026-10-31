# Result — WallSprint v0.1c43 Request presence correction

2026-10-31. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C42 RUNTIME FAILURE CAUSE VERIFIED / C43 CORRECTION PUBLISHED / READ-ONLY CONTRACT PRESERVED / FULL OWNER PIPELINE PENDING / C43 ACTUAL-GAME RUNTIME PENDING.**

## C42 ACTUAL RUNTIME RESULT

The owner successfully installed and launched `v0.1c42 / package 0.1.67` and supplied a game log and video for the ground Sprint -> reverse -> neutral -> ordinary WallRun left -> right -> down sequence.

The exact target build was accepted (`Spider-Man2.exe` 2.810.0.0, on-disk SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`). RequestSimpleMotion admission then produced:

- whole-site SHA: correct / `hash_ok=1`;
- seven-byte prefix: correct / `prefix_ok=1`;
- stolen span: correct / `span_ok=1`;
- runtime-function begin/end: correct (`0x11A1BD0..0x11A1CA2`);
- actual `.pdata` UnwindData RVA: `0x94697E0`;
- c42 expected UnwindData RVA: `0x1154EDC`;
- `pdata_ok=0`;
- final admission: `admitted=0 reason=site_identity_or_span_or_pdata_mismatch`.

Therefore c42 auto-capture never armed and produced no valid RequestSimpleMotion records. Absence of records is not a zero-call result.

The prior accepted owner hook audit `MSM2-WS-presence-hook-audit-20261031-161445.json` had already recorded RequestSimpleMotion UnwindData `155621344 / 0x94697E0`, matching the c42 actual game runtime. The c42 `0x1154EDC` production constant was an implementation/transcription consistency error, not a changed game build.

The independent WallRun lifecycle hooks remained functional. The main recorded WallRun was session 5, approximately `12:45:11.548Z` to `12:45:29.926Z` (~18.38 s). The owner reports that the final down input did not produce the same immediate reversal seen in the prior run. Preserve that as observed behavior only: the c42 Request capture was rejected and the log has no authenticated keyboard-input stream, so no Request argument semantics or causal input mapping are claimed.

Detailed evidence: `analysis/REQUEST_PRESENCE_C42_RUNTIME_FAILURE_20261031.md`.

## C43 IMPLEMENTATION

Version is now `v0.1c43`, package `0.1.68`.

Changes are limited to the read-only Request presence diagnostic identity contract and synchronized metadata/tests:

- `kRequestUnwindRva` corrected to accepted/runtime `0x94697E0`;
- production Request begin/end/SHA remain unchanged;
- automatic capture on successful admission retained;
- maximum capture remains 300000 ms / 5 minutes;
- record cap remains 131072;
- no F11/F12 input required;
- `WALLSPRINT_ENABLE=0`;
- `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`;
- orientation/adhesion/movement writes remain OFF;
- Request arguments remain opaque bits; no captured-pointer dereference or argument modification.

## PREVENTING THE SAME ERROR

`source/tools/audit_presence_hook_sites.py` now pins the accepted RequestSimpleMotion UnwindData RVA for the exact target build.

`source/tests/request_presence_static.py`, executed at the start of `source/tools/test.sh`, now parses the production Request begin/end/UnwindData/SHA and the accepted audit constants and requires exact four-field equality. It explicitly rejects the old `0x1154EDC` production value and also checks c43 version/package, mod metadata, both READMEs, read-only INI state, 5-minute/131072 capture bounds and package metadata.

## FILES CHANGED FOR C43

- `source/src/msm2/request_presence.cpp`
- `source/tools/audit_presence_hook_sites.py`
- `source/tests/request_presence_static.py`
- `source/include/version.hpp`
- `source/mod/info.json`
- `source/mod/WallSprint.ini`
- `source/tools/package.py`
- `source/tools/test.sh` (comment only; full prior test sequence retained)
- `README_TEST.md`
- `source/README_TEST.md`
- `analysis/REQUEST_PRESENCE_C42_RUNTIME_FAILURE_20261031.md`
- `docs/CODEX_RESULT.md`

## VALIDATION STILL REQUIRED

The corrected c43 source is published, but the normal owner Windows/WSL `Update-WS.cmd` full pipeline has not yet been run against these c43 changes. Do not claim c43 build/package/deployment or actual Request admission success yet.

Next action:

1. Run `Update-WS.cmd` with the game and shared Overstrike closed.
2. Require the complete full-pipeline PASS and c43 deployment.
3. Only then rerun the same automatic-capture video sequence; no F11/F12.
4. For c43 runtime acceptance, first verify `REQUEST_PRESENCE_ADMISSION admitted=1` and `REQUEST_PRESENCE_PHASE_ARM ... controls=automatic_on_admission` before interpreting any Request records.
