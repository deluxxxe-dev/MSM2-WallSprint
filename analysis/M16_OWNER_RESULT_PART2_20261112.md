# M16 owner result, part 2 (2026-11-12, build fe01adf, log MSM2-WS-v0.1c48-20261112-085357, tests 5 onward)

Inspected in full: the log (12172 lines) and both videos over their whole duration (3 frames/s): Web_Zip (9.1 s,
captions up/down/left/right) and fall (9.6 s). Part 1: `analysis/M16_OWNER_RESULT_PART1_20261112.md`.
Test 7 (walk off toward another surface) was not done: the instruction was not concrete enough.

## Owner report
- Web zip direction still wrong.
- Walking off a face: the hero now falls in the face-gravity direction. The fall starts late compared with the visible
  position/motion, and the camera does not follow.

## 1. Walk-off fall (VERIFIED-RUNTIME)
- Two genuine walk-offs: 23:58:27.686 (fell 4.7 s / 127 m, ended `game_fall_ended`) and 23:59:09.078 (fell 2.15 s / 51 m,
  ended by B). During both, `game_step_u` stayed ~0 (pure world-down game steps); the release moved along -u at up to
  30 m/s. Must-have 2 works in principle.
- One more release 23:59:15.579 right after a stop hop landing (v_plane 0.001, on the face), ended after 0.48 s by
  `game_fall_ended`: the same false trigger as part 1 (the game keeps its fall state on the face).
- Late start: the game's own fall began before 23:59:08.746 (first undo already at 9.9 m/s, so ~0.35 s earlier); the
  release came at 09.078. The fall guard undid the game's fall every frame meanwhile (hover with the fall animation).
  The 4-frames-rising rule needed 22 frames because the per-frame speed is noisy (9.0 / 19.4 / 14.4 / 14.7 ... m/s).
  Total delay ~0.7 s.
- Camera: the video shows the camera staying where the hero left the face while the hero falls out of view (1.7-6.0 s);
  after B (game control again) the view jumps to the hero. Same as M11 ("camera manager position stale" during holds):
  the game camera does not follow positions written into Transform+0x30. HYPOTHESIS: the camera (and probably the game's
  collision/controller) follows another hero position that the game updates itself and WallSprint does not write.

## 2. Web zip (VERIFIED-RUNTIME)
- 4 zips after standing jumps; every one used `walk_pair` with pair_age_s 11.1-17.3 s and theta -117.4 deg; results:
  stick up -> e_up -0.42 / side -0.91, down -> +0.40 / +0.92, left -> -0.93 / +0.37, right -> +0.87 / -0.49. Each is
  ~65-110 deg off.
- Cause: a stick -> face-direction mapping is only valid for the camera pose at the time it was measured; the camera
  moves after every zip. Any stored pair (or 2 s regression) is stale after 10+ s. A live camera basis is required.
- Candidate live source (OBSERVED, not validated): `WS_CAMERA_SETTER m0` during face holds has row 1 = the hero up
  (0.931, 0, -0.365) and a row 2 that changes between samples (-0.356,-0.229,-0.906 / -0.167,0.890,-0.425 /
  -0.281,0.638,-0.717 / -0.005,-1.000,-0.014). It must be validated against the game's own walking direction before use.
- `WS_FACE_AIR_FACING` held the forward at the zip direction (landing forward = zip dir), so the body follows the zip;
  the direction itself is the error.

## 3. Snapshot
- Final: F 60, W 60, J 35, H 0, G 60, A 60, L 60; candidate_count_A 0. Confirms part 1: no separating field in hero bytes
  0x000-0xFFF.
