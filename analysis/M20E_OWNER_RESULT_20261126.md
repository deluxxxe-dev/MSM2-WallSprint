# M20e owner result and M20f (2026-11-26, build 7bb3110, log MSM2-WS-v0.1c48-20261126-140033)

Inspected in full: the log (11342 lines). No video.

## Owner report
- Up/side/down on the face: no problem (the WR posture no longer appears).
- Entering a wall from the ground sprint (S): the angle/position are off the wall as before (the vanilla WR-based WS).

## WR veto (VERIFIED-RUNTIME)
- `WS_FACE_WR_REFUSED` from 05:05:04.225: while RT and the stick stayed toward the roof the game re-requested WR about 3 times
  per frame (state before = after = RVA 0xB4519A0, result 0); no `WS_FACE_WR_VETO_IGNORED`, no WR session on the face: the
  callback path is the one the game uses here.
- Bug found: the veto cap (2000) was global for the process; at ~180 requests/s it runs out after ~11 s of up-sprint in
  total, after which WR starts again.
- The ground-sprint entry (street WR request, hold=0) is not vetoed by design; it is the normal WS start (phase C/D).

## M20f (implemented by Claude Code)
- Veto cap per hold (200000), reset at the hold end with `WS_FACE_WR_REFUSED event=hold_end vetoes=`.
- Read-only capture of the WR request context (4th callback argument, 0x140 bytes) for the first 12 WR requests, both
  vetoed ones on the face and normal ones from the street: an observe-only callback (returns the game's own decision
  unchanged) is placed in the empty [machine+0xE30] slot of the hero machine for one call at a time. `WS_WR_CONTEXT` /
  `WS_WR_CONTEXT_BYTES` (hex + float view). Purpose: find the wall contact point/normal the game uses when it requests WR
  from a ground sprint, for the no-hop ground -> wall entry (phase C/D).
- Checks: build PASS, test.sh PASS, package PASS (script.disabled sha256 ff1cb637b81c15a097ad59beaa251b0fcd6bf0cf7dac43804cab69b5ea2b11d8, DLL 088139a4890ed21114296edc1525478e3ee70cf712c5b425d777d15d734c811a); smoke/hooks UNAVAILABLE (wine).
  Tree c6031adf0ec71d6aab6d0979cfed3464185d41b3.
