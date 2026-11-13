# Ground sprint on the face: owner test result (2026-11-13, log MSM2-WS-v0.1c48-20261113-162504)

Inspected in full: the log (10149 lines) and both videos over their whole duration (6 frames/s): WS-Stop.mp4 (4.9 s) and
vanillasprint-stop.mp4 (2.1 s, reference). Direction check: `analysis/WS_GROUND_SPRINT_DIRECTION_20261113.md`.

## Videos
- WS-Stop: WS up the face 0-1.3 s; stop at ~1.4 s; crawl pose 1.5-3.6 s; A jump 3.7-4.2 s; standing from 4.3 s.
- vanillasprint-stop (owner's target): ground RT sprint; braking starts ~0.9 s, a short brace/skid pose 1.0-1.3 s, standing
  idle by ~1.5 s. One continuous ground animation, no air phase.

## Log (VERIFIED-RUNTIME)
- Hold 1 (07:26:12-07:26:24, face-as-ground after the stop hop and a face jump):
  - walking 07:26:13-17 at ~1.2 m/s;
  - 07:26:17.4-07:26:20.2 ~10 m/s with WallSprint=inactive and no new WR session: per-frame game steps step_side
    0.169-0.172 m, step_n 0.0000, plane_d 0.000 -> the game's own ground sprint runs ON the face, sideways, with no
    WallSprint correction;
  - 07:26:20.2-21.2 speed 5.97 -> 1.29 -> 0: the game's own sprint stop, no hop, no crawl.
- Hold 2 (07:26:29-07:26:39): walking ~1.0-1.4 m/s, then sprint up the face 3.7 -> 9.5 -> 11.9 m/s; at 07:26:39.4 a vanilla
  WR session started (session_timeline id=3, candidate WallRunUp) and became a new WS (`STOP_TRACE event=cancel
  reason=new_ws`).

## Conclusion
- The game's ground sprint and its natural stop work in the face-as-ground state (sideways run verified). This is the
  owner's target behaviour.
- Running up the face with RT makes the game start its WR (wall run up). That must not happen while the hero stands on the
  face.
- Plan: (1) no WR start while the hero stands on the face in the hold; (2) then make the WS start enter the face-as-ground
  state directly (the hero arrives at the wall from the air), so all running is the game's sprint and the stop hop is
  removed.
