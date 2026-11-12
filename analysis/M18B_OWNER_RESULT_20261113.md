# M18b owner result (2026-11-13, build 02ee018, log MSM2-WS-v0.1c48-20261113-022029)

Inspected in full: the log (18769 lines). No video this time. Owner: "does not stand unless I jump".

## Stops (VERIFIED-RUNTIME)
| WS end | debounce | hold_start plane_d (frame 1) | STOP_HOP | result |
|---|---|---|---|---|
| 17:26:54.956 | kept 0.219 s | -0.00 | none | crawl; trace `unavailable reason=episode_not_started` |
| 17:27:51.815 | kept 0.219 s | +0.11 | takeoff v_plane0 8.70, landing 0.356 s, slide 1.52 m | stands (crawl_suspect=0) |
| 17:28:05.333 | kept 0.219 s | -0.00 | none | crawl; owner jumped at 17:28:06.142 |
| 17:28:28.689 | kept 0.219 s | +0.12 | takeoff v_plane0 5.52, landing 0.366 s, slide 0.97 m | stands (crawl_suspect=0) |

## Cause (VERIFIED-STATIC, matches every row)
`StopHopArc::solve(distance)` returns false for `distance < 0`. During the new 0.219 s debounce the game already pulls the
hero onto the face (WS_JUMP_TRACE disp_n -0.35 to -0.44 m), so at hold start the hero is exactly on the plane or a few mm
inside it (-0.00). Then no hop is launched, silently, and the game stays in its wall-contact (crawl) state. With +0.11 /
+0.12 m the hop runs and the hero stands.

## Other measurements
- Stop trace now works (156 lines): during the working stops up_dot_n 0.99993-1.0 in every frame, in-plane speed falls
  smoothly to 0 by ~0.8 s, release_blocked holds; rotation writes 0 / 40 with max deviation 0.5-1.0 deg (no 90 deg fight
  this time).
