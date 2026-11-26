# M21i owner result and M21j (2026-11-26, build 617ee2c, log MSM2-WS-v0.1c48-20261126-185442)

Inspected in full: the log (15486 lines) and all 3 videos (2-Failed 12.4 s, 3-Failed-RT 8.5 s, 3-Failed-noRT 7.5 s;
frames every 0.25 s over the whole duration).

## Owner report
- WR -> WS: still a short hop then crawl, and in the crawl the controls are not accepted.
- With RT the edge gives the parkour sprint jump or a web swing: not wanted while LB is held. Without RT (LB + stick) the
  wrap happens, but the hero flies out and turns in steps, not smoothly.

## Evidence (VERIFIED-RUNTIME)
- The MOD callback can refuse states other than WR: 80 `WS_FACE_STATE_REFUSED rva=0xb463c10` (crawl idle).
- WR -> WS (09:56:39-47): WR -> 0xB465AB0 (WR exit) at 40.529; the crawl idle request was refused continuously, the game
  stayed in 0xB465AB0 (prone pose of video 2-Failed) for 6 s until B; the hold started at 41.000 without the stop hop.
  The open-ended refusal caused the unresponsive state.
- RT at the edge: the game asked for its sprint jump 0xB454950 before the MOD walk-off release (31.851 -> 31.937,
  40.320 -> 40.373); air 0xB45D6B0 followed 1.8 s later. No refusal fired because the wrap window had not started yet.
- Without RT: no game state change at the edge; wraps came 0.24-0.26 s after the walk-off release (10 requests), then
  the new hold started 1-1.7 m off the guessed face and the hero fell onto it (`land reason=contact` 1-4.5 m).
- No `WS_FACE_RELEVEL`: no same-face context 1.5-20 deg off occurred.

## M21j (Claude Code)
- With LB in a hold, the game's jump 0xB454950, air 0xB45D6B0 or ledge 0xB45E2B0 request starts the wrap at once from the
  last pinned edge position and is refused; the sprint jump is refused while LB is held in a hold.
- WR exit 0xB465AB0 while WS is armed starts the hold immediately (`force_hold reason=wr_exit`, the stop hop runs) and the
  crawl states are refused only for 0.5 s after it (no permanent refusal).
- LB walk-off wrap without the 0.2 s probe: 0.05 s after the release and 0.08 m behind the face plane.
- Checks: build PASS, test.sh PASS (95), package PASS (script.disabled sha256
  ca87a7728cd9abdbab2d638f9281350d152a4d81e2faae172ec96d4739b531e6, DLL
  2d1ed6bde5d0b88dcecc72ff8ad13c90354de0ef6becea7d50f3438e39ba28c0); smoke/hooks UNAVAILABLE (wine).
