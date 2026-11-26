# M21e owner result and M21f fixes (2026-11-26, build 7bca0b8, log MSM2-WS-v0.1c48-20261126-172006)

Inspected in full: the log (4446 lines) and both videos (failed 10.0 s, crush 5.3 s; frames every 0.25 s over the whole
duration).

## Owner report
- Basically failed: WS entry did not work well; the game froze at an LB press.

## Evidence (VERIFIED-RUNTIME)
- Lost entries 08:22:23.4-27.9: 4 `WS_GROUND_ENTRY event=request` (WR vetoed) without `start`. The hero had changed
  (hero 0x2a7b50b19c0 -> 0x2a7b5bb7700) and the gravity block was unknown until the first non-vertical up at 28.73
  (`WS_SET_GRAVITY_HERO_ID`). The worker returns early without a block but `geArmedNow` stayed true from before, so the
  callback kept refusing WR for entries that could not start. Matches video "failed" (wall contact without WS).
- Freeze 08:23:24.5 (video crush: the image stops at 2.0 s while the hero walks on a face n=(-1,0,0)): the log just ends,
  even the rumble lines; no walk-off, no wrap, no MOD event before it. HYPOTHESIS: the game's own LB action (abilities)
  under the rotated gravity; the MOD only read LB at a walk-off release, which did not happen.
- No edge reached in this log: no `WS_FACE_EDGE_WRAP` line, so the M21e wrap itself was not exercised.

## M21f (Claude Code)
- `geArmedNow` is cleared on every early exit of the worker (no hero / no gravity block); without a block the game's WR
  runs as before (the old WR path discovers the block).
- While a face hold or entry is active (`gbHoldA` or force on) LB is removed from the WGI reading the game gets (the
  game's pad path, M8); the MOD still reads it (`WS_LB_HIDDEN event=press`).
- Checks: build PASS, test.sh PASS (91), package PASS (script.disabled sha256
  94adebb9e3b48f91350cecf1e376c29f135e8296101802834d0cbfa4d48bb9b2, DLL
  6d7fb44f9ab363528d5096272d409273b46896d41ecfc10831c43b1156756c61); smoke/hooks UNAVAILABLE (wine).
