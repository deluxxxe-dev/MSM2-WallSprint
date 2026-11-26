# M21c owner result and M21d edge wrap / B tap (2026-11-26, build 79bf0c4, log MSM2-WS-v0.1c48-20261126-162138)

Inspected in full: the log (9627 lines) and all 6 videos (little-cameraloatetion 3.6 s, 3-outcorner-1 7.3 s,
3-outcorner-2 6.1 s, 3-incorner-1 5.8 s, 3-incorner-2 8.3 s, 4 2.7 s; frames every 0.25 s over the whole duration).

## Owner report
- 2 (corner entry crawl/camera turn back): basically solved; once a small camera rotation.
- 3 (corners): no wrap at outer corners; the game's parkour ledge auto-jump or a fall instead.
- 4 (jump from WS): a short lag before the fall, hanging in the air.
- Decisions: at a face edge, B held = wrap, not held = jump as now. Only a short B press cancels WS/gravity/camera; a long
  press and release must not cancel. Update-WS should close a running Overstrike and continue.

## Evidence (VERIFIED-RUNTIME)
- 8 entries; 21 `decision=transfer` and 4 face transfers (tests 3, 4, 5, 7: normals changed 17-90 deg) ran without
  `ws_again`; 57 `refused_settling`. One `ws_again` (test 8) remained.
- Outer edge (07:26:00.645): walk-off release with v_plane 15 m/s; probe clean after 0.21 s; plane distance -0.36 m at
  0.23 s, -3.3 m at 0.47 s and -171 m at the end: the hero fell along the old face gravity past the edge (outcorner-1).
- Jump (07:28:30.27, video 4): takeoff at the press (4 ms), arc 0.65 s, apex 1.8 m, gravity 33 m/s^2, stick 0, so the
  hero rose and came back straight with no in-plane motion. The log shows no extra delay; the "hanging" is the 0.2 s
  near the apex without travel and the game's air pose (HYPOTHESIS; needs a video with stick input to compare).

## M21d (Claude Code)
- B tap: the cancel counts only presses released within 0.3 s (`wgiBTaps`); B held is tracked (`wgiBDown`).
- Edge wrap: during a walk-off, if within 1 s of the release the contact probe is clean (no wall pushback) and the hero
  is more than 0.3 m behind the face plane while B is held, a face transfer is requested with normal = in-plane travel
  direction and plane point = release position moved back by speed x detector delay (<= 0.2 s), the earliest possible
  edge (`WS_FACE_EDGE_WRAP event=request|skipped reason=b_not_held`). The worker rotates the gravity in 0.25 s (M21c).
  HYPOTHESIS: the face around a building edge is perpendicular to the travel direction (box corners).
- Update-WS: new Mod-Tools action `update-check` (environment/windows.ps1) closes the shared Overstrike at the exact
  game-root path (Stop-Process, waits up to 15 s, verifies) and continues; a running game/`Check for updates` or a
  foreign Overstrike is still refused without stopping anything. Backup/restore/install keep the non-stopping `check`.
  PowerShell contract test extended; not runnable here (no pwsh).
- Checks: build PASS, test.sh PASS (89), environment python tests PASS, test-workspace PASS, package PASS
  (script.disabled sha256 c2a668394a3da7cd6035f3670fcde4a76ac64cbf710a1b719a106115abda23fb, DLL
  a56b9e0782e0e2f171e8801298c556264fa194b0bfaa59c76d2ca694c40a34e3); smoke/hooks UNAVAILABLE (wine).
