# M21h owner result and M21i (2026-11-26, build 0711d7e, log MSM2-WS-v0.1c48-20261126-183231)

Inspected in full: the log (13400 lines, 204 `WS_FACE_STATE_CHANGE`) and all 3 videos (2-failed 6.2 s, 2 17.0 s,
WR-WS_Failed 8.8 s; frames every 0.25 s over the whole duration).

## Owner report
- No visible wrap: plain falls (falling under the rotated gravity into another building goes behind its face), or the
  orientation changes while the hero flies out and turns in jerks. A transferred face can be tilted: re-level the ground
  every few seconds/metres during WS.
- WR -> WS still fails: stopping on the wall gives the crawl, a second WS then stops.

## Evidence (VERIFIED-RUNTIME)
- No `ledge_request`: in face holds the game never asked for the ledge state 0xB45E2B0 at an edge. Edges gave the MOD
  walk-off (4 wrap requests, 7 skipped without LB). After a wrap request with RT held the game asked for its jump
  0xB454950 then air 0xB45D6B0 (09:35:09.384 request -> 09.597 jump -> 10.575 air; also 25.362, 39.250): this threw the
  hero into the air. After the transfers the hero fell 4-14 m before contact.
- WR -> WS (09:34:00-10): WR 0xB467FD0 -> 0xB465AB0 -> crawl idle 0xB463C10 -> crawl 0xB463E40 while the old WS-stop hold
  started (with the stop hop): the crawl pose of the video.
- Same-face WR contexts during holds give the measured wall normal (cos 0.94-1.0), usable to level the face.

## M21i (Claude Code)
- Jump 0xB454950, air 0xB45D6B0 and ledge 0xB45E2B0 requests are refused for 1.5 s after any wrap request.
- Crawl states 0xB463C10 / 0xB463E40 / 0xB4635E0 / 0xB4633E0 are refused while WS is armed or a hold runs (new temporary
  hold callback, installed only when no other MOD callback is, restored after the call). `WS_FACE_STATE_REFUSED` logs
  each refusal; it also shows whether the callback sees these requests (HYPOTHESIS until logged).
- Re-levelling: a same-face context 1.5-20 deg away from the hold normal turns the hold normal halfway toward it, at most
  every 0.5 s, wall faces only (`WS_FACE_RELEVEL`).
- Open: falling through other buildings under rotated gravity (P5), real adjacent-face geometry for wraps.
- Checks: build PASS, test.sh PASS (94), package PASS (script.disabled sha256
  e42806ccf0262e30f6b3433d42043ab39d8a6177e3b525d48249b558b2b96fdb, DLL
  d36ca20c76aec42cfda6f3d22f5924542a2be4c328fc5c02f3780f7e8c0075b1); smoke/hooks UNAVAILABLE (wine).
