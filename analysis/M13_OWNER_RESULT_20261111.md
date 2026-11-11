# M13 owner result (2026-11-11, build 769d39a, log MSM2-WS-v0.1c48-20261111-225552)

Inspected in full: the log (17504 lines) and all four videos over their whole duration (3 frames/s):
WS_Stop (7.5 s), WallJump (16.2 s, captions up/down/left/right), WebZip (9.2 s, captions up/down/left/right),
DontFall (7.3 s). Terms: WS = Wall Sprint, WR = vanilla Wall Run.

## Owner report
1. Stop from WS: no jerk any more, but the hero ends in the vanilla wall crawl pose, not standing; a face jump from there
   ends standing.
2. Walking toward the ground still hitches (also with the camera turned 180 deg, so it depends on the ground direction,
   not on the screen direction).
3. Face jumps: same body turns as before; the visible height and the landing are out of sync.
4. Web zip: every direction except up turns the body to screen left.
5. After walking off the wall during the gravity hold the hero stays in the air and does not fall (DontFall).
6. Owner questions: is this way of working right, why is it hard when the face already works as ground, and can the
   changed gravity stay until B (falling included) or is a structural change needed.

## 1. Stop ends in the crawl (VERIFIED-RUNTIME)
- 4 stops, each `STOP_PUSH off_m=0.5 hold_s=0.12` -> one `WALL_LANDED` at t=0.11-0.16 s. No `REAPPROACH` line in the whole
  log (M13 task 2 works; the M12 triple landing is gone).
- After `WALL_LANDED` the game position is frozen to 1e-5 m and the game step is 0 for seconds (sessions 1-3). The video
  shows the crawl pose from 2.3 s; after the face jump at 3.7 s the hero stands (4.3 s on).
- The game's face jump from that pose produced 0 in-plane motion (`delta_up/side=0`, source=none) and then standing.
- Compare M12: the push 0.9 m / 0.25 s gave a real air phase and the game landed standing 3/3. With 0.12 s the hero never
  leaves the game's wall-contact (crawl) state. Conclusion: the game stands up only after an air phase that it detects;
  the length of the air phase, not the landing count, decides crawl vs standing.

## 2. Walking-down hitch = frame detection bug (VERIFIED-RUNTIME + VERIFIED-STATIC)
- 100 `WS_FACE_FALL_UNDO reason=speed_over_9` (log cap reached), all while walking. speed = undo / dt gives dt = 2.0-2.2
  ms for almost every undo (undo 0.02-0.09 m = a normal walking step; speeds 10-41 m/s are impossible for walking).
- `gfStep`: `newFrame = gap since previous SetPosition call >= 0.002 s`. The 3-4 hero SetPosition calls of one frame are
  2.0-2.2 ms apart, so a later call of the same frame is taken as a new frame with dt ~2 ms; the fall guard divides the
  frame's step by that dt and undoes it. `WS_GRAVITY_FORCE_SAMPLE` also shows dt_ms=2.1 / 3.3 on such frames.
- Only downward steps are checked by the guard, which is why only walking toward the ground hitches.
- The same wrong frame boundary feeds every per-frame quantity (dt of the force model, follow, walk samples).

## 3. Face jumps (VERIFIED-RUNTIME)
- 24 face jumps. Walking jumps: takeoff in-plane 5.0 m/s along the walking direction, travel 2.1-2.4 m, airtime
  0.36-0.47 s, landing delta_n 0.01-0.02 m. Directions match the stick in every walking jump (up -> e_up -1 with the
  camera looking at the ground, down -> +1, left -> side -1, right -> side +1). M13 task 4 works for the position.
- Standing jumps from the crawl: 0 in-plane motion. Standing jumps with no stick (source=none) still travelled 1.5-1.9 m
  in the stick direction pushed after takeoff (air steering).
- Airtime: launch v_n = 10.9 m/s, g = 33 gives 0.66 s, but landings are logged at 0.36-0.47 s. The model lands before
  the arc ends. The game's own jump animation keeps its own time, which matches the owner's "height and landing lag".
- Body facing: the turn is the game's. The game gets a neutral stick only while `gfFaceAirNow` is set (after takeoff);
  on the A press frame the game still reads the real stick. HYPOTHESIS: the game's jump start turns the body toward the
  world-horizontal projection of the stick, which on a face is always the side axis (up -> one side, down -> the other;
  matches the owner's "up -> right, down -> left" and the Wisdoms fact that the game's air actions are world-horizontal).

## 4. Web zip (VERIFIED-RUNTIME)
- 6 `WS_FACE_ZIP`. The zip uses the frozen learned mapping (h, theta), not the takeoff direction. Frozen theta per zip:
  up 173.3 deg (correct), down 73.7 deg (zip dir up -0.40 / side -0.92 for stick down: wrong, goes to the side),
  left 155.7 deg (about 20 deg off), right -178.0 deg (correct). The takeoff walking direction of the same jumps was
  correct each time (stick down jump: v_up +5.0).
- mirror_agree/total at those takeoffs: 0/63, 25/63, 17/62: the learned mirror sign was not consistent, yet used.
- The body turn during the zip is the game's (its zip direction is world-horizontal); not changed by our impulse.

## 5. DontFall: hovering after walking off the wall (VERIFIED-STATIC; time not localisable in the log)
- `gfStep` clamps every frame `if d < 0: q += (want - d) * n0`: the hero is never allowed below the plane. The plane is a
  mathematical infinite plane, not the wall; past the edge of the wall there is nothing below the hero, but the clamp
  holds him on the extension of the plane -> hovering. The video shows the game's fall pose while the position stays.
- The game itself does not move an airborne hero while the gravity is a wall up (M10z-r17b: every game SetPosition step
  in face jumps was 0; here the game position is frozen in the crawl and in the air). So nothing makes him fall.
- `WS_GRAVITY_FORCE_SAMPLE` is capped at 107 lines, so the DontFall moment is not in the sampled frames.

## 6. Assessment of the owner's questions
What the game does itself with the changed gravity (VERIFIED-RUNTIME): standing, walking, running and its collision on
the face. What it does not do: any motion in the air (jump, fall, zip) - its steps are 0 -, the body yaw in the air and
air actions (world-horizontal), and the camera. Every one of those is replaced by WallSprint's own position model, which
the game's animation/state does not know. That is why tuning numbers on our side (speeds, heights, timings) made little
visible progress: the visible mismatches come from game state we do not read (crawl/air/ground, jump animation time,
body yaw). The frame bug in section 2 was introduced by the guard itself.

Can the changed gravity stay until B, with falling? The gravity substitution already persists until B. Falling needs
two things the current code lacks: (a) knowing whether the game considers the hero on ground / in the air / in the crawl
(instead of the infinite plane), and (b) collision for our falling motion (SetPosition does no collision; the game's
collision runs only in its ground locomotion). (a) is an addition (a read-only state signal). (b) decides the structure:
- if the game's air controller can be made to run with the wall gravity (find why its steps are 0), the game does jump,
  fall, collision and animation timing itself and most of our air model is removed (best result, needs investigation);
- otherwise our model must fall without collision and land when the game reports ground; passing through geometry at
  speed remains a risk.
No full rewrite of the mod is needed; the air part changes from "one plane" to "game state + fall along the gravity".

Recommended change of procedure: stop tuning parameters of the air model; fix the frame bug; make the stop give the air
phase the game needs; then read the game's ground/air/crawl state (read-only snapshot diff) before more air work.
