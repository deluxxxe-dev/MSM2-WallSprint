# Result — WallSprint v0.1c47 verified-WallRun Update scalar diagnostic

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C44 REQUEST ZERO-HIT ACCEPTED / C45 GRAVITY OWNER ZERO-HIT ACCEPTED / C46 OWNER PIPELINE+DEPLOY+RUNTIME EXECUTED BUT SCALAR OBSERVER DID NOT RUN / C46 ZERO-SAMPLE RESULT REJECTED AS ENGINE EVIDENCE / C47 TELEMETRY-GATE FIX PUBLISHED / OWNER FULL PIPELINE + C47 RUNTIME PENDING.**

## C46 ACTUAL RUNTIME RESULT

The owner supplied the `v0.1c46 / package 0.1.71` runtime log and video.

Verified runtime facts:

- exact target build accepted: Spider-Man2.exe `2.810.0.0`;
- on-disk SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- all four verified WallRun lifecycle hooks installed and authenticated;
- `wallrun_update_scalar_plan enabled=1` with offsets `0x208/0x220/0x238/0x250/0x40C`;
- c45 Gravity presence hook correctly disabled (`REQUEST_PRESENCE_DIAGNOSTIC=0`);
- one verified WallRun: Init=1 / Entry=1 / Update=1889 / Deinit=1;
- duration about `22.273624 s`, Update rate about `84.809/s`;
- supplied video shows sustained ordinary WallRun activity during the test.

But c46 emitted zero:

- `wallrun_update_scalar_sample`;
- `wallrun_update_scalar_summary`;
- `surface_source_session`.

This is **not** a five-field zero/unreadable result.

## C46 ROOT CAUSE

Current source inspection proves:

- `telemetry.cpp::settings()` intentionally initializes `result.hooks.surfaceTelemetry=false`;
- c46 `surface_source.hpp::begin/observe/finish` returned on `!runtimeObservationEnabled` before the newly added scalar observer operations;
- therefore the scalar observer never armed, sampled, or finalized even though the static plan was admitted and printed as enabled.

Classification:

- WallRun lifecycle activity: VERIFIED-RUNTIME;
- scalar plan admission: VERIFIED-RUNTIME;
- actual c46 scalar values/changes: UNKNOWN / NOT OBSERVED;
- c46 zero samples as engine behavior: REJECTED;
- missing-observer cause: VERIFIED-STATIC.

Detailed evidence: `analysis/C46_SCALAR_GATE_RUNTIME_RESULT_20261101.md`.

## C47 IMPLEMENTATION

Version `v0.1c47`, package `0.1.72`.

C47 changes only scalar observation wiring:

- scalar session begin now occurs before the legacy `runtimeObservationEnabled` early return;
- the five direct owner+offset 4-byte reads now occur before that early return;
- scalar session summary finalization now occurs before that early return;
- `wallrun_update_scalar_session` is logged when the scalar observer arms;
- existing rich vector/root/child-pointer surface observation remains behind `runtimeObservationEnabled` and remains disabled;
- the exact five offsets and authenticated WallRun Update SHA are unchanged.

No new native hook is added.

## SAFETY / ACTIVE CONFIG

- `WALLSPRINT_ENABLE=0`;
- `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`;
- orientation / adhesion / movement / gravity / native-source writes OFF;
- `REQUEST_PRESENCE_DIAGNOSTIC=0`;
- verified WallRun lifecycle/object diagnostics ON;
- scalar reads are bounded direct `WallRun.this + {0x208,0x220,0x238,0x250,0x40C}` 4-byte reads;
- no scalar pointer dereference;
- no new game call;
- no semantic field names assigned.

## REGRESSION PREVENTION

`source/tests/request_presence_static.py` now explicitly requires:

- `telemetry.cpp` still contains `result.hooks.surfaceTelemetry=false`;
- scalar `begin` precedes the begin-path telemetry early return;
- scalar `observe` precedes the observe-path telemetry early return;
- scalar `finish` precedes the finish-path telemetry early return;
- c47 version/package/config/README/package metadata stay synchronized.

The existing ASan/UBSan/`-Werror` scalar observer unit test remains in the normal full test chain.

## CHANGED FOR C47

- `source/include/msm2/surface_source.hpp`
- `source/include/version.hpp`
- `source/mod/info.json`
- `source/mod/WallSprint.ini`
- `source/mod/presets/request_presence_read_only.ini`
- `source/tools/package.py`
- `source/tests/request_presence_static.py`
- `README_TEST.md`
- `source/README_TEST.md`
- `analysis/C46_SCALAR_GATE_RUNTIME_RESULT_20261101.md`
- `docs/CODEX_RESULT.md`
- `docs/CODEX_NEXT.md`

No SocialFeed or other MOD repository/file was changed.

## VALIDATION STILL REQUIRED

Run installed `MSM2-WS/Update-WS.cmd` with game/shared Overstrike closed and require the full build/test/smoke/hooks/package/deploy PASS for `v0.1c47 / 0.1.72`.

After PASS, actual-game validation is one ordinary WallRun only:

left ~3 s -> right ~3 s -> down ~5 s -> natural end.

No F11/F12 and no post-test wait.

The returned c47 log must contain `wallrun_update_scalar_session`; then `wallrun_update_scalar_sample` / `wallrun_update_scalar_summary` determine whether the five candidates are useful. No write/A-B test is authorized yet.
