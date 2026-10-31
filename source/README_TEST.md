# WallSprint v0.1c44 — streaming RequestSimpleMotion presence diagnostic

Runtime **v0.1c44**, package **0.1.69**.

This is a dedicated read-only diagnostic build. `WALLSPRINT_ENABLE=0`,
`WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`, orientation/adhesion/movement writes are all
OFF, and `REQUEST_PRESENCE_DIAGNOSTIC=1`.

## Update

1. Close the game and shared Overstrike.
2. Run the installed `MSM2-WS\Update-WS.cmd`.
3. In shared Overstrike, select the usual mods including WS, then `Install mods` -> `Run modded game`.

## Runtime test

No F11/F12 input is required and there is no post-test waiting period.
After exact RequestSimpleMotion hook admission succeeds, capture starts automatically.
The worker pauses the capture gate every 500 ms, waits for in-flight callbacks to finish,
and persists up to 16 retained opaque Request records plus overflow/contention counts.
Already-written batches do not depend on game/script shutdown for persistence.

1. Start video recording once normal gameplay is visible.
2. Perform the sequence naturally:
   - ground Sprint straight;
   - reverse direction;
   - release movement input;
   - enter ordinary WallRun;
   - move left;
   - move right;
   - move down.
3. Stop the video and close the game normally whenever the sequence is complete. No extra wait is required. F8 is emergency stop only.
4. Return the newest `MSM2-WS-v0.1c44-YYYYMMDD-HHMMSS.log` from `MSM2-WS\logs` together with the video.

The log's `session_offset_ms_est` values are approximate relative timestamps for
matching RequestSimpleMotion records to the video. They are not authenticated engine
state labels or engine timestamps. Captured argument register values remain opaque bits
until runtime evidence supports a narrower interpretation.

The streaming capture is bounded to 16 retained records per 500 ms batch and 65536
retained records per script session. Calls beyond a full batch are counted as overflow
and still forwarded to the original game function.

## Admission identity history

- v0.1c40 is rejected as RequestSimpleMotion runtime evidence because its expected whole-function SHA was wrong.
- v0.1c42 is rejected as RequestSimpleMotion runtime evidence because its expected `.pdata` UnwindData RVA was wrong (`0x1154EDC`). The actual game and earlier accepted audit report `0x94697E0`.
- v0.1c43 proved the exact Request site, `.pdata`, observer unwind and MinHook trampoline can all admit in the real game, but it buffered records until phase finalization; the supplied normal game-exit log contained no finalization/record output. Therefore c43 does not establish Request call presence/absence.
- v0.1c44 preserves the accepted c43 hook identity and changes only capture persistence to periodic worker-side drains.

The accepted RequestSimpleMotion function SHA remains
`2d8792492c8b89d4e9885d55fa45fb2c6b4126b4ae992f854f6d687522163d0c`, with UnwindData RVA `0x94697E0`.
