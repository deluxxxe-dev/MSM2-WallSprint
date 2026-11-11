# Next task — M13: remove jerks in the face-as-ground state (stop transition, re-approach loop, walking down)

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_M12_20261111.md`. Read first: `analysis/M12_OWNER_RESULT_20261111.md` (evidence for every
item), then `analysis/M11_OWNER_RESULT_20261111.md` and the M10z sections at the end of
`analysis/M8_JUMP_PLAN_20261106.md`.

Owner priority from now: **no jerks**. Minimum requirement of the mod: on a wall the hero moves exactly like on the
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

## Not in scope (recorded, later tasks)
Face jump speed/feel and body facing, web zip direction and camera swing, charge jump, velocity search, mover editing,
live camera search, L3 lock. Do not change the M12 jump/zip direction code in this task.

## Order and reporting
Task 1, Task 2, Task 3A. Cloud verification and handoff as before: build, test.sh, smoke_windows, test_hooks_windows
(record flaky reruns exactly), package, `docs/CLOUD_VERIFIED.txt`, `docs/CODEX_RESULT.md`, analysis file. Finish with
STATUS / CHANGED / RESULT / NEED; NEED lists the owner test: `Update-WS.cmd` -> Install mods -> Run modded game; ~20 s
idle in freeroam; WS -> stop x3 (wait 3 s standing after each); stand still on the face 10 s; walk up/down/left/right
5 s each, then walk a long way (> 20 m) along the wall and stand 10 s there; return the newest
`logs\MSM2-WS-v0.1c48-<timestamp>.log` and captioned videos.
