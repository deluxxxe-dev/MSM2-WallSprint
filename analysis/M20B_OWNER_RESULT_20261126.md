# M20b owner result (2026-11-26, build 6330ee6, log MSM2-WS-v0.1c48-20261126-125018)

Inspected in full: the log (6808 lines) and up.mp4 (3.4 s, 4 frames/s over the whole duration).
Owner: up, side and down all fine. Question: the face sprint feels slightly slower than the ground sprint.

## Face sprint (VERIFIED-RUNTIME)
- Down: three `WS_FACE_LOCOMOTION_DOWN` start/end pairs, no FALL_UNDO, no release. Up: no WR start this run (video: run up
  the face and natural stop). Side: run and stop.
- Speed from Hero positions (0.55 s samples): face sprint up 9.84-10.03 m/s, down 9.85-10.11 m/s, side 9.54-10.40 m/s;
  ground sprint at the end (y = 1.0) 9.75-10.12 m/s. The steady speeds are the same (~10.0 m/s). The impression of a
  slower run is not in the position data (likely the camera distance/angle on the face and the uniform glass surface).

## WR-start caller (VERIFIED-STATIC from the captured bytes)
- `WS_WR_CALLER_DUMP`: the function containing RVA 0x367E6AF is 0x367E5B0-0x367E700 (0x150 bytes, fully captured).
- It is a generic state transition helper: rcx (rsi) = state machine, rdx (r14) = requested state; it resolves the
  old/new state objects (calls through vtable +0x48/+0x58/+0x50 and 0x367E4B0), optionally calls a callback at [rsi+0x90],
  then calls the leaving object's vtable +0x50 (return address 0x367E6AF = the hooked WR Init call) and the entering
  object's vtable +0x58 (return address 0x367E6CE = the hooked WR Entry call), and returns a bool.
- So the decision to start WR is made by the caller of 0x367E5B0, one level up; the M19 diagnostic records only the
  immediate return address. A WR refusal needs that upstream caller (next evidence: a bounded return-address chain at the
  WR Init call while standing on the face).

## M20c (implemented by Claude Code, 2026-11-26)
- Read-only background scan at load: every direct E8/E9 rel32 call/jump to RVA 0x367E5B0 in executable sections
  (`WS_WR_TRANSITION_CALLER site_rva= op= func_begin= func_end= pdata=`, <= 64) and up to 0x200 bytes before each of the
  first 12 sites (`WS_WR_TRANSITION_BYTES`), then `WS_WR_TRANSITION_SCAN direct_sites=`. Purpose: find the caller that
  decides the WR start (phase B). If the helper is only reached indirectly, direct_sites=0 and a return-address chain is
  the next evidence.
- Checks: build PASS (prebuilt capstone), test.sh PASS (84), package PASS (script.disabled sha256
  e188ad83c1c5bc0febf08881672a83333b2acc88995058876df3b98919691261, DLL
  cb941c380bf494763c8b67579bcfa83d60f0a20b2dc0ad3c0cdf29fe13023d21); smoke/hooks UNAVAILABLE (wine). Tree f02f0e26fc2ba00cfb6c5896a77a81d7ab8bcf6c.
- Roadmap updated: `docs/FEATURE_ROADMAP.md` "Current roadmap (owner decisions 2026-11-26)".
