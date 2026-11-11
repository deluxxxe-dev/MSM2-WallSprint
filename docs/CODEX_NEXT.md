# Next task — M12: face jump/zip direction from the game's own face walking; stop must end on the wall

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_M11_20261111.md`. Read first: `analysis/M11_OWNER_RESULT_20261111.md` (evidence for
everything below), then `analysis/M11_FACE_CONTROL_20261111.md`, `analysis/M10Z_R19_RESULT_20261111.md` and the M10z
sections at the end of `analysis/M8_JUMP_PLAN_20261106.md`.

Owner goal (minimum requirement of the mod): on a wall the hero moves exactly like on the ground, no shaking or jerks.
Terms: WS = Wall Sprint (this mod), WR = vanilla Wall Run. M11 task 2 (walking down the face) is accepted: do not change it.

All code is in `source/src/NativeHooks/camera_ref.cpp` (`gfStep`, `faceScreenAxes`, `faceAirDir`, `sp2Detour`,
`wgiReadDetour`, `m10GravityTest`). Keep exact-build guards, F8 kill, INI switches, code defaults OFF. One commit per task.

## Task 1 — stick direction on the face from the game's own walking (face jump, air steering, web zip)
Fact: the camera manager matrix/position (`ws_camref_this` +0xD24..+0xD54) is stale while the hero stands on the face
(analysis section 2). Remove it from `faceScreenAxes` / `faceAirDir` / the zip. The only verified reference is the
game's own face walking, which follows the stick correctly.
Do:
1. Face basis: `e_up` = world up projected on the face (normalised), `e_side = cross(n, e_up)`.
2. Walking samples: while pinned, not in a face jump, raw stick magnitude > 0.5 and the game's in-plane step speed
   > 1 m/s (step since the previous frame, per frame not per SetPosition call): stick angle `a_s = atan2(sx, sy)`,
   step angle `a_w = atan2(dot(step, e_side), dot(step, e_up))`.
3. Model: direction angle = `h * a_s + theta`, h = +-1 (mirror), theta = rotation. Keep a ring of the last 64 samples
   (age <= 20 s). Fit both h = +1 and h = -1 (theta = circular mean of `a_w - h * a_s`), choose the h with the smaller
   circular variance when the samples span >= 60 degrees of stick angle; otherwise keep the previous h (initial +1).
   theta is always the circular mean of the samples of the last 2 s (so it follows camera turns while walking).
4. Use:
   - Face jump takeoff: if a walking sample is younger than 0.3 s, in-plane velocity = the game's current walking
     velocity (direction and speed as measured); else if the model is younger than 20 s, 5 m/s along the model
     direction of the stick; else zero (straight jump).
   - Air steering and web zip impulse: model direction of the current stick (zip with a neutral stick: current in-plane
     velocity direction, else no impulse).
5. Log once per face jump and per zip: stick, a_s, h, theta (deg), sample count/age, source (walk / model / none) and the
   resulting direction in (e_up, e_side) components. Remove the camPos fields.
Acceptance (owner videos with captions up/down/left/right): walking in a direction and jumping continues in that
direction; standing jumps and web zips with the stick held go where the same stick direction walks.
Known limit (record it, do not solve here): if the camera turns while the hero is airborne or standing still, the
model keeps the last walking mapping.

## Task 2 — stop from WS must end at the wall
Fact: failures rest pinned 0.56 m in front of the wall-point plane with zero model velocity (analysis section 3).
Do:
1. Log the plane point `gfP0` (and its source) at hold start, at `WALL_LANDED`, and whenever it changes by > 0.05 m.
2. Do not move `gfP0` while a hold is in the air phase (between hold start and landing); take it once at hold start.
3. Pinned but off the wall: if pinned, not in a face jump, and the distance to the current plane is > 0.15 m for
   > 0.1 s, clear the pin and continue the wall-gravity fall toward the plane (landing rule unchanged), instead of
   holding the hero in the air. Log `WS_GRAVITY_FORCE_REAPPROACH d=`.
4. Limit the stop push to the requested 0.9 m (currently peaks at 2.4-2.7 m): clamp the pushed distance.
Acceptance: `WS -> stop` three times in one run: each ends with rest plane_d within +-0.05 m, `pin_drops_undone` near 0
and the standing pose (owner video).
The SETPOS=0 comparison of M11 task 3 is no longer needed.

## Not in scope
Finding the live camera, body yaw during jumps, the camera swing and web line of the zip, charge-jump height,
velocity search, mover editing, L3 lock.

## Order and reporting
Task 2 first (smaller), then Task 1; push after each. Cloud verification and handoff as in the archived M11 text
("Verification and handoff"): build, test.sh, smoke_windows, test_hooks_windows (record flaky reruns exactly),
package, `docs/CLOUD_VERIFIED.txt`, `docs/CODEX_RESULT.md`, analysis file. Finish with STATUS / CHANGED / RESULT / NEED;
NEED lists the owner test: `Update-WS.cmd` -> Install mods -> Run modded game; ~20 s idle in freeroam; WS -> stop x3;
walk on the face in four directions for a few seconds each (this also trains the mapping); walking jumps and standing
jumps up/down/left/right x2; web zips up/down/left/right; return the newest `logs\MSM2-WS-v0.1c48-<timestamp>.log` and
captioned videos.
