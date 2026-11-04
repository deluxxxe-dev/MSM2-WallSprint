# M3 free 360-degree direction — first data (2026-11-05)

Log `MSM2-WS-v0.1c48-20261105-002908.log` (not committed). Owner input order: up-right, down-right, up-left, down-left, up-right.
Owner observation (VERIFIED-RUNTIME, owner report): a diagonal stick during WS moves along the nearest of up/down/left/right only.
Diagonal travel happens only when the WR before WS was already moving up-right/up-left; body facing then stays the side or up WS pose.

Log (WALL_PLANE_HEADING, displacement-derived, first ~0.6 s per session only because of the diagnostic cap):
- sessions 2/4: pure vertical travel (delta Y only), heading -90/+90deg;
- session 3: up+lateral travel ~51deg from vertical, session 6: ~21deg; one heading turn then deadband hold (no further turning);
- session 5: almost pure lateral.
Classification: travel direction in WS is set by vanilla WallRun, which appears to keep its entry direction and otherwise snap to
an axis (OBSERVED). Mechanism UNKNOWN. Vanilla speed config has separate Vertical*/Horizontal* setups (VERIFIED-STATIC), compatible
with separate vertical/horizontal WR modes (HYPOTHESIS). WS heading follows displacement only (by design), so it cannot add diagonals.

## M3a (one change, auto-deployed): TurnSpeedFast/Slow 100/75 -> 255/255
WSU also raised TurnSpeedFast (200) in its wall-slide stage (reference only). Earlier probes LOWERED these with no visible effect.
| Observation | Next |
| --- | --- |
| diagonal stick now turns travel diagonal | config route works; tune values |
| no change | turn rate is not the limit -> native route (direct heading/velocity control in WR Update, bounded write rules) |
| WR unstable | revert |

## M3a result (owner, 2 runs, 2026-11-05) — VERIFIED-RUNTIME (owner report)
TurnSpeed 255/255: at most a slight upward drift while running sideways with an up-diagonal stick; far from free turning.
New regression: sideways WS -> stop on wall -> opposite direction produced a wrong body orientation and a strong downward pull.
Decision: config turn rate is not the limit; TurnSpeed reverted to vanilla (ws_assets.TURN_SPEEDS=None). Next is the native route.

## M3b (read-only, one run): locate the WallRun travel-direction field
`include/wallrun_direction_scan.hpp`: every 100 ms worker sample copies WallRun.this+[0,0x800) with guarded reads and, for each
4-byte-aligned float3 that is a unit vector and changes during the session, accumulates its dot product with the observed travel
direction (speed > 3). At WallRun end it logs the top aligned/opposite offsets (`wallrun_direction_scan_candidate`). No writes.
| Observation | Next |
| --- | --- |
| an offset with mean_dot >= 0.95 over varied directions | M3c: bounded write of that field toward a wall-relative stick direction |
| none | widen to the WallRun child objects already sampled (+0x128 child) or the Update locals |

## M3b result (log `MSM2-WS-v0.1c48-20261105-011724.log`, not committed) — VERIFIED-RUNTIME scan output
In 5 of 7 sessions the top aligned candidate is WallRun.this+0x240 (identical copy at +0x24C): unit vector, mean dot with observed
travel 0.72-0.84, values are axis-aligned world vectors ((0,0,-1),(0,1,0),(1,0,0),(0,0,1)), only 2-11 value changes per session.
min_dot -1 at reversals = lag between the stored direction and finite-difference travel.
Consequences (OBSERVED-CORRELATION / static consistency):
- +0x250 (c47/c48 scalar) is the Y component of the +0x24C vector; V8 consumers `y > 0.5` / `y > 0.71` therefore read as
  "direction mostly upward" thresholds, consistent with separate up vs sideways WallRun behaviour (HYPOTHESIS).
- the vanilla direction is quantized to axes, matching the owner's "only 4 directions" report.
A second pair at +0x274/+0x280 behaves similarly with fewer unit samples (not used).

## M3c (bounded write, one change): tilt a horizontal +0x240/+0x24C direction 20deg toward world up
In the existing pre-original Update callback (same gate as c48: verified WallRun only, F8/script-disable stop it). Writes only when
both copies are the same unit horizontal vector; cap 200000 writes per process; the game's own value resumes after writes stop.
Log key: `wallrun_m3c_heading_tilt`.
| Observation | Next |
| --- | --- |
| sideways WS travels ~20deg upward | direction field controls travel: next drive it from the stick (wall-relative) |
| travel unchanged, counter > 0 | field is a mode/label, not the travel vector; look at +0x274/+0x280 or the Update locals |
| counter 0 | shape gate never met; read skip counters |
| unstable / crash | F8, revert |

## M3c result (owner + log `MSM2-WS-v0.1c48-20261105-013952.log`, not committed) — VERIFIED-RUNTIME
Owner: sideways WS travelled diagonally upward. Log: writes=112, skipped_not_horizontal=2211 -> once written, the game kept the tilted
value (it does not recompute the pair every frame; it changes it on its own events). So +0x240/+0x24C is the WallRun travel direction
and is writable state (VERIFIED-RUNTIME for this build, bounded experiment). Reversal-from-stop downward pull still present (not caused by M3a).

## M3d (one change): stick/WASD-driven direction
Every pre-original Update with a non-neutral stick (XInput pad 0, deadzone 0.35) or WASD: d = normalize(sx*k*lateral + sy*wallUp), wallUp =
world up projected on the wall normal (+0x398), lateral = wallUp x normal; k (screen-right sign) calibrated once per WallRun owner from
vanilla's own horizontal choice for a clearly sideways input. Neutral input: no write. Same gate/cap/F8 as M3c. Log: `wallrun_m3d_stick_drive`.
| Observation | Next |
| --- | --- |
| travel follows stick in any direction, reversal ok | M3 core done; then facing/animation for diagonals, tuning |
| follows only while stick held steady | game overwrites on stick changes -> also write after original Update |
| lateral mirrored | calibration sign wrong: inspect lateral_sign / uncalibrated counters |
| unstable | F8, revert |
