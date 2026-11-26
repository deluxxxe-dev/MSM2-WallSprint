# M19 owner result (2026-11-26, build 3411563, log MSM2-WS-v0.1c48-20261126-114401)

Inspected in full: the log (14631 lines) and all three videos over their whole duration (4 frames/s): up.mp4 (5.4 s),
down.mp4 (6.3 s), side.mp4 (4.4 s). Owner order: up -> down (ended behind the building, fast travel) -> side -> down.
Terms: WS = Wall Sprint, WR = vanilla Wall Run.

## Owner report
- down: pushing the stick toward the ground, the hero turned sideways at once, left the camera view and moved upward; after
  stopping it was on the back side of the building.

## Up (VERIFIED-RUNTIME + video): the game's sprint and stop, no WR
- 02:50:00.4-02:50:05.3: ~10 m/s straight up the face (world dy +5.4 m per 0.55 s), WallSprint inactive, no WR session;
  02:50:05.3-05.9: 2.3 -> 0.3 -> 0 m/s, the game's own stop. Video: run toward the roof, natural stop and standing at
  4.5-5.3 s. The 2026-11-13 WR start while running up did not happen this time.

## Side (VERIFIED-RUNTIME + video): the game's sprint and stop
- 02:50:38.2-02:50:41.4: ~10 m/s sideways (dy -0.13), stop 8.4 -> 1.8 -> 0. Video: natural run and stop.

## Down (VERIFIED-RUNTIME): WallSprint's own fall rules break the sprint toward the ground
- 02:50:10.8-11.3 the game's sprint down accelerates (step_up -0.032 -> -0.105 m/frame). At 9.38 m/s the fall guard
  (`speed_over_9`) undid 0.105 m of the game's step (the sprint is ~10 m/s, above the 9 m/s rule).
- 02:50:11.678 the walk-off rule released (`WS_FACE_FALL_DETECT window_speed 4.74 -> 6.28 m/s`): the sprint start
  (0 -> 10 m/s in ~0.5 s) has the same acceleration as a fall. 6 ms later a vanilla WR started (session 2,
  candidate WallRunUp) and became a new WS (`WS_FACE_FALL event=end reason=new_ws`). Video: the body turns sideways (WR
  pose) at 0.5 s, the camera spins, the hero ends on another face.
- Second down (02:50:49.136): the same release at v_plane 10.06 (`window 2.13 -> 4.26`), a 0.025 m probe/contact abort, and a
  WR session 4 (WallRunUp) at 02:50:49.155.
- `WS_FACE_WR_START`: 0 lines. The diagnostic only records while the hero is pinned on the face; both WR starts came right
  after the release (no longer pinned), so the gate excluded them.

## Conclusion
- The game's ground sprint and its natural stop work on the face up and sideways (owner's target behaviour).
- Down fails because WallSprint's fall guard (9 m/s) and walk-off detection (window acceleration) treat the sprint toward
  the ground as a fall; the release then lets the game start a WR.
- The WR refusal (M19 step 2) is not needed for these cases; it may become unnecessary if releases stop misfiring.
