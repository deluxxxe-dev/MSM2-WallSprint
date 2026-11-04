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
