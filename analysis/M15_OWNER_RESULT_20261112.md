# M15 owner result (2026-11-12, build e8e1131, log MSM2-WS-v0.1c48-20261112-015554)

Inspected in full: the log (11710 lines) and all four videos over their whole duration (3 frames/s): WS_Stop (6.2 s),
WallJump (9.9 s, captions up/down/left/right), WallJumo_Failed (11.5 s, repeated stick jumps after the test), WebZip
(9.3 s, captions up/down/left/right). Terms: WS = Wall Sprint, WR = vanilla Wall Run.

## Owner report
- Stop from WS: the hero now stands automatically, but the position shifts for a moment (unnatural).
- Up/down stick jumps: less landing lag and no more sideways body turn; the camera jerks for a moment. Left/right stick
  jumps: landing still lags. After the test, repeated stick jumps showed the old faults again (up input -> faces right;
  keeps facing left while jumping right).
- Web zip: direction still wrong.
- Walking off the wall during the gravity hold: no fall (fall animation plays, position stays).
- Test 8 (walk into an obstacle on the face) not done (no suitable building found).

## 1. Stop (VERIFIED-RUNTIME)
- 4 stop hops; landing airtime 0.41-0.43 s; the hero stands (owner + video 3.7 s on). Single gravity (M15 task 1) works.
- The hop starts at plane_d = 0.45 m (the WS hero is ~0.45 m off the wall plane), rises 0.5 m to ~0.95 m and comes back to
  0: the visible outward-and-back motion is ~1 m, plus the 0.45 m approach. That is the owner's "position shift".
- One stop (17:00:58.9) logged a takeoff and no landing (the next WS started 4 s later).

## 2. Face jumps (VERIFIED-RUNTIME)
- Arc now as designed in every jump: peak 1.80 m at 0.32-0.335 s, landing 0.64-0.66 s, travel 3.3 m (walking takeoff)
  and 8.5-8.9 m with a zip.
- Facing: first jumps keep forward = walking direction through landing. Later jumps (17:02:50-17:02:54) start with the
  right forward and then turn ~70 deg toward the side axis during the air phase (e.g. forward_up -0.95 -> -0.31,
  forward_side 0.32 -> 0.95 at landing). Zips also turn the body (17:02:13: -1.00/0.01 -> -0.68/0.74). So the game still
  rotates the body in the air in some jumps; masking the stick is not sufficient.
- Wrong directions: jumps with takeoff source `model` (standing with the stick pushed) used `walk_pair` mappings with
  theta 81.7 deg and -86.2 deg (correct ~ +-175 deg): stick down -> side -1, stick right -> e_up -1. Those pairs were
  stored while the walking direction was still turning toward a new stick direction (the game turns the hero
  gradually), so the pair mixed the new stick with the old walking direction.
- Left/right landing lag: the arcs are identical for all directions (0.65 s); the lag is not in the position model
  (OBSERVED: owner sees it only for left/right; cause unknown, possibly the game's side-jump animation).

## 3. Web zip (VERIFIED-RUNTIME)
- 5 zips; each used the frozen mapping of its jump, so its direction was wrong whenever the jump's mapping was wrong
  (section 2). The body turn during zips is the game's.

## 4. Walking off the face: the game falls in WORLD down (VERIFIED-RUNTIME) - corrects an earlier claim
- 17:01:59.0-59.5 and 17:02:55.9-56.6: after the hero walked past the edge, every frame the game's own step went down in
  world y with a speed rising 10.7 -> 24.4 m/s in 0.5 s (~26 m/s^2): the game treats the hero as airborne and applies
  WORLD gravity (its internal fall velocity), not the substituted wall gravity.
- The fall guard undid this every frame (`WS_FACE_FALL_UNDO speed_over_9`, undo 0.18-0.40 m/frame) -> the hero hovered while
  the game played the fall animation (owner: "fall animation only, position stays").
- Then the walk-off proxy released (`five_of_ten_walk_then_eight_zero`) and 3 frames later "landed" (`three_game_steps`):
  the "game moves the hero again" rule fired on the game's own world-down fall steps; the plane was re-placed at the
  current position (`walkoff_land_proxy`) -> hovering again. Both times.
- Correction: "the game does not move an airborne hero under a wall up" is wrong in general. During WallSprint's own face
  jumps (position overridden every call) the game's steps are 0, but when the game itself considers the hero airborne it
  moves it with world gravity.
- Usable signal: the game's own fall signature (world-down step speed rising by ~0.4 m/s per frame) is a direct
  "game says airborne" indicator, available without the snapshot work. Walking down a face gives a steady <= 6 m/s.

## 5. Snapshot (VERIFIED-RUNTIME)
- Final report F=60 W=60 J=55 H=5 G=60 A=60, coverage_complete=0 (H < 10), candidate_count=0, no move-vector lines.
- J (WallSprint face jump) is not the game's airborne state (the game does not fall during J, section 4), so requiring one
  offset to separate {F,W,G} from {J,A} together is the wrong question. The separation must be tested against A (and H)
  without J.
