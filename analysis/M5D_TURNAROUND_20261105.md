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

## M5d-1 result (log `MSM2-WS-v0.1c48-20261105-205659.log`, not committed)
Owner: turn-arounds now happen less often than before.
- `wallrun_m5d_input_stack loaded=xinput9_1_0.dll,hid.dll,setupapi.dll exe_input_imports=import:HID.DLL`. XInput 9.1.0 is already in the process before WS loads XInput, but the EXE does not import it statically or by delay-load (loaded dynamically, by an unknown module). Which library the game's own pad path uses: UNKNOWN.
- 22 samples (8 reversal, 14 override) were fitted offline against stick/camera mappings: yaw-only camera mapping (both signs), camera forward/right, camera up/right, and ours. None agrees beyond chance (7-14/22; mean dot -0.18..+0.05).
- The game's value is almost always the pure horizontal wall axis with one sign held over a stretch: +x 12:00:29-39, -x 12:00:46-51, -z 11:59:44, +z 11:59:50-51. It is replaced while our direction has a large vertical part or the opposite lateral sign.
- HYPOTHESIS: the game keeps its own left/right run direction for the WallRun and reasserts it. A turn-around plays when our direction opposes it. Not stick-driven, so the stick-rotation route is dropped for now.
M5d-2 diagnostic (no behavior change): `wallrun_m5d_direction_slots`. On every frame where the game replaced our direction with a pure horizontal axis, each WallRun this+0..0x600 unit slot within 0.97 of that axis is counted as same sign / opposite sign, and as same sign one frame earlier.
Decision: a slot with same=all and opposite=0 over both event signs (events_pos and events_neg > 0) -> the game's run-direction state. Next one change: keep that slot's sign equal to our lateral sign during WS. No such slot -> the state lies outside this+0x600 (widen the range or use a write watch).
