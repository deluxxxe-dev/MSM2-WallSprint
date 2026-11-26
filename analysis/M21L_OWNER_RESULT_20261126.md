# M21k owner result and M21l baseline fixes (2026-11-26, build 2a099bc, log MSM2-WS-v0.1c48-20261126-195525)

Inspected in full: the log (11179 lines). No video. Experiments confirmed OFF (`WS_EXPERIMENTS edge_wrap=0 wr_exit_assist=0
face_relevel=0`).

## Owner report
- Ground -> wall WS: the hero floats and the angle changes.
- WR -> WS: same as before; going from the crawl into WS again freezes; A makes the hero stand.

## Evidence (VERIFIED-RUNTIME)
- Freeze: in a hold the game was in its crawl/wall state 0xB465FB0 (crawl 0xB463E40 + RT) and asked for WR; the face veto
  refused it continuously (`WS_FACE_WR_REFUSED ... state_before=...b5fb0` x40 from 10:56:38.413, same at 53.940) and the
  hero stayed frozen until B. Cause: the M20e veto assumes the game's ground stand (0xB4519A0) on the face.
- Crawl after WR -> WS: RT release -> WR exit 0xB465AB0 -> crawl idle 0xB463C10 (0.4 s later). Crawl idle under the face
  gravity is left only on a player input: A -> 0xB44F0A0 -> ground stand (2/2 in log 192703), B -> 0xB44F6C0 (drop; every
  0xB44F6C0 came 0.1 s before a B cancel: 195525 43.547/03.252, 183231 09.434), stick -> crawl 0xB463E40, RT -> 0xB465FB0.
  (The earlier reading "spontaneous stand-up" was wrong: it was the B drop.)
- Ground entry float (10:58:19): 0.33 s after the WR request the game turned its run into the sprint jump 0xB454950
  (19.474) while the gravity was already the wall normal; the hero was thrown 6.5 m off the wall (plane_d 1.1 -> 4.9 -> 6.5,
  REAPPROACH 2.2/8.6 m) and its body turned in the air. The other 3 entries in the log landed on the plane within 0.13-0.37 s.
- The callback path can refuse non-WR states (crawl idle refused in log 185442; sprint jump refused in log 192703 with no
  jump state afterwards).

## M21l (Claude Code; INI code default 0, shipped 1)
- `WALLSPRINT_WS_CRAWL_WR`: in a hold the WR request is not vetoed while the game is in its crawl/wall family
  (0xB465FB0, 0xB463E40, 0xB463C10, 0xB4635E0, 0xB465AB0, 0xB4633E0); the old WR -> WS path takes over
  (`WS_FACE_WR_ALLOWED reason=crawl_state`). No freeze.
- `WALLSPRINT_WS_CRAWL_STAND`: in a hold, after 0.1 s in crawl idle 0xB463C10 the game alone gets one 60 ms A press
  (WGI reading, after every MOD read of A, so no MOD face jump), at most 3 per hold, 1.5 s apart (`WS_CRAWL_STAND
  event=inject_a`). HYPOTHESIS: A -> 0xB44F0A0 -> stand as with the owner's A; the state log will show it.
- `WALLSPRINT_WS_ENTRY_NO_JUMP`: from the entry start for 1.0 s (0.33 s rotation + 0.6 s of the hold) the sprint jump
  0xB454950 and the ledge take-off 0xB45E2B0 are refused (`WS_GROUND_ENTRY event=jump_refused`); B clears it.
- Checks: build PASS, test.sh PASS (97), package PASS (packaged INI CRAWL_WR=1, CRAWL_STAND=1, ENTRY_NO_JUMP=1, experiments
  0; script.disabled sha256 1a4ed1421a596a08d02998bab84f9e7d4575a2b572a909b3a97b0b47d4d198d3, DLL
  c842a1c31b5c1f6a0c03404b00fbac7c67b754af692e6127b438c04186d70244); smoke/hooks UNAVAILABLE (wine).
