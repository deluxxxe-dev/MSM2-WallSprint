# c48 + M1 runtime result — 2026-11-04

Log `MSM2-WS-v0.1c48-20261104-214321.log` (not committed). Conditions: WS ON + package M1 (TurnMaxDown 12->60, SettleSpeedMax/Min x2). Not comparable 1:1 with c47.

## Owner observations (VERIFIED-RUNTIME, owner report)
- Initial speed same as vanilla, acceleration clearly stronger -> config assets ARE loaded (speed-control field mapping plausible).
- Moving downward on the wall is now possible (previously impossible, ~-12deg). Animation is still the sideways WallRun; travel is diagonal-down (sideways + down). Posture unchanged (vanilla WR).

## c48 counters (VERIFIED-RUNTIME)
- admission complete_verified=1 admitted=1 write_dispatch_gate=0; thresholds 0x3f000000 / 0x3f35c28f, unchanged; read_fail=0; no hook_queue_dropped.
- Coverage exact: 1 + 1054 Update calls = 1055 samples (session #1 + #2).
- `+0x250 > 0.5`: 168 frames; `> 0.709999979`: 149 frames, one contiguous ~2.5 s window (session elapsed ~7.5-10.0 s).
- Worker samples (100 ms): `+0x250` is a smooth signed value, min -0.865, max +0.996, first positive peak ~8.0 s, negative again from ~10.2 s.

## Classification
- VERIFIED-RUNTIME: under M1 the field exceeds both V8 thresholds for sustained periods (c47's "below both" does not hold here).
- OBSERVED-CORRELATION only: M1 config change may have changed the field's range vs c47 (0..-0.2); which input/state it tracks is UNKNOWN. Range [-1,1] is compatible with a dot-product/cosine (HYPOTHESIS, unchanged).
- Decision table row 1 satisfied (speed changed AND down works): TurnMaxDown (or the combination) lifts the down limit. Attribution to TurnMaxDown alone is untested (speed was also changed).
- Route B (`+0x250` writes) is NOT needed now.

## Next (one change): M1b = TurnMaxDown 90 only, speed vanilla
Tests attribution (speed removed) and whether a larger limit gives straight-down travel. Animation/posture remain a separate milestone (M2).

## M1b result (owner, 2026-11-04) — VERIFIED-RUNTIME (owner report)
TurnMaxDown 90 only (speed vanilla): WallRun now moves straight down. Animation unchanged (sideways WallRun clip). So `TurnMaxDown` alone lifts the down limit; the speed change was not needed.
Units: 12 -> ~-12deg observed earlier, 60 -> diagonal, 90 -> straight down: consistent with degrees (OBSERVED-CORRELATION). Remaining gap: animation/posture (M2).

## Deployment
`Update-WS.cmd` now deploys the steering stage automatically (`source/tools/ws_assets.py`): put the vanilla `hero_WallRunSteeringSetup_Standard.config` in `MSM2-WS\_inputs\`; stage is written to `Mods Library\Mods-Developing\MSM2-WS-Assets\MSM2-WS-WallRunSteering.stage` (TurnMaxDown 90). Engine runs pre-pull code, so the first Update-WS after this commit only pulls; run it twice.
