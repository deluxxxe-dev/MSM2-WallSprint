# Next task — M1b: TurnMaxDown 90 only (M1 result: down movement works, see analysis/C48_M1_RUNTIME_RESULT_20261104.md)

Status: M4 PASSED 2026-11-05 (analysis/M4_ACCEPTANCE_20261105.md); stage 1 accepted. Next: M5 camera — step 1 done (configs have offsets/FOV only, no roll; analysis/M5_CAMERA_20261105.md). Step 2: ActorRelativeCamera reference strings, else runtime camera-matrix scan. Default target: camera rolls so the wall reads as the floor, smooth entry/exit. Earlier: M3 DONE.

## M4 acceptance sweep (one owner session, no code change)
1 stop: release stick on the wall for ~5 s. 2 detach: jump off and end WR normally (body upright again?). 3 another wall facing a different compass direction: directions and left/right correct? 4 reach a wall edge/corner (just observe). 5 ground sprint after WS still normal. 6 fast travel or reload after WS (no leftover effects).
Decision: each failing item becomes one bounded fix task; all pass -> stage 2 camera.
 M2 core DONE 2026-11-04 (sprint pose on wall via clip swap + orientation override). Next: M3 free 360-degree direction — step 1 owner diagonal-input run, analyse logged travel angles. Earlier: M1 DONE 2026-11-04 (M1c accepted: no sideways drift, straight down works). Next: M2 animation/posture — plan in analysis/M2_ANIMATION_PLAN_20261104.md (M2a clip swap). Step 1 was an owner-side asset name listing (configs/anim containing 'wallrun'/'wall_run'/'sprint'); no code until a candidate asset exists. Previously: M1c ACTIVE (TurnGain 0 + TurnMaxDown 90 via ws_assets.py; decision table in analysis/C48_M1_RUNTIME_RESULT_20261104.md). Sideways WR drifts to straight-down with TurnMaxDown 90. M1b DONE 2026-11-04 (straight-down works). Steering stage now auto-deployed by Update-WS (docs/LOCAL_WORKSPACE.md). Next: M2 animation/posture discovery. Earlier: M1 run done 2026-11-04 (speed changed, down possible but diagonal). M1b package: steering TurnMaxDown 90, no speed change. Owner replaces the M1 stage with M1b, one WallRun (same sequence), reports whether travel is straight down / angle. Then M2 (animation/posture). Text below is the original M1 spec.

# (original) M1

Branch `codex/v01c20-edi-value-provenance`; no main merge. Previous c48 task text: `analysis/C48_CODEX_NEXT_ARCHIVE_20261104.md`.

## Direction (owner-approved 2026-11-04)

Extend vanilla WallRun instead of replacing it. Current blocker: pressing down yields ~-12° and left/right oscillation, also with WS OFF
(`TurnMaxDown=12` correlation only). Stop adding diagnostics that do not change which route is chosen. One hypothesis, one change, one game run.

Owner decisions: reversible/bounded gameplay writes are permitted; modified vanilla configs may be distributed
(record concerns in `docs/DISTRIBUTION_RISKS.md`); logs are returned in the Claude Code session, not via ChatGPT.
Frozen: RequestSimpleMotion/GravityOrientationOwner, ground-Sprint name route, c36-c43 CFG/slot/DR6 ownership proofs.

## Experiment

Package `MSM2-WS-M1-DownLimit` built by `source/tools/build_wallrun_m1_experiment.py` from owner-extracted vanilla files:

- steering Standard: `TurnMaxDown` 12 -> 60 (the only steering change);
- speed Standard (positive control): `SettleSpeedMax/Min` x2 in all 10 field structs. Field names are inferred (HYPOTHESIS).

Pins: steering SHA256 `32418c21...b7b`, speed SHA256 `cc07f41b...90c`. The builder fails closed on layout/value self-checks.
c48 (read-only) runs unchanged in the same session. Package stays out of Git.

## Owner run (one session, WS ON)

Install package + WS via Overstrike, WallRun: right -> reverse left -> reverse right -> hold down -> end. Return the newest
`MSM2-WS-v0.1c48-*.log` and say (a) did WallRun speed visibly change, (b) could Spider-Man move downward / what angle.

## Decision table (predeclared)

| Observation | Meaning | Next |
| --- | --- | --- |
| speed changed AND down works | config loaded, TurnMaxDown is the limit | M1 done; tune value; then repo cleanup |
| speed changed, down unchanged | config loaded, TurnMaxDown not the limit | route B: bounded write of the `+0x250` threshold inputs from the existing Update thunk (see c48 counters) |
| speed unchanged | config not loaded/selected | fix install path or choose another field; no native work |
| c48 counters both 0 | `+0x250` thresholds not active in reversal/down | close the +0x250 branch |

Route B (if reached) needs: single field, WallRun session only, hard count cap, automatic restore, default OFF, F8 stop, one test.

## Do not

Do not repeat TurnSpeed probes, c73, ON/OFF comparisons, or add observers beyond what the decision table needs.
