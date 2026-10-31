# Result — WallSprint v0.1c40 continuous Request presence capture

2026-10-31. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**IMPLEMENTED / READ-ONLY CONTRACT PRESERVED / ASSEMBLY CHECK PASS / LOCAL FULL PIPELINE REACHED PACKAGE / PACKAGE POLICY FIX PUBLISHED / ACTUAL-GAME RUNTIME PENDING.**

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

## PACKAGE FAILURE FOUND ON OWNER UPDATE-WS RUN

The owner full updater run progressed through the existing build/test suites and failed at `stage=package`.
The direct failure was the stale root `README_TEST.md`: `package.py` requires the current `versionLabel` and `packageVersion` to occur in that file, while it still advertised v0.1c38r5 / 0.1.61.

A second stale package-policy assertion was also identified before requesting another owner rerun: the historical monolithic package implementation still required `WALLSPRINT_ENABLE=1` and `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=1`, while c40 intentionally requires both values to remain 0 and presence diagnostics to be enabled.

Fix:

- root `README_TEST.md` now documents only the v0.1c40 / 0.1.65 one-F11 video capture procedure;
- the old README is preserved unchanged as `docs/README_TEST_HISTORY_20261031.md`;
- the previous `source/tools/package.py` implementation is preserved byte-for-byte as `source/tools/package_base.py`;
- the new small `source/tools/package.py` wrapper fail-closed patches exactly one known historical policy assertion in memory: it now requires `WALLSPRINT_ENABLE=0`, `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`, and `REQUEST_PRESENCE_DIAGNOSTIC=1` before executing the otherwise unchanged package implementation;
- if the preserved base assertion shape changes, packaging aborts rather than silently changing policy.

The failed updater explicitly retained the previous active script and performed no deployment. The repaired package path still requires a fresh local `Update-WS.cmd` run before v0.1c40 is considered locally built/deployed.

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
- Owner `Update-WS.cmd` run reached package after the preceding build/test suites; package failed only on the stale README assertion observed in the supplied console log.
- The package-policy repair itself has not yet completed a fresh owner Windows/WSL full pipeline; rerun is required.

## REQUIRED HUMAN RUNTIME TEST

After a successful `Update-WS.cmd` build/deploy:

1. Start video recording before the capture.
2. Press F11 once.
3. During the following 30 seconds, demonstrate ground Sprint straight -> reverse -> neutral, then ordinary WallRun left -> right -> down in one continuous take.
4. Let capture close automatically. F8 is only an emergency stop.
5. Return the newest complete `MSM2-WS-v0.1c40-YYYYMMDD-HHMMSS.log` from `MSM2-WS\logs` together with the video.

ChatGPT should first verify `REQUEST_PRESENCE_ADMISSION admitted=1`, then correlate `REQUEST_PRESENCE_RECORD offset_ms_est=...` with visible actions in the video. `offset_ms_est` is approximate relative alignment, not an authenticated engine timestamp or state label.
