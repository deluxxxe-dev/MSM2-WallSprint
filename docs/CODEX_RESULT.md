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
