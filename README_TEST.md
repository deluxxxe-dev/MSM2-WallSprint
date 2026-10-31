# WallSprint v0.1c41 — RequestSimpleMotion continuous presence diagnostic

Runtime **v0.1c41**, package **0.1.66**.

This is a dedicated read-only diagnostic build. `WALLSPRINT_ENABLE=0`,
`WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`, orientation/adhesion/movement writes are all
OFF, and `REQUEST_PRESENCE_DIAGNOSTIC=1`.

## Update

1. Close the game and shared Overstrike.
2. Run the installed `MSM2-WS\Update-WS.cmd`.
3. In shared Overstrike, select the usual mods including WS, then `Install mods` -> `Run modded game`.

## Runtime test

1. Move to a place where both ground Sprint and ordinary WallRun can be performed.
2. Start video recording.
3. Press **F11 once**. Do not press F11/F12 again during the capture.
4. After about two seconds, perform the whole sequence naturally within the 45-second capture window:
   - ground Sprint straight;
   - reverse direction;
   - release movement input;
   - enter ordinary WallRun;
   - move left;
   - move right;
   - move down.
5. The capture closes automatically after 45 seconds or the 16384-record limit. F8 is emergency stop only.
6. Return the newest complete `MSM2-WS-v0.1c41-YYYYMMDD-HHMMSS.log` from `MSM2-WS\logs` together with the video.

The log's `offset_ms_est` values are approximate relative timestamps for matching
RequestSimpleMotion records to the video. They are not authenticated engine state
labels or engine timestamps. Captured argument register values remain opaque bits
until runtime evidence supports a narrower interpretation.

v0.1c40 is rejected as runtime evidence for RequestSimpleMotion records: its
runtime admission constant used the wrong whole-function SHA and therefore the
F11 capture never armed. The accepted audit/runtime SHA for the exact pinned site
is `2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c`.
