# Result — WallSprint v0.1c40 continuous Request presence capture

2026-10-31. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**IMPLEMENTED / READ-ONLY CONTRACT PRESERVED / C49 OWNER WINE FIX CONFIRMED / PACKAGE CONTRACT FIXED AGAINST ACTUAL SOURCE README PATH / FULL OWNER PIPELINE STILL PENDING / ACTUAL-GAME RUNTIME PENDING.**

The former six manually armed 2-second ground/WallRun phases are replaced by one video-correlatable capture. The user starts recording, presses F11 once, performs the whole movement sequence naturally, and the diagnostic closes automatically after 30 seconds or 8192 retained records. F12 no longer arms a second phase.

Captured `RCX/RDX/R8/R9`, return PC and thread token remain opaque bits. Ground/WallRun/action classification is performed afterward from the supplied video. `offset_ms_est` is approximate video-alignment metadata, not an authenticated engine timestamp or state label.

## CURRENT C40 IMPLEMENTATION

- `source/src/msm2/request_presence.cpp`
  - one capture/session;
  - 30-second maximum duration;
  - 8192-record cap;
  - F11 single start;
  - F12 legacy second-phase control rejected;
  - TSC/GetTickCount64 endpoint interpolation emits `offset_ms_est`.
- `source/src/msm2/request_presence.S`
  - exact-site preserving observer remains no-call;
  - record cap 8192;
  - leaf TSC reader outside the observer;
  - ten split unwind regions retained.
- `source/mod/WallSprint.ini`
  - `WALLSPRINT_ENABLE=0`;
  - `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`;
  - orientation/adhesion/movement writes OFF;
  - `REQUEST_PRESENCE_DIAGNOSTIC=1`.
- version: `v0.1c40`, package `0.1.65`.

## OWNER PIPELINE RESULTS

### Run 1 — package failure

The package step failed on a stale README/version assertion. The first repair updated the project-root README and converted the historical package policy to the c40 read-only policy, but the package path was not yet traced far enough to identify which README the historical packager actually reads.

### Run 2 — hooks failure

The next run stopped at the historical c49 test-only Wine deadline fixture. The exact failure was `now() >= windows[0].end` after one calculated `Sleep`.

The test-only fixture was changed to poll the same QPC-derived deadline with `Sleep(1)` and a separate 2000 ms wall-clock bound. No c49 identity/order/debt/DR/VCH assertion was relaxed and no WallSprint.dll/game hook source was changed.

### Run 3 — c49 fix confirmed, package path error exposed

The third owner run confirms the c49 repair: all six mixed SEARCH/downstream observations parsed and the pipeline continued through later hook/scanner suites, including c71.

It then reached `stage=package` and failed at `package_base.py` line 9:

`readme=(root/'README_TEST.md').read_text(); assert label in readme and version in readme`

Here `root` is `source/`, because `package_base.py` is under `source/tools/`. Therefore the actual checked file is `source/README_TEST.md`, not the project-root `README_TEST.md`.

The source README was still `v0.1c38r7 / package0.1.63`. This was the direct cause of the repeated package failure.

## CURRENT PACKAGE FIX

The canonical branch now contains all of the following:

- `source/README_TEST.md` updated to `v0.1c40 / 0.1.65` and the one-F11 30-second procedure.
- project-root `README_TEST.md` remains c40 as well.
- `source/tools/package.py` still preserves the historical monolithic implementation in `package_base.py`, but:
  - replaces exactly the stale active-config assertion with the c40 read-only condition;
  - after the historical packager completes, normalizes the generated `BUILD_MANIFEST.json` so the current package cannot claim historical native-source/orientation writes are active;
  - records `package_mode=request_presence_read_only`;
  - records native-source/orientation/adhesion/movement/Position/direct-Actor/camera/animation/physics/gravity writes as OFF for the current package;
  - records the one-F11/30-second/8192 Request presence diagnostic.
- `source/tests/request_presence_static.py` now checks both root and source READMEs plus the c40 package metadata contract.
- `source/tools/test.sh` runs that static contract before the long sanitizer/native suites, so this class of version/config/package mismatch fails immediately rather than at the final package step.

The historical analysis and proof fields remain in the source/package history; only current active-package state is normalized.

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

## VALIDATION STATUS

Confirmed from owner run 3:

- c49 Wine deadline stabilization passes.
- subsequent c49/c50/c51/.../c71 suites continue and pass/produce their expected UNAVAILABLE classifications.
- pipeline reaches package.
- package failure is exactly the stale `source/README_TEST.md` version assertion.
- previous active WS script is retained; no failed-run deployment occurred.

Checked in Chat after the fix:

- replacement `package.py` Python syntax: PASS.
- updated `request_presence_static.py` Python syntax: PASS.
- updated `test.sh` shell syntax (`bash -n`): PASS.
- GitHub readback required before final handoff.

A complete owner Windows/WSL run with the latest package fix has not yet completed. Do not claim deployment success until that run reaches the final full-pipeline PASS.

## HUMAN RUNTIME TEST

Only after a successful `Update-WS.cmd` build/deploy:

1. Start video recording.
2. Press F11 once.
3. Within 30 seconds: ground Sprint straight -> reverse -> neutral, then ordinary WallRun left -> right -> down.
4. Let capture close automatically. F8 is emergency stop only.
5. Return the newest complete `MSM2-WS-v0.1c40-YYYYMMDD-HHMMSS.log` together with the video.
