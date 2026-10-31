# WallSprint v0.1c46 — verified-WallRun Update scalar observation

Runtime **v0.1c46**, package **0.1.71**.

This is a read-only diagnostic build. `WALLSPRINT_ENABLE=0`,
`WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`, and orientation/adhesion/movement/gravity writes are all OFF.
The c45 `GravityOrientationOwner` presence hook is disabled (`REQUEST_PRESENCE_DIAGNOSTIC=0`).

## Why c46

v0.1c45 fully admitted the exact `GravityOrientationOwner` hook on the pinned target build and kept its 500 ms streaming capture active through the supplied gameplay session. All 418 persisted batches retained zero records, including a verified WallRun lasting about 16.28 seconds with 1470 Update calls. The exact Gravity owner entry is therefore a bounded zero-hit for the tested ordinary locomotion/WallRun sequence and is retired as the next vanilla observation source.

The next smallest question is inside the already verified WallRun `Update` function itself. Static analysis of its exact authenticated body shows five unique 4-byte fields read directly from the WallRun object through the preserved `this` alias:

- `+0x208`
- `+0x220`
- `+0x238`
- `+0x250`
- `+0x40C`

c46 observes only those five fields from the existing worker-side verified WallRun object at about 100 ms cadence. It adds no native hook and performs no pointer dereference or game write. The values remain semantically unknown.

For comparison only, the log counts exact numeric matches against previously recovered serializer numbers `0.4`, `12`, `75`, `100`, and `50`. A numeric match does **not** prove which setting or engine meaning a field has.

## Update

1. Close the game and shared Overstrike.
2. Run installed `MSM2-WS\Update-WS.cmd`.
3. Require the normal full build/test/smoke/hooks/package/deploy PASS for `v0.1c46 / 0.1.71`.
4. In shared Overstrike, use `Install mods` -> `Run modded game`.

## Runtime test

No F11/F12 input is required and no post-test wait is required.

1. Start video recording once normal gameplay is visible.
2. Enter one ordinary WallRun and keep it going long enough to perform:
   - left for about 3 seconds;
   - right for about 3 seconds;
   - down for about 5 seconds.
3. End the WallRun naturally, stop the video, then close the game normally.
4. Return the newest `MSM2-WS-v0.1c46-YYYYMMDD-HHMMSS.log` together with the video.

Expected log keys:

- `wallrun_update_scalar_plan`
- `wallrun_update_scalar_sample`
- `wallrun_update_scalar_summary`
- normal verified `WALLRUN_HOOK` / `WallRun verified session` lifecycle records.

The scalar sample log is limited to the first value and later bit-pattern changes, with a bounded change-log count. The session summary reports attempts, read count, change count, first/last/min/max and exact numeric-match counts. This is intended to distinguish stable configuration-like values from values that vary during steering without assigning semantics prematurely.
