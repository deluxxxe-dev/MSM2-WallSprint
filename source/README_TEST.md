# WallSprint v0.1c48 — exact Update-entry `this+0x250` threshold probe

Runtime **v0.1c48**, package **0.1.73**.

This is a read-only diagnostic build. `WALLSPRINT_ENABLE=0`, `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`, and orientation/adhesion/movement/gravity/native-source writes are all OFF. The retired c45 `GravityOrientationOwner` presence hook remains disabled (`REQUEST_PRESENCE_DIAGNOSTIC=0`).

## Why c48

c47 established that `WallRun.this+0x250` changes during verified WallRun and that its negative pulses correlate in time with direction reversals / the down-hold oscillation. V8 static consumer analysis then proved two authenticated Update comparisons of this field:

- file-backed initial `0.5f` compared at `0x1424585`, with the comparison result stored by `SETB`;
- file-backed initial approximately `0.71f` compared at `0x142460E`, with `JAE` skipping the following block when the field is less than or equal to the threshold for ordinary finite values.

Every retained c47 100 ms sample was below both thresholds, but a short positive excursion between worker samples was not excluded.

c48 removes only that sampling-gap ambiguity. It retains the c47 five-field worker observation and additionally uses the already-installed authenticated Update pre-original callback. For every post-bridge-admitted Update it performs one guarded 4-byte read of `this+0x250` and counts whether the value exceeds the two live threshold values captured at admission (NaN/unordered is counted separately and follows the relevant `VCOMISS` CF behavior).

The probe adds no native hook, no pointer dereference, no game call and no game write. It returns `0` from the existing write callback, so no old writer path is re-enabled.

## Update

1. Close the game and shared Overstrike.
2. Run installed `MSM2-WS\Update-WS.cmd`.
3. Require the full build/test/smoke/hooks/package/deploy PASS for `v0.1c48 / 0.1.73`.
4. In shared Overstrike, use `Install mods` -> `Run modded game`.

## Runtime test

No F11/F12 input, markers, or post-test waiting are required.

1. Enter one ordinary WallRun.
2. Move right, reverse left, reverse right, then hold down long enough to reproduce the short left/right oscillation.
3. End the WallRun and close the game normally.
4. Return only the newest `MSM2-WS-v0.1c48-YYYYMMDD-HHMMSS.log`.

Video is not required for this discriminator unless the in-game behavior differs materially from the already documented c47 sequence.

Required log keys:

- `wallrun_0x250_threshold_probe_admission`
- `wallrun_0x250_threshold_probe`
- normal `WallRun verified session` summary
- existing `wallrun_update_scalar_sample` records are useful but secondary.

Interpretation gate:

- first require live threshold reads to remain unchanged from admission and the V8 expected threshold bits to match;
- compare exact pre-original sample count with verified Update count; a bridge rejection makes the exact-entry result partial;
- if both threshold counters remain zero with complete coverage, the c47 negative-pulse correlation did not activate either authenticated threshold consumer during this sequence;
- any nonzero threshold count becomes the next bounded event to analyze; it does not by itself prove field semantics or authorize a write.
