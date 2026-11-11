# Next task — M16: fall along the changed gravity after walking off, smaller stop hop, body facing held in the air, steady walking pairs

Branch `codex/v01c20-edi-value-provenance` (no main merge, no force-push). Previous task text:
`analysis/CODEX_NEXT_ARCHIVE_M15_20261112.md`. Read first: `analysis/M15_OWNER_RESULT_20261112.md` (evidence for every
item), then `analysis/M14_OWNER_RESULT_20261112.md`.

Owner must-haves (both mandatory, CLAUDE.md): (1) on a wall the hero moves exactly like on the ground, no shaking or
jerks; (2) in the air after jumping or stepping off a face the changed gravity is kept and the hero falls along it until
a new WS or B (never hovering, never world down).
Terms: WS = Wall Sprint (this mod), WR = vanilla Wall Run.
Accepted, do not regress: walking in all directions without lag; stop ends standing; face jump arc 1.8 m / 0.65 s with
one gravity; walking-takeoff jump direction; 0 `REAPPROACH`.

All code is in `source/src/NativeHooks/camera_ref.cpp`. Keep exact-build guards, F8 kill, INI switches, code defaults
OFF (enable new switches in the shipped INI like the existing WS gravity switches). One commit per task, push after each.

## Task 1 — walk-off fall along the substituted gravity (must-have 2)
Fact: past the edge the game itself treats the hero as airborne and moves it in WORLD down (step speed 10.7 -> 24.4 m/s in
0.5 s); the fall guard undid it (hover); the walk-off release then took the game's own fall steps as "ground again" and
re-pinned after 3 frames (analysis section 4).
Do:
1. Ground lost = the game's fall signature while pinned, not in face air, not in the stop hop: the game's world-down step
   speed > 6 m/s and rising by >= 0.25 m/s per frame for >= 4 consecutive frames. Remove the zero-step proxy.
2. Released state ("face fall"): from that frame every hero SetPosition call gets our position (as in face air; the
   game's steps are ignored, no fall guard, no plane clamp). Velocity: in-plane velocity of the last walking frames before
   the signature (not the fall speed), then `v -= 33 * u * dt` with u = the substituted up (unchanged until B / new WS),
   cap 30 m/s. Air A = the existing zip impulse.
3. Landing: (a) moving toward the original face plane and within 0.02 m of it -> the existing face landing; (b) the
   game's fall signature ends: its world-down step speed < 2 m/s for 3 consecutive frames after having been > 6 ->
   pin with the plane through the current position and normal u (log `reason=game_fall_ended`).
   HYPOTHESIS for (b): the game ends its fall when its ground probe (along its gravity) finds a surface under our position.
4. Log `WS_FACE_FALL event=release|land|end reason= t= v_u= v_plane= plane_d=`, and every 5th frame of the fall the
   game step (world y, along u, in-plane) and plane_d (<= 300 lines). B / new WS end it as now.
Acceptance: walking off a face edge, the hero falls along the face gravity (not world down, not hovering) until B or a
landing; owner video + log. If a landing on another surface is never detected, report the game-step trace near the
contact; do not guess another rule.

## Task 2 — stop hop without the visible position shift
Fact: the hop starts 0.45 m off the plane, rises 0.5 m and returns: ~1 m visible motion; airtime 0.41-0.43 s made the
game stand (analysis section 1).
Do: apex = start distance + 0.15 m and landing on the plane at t = 0.35 s after takeoff: solve the hop's own launch speed
v and gravity g_hop from d(t) = d0 + v t - g t^2 / 2 with v^2 / (2 g) = 0.15 and d(0.35) = 0 (this hop only; the face jump
keeps 33). Log `WS_GRAVITY_FORCE_STOP_HOP ... d0= v= g_hop=`.
Acceptance: the hero stands after every stop; owner sees no shift.
Decision if the hero crawls again: report it; the next change is airtime, not height.

## Task 3 — hold the body facing during face air (bounded rotation write experiment)
Fact: in later jumps and in zips the body turned ~70 deg toward the side axis during the air phase although the stick was
masked; Transform rows +0x00 / +0x10 / +0x20 are right / up (= n) / forward (`WS_STATE_TRANSFORM`, analysis section 2).
Do (INI `WALLSPRINT_WS_FACE_AIR_FACING`, code default 0, shipped 1; F8 stops it):
1. At takeoff store the forward f0 = walking direction on the face (for a zip: the zip direction), up = n, right = up x f0
   (handedness as the game's rows: check right/up/forward of the current rows before the first write and keep the same
   sign convention).
2. During face air and face fall only, on each hero SetPosition call read rows 0x00-0x2C of that Transform; if the forward
   row differs from f0 by > 2 deg, write the three rows (12 floats, w components untouched) and count it. Cap 4000 writes
   per hold. Never write outside face air / fall; after landing the game's rotation is left alone.
3. Log per jump `WS_FACE_AIR_FACING writes= overwritten_next_call= max_dev_deg=` (overwritten = the next call read the
   game's turned rows again).
Acceptance: forward at landing = takeoff forward in `WS_FACE_JUMP_FACING`; owner sees no body turn in jumps and zips.
Decision table: overwritten_next_call ~ every call and the owner still sees the turn -> the game sets the rendered rotation
elsewhere; report, do not search further in this task.

## Task 4 — walking pairs only from steady walking
Fact: walk pairs with theta 81.7 / -86.2 deg (correct ~+-175) were stored while the game was still turning the hero toward
a new stick direction (analysis section 2).
Do: store a pair only when, over the last 0.3 s, the stick angle changed < 10 deg, the walking direction changed < 10 deg,
and the walking speed > 2 m/s. Log `pair_steady_s` with the pair.
Acceptance: standing jumps and zips with up/down/left/right go the same way as walking with that stick.

## Task 5 — snapshot separation against the game's airborne state
Fact: J (WallSprint face jump) is not the game's airborne state; candidates were tested against {J, A} together (analysis
section 5).
Do: report candidates separately for {F, W, G} vs {A} and {F, W, G} vs {H}, with J values printed for information only;
H minimum 5. Add label L = released face fall (task 1).
Acceptance: two candidate lists (empty is a result).

## Not in scope (recorded)
Camera jerk at face jump takeoff, left/right landing lag (arcs identical; record what is seen), zip camera swing and web
line visual, charge jump, velocity search, mover editing, live camera search, L3 lock.

## Order and reporting
Task 1 to Task 5 (one commit each, push after each). Cloud verification and handoff as before: build, test.sh,
smoke_windows, test_hooks_windows (record flaky reruns exactly), package, `docs/CLOUD_VERIFIED.txt`,
`docs/CODEX_RESULT.md`, analysis file. Finish with STATUS / CHANGED / RESULT / NEED; NEED lists the owner test:
`Update-WS.cmd` -> Install mods -> Run modded game; ~20 s idle in freeroam; WS -> stop x3 (wait 3 s after each); walking
jumps up/down/left/right x2; standing (stick pushed) jumps with web zip up/down/left/right; walk off the side edge of a
face and fall 3 s, B; again walk off the top or bottom edge toward another surface and let the hero land; without WS on
the street (no RT): stand 3 s, move 3 s, jump twice, step off a rooftop; return the newest
`logs\MSM2-WS-v0.1c48-<timestamp>.log` and captioned videos.
