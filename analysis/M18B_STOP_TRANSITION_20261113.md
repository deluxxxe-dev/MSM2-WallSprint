# M18b P1 stop correction, 2026-11-13 JST
Baseline6c771b7. Read M18 owner result fully and M17 section1; no raw log/video
attached to this cloud turn. The supplied derived evidence describes full log/
video inspection; no claim of independently viewing those attachments. Unchanged
repository/CLAUDE rules apply. P2-P6 remain deferred. Seamless motion and retained
changed gravity remain mandatory. M18 release guard/deceleration/normal hop arc
retained; owner standing acceptance preserved as evidence, new outcome UNKNOWN.

Step1: explicit0.15s pending-stop debounce after WS-active expiry (the existing
active predicate already has150ms callback freshness). Poll-independent callback
identity cancels a resume even if the next100ms telemetry poll misses it. Capture
Transform, projected velocity and forward at the pending end; debounce must not
age the carry out. <=60 kept/cancelled lines. Jump-release holds retain their
existing immediate path. No hold_start/hop on cancelled pending stop.
A new actual WS callback cancels an active stop hop/rotation immediately, releases
rows without blend and disables that stop force before another hold is admitted.
A confirmed stop bypasses the old WR-freshness pause (only stop): continuing WR
callbacks must not stretch the stop arc. Existing non-stop WR/jump routes remain.
Owned tests cover cancellation, missed resumes, exact boundary and single keep.
Test helper extraction initially duplicated a nested marker; fixed extraction,
no assertion relaxation. Production Werror initially found a misleading-indentation warning; fixed in step2, which passes the compile.

Step2: trace counts are local to GfLocal/hold generation (first3 stops), each
native frame for1.5s,100 lines. Independent native frame clock avoids the model
frame-counter comparison that silently lost data. Read game XYZ rows BEFORE
gfStep/axes writes; compare corrected rows afterwards and report written0/1.
One unavailable reason per stop: read_failed/invalid_rows/no_native_call/
episode_not_started, including fallback worker/summary. Corrected pose and raw
pre-write pose are separate fields. Model speed remains labelled stop_model.
Owned budget tests pass, production Werror compile passes. No trace side effect
on game movement or camera. Cancellation also disables a completed stop's force
on resumed WS, so pending debounce cannot reuse an old stop hold.

Step3: keep up=n for the entire stop writer lifetime. Only projected forward is
normalised/interpolated; game up never becomes the output up. At the planned
blend start, native pre-write up deviating>10deg extends hold in.1s steps up to
.8s holding+.2s blend=1.0s total after landing. This defers forward blend while
the game still requests world up, avoiding repeated restarts of an already
completed blend. Log extension and final extended_s; retain existing small-error
write refusal, coherent row/handedness/identity/XYZ-only and4000-write cap/F8.
Owned tests cover clean non-extension, repeated90deg deviations and hard total
bound. Production Werror compile passes. Actual pose continuity remains UNKNOWN.
No raw proprietary capture or new engine fact was produced; shared Wisdoms
3817f5e was consulted and is not edited by this bounded WS task.

## Cloud verification on final source tree

`build_windows.sh` PASS; `test.sh` **70 PASS**; `smoke_windows.sh` **7 PASS**;
`test_hooks_windows.sh` **907 PASS**,5 historical private-data SKIP and17
UNAVAILABLE (no owner runtime fixtures); `package.py` **2 PASS**. Final-source
hook attempts: 1. Exact per-attempt exits/log hashes/assertions are in the validation
JSON. No manual skips, fixture deadline extension or assertion relaxation.

Package: `source/output/MSM2-WS-v0.1c48/MSM2-WS-v0.1c48.script.disabled`.
CRC PASS; exactly info.json/WallSprint.ini/WallSprint.dll; packaged DLL equals
the new build and FACE_CAMERA_MAP/AIR_FACING/WALKOFF all1. DLL SHA256
`10191f811a4d8caeb195eedd00e13e6475185a3da90c170a35c249baaa6bd6fa`;
package SHA256 `bdc996b3cb3e393d349f7ac89dc46ad26f2c65f99a84583e244d28b06d7b7063`.
Artifact/log hashes identify cloud outputs; they do not replace real-game proof.

Final source tree `13cca1484309e28aa6ee6a11ae6861ea71df8d93`; task commits and QA persisted above.
Real-game owner acceptance remains UNKNOWN.
