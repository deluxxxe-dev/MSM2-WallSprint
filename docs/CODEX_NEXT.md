# Next task — M18: stop from WS without position/rotation jerk (P1 only)

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_M17_20261112.md`. Read first: `analysis/M17_OWNER_RESULT_20261112.md` section 1 (and the
problem list P1-P6; only P1 is in this task).

Owner decision (2026-11-12): fix and test one problem at a time. This task is P1 only. Do not change jumps, zips,
falls/walk-off detection (except the stop window in step 1), camera or mapping code.
Owner must-haves (CLAUDE.md): (1) seamless motion, no jerks; (2) changed gravity kept in the air with a fall along it.
Terms: WS = Wall Sprint, WR = vanilla Wall Run.
Accepted, do not regress: the stop ends standing (apex 0.8 m, landing 0.355 s, crawl_suspect=0).

All code is in `source/src/NativeHooks/camera_ref.cpp`. Keep exact-build guards, F8 kill, INI switches, code defaults
OFF (shipped INI enables like the existing switches). One commit per step, push after each.

## Step 1 — no fall release during and after the stop (motion d)
Fact: 2 of 3 stops got a walk-off release 0.70 s after the hop landing (after the 0.5 s guard), a 0.1 m probe into the face
and a `contact` landing: a second jerk.
Do: from the WS end until 1.5 s after the stop-hop landing, and afterwards as long as the stick stays neutral (< 0.2) and
no A was pressed since the stop, the walk-off rule is disabled; the existing fall guard keeps undoing the game's world-down
steps. Log `WS_STOP_TRACE event=release_blocked` once per stop with the count of blocked frames.

## Step 2 — keep the body rotation through the landing and blend back (motion c)
Fact: after the landing the game swings the hero up toward world up for ~0.2 s ((0.76,0.64,0.08) -> (0,1,0) -> n) and turns
the forward to the face-horizontal axis; the facing writes stop at the landing.
Do: extend the existing face-air rotation write (same row layout/handedness/identity checks as M16 task 3) for the stop
hop through landing + 0.40 s: up = n always; forward = the hero forward at the hop takeoff projected on the face. Then
blend from these rows to the game's own rows over 0.20 s (normalised linear blend of forward with up = n, then
re-orthogonalise), and stop writing. Cap and F8 as before. Log per stop `WS_STOP_TRACE event=rotation writes= max_up_dev_deg=
max_fwd_dev_deg= blend_s=0.2`.

## Step 3 — decelerate instead of stopping in one frame (motion a)
Fact: the hop starts with zero in-plane velocity while the hero still moves 4.3-5.0 m/s along the face.
Do: the stop hop keeps the in-plane velocity of the last frame before the hop and decelerates it linearly to 0 at the hop
landing (0.355 s). Log `v_plane0` in `STOP_HOP event=takeoff` and the in-plane distance at landing.

## Step 4 — per-stop trace for the owner run
For 1.5 s from the WS end, every 2nd frame (<= 3 stops x 60 lines): `WS_STOP_TRACE t= plane_d= v_plane= up_dot_n= fwd_up=
fwd_side= game_step_n= game_step_plane= writes= release_blocked=`.

Acceptance: the owner sees no position or rotation jerk at the stop and the hero still stands; the trace shows up_dot_n
>= 0.99 throughout, no release, and in-plane speed falling smoothly to 0.
Decision table for the next run: remaining jerk at the start -> hop shape (b); at the landing -> rotation (c);
~0.7 s later -> release (d) not blocked; report which, do not change b in this task.

## Order and reporting
Steps 1-4 (one commit each, push after each). Cloud verification as before (build, test.sh, smoke_windows,
test_hooks_windows with exact flaky reruns, package, `docs/CLOUD_VERIFIED.txt`, `docs/CODEX_RESULT.md`, analysis file).
Finish with STATUS / CHANGED / RESULT / NEED; NEED: `Update-WS.cmd` -> Install mods -> Run modded game; WS -> stop x3
(wait 3 s after each; one short video covering the three stops); return the newest `logs\MSM2-WS-v0.1c48-<timestamp>.log`.
