# M5d: vanilla WallRun turn-around during sideways WS (2026-11-05)

## Known so far
- During sideways WS, the game's own WallRun update keeps replacing our direction (+0x240/+0x24C) with the exact opposite horizontal axis for runs of consecutive frames. The turn-around animation plays (OBSERVED-CORRELATION, M5c runs 6-8).
- It happens in open space, more often on a wide ellipse with the stick near horizontal (owner).
- The game's target is not stored in WallRun this+0..0x600 before the update. It is computed inside the original update from input and camera (M5c-r7/r8 diagnostic).
- Before the WS camera roll (M3d, unrolled camera), the game's lateral choice agreed with the screen (the M3d lateral calibration was taken from the game's own choice).

## Route options
1. Make the game choose our direction itself, so no disagreement and no turn-around:
   if the game reads the pad through XInput, rotate the left stick the game sees during WS only (the right stick/camera is untouched).
2. Otherwise: find the game's stick->direction formula and the input path it uses (needs the input library or code location).

## M5d-1 diagnostic (no behavior change)
- `wallrun_m5d_input_stack`: input DLLs already loaded by the game before WS loads XInput, and the input-related DLL names in the EXE import and delay-import tables (read-only PE walk).
- `wallrun_m5c_vanilla_reversal` now also has camera forward/up (`cam_f`, `cam_u`).
- New `wallrun_m5d_vanilla_override`: every frame where the game's update changed our direction (dot < 0.98, not only reversals), same fields, 2 Hz.
Decision table:
| result | next |
|---|---|
| XInput imported/loaded by the game | M5d-2: WS-only left-stick rotation in an XInputGetState hook (one change) |
| Only another input stack (GameInput/HID/ScePad) | fit the formula from the override samples; choose a hook on that stack or a different route |
| Override samples fit "lateral = world-up x camera forward" (no roll) | confirms the roll is the cause; the stick-rotation fix stays valid |
