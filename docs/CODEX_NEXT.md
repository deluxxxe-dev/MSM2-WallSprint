# DONE (2026-11-26, implemented by Claude Code in 413d236) — next action is the owner's test, not Codex.
# Result and owner test: `analysis/M20_LOCOMOTION_DOWN_20261126.md`. Do not re-implement M20.

# Next task — M20: the game's sprint toward the ground must not be treated as a fall

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_M19_20261126.md`. Read first: `analysis/M19_OWNER_RESULT_20261126.md`, then
`analysis/WS_GROUND_SPRINT_RESULT_20261113.md`.

Owner goal: on the face the hero runs and stops like on the ground (the game's own RT sprint and stop), in every direction.
Owner must-haves (CLAUDE.md) stay: a genuine fall off a face must still fall along the changed gravity.
Terms: WS = Wall Sprint, WR = vanilla Wall Run. One problem at a time: only the fall guard and walk-off detection
while standing on the face. Do not change jumps, zips, camera, mapping, the stop hop or WR hooks (M19 step 2 stays not
implemented).

Fact (VERIFIED-RUNTIME): sprint up and sideways on the face: ~10 m/s and a natural stop with no WallSprint correction.
Sprint toward the ground: the fall guard undid steps at 9.38 m/s (`speed_over_9`), and the walk-off rule released at the
sprint start (window 4.7 -> 6.3 m/s and 2.1 -> 4.3 m/s); a vanilla WR started ~6-20 ms after each release.

All code is in `source/src/NativeHooks/camera_ref.cpp`. Keep exact-build guards, F8 kill, INI switches, code defaults
OFF. One commit per step, push after each.

## Step 1 — locomotion exemption for world-down motion
Define "locomotion down" while pinned on the face (not face air, not stop hop): the stick is pushed (> 0.5) and the game's
in-plane movement direction over the last 0.3 s (frames with speed > 1 m/s) points within 45 deg of the world-down
direction projected on the face, and the world-down speed is <= 11.5 m/s (sprint top ~10.2 m/s measured).
While "locomotion down" holds: the fall guard does not undo, and the walk-off rule does not release.
A genuine fall is still caught: world-down speed > 11.5 m/s, or world-down acceleration while the 0.3 s movement direction
was NOT within 45 deg of world down (walking off a side or top edge), or the stick is neutral. Log each exemption start/end
`WS_FACE_LOCOMOTION_DOWN event=start|end speed= dir_cos= stick=` (<= 60).

## Step 2 — record every WR start during a hold
`WS_FACE_WR_START` records also when the hero is not pinned (just released, face air, stop hop), with a `state=` field
(pinned / released / face_air / stop_hop / other) and the time since the last release. Keep it read-only.

Acceptance (log + owner): sprint toward the ground with RT runs at ~10 m/s and stops naturally, with no
`WS_FACE_FALL event=release`, no `FALL_UNDO`, no WR session during it; up and sideways unchanged; walking off a side edge
of a face still releases and falls.

## Order and reporting
Steps 1-2 (one commit each, push after each). Cloud verification as before (build, test.sh, smoke_windows,
test_hooks_windows with exact flaky reruns, package, `docs/CLOUD_VERIFIED.txt`, `docs/CODEX_RESULT.md`, analysis file).
Finish with STATUS / CHANGED / RESULT / NEED; NEED: `Update-WS.cmd` -> Install mods -> Run modded game; get the hero
standing on a face (WS, stop, A jump if it crawls); RT sprint toward the ground 3 s, release, wait 2 s; the same toward the
roof and sideways; then walk off the side edge of the face and fall 2 s, B; one short video per direction; return the
newest `logs\MSM2-WS-v0.1c48-<timestamp>.log`.
