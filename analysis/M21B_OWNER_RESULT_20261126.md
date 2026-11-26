# M21 owner result and M21b fixes (2026-11-26, build f69964e, log MSM2-WS-v0.1c48-20261126-143324)

Inspected in full: the log (10966 lines) and all 7 videos (2-success 6.9 s, 2-angled 4.4 s, 2-camera_filed 8.6 s,
2-failed 6.4 s, 2-filed-2 8.6 s, 4 4.3 s, 5-failed 7.2 s; frames every 0.25 s over the whole duration).

## Owner report
- Tests 2/3 (ground S into a wall, stop on the face): basically success. Slight jerk at the switch into WS; shallow-angle
  approaches do not enter (or rarely); some cases go into WR, float off the wall or get a wrong angle.
- Test 4 (entry from the air, swing/zip into a wall): success (video 4: lands standing on the face, walks on).
- Test 5 (B within 0.5 s of the entry): often not released (video 5-failed: the hero is back on the wall at once).

## Evidence (VERIFIED-RUNTIME)
- 25 entries: every `WS_GROUND_ENTRY request` led to `start`/`hold`/`hold_no_hop` after ~0.33 s, and `WALL_LANDED` followed
  0.10-0.21 s later. No `event=refused`; no WR request at all came for the shallow approaches (the game requests no WR
  there), so the entry cannot start from this path. UNKNOWN: an own wall probe would be needed for shallow entries.
- B not released: after `cancel_by_B` (or `entry_cancel_by_B`) the hero, still pushing stick/RT into the wall, got a
  vanilla WR within 0.4-1.0 s (the entry callback is not armed while the 1 s B return runs, and the face veto needs a
  pinned hold), and the old WS-from-WR path started a new hold with the stop hop: tests 9, 20, 21->22, 23, 24, 25
  (e.g. B 05:38:38.429, `start test=20` + STOP_HOP 05:38:39.405).
- WR/float/wrong angle after entry: tests 4, 7, 13, 17. 0.2-0.5 s after the no-hop hold started, the walk-off detector
  released the hero (`WS_FACE_FALL release reason=world_down_window_acceleration`, window speed -3 -> +3 m/s while the
  sprint along the face toward the ground built up), the face state became `released`, the face veto did not apply and
  the game's WR started (`WS_FACE_WR_START state=released since_release_s=0.000-0.125`). The old path then restarted the
  hold (`ws_again`, stop hop, test 4 with a 90 deg different normal at a corner, test 7 normal 0.975,0,0.224 vs 1,0,0.027).
  In 13/17 the release ended after 26 ms (`game_fall_ended`), i.e. a false release.

## M21b (Claude Code)
1. B block: B (hold or entry rotation) refuses every hero WR request and ignores WS detection for 1.5 s
   (`WS_B_CANCEL event=block_start|block_end wr_vetoed=N`).
2. Same-face refusal: during a hold that is not pinned (fall release, face air, stop hop) a WR request whose context normal
   is within 20 deg of the hold normal and whose contact point is within 0.6 m of the hold plane is refused (temporary
   empty-slot callback, restored). Other faces keep the game's decision (`WS_FACE_WR_SAME_FACE decision=refused|game`,
   hold_end `same_face=N`).
3. The walk-off/fall detectors ignore the first 0.5 s after a no-hop entry (`hold_no_hop ... fall_ignore_s=0.5`).
- Not changed: shallow-angle entry (needs its own contact source), entry onto another face during a hold (still the old WR
  path), camera follow in falls (P4).
- Checks: build PASS, test.sh PASS (87), package PASS (script.disabled sha256
  8231733bad90d8e6d6aa485ceed4261bea5e6813c71d0aac328caac985aa67ae, DLL
  1074aac509abceddbae93e640e8051789bcd623f41bc39ac310224bea89ef210); smoke/hooks UNAVAILABLE (wine).
