# M21j owner result and M21k: back to the last accepted behaviour (2026-11-26, build e18274e, log MSM2-WS-v0.1c48-20261126-192703)

Inspected in full: the log (9036 lines) and all 3 videos (3-RT 4.3 s, 3-noRT 6.8 s, WRWS-failed 12.7 s; frames every
0.25 s over the whole duration).

## Owner report
- WR -> WS unchanged (crawl). Ground -> wall WS has a wrong angle and floats. Corner still fails; a tilted face does not
  recover. "It got worse" — check the versions that worked and move forward, even if it takes time.

## Version history and owner verdicts (this session)
| Build | Change | Owner verdict |
|---|---|---|
| 0db5961 M20b | locomotion toward the ground | all directions fine |
| 7bb3110 / eadb16e M20e/f | WR veto on the face, context capture | no WR posture on the face; ground S entry still off |
| f69964e M21 | no-hop ground/air entry from the WR context | ground entry basically success, air entry success, B cancel issue |
| b645c77 M21b | B block, same-face refusal, entry fall grace | swing entry natural, jump entry jerky, one corner crawl |
| 79bf0c4 M21c | other-face settle + face-to-face transfer | corner crawl basically gone; LAST ACCEPTED STATE |
| c13b6f7..e18274e M21d-j | B tap, B/LB edge wrap, LB hiding, ledge/jump/crawl refusals, re-level, WR-exit assist | every wrap test failed; WR -> WS froze/crawled; ground entry reported worse at M21j |

## Evidence from this log (VERIFIED-RUNTIME)
- WR -> WS: the forced hold started 0.06-0.1 s after the WR exit 0xB465AB0, the hop ran (d0 0.31-0.38), the game still went
  to crawl idle 0xB463C10 at the hop landing; it reached ground/stand only after a later A press (46.155 -> 0xB44F0A0 ->
  0xB4519A0; 08.473 the same). The assist did not help.
- Wraps: ledge_request at 0xB454950 (RT) and 3 LB walk-off requests; the transfers put the hero on guessed planes 0.4-3.5 m
  away; one WR correction jumped to a face 12.2 m behind the guessed plane (`decision=transfer cos=0.938 plane_d=-12.214`),
  i.e. the tilted/wrong faces of the videos come from the guessed geometry.
- 43 `ledge_skipped reason=downward` in 0.2 s: the hold callback runs on every request.
- No re-level event in this log.

## M21k (Claude Code)
- All experiments after M21c are switched off by INI (code default 0, shipped 0): `WALLSPRINT_WS_EDGE_WRAP` (LB wrap,
  LB hiding, jump refusals, carry stop, hold callback), `WALLSPRINT_WS_WR_EXIT_ASSIST` (forced hold + crawl refusal),
  `WALLSPRINT_WS_FACE_RELEVEL`. With them off the behaviour is M21c plus only these fixes: B tap-only cancel (owner
  request), stale entry arming cleared (lost entries), reset on hero change/teleport (owner request), no new entry from
  a running vanilla wall state (stuck hero), no transfer back to the face just left within 1 s, read-only state log.
- Corner wrap is not continued by guessing: it needs the real adjacent face. Next evidence (not implemented): the vanilla
  crawl corner turn 0xB4633E0 ends with the hero transform up = the new face normal; read that geometry from the crawl
  instead of guessing (a separate read-only step after the owner confirms the restored baseline).
- Checks: build PASS, test.sh PASS (96), package PASS (packaged INI: EDGE_WRAP=0, WR_EXIT_ASSIST=0, FACE_RELEVEL=0,
  GROUND_ENTRY=1; script.disabled sha256 fd3ad075aac8b05903e395f7d8017aecc9595daf53e3dc9d5ec0fbb1f9917c8a, DLL
  1b614ed6e24951f2b3c9b03a914bc53fc78d94a0c4b8ccb5f036ec1bccb1d46b); smoke/hooks UNAVAILABLE (wine).
