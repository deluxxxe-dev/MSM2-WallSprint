# WallSprint v0.1c43 — automatic RequestSimpleMotion presence diagnostic

Runtime **v0.1c43**, package **0.1.68**.

This is a dedicated read-only diagnostic build. `WALLSPRINT_ENABLE=0`,
`WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`, orientation/adhesion/movement writes are all
OFF, and `REQUEST_PRESENCE_DIAGNOSTIC=1`.

## Update

1. Close the game and shared Overstrike.
2. Run the installed `MSM2-WS\Update-WS.cmd`.
3. In shared Overstrike, select the usual mods including WS, then `Install mods` -> `Run modded game`.

## Runtime test

No F11/F12 input is required. After exact RequestSimpleMotion hook admission succeeds,
the capture starts automatically and runs for up to 5 minutes or 131072 records.

1. Start video recording once normal gameplay is visible.
2. Perform the sequence naturally:
   - ground Sprint straight;
   - reverse direction;
   - release movement input;
   - enter ordinary WallRun;
   - move left;
   - move right;
   - move down.
3. Keep the final state visible for several seconds, then stop the video. F8 is emergency stop only.
4. Return the newest complete `MSM2-WS-v0.1c43-YYYYMMDD-HHMMSS.log` from `MSM2-WS\logs` together with the video.

The log's `offset_ms_est` values are approximate relative timestamps for matching
RequestSimpleMotion records to the video. They are not authenticated engine state
labels or engine timestamps. Captured argument register values remain opaque bits
until runtime evidence supports a narrower interpretation.

## Admission identity history

- v0.1c40 is rejected as RequestSimpleMotion runtime evidence because its expected whole-function SHA was wrong.
- v0.1c42 is rejected as RequestSimpleMotion runtime evidence because its expected `.pdata` UnwindData RVA was wrong (`0x1154EDC`). The actual game and the earlier accepted owner audit both report `0x94697E0` for the pinned RequestSimpleMotion entry.
- v0.1c43 uses `0x94697E0` and the static test now requires production begin/end/SHA/UnwindData to match the accepted audit constants before the long test pipeline runs.

The accepted RequestSimpleMotion function SHA remains
`2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c`.
