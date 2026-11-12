# M18 owner result (2026-11-13, build 71f8c03, log MSM2-WS-v0.1c48-20261113-013713)

Inspected in full: the log (21631 lines) and the video WS_Stop.mp4 (11.2 s, 6 frames/s over the whole duration).
Owner: "still not smooth". Scope: P1 (stop jerk) only.

## Video
- 0-2.2 s: the hero on a glass face, short motion, crouch; 2.3-3.0 s black frames (recording); 3.2-4.8 s the hero moves
  along the face, hops with arms spread; 5.0-9.5 s the hero stays in the arms-spread air pose almost without motion;
  9.6-11 s moves again and ends standing/crouching. The long air pose matches hold session 1 below.

## 1. Session 1 (16:40:57-16:41:47): WS end flickers, overlapping stop hops (VERIFIED-RUNTIME)
- 29 `ws_end` in 50 s while one vanilla WR session (session_timeline id=1, 50.0 s) stayed active. Every WS end started a
  new hold (`hold_start`) and a stop hop; new WS starts interrupted hops (`STOP_TRACE event=rotation reason=new_ws`).
- Hop airtimes 0.63 / 1.52 / 1.32 s instead of 0.355 s and `carry_valid=0` in every hop of this session: the hop model
  did not run every frame while the WS/WR state changed. This is the long air pose in the video.
- Cause (VERIFIED-RUNTIME): the stop starts on every WS end, even when WS resumes within 0.2-1.5 s and the WR session never
  ended.

## 2. Sessions 3-5 (clean single stops 16:42:42 / 16:42:58 / 16:43:09)
- Step 3 works: v_plane0 4.65 / 4.78 / 5.30 m/s, slide 0.81 / 0.84 / 0.93 m to the landing at 0.352-0.362 s.
- Step 1 works: release_blocked 135 / 136 / 150 frames, no walk-off release, crawl_suspect=0 (stands).
- Step 2: `rotation writes=21-29 max_up_dev_deg=90.000 max_fwd_dev_deg=90.000`: during the hold window the game's own rows
  were turned 90 deg away from ours (up row = world up) and WallSprint overwrote them up to 29 times. Whether the rendered
  pose showed the game's rows on some frames, and what the 0.2 s blend blended toward, is not measured.
- Step 4 trace (`WS_STOP_TRACE t=`) is missing: 0 lines. The budget allows only episodes 1-3, and session 1 consumed them;
  even those produced no line (the native transform read or the episode start condition failed). Bug.

## Next (M18b)
1. Start the stop only when WS stays ended (debounce) and never start a second hop while one runs.
2. Make the per-frame trace work (episode count per hold, not global; log a reason when a sample is skipped) and include
   the game's own rows before WallSprint's write, so the rotation fight and the blend target are visible.
