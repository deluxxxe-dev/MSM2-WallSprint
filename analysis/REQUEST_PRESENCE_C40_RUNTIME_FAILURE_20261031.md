# RequestSimpleMotion c40 runtime admission failure — 2026-10-31

Status: **VERIFIED-RUNTIME failure cause / c41 correction published / c41 runtime pending**.

## Supplied runtime evidence

The supplied `MSM2-WS-v0.1c40-20261031-210529.log` is from the pinned game build:

- `Spider-Man2.exe` version `2.810.0.0`;
- on-disk SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- target build gate accepted;
- `REQUEST_PRESENCE_DIAGNOSTIC=1`.

At startup, the RequestSimpleMotion site itself produced:

- runtime whole-site SHA256 `2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c`;
- c40 expected SHA256 `2d879249c65b8074bb548e6563b5e0c2955ab672c4fd3cf080adb83b48691d64`;
- `hash_ok=0` while the 7-byte prefix still matched;
- `REQUEST_PRESENCE_ADMISSION admitted=0 reason=site_identity_or_span_or_pdata_mismatch`.

At the user's F11 press, the runtime truthfully logged
`REQUEST_PRESENCE_ARM rejected=1 ... reason=diagnostic_not_admitted`.
Therefore c40 produced **no RequestSimpleMotion presence records** and the supplied
video cannot be correlated to Request arguments. Do not interpret absence of
records as zero Request calls.

## Cross-check against existing accepted static evidence

The existing exact-site audit source already pins RequestSimpleMotion as:

- begin RVA `0x11A1BD0` / decimal `18488272`;
- end RVA `0x11A1CA2` / decimal `18488482`;
- SHA256 `2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c`.

This exactly matches the whole-site SHA measured by the actual game in c40.
The failure was therefore a c40 implementation consistency error: the runtime
`kRequestSha256` constant did not match the already accepted audit tuple.

## User action / WallRun evidence

The user used digital WASD input in the requested order. The supplied video is
32.75 seconds long. The user reports that after right input during ordinary
WallRun, holding down caused a period of leftward WallRun followed by rapid
left/right reversals, with down still held near the end of the recording.

Independent existing WallRun lifecycle hooks remained valid during the test.
The log contains WallRun session 8 from approximately `12:10:20.782Z` to
`12:10:41.704Z`, with Init=1, Entry=1, Update=1798, Deinit=1 and duration
20.918701 s. This verifies that the ordinary WallRun portion occurred, but does
not repair or substitute for the rejected Request presence capture.

The 30-second c40 capture budget was also too tight for this real user sequence:
F11 occurred around `12:10:07.509Z`, while the WallRun session continued beyond
`12:10:37.509Z` to about `12:10:41.704Z`.

## c41 correction

`v0.1c41` / package `0.1.66` changes only the read-only diagnostic contract:

- runtime Request SHA now equals the accepted audit/runtime SHA above;
- capture duration increased from 30000 to 45000 ms;
- bounded record cap increased from 8192 to 16384;
- no gameplay writes are enabled;
- a static test now parses the Request tuple from both runtime source and
  `audit_presence_hook_sites.py` and requires exact begin/end/SHA equality, so
  the specific c40 drift cannot pass the normal test pipeline again;
- package/readme/version/preset metadata are synchronized to c41.

Actual c41 MinHook/site/unwind/trampoline admission and Request records remain
**PENDING REAL RUNTIME**. The next owner operation is the normal `Update-WS.cmd`
full pipeline; only after it succeeds should the same one-F11 video test be rerun.
