# Result — WallSprint v0.1c40 continuous Request presence capture

2026-10-31. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**IMPLEMENTED / READ-ONLY CONTRACT PRESERVED / ASSEMBLY CHECK PASS / OWNER FULL PIPELINE CURRENTLY PENDING AFTER C49 WINE TEST STABILIZATION / ACTUAL-GAME RUNTIME PENDING.**

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

## FIRST OWNER UPDATE FAILURE — PACKAGE METADATA/POLICY

The first owner full updater run reached `stage=package` and failed because root `README_TEST.md` still advertised v0.1c38r5 / 0.1.61 while `package.py` requires the current `versionLabel` and `packageVersion` to occur in that file.

A second stale package-policy assertion was identified before requesting another owner rerun: the historical monolithic package implementation still required `WALLSPRINT_ENABLE=1` and `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=1`, while c40 intentionally requires both values to remain 0 and presence diagnostics to be enabled.

Fix:

- root `README_TEST.md` now documents only the v0.1c40 / 0.1.65 one-F11 video capture procedure;
- the old README is preserved unchanged as `docs/README_TEST_HISTORY_20261031.md`;
- the previous `source/tools/package.py` implementation is preserved byte-for-byte as `source/tools/package_base.py`;
- the new small `source/tools/package.py` wrapper fail-closed patches exactly one known historical policy assertion in memory: it now requires `WALLSPRINT_ENABLE=0`, `WALLSPRINT_NATIVE_SOURCE_OVERRIDE=0`, and `REQUEST_PRESENCE_DIAGNOSTIC=1` before executing the otherwise unchanged package implementation;
- if the preserved base assertion shape changes, packaging aborts rather than silently changing policy.

The first failed updater retained the previous active script and performed no deployment.

## SECOND OWNER UPDATE FAILURE — C49 WINE DEADLINE FIXTURE

The next owner updater run stopped earlier at `stage=hooks:line51` inside the historical c49 test-only VCH search bridge fixture. The failing expression was exactly:

`now() >= windows[0].end`

The fixture had used one calculated `Sleep(ceil(remaining)+1)` and immediately asserted that the QPC-derived deadline had been crossed. On the owner Wine run that sleep returned before the QPC-derived deadline, producing `owned_probe_assertion_failed`. The failure occurred in `vch_search_bridge_smoke.cpp`, which is explicitly TEST ONLY and is never linked into `WallSprint.dll`.

Fix published in commit `03b10053dbd0a3c045840f4ef109611faeef00c1`:

- the deadline scenario now samples the existing QPC-derived `now()` repeatedly with `Sleep(1)`;
- it stops as soon as the exact existing `windows[0].end` threshold is reached;
- a separate `GetTickCount64` wall-clock bound of 2000 ms prevents an infinite or silently skipped wait;
- if the deadline is still not reached within that bound, the same fail-closed assertion remains;
- no c49 identity/order/debt/DR/VCH assertions were relaxed;
- no production/game hook source was changed.

Important pipeline-order correction: `workspace-build.sh` runs `build -> portable -> smoke -> hooks -> package`. Therefore this second run does **not** prove the earlier package fix yet; hooks stopped before package. A fresh owner `Update-WS.cmd` run is still required to verify both the c49 stabilization and the package repair together.

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
- First owner `Update-WS.cmd` run reached package and exposed the stale README/package-policy issue.
- Second owner run reached hooks and exposed the c49 single-sleep deadline flake before package was executed.
- c49 source readback confirms the bounded repeated deadline wait is present on the canonical branch.
- A complete fresh owner Windows/WSL pipeline after commit `03b10053dbd0a3c045840f4ef109611faeef00c1` has not yet run; do not claim package/deployment success yet.

## REQUIRED HUMAN RUNTIME TEST

Only after a successful `Update-WS.cmd` build/deploy:

1. Start video recording before the capture.
2. Press F11 once.
3. During the following 30 seconds, demonstrate ground Sprint straight -> reverse -> neutral, then ordinary WallRun left -> right -> down in one continuous take.
4. Let capture close automatically. F8 is only an emergency stop.
5. Return the newest complete `MSM2-WS-v0.1c40-YYYYMMDD-HHMMSS.log` from `MSM2-WS\logs` together with the video.

ChatGPT should first verify `REQUEST_PRESENCE_ADMISSION admitted=1`, then correlate `REQUEST_PRESENCE_RECORD offset_ms_est=...` with visible actions in the video. `offset_ms_est` is approximate relative alignment, not an authenticated engine timestamp or state label.
