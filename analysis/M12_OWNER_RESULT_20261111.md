# M12 owner result and analysis (2026-11-11, build 912857b: M12 tasks 1 and 2)

Evidence inspected (all of it): log `MSM2-WS-v0.1c48-20261111-190151.log` (10259 lines) and four owner videos sampled over
their full duration: `WS_Success` (4.6 s), `Getting_caught_while_walking_on_a_wall_above` (5.9 s), `WallJump_Test`
(21.1 s; captions up, down, left, right), `Webzip_Test` (10.7 s; captions up, down, left, right). Raw files not committed.

Owner report:
- Stop from WS: 3 of 3 succeed, but with jerks of the camera and the hero. **From here the priority is removing jerks.**
- Face walking: only when walking toward the ground (screen up) the motion hitches every few seconds (the animation
  slows down briefly; not a position shake).
- Face jumps: body faces right for up input, left for down, left for left, right for right; the movement is always
  "into the screen" (hardly visible for up); unlike a ground directional jump the hero drifts slowly some time after
  the takeoff.
- Web zip still wrong: the web line direction differs per zip, but the final movement and facing are always screen right.

## 1. Stop from WS: three landings within ~0.4 s (VERIFIED-RUNTIME) = the jerk
Every stop shows `STOP_PUSH` -> `WALL_LANDED` -> `REAPPROACH d=1.21/0.54/0.32` -> `WALL_LANDED` -> `REAPPROACH
d=0.52/0.33` -> `WALL_LANDED` (10:03:21.88 / 22.06 / 22.28; 10:03:28.05 / 28.19 / 28.38; 10:03:33.42 / 33.60). After
each landing the game moves the hero 0.3-1.2 m back out and the M12 re-approach pulls it in again. The hero finally
stands (pin_drops_undone 14-15, dy_game_sum ~0), but the sequence is push 0.9 m out, land, out, land, out, land.

## 2. Periodic re-approach loop while standing elsewhere on the wall (VERIFIED-RUNTIME)
From 10:05:09.67 on: `REAPPROACH d=0.180` and `WALL_LANDED` repeat every ~0.41 s (5+ cycles) with the plane point frozen
at the hold start point (-192.319, 87.544, 181.483), ~30 m of height away from the hero. At the hero's current place the
game rests it 0.18 m from that plane (the wall is not exactly that plane there), which exceeds the 0.15 m re-approach
threshold forever. Each cycle is a visible jerk.

## 3. Walking toward the ground hitches (VERIFIED-RUNTIME)
`WS_FACE_MOVE_SAMPLE`: 49 downward game steps, mean 0.053 m, max 0.089 m per frame; 3 of them were clipped by the M11
per-frame budget (`store_dy` 0.006 / 0.037 / 0.013 m with `down_budget_left=0`), at 10:03:47.9, 10:03:49.6, 10:03:53.2 =
every few seconds. The walk/run speed down the face reaches ~5.3 m/s, at the 6 m/s budget, so longer frames exceed it.
The clipped frame moves the position less than the animation expects (owner: motion slows briefly).

## 4. Face jumps (VERIFIED-RUNTIME + video)
- `WS_FACE_JUMP_INPUT source=walk` for all directional jumps; directions match the walking direction: stick up ->
  dir_up=-1.00 (down the building = screen up), down -> +1.00, left -> dir_side=-1.00, right -> +1.00. So the takeoff
  direction itself is now consistent with walking.
- Speeds are the measured walking speed at the press: 1.0-1.8 m/s for the up jumps, 5.0-5.2 (down), 2.9-4.3 (left),
  2.6-5.2 (right). A face jump lasts ~0.4 s, so the travel is 0.4-2 m: slow drift instead of a ground-like directional
  jump (owner). Ground jump speed reference: sprint/run speed is kept, not the instantaneous sampled step.
- Body facing during the jump is the game's own yaw (world-horizontal air facing) and is not controlled by us:
  it explains "faces right/left" independent of the travel. Not solved.

## 5. Web zip (VERIFIED-RUNTIME + video)
- `WS_FACE_ZIP source=model`: the learned mapping changes between zips: h/theta = -1/177, +1/136, -1/-18, -1/126 deg.
  Stick left gave dir_side=+0.92 (wrong side), stick right gave mostly dir_up. The camera swings after every zip and the
  2 s walking window mixes samples from different camera poses, so the model is not stable here.
- Video: after each zip the game camera rotates to a world-oriented view and the hero ends at the right of the screen;
  the game's own zip motion/facing (world-horizontal) is still active besides our impulse.
