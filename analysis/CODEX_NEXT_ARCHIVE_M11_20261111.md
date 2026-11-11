# Next task — M11: face-as-ground control fixes (direction mapping, walking down, stop landing)

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_20261111.md`. Read first: `analysis/M10Z_R19_RESULT_20261111.md` (evidence for every item
below) and the M10z sections at the end of `analysis/M8_JUMP_PLAN_20261106.md`. Shared facts:
`MSM2-Wisdoms/docs/WALLSPRINT_INTERNALS.md` ("Wall gravity hold findings", "Hero position writer and character mover").

Owner goal (minimum requirement of the mod): on a wall the hero moves exactly like on the ground, with no shaking or
jerks. WS face stays the ground until a new WS or B (Xbox B). Terms: WS = Wall Sprint (this mod), WR = vanilla Wall Run.

All code is in `source/src/NativeHooks/camera_ref.cpp` (force model `gfStep`, SetPosition hook `sp2Detour`, WGI input
hook `wgiReadDetour`, worker `m10GravityTest`). Keep exact-build guards (pdata + sha checks), F8 kill, INI switches and
default-OFF code defaults. One hypothesis -> one change -> one owner run; each task below is its own commit.

## Task 1 — screen axes on the face (fixes face jump + web zip direction)
Problem: `faceScreenAxes()` is 90 degrees off in the face-as-ground state (analysis section 1).
Do:
1. Stop using camera matrix rows. Compute the screen-forward direction on the face from positions:
   `p = normalize(proj_face(heroPos - camPos))`, camPos = camera manager `ws_camref_this` + 0xD54 (position row used by
   the M6g camera follow in `actor_writer.cpp`), heroPos = Transform+0x30. If |proj| < 0.2 keep the last valid p.
2. `screen_right = cross(p, n) * s`, s = +-1. Determine s at runtime instead of assuming it: while the hero walks on the
   face (pinned, not faceAir) the game's own walking is correct, so compare the game's in-plane step direction with the
   stick: accumulate sign(dot(step, cross(p, n)) * stick_x) over samples with |stick_x| > 0.7; use the majority once
   >= 20 samples agree >= 80 %; until then s = `ws_camera_handedness`. Log the decision once.
3. Keep `WS_FACE_JUMP_INPUT` and add camPos, heroPos and s to it.
Acceptance (owner video, camera looking down the face): stick up -> toward the top of the screen, down -> bottom,
left/right -> left/right, for face jumps and web zips, also after the camera has turned.

## Task 2 — walking toward the ground on the face
Problem: all downward game steps are undone (analysis section 2).
Do: replace the per-call rules with a per-frame budget. Group SetPosition calls into frames (gap >= 2 ms starts a
frame). Per frame allow a total downward world-y change of `6 m/s * frame_dt` when the stick is pushed (> 0.25) and the
last landing is older than 0.3 s; otherwise 0. Apply the budget across all calls of the frame (first and later calls
alike). Keep "no store when nothing is corrected".
Acceptance: `WS_FACE_MOVE_SAMPLE` shows downward steps passing (store_dy = 0) while the stick points down the face;
no slide when the stick is neutral; owner can walk down, up and sideways at the same speed.

## Task 3 — stop from WS must stand on the face
Problem: after the stop push the game stays in its falling state (analysis section 3). Cause unknown; decide by test.
Step A (no code): ask the owner for one run with `WALLSPRINT_WS_GRAVITY_SETPOS=0` in `WallSprint.ini` (thread path as
in M10z-r6..r14): stop from WS 3 times. Decision table:
| Result | Meaning | Next |
|---|---|---|
| stands with SETPOS=0 | the SetPosition path prevents the game's grounding | in the SetPosition path, after `WALL_LANDED` pass the game's value through unchanged (no plane clamp, no drop undo) until the game's own step is zero for 5 frames or 0.5 s passed |
| still falling | not the path | log the game's steps (normal / wall-up / side) for 1 s after `WALL_LANDED` and test landing with a real approach: keep gravity toward the face until the game's step along -n is blocked for 3 frames instead of pinning at 0.05 m |
Do not add an A injection (rejected in M10z-r5: the game pulled the hero into the crawl).

## Order and reporting
Implement Task 1 and Task 2 first (one commit each) and push. Task 3 Step A is an owner test: put its steps in NEED.
Finish with STATUS / CHANGED / RESULT / NEED (CLAUDE.md "Finish response").

## Not in scope
Body yaw during jumps, the camera swing after a web zip, the web line visual, charge-jump height, velocity search,
mover editing (`WALLSPRINT_WS_GRAVITY_MOVER` stays 0), L3 lock (cancelled by the owner).

## Verification and handoff
Cloud: `tools/build_windows.sh`, `tools/test.sh`, `tools/smoke_windows.sh`, `tools/test_hooks_windows.sh`
(`native_source_override_smoke` and `orientation_trace_smoke` are timing-flaky in fresh containers; rerun and record
exactly what happened), `tools/package.py`; append the source tree to `docs/CLOUD_VERIFIED.txt`; update
`docs/CODEX_RESULT.md` and the analysis file; push to the branch. Owner test: `Update-WS.cmd` -> Install mods -> Run
modded game; wait ~20 s in freeroam; then WS -> stop x3, face walk in four directions, face jumps up/down/left/right
x2, web zip in four directions; return the newest `logs\MSM2-WS-v0.1c48-<timestamp>.log` and videos with English titles.
Inspect every attached log and video completely before analysing (CLAUDE.md "Attached evidence").
