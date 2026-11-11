# Next task — M13: remove jerks (stop transition, re-approach loop, walking down) + face jump speed + web zip direction

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_M12_20261111.md`. Read first: `analysis/M12_OWNER_RESULT_20261111.md` (evidence for every
item), then `analysis/M11_OWNER_RESULT_20261111.md` and the M10z sections at the end of
`analysis/M8_JUMP_PLAN_20261106.md`.

Owner (2026-11-11): stop from WS is now stable; its remaining jerks must go, and the other items continue in parallel. Minimum requirement of the mod: on a wall the hero moves exactly like on the
ground, no shaking or jerks. Terms: WS = Wall Sprint (this mod), WR = vanilla Wall Run.
Accepted, do not regress: walking on the face in all four directions; stop from WS ends standing (3 of 3).

All code is in `source/src/NativeHooks/camera_ref.cpp` (`gfStep`, `sp2Detour`, `m10GravityTest`). Keep exact-build
guards, F8 kill, INI switches, code defaults OFF. One commit per task, push after each.

## Task 1 — walking toward the ground must never be clipped by the fall guard
Fact: the 6 m/s per-frame budget clips normal walking/running steps (analysis section 3).
Do: the guard exists only to stop an unbounded fall. Replace the budget by a fall detector: allow any downward step
while the stick is pushed, unless the downward speed (per frame) exceeds 9 m/s OR increased in each of the last 6 frames
by more than 0.2 m/s (gravity signature, 33 m/s^2 ~ 0.55 m/s per frame). Only then undo that frame's downward part.
Neutral stick: unchanged (no downward motion). Log every undo while the stick is pushed (speed, reason), cap 100.
Acceptance: `WS_FACE_MOVE_SAMPLE` has no `store_dy` while walking down; owner sees no hitch.

## Task 2 — the plane must follow the wall under the hero; no re-approach loop
Fact: the plane point is frozen at the hold start and is wrong by 0.18 m elsewhere on the wall, so the M12 re-approach
fires every 0.4 s (analysis section 2).
Do:
1. While pinned and not in a face jump, when the game leaves the hero at rest (no game step along the normal for 5
   frames), take the hero's current distance as the wall there: move the plane point along the normal so that plane_d
   becomes 0 (low-pass, <= 0.05 m per frame). Log changes > 0.05 m (`WS_GRAVITY_FORCE_PLANE event=follow`).
2. Re-approach only when the hero is farther than 0.3 m from the (followed) plane for > 0.2 s, and at most once per
   second. Log the reason.
Acceptance: no `REAPPROACH` lines while the hero stands or walks; face jumps still land.

## Task 3 — one landing per stop
Fact: each stop is push 0.9 m -> land -> game moves the hero out 0.3-1.2 m -> re-approach -> land, three times in 0.4 s
(analysis section 1).
Do, in this order, one owner run each (decision by `WALL_LANDED` count per stop and the owner video):
A. Shorten the path: stop push 0.9 m / 0.25 s -> 0.5 m / 0.12 s; approach speed toward the face capped at 4 m/s; after
   `WALL_LANDED` ignore the game's outward step (keep plane_d <= 0.02 m) for 0.3 s instead of letting the hero drift out.
   Expected: exactly one `WALL_LANDED` per stop.
B. Only if A still shows more than one landing or the hero does not stand: log for 0.6 s after the first landing, per
   frame, the game's step split into normal / wall-up / side and plane_d (`WS_STOP_TRACE`, <= 240 lines) and stop;
   report the trace instead of guessing.
Camera jerk during the stop: record what is seen, do not change camera code in this task.

## Task 4 — face jump: ground-like directional speed
Fact: takeoff in-plane speed is the instantaneous walking step at the press (1.0-5.2 m/s; up jumps 1.0-1.8 m/s); a face
jump lasts ~0.4 s, so the hero drifts 0.4-2 m (owner: slow drift, unlike a ground directional jump). The takeoff
direction already matches walking (analysis section 4). Owner also saw all jumps travel "into the screen".
Do:
1. Takeoff speed = mean in-plane walking speed over the last 0.3 s (not one step), with a floor of 5 m/s whenever the
   stick is pushed (> 0.5) and a walking direction is known; keep the direction as now.
2. Face jump airtime: launch along the normal for 1.8 m height as now; keep the in-plane velocity constant in the air
   (no decay) unless the stick steers.
3. Trace each face jump: at takeoff and at landing log the hero position split into (e_up, e_side, normal) relative to
   the takeoff point, the takeoff velocity, the stick and the source (`WS_FACE_JUMP_TRACE`, <= 60). This checks the
   owner's "into the screen" against the logged direction.
Acceptance: walking or standing-with-stick jumps travel ~2 m or more in the stick/walking direction with the same arc
feel as a ground jump.
Body facing during jumps (game yaw, world-horizontal) stays out of scope; record what the owner sees.

## Task 5 — web zip: stable direction
Fact: the zip uses the walking-learned mapping (h, theta) of the last 2 s, which changes between zips because the game
camera swings after each zip: h/theta -1/177, +1/136, -1/-18, -1/126 deg in one run; stick left once went to the right
(analysis section 5). The game's own zip motion/facing is also still visible.
Do:
1. Freeze the mapping at the face jump takeoff (h, theta at that moment) and use the frozen mapping for steering and
   the zip of that jump; never update it in the air.
2. h may change only after >= 30 consistent samples (>= 80 % agreement) across >= 60 degrees of stick angle; theta
   keeps the 2 s mean but only from samples taken while the stick was held steady (direction change < 20 degrees).
3. Log in `WS_FACE_ZIP` the frozen h/theta and the takeoff time.
Acceptance: zips with up/down/left/right go to the same sides as walking with the same stick in that camera pose.
The camera swing and web line visual stay out of scope.

## Not in scope (recorded, later tasks)
Body facing during jumps, zip camera swing and web line visual, charge jump, velocity search, mover editing, live camera
search, L3 lock.

## Order and reporting
Task 1, Task 2, Task 3A, Task 4, Task 5 (one commit each, push after each). Cloud verification and handoff as before: build, test.sh, smoke_windows, test_hooks_windows
(record flaky reruns exactly), package, `docs/CLOUD_VERIFIED.txt`, `docs/CODEX_RESULT.md`, analysis file. Finish with
STATUS / CHANGED / RESULT / NEED; NEED lists the owner test: `Update-WS.cmd` -> Install mods -> Run modded game; ~20 s
idle in freeroam; WS -> stop x3 (wait 3 s standing after each); stand still on the face 10 s; walk up/down/left/right
5 s each, then walk a long way (> 20 m) along the wall and stand 10 s there; walking jumps and standing jumps with
the stick up/down/left/right x2; web zips up/down/left/right; return the newest
`logs\MSM2-WS-v0.1c48-<timestamp>.log` and captioned videos.
