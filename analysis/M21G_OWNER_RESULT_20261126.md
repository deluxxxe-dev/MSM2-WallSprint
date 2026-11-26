# M21f owner result and M21g reset / state diagnostic (2026-11-26, build a9662a7, log MSM2-WS-v0.1c48-20261126-173439)

Inspected in full: the log (9570 lines) and all 3 videos (4-incorner_failed 2.2 s, 4-outcorner-failed 8.5 s,
4-outcorner-loof-failed 10.1 s; frames every 0.25 s over the whole duration).

## Owner report
- No visible wrap. Toward the roof the gravity changed in the end, unclear whether by wrap or by landing on another face.
- Proposal: vanilla wall crawl goes around corners; use it as a reference.
- Request: reset the orientation automatically on fast travel / hero change.

## Evidence (VERIFIED-RUNTIME)
- No freeze; LB was hidden 25 times (`WS_LB_HIDDEN`). Entries worked from the first try (stale-arming fix).
- 14 `WS_FACE_EDGE_WRAP event=request` and the matching `face_transfer` holds (e.g. around one building
  (1,0,0) -> (0,0,1) -> (-1,0,0) -> (0,0,-1) -> (1,0,0); roof (0,1,0) three times). 12 `skipped` (9 lb_not_held,
  1 downward).
- But every request came 0.23-0.36 s after the walk-off release, when the hero was already in the air: videos show the
  game's own ledge jump/vault at the edge (outcorner 0.5-1.0 s, roof 0.5-1.0 s), then a flight, then a landing on the
  next face. After several transfers the hero fell 8-60 m before contact (`land reason=contact original_plane_d` -8.5,
  -10.2, -16.6, -60.5): the guessed plane through the release point is not reliable (setbacks/non-box geometry).
- Inner corner (video incorner_failed): the hero stops against the wall ahead; no WR request/transfer was logged there.
- CONCLUSION: wrap after the walk-off cannot look like walking around the edge. It needs the real geometry before the
  edge (the game's own surface probe, as the vanilla crawl uses) and the ledge vault kept from starting. UNKNOWN: which
  state/descriptor the vault is and which probe the crawl uses.

## M21g (Claude Code)
- Reset: a new hero/Transform ends the face state without writing into the old gravity block; a jump of more than 40 m
  between worker ticks (fast travel/teleport) ends it with the vanilla gravity written back (`WS_FACE_RESET`).
- Read-only diagnostic `WS_FACE_STATE_CHANGE`: hero machine state changes seen in the request routine (descriptor and
  vtable RVAs, hold flag, face state, LB, stick, RT), 400 per session, also outside holds for the vanilla crawl
  reference.
- Checks: build PASS, test.sh PASS (92), package PASS (script.disabled sha256
  1fbc22fe38ea91bf968cca59761a2fc896a9a88c7ecc9260f635e69ebb7f6ea5, DLL
  508bcdb34860edc3df8e9ab3f6163c4ab066aeb1a55fe3f4d807ca1cc5537889); smoke/hooks UNAVAILABLE (wine).
