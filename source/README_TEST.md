# WallSprint v0.1c47 — verified-WallRun Update scalar observation

Runtime **v0.1c47**, package **0.1.72**.

This is a read-only diagnostic build. `WALLSPRINT_ENABLE=0`,
`WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`, and orientation/adhesion/movement/gravity writes are all OFF.
The retired c45 `GravityOrientationOwner` presence hook remains disabled (`REQUEST_PRESENCE_DIAGNOSTIC=0`).

## Why c47

The c46 owner test successfully established one verified WallRun (`Init=1 / Entry=1 / Update=1889 / Deinit=1`, about 22.27 seconds) on the exact target build. However, c46 produced no scalar sample or summary records because the five-field observer was accidentally placed behind the intentionally disabled legacy `surfaceTelemetry` gate. Therefore c46 is **not** accepted as zero-value or unreadable-field evidence.

c47 fixes only that instrumentation wiring. The five direct 4-byte reads are now armed, sampled, and finalized independently of legacy surface telemetry. The old rich surface/child observation remains disabled, so c47 does not broaden memory traversal.

The exact authenticated WallRun `Update` body still supplies the same five unique direct `this` scalar-read candidates:

- `+0x208`
- `+0x220`
- `+0x238`
- `+0x250`
- `+0x40C`

For comparison only, the log counts exact numeric matches against `0.4`, `12`, `75`, `100`, and `50`. Numeric equality alone does not authenticate a field name or engine meaning.

## Update

1. Close the game and shared Overstrike.
2. Run installed `MSM2-WS\Update-WS.cmd`.
3. Require the full build/test/smoke/hooks/package/deploy PASS for `v0.1c47 / 0.1.72`.
4. In shared Overstrike, use `Install mods` -> `Run modded game`.

## Runtime test

No F11/F12 input and no post-test waiting are required.

1. Start video recording once normal gameplay is visible.
2. Enter one ordinary WallRun and perform:
   - left for about 3 seconds;
   - right for about 3 seconds;
   - down for about 5 seconds.
3. End the WallRun naturally, stop the video, then close the game normally.
4. Return the newest `MSM2-WS-v0.1c47-YYYYMMDD-HHMMSS.log` with the video.

Required log keys:

- `wallrun_update_scalar_plan`
- `wallrun_update_scalar_session` — proves the five-field observer actually armed even while legacy surface telemetry is off
- `wallrun_update_scalar_sample`
- `wallrun_update_scalar_summary`
- normal verified `WALLRUN_HOOK` / `WallRun verified session` lifecycle records

The summary reports attempts, successful reads, bit changes, first/last/min/max and exact numeric-hit counts. If the five fields are readable, c47 can finally distinguish stable configuration-like values from values that vary during steering without assigning semantics prematurely.
