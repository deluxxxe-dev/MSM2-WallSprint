# M21q owner result for M21p (2026-11-28)

Inspected: log `MSM2-WS-v0.1c48-20261128-200809.log` (all 5578 lines). Owner: tests 2 and 3 OK except the first try
of each had no camera roll; test 4 (reload) floated.

## Block discovery (VERIFIED-RUNTIME)
- Load (hero_epoch=3, 11:08:51.128): `fast=1` found the block 0.11 s later (11:08:51.241); the slow scan found the same
  block at 11:08:55. Entries 1-6 all started as ground entries (M21p worked for the load and the fast travel/hero change,
  which kept the same hero/Transform pointers).
- Reload (hero_epoch=5, 11:10:25.445): no `fast=1`; 11:10:34.528 0xB4519A0 -> WR (float); `WS_SET_GRAVITY_HERO_ID`
  at 11:10:36.122. The M21o scan again found `hero 0x68/0x298` and `hero 0x80/0x328` with delta 0x5F4, as in log
  20261128-194655 (2 of 2 reloads). Why hero+0x80 -> +0x378 -> gbChain fails after a reload is UNKNOWN.

## Camera (VERIFIED-RUNTIME)
- Entries 1 and 2 (11:09:20, 11:09:25) and entry 5 (11:10:04, first after the fast travel) ran with `manager=0x0`: the
  only caller (raw 0x241197fc7a0) failed the 30 m camera-position check. It was selected only while the hero was in the
  air (11:09:31, 11:10:11), after which entries engaged the WS camera (`WS_CAMERA_REF reengage reason=gravity_hold`).

## M21q change
- `gbFastPath2`: if the M21p path fails, accept block = [[hero+0x80]+0x328]+0x5F4 only when it equals
  [[hero+0x68]+0x298]+0x5F4 and the block has a finite unit up row. Log `fast=2`.
- `WS_GRAVITY_BLOCK fast_miss` (once per hero, after 2 s without a block): the raw values of all fast paths.
- Camera: during a gravity hold, when no manager passes the live check and there is exactly one recent caller with a
  unit forward, that caller is selected (logged as `WS_CAMERA_SELECT ... distance=-1.00`).
