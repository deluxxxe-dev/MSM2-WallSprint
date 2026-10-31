# WallSprint v0.1c45 — streaming GravityOrientationOwner presence diagnostic

Runtime **v0.1c45**, package **0.1.70**.

This is a dedicated read-only diagnostic build. `WALLSPRINT_ENABLE=0`,
`WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`, orientation/adhesion/movement/gravity writes are all
OFF, and the legacy config switch `REQUEST_PRESENCE_DIAGNOSTIC=1` now enables the
`GravityOrientationOwner` presence observer.

## Why this target

v0.1c44 successfully proved the RequestSimpleMotion observer and 500 ms streaming
persistence work in the real game, but the exact RequestSimpleMotion entry produced a
bounded **zero-hit** result across the tested ordinary movement/WallRun session. That
route is therefore stopped as the vanilla locomotion observation source.

v0.1c45 changes only the observed native target identity. It asks whether the already
accepted `GravityOrientationOwner` entry executes during ordinary ground Sprint and
ordinary WallRun. It does not interpret the captured arguments and does not hook or call
`SetGravityVector`.

## Update

1. Close the game and shared Overstrike.
2. Run the installed `MSM2-WS\Update-WS.cmd`.
3. Require the normal full build/test/smoke/hooks/package/deploy PASS.
4. In shared Overstrike, select the usual mods including WS, then `Install mods` -> `Run modded game`.

## Runtime test

No F11/F12 input is required and there is no post-test waiting period.
After exact GravityOrientationOwner hook admission succeeds, capture starts automatically.
The worker pauses the capture gate every 500 ms, waits for in-flight callbacks to finish,
and persists up to 16 retained opaque presence records plus overflow/contention counts.
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
3. Stop the video and close the game normally whenever the sequence is complete. **No extra wait is required.** F8 is emergency stop only.
4. Return the newest `MSM2-WS-v0.1c45-YYYYMMDD-HHMMSS.log` from `MSM2-WS\logs` together with the video.

The log keeps the established `REQUEST_PRESENCE_*` record keys because the proven
observer module is reused with minimal code change. c45 adds
`target_role=GravityOrientationOwner` to identify the actual observed native target.
`session_offset_ms_est` values are approximate relative timestamps for video correlation;
they are not authenticated engine-state labels or engine timestamps. Captured RCX/RDX/R8/R9
values remain opaque bits until later evidence supports a narrower interpretation.

The streaming capture is bounded to 16 retained records per 500 ms batch and 65536
retained records per script session. Calls beyond a full batch are counted as overflow
and still forwarded to the original game function.

## Exact c45 target identity

- role: `GravityOrientationOwner`
- begin RVA: `0x134EF00`
- end RVA: `0x134EFBB`
- whole-function SHA256: `bc9e1a75a4d45f4dc1a1baf99b704778714e83353d58688dce82bab901514a53`
- `.pdata` UnwindData RVA: `0x94A1CB8`
- stolen prefix: 5 bytes / one instruction, accepted prefix SHA256 `0666a96c5b537b54fb43e8d983f54c516a379f17f302825c7f047c7e9ff4cab0`

The accepted function contains a previously verified direct call to the partial
SetGravityVector target, but c45 does not hook, invoke, suppress, or modify that call.
