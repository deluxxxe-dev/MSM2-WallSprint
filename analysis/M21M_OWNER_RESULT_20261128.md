# M21l owner result and M21m (2026-11-28, build cd8f678, log MSM2-WS-v0.1c48-20261128-173843)

Inspected in full: the log (15725 lines) and the video 2.mp4 (6.0 s, frames every 0.25 s plus full frames at 1.5/2.5/3.75/5.0 s).

## Owner report
- WS from the ground S and S from the crawl always float; after a jump from the crawl the hero stands and no longer floats.
- Update-WS with Overstrike open only closes Overstrike and stops.

## Evidence (VERIFIED-RUNTIME)
- The face plane is the same for all holds on this wall (p.n = -243.05..-243.08 for old-path, entry and jump-landing holds),
  so the floating is not a plane offset.
- Video 2 = entry at 08:41:23.478: the game had started its sprint jump 0xB454950 1.0 s before the WR request; pinned on the
  face it stayed in that jump for 2.8 s (air pose while moving on the wall), then 0xB45D6B0 -> 0xB44FD00 -> stand.
- S from the crawl (08:41:00.7-01.16): crawl -> 0xB465FB0 -> WR 0xB467FD0 allowed by M21l -> `ws_again`: the old WR-based WS,
  upright and off the wall.
- A from crawl idle -> 0xB44F0A0 -> stand again (08:41:07.886, 3/3 overall).
- No `WS_CRAWL_STAND`: the crawl-idle timer was shared by every machine of the hero class; filtered to the hero owner now.

## M21m (Claude Code)
- Hero-owned machine only for the state tracking (owner = hero).
- WR request in the crawl family during a hold: refused and the game alone gets one A press (<= 3 per hold, 1 s apart);
  WR is allowed only after 3 presses without leaving the crawl (no freeze).
- Ground entry while the game is in its sprint jump 0xB454950 / ledge take-off / 0xB45CBB0: the WR is refused without an
  entry for up to 1.2 s (`WS_GROUND_ENTRY event=deferred reason=game_jump`); the next WR request starts the entry.
- Update-WS.cmd first runs `close-overstrike-for-update.ps1` (exact <game>\Overstrike\Overstrike.exe only: close window, then
  stop, wait, report) and the updater prints its steps (checking processes, pulling, building, deployed/starting Overstrike).
- Checks: build PASS, test.sh PASS (98), test-workspace PASS, package PASS (script.disabled sha256
  08d3648fe26c1900a61fd786e897fdec44bee095aa125d7c6772a8f85a8af60b, DLL
  482a93259f60c355001f9bf5cbec85aede909b527515e5cb7f6981ab8974d0e0); smoke/hooks UNAVAILABLE (wine); PowerShell not run here.
