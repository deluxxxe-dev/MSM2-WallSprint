# RequestSimpleMotion c42 runtime admission failure — 2026-10-31

Status: **VERIFIED-RUNTIME failure cause / c43 correction published / c43 runtime pending**.

## Supplied runtime evidence

The supplied `MSM2-WS-v0.1c42-20261031-214305.log` is from the pinned target build:

- `Spider-Man2.exe` version `2.810.0.0`;
- on-disk SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- target build gate accepted;
- `REQUEST_PRESENCE_DIAGNOSTIC=1`;
- automatic capture configured for 300000 ms / 131072 records.

RequestSimpleMotion identity checks produced:

- whole-site SHA256 `2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c`: PASS;
- seven-byte entry prefix: PASS;
- stolen instruction span: PASS, 7 bytes / 2 instructions / no relative instruction;
- `.pdata` begin RVA: `0x11A1BD0`: PASS;
- `.pdata` end RVA: `0x11A1CA2`: PASS;
- actual `.pdata` UnwindData RVA: `0x94697E0`;
- c42 expected UnwindData RVA: `0x1154EDC`: FAIL.

Admission therefore failed closed with
`REQUEST_PRESENCE_ADMISSION admitted=0 reason=site_identity_or_span_or_pdata_mismatch ... pdata_ok=0`.
No automatic phase was armed and no `REQUEST_PRESENCE_RECORD` entries exist. This is **not** evidence that RequestSimpleMotion had zero calls.

## Cross-check against prior accepted owner audit

The already accepted owner hook audit `MSM2-WS-presence-hook-audit-20261031-161445.json` recorded the same RequestSimpleMotion runtime-function identity:

- begin `18488272` / `0x11A1BD0`;
- end `18488482` / `0x11A1CA2`;
- UnwindData `155621344` / `0x94697E0`;
- function SHA256 `2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c`;
- unwind metadata SHA256 `9f25979a133ba75c02e2da0f96631b692a6445a12456f96c4b477fac8fd00ce5`.

Thus c42's runtime observation agrees with the earlier accepted audit. `0x1154EDC` was an implementation/transcription error in the production admission constant, not a new game-build change.

## WallRun behavior observed in the same run

The independent verified WallRun lifecycle hooks remained functional. Several short/vertical WallRun sessions occurred before the main recorded comparison, consistent with the owner's note that extra vertical wall movement may appear early.

The main WallRun session was session 5:

- Init approximately `12:45:11.548Z`;
- Deinit approximately `12:45:29.926Z`;
- duration approximately 18.38 seconds;
- the position-difference telemetry shows several horizontal direction reversals during the session;
- later samples show a sustained negative world-Y component while horizontal WallRun continues.

The owner reports that the final down input did **not** produce the same immediate reversal seen in the prior c40 run. Preserve this as observed behavior only. There is no authenticated keyboard-input stream in this log and RequestSimpleMotion capture was not admitted, so no Request argument semantics or causal input mapping can be inferred from this run.

## c43 correction and recurrence prevention

`v0.1c43` / package `0.1.68` changes only the Request presence diagnostic identity contract and synchronized metadata:

- production `kRequestUnwindRva` corrected to `0x94697E0`;
- existing automatic-on-admission capture retained;
- 300000 ms / 131072-record bounds retained;
- no F11/F12 input required;
- all WallSprint movement/orientation/adhesion/native-source writes remain OFF;
- accepted Request arguments remain opaque bits with no pointee dereference or argument modification.

`audit_presence_hook_sites.py` now pins the accepted Request UnwindData RVA for the exact target build. `request_presence_static.py` parses production begin/end/UnwindData/SHA and the accepted audit constants and requires exact four-field equality before the long test suites run. It also explicitly rejects the old `0x1154EDC` production value.

Actual c43 MinHook/site/unwind/trampoline admission and Request records remain **PENDING REAL RUNTIME**. First require the normal owner `Update-WS.cmd` full pipeline and deployment; only then repeat the same video sequence.
