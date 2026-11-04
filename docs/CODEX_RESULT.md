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
