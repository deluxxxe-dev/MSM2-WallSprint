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
