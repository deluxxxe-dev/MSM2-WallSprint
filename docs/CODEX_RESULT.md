# Result — V8 consumer thresholds accepted; c48 exact Update-entry discriminator implemented

2026-11-01. Active implementation branch: `codex/v01c20-edi-value-provenance`. No main merge.

## STATUS

**C47 `WallRun.this+0x250` RUNTIME CORRELATION RETAINED / V3-V7 SELECTED OWNER-CALLEE PRODUCER CHAIN EXHAUSTED WITHOUT A DIRECT WRITER / V8 PROVES TWO AUTHENTICATED FLOAT THRESHOLD CONSUMERS (`0.5` AND APPROX `0.71`) / C48 READ-ONLY EXACT UPDATE-ENTRY THRESHOLD COUNTER IMPLEMENTED / NO WRITE AUTHORIZED.**

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

Owner returned:

`MSM2-WS-wallrun250-consumers-20261101-134046.json`

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

Detailed analysis:

- `analysis/WALLRUN250_V8_CONSUMER_RESULT_20261101.md`

## c48 implementation

Runtime/package:

- `v0.1c48`;
- package `0.1.73`.

c48 retains c47 worker scalar observation and changes no gameplay write gate.

The existing authenticated Update pre-original callback `ws_write_update_pre` was previously a no-op returning `0`. c48 keeps the return value `0` and uses that existing callback only as a read-only diagnostic point.

When the complete verified WallRun observation plan is admitted:

1. worker-side admission reads the two live consumer-threshold globals at RVAs `0x5303DE8` and `0x5337098`;
2. each post-bridge-admitted Update callback performs one guarded 4-byte read of `owner+0x250`;
3. atomics count read success/failure, finite/NaN samples, and the two exact comparison conditions (`field > threshold` or unordered);
4. no logging is performed on the native Update thread;
5. the worker drains only cumulative/final summaries and rechecks the live threshold globals;
6. any update-bridge rejection disables the probe fail-closed;
7. old Actor write dispatch remains disabled, `ws_write_update_pre` still returns `0`, `ws_write_update_post` remains empty.

No extra hook, pointer dereference, game call, field write, transform write, gravity write, or native-source write was added.

## Files changed for c48

- `source/src/msm2/actor_writer.cpp`
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
- guarded read through `msm2::readMemory`;
- no log call on the native Update callback;
- no `WriteProcessMemory` / remote-thread / context-write API;
- write callback still returns `0`;
- post callback remains empty;
- direct Actor writer remains inactive;
- c48/0.1.73 package/config/readme alignment.

The normal installed updater must still perform the repository's full Windows/WSL build, portable tests, smoke, hooks and package validation before runtime use. Do not claim a successful c48 build until that updater passes on the owner machine.

## Next runtime purpose

One ordinary WallRun sequence is sufficient:

right -> reverse left -> reverse right -> hold down until the known left/right oscillation -> end WallRun -> normal game exit.

No video is required unless behavior differs from the already documented sequence.

Acceptance requires:

- `wallrun_0x250_threshold_probe_admission admitted=1`;
- V8 expected threshold bits match at admission;
- live threshold rechecks remain unchanged;
- read failures remain zero;
- compare `exact_update_pre_original_samples` with the verified WallRun Update count to establish coverage;
- inspect both threshold counters.

If both counters are zero with complete coverage, the c47 correlated negative pulses did not activate either authenticated threshold consumer during this sequence. If either is nonzero, analyze that bounded crossing evidence next. Neither result authorizes a write by itself.
