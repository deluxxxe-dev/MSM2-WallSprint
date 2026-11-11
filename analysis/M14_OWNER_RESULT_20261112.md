# M14 owner result (2026-11-12, build 781b768, log MSM2-WS-v0.1c48-20261112-010128)

Inspected in full: the log (13899 lines) and both videos over their whole duration (3 frames/s): WallJump (9.4 s,
captions up/down/left/right), WebZip (9.5 s, captions up/down/left/right). Terms: WS = Wall Sprint, WR = vanilla Wall Run.

## Owner report
Solved: walking toward the ground no longer lags; the direction of stick jumps is right.
Open: stop from WS ends in the crawl (a jump makes the hero stand); stick-jump distance, airtime and landing lag; web zip
in general; no fall while the gravity is changed.
Owner must-have (CLAUDE.md): seamless motion, and the changed gravity kept in the air with a fall along it.

## 1. Gravity is applied twice in every face-air phase (VERIFIED-STATIC + VERIFIED-RUNTIME) - main cause
- `gfStep` subtracts the substituted gravity every frame for every state (`gfL.vg -= kGfG * u * dt`, u = substituted up,
  around line 1741) and the face-air block subtracts it again (`gfL.vg -= kGfG * n0 * dt`, around line 1846); u = n0 on a
  face, so the air phase runs with 66 m/s^2 instead of 33.
- Prediction with 66: jump launch 10.9 m/s -> peak 0.90 m at 0.165 s, landing 0.33 s; stop hop launch 5.745 m/s -> peak
  0.25 m, landing 0.17 s. Measured: every walking jump `peak_delta_n=0.899-0.901 peak_time_s=0.151-0.172`, landing
  0.31-0.33 s; stop hops landed after 0.19-0.22 s. Designed values (33): jump 1.8 m / 0.66 s, hop 0.5 m / 0.35 s.
- Consequences: jumps are half as high and half as long (1.65-1.71 m travel instead of ~3.3 m at 5 m/s), the model lands
  while the game's jump animation is still in its air part (owner: landing lag), and the stop hop is too short for the
  game to leave its wall-contact state -> crawl (same failure as M13's 0.12 s push).

## 2. Stop (VERIFIED-RUNTIME)
- 3 stops, each one `STOP_HOP takeoff` + one landing, 0 `REAPPROACH`; afterwards the game position is frozen (crawl, as in
  M13). Expected to change with the correct gravity (0.35 s air). If not, the hop height is the next variable.

## 3. Face jump facing solved (VERIFIED-RUNTIME)
- `WS_FACE_JUMP_FACING`: at press, takeoff + 0.1 s and landing the hero forward equals the walking direction (e.g.
  -1.000/-0.014 -> same at all three times). Masking the stick from the A press read fixed the body turn.

## 4. Web zip (VERIFIED-RUNTIME)
- 4 zips. Zip 12 (walking takeoff, mapping_source=takeoff) went the right way. Zips 9-11 were jumps whose takeoff source
  was `model` (the hero stood with the stick pushed, last walking sample 1.9-6.4 s old), so the 2 s learned mapping was
  used: theta -91.5 / -67.4 / 6.4 deg while the correct value from the walking takeoffs of this hold is about -170 deg;
  mirror agreement 29-54 of 64. Results: stick up -> side -1, stick down -> side +0.99, stick left -> side +1 (all wrong).
- The learned 2 s regression is not reliable when the camera turns; a single recent walking pair (stick angle, walking
  direction) was right every time it was used.
- Video: strong camera swings during zips (game camera; out of scope so far).

## 5. Fall guard after landings (VERIFIED-RUNTIME)
- 100 `WS_FACE_FALL_UNDO reason=accelerating_six_frames` at 3-4 m/s, right after face-jump landings while walking away:
  the run start accelerates by more than 0.2 m/s per frame. Below walking speed this is not a fall.

## 6. Ground/air state snapshot (VERIFIED-RUNTIME)
- `WS_STATE_SUMMARY` x3 (at each B): candidate_count=0, coverage_complete=0. Label counts per report: F 32/55/133,
  W 0/0/84, J 1/2/8, H 0, G 185/149/0, A 0/2/0. The ring of 240 x 0.25 s keeps only the last 60 s, there is no report at
  game exit, and the walk-off / street-jump steps were not in the captured window (H=0, A<=2). No conclusion possible.
- `WS_STATE_TRANSFORM`: Transform rows +0x00 / +0x10 / +0x20 during a face jump are right / up (= wall normal) / forward.

## 7. Falling (must-have) - status
- Not implemented; the infinite plane clamp still holds the hero above nothing past a wall edge. The force model already
  has a general gravity integration along the substituted up for the not-pinned state; what is missing is when to leave
  the pinned state (ground lost) and when to land on another surface. Both need the game's ground/air signal (section 6)
  or a defined proxy. Task 4 of M15 collects it with corrected coverage; task 5 adds the walk-off release.
