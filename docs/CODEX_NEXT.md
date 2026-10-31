# Next task — validate v0.1c46 verified-WallRun Update scalar observation

Branch `codex/v01c20-edi-value-provenance`; no main merge.

Read AGENTS required docs, `analysis/GRAVITY_OWNER_C45_ZERO_HIT_AND_C46_SCALAR_PLAN_20261031.md`, and current `docs/CODEX_RESULT.md`.

## Accepted runtime results

Do not repeat RequestSimpleMotion or GravityOrientationOwner presence testing for the same ordinary locomotion / WallRun sequence.

- c44 exact RequestSimpleMotion entry: 335 persisted 500 ms batches, all zero-hit, while verified WallRun activity occurred.
- c45 exact GravityOrientationOwner entry: 418 persisted 500 ms batches, all zero-hit, while one verified WallRun ran Init=1 / Entry=1 / Update=1470 / Deinit=1 for about 16.280694 s.

Both exact presence routes are stopped for this tested vanilla sequence. Do not enlarge buffers, wait longer, hook SetGravityVector, or search adjacent reference targets without a materially different state-specific reason.

## Current implementation

`v0.1c46 / package 0.1.71` is published.

C46 disables the c45 Gravity presence hook (`REQUEST_PRESENCE_DIAGNOSTIC=0`) and reuses only the already verified WallRun lifecycle/object observation path.

The exact authenticated WallRun Update body is `0x1424530..0x14246D6`, SHA256 `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`.

Static direct-read evidence selects exactly five unique 4-byte `WallRun.this` candidates:

- `0x208`
- `0x220`
- `0x238`
- `0x250`
- `0x40C`

The observer samples only those direct fields on the existing ~100 ms worker observation gate while the verified WallRun session is active and has recent native Update activity.

It logs:

- first raw bit pattern/value;
- later bit-pattern changes, bounded to 64 change logs per session;
- session summary with attempts/read count/change count/first/last/min/max/stability;
- exact numeric-hit counts for `0.4 / 12 / 75 / 100 / 50` only as schema-number comparison.

No field semantic name is authenticated. Numeric equality alone must not be promoted to `TurnMaxDown`, `TurnSpeed*`, etc.

## Safety contract

- no new native hook;
- no pointer dereference;
- no game call;
- no scalar write;
- no gravity/orientation/adhesion/movement/native-source write;
- `WALLSPRINT_ENABLE=0`;
- `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`;
- existing four verified WallRun hooks remain the only required lifecycle hooks;
- compiled dormant c45 presence observer retains its exact identity/safety static checks.

## Completion / owner validation

Run installed `MSM2-WS/Update-WS.cmd` with game/shared Overstrike closed and require the full build/test/smoke/hooks/package/deploy PASS for `v0.1c46 / 0.1.71`.

After PASS, Chat-side actual-game test is one ordinary WallRun only:

- left about 3 seconds;
- right about 3 seconds;
- down about 5 seconds;
- end WallRun naturally;
- no F11/F12 and no post-test wait.

Return the c46 log and video.

## Acceptance question

For each of the five offsets, determine:

1. Was the field readable throughout the verified WallRun?
2. Was the raw bit pattern stable or did it change?
3. Did it exactly equal any of the known serializer numbers `0.4/12/75/100/50`?
4. If it changed, did the timing correlate with left/right/down intervals?

Do not perform a write/A-B test until this read-only result identifies a defensible candidate. If none of the five candidates provides useful linkage, stop this bounded Update-scalar route and return to Chat for the next source decision rather than broad memory scanning.
