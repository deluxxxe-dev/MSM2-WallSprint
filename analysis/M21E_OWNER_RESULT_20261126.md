# M21d owner result and M21e LB edge wrap (2026-11-26, build e42c84d, log MSM2-WS-v0.1c48-20261126-170335)

Inspected in full: the log (7549 lines). No video. Owner tested only outer corners.

## Owner report
- The wrap basically failed. While B was held, the dodge sometimes played at the press (gravity/camera kept).
- Request: use LB instead of B as the wrap button.

## Evidence (VERIFIED-RUNTIME)
- 6 edge walk-offs: 3 `skipped reason=b_not_held`, 3 `request`:
  - 07:43:52.093 speed 5.6, travel dir (0.929,0.370,0) used as the new normal; the real face (from a later WR context
    1.4 s after) was (0.937,0,-0.349): the vertical part of the travel direction tilted the guess.
  - 07:43:59.679 speed 13.7: normal (-0.348,0.078,-0.934) was right (90 deg turn) but the carried 13.7 m/s moved the
    hero 4.7 m past the edge before the hold started (standoff 4.689); it then fell 78 m (`original_plane_d=-78.3`).
  - 07:44:59.908 speed 2.1, dir mostly downward -> normal (-0.178,-0.853,-0.490) (a ceiling): wrong.
- The B dodge is the game's own reaction to the B press (B is its dodge button); a long press cannot avoid it.

## M21e (Claude Code)
- LB (WGI LeftShoulder 0x400) replaces B as the wrap button. B keeps only the tap cancel (< 0.3 s).
- With LB held at the walk-off release, the in-plane carry stops (only the normal part stays), so the hero stays at the
  edge instead of flying past it.
- The wrap normal is snapped: travel mostly up (y > 0.7) -> world up (roof); mostly down -> no wrap (`reason=downward`);
  otherwise the horizontal part of the travel direction (building corner).
- A wrap hold accepts the real face from the game's WR context at once (no 1 s settle), so a snapped guess is corrected
  by the measured wall normal; a WR back onto the face just left is refused for 1 s after any transfer.
- HYPOTHESIS: buildings whose corners are not 90 deg rely on that WR correction.
- Checks: build PASS, test.sh PASS (90), package PASS (script.disabled sha256
  594b73ce60b1ecbf453efd138f5a0886ed9ae92132dce0e08f20515140463f2f, DLL
  d2e82c3c289705fdce2e4bebb9743f95afa71afb8fa3f6792ab1344498f9fb2a); smoke/hooks UNAVAILABLE (wine).
