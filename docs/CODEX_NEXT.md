# Next task — validate v0.1c47 verified-WallRun Update scalar observation

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/C46_SCALAR_GATE_RUNTIME_RESULT_20261101.md`, and current `docs/CODEX_RESULT.md`.

## Accepted prior runtime results

Do not repeat RequestSimpleMotion or GravityOrientationOwner presence testing for the same ordinary locomotion / WallRun sequence.

- c44 exact RequestSimpleMotion entry: 335 persisted 500 ms batches, all zero-hit while verified WallRun activity occurred.
- c45 exact GravityOrientationOwner entry: 418 persisted 500 ms batches, all zero-hit while one verified WallRun ran Init=1 / Entry=1 / Update=1470 / Deinit=1 for about 16.28 s.

C46 must **not** be treated as scalar zero-hit evidence. Its owner runtime had a real verified WallRun (Init=1 / Entry=1 / Update=1889 / Deinit=1, about 22.27 s), but the scalar observer was behind `surfaceTelemetry=false` and therefore never ran. Zero scalar sample/summary records are an instrumentation-gate failure, not an engine result.

## Current implementation

`v0.1c47 / package 0.1.72` is published.

The exact authenticated WallRun Update body remains `0x1424530..0x14246D6`, SHA256 `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`.

Observe exactly five unique direct 4-byte `WallRun.this` candidates:

- `0x208`
- `0x220`
- `0x238`
- `0x250`
- `0x40C`

C47 decouples only these five scalar reads from the intentionally disabled legacy rich surface telemetry. Scalar begin/read/finalize occur before the legacy telemetry early returns. Existing vector/root/child-pointer surface observation stays disabled.

New runtime proof key:

- `wallrun_update_scalar_session` — must appear for an actual WallRun and reports `surface_telemetry=0` while the direct scalar observer is armed.

Existing scalar keys:

- `wallrun_update_scalar_sample`
- `wallrun_update_scalar_summary`

For comparison only, summaries count exact numeric hits for `0.4 / 12 / 75 / 100 / 50`. Numeric equality alone is not field-semantic proof.

## Safety contract

- no new native hook;
- no pointer dereference for scalar capture;
- no game call;
- no scalar write;
- no gravity/orientation/adhesion/movement/native-source write;
- `WALLSPRINT_ENABLE=0`;
- `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`;
- `REQUEST_PRESENCE_DIAGNOSTIC=0`;
- existing four verified WallRun lifecycle hooks remain the only required native lifecycle hooks.

Static regression must continue to prove that `surfaceTelemetry=false` remains in telemetry settings while scalar begin/observe/finish precede the corresponding early-return gates.

## Owner validation

Run installed `MSM2-WS/Update-WS.cmd` with game/shared Overstrike closed and require the full build/test/smoke/hooks/package/deploy PASS for `v0.1c47 / 0.1.72`.

After PASS, one actual-game WallRun only:

- left about 3 seconds;
- right about 3 seconds;
- down about 5 seconds;
- end naturally;
- no F11/F12;
- no post-test wait.

Return the c47 log and video.

## Acceptance question

First require `wallrun_update_scalar_session` during the verified WallRun. Then, for each of the five offsets:

1. Was it successfully read?
2. Was the raw bit pattern stable or changing?
3. Did it exactly equal any known comparison number `0.4/12/75/100/50`?
4. If changing, did timing correlate with left/right/down intervals?

If c47 still lacks `wallrun_update_scalar_session`, stop immediately as instrumentation failure; do not interpret engine behavior. If the session key exists but all reads fail, inspect the bounded direct-read validity path before any wider memory scan. No write/A-B test until a defensible read-only candidate is identified.
