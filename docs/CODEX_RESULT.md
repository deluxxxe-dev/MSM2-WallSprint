# Result — WallSprint v0.1c40 continuous Request presence capture

2026-10-31. Implementation branch: `chat/request-presence-video-capture`, based directly on `codex/v01c20-edi-value-provenance` head `c6ce7ca5754a4ade92f9fa119fff09a8b894c0c9`. No main merge.

## STATUS

**IMPLEMENTED / READ-ONLY CONTRACT PRESERVED / ASSEMBLY CHECK PASS / FULL WINDOWS BUILD PENDING / ACTUAL-GAME RUNTIME PENDING.**

The former six manually armed 2-second ground/WallRun phases are replaced by one video-correlatable capture. The user starts recording, presses F11 once, performs the whole movement sequence naturally, and the diagnostic closes automatically after 30 seconds or 8192 retained records. F12 no longer arms a second phase.

This does not establish any engine meaning for the captured arguments. `RCX/RDX/R8/R9`, return PC and thread token remain opaque captured bits. Ground/WallRun/action classification is performed afterward from the supplied video, not asserted by the hook.

## CHANGED

- `source/src/msm2/request_presence.cpp`
  - one capture/session;
  - 30-second maximum duration;
  - 8192-record cap;
  - F11 single start, F12 rejected as legacy control;
  - capture start/end TSC plus `GetTickCount64` span used to emit `offset_ms_est` for post-hoc video alignment;
  - zero-hit, overflow, contention and close reason remain explicit.
- `source/src/msm2/request_presence.S`
  - record bound increased to 8192;
  - added a leaf TSC reader used outside the hooked callback;
  - observer register/RFLAGS preservation, no-call callback path and ten split unwind regions remain unchanged.
- `source/mod/WallSprint.ini`
  - c40 is intentionally a dedicated read-only diagnostic package: WallSprint enable/native-source/orientation/adhesion/movement writes are all OFF and `REQUEST_PRESENCE_DIAGNOSTIC=1`.
- `source/mod/presets/request_presence_read_only.ini`
  - documents the same one-F11 capture procedure.
- `source/tests/request_presence_static.py`
  - asserts the new bounds, one-button/video-alignment contract and read-only package configuration.
- `source/include/version.hpp`, `source/mod/info.json`
  - v0.1c40 / package 0.1.65.

## SAFETY / ADMISSION

Unchanged production admission requirements remain in force:

- exact Spider-Man2.exe build gate;
- RequestSimpleMotion RVA `0x11A1BD0`, exact function span and SHA256;
- exact leading instruction span and `.pdata` identity;
- validated MinHook trampoline and relay;
- observer unwind metadata validation;
- callback performs no allocation, lock, I/O, captured-pointer dereference, argument write or request suppression;
- F8/script shutdown closes the capture gate before hook shutdown.

No movement, Position, input, adhesion, animation or camera implementation was added.

## AUTOMATED / STATIC VALIDATION

- `request_presence.S` was cross-assembled as Windows x86-64 COFF with clang targeting `x86_64-w64-windows-gnu`: PASS.
- Resulting object retained `.pdata` size `0x78`, i.e. ten 12-byte runtime-function records for the observer/restore split: PASS.
- Repository diff from the c39 presence baseline is limited to the continuous-capture implementation, diagnostic configuration, regression contract and version/result metadata.
- Full repository Windows DLL build/test/package pipeline is unavailable in this chat execution environment because the repository's configured Windows cross-toolchain is not installed here. It must run through the existing local `Update-WS.cmd` workflow before deployment.

## REQUIRED HUMAN RUNTIME TEST

1. Run the normal WS updater/build path and install v0.1c40.
2. Start video recording before the capture.
3. Press F11 once.
4. During the following 30 seconds, demonstrate the relevant ground Sprint and ordinary WallRun movements in one continuous take. No F11/F12 presses between actions.
5. Let capture close automatically. F8 is only an emergency stop.
6. Return the newest complete WallSprint runtime log together with the video.

ChatGPT should first verify `REQUEST_PRESENCE_ADMISSION admitted=1`, then correlate `REQUEST_PRESENCE_RECORD offset_ms_est=...` with visible actions in the video. `offset_ms_est` is an approximate relative alignment value derived from the capture's TSC/GetTickCount64 endpoints; it is not an authenticated engine timestamp or state label.
