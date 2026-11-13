# ON HOLD (owner test first) — do not implement M18c until the owner test in
# `analysis/WS_GROUND_SPRINT_DIRECTION_20261113.md` is analysed. The next action is the owner's, not Codex's.

# Next task — M18c: the stop hop must also start when the hero is already on the face (P1 only)

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_M18B_20261113.md`. Read first: `analysis/M18B_OWNER_RESULT_20261113.md`.

Owner decision: one problem at a time; this task is P1 (stop) only. Do not change jumps, zips, falls, camera or mapping.
Owner must-haves (CLAUDE.md) stay. Terms: WS = Wall Sprint, WR = vanilla Wall Run.
Accepted, keep: M18b debounce, carry/deceleration, release blocking, rotation hold, per-hold stop trace.

All code is in `source/src/NativeHooks/camera_ref.cpp`. Keep exact-build guards, F8 kill, INI switches, code defaults
OFF. One commit per step, push after each.

## Step 1 — no silent refusal of the stop hop
Fact: 2 of 4 stops launched no hop; in both the hero was at plane_d -0.00 at hold start (the game had pulled it onto the
face during the 0.219 s debounce) and `StopHopArc::solve` rejects distance < 0; the hero stayed in the crawl.
Do: in the stop-hop launch, clamp the start distance: d0 = max(0, distance) when distance >= -0.30 m (a hero up to 0.30 m
inside the plane starts the hop from the plane); refuse only for non-finite values, distance < -0.30 or >= 0.8, or a jump
press within 600 ms. Every refusal logs `WS_GRAVITY_FORCE_STOP_HOP event=refused reason= distance=` (never silent).
Add a unit test for distance -0.003, 0, 0.11 and -0.31.

## Step 2 — stop trace starts with the hold even without a hop
Fact: the refused stops logged only `WS_STOP_TRACE event=unavailable reason=episode_not_started`.
Do: start the stop trace episode at the debounced WS end (hold start), independent of the hop, so a refused or missing hop
still shows plane_d / game rows for 1.5 s.

Acceptance: every stop logs a STOP_HOP takeoff (or a refusal with reason); owner: the hero stands after every stop
without jumping.

## Order and reporting
Steps 1-2 (one commit each, push after each). Cloud verification as before (build, test.sh, smoke_windows,
test_hooks_windows with exact flaky reruns, package, `docs/CLOUD_VERIFIED.txt`, `docs/CODEX_RESULT.md`, analysis file).
Finish with STATUS / CHANGED / RESULT / NEED; NEED: `Update-WS.cmd` -> Install mods -> Run modded game; five times: start
a WS (RT), run 2 s, release RT and the stick, wait 3 s, B; one short video of the five stops; return the newest
`logs\MSM2-WS-v0.1c48-<timestamp>.log`.
