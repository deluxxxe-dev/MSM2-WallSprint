# M21b owner result and M21c face-to-face transfer (2026-11-26, build b645c77, log MSM2-WS-v0.1c48-20261126-151613)

Inspected in full: the log (9399 lines) and all 4 videos (4-swing-camerarotation 14.0 s, 4-swing-nosmooth 6.9 s,
4-jump-nosmooth 7.8 s, 4-swing-littlesmoother 9.5 s; frames every 0.25 s over the whole duration).

## Owner report
- Jump -> WS: camera and hero facing jerk.
- Swing -> WS: the camera comes close for a moment at the face landing. With the vanilla swing landing or swing-release
  jump motion before the WS it looks natural (suggested also for jumps). A landing motion that does not move while the
  position still slides looks wrong; it should not play on air entries.
- Once, a swing entry went into a crawl, the camera turned back to default in small steps and the hero showed the vanilla
  one-hand wall pose (video 4-swing-camerarotation 9.0-13.75 s).

## Evidence (VERIFIED-RUNTIME)
- 29 entries (ground and air, vy -24.5..+20 m/s): all reached `hold_no_hop` and `WALL_LANDED` 0.07-0.21 s later.
- B: 28 cancels; every block window ended without a new WS (`wr_vetoed` up to 288). Test-5 failure not reproduced.
- Same-face refusal worked (`same_face` 3-51 per hold). 2 false fall releases ended by contact in 9-23 ms.
- Test 21 (corner): 0.11 s after the entry on n=(0.937,0,-0.349) the game started WR on the adjacent face; the old WR path
  flipped the hold to n=(0.347,0,0.938) and back (`ws_again` x2) and restarted it with the stop hop 2.5 s later. Matches
  the crawl / camera turning back / vanilla pose of the video. Tests 23 and 29 (face not flat, plane moved 4-13.6 m along
  setbacks, falls at 11-22 m/s along the face) also ended in `ws_again` through the game's WR.
- Camera distance (0xd54, read only): ~8 m before the swing entry, 3.5-3.8 m from the entry on (test 11). The close-in at
  the landing is this distance change; no write path to it exists (UNKNOWN how to blend it).
- Landing animation/slide: no evidence in the log of which animation plays (UNKNOWN; needs an animation-state source).

## M21c (Claude Code)
- During a hold, a WR request on another face (normal > 20 deg or plane > 0.6 m away) is refused for the first 1 s of the
  hold (`decision=refused_settling`). After that, a near-vertical face (|n.y| < 0.3) becomes a no-hop face-to-face
  transfer: the hold ends (`face_transfer`) and the gravity rotates in 0.25 s from the current normal to the new one,
  then a new no-hop hold starts on the new plane (`WS_GROUND_ENTRY event=start ... source=face_transfer`).
  Other faces keep the game's decision. Same-face log budget is now 12 per hold.
- Open: camera distance blend at air entries, landing animation without movement, jump-entry facing/camera jerk.
- Checks: build PASS, test.sh PASS (88), package PASS (script.disabled sha256
  f34741d74abc2483008d87976aa55f242aff4e98f0798ca73e07a1ea27ad2dcc, DLL
  68db1e40e06704575aeb82cc51f950bee77fad7ea59fe7560a29ca0a6f4a399f); smoke/hooks UNAVAILABLE (wine).
