# M20 owner result and M20b (2026-11-26, build 413d236, log MSM2-WS-v0.1c48-20261126-122944)

Inspected in full: the log (7365 lines) and all four videos over their whole duration (4 frames/s): up.mp4 (4.4 s),
down.mp4 (4.5 s), side.mp4 (3.9 s), fall.mp4 (8.1 s). Terms: WS = Wall Sprint, WR = vanilla Wall Run.

## Owner report
- down: natural now, with a small snag. up: moves, but body orientation/height leave the wall, like the old WR behaviour.
- fall: the camera does not follow (P4, not this phase).

## Down (VERIFIED-RUNTIME)
- `WS_FACE_LOCOMOTION_DOWN start` at 03:31:52.413 (1.4 m/s, dir_cos 0.994); game sprint steps 0.134-0.163 m/frame.
- 03:31:53.911 `end speed=13.429` and one `FALL_UNDO speed=16.363 undo_y=0.162` at 53.923, restart 11 ms later: single
  frames reach 13-16 m/s inside the ~10 m/s sprint (frame-time jitter); the per-frame 11.5 m/s limit cut one frame -> the
  snag. No release, no WR during the down sprints; natural stop (video).

## Up (VERIFIED-RUNTIME): the game starts a WR while the hero stands on the face
- 03:32:00.620 `WS_FACE_WR_START` hook=init caller_rva=0x367e6af and hook=entry caller_rva=0x367e6ce, state=pinned,
  speed 1.72 m/s, move_dir_up 0.9999, stick up, RT 1.0, since_release -1 (no release involved). WR session 2 (WallRunUp)
  followed: the video shows the WR posture (body off the face, hopping/leaning) = the owner's "like before the posture
  fix". On 2026-11-26 11:44 the same up sprint did not start a WR: the start is intermittent at the sprint start.
- This is the M19 problem (WR start on the face) with the call sites now measured.

## Fall
- 03:32:23.468 walk-off release (window 0.52 -> 3.10 m/s), fell 4.07 s to plane_d -108.6 m until B. Camera did not follow
  (P4, later phase).

## M20b changes (implemented by Claude Code)
- Locomotion-down limit uses the world-down speed over 0.1 s (11.5 m/s); the fall guard tolerates single frames up to
  20 m/s while exempt; coasting after stick release continues while the 0.1 s speed does not rise by more than 1 m/s
  against the previous 0.1 s (a real fall gains ~2.6 m/s per 0.1 s), at most 1 s.
- Read-only one-time capture at load: `WS_WR_CALLER_DUMP` (pdata bounds of the function containing RVA 0x367E6AF) and
  `WS_WR_CALLER_BYTES` (code from up to 0xC00 before to 0x100 after the call site), for offline analysis of the WR start
  decision (next step: find an evidenced condition to keep the ground sprint when the hero stands on the face).
- Checks: build PASS (prebuilt capstone, proxy refused the download), test.sh PASS (84), package PASS
  (script.disabled sha256 23da2db7a9bb37b5e7f462083993b98a5a93bd06641005215ff7f5bf34f98734, DLL
  1a369c7087ee7072318dc470bdd739210ca552f6a3a26e3c051cf8aeb8718923); smoke/hooks UNAVAILABLE (wine aborts here).
  Source tree 0db5961a46a6b45e39010272eb567953ad9ab975.
