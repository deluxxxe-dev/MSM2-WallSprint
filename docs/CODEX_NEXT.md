# Next task — M15: single gravity in the air, zip mapping from walking pairs, fall after walking off the face

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_M14_20261112.md`. Read first: `analysis/M14_OWNER_RESULT_20261112.md` (evidence for every
item), then `analysis/M13_OWNER_RESULT_20261111.md`.

Owner must-haves (both mandatory, CLAUDE.md): (1) on a wall the hero moves exactly like on the ground, no shaking or
jerks; (2) in the air after jumping or stepping off a face the changed gravity is kept and the hero falls along it until
a new WS or B (never hovering, never world down).
Terms: WS = Wall Sprint (this mod), WR = vanilla Wall Run.
Accepted, do not regress: walking in all directions incl. toward the ground without lag; face jump direction and body
facing (stick masked from the A press read); 0 `REAPPROACH`; walking-takeoff zip (mapping_source=takeoff).

All code is in `source/src/NativeHooks/camera_ref.cpp`. Keep exact-build guards, F8 kill, INI switches, code defaults
OFF. One commit per task, push after each.

## Task 1 — gravity applied once (cause of short jumps, landing lag and the crawl after the stop)
Fact: the general per-frame `gfL.vg -= kGfG * u * dt` (u = substituted up, ~line 1741) and the face-air
`gfL.vg -= kGfG * n0 * dt` (~line 1846) both run during a face jump / stop hop; measured peak 0.90 m at 0.165 s and
landing 0.32 s = exactly the 66 m/s^2 prediction (analysis section 1).
Do: in a frame where the face-air block runs, the general subtraction must not run (one gravity source per frame). Check
every other state for the same double application and list them in CODEX_RESULT. Add a unit test that integrates one
face jump with the real code path and expects peak 1.8 m +- 0.05 at 0.33 s +- 0.02, and a stop hop peak 0.5 m at
0.175 s.
Acceptance (log): `WS_FACE_JUMP_FACING ... peak_delta_n` ~1.8 at ~0.33 s, landing ~0.66 s; `STOP_HOP event=landing
airtime_s` ~0.35. Owner: stop ends standing (if still crawling, report it; the hop height is the next variable, do not
change it in this task).

## Task 2 — zip / standing-jump direction from the last walking pair, not from the 2 s regression
Fact: jumps with takeoff source `model` used the learned 2 s mapping (theta -91 / -67 / 6 deg, correct ~-170) and every
such zip went to the wrong side; single walking pairs were right every time (analysis section 4).
Do:
1. While walking on the face (game step > 1 m/s, stick > 0.5), every frame store the pair: theta_w = angle(walking
   direction in e_up/e_side) - h * angle(stick), with h = the trusted mirror (as now). Keep the newest pair and its time.
2. Any face jump whose takeoff source is not `walk` (model / none) freezes the newest walking pair (any age; log the age)
   instead of the regression. The regression stays only as the fallback when no pair exists in this hold.
3. `WS_FACE_ZIP` / `WS_FACE_JUMP_INPUT`: `mapping_source=walk_pair`, `pair_age_s`.
Acceptance: zips up/down/left/right after standing jumps go the same way as walking with that stick.

## Task 3 — fall guard does not fire below walking speed
Fact: 100 `accelerating_six_frames` undos at 3-4 m/s right after landings (run start), analysis section 5.
Do: the acceleration rule applies only when the downward speed is > 6 m/s. Keep the 9 m/s rule.

## Task 4 — ground/air snapshot with usable coverage (read-only)
Fact: M14 snapshot: 0 candidates, H=0, A<=2, only the last 60 s kept, no report at game exit (analysis section 6).
Do:
1. Cadence 50 ms. Instead of one ring keep a reservoir per label (newest 60 snapshots per label F, W, J, H, G, A), so
   street labels survive later holds. A = no hold, world gravity, |vertical speed| > 1.5 m/s for >= 3 consecutive
   snapshots (jump or fall). J = the whole face-air phase (takeoff + 0.05 s to landing - 0.05 s).
2. Report (same format as M14 + `WS_STATE_SUMMARY`) at every B, every 60 s while the hero exists, and at hero loss.
3. Also report 3-float vectors (12-byte, 4-aligned) in the hero bytes whose direction matches the walking direction
   (cos > 0.98) in >= 90 % of W snapshots (`WS_STATE_MOVE_VECTOR off= match= n=`), <= 16 lines: a candidate for the game's
   own camera-correct move intent.
Acceptance: label counts >= 10 for F, W, J, H, G, A in the owner run; candidate list printed (empty is a result).

## Task 5 — walking off the face: release the plane and fall along the changed gravity (must-have 2, first step)
Fact: past a wall edge the plane clamp (`if d < 0 -> q += (want - d) * n0`) keeps the hero hovering; the game does not move
an airborne hero under a wall up (analysis section 7).
Do (INI `WALLSPRINT_WS_FACE_WALKOFF`, code default 0, enabled in the shipped INI like the other WS gravity switches):
1. Ground-lost proxy while pinned, not in face air, not in the stop: the hero walked (game in-plane step > 1 m/s in >= 5
   of the last 10 frames) and then the game step is 0 (< 1 mm) for >= 8 consecutive frames while the stick stays > 0.5.
2. On it: unpin, stop clamping to the plane, keep the in-plane velocity of the last walking frames, and let the existing
   general model integrate gravity along -u (the substituted up, unchanged until B / new WS), speed cap 30 m/s. No plane
   clamp while released. Air A = the existing zip impulse.
3. Leave the release when: B / new WS (existing), or the game's own step becomes non-zero for 3 frames (the game moves
   the hero again = it has ground) -> pin with the plane through the current position and the current u as normal.
4. Log `WS_FACE_WALKOFF event=release|land|end reason= speed= plane_d= frames=` and, during the release, every 5th frame
   the game step split into u / in-plane and plane_d (<= 200 lines).
Acceptance: walking off the side or top/bottom edge of a face the hero falls along the face gravity (not world down, not
hovering) until B; walking into an obstacle on the face does not trigger (report any false release with its log lines).

## Not in scope (recorded, later tasks)
Landing on another surface after a release (needs task 4/5 data), zip camera swing and web line visual, charge jump,
velocity search, mover editing, live camera search, L3 lock.

## Order and reporting
Task 1 to Task 5 (one commit each, push after each). Cloud verification and handoff as before: build, test.sh,
smoke_windows, test_hooks_windows (record flaky reruns exactly), package, `docs/CLOUD_VERIFIED.txt`,
`docs/CODEX_RESULT.md`, analysis file. Finish with STATUS / CHANGED / RESULT / NEED; NEED lists the owner test:
`Update-WS.cmd` -> Install mods -> Run modded game; ~20 s idle in freeroam; without WS on the street: stand 3 s, run 3 s,
jump twice, then step off a rooftop and fall ~2 s; WS -> stop x3 (wait 3 s after each); walk on the face 5 s; walking
jumps up/down/left/right; standing (stick pushed) jumps with web zip up/down/left/right; walk into an obstacle on the
face 3 s; walk off the edge of the face, fall 3 s, B; return the newest `logs\MSM2-WS-v0.1c48-<timestamp>.log` and
captioned videos.
