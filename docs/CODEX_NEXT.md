# Next task — M14: frame bug, stop that ends standing, jump facing, zip direction, game ground/air state snapshot

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_M13_20261111.md`. Read first: `analysis/M13_OWNER_RESULT_20261111.md` (evidence and the
assessment for every item), then `analysis/M12_OWNER_RESULT_20261111.md` and the M10z sections at the end of
`analysis/M8_JUMP_PLAN_20261106.md`.

Owner must-haves (both mandatory): (1) on a wall the hero moves exactly like on the ground, no shaking or jerks;
(2) in the air after jumping or stepping off a face the changed gravity is kept and the hero falls along it until a new
WS or B (never hovering, never world down). Task 5 exists to make (2) implementable; falling is the next task after it.
Terms: WS = Wall Sprint (this mod), WR = vanilla Wall Run.
Accepted, do not regress: walking on the face in all directions; no re-approach loop (0 `REAPPROACH` in the M13 run);
stop without jerk; walking jumps move ~2 m in the stick direction.

All code is in `source/src/NativeHooks/camera_ref.cpp` (`gfStep`, `sp2Detour`, `wgiReadDetour`). Keep exact-build
guards, F8 kill, INI switches, code defaults OFF. One commit per task, push after each.
Procedure change (owner question, analysis section 6): no more tuning of air-model numbers in this task. Tasks 1-4 fix
measured causes; task 5 reads the game state that later air work (falling, landing timing) needs.

## Task 1 — frame boundary bug (cause of the walking-down hitch)
Fact: the 3-4 hero SetPosition calls of one frame are 2.0-2.2 ms apart; `newFrame` uses a 2 ms gap, so later calls of
the same frame start a "frame" with dt ~2 ms; the fall guard then sees 10-41 m/s and undoes normal walking steps
(100 `WS_FACE_FALL_UNDO`, analysis section 2).
Do:
1. A new frame starts only when the gap since the previous hero SetPosition call is >= 5 ms. dt of a frame = time between
   the first calls of two consecutive frames. Apply it to every per-frame quantity (force model, fall guard, follow,
   walk samples, samples' dt_ms).
2. Count per hold the maximum gap inside a frame and the minimum frame dt; print both in `WS_GRAVITY_FORCE_END`
   (`max_intra_frame_gap_ms`, `min_frame_dt_ms`).
Acceptance: no `WS_FACE_FALL_UNDO` while walking down; `dt_ms` in samples >= 5; owner sees no hitch.

## Task 2 — stop ends standing (one air phase the game detects)
Fact: the 0.12 s push lands at 0.11-0.16 s without the game ever leaving its wall-contact state -> crawl pose, game
position frozen. M12 (0.9 m / 0.25 s push) and every face jump from the crawl ended standing (analysis section 1).
Do: replace the stop push by a stop hop through the existing face-jump air model: launch along the normal with
v = sqrt(2 g 0.5 m), in-plane velocity 0, neutral stick to the game during the hop, one landing (`FACE_JUMP_LANDED`
path). Keep the 0.3 s outward-step limit after the landing. Log `WS_GRAVITY_FORCE_STOP_HOP height_m=0.5 airtime_s=`.
Acceptance: standing after every stop, exactly one landing per stop, 0 `REAPPROACH`.

## Task 3 — body facing at the face jump takeoff
Fact: the game gets the neutral stick only after takeoff (`gfFaceAirNow`); on the A press frame it reads the real stick.
HYPOTHESIS: the game's jump start turns the body toward the world-horizontal projection of the stick = the side axis on
a face (owner: up -> right, down -> left).
Do:
1. While the hero stands on the face in the hold (pinned, not in the air), on the WGI read in which A goes down, give the
   game a neutral stick already in that read and keep it neutral until 0.1 s after landing. A itself still reaches the
   game. Our own air control keeps the raw stick (unchanged).
2. Log `WS_FACE_JUMP_FACING` per jump (<= 40): hero forward (Actor axes forward or Transform+0x20 row, whichever the code
   already reads) projected on (e_up, e_side) at the press, at takeoff + 0.1 s, at landing; plus the walking direction at
   the press. Also log the arc: peak delta_n and the time of the peak.
Decision table (for the analysis, not for this code): forward stays the walking direction -> facing solved; forward still
turns to the side -> the turn is set by the game independent of the stick -> next task writes the rotation.

## Task 4 — web zip direction from the takeoff, not from the 2 s mapping
Fact: zip directions used the frozen learned theta (73.7 / 155.7 deg for down / left, wrong by 90 / 20 deg) while the
takeoff walking direction of the same jumps was correct; mirror agreement was 0-40 % (analysis section 4).
Do:
1. At a face-jump takeoff with source=walk, compute the jump's own mapping from that one pair: theta_t = angle of the
   walking direction on the face (e_up, e_side) minus h * angle of the stick, h = the current mirror only if
   mirror_agree / mirror_total >= 0.8, else the last h that met 0.8 in this hold (start value +1).
2. The zip of that jump uses theta_t and that h. Standing jumps (source=none) use the last theta_t/h of this hold that
   is < 10 s old, else the current model (as now).
3. `WS_FACE_ZIP` adds `theta_takeoff_deg`, `h_used`, `mapping_source=takeoff|recent_takeoff|model`.
Acceptance: zip with stick up/down/left/right goes the same way as walking with that stick. The body turn during the zip
is the game's and stays (logged by task 3's facing line at the zip too).

## Task 5 — read-only snapshot of the game's ground / air state (decides falling and landing work)
Fact: the game does not move an airborne hero while the gravity is a wall up, and our infinite plane clamp holds the hero
above nothing after walking off the wall (DontFall). Falling and landing on any surface need the game's own ground/air
state (analysis sections 5-6). Read-only diagnostic; no write.
Do:
1. Every 0.25 s while the hero exists (with or without a hold), copy the hero object bytes 0x000-0xFFF with a guarded
   read (stop at the first unreadable 0x100 block, log the readable size once) into a ring of 240 snapshots with a label:
   F = hold, pinned, not in a face jump, stick < 0.2; W = hold, pinned, walking (stick > 0.5, game step > 0);
   J = hold, face jump in the air (t 0.1-0.3 s after takeoff); H = hold, pinned, stick > 0.5 and game step 0 for >= 10
   frames (stuck or walked off the wall); G = no hold, world-vertical gravity, |game y step| < 0.002 m for 10 frames;
   A = no hold, world-vertical gravity, falling (y speed < -3 m/s from positions); other = no label.
2. When the hold ends by B and again at game exit, for each 4-byte offset classify it (byte flag / small int / float /
   pointer-like) and report offsets whose values separate {F, W, G} from {J, A} with no overlap (and say where H falls),
   at most 64 lines `WS_STATE_CANDIDATE off=0x.. type= ground_values= air_values= H_values=`. Also print the label counts.
3. Same for the hero Transform object bytes 0x00-0xFF (rotation rows), label-wise means for J vs W
   (`WS_STATE_TRANSFORM`), <= 16 lines.
Acceptance: the log has label counts with >= 4 snapshots for each of F, W, J, G, A and the candidate list (empty is a
valid result). No gameplay change.

## Not in scope (recorded, later tasks)
Falling after walking off the wall and landing on other surfaces (needs task 5), jump arc/landing timing, rotation writes,
zip camera swing and web line visual, charge jump, velocity search, mover editing, live camera search, L3 lock.

## Order and reporting
Task 1, Task 2, Task 3, Task 4, Task 5 (one commit each, push after each). Cloud verification and handoff as before:
build, test.sh, smoke_windows, test_hooks_windows (record flaky reruns exactly), package, `docs/CLOUD_VERIFIED.txt`,
`docs/CODEX_RESULT.md`, analysis file. Finish with STATUS / CHANGED / RESULT / NEED; NEED lists the owner test:
`Update-WS.cmd` -> Install mods -> Run modded game; ~20 s idle in freeroam; WS -> stop x3 (wait 3 s standing after each);
walk toward the ground 5 s twice (once with the camera turned 180 deg), then up/left/right 5 s each; walking jumps
up/down/left/right once each and one standing jump; web zips up/down/left/right; walk off the edge of the wall once and
wait 3 s, then B; without WS on the street: stand 3 s, run 3 s, jump twice; return the newest
`logs\MSM2-WS-v0.1c48-<timestamp>.log` and captioned videos.
