# M20 — the game's sprint toward the ground is not a fall (implemented by Claude Code, 2026-11-26)

Task: `docs/CODEX_NEXT.md` (M20). Evidence: `analysis/M19_OWNER_RESULT_20261126.md`.

## Change (source/src/NativeHooks/camera_ref.cpp)
- New `FaceLocomotionDown` (M16_FALL section). Per native frame while pinned on the face (not face air, stop hop, stop
  push or a released fall): the in-plane movement direction of the last 0.3 s (frames > 1 m/s), compared with world down
  projected on the face. "Locomotion down" = stick > 0.5, >= 3 moving frames, direction within 45 deg of world down,
  world-down speed <= 11.5 m/s (measured sprint ~10.2 m/s).
- Stop coasting: after locomotion down, with the stick released (the game's own sprint stop) the exemption stays while the
  world-down speed does not increase by more than 0.3 m/s per frame, for at most 1 s (a real fall gains ~0.4 m/s per
  16 ms frame).
- While exempt: the walk-off detector is not eligible (no release) and the fall guard does not undo world-down steps up
  to 11.5 m/s (and treats the frame as walking so the neutral-stick "no downward motion" rule does not stop the coast).
- Logs: `WS_FACE_LOCOMOTION_DOWN event=start|end speed= dir_cos= stick= limit=11.5` (<= 60) and
  `event=hold_end exempt_frames=` at the hold end.
- `WS_FACE_WR_START` now records in every state of an admitted hold (not only pinned), with `state=pinned|released|
  face_air|stop_hop|other`, `step_valid=` and `since_release_s=`. Still read-only; no WR refusal (M19 step 2 stays
  unimplemented).
- Unchanged: jumps, zips, camera, mapping, stop hop, WR hooks. Genuine falls: a walk-off from a side/top edge (movement
  not toward world down), a neutral stick from the start, or world-down speed > 11.5 m/s still release/undo as before.
- Known limit (HYPOTHESIS, untested): walking off the BOTTOM edge of a face while walking toward the ground is treated
  as locomotion until 11.5 m/s (~0.45 s at 26 m/s^2), so the game's world-down motion passes for that time before the
  fall rules take over.

## Checks in this cloud session
- `build_windows.sh` PASS (the capstone download step was skipped because the proxy refused codeload.github.com;
  the prebuilt `build/deps/capstone-5.0.6/capstone.lib` was used; every compile/link step ran).
- `tools/test.sh` PASS (84 PASS lines), including the new `tools/test_face_locomotion_down.py` (sprint start/steady/
  stop-coast exempt; acceleration, > 1 s, > 11.5 m/s, neutral stick, side walk-off, roof direction, 60 deg, floor and
  invalid input refuse; the guard keeps a 10.2 m/s step and undoes 12 m/s; without the gate the walk-off window releases
  on a sprint start, the measured failure) and the extended `tools/test_face_wr_start.py`.
- `smoke_windows.sh` and `test_hooks_windows.sh`: UNAVAILABLE in this container. Wine aborts at start-up
  (`free(): invalid pointer`) even for a minimal hello-world exe, independent of this change.
- `package.py` PASS: `source/output/MSM2-WS-v0.1c48/MSM2-WS-v0.1c48.script.disabled`
  sha256 7f08d8527113897957fc944441810b9af8813cc461855dfcee5b96e6fe132c71; DLL sha256
  82926f5ca919440a2afa476ce8edfb08d598de7f9e3b06b5ceb2c04e35ae4082 (cloud artifact; Update-WS rebuilds locally).
- Source tree a1cb4f2bd957b89d9f1cfa050cedacb80cf6a21b. Real-game acceptance: UNKNOWN until the owner run.
