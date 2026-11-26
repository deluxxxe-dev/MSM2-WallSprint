# M20d owner result and M20e WR veto (2026-11-26, build 28d6d23, log MSM2-WS-v0.1c48-20261126-133537)

Inspected in full: the log (16398 lines) and failedA.mp4 (8.2 s, 3 frames/s over the whole duration).

## WR start on the face (VERIFIED-RUNTIME)
- `WS_WR_REQUEST_HOOK installed=1` (exact bytes). 9 `WS_WR_REQUEST` lines; 6 of them while the hero stood on the face
  (hold=1, face_state=pinned, stick up, RT 1.0, move_dir_up 0.997-1.0, speed 2.0-2.7 m/s at the up-sprint start),
  each followed by `WS_FACE_WR_START` and a WallRunUp session: the start is reproducible at the up-sprint start.
- Every WR start (also the 3 normal WS starts from the street, hold=0): machine 0x28a08cf0ef0 (vtable RVA 0x85EA4A8, the
  hero traversal machine), requested/new state descriptor 0x7ff6eee47fd0 = RVA 0xB467FD0 (module base 0x7ff6e39e0000),
  callback [machine+0xE30] = 0, result 1. States before on the face: RVA 0xB465FB0 (state object vtable RVA 0x140E1C0).

## Owner report: A could not cancel the face state (video + log)
- After the test the owner pressed A repeatedly to leave the face state; each A made a WallSprint face jump
  (`WS_FACE_JUMP_TRACE` 04:46:24.968, 04:46:29.405) and the hero stayed on the face. By the owner decision of 2026-11-08
  the face gravity is kept until a new WS or B; A on the face is the face jump. Two walk-off releases followed (04:46:31.198
  fell 48 m along the face gravity until a contact; 04:46:33.800). The video shows the dodge/jump poses on the face and the
  camera returning to a world orientation. Point-zipping to a pole/corner ended it (a new traversal state).

## M20e (implemented by Claude Code)
- While the hero stands on a face in the hold (pinned, not face air, not stop hop, not released, no active WS) and only
  for the hero machine (vtable RVA 0x85EA4A8) with an empty [machine+0xE30], the detour places a veto callback in
  [machine+0xE30] for exactly one original call of 0x36825E0 and restores it afterwards (only if still ours). The
  callback returns false only for the requested state RVA 0xB467FD0 (WR) and returns the game's own `allow` otherwise.
- Static basis: 0x36825E0 calls [machine+0xE30](owner, current, requested, context, allow) when non-null and, on the path to
  the transition at 0x36827AC, skips it when the result is 0 (0x368279A test r14b / je 0x3682AA3). The other path (site
  0x3682A56) is not proven to honour it: `WS_FACE_WR_VETO_IGNORED` is logged if a WR Init still happens after a veto.
- INI `WALLSPRINT_WS_FACE_NO_WR` (code default 0, shipped 1), cap 2000 vetoes, F8 makes the detour a pass-through.
  Logs `WS_FACE_WR_REFUSED` (<= 40).
- Checks: build PASS, test.sh PASS (85; static contract: original-first, the only store is the temporary empty-slot
  callback, veto limited to hero machine/WR state/pinned face), package PASS (script.disabled sha256
  3548cc20f258fdcd0855fc0dea72451e516c11abd200178a5bddab8917f16250, DLL
  ebf28bf0733c4fd04d54c1659d04adf2bb98d0009063075a463d050029b2ed04, packaged INI NO_WR=1); smoke/hooks UNAVAILABLE (wine).
  Tree bd6710373b01bb7464d88cba9adfb5e26b0055e6.
