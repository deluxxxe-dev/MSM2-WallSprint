# Next task — M18b: stop only on a real WS end, one hop at a time, working stop trace (P1 only)

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_M18_20261113.md`. Read first: `analysis/M18_OWNER_RESULT_20261113.md`, then
`analysis/M17_OWNER_RESULT_20261112.md` section 1.

Owner decision: one problem at a time; this task is P1 (stop jerk) only. Do not change jumps, zips, falls, camera or
mapping code. Owner must-haves (CLAUDE.md) stay. Terms: WS = Wall Sprint, WR = vanilla Wall Run.
Accepted, keep: M18 steps 1 (release blocked after the stop) and 3 (in-plane deceleration, slide 0.8-0.9 m); stands
after the stop (apex 0.8 m, landing ~0.355 s, crawl_suspect=0).

All code is in `source/src/NativeHooks/camera_ref.cpp`. Keep exact-build guards, F8 kill, INI switches, code defaults
OFF (shipped INI enables like the existing switches). One commit per step, push after each.

## Step 1 — the stop starts only on a real WS end
Fact: 29 WS ends in 50 s inside one WR session; each started a hold and a stop hop, new WS starts cut hops, airtimes
0.63-1.52 s, carry_valid=0, long air pose in the video (analysis section 1).
Do: start the stop hop only when WS has stayed inactive for 0.15 s (keep the WS-end state and velocity of that moment for
the carry). If WS becomes active again before that, nothing happens (no hold_start, no hop). While a hop or its 0.4 s
rotation window runs, a new WS start ends it cleanly first (rows released immediately, no blend), and no second hop can
start until it has ended. Log `WS_STOP_TRACE event=debounce kept|cancelled ws_off_s=` per WS end (<= 60).

## Step 2 — working per-frame stop trace with the game's own rows
Fact: 0 `WS_STOP_TRACE t=` lines; the 3-episode budget is global and was consumed by session 1, and even those episodes
logged nothing (analysis section 2).
Do: budget per hold: the first 3 stops of every hold, 1.5 s each, every frame (not every 2nd), <= 100 lines per stop.
Fields as M18 step 4 plus `game_up_dot_n` / `game_fwd_up` / `game_fwd_side` = the rows read BEFORE WallSprint's write in
that call, and `written=0|1`. When a sample cannot be taken, log one line per stop with the reason (read failed /
no native call / episode not started).

## Step 3 — rotation hold only where the game deviates, and blend toward the face
Fact: `max_up_dev_deg=90` in every clean stop; the game's rows were turned to world up during the window and were
overwritten up to 29 times; the blend target after 0.4 s is the game's rows (analysis section 2).
Do: keep up = n during the whole window. Blend at the end only the forward (within the face plane) from the held forward
to the game's forward projected on the face; never blend the up toward the game's up. If the game's up still deviates
> 10 deg from n at the end of the window, extend the window in 0.1 s steps up to 1.0 s total and log `extended_s`.

Acceptance (log): one hop per real WS end, airtime ~0.355 s, carry_valid=1, trace lines present for each stop with
up_dot_n >= 0.99 in every written frame. Owner: no jerk at the stop.

## Order and reporting
Steps 1-3 (one commit each, push after each). Cloud verification as before (build, test.sh, smoke_windows,
test_hooks_windows with exact flaky reruns, package, `docs/CLOUD_VERIFIED.txt`, `docs/CODEX_RESULT.md`, analysis file).
Finish with STATUS / CHANGED / RESULT / NEED; NEED: `Update-WS.cmd` -> Install mods -> Run modded game; three times:
start a WS (RT), run 2 s, release RT and the stick, wait 3 s, B; one short video of the three stops; return the newest
`logs\MSM2-WS-v0.1c48-<timestamp>.log`.
