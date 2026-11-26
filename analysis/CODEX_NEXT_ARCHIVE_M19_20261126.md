# Next task — M19: keep the game's ground sprint on the face (no WR start while standing on the face)

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text (M18c, superseded, not to be
implemented): `analysis/CODEX_NEXT_ARCHIVE_M18C_ONHOLD_20261113.md`. Read first:
`analysis/WS_GROUND_SPRINT_RESULT_20261113.md`, `analysis/WS_GROUND_SPRINT_DIRECTION_20261113.md`.

Owner goal for the stop (2026-11-13): stopping from WS must look like stopping a ground RT sprint (reference video
described in the result file), not crawl -> hop -> stand. Owner must-haves (CLAUDE.md) stay.
Terms: WS = Wall Sprint, WR = vanilla Wall Run. One problem at a time: this task changes only the WR start while standing
on the face. Do not change jumps, zips, falls, camera, mapping or the stop hop.

Fact (VERIFIED-RUNTIME): in the face-as-ground state the game's own sprint ran sideways at ~10 m/s with step_n 0 and
plane_d 0 and stopped naturally in ~1 s; sprinting up the face started a vanilla WR (WallRunUp) at ~12 m/s, which became a
new WS.

All code changes in WallSprint-owned code (`source/src/NativeHooks/camera_ref.cpp` and the existing wallrun hook code).
Keep exact-build guards, F8 kill, INI switches, code defaults OFF (shipped INI enables like the existing switches). One
commit per step, push after each.

## Step 1 — record how the WR start happens on the face (read-only)
While the hero stands on the face in a hold (pinned, not face air, not stop hop), log every WR entry/init the existing
wallrun hooks see: `WS_FACE_WR_START t= speed= move_dir_up= move_dir_side= stick= rt=(if readable) hook=entry|init
caller_rva=` (<= 40). This tells which hook call starts it.

## Step 2 — refuse that WR start while standing on the face (bounded, reversible)
Only if the existing hook code already has a proven way to make the WR start not happen (an existing, evidenced refusal
/ skip path of the hooked function; check `docs/` and `analysis/` for the hook's ABI notes): use it while the hero stands
on the face in the hold. INI `WALLSPRINT_WS_FACE_NO_WR`, code default 0, shipped 1, F8 stops it, counter + cap 2000 per
hold, `WS_FACE_WR_REFUSED` log (<= 40). If no evidenced refusal path exists, do not invent one: stop after step 1 and
report the call site and what evidence is missing.
Effect expected: RT on the face in any direction keeps the game's ground sprint; releasing RT and the stick gives the
game's own sprint stop.

## Step 3 — sprint trace
While standing on the face, every 5th frame when the game's in-plane speed > 3 m/s or within 1.5 s after it falls below
3 m/s: `WS_FACE_SPRINT_TRACE speed= step_up= step_side= step_n= plane_d= clamps=` (<= 300). Shows that sprint and stop are
the game's own (step_n 0, no clamps).

## Order and reporting
Steps 1-3 (one commit each, push after each). Cloud verification as before (build, test.sh, smoke_windows,
test_hooks_windows with exact flaky reruns, package, `docs/CLOUD_VERIFIED.txt`, `docs/CODEX_RESULT.md`, analysis file).
Finish with STATUS / CHANGED / RESULT / NEED; NEED: `Update-WS.cmd` -> Install mods -> Run modded game; get the hero
standing on a face (WS, stop, A jump if it crawls); then hold RT with the stick up the face 3 s, release both, wait 2 s;
the same down the face and sideways; one short video; return the newest `logs\MSM2-WS-v0.1c48-<timestamp>.log`.
