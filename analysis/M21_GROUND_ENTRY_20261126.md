# M20f owner result and M21 no-hop wall entry (2026-11-26, build eadb16e, log MSM2-WS-v0.1c48-20261126-141647)

Inspected in full: the log (23583 lines). No video.

## Owner report
- After entering a wall from the ground and cancelling before the stop, the camera and gravity returned to default but the
  face state stayed (as in failedA). Otherwise no problem.

## B cancel (VERIFIED-RUNTIME + VERIFIED-STATIC)
- Test 1: B at 05:18:22.339, 0.22 s into the hold (during the stop hop). 0.1 s later `restore reason=landed_game`: the
  setter rule "the game passes an up turning vertical while |vy| < 2 = landed" (active because the force is off during the
  B return) cut the 1 s return; gravity went to world up at once and the hero body stayed on the wall normal
  (Actor Up (-0.866,0.037,0.499)) for 12 s until 05:18:34.8. Tests 2/3 (B after 1.2/2.0 s) ran the full return
  (`cancel_done`) and the body returned to world up within ~1 s.
- Fix: the landed rule is disabled while the B return blend runs (`sbBlendQpc`), so the return always completes.

## WR request context (VERIFIED-RUNTIME)
- `WS_WR_CONTEXT` x4 (street WR requests): context +0x2C..+0x34 = a contact point next to the hero at the wall
  (e.g. -381.873, 1.950, 1903.418) and +0x38..+0x40 = the wall normal (-0.867, 0.000, 0.499) / (-0.865, -0.067, 0.498),
  equal to the WS normal used by the hold (n = -0.866, 0, 0.500). So the game's WR request already carries the wall.

## M21 (implemented by Claude Code; INI `WALLSPRINT_WS_GROUND_ENTRY`, code default 0, shipped 1)
- With no hold and no active WS, a WR request of the hero machine is converted on the game thread into an entry request:
  the contact point/normal are read from the context (unit, |n.y| < 0.3), the WR is vetoed (temporary [machine+0xE30]
  callback, restored after the call) and stays vetoed while the entry runs.
- The worker then rotates the substituted hero gravity up from world up to the wall normal over 0.25 s (smoothstep,
  `gbState 4`), then starts the face hold (plane = contact point, normal = wall normal) without the stop hop
  (`WS_GROUND_ENTRY event=hold_no_hop`). B during the rotation cancels it. The same path applies when the WR request comes
  while airborne (entry from the air).
- This replaces the vanilla WR-based WS start while the switch is on. HYPOTHESIS to test: the game keeps its ground
  locomotion while the gravity rotates and the hero ends standing on the face without a hop.
- Logs: `WS_GROUND_ENTRY event=request|start|hold|hold_no_hop|refused`, `WS_WR_CONTEXT why=ground_entry`.
- Checks: build PASS, test.sh PASS, package PASS (script.disabled sha256 678be590b82fe971a5096435b9b8d4a608671c72f6ab8b9b114033ab61182123, DLL 8f8e2c70ba878d2454ccb3f514a18ccad6989a28357bb60d81ea4290516d3fe1; packaged INI has
  GROUND_ENTRY=1, NO_WR=1); smoke/hooks UNAVAILABLE (wine). Tree e3fbb7f348fee32816fdb85805fd5b1ba5fb7b43.
