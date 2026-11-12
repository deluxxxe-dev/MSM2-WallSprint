# Next task — M17: safe walk-off release (contact probe), prompt fall start, stop that stands, live camera mapping, camera/controller position

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_M16_20261112.md`. Read first: `analysis/M16_OWNER_RESULT_PART1_20261112.md` and
`analysis/M16_OWNER_RESULT_PART2_20261112.md` (evidence for every item), then `analysis/M15_OWNER_RESULT_20261112.md`.

Owner must-haves (both mandatory, CLAUDE.md): (1) on a wall the hero moves exactly like on the ground, no shaking or
jerks; (2) in the air after jumping or stepping off a face the changed gravity is kept and the hero falls along it until
a new WS or B (never hovering, never world down).
Terms: WS = Wall Sprint (this mod), WR = vanilla Wall Run.
Accepted, do not regress: walking in all directions; face jump arc and walking-takeoff direction; facing rows held in
face air; genuine walk-off falls along -u.

All code is in `source/src/NativeHooks/camera_ref.cpp`. Keep exact-build guards, F8 kill, INI switches, code defaults
OFF (enable in the shipped INI like the existing WS gravity switches). One commit per task, push after each.

## Task 1 — walk-off release must never enter the face (bug: fell through the face)
Fact: the game can keep its own fall state while the hero stands on the face after a WallSprint face-jump landing or stop
hop; the M16 rule then released and moved the hero along -u into the face; the game pushed back along +u (0.05-0.92
m/frame) but was ignored; `game_fall_ended` re-placed the plane inside the face (part 1 section 1, part 2 section 1).
Do:
1. Probe phase (first 0.20 s after a release): move along -u at most 0.10 m total. If any game step has a +u component
   > 0.01 m while plane_d < 0, the face is real: abort, put the hero back to plane_d = 0 on the ORIGINAL plane, pin, and
   ignore the fall signature for 1.0 s (`reason=contact_abort`). Only after a clean probe continue the full fall.
2. During the full fall, a +u game step > 0.01 m (collision push-back) is a contact: land with the plane through the
   position BEFORE that frame's penetration (never through a penetrated position) (`reason=contact`).
3. `game_fall_ended` may only land when plane_d relative to the original plane is > -0.05 m or after a contact; never
   re-place the plane at a deeper position.
4. No release in the 0.5 s after a face-jump landing or a stop-hop landing.
Acceptance: no fall through a face in any jump/stop test; genuine walk-offs still fall (log shows probe clean).

## Task 2 — start the fall when the game starts it
Fact: the release needed 22 frames because the 4-frames-rising rule fails on noisy per-frame speeds; ~0.7 s of hover with
the fall animation (part 2 section 1).
Do: compute the game's world-down speed over a 0.1 s sliding window (displacement / time). Signature = window speed > 3
m/s and its increase over the last 0.1 s > 1.5 m/s (gravity ~26 m/s^2 gives ~2.6). Then start the task-1 probe. Log
`detect_delay_s` = release time - first frame with window speed > 1 m/s.
Acceptance: detect_delay_s <= 0.2 s in walk-offs; no release while walking down a face (steady <= 6 m/s).

## Task 3 — stop hop that makes the game stand
Fact: the game stood only when the hop apex was ~0.95 m from the face (M15); apex 0.25 m (M14) and 0.29-0.55 m (M16)
left it crawling (part 1 section 2).
Do: apex = 0.8 m from the plane (absolute, not d0-relative), landing at 0.35 s after takeoff (solve v and g_hop as in
M16). Log apex and the result after landing: game position frozen for 1 s while stick neutral = `crawl_suspect=1`.
Decision for the owner run: stands -> keep 0.8; crawls -> next try 0.95 (do not change it in this task).

## Task 4 — live camera mapping for zips and air steering (validated before use)
Fact: stored walking pairs are stale after the camera moves (11-17 s old pairs, 65-110 deg errors); `WS_CAMERA_SETTER m0`
row 1 = hero up and row 2 changes during holds (part 2 section 2). Not yet validated.
Do:
1. Each frame while walking on the face (stick > 0.5, speed > 2 m/s, steady 0.3 s as in M16), predict the walking
   direction from the stick and the latest setter matrix: screen forward = row 2 projected on the face (try also -row 2,
   row 0, -row 0; keep the four predictions), screen right = forward x n with both handedness signs. Record the angle
   between each prediction and the actual walking direction.
2. `WS_CAMERA_MAP_CHECK` every 0.5 s while walking (<= 200): errors of the 8 candidates (4 rows x 2 signs), the camera row
   values, and the number of samples.
3. A candidate is valid when its median error < 10 deg over >= 40 samples taken across >= 3 different camera row-2
   directions (> 30 deg apart). Then (INI `WALLSPRINT_WS_FACE_CAMERA_MAP`, shipped 1) zips and air steering use the
   live mapping of that candidate (`mapping_source=camera_live`); otherwise keep the current behaviour and log why.
Acceptance: either a validated candidate and zips going the stick direction, or the check lines showing none is valid.

## Task 5 — which position does the camera follow (read-only diagnostic)
Fact: the camera does not follow Transform+0x30 writes during holds (falls, earlier M11 jumps) and jumps to the hero after
B (part 2 section 1).
Do: at 50 ms cadence, in hero bytes 0x000-0xFFF and in every object pointed to by 8-byte pointers in hero bytes
0x000-0x400 (read 0x200 bytes each, guarded, at most 64 objects), find 12-byte float vectors equal to the Transform
position (within 0.05 m) in >= 90 % of G and W snapshots. Then report for each such offset its distance to the Transform
position during L (released fall) and during face jumps (`WS_POSITION_MIRROR obj= off= G_match= W_match= L_mean_dist=
J_mean_dist=`, <= 32 lines). A mirror that stays behind during L is the camera/controller candidate.
Acceptance: the list (empty is a result). No write.

## Not in scope (recorded)
Writing the candidate position (next task after task 5), camera jerk at jump takeoff, left/right landing lag, zip camera
swing, web line visual, charge jump, velocity search, mover editing, L3 lock.

## Order and reporting
Task 1 to Task 5 (one commit each, push after each). Cloud verification and handoff as before: build, test.sh,
smoke_windows, test_hooks_windows (record flaky reruns exactly), package, `docs/CLOUD_VERIFIED.txt`,
`docs/CODEX_RESULT.md`, analysis file. Finish with STATUS / CHANGED / RESULT / NEED; NEED lists the owner test:
`Update-WS.cmd` -> Install mods -> Run modded game; ~20 s idle in freeroam; WS -> stop x3 (wait 3 s after each); walking
jumps up/down/left/right x2 (watch for falling through the face); walk 5 s while turning the camera slowly in a full
circle (for task 4), then standing jump + zip up/down/left/right after turning the camera to a new angle each time; walk
off the side edge of a face and fall 3 s, B; on a building whose face has a step (a ledge sticking out), walk off the
ledge edge toward the recessed part of the same face so the hero lands on it; without WS on the street (no RT): stand 3 s,
move 3 s, jump twice; return the newest `logs\MSM2-WS-v0.1c48-<timestamp>.log` and captioned videos.
