# M21p first WS after a reload (2026-11-28)

Inspected: log `MSM2-WS-v0.1c48-20261128-194655.log` (all 6391 lines; owner: only test 4, the reload, floated).
Cross-check: every uploaded owner log that contains `WS_GRAVITY_BLOCK found` or `WS_SET_GRAVITY_HERO_ID`.

## Timeline (VERIFIED-RUNTIME, log)
- 10:47:26 load, 10:47:45 hero change: the scan found the block at 10:47:51 (`hero slot=0x80 slot2=0x378`); ground
  entries at 10:48:01 and 10:48:22 started and held normally (tests 2 and 3 OK).
- 10:48:39.397 reload (`hero_epoch=5`, new hero/Transform). No `WS_GRAVITY_BLOCK found` or `not_found` until the
  first WR: at 10:48:49.131 the state went 0xB4519A0 -> WR 0xB467FD0 without `WS_GROUND_ENTRY` (float).
- 10:48:52.024 `WS_SET_GRAVITY_HERO_ID block=0x1b86c3145f4` at the WR exit (0xB465AB0). The same block had already
  received a vanilla setter call at 10:48:39.292, so it existed from the reload on.
- Cause: the slow scan (transform root first, 1 ms sleep every 64 bases) was still running after > 10 s. A scan that
  finishes after the setter identification stores and logs nothing, which matches the missing lines.

## M21o chain scan result (VERIFIED-RUNTIME, one sample)
`WS_GRAVITY_CHAIN root=hero off1=0x68 off2=0x298 delta=0x5f4` and `off1=0x80 off2=0x328 delta=0x5f4` (hits=2).
Both point at block-0x5F4, the same +0x5F4 offset as the known `gbChain` (owner+0xF0 -> +0xF8 -> +0x5F4).

## Cross-log check (OBSERVED-CORRELATION)
In all uploaded logs, whenever the scan found a block through hero+0x80 -> +0x378 -> gbChain and the setter later
identified the hero block for the same hero (no hero/Transform change in between), they were equal: 23 of 23, no
mismatch. The other scan paths (hero+0x968, transform slots) occurred only 5 times.

## M21p change
`gbFastPath`: while no block is known (and no hold is active), every worker tick reads hero+0x80 -> +0x378 and runs
`gbChain` (vanilla matrix required, unchanged). Found -> `WS_GRAVITY_BLOCK found ... fast=1`. The slow scan and the
setter identification remain as fallbacks; the setter identification still overrides the stored block.

Expected: the block is known within ~0.1 s after a load/reload/hero change, so the first wall contact uses WS.
Check in the next log: `fast=1` shortly after each `Hero/Transform identity changed`, and the later
`WS_SET_GRAVITY_HERO_ID` shows the same block.
