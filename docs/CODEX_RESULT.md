# M13 result — jerk guards, jump speed and frozen zip mapping (2026-11-11 JST)

STATUS: five implementation tasks committed/pushed; cloud validation below;
actual owner-game acceptance UNKNOWN. Branch `codex/v01c20-edi-value-provenance`.
No main merge, force push, local Windows deployment or runtime acceptance claim.
M12 accepted four-direction walking and standing after3/3 stops are the baseline.

## Changes, one commit/push per task in required order

| Task | Commit | Resulting source behavior |
|---|---|---|
| 1 | `16acb35` | Replace6m/s budget with cumulative raw frame fall detector: >9m/s or six consecutive >0.2m/s rises; restore the detected frame's downward parts, neutral unchanged;100 pushed-undo logs. Actual elapsed dt is used for detection. |
| 2 | `2785b5d` | Five quiet game frames permit25% local plane adaptation, at most0.05m/frame;>0.05m cumulative movement logs follow. Reapproach needs actual current rest>0.3m/>0.2s and>=1s between attempts. |
| 3A | `20cace8` | Stop push0.5m/0.12s,4m/s airborne normal approach with shared-frame distance floor, no landing during push, first stop landing's0.3s outward clamp at plane_d<=0.02m. |
| 4 | `bb40fa4` | Mean qualified walking speed over0.3s, minimum5m/s for pushed stick with known direction; constant-speed planar steering, no neutral decay; bounded takeoff/landing relative trace. |
| 5 | `a88602c` | Copy h/theta/validity/time once at takeoff for all air steering/zips; h changes only with>=30 inliers,>=80% agreement and>=60deg span;2s rotation mean uses steady-stick samples. |

Production code is only `source/src/NativeHooks/camera_ref.cpp`.
`source/tools/test_face_control.py` and `test_face_landing.py` compile actual pure
helpers against owned data and remain in the existing portable suite. Analysis:
`analysis/M13_FACE_CONTROL_20261111.md`; exact validation/log/artifact hashes:
`analysis/M13_CLOUD_VALIDATION_20261111.json`. Final documentation changes are
this file, CLOUD_VERIFIED and the M13 analysis/validation record.

Completed-frame game steps subtract WS-applied corrections, including later
SetPosition calls; plane follow uses pre-call actual Hero position, not a desired
outward step that may subsequently be rejected. Rest tolerance2mm, plane low-pass
25%, and mirror agreement residual20deg are authored implementation choices,
not verified engine constants. Mirror evidence/steady rotation use the bounded64
sample/20s walking ring. Untrained/expired source still refuses planar direction.
Zip logs expose frozen_h/frozen_theta_deg/takeoff_time_s and mirror counts.
WS_FACE_JUMP_TRACE cap60 logs capture takeoff-relative up/side/normal displacement,
full takeoff velocity, stick and source. WS_FACE_FALL_UNDO cap100 captures every
pushed-stick undo until the cap; move samples expose speed/rises/blocked state.

Task3A is the only landing experiment implemented. Task3B requires the owner A
result first: if repeated landings or no standing remain, the next task logs the
specified0.6s/240-line WS_STOP_TRACE and stops, instead of guessing another fix.
No Task3B instrumentation or extra gameplay experiment was added now.

Normal jump launch expression/arc integration is retained; actual peak/airtime
and >=2m directional travel remain game measurements. Camera turns while still
or airborne keep the previous walking mapping; air mapping is immutable through
that jump. Body yaw, camera jerk/swing, web-line visual, charge, velocity search,
mover edits and L3 are outside scope. Exact-build/pdata/SHA, F8, INI/default-OFF
code guards and WS-only updater isolation remain. Mover remains0. No new native
hook/signature, live camera search or shared/SF modification.

No raw owner attachment was supplied here: the committed M12/M11/M10z derived
reports were used. Owned cloud fixtures do not prove game improvement. No new
runtime engine fact or Wisdoms edit is claimed; prior in-session Wisdoms context
was consulted and cross-repository writes were not authorized.

## Validation

Final tested source tree: `2a843a94b16fd93f2648ca5a9cd47efe8ab5aa15`.
Each task's camera_ref production Windows cross-compile and focused helper tests
passed before its push. Full final commands run from `source/`:

| Command | Result |
|---|---|
| `bash tools/build_windows.sh` | exit0, production warnings-as-errors |
| `bash tools/test.sh` | exit0,47 PASS lines plus4 selector unittest cases |
| `bash tools/smoke_windows.sh` | exit0,7 PASS lines under Wine |
| `bash tools/test_hooks_windows.sh` | exit0 first attempt,907 PASS /5 SKIP /17 UNAVAILABLE |
| `python3 tools/package.py` | exit0, CRC/member set/exact newly built DLL independently checked |
| `git diff --check` | PASS |

No final-suite failures, timing flakes, reruns, assertion/deadline changes or
skipped steps. Optional historical private captures/native-Windows evidence
remain unavailable and are not game passes. CLOUD_VERIFIED is appended only
after this complete final pipeline passed. Tested source is unchanged by final
docs/analysis recording, so no extra full-suite repetition is required.

Generated package:
`source/output/MSM2-WS-v0.1c48/MSM2-WS-v0.1c48.script.disabled` (package0.1.73;
normal deployment exposes one `MSM2-WS-v0.1c48.script`). Package SHA256:
`d9c04e031ab2ef25d013b188ae992b0401ba1c1d93878a750f69b644c8af0e23`.
DLL SHA256:
`caef8b0f527893714e4db0c45e4b0750111e3b08db407c291a33148359461989`.
These remain generated build outputs; committed source reproduces them through
Update-WS. Historical c48 package-manifest wording is not evidence of current
experimental runtime write behavior. No ZIP download or cloud game-drive edit.

## NEED — one normal owner run after publication

1. Run installed `MSM2-WS/Update-WS.cmd` -> Overstrike Install mods -> Run modded
   game. Keep normal packaged SETPOS=1/mover0. Wait ~20s in freeroam.
2. WS -> stop3 times, waiting3s standing after each. Then stand still on the
   face10s. Capture hero and camera jerks/standing in video. Decision: exactly
   one WALL_LANDED per stop and standing -> accept3A; repeats/no standing ->
   task3B trace-only follow-up, without guessing another movement change.
3. Walk up/down/left/right5s each; then travel>20m along the wall and stand10s
   there. Check down walking has no hitch/guard undo and quiet rest has no
   periodic REAPPROACH. This walking also trains the direction model.
4. Walking jumps and standing-with-stick jumps up/down/left/right twice each;
   web zips up/down/left/right. Caption input direction and walking/standing/zip;
   record visible body facing/camera behavior without assuming it proves travel.
   If camera turns while still/airborne, walk again to update the next takeoff's
   mapping. This jump's mapping remains frozen.
5. Return the newest complete `logs/MSM2-WS-v0.1c48-<timestamp>.log` and captioned
   videos. Directory: `<game>/Mods Library/Mods-Developing/MSM2-WS/logs`.
   No SETPOS=0 comparison or other extra experiment is requested.

All five implementation commits were pushed separately in order. GitHub HEAD
`a88602cd54ce231f64feac5ee8b19c6476665b30` was checked after those pushes. Final
record commit is pushed and checked against local HEAD before the chat handoff;
it changes no tested source/runtime bytes. User Windows/game installation and
acceptance are still pending, not performed by Git publication.

---
Historical results below are retained; M13 above is the current result.

# M12 result — stop plane and learned face-walking directions (2026-11-11 JST)

STATUS: implementation/push complete; cloud validation below; actual owner-game
acceptance UNKNOWN. Branch `codex/v01c20-edi-value-provenance`, no main merge/force.
M11 downward walking is owner-accepted and preserved. M11 SETPOS=0 comparison is
superseded and must not be requested again for this task.

## Changes and implementation commits

Task2 first: `4420744` — `camera_ref.cpp` publishes the on-WS candidate plane once
at hold start, logs point/source on start/landing/>0.05m changes, reapproaches
pinned non-faceAir rest >0.15m sustained >0.1s, and clamps every stop-push output
(first/later/final push frame) to0.9m from that plane. Existing landing rule stays.

Task1 second: `2979f6c` — face directions use a64-sample circular rotation/mirror
fit from the game's per-frame face walking. No camera matrix/position reference
remains in face axes/air direction/zip. Only eligible walking frames with raw
stick>0.5 and planar speed>1m/s train; current frame anchors include later calls,
subtract WS-applied corrections and pair the prior interval's raw stick. Mirror
selection needs>=60deg span and lower circular variance over<=20s samples;
rotation uses<=2s residuals while walking. Existing previous mirror starts+1.

Takeoff keeps <0.3s measured walking velocity exactly on its first frame;
otherwise the <20s model gives5m/s along the stick, otherwise straight normal
jump. Air steering/active-stick zip use the model. Neutral zip uses current
planar velocity, otherwise no added impulse.0 is no longer mistaken for an
uninitialized zip-request counter. One input log per jump/zip includes angles,
mirror, count/age, source, basis-relative direction and speed; no camPos fields.

Known limit: rotating the camera while standing still or airborne keeps the last
walking mapping. Walk again to refresh it; expire after20s. This task does not
find a live camera, alter body yaw/camera swing/web line/charge height, search
velocity, edit mover or restore L3 lock. Code defaults OFF, INI/F8/exact-build/
pdata/SHA guards and WS-only updater isolation remain. Accepted M11 budget
helper is byte-identical and its first/later-call thresholds/tests are preserved.

Files: `source/src/NativeHooks/camera_ref.cpp`, `source/tools/test_face_control.py`,
`source/tools/test_face_landing.py`, `source/tools/test.sh`,
`analysis/M12_FACE_CONTROL_20261111.md`, `analysis/M12_CLOUD_VALIDATION_20261111.json`,
`docs/CLOUD_VERIFIED.txt`, this result. Source details/limits in the M12 analysis.
No raw logs/videos were attached to this cloud task; committed derived reports
were read. Wisdoms was consulted in-session. No new runtime engine fact or
cross-project edit was made; stale face-camera findings may be synchronized into
Wisdoms by the owner/ChatGPT separately (cross-repository writes not authorized).

## Validation

Final tested `HEAD:source`: `85a140729f80c5a4524407050d104eb03758a0b5`.
Production task2-only cross-compile and actual-helper ASan/UBSan tests passed
before its push; full suites below validate both tasks together, unchanged tests.

| Command (cwd source) | Result |
|---|---|
| `bash tools/build_windows.sh` | exit0, production warnings-as-errors |
| `bash tools/test.sh` | exit0,44 PASS lines plus4 selector unittest cases |
| `bash tools/smoke_windows.sh` | exit0,7 PASS lines under Wine |
| `bash tools/test_hooks_windows.sh` attempt1 | exit3, position_orientation_integration.exe assertion at native_source_override_smoke.cpp:154 |
| same canonical hooks attempt2 | exit0,907 PASS /5 SKIP /17 UNAVAILABLE |
| `python3 tools/package.py` | exit0, CRC/member set/exact built DLL independently checked |
| `git diff --check` | PASS |

Initial assertion: `msm2::sample(actual) &&
ws::dot(ws::diag::actorAxes(actual.prefix).up,normal) > .999f` in the
WALLSPRINT_REAL_POSITION_WATCH_TEST variant. The unchanged entire canonical suite
passed on one retry; no assertions, gates, deadlines or steps were modified or
skipped. This historical fixture does not link camera_ref.cpp. Optional private
historical captures/native-Windows evidence remain unavailable; those rows are
not game passes. Early cwd-only invocation mistakes, exact counts/log hashes,
retry and artifact hashes are recorded in M12_CLOUD_VALIDATION_20261111.json.

Final source tree is listed in CLOUD_VERIFIED only after the complete retry
passed. No production source changed between build, portable checks, smoke,
package and hooks attempts; object timestamp confirmed the final log-field edit
preceded camera_ref compilation.

Generated package:
`source/output/MSM2-WS-v0.1c48/MSM2-WS-v0.1c48.script.disabled` (package0.1.73;
deployed active member `MSM2-WS-v0.1c48.script`). SHA256:
`f9327b7aba04cd823b404ed9090265ccaee6b952bacd294f462fae07f9f9a1d5`.
DLL SHA256:
`48a5222a3eff2cdda9570c3b8384bb09d874fe3958bd23572b7d357a113b0b73`.
These are generated outputs; committed source reproduces the package via the
existing updater. Historical c48 packager wording is not evidence of the current
experimental runtime's write behavior; the authorized M12 changes are above.
No ZIP download or cloud access to the owner's game drive is required.

## NEED — owner after publication

1. Run installed `MSM2-WS/Update-WS.cmd`; Overstrike: Install mods -> Run modded
   game. Use normal packaged SETPOS=1, mover0. Wait ~20s in freeroam.
2. WS -> stop3 times. Each should rest within+-0.05m of the logged fixed plane,
   with near-zero pin drop undos and a standing pose (video acceptance pending).
3. Walk on the face up/down/left/right for a few seconds each to train the model.
   Then walking jumps and standing jumps in all4 directions, twice each; web zip
   in all4 directions. Caption videos with direction and walking/standing/zip.
   After rotating the camera while still/airborne, walk again to refresh the
   mapping. A >20s-old model deliberately stops supplying planar launch/steering.
4. Return the newest complete `logs/MSM2-WS-v0.1c48-<timestamp>.log` plus captioned
   videos. Directory is `<game>/Mods Library/Mods-Developing/MSM2-WS/logs`.
   The former SETPOS=0 comparison is no longer needed. No conditional later fix
   or extra experiment was implemented beyond current CODEX_NEXT.

Implementation commits4420744 and2979f6c were pushed in the required order.
Remote HEAD2979f6c was checked after those pushes. The final docs/analysis record
is pushed and checked against local HEAD before the final response; it changes
no tested source/runtime bytes, so the full pipeline is not repeated for it.

---
Historical results below are retained; M12 above is the current result.

# M11 result — face screen axes and downward walking (2026-11-11 JST)

STATUS: implementation and cloud checks complete; actual Windows/game acceptance pending.
Branch: `codex/v01c20-edi-value-provenance`; no main merge or force push.

## Changed

- Task1 commit `690f10cd9a7f3ac9c7c9ac6f317b715adf708f64`: in
  `source/src/NativeHooks/camera_ref.cpp`, screen forward derives from the face
  projection of Hero minus camera position, with last-valid direction for a
  <0.2m projection. Walking determines the right-axis sign at >=20 votes/80%
  majority; fallback remains configured handedness. Jump logs include both
  positions and sign. History resets for a new hold/camera manager.
- Task2 commit `b48c0c3d515432f8070f2089aa4821b11ce3bf54`: all SetPosition calls
  in a frame share6m/s*frame_dt downward allowance, gated on stick>0.25 and
  landing age>0.3s. Consecutive-call gap>=2ms starts a frame; existing50ms dt cap
  remains. No upward replenishment; landings reset budget; neutral input revokes
  remainder. Original desired position is preserved exactly within budget.
  Bounded move samples include first/later calls and remaining allowance.
- `source/tools/test_face_control.py` compiles the actual pure production helpers
  with owned test positions/steps under ASan/UBSan; wired into `tools/test.sh`.
- `analysis/M11_FACE_CONTROL_20261111.md` records evidence limits and Task3's
  two-way decision table. This file and CLOUD_VERIFIED record validation/handoff.

Exact-build/pdata/hash guards, F8 kill, INI switches, default-OFF code defaults,
WS-only updater/isolation and mover editing0 remain. No Task3 landing fix, A
injection, body yaw, camera swing, charge-height or velocity/mover work added.
Wisdoms WALLSPRINT_INTERNALS.md consulted; no new runtime engine facts or shared
infrastructure changes, so no cross-project edit is required.

## Verification

Run from `source/`:

- `python3 tools/test_face_control.py`: PASS, position/camera-turn/degenerate axes,
  positive/negative/mixed handedness threshold and abstention; first/later downward
  calls, aggregate budget, neutral/landing guards, no replenishment and dt cap.
- `bash tools/build_windows.sh`: exit0 (third-party Capstone ignored-MSVC-pragma
  warnings only; production compilation succeeds with warnings-as-errors).
- `bash tools/test.sh`: exit0,38 PASS lines plus4 selector unittest cases.
- `bash tools/smoke_windows.sh`: exit0,7 PASS lines under Wine.
- `bash tools/test_hooks_windows.sh`: exit0,907 PASS /5 SKIP /17 UNAVAILABLE.
  No reruns, skipped steps or timing-test failures. Optional historical private
  captures/native-Windows evidence remain unavailable; these are not game passes.
- `python3 tools/package.py`: exit0; package CRC, exact three-member set and
  byte-for-byte newly built DLL equality independently checked.
- `git diff --check`: PASS. Tested source tree:
  `e19184f640e3f06e69f41add5ebd66a07b86d991`, appended to CLOUD_VERIFIED.

Detailed counts/hashes: `analysis/M11_CLOUD_VALIDATION_20261111.json`.
Generated package: `source/output/MSM2-WS-v0.1c48/MSM2-WS-v0.1c48.script.disabled`
(version/package0.1.73 unchanged as requested; deployment exposes the single
`MSM2-WS-v0.1c48.script`). Package SHA256:
`0d31f1e86fcedb80fdb7bf202a9f953c1bc8e34d137bb4a9b3b2bdc5c6597858`.
DLL SHA256: `be1d69d4f073241d03546b043b5785391e9c30b70c043363a33f4156993312b7`.
Generated artifacts remain build outputs; committed source reproduces them through
Update-WS. No install ZIP download or local game-drive mutation is needed here.

## NEED — owner, after publication

1. Run installed `MSM2-WS/Update-WS.cmd`; Overstrike: Install mods -> Run modded
   game. Normal SETPOS=1 run: wait ~20s in freeroam, WS -> stop3 times; walk the
   face up/down/left/right; face jump in all4 directions twice each, including
   after turning the camera; web zip in all4 directions. Check neutral stick does
   not slide and downward walking is not undone. Calibration requires some
   strong lateral-stick walking (>=20 eligible observations).
2. Task3 StepA is a separate run, required before choosing a landing fix. Exit,
   preserve the exact loaded WallSprint.ini (startup log identifies its path),
   set only `WALLSPRINT_WS_GRAVITY_SETPOS=0`, restart without Install mods again,
   wait ~20s in freeroam, and WS -> stop3 times. Report stands vs still falling.
   Exit and restore the original INI bytes afterward. Do not change mover0.
3. Return both complete `logs/MSM2-WS-v0.1c48-<timestamp>.log` files and videos
   with English titles identifying normal controls vs SETPOS0 stop. Logs live in
   `<game>/Mods Library/Mods-Developing/MSM2-WS/logs`. No raw log/video was supplied
   in this cloud task; only the committed M10z-r19 derived evidence was inspected.

Task3's conditional fixes remain unimplemented until the owner result. No claim
of improved gameplay, grounding, Windows installation or real-game acceptance.

Publication: implementation commits are already on the tracked branch (merge
`15a05b0c28737c0ba64214c6c0d77402828455c0` includes the concurrent documentation-only
`055c882` finish-response clarification). Successful push and ls-remote/local HEAD
match confirmed. The following validation-record commit changes only docs/analysis;
its publication is independently checked before the final response. Tested source
and runtime bytes are unchanged, so no second full pipeline is needed.

---
Historical results below are retained as history; M11 above is the current result.

# Result — V8 consumer thresholds accepted; c48 exact Update-entry discriminator implemented

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C47 `WallRun.this+0x250` RUNTIME CORRELATION RETAINED / V3-V7 SELECTED OWNER-CALLEE PRODUCER CHAIN EXHAUSTED WITHOUT A DIRECT WRITER / V8 PROVES TWO AUTHENTICATED FLOAT THRESHOLD CONSUMERS (`0.5` AND APPROX `0.71`) / C48 READ-ONLY EXACT UPDATE-ENTRY THRESHOLD COUNTER IMPLEMENTED WITH A DEDICATED NON-WRITER GATE / NO WRITE AUTHORIZED.**

## Accepted runtime evidence

c47 established:

- `WallRun.this+0x250` changes during verified WallRun;
- repeated negative pulses correlate in time with ordinary direction reversals and the later down-hold left/right oscillation;
- direct signed left/right-input semantics are unsupported because both reversal directions produced negative pulses;
- retained samples were mostly `0` to about `-0.208`, with only a tiny positive value near `+0.0057` before zero;
- field name, units, producer and writability remain UNKNOWN.

The c47 worker observer sampled asynchronously at about 100 ms, so short excursions between samples were not excluded.

## V3-V7 producer-path result

The bounded authenticated RCX-owner chain did not reveal a direct `owner+0x250` writer:

- Update: no direct write;
- `0x367DD10` / `0x29BD5F0`: no direct write;
- Path A `0x29C06F0`: no owner-relative access and no next direct call/tail lead;
- Path B `0x29C18B0`: one owner-preserving call `0x29C18D4 -> 0x308A0F0`;
- V7 `0x308A0F0`: 8 reachable instructions, no `owner+0x250` access, no next direct call/tail lead.

This chain is exhausted and must not be reopened without new evidence. This does **not** prove the field has no writer elsewhere.

## V8 consumer result — accepted

Owner returned `MSM2-WS-wallrun250-consumers-20261101-134046.json`.

Schema/result:

- schema `8`;
- `WALLRUN_0X250_AUTHENTICATED_UPDATE_CONSUMER_SLICES`;
- exact EXE and authenticated Update identities passed.

### Consumer A

- `0x142457D`: load file-backed initial `0.5f` from RVA `0x5303DE8` into `xmm11`;
- `0x1424585`: `vcomiss xmm11, dword ptr [rsi+0x250]`;
- `0x1424596`: `setb byte ptr [rbp+0x120]` with comparison flags still live.

For ordinary finite operands this local boolean is true when `this+0x250 > 0.5f`. Unordered/NaN also sets the relevant CF condition. The local boolean's semantic purpose remains UNKNOWN.

### Consumer B

- `0x14245FB`: load file-backed initial approximately `0.71f` (`0.7099999785`) from RVA `0x5337098` into `xmm0`;
- `0x142460E`: `vcomiss xmm0, dword ptr [rsi+0x250]`;
- `0x1424616`: `jae 0x142464F`.

For ordinary finite operands the branch is taken when `this+0x250 <= 0.71f`; the intermediate block is entered only when the field is greater than that threshold. That block has additional gates, calls `0x29BD5F0`, and later computes another boolean using a separate comparison against an initial `2.5f`. No semantic names are assigned.

Therefore both authenticated `+0x250` reads are **VERIFIED-STATIC threshold/control consumers**.

The numeric pair `0.5` / approximately `0.71` is compatible with angle/alignment/dot-product-style cutoffs, but that is only a HYPOTHESIS. V8 does not prove a dot product, angle, units, producer, or safe write semantics.

Detailed analysis: `analysis/WALLRUN250_V8_CONSUMER_RESULT_20261101.md`.

## c48 implementation

Runtime/package:

- `v0.1c48`;
- package `0.1.73`.

c48 retains c47 worker scalar observation and changes no gameplay write gate.

A pre-runtime review found that the old `ws_write_update_pre` callback is reached only through the writer bridge, which is correctly disabled when `ws_write_dispatch_enabled=0`. Enabling that writer gate merely to observe the field would have unnecessarily entered caller-unwind/argument-clone machinery and blurred the read-only boundary. That design was rejected before owner runtime use.

The final c48 design instead adds a separate atomic `ws_update_probe_enabled` gate used **only by the existing WallRun Update thunk**:

1. the Update thunk's initial local-callback gate includes `ws_update_probe_enabled` so CPU state is saved even when all writers are disabled;
2. after the existing source-lifecycle callback, the thunk checks `ws_update_probe_enabled` and directly calls local `ws_update_probe_sample(owner)` while the original native CPU state is preserved;
3. the old `ws_write_dispatch_enabled` check remains separate and OFF, so the writer bridge / `ws_update_prepare` path is not entered by c48;
4. the probe performs one guarded 4-byte `owner+0x250` read and updates atomics only;
5. the worker emits cumulative/final summaries and rechecks the two live threshold globals;
6. `ws_write_update_pre` remains the old no-op returning `0`, `ws_write_update_post` remains empty, and `ws_write_dispatch` remains empty.

Admission reads the two live consumer-threshold globals at RVAs `0x5303DE8` and `0x5337098`, records whether they match V8's file-backed bits `0x3F000000` / `0x3F35C28F`, and enables the dedicated probe only when the complete verified WallRun observation plan is admitted and both reads succeed.

Per exact pre-original Update callback, atomics count:

- total samples;
- read success/failure;
- field NaN vs non-NaN;
- consumer-A true condition (`field > threshold0` or unordered);
- consumer-B fallthrough condition (`field > threshold1` or unordered).

`VCOMISS` unordered handling is mirrored as NaN on either operand; infinities are not incorrectly classified as unordered.

No logging occurs in the native callback. No extra hook, child-pointer dereference, game function call, caller unwind, argument clone, field write, transform write, gravity write, or native-source write was added.

## Files changed for c48

- `source/src/msm2/actor_writer.cpp`
- `source/include/msm2/actor_writer.hpp`
- `source/src/msm2/hook_thunks.S`
- `source/include/version.hpp`
- `source/tests/wallrun250_threshold_probe_static.py`
- `source/tools/test.sh`
- `source/tests/request_presence_static.py`
- retained V7/V8 static tests updated for c48 version/package
- `source/mod/WallSprint.ini`
- `source/mod/presets/request_presence_read_only.ini`
- `source/mod/info.json`
- `source/tools/package.py`
- root/source `README_TEST.md`
- V8 analysis/docs.

## Validation status

Authored static contracts now pin:

- exact field offset `0x250`;
- exact V8 threshold RVAs and expected file bits;
- dedicated Update-only `ws_update_probe_enabled` gate;
- generic Init/Entry/Deinit thunks do not reference the new gate;
- direct probe callback occurs before the separate old write-dispatch check;
- guarded read through `msm2::readMemory`;
- no log call on the native Update callback;
- no `WriteProcessMemory` / remote-thread / context-write API;
- `ws_write_dispatch_enabled` is never set to `1` by c48;
- old `ws_write_update_pre` remains a no-op returning `0`;
- post callback remains empty and direct Actor writer remains inactive;
- c48/0.1.73 package/config/readme alignment.

The normal installed updater must still perform the repository's full Windows/WSL build, portable tests, smoke, hooks and package validation before runtime use. Do not claim a successful c48 build until that updater passes on the owner machine.

### c48 hooks-link fix (2026-11-04, test-only)

Owner updater run failed at `source/tools/test_hooks_windows.sh` line 22 (`actor_writer_smoke.exe` link): `undefined reference to msm2::readMemory(unsigned long long, void*, unsigned long long)`.

Cause: c48 added two guarded `msm2::readMemory` calls to `actor_writer.cpp` (live threshold read and `owner+0x250` probe read). `actor_writer_smoke.cpp` includes `actor_writer.cpp` directly and links without `sdk_bridge.cpp`; it stubbed only `writeLog`. `wallrun_runtime_smoke.cpp`, which links `actor_writer.o`, already supplies its own test `readMemory` and was unaffected.

Fix (test code only):

- `source/src/msm2/actor_writer_smoke.cpp`: adds a test-only `msm2::readMemory` with the same null/zero/overflow guard and same-process `ReadProcessMemory` contract as `sdk_bridge.cpp`. The smoke still never enables `ws_update_probe_enabled` or `ws_write_dispatch_enabled`; its existing hard-disabled-writer assertions are unchanged.
- `source/tests/wallrun250_threshold_probe_static.py`: regression check that this smoke defines the test-only `msm2::readMemory` and does not enable either gate or use `WriteProcessMemory`. Verified that it rejects the previous smoke source.

No production source, runtime behavior, read-only/c48 gate, MOD version (`v0.1c48` / `0.1.73`) or package metadata changed.

Cloud checks (system mingw-w64 g++ posix + clang for `guard.c`, Wine 64-bit):

- previous `actor_writer_smoke.cpp` reproduces the exact undefined-reference link error;
- fixed `actor_writer_smoke.exe` links and prints `PASS: Direct Actor writes hard-disabled ...`;
- `wallrun_runtime_smoke.exe` (links `actor_writer.o`) also links and passes;
- portable `source/tools/test.sh` passes completely.

The full owner toolchain `test_hooks_windows.sh` (later lines, capstone, other smokes) was not run in cloud; the owner updater remains the acceptance gate.

## Next runtime purpose

One ordinary WallRun sequence is sufficient:

right -> reverse left -> reverse right -> hold down until the known left/right oscillation -> end WallRun -> normal game exit.

No video is required unless behavior differs from the already documented sequence.

Acceptance requires:

- `wallrun_0x250_threshold_probe_admission complete_verified=1 admitted=1 update_probe_gate=1 write_dispatch_gate=0`;
- V8 expected threshold bits match at admission;
- live threshold rechecks remain unchanged;
- `read_fail=0`;
- no hook queue drops;
- for a run containing only the requested WallRun, compare `exact_update_pre_original_samples` with verified `Update=` count; a mismatch makes coverage partial;
- inspect both threshold counters.

If both counters are zero with complete coverage, the c47 correlated negative pulses did not activate either authenticated threshold consumer during this sequence. If either is nonzero, analyze that bounded crossing evidence next. Neither result authorizes a write by itself.

## 2026-11-04 — M1 experiment tooling prepared (no runtime change)

- Added `source/tools/dat1_config_fields.py` (read-only DAT1 field-struct walker) and `source/tools/build_wallrun_m1_experiment.py` with
  `source/tools/test_build_wallrun_m1_experiment.py` (synthetic tests, wired into `source/tools/test.sh`).
- Layout of owner speed config decoded: 10 field structs, packed data in entry order, u8 or float per flag; SettleSpeedMax/Min identified by
  alphabetical position (HYPOTHESIS). Verified against real owner file: 20 edits, Max>=Min in every pair.
- Steering builder self-checks against known offsets 0x218/0x219 and values; the owner steering file was not available in cloud, so the real
  steering package is not built yet.
- Docs: `DISTRIBUTION_RISKS.md` (new), `DEV_RULES.md`, `AGENTS.md` owner decisions, `CODEX_NEXT.md` rewritten (old copy archived), `PROJECT_STATE.md` current-position header.
- Not game-tested. Runtime v0.1c48/0.1.73 unchanged.
- 2026-11-04: real steering file (SHA matches vanilla pin) parsed; layout self-check passed (TurnSpeedFast/Slow at 0x218/0x219, values 100/75, TurnMaxDown=12 at 0x213; signed bytes use flag 0x04000010). Package `MSM2-WS-M1-DownLimit.zip` built (steering SHA256 `e12f8153...2d04`), delivered to owner outside Git. Field-order hypothesis now VERIFIED-STATIC for steering; speed names still HYPOTHESIS.
- 2026-11-04 runtime: M1 run accepted (see analysis/C48_M1_RUNTIME_RESULT_20261104.md); M1b package TurnMaxDown 90 only prepared (builder names stage by variant).
- 2026-11-04: M1b accepted (TurnMaxDown 90 only -> straight down). Added ws_assets.py + Update-WS hook (tests: test_ws_assets.py, test-workspace.py PASS). Not Windows-tested.
- 2026-11-04: sideways WR drifts down at TurnMaxDown 90 -> M1c (TurnGain 0) via ws_assets.TURN_GAINS; Update-WS now reloads asset modules after pull (first run after this commit still uses old engine: run twice). Tests PASS, not Windows-tested.
- 2026-11-04: Windows updater hooks stage failed once at watch_retirement_smoke.cpp:150 (`now()>=windows[0].end` after Sleep; same suite passed earlier). Test-only fix: wait loop until the real deadline (max 200 ms). Root cause = timer granularity is the likely explanation, not reproduced in cloud (no Windows/Wine build env); syntax checked only.
- 2026-11-04: second hooks failure was caused by my 3ee8fb8 fix (200 x Sleep(1) cap shorter than the up-to-1.5 s window). Replaced with re-sleep-until-deadline bounded by 5 s wall time. Built and ran watch_retirement_smoke under Wine in cloud (capstone from PyPI sdist): all 10 scenarios PASS/UNAVAILABLE as before; 3ee8fb8 version reproduces the --timeout failure; original version passes in cloud (owner's first failure = early Sleep return, environment-specific).
- 2026-11-04: same early-return deadline wait fixed in continuation_witness / production_witness_review / vch_search_identity smokes. Full build_windows.sh + test_hooks_windows.sh now run in cloud (mingw + Wine, capstone from PyPI sdist, decoder download skipped): rc=0, 907 PASS lines, through c71.
- 2026-11-04: M1c accepted by owner; M1 done. Next M2 (animation) step 1 = owner asset listing.
- 2026-11-04: M2a clip swap (sprint -> WallRun loop clip, Peter+Miles) added to ws_assets as optional inputs. Tests PASS. Not game-tested.
- 2026-11-04: M2a accepted (sprint clip plays, body upright). M2b: default INI re-enables native source override. Tests PASS. Version label unchanged (v0.1c48/0.1.73) to avoid touching version contracts.
- 2026-11-04: package.py policy/manifest aligned with M2b (ENABLE=1/OVERRIDE=1, native_source write true). package.py + release.py run in cloud on full Windows build: PASS.
- 2026-11-05: M3 data analysed; M3a TurnSpeedFast/Slow 255 added to ws_assets (TURN_SPEEDS). Tests PASS. Not game-tested.
- 2026-11-05: M3a rejected/reverted; M3b read-only WallRun.this direction-field scanner added (unit test + full cloud Windows build/hooks/package PASS). Version label unchanged.
- 2026-11-05: M3b found WallRun+0x240/+0x24C direction; M3c bounded tilt write added in pre-original Update callback (cloud full Windows build/hooks/package PASS). Not game-tested.
- 2026-11-05: M3c accepted (diagonal-up travel); M3d stick/WASD drive added (XInput dynamic load, wall-relative mapping, once-per-owner lateral calibration).
- 2026-11-05: XInput now loaded only inside Spider-Man2.exe; loading it in the Wine actor_writer_smoke left the process hanging (cloud hooks run timed out after that smoke).
- 2026-11-05: M3d accepted; drive write cap changed to per WallRun session. M4 acceptance sweep defined (CODEX_NEXT).
- 2026-11-05: cloud hooks run after the cap change failed once in native_source_override_smoke.cpp:238 (c10 real-thread diagonal race timing; smoke does not link actor_writer). Reran test_native_source.sh 3x: PASS 3/3. Known intermittent timing test; if the owner updater hits it, rerun Update-WS.
- 2026-11-05: M4 acceptance sweep all pass; stage 1 accepted. Next M5 camera (config decode first).
- 2026-11-05: M5 step 1 camera config decode: no roll field; next reference/scan.
- 2026-11-05: ActorRelativeCamera strings reviewed (research only); M5a combined owner test defined.
- 2026-11-05: M5a result recorded; offline find_camera_refs.py locator added (self-tested on synthetic PE).
- 2026-11-05: M5b own camera hook (entry record thunk, WS-only +0x304) + camera-forward scan added. Cloud build OK; hooks/package run in progress at commit.
- 2026-11-05: M5b accepted; M5c camera-relative stick mapping with ground handedness calibration added.
- 2026-11-05: M5c calibration relaxed (handedness never completed in first run); build+package PASS.
- 2026-11-05: +0x1E0 rejected as camera forward; combined forward/sideways ground scan with auto-accept added.
- 2026-11-05: camera forward +0xD34 accepted and hardcoded; camera forward latched per stick press. Build PASS; hooks run in progress at commit.
- 2026-11-05: M5c-r4: camera-forward latch dropped for 1 s after WS camera engage and re-taken on >45 deg stick change. Build PASS; hooks run in progress at commit.
- 2026-11-05: M5c-r4 cloud hooks run PASS (exit 0) and package PASS.
- 2026-11-05: M5c-r5: camera-forward latch removed (live +0xD34), singular-view hold, wallrun_m5c_map 2 Hz diagnostic. Build PASS; hooks run in progress at commit.
- 2026-11-05: M5c-r5 cloud hooks run PASS (exit 0) and package PASS.
- 2026-11-05: M5c-r6 diagnostics: vanilla reversal event log and read-only camera axis scan near +0xD34. Build PASS; hooks run in progress at commit.
- 2026-11-05: M5c-r6 cloud hooks run PASS (exit 0) and package PASS.
- 2026-11-05: M5c-r7: camera up (+0xD44) assists wall-plane forward; reversal-frame WallRun state diagnostic. Build PASS; hooks run in progress at commit.
- 2026-11-05: M5c-r7 cloud hooks run PASS (exit 0) and package PASS.
- 2026-11-05: M5c-r8: orientation per-session write cap 4096 -> 262144 (long WS turned upright); camera side axis for screen-forward during camera roll. Build PASS; hooks run in progress at commit.
- 2026-11-05: M5c-r8 cloud hooks run PASS (exit 0) and package PASS.
- 2026-11-05: M5c-r9: short ctx_wallrun_inactive gaps (<=3 s, identical native WallRun identity) hold instead of hard-closing orientation. Build PASS; hooks run in progress at commit.
- 2026-11-05: M5c-r9 cloud hooks run PASS (exit 0) and package PASS.
- 2026-11-05: M5c-r10: WallRun observer resumes a timeout-closed session on same-owner Update (game stall); ctx-gap hold 10 s. Build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M5c-r10 cloud unit tests (tools/test.sh) PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M5c-r11: orientation entry forward falls back to projected actor up/right (head-on swing into wall refused entry). Unit tests updated; build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M5c-r11 cloud unit tests (test.sh, test_reversal_heading.sh) PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M5c-r11 accepted (10/10 sessions verified Deinit exit; no upright after swing entry).
- 2026-11-05: M5d-1 turn-around diagnostics: input stack/PE import scan, game override samples with camera axes. Build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M5d-1 cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M5d-1 analysed (no stick/camera formula fits; game holds a lateral run direction). M5d-2 direction-slot sign counters added. Build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M5d-2 cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M5d deferred (one-sign events only). M5e-1 camera-distance diagnostic at WS entry added. Build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M5e-1 cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M5e-1 analysed (+0xD54 camera position; dive at end of 0.25 s orientation rotation). M5e-2: WS camera engage delayed 1000 ms. Build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M5e-2 cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M5e-2 rejected (dive independent of rotation; pitch carry-over hypothesis). M5e-3: delay reverted, read-only camera pitch-slot scan. Build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M5e-3 cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M5e-3 run produced no camera diagnostics; M5e-3b status line added. Build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M5e-3b cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M5e-3b found the recorded camera manager was a non-live instance (+0xD34 zero). M5e-3c: thunk caller ring + worker live-camera selection. Build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M5e-3c cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M5e-3c camera selection accepted; M5e-3d pitch scan uses camera->hero view line and reports row/view-line dots. Build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M5e-3d cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M5e deferred (+0xD24..+0xD54 likely the actor-relative reference frame, not the rendered camera). M6a natural turning: 600 deg/s in-plane turn rate. Build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M6a cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M6b facing follows commanded travel; M6c ground sprint turn-rate histogram (read-only). Build PASS; unit/hooks run in progress at commit.
- 2026-11-05: M6b/M6c cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-05: M6d WS camera framing config patch (ground traversal offsets) in ws_assets; test_ws_assets PASS (synthetic + real vanilla file patch verified: 14 bytes). Python-only change.
- 2026-11-06: M6e: WS turn rate 400 deg/s (ground measured); read-only camera framing comparison (ground vs WS). +0xD44 = camera forward. Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6e cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6e framing measured (WS camera closer/lower, not following travel). M6f: WS camera config uses standard Offset* values. test_ws_assets PASS; real-file patch verified. Python-only.
- 2026-11-06: M6g: uniform WS camera offset (4.5 m any pitch); bounded camera yaw-follow experiment (rows rotate toward travel, 90 deg/s). Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6g cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6h: WS speed parity MoveSpeedFast 16->9 (measured WS 16 m/s vs ground 9-10) and camera height Y=1 m, pivot 1.55. Python tests PASS. Config-only.
- 2026-11-06: M6i steady straight-line speed diagnostic (ground vs WS). Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6h/M6i cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6i: MoveSpeedFast rejected as WR speed (WS faster at 9). M6j: WS camera distance Z -3.0 (measured WS 6 m vs ground 4 m). test_ws_assets PASS. Config-only.
- 2026-11-06: M6k WS camera Z -2.5 (measured WS ~4.5 m vs ground ~4.0 m). test_ws_assets PASS. Config-only.
- 2026-11-06: owner requirement recorded: per-player ModSettings presets for distance/speed etc.; M6 searches defaults (FEATURE_ROADMAP).
- 2026-11-06: M6l: camera framing split by travel direction (up/side/down) to separate speed lag from pitch-dependent camera behavior. Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6l cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6l: off-center 36-44 deg only when running down (speed similar) -> M6m camera aim correction toward hero pivot (90 deg/s, 5 deg deadband). Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6m cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6n: WS camera PivotHeight 0 (world-up pivot made distance depend on travel direction), Z -3.5. test_ws_assets PASS. Config-only.
- 2026-11-06: M6o: WS camera auto-centering (follow+aim) default OFF via WALLSPRINT_CAMERA_AUTOCENTER=0. Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6o cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6p: vanilla follow-camera centering experiment (herofollowcamconfig VelocityFollowData.Enabled=false); follow camera configs decoded. test_ws_assets PASS. Config-only.
- 2026-11-06: M6p rejected/disabled; M6q WS camera yaw hold when auto-centering is off. Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6q cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6q rejected (camera rows are recomputed output; M6g/M6m effect reinterpreted as vanilla). M6r read-only reference-setter capture hook (0x3350220). Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6r cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6r: setter block = [start right, wall normal, current hero right]; M6s yaw lock replaces current right with the session-start right (pinned SHA). Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6s cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6s rejected; M6t setter preserve flags (R9|=3) for auto-centering off. Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6t cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6t rejected; M6u climb follow camera velocity-follow off (config experiment); offline dump_camera_code.py added. test_ws_assets PASS.
- 2026-11-06: setter disassembled (rdx fwd, r8 up, r9 flags; view angles mover+0x2CC/+0x2D0). M6v angle hold for auto-centering off. Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6v cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6v installed but mostly bypassed (GetLastInputInfo manual 85%); M6w right-stick-only manual detection. Build PASS; unit/hooks run in progress at commit.
- 2026-11-06: M6w cloud unit tests PASS, hooks run PASS (exit 0), package PASS.
- 2026-11-06: M6w accepted by owner (WS auto-centering off works).
- 2026-11-06: cloud full suites incl. smoke_windows.sh PASS on source tree 261c554503a9aa11c256b23fa21ca4cd0db1cd0d (cloud env needed a UTF-8 locale and wine path links; no repo change). Listed in docs/CLOUD_VERIFIED.txt.
- 2026-11-06: M7a mouse look detection (WH_MOUSE_LL, physical moves) for the camera angle hold; M7 prep: ws_settings.hpp + WallSprint.ini [WallSprintSettings] + docs/SETTINGS.md (turn rate, mouse/stick manual thresholds). Build in progress.
- 2026-11-06: M7a/M7-prep cloud full suites PASS (test.sh, smoke, hooks, package) on source tree dfd7e04ec6f653e46deb364fac18273c96b63477; listed in CLOUD_VERIFIED.
- 2026-11-06: M7a accepted (mouse look). M6x WS-exit camera diagnostic added.
- 2026-11-06: M6x cloud full suites PASS on tree 8dc46c192ffc404b328109897c8e0bce035668b8 (listed). Wisdoms updated: MSM2-Wisdoms main 97cb80d (camera/config/input engine facts, WS milestone).
- 2026-11-06: M6x analysed (camera follows the hero's fast upright return); M6y 700 ms exit blend of the camera reference up.
- 2026-11-06: M6y cloud full suites PASS on tree 5b7aa310078a69be3e8c51b00ff45769392cf978 (listed).
- 2026-11-06: M6y rejected (blend rarely triggered); M6z rate-limited camera up (150 deg/s) for entry and exit.
- 2026-11-06: M6z cloud full suites PASS on tree a98075fe490354e09e9fff64f70e2d6f7355e596 (listed).
- 2026-11-06: M8a WS jump clip swap (ground sprint jump -> 7 sideways WR jump clips), tooling only, no native change. Plan/decision table: analysis/M8_JUMP_PLAN_20261106.md. Charge jump clip names pending (owner search).
- 2026-11-06: M8a cloud full suites PASS on tree 17a3e13322b8e252f4a1b288069a6e5bb2b2c50c (listed).
- 2026-11-06: M8a accepted (sprint jump clip plays for Peter/Miles). M8b: WR jump config YBoost 0 + read-only WS_JUMP_TRACE (analysis/M8_JUMP_PLAN_20261106.md, risk D21).
- 2026-11-06: M8b cloud full suites PASS on tree 2d004a93f4f24554563c4e511ba3612bb1d018ba (listed).
- 2026-11-06: M8b A/B: YBoost = world-up boost at WR jump-off (VERIFIED-RUNTIME). M8c: WS-only in-memory YBoost 0 (scan + restore), file patch disabled (D21 closed, D22).
- 2026-11-06: M8c cloud full suites PASS on tree 152a3c6512f6c3f3d6d2673d0962c809fd73f72f (listed).
- 2026-11-06: M8c scan timed out (no write). M8c-r2 faster scan; cloud full suites PASS on tree ae64bb50177898f6e121f6c0c0f2d0c563c5c378 (listed).
- 2026-11-06: M8c-r2 scan timed out again; M8c-r3 linear resident-only scan, cloud full suites PASS on tree 7e9c56f2170927140ab7f9ef46b98cef54661fa9 (listed).
- 2026-11-06: M8c-r3 scan finished, no raw block. M8c-r4 mapped/readonly + float-pair dump; cloud full suites PASS on tree a98077cd1719e503356d479aa62ac3d331f6dc74 (listed).
- 2026-11-06: M8c-r4 found the runtime float layout. M8c-r5 WS-only YBoost write on it; cloud full suites PASS on tree cbe9176e8733abfd3d951d699754767b1adbd8d4 (listed).
- 2026-11-06: M8c-r5 WS-only YBoost write VERIFIED (scan accepted, WS jumps no upward boost). M8d read-only jump press/release timing (WS_JUMP_INPUT).
- 2026-11-06: M8d cloud full suites PASS on tree e7810c3e1afee67d15e61d06e9b9e5822be7e8c0 (hooks suite re-run: first run hung in orientation_trace_smoke, which links no changed file).
- 2026-11-06: M8d: WS jump fires on press (VERIFIED). M8e read-only: XInput pass-through hooks (calls/rumble) + ground charge hold-vs-rise measure.
- 2026-11-06: M8e cloud full suites PASS on tree 9648d16db9f5471ad2b5310fce83279a6d12e927 (listed).
- 2026-11-06: M8e: vanilla charge curve measured (threshold ~0.5-0.8 s, ~28-39 m); xinput9_1_0 not the pad path. M8f: hooks on all XInput DLLs + module list.
- 2026-11-06: M8f cloud full suites PASS on tree 698066c1d22e6b63aa614ab08fa0a855af68ab9f (listed).
- 2026-11-06: M8f: XInput polled ~16/s, no XInput rumble; charge curve refined. M8g: hide A from XInput during WS (route test).
- 2026-11-06: M8g cloud full suites PASS on tree 681ba54acabf8720552979b1985acd03d24f338d (listed).
- 2026-11-07: M8g: XInput is not the gameplay pad path (VERIFIED). M8h: WGI Gamepad reading/vibration hooks + A hold-back test.
- 2026-11-07: M8h cloud full suites PASS on tree eb4ac4e1ec3fa17bd68e7a4d3b4666909ae2e7b6 (listed).
- 2026-11-07: M8h: WGI hook not installed (no gamepad visible to our query); jump still fires without XInput A. M8i: Raw Input / hid.dll call counting.
- 2026-11-07: M8i cloud full suites PASS on tree fff9e62801385868a68da52e3476ed2d3a093ff1 (listed).
- 2026-11-07: M8i: gameplay pad = WGI GetCurrentReading, rumble = WGI put_Vibration (VERIFIED). M8j: WS charge jump v1 (hold-back + Speed scaling).
- 2026-11-07: M8j cloud full suites PASS on tree c91f417c46a76e0e27dcedbac0324602301e7ba6 (listed).
- 2026-11-07: M8j hold-back did not engage; M8j-r2 signed WS check + per-press diagnostics; cloud full suites PASS on tree 5e16a623b9087f972a7fb875c5add88ce59bf8b6 (listed).
- 2026-11-07: M8j-r2 found the edge-detection bug; M8j-r3 fix; cloud full suites PASS on tree 0bb089ace31c160bfe0f740252ef2ad4ed3de37e (listed).
- 2026-11-07: M8j-r3 charge hold-back VERIFIED; Speed* scaling does not set the launch (capped ~22 m/s). M9a read-only hero velocity field search.
- 2026-11-07: M9a cloud full suites PASS on tree 943fc4010241dd1390c458f977ff7d6b6a1802a0 (listed).
- 2026-11-07: M9a velocity candidates [hero+0xBA0]+0x7FC / [hero+0xAE8]+0x1E0 (0.92/0.90). M9b bounded velocity write on WS charge release.
- 2026-11-07: M9b cloud full suites PASS on tree b63f07b3cbffe0593270d7982ed258393cae74f0 (listed).
- 2026-11-07: M9b: slot 1 is not the velocity source; slot 0 untested (check too strict). M9c: slot 0 test + RT held during charge (vanilla-like release).
- 2026-11-07: M9c cloud full suites PASS on tree 115f6783fb1077b1138567603107121758c93ba2 (listed).
- 2026-11-07: M9c: RT+A release VERIFIED; both velocity candidates rejected. M9d: charge scales Speed/GenBoost/InSpeedMax.
- 2026-11-07: M9d cloud full suites PASS on tree e750dd98d8bbade16aa653c915e4ff66d657f204 (listed).
- 2026-11-07: M9d: charge jump ~2x speed, mostly along the wall. M9e: per-charge field attribution (InSpeedMax/Speed/GenBoost).
- 2026-11-07: M9e cloud full suites PASS on tree 7b0cfe8cbdb365256c24d38828f6504d4fa7cbcb (listed).
- 2026-11-07: M9e: no single config field aims the charge along the normal. M9f: two-level velocity search + bounded +4 m/s fall probe.
- 2026-11-07: M9f cloud full suites PASS on tree af795a64f06b830ab021a49816abdadda20fa981 (listed).
- 2026-11-07: M9f: 3 probes, field 0x4e4 object rejected. M9g: alias skipping, faster probes, 2-sample effect.
- 2026-11-07: M9g cloud full suites PASS on tree c6be2e8764770fdf83b1772a15de55756aeccfd3 (listed).
- 2026-11-07: M9g probes inconclusive (+-3 noise). M9h: +15 m/s probe, landings discarded.
- 2026-11-07: M9h cloud full suites PASS on tree 639b5c52397001acddd91dd28fc780e06fedae98 (listed).
- 2026-11-07: M9h: velocity search within two pointer levels closed (no writable source). Probe off (INI 0).
- 2026-11-07: probe-off tree cloud full suites PASS (0382e89fb0d1a411e47de850abb16d8234b821dd, listed).
- 2026-11-07: owner requirements recorded (FEATURE_ROADMAP). M10a: gravity chain discovery (read-only) + sprint-charge clip identification.
- 2026-11-07: M10a cloud full suites PASS on tree 6eded085af2d833d87de5fcd9d5a33c191695b92 (listed).
- 2026-11-07: M10a: no gravity-like chain; sprint charge ~7-10 m (separate jump). M10b: log all chains, test stand_chargejump_fwd.
- 2026-11-07: M10b cloud full suites PASS on tree 3b37c482fa47b53d1a43a6b8416ebf12cdd435a7 (listed).
- 2026-11-07: M10b: stand_chargejump_fwd = sprint charge launch clip (VERIFIED); gravity-orientation block seen. M10c: bounded gravity-toward-wall test after WS jumps.
- 2026-11-07: M10c cloud full suites PASS on tree e2b8bbedbe22466724624560f968caeac1c1a613 (listed).
- 2026-11-07: M10c broke WS (worker stall from the gravity search; zero-normal write). M10c-r2: search on its own thread, normal check.
- 2026-11-07: M10c-r2 cloud full suites PASS on tree 5124508807b6d13a55076ed897b1ae257e730a53 (listed).
- 2026-11-07: M10c-r2: WS fixed; gravity block not found. M10d: orientation re-aims on moderate wall-normal changes instead of closing.
- 2026-11-07: M10e: SetGravityVector pass-through observer (identity-checked) feeding the gravity test.
- 2026-11-07: M10d+M10e cloud full suites PASS on tree c185298937b5a6b5fbb8c903c3d0c6f9ac3f9ad5 (listed).
- 2026-11-07: M10d accepted (WS follows uneven walls). M10e: gravity block = hero gravity orientation (up), zeroes motion along it, no pull. M10f: block during the whole WS, vanilla YBoost.
- 2026-11-07: M10f cloud full suites PASS on tree c4e1c7807d5896c5dc00d41c4de37be5136e4921 (listed).
- 2026-11-07: M10f: gravity block = orientation/up only (jump and force use world up); still-stand in WS pose observed. M10g: scan for (0,-g,0) gravity force vectors.
- 2026-11-07: M10g cloud full suites PASS on tree 8e5d983fa47ff0a1660bebdde5a735fd0b16cbee (listed).
- 2026-11-07: M10g gravity-vector scan inconclusive (too many (0,-g,0) triples). M10h: SetGravityVector MinHook status diagnostics.
- 2026-11-07: M10h cloud full suites PASS on tree ba230c696460c84a264856ac7cef01613aa2bf1a (listed).
- 2026-11-07: M10h: MinHook cannot hook SetGravityVector (VEX prefix). M10i: exact-prefix manual hook (Wine-tested).
- 2026-11-07: M10i cloud full suites PASS on tree bfb672581cf1771b11d860d18e7548ee06aa3372 (listed).
- 2026-11-07: M10i: SetGravityVector hook works (340 calls/s, 13 blocks, input = up). M10j: per-block counts + setter input substitution for the hero block.
- 2026-11-07: M10j cloud full suites PASS on tree 2f89e382375d70007da28162329238764c94379e (listed).
- 2026-11-07: M10j: hero block dropped (non-vanilla), no test. M10k: sticky hero block, log the game's hero up inputs.
- 2026-11-07: M10k cloud full suites PASS on tree 7a5ca3286f2be6e50a1cae546513a7fd91cf4a77 (listed).
- 2026-11-07: M10k: chain block was not a setter block. M10l: identify the hero block by the first non-vertical setter up (wall crawl).
- 2026-11-07: M10l cloud full suites PASS on tree 4cd2f27bf4cb3e6b6c44994c661d0f48587a9fdc (listed).
- 2026-11-07: M10l: WALL GRAVITY ACHIEVED (pull + orientation, wall-to-wall). M10m: normal freshness = current WS session.
- 2026-11-07: M10m cloud full suites PASS on tree 5a5d1cffe06e356d5a66164f9106589fda569b1b (listed).
- 2026-11-07: M10m accepted (no vanilla WS jumps, hero id by WS). M10n: hold wall gravity until wall/ground/stop or 3 s.
- 2026-11-07: M10n cloud full suites PASS on tree f5fe48543a148cebf8726dbf2b0049abcf8de203 (listed).
- 2026-11-07: M10n videos: slow ground reset, mid-air drops, camera reset, direction flip. M10o: hold armed during WS, WallRun normal, landing by vertical-speed stop, WS camera kept.
- 2026-11-07: M10o cloud full suites PASS on tree a22e5a2964dd650d1a4eb607fc67107bdbae865b (listed).
- 2026-11-07: M10p: ground contact via game setter, 8 s hold + 2 s blend back, swing keeps hold, L3 gravity lock, camera re-centering in air.
- 2026-11-07: M10p cloud full suites PASS on tree d603245b56845a8cafbac67c0eb3074bceeb919c (listed; first push had a build error, fixed).
- 2026-11-08: M10q: wall gravity by per-frame Transform::SetPosition correction (WALLSPRINT_WS_GRAVITY_FORCE=1), per-frame smooth gravity return, landing/contact/stall ends, L3 stand (stick neutral until re-pressed). Cloud suites PASS on tree 3b36c6ac7e6033bd9bd076277e4f2b5b76dd5bbb (hook suite rerun after one orientation_trace_smoke timeout).
- 2026-11-08: M10r: SetPosition leaf admission + guarded readback, direct-store fallback; force only when the game treats the hero as airborne; lock not ended by RT release; stalled only with force; wall stand/walk clip mapping (7 vanilla ground clips -> 109 crawl paths). Cloud suites PASS on tree 3c7c5441729bdf1c7aa4e5e2e309e22c1601d958.
- 2026-11-08: M10s: force on its own ~500 Hz thread (setter calls are bursts), direct Transform+0x30 route; no force while locked; lock ends off-wall (>1.5 m); stalled only after force ticks; crawl stand clip mapping off. Cloud suites PASS on tree df9fd3c53392528dfd759315f44aa16be8663703.
- 2026-11-08: M10t: wall-normal launch from the released jump height (normal 1.8 m, charge 15-39 m), force paused during WallRun updates, wall contact ends hold after 150 ms. Cloud suites PASS on tree 415bf1df49553b53e20e4e85fb6681d806358b92.
- 2026-11-08: M10u: charge along normal 4-12 m, 1.5 s wall landing hold, landed_velocity off under force, hero vertical velocity search + per-hold candidate write test. Cloud suites PASS on tree 36e6f0f45132f244f2a9dad9b74a4af23a3e28b3.
- 2026-11-08: M10u-r2: velocity search checks continue across holds (phase 1 kept), wider tolerances + change-consistency check. full suites PASS on tree fe6369fbe780bef30a14d77163ef9bc5e0cc283d (hook suite passed on the 4th run; earlier runs failed only the native_source_override_smoke timing assertion, unrelated file, flaky after container restart).
- 2026-11-08: M10v: launch-model fallback removed (fast-flight bug), velocity search by fall-rate rule (no estimate). Tree 7b39e95289e766f6595d60f1135ca453cd871c43: build, test.sh, smoke, package PASS; hook suite PASS except native_source_override_smoke timing assertion (fails 3/3 in this container, does not link the changed file; rest PASS with that step skipped).
- 2026-11-08: M10w: landing plane from the last on-wall WS position; velocity sweep resumes across holds over the whole address space. Tree 4a5dc3ea3c8aed0d3efe2d2fa799e1955d821db2: build, test.sh, smoke, package PASS; hook suite PASS with only the native_source step skipped (that timing test fails in this container, does not link the changed file); one full hook run hung in orientation_trace_smoke (known intermittent).
- 2026-11-08: M10x: landing plane = WallRun wall point (owner+0x338), velocity sweep off by default; Smooth-video analysis corrected per owner. Tree 8bfd9f41ec912072d2b28d0a117a98e3fadb323c: build, test.sh, smoke, package PASS; hook suite PASS except the native_source_override_smoke timing test (fails in this container, links no changed file).
- 2026-11-08: M10y: restore hold-start resets swallowed by a comment (jump 9 cause), contact flag per hold, wall hold until new WS / A / 30 s, jump_no in log. Cloud suites PASS on tree e1551285ad38bd16b3fd5d6c7877e67803242b7f (full hook suite).
- 2026-11-08: M10z: L3 lock cancelled; WS face stays the ground until a new face or B (Xbox B, 1 s smooth return; owner corrected A -> B); stalled/8 s/30 s ends removed. Cloud suites PASS on tree 5ec94c3107d77418d90e05d096ab1259d7896012 (full hook suite).
- 2026-11-09: M10z-r3: wall pin uses the game's own position, undoes only >0.1 m world-y drops, keeps -0.05..0.6 m from the wall. Cloud suites PASS on tree f337454abe5c90ac34337b76fecd9b13d7e7b00c (full hook suite).
- 2026-11-09: M10z-r4: A jumps off the face (pin cap released, 1.8 m normal launch); drop undo skipped while walking + face-move diagnostic; WS stop gives a small hop into the face-as-ground mode. Cloud suites PASS on tree ff5bdc6305b26fabeaced0a1f953fff3a67bfff8 (full hook suite).
- 2026-11-09: M10z-r5: WS stop pushes 0.9 m off the face for 0.25 s then lands (replaces the A injection); per-hold samples + face-jump samples. Walking snag found to be game-side (no store corrections while walking). Cloud suites PASS on tree 97830dc38dbc6835a3459bd2a5dbe82c15d0b719 (full hook suite).
- 2026-11-09: M10z-r6: stop push at every non-jump hold near the face; downward steps undone for 1.5 s after a landing (no slide while the stick is held). Cloud suites PASS on tree e9087b13f8ff5292c46c2c1ad8ce735142f6597d (full hook suite).
- 2026-11-09: M10z-r7: WS camera re-engaged on the current camera manager during a gravity hold (camera_manager_changed returned it to vanilla). Cloud suites PASS on tree b50d8d69e40551cfd38ab30e3978337b194729e3 (full hook suite).
- 2026-11-09: M10z-r8: camera manager kept during a gravity hold (30 m live check dropped it); one-time code capture of the position writer and callers for the jitter trace. Cloud suites PASS on tree 0bcc828de8c5bedf4c640b1fd67d21ee02b52ac4 (full hook suite).
- 2026-11-09: M10z-r9: wall gravity driven through the hero mover (RVA 0x3519E10 desired position; collision kept), velocity-offset diagnostic; code trace: 0x2CFC900 = Transform::SetPosition. Cloud suites PASS on tree be7d97ef92b19abe18438d9853d94b5c17d917c9 (full hook suite).
- 2026-11-09: M10z-r10: mover editing off (hero mover not per-frame in holds; it idled the thread), diagnostic call intervals; the air A action no longer ends the kept gravity. Cloud suites PASS on tree 2bf42332421026f0dd1a2709789297173d42cfca (full hook suite).
- 2026-11-09: M10z-r11: fallback plane at the estimated wall, B stops position stores at once (no fall-through), air A replaced by a face-plane dash along the camera forward. Cloud suites PASS on tree 56dc59af1ef07a185c971b42189d4b0aab43a602 (full hook suite).
- 2026-11-09: M10z-r12: no store without a correction (the race that caused the face snag; owner irregular smooth run), air dash only on a new A press while airborne. Cloud suites PASS on tree 1fd537d35c08c83a2335337bde50bbd8a3559685 (full hook suite).
- 2026-11-09: M10z-r13: face jumps game-native (no unpin/launch/dash; 0.6 m cap removed), WS-jump air dash uses the hero facing. Cloud suites PASS on tree 497caa9290bdfb4d74bdea74def013b6f0785a3b (full hook suite).
- 2026-11-09: M10z-r14: face jump uses the game's arc rotated into the face frame (no roof ratchet), dash along the face walking / WS travel direction (also in face jumps). Cloud suites PASS on tree 1cf166ea5fae471bcdbb7d67c19a1e8752c05383 (full hook suite).
- 2026-11-09: M10z-r15: hero position computed inside Transform::SetPosition (game thread, no race/flicker), face jump only from the face, stick hidden during face jumps with the takeoff walking velocity kept. Cloud suites PASS on tree 189d70b3826c29c68e664822973f0f81cf5f50cb (full hook suite).
- 2026-11-09: M10z-r16: every SetPosition call processed (same-frame calls reuse the correction), face jump rotates the full game step incl. its air control and A action, takeoff walk velocity unrotated, own dash disabled. Cloud suites PASS on tree 883e103a7e8ef1b910f498eb0537739f6ca01761 (full hook suite).
- 2026-11-09: M10z-r17: per-call drop undo on the face (slips), face jump = game arc along the normal + own stick/camera air control, face-jump step diagnostic; r17b web-zip redirect onto the face. Cloud suites PASS on tree d5c93d768e11e831c3470402f29af8644fbeff9d (full hook suite).
- 2026-11-09: M10z-r18: face jump fully modelled with the verified WS camera mapping, web zip impulse on A, downward motion on the face limited to walking speed. Cloud suites PASS on tree 77befdbff4f07e00c2ddc92acff66f5a6fbd58dd (full hook suite).
- 2026-11-11: M10z-r19: no false landing 0.87 m off the face after the stop (mover-only rule), face jump direction from the stick via the verified camera mapping, face-jump input log. Cloud suites PASS on tree bfdadbae6971325cc8ae2ac5f7b932326de96bb1 (full hook suite).
- 2026-11-11: M10z-r19 owner result analysed (analysis/M10Z_R19_RESULT_20261111.md): screen axes 90 deg off in the face state, downward walking always undone, stop keeps the game's fall state. No code change; next task M11 written to docs/CODEX_NEXT.md for Codex (old CODEX_NEXT archived).
- 2026-11-11: M11 owner result analysed (analysis/M11_OWNER_RESULT_20261111.md): task 2 accepted; camera manager position stale in the face state (direction source invalid); stop failures rest 0.56 m off the wall. No code change; M12 written to docs/CODEX_NEXT.md for Codex.
- 2026-11-11: M12 owner result analysed (analysis/M12_OWNER_RESULT_20261111.md): stop succeeds 3/3 with triple landing, re-approach loop every 0.4 s away from the hold start, walking-down steps clipped by the 6 m/s budget, jump direction consistent with walking but slow, zip mapping unstable. Owner: stop stable, remove its jerks, other items in parallel. No code change; M13 written to docs/CODEX_NEXT.md for Codex.
- 2026-11-11: M13 owner result analysed (analysis/M13_OWNER_RESULT_20261111.md): stop lands once but ends in the crawl (no air phase for the game), walking-down hitch caused by a 2 ms frame boundary (fall guard dt ~2 ms), zip uses a wrong frozen theta, hovering past the wall edge from the infinite plane clamp. No code change; M14 written to docs/CODEX_NEXT.md for Codex.
