# Result — WallSprint v0.1c41 Request presence correction

2026-10-31. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C40 RUNTIME FAILURE CAUSE VERIFIED / C41 CORRECTION PUBLISHED / READ-ONLY CONTRACT PRESERVED / FULL OWNER PIPELINE PENDING / C41 ACTUAL-GAME RUNTIME PENDING.**

## C40 ACTUAL RUNTIME RESULT

The owner successfully installed and launched `v0.1c40 / package 0.1.65`, then supplied the game log and video for the one-F11 ground Sprint -> reverse -> neutral -> ordinary WallRun left -> right -> down sequence.

The target game build was accepted (`2.810.0.0`, on-disk SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`). However RequestSimpleMotion admission failed before F11 capture:

- actual whole-site SHA: `2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c`;
- c40 runtime expected SHA: `2d879249c65b8074bb548e6563b5e0c2955ab672c4fd3cf080adb83b48691d64`;
- prefix matched, whole hash did not;
- admission logged `admitted=0 reason=site_identity_or_span_or_pdata_mismatch`;
- F11 then logged `reason=diagnostic_not_admitted`.

Therefore c40 produced no valid RequestSimpleMotion records. The absence of Request records is not a zero-call result.

The existing accepted audit source had already pinned the correct Request tuple: RVA `0x11A1BD0` to `0x11A1CA2` with SHA `2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c`. The c40 error was an implementation consistency mismatch in `kRequestSha256`.

The independent WallRun hooks remained functional. WallRun session 8 recorded Init=1, Entry=1, Update=1798, Deinit=1 over 20.918701 s, so the ordinary WallRun portion did occur.

The supplied video is 32.75 s. F11 was logged around `12:10:07.509Z`, while the relevant WallRun session continued until about `12:10:41.704Z`, so the old 30-second capture window was also too short for this actual sequence.

Detailed evidence: `analysis/REQUEST_PRESENCE_C40_RUNTIME_FAILURE_20261031.md`.

## C41 IMPLEMENTATION

Version is now `v0.1c41`, package `0.1.66`.

Changes are limited to the read-only presence diagnostic and its metadata/tests:

- RequestSimpleMotion runtime whole-site SHA corrected to the accepted audit/runtime SHA above;
- one F11 continuous capture retained;
- maximum capture duration increased to 45 seconds;
- bounded record cap increased to 16384;
- F12 remains disabled for this capture;
- `WALLSPRINT_ENABLE=0`;
- `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`;
- orientation/adhesion/movement writes remain OFF;
- Request arguments remain opaque bits with no captured-pointer dereference or argument modification.

## PREVENTING THE SAME ERROR

`source/tests/request_presence_static.py` now parses the RequestSimpleMotion begin/end/SHA tuple from both production `request_presence.cpp` and the existing accepted `audit_presence_hook_sites.py` and requires exact equality.

This directly prevents another build in which the audit identity and runtime admission constant silently diverge. The test also checks c41 version/package, 45-second duration, 16384 record cap, ASM cap, both READMEs, read-only INI state, and package manifest metadata.

## FILES CHANGED FOR C41

- `source/src/msm2/request_presence.cpp`
- `source/src/msm2/request_presence.S`
- `source/tests/request_presence_static.py`
- `source/include/version.hpp`
- `source/mod/info.json`
- `source/mod/WallSprint.ini`
- `source/mod/presets/request_presence_read_only.ini`
- `source/tools/package.py`
- `README_TEST.md`
- `source/README_TEST.md`
- `analysis/REQUEST_PRESENCE_C40_RUNTIME_FAILURE_20261031.md`
- `docs/CODEX_RESULT.md`

## VALIDATION STILL REQUIRED

The corrected c41 source is published, but the owner has not yet run the normal Windows/WSL `Update-WS.cmd` pipeline against these c41 changes. Do not claim c41 build/package/deployment or actual Request admission success yet.

Next action:

1. Run `Update-WS.cmd` with the game and shared Overstrike closed.
2. Require the complete full-pipeline PASS and c41 deployment.
3. Only then rerun the same one-F11 video test.
4. For c41 runtime acceptance, first verify `REQUEST_PRESENCE_ADMISSION admitted=1` before interpreting any records.
