# M21m owner result and M21n (2026-11-28, build 8cc6ff6, log MSM2-WS-v0.1c48-20261128-175726)

Inspected in full: the log (19503 lines) and both videos (3-Ground 5.7 s, 3-Swing 11.3 s; frames every 0.25 s).

## Owner report
- Still floating; when the floating dash stops the hero crawls and then jumps automatically to stand (unnatural). Owner's
  guess: the floating part is WR; this was fine up to about M21.
- Swing -> WS became hard: before, touching a wall while swinging started WS at once (like the vanilla WR); now only after
  releasing RT.

## Evidence (VERIFIED-RUNTIME)
- Floating dash = the old WR-based WS (owner's guess is right): tests 1-3 started from 0xB4519A0 -> WR 0xB467FD0 (`armed`,
  old path), test 4 from the crawl. 0xB4519A0 is also the game's ordinary standing idle on the street; M21h added it to the
  "no entry from a wall state" list, so every run started from standing next to a wall took the vanilla WR (regression
  since M21h). The stop then went WR exit -> crawl idle -> injected A -> 0xB44F0A0 (and once the MOD face jump) -> stand:
  the automatic jump the owner saw.
- 0xB454950 is the web swing (RT held, lasting 2-4 s: 20.630-23.283, 04.434-06.535, 07.604-11.477), not a sprint jump.
  The M21m deferral refused every WR during the swing (`deferred reason=game_jump` from :10.417) -> no entry until RT was
  released.
- Entries from ground run / air worked (tests 5-14 reached the stand state 0xB4519A0 or ground 0xB44E0B0).

## M21n (Claude Code)
- The entry block covers only 0xB465FB0 and WR 0xB467FD0 (standing idle removed).
- The swing deferral is removed. If the entry starts while the game is in the swing 0xB454950, the game gets RT = 0 (after
  the MOD's own RT read) while it stays in the swing, at most 1.2 s (`WS_GROUND_ENTRY event=swing_end`): the swing ends as
  with the vanilla WR start.
- `WALLSPRINT_WS_CRAWL_STAND` shipped 0 (automatic A in crawl idle). The A instead of WR in the crawl (M21m) stays.
- Checks: build PASS, test.sh PASS (99), package PASS (script.disabled sha256
  a0fd026d4779366c97c806ed24076222675daa656918930b0ab5d47d3a06edae, DLL
  65796853fb3d4e08ec2d1fe2cc4754dcad8b2de5111b0ed7854470c5db361495); smoke/hooks UNAVAILABLE (wine).
