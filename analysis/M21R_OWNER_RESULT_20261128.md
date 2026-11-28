# M21r owner result for M21q (2026-11-28)

Inspected: log `MSM2-WS-v0.1c48-20261128-204631.log` (all 4979 lines). Owner: no float; test 2 first entry and every
test 4 entry had no camera roll.

## Block discovery (VERIFIED-RUNTIME)
`fast=1` found the block 0.0 s after the load (20:47:12.409... hero_epoch=3) and 0.1 s after the reload (hero_epoch=5).
All 11 entries started as ground entries. `fast=2` and `fast_miss` were not needed.

## Camera (VERIFIED-RUNTIME)
- Load: entry 1 ran with `manager=0x0`; the caller was selected only after the hold (in the air). Entry 2 and 3 engaged.
- Reload: the camera caller changed (raw 0x2a699848ff0 -> 0x2a699336f80); the old manager was dropped and the new one
  never passed the live check; entries 4-11 all ran with `manager=0x0`, `cam_f_read=0`.
- The M21q fallback (single caller, unit forward at +0xD34) never fired, so the forward block is not valid either while
  the hero is on the ground/wall after a load (OBSERVED-CORRELATION).

## M21r change
During a gravity hold, when no manager passes the live check, the latest recorded caller of the camera function
(`ws_camref_raw`) is selected directly (`WS_CAMERA_SELECT ... distance=-1.00`), then engaged as before.
