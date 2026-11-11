# M11 owner result and analysis (2026-11-11, build 7c9da41: M11 tasks 1 and 2)

Evidence inspected (all of it): log `MSM2-WS-v0.1c48-20261111-180957.log` (16272 lines; `WALLSPRINT_WS_GRAVITY_SETPOS=1`,
i.e. the normal run, not the SETPOS=0 comparison) and three owner videos sampled over their full duration:
`WS_Test` (17.9 s; captions Test#1_Success, Test#2_Failure, Test#3_Failure), `WallJump_Test` (14.6 s; captions up, down,
left, right), `WebZip_Test` (8.8 s; captions down, up, left, right). Raw files are not committed.

Owner report: walking toward the ground on the face is fixed. Stop from WS: 1 of 3 succeeded. Face jump and web zip:
same kind of wrong directions as before.

## 1. Task 2 result: walking down the face — ACCEPTED by the owner.

## 2. Face jump / web zip direction: the camera manager position is stale in the face state (VERIFIED-RUNTIME)
- `WS_FACE_JUMP_INPUT` logs `camPos=-194.290,62.293,174.730` unchanged for nine jumps from 09:14:23 to 09:14:42 while
  `heroPos` moves by several metres; `WS_CAMERA_DISTANCE distances=0xd54` grows 4.41 -> 6.92 m in the same period. In the
  video the real camera follows the hero at a constant distance. So `ws_camref_this + 0xD54` (and therefore the whole
  matrix at +0xD24) is not the rendered camera while the hero stands on the face.
- Consequence: `screen_fwd = proj(heroPos - camPos)` points from a fixed world point to the hero and turns with the
  hero position: (-0.17,0.87,-0.45) -> (-0.33,0.36,-0.87) -> (0.02,1.00,0.07) -> (-0.13,-0.93,-0.35) within 20 s with an
  unchanged real camera. Directions are effectively arbitrary (owner videos: up/down/left/right jumps all end in
  similar small sideways hops; zips fly diagonally and the camera swings).
- The sign learning (`s=1` in every line) never decided.
- What IS correct in this state: the game's own face walking follows the stick (owner: walking works in all four
  directions). The game's in-plane step while walking is the only verified stick->direction reference available.

## 3. Stop from WS: success and failure differ by the rest distance from the wall (VERIFIED-RUNTIME)
| Run | start_d | WALL_LANDED | rest plane_d after landing | pin_drops_undone / dy_game_sum | Video |
|---|---|---|---|---|---|
| 09:13:22 (Test#1) | 0.16 | t=0.35 | -0.00 .. -0.01 m | 0 / -0.03 m | lands, crouch, stands |
| 09:13:29 (Test#2) | 0.48 | t=0.44 | 0.55 .. 0.56 m | 384 / -9.7 m | stays in the fall pose |
| 09:13:43 (Test#3) | 0.46 | t=0.45 | 0.56 m | 225 / -3.7 m | stays in the fall pose |
- In the failures the model velocity is zero (pinned) while the hero rests 0.56 m in front of the wall-point plane: it
  is held in the air, so the game never grounds and keeps falling (cancelled every frame by the drop undo).
- 0.56 m equals the WallRun standoff (start_d 0.46-0.48, `ws_standoff=0.50`). HYPOTHESIS: the landing was evaluated
  against a plane ~0.5 m off the wall (plane point `gfP0` replaced between landing and later samples: it is rewritten on
  every worker tick while WS is active, from the wall point or from `hero - 0.5 n`), or the game moved the hero back out
  after the landing and nothing brought it back. Not distinguishable from this log (gfP0 is not logged).
- Side observation: the stop push peaks at plane_d 2.4-2.7 m although 0.9 m is requested.

## 4. Web zip
- Same direction source as the face jump (item 2). The zip animation/camera swing is the game's.
