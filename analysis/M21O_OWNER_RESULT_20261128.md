# M21o owner result and first-entry diagnostic (2026-11-28)

Inspected: log `MSM2-WS-v0.1c48-20261128-192854.log` (all 8736 lines), videos `2-failed.mp4` (4.8 s, frames every 0.25 s)
and `2-Success.mp4` (4.1 s, frames every 0.25 s).

## Owner result (M21n)
- Test 2 (ground -> wall): the first try after loading floated (vanilla WR pose, diagonal, then fell onto the face
  upright); after test 4 the same test succeeded (standing run on the face). OK on retry.
- Test 4 (swing -> wall): OK, the swing switches to the wall without releasing RT.
- Owner: the successful ground -> wall switch is the reference look for the corner wrap (recorded in FEATURE_ROADMAP).

## Cause of the first-try float (VERIFIED-RUNTIME, log)
- Until the first WR exit the hero gravity block is unknown: `WS_GRAVITY_BLOCK` search `not_found bases=4881`,
  `hero_block=0x0`. Without the block no entry is armed, so the wall contact at 19:29:13.534 went 0xB4519A0 -> WR with
  no `WS_GROUND_ENTRY`, i.e. the vanilla WR (old WS path).
- `WS_SET_GRAVITY_HERO_ID` (first non-vertical up in the setter) identified the block at the WR exit (19:29:16.368);
  every later entry in the log worked.
- Across 5 logs the block had no constant offset from the hero/Transform pointers (OBSERVED-CORRELATION).

## M21o change (read-only diagnostic, no behaviour change)
At the first `WS_SET_GRAVITY_HERO_ID` per hero, a low-priority thread reads the hero, its Transform and the hero's
traversal machine (0x2000 bytes each) and the objects they point to (0x800 bytes each) and logs every pointer that
equals the block or points up to 0x800 bytes before it: `WS_GRAVITY_CHAIN root= off1= [off2=] delta= level=`, at most
30 hits, then `WS_GRAVITY_CHAIN event=done`. Two or more logs with the same root/offsets/delta give a fixed chain
that can find the block right after loading (next step), so the first WS works without a prior WR.

Workaround until then: after loading or changing hero, do one vanilla WR (run up a wall and let go) before WS.
