# Codex result — WallSprint v0.1c3

Branch: **codex/v01c3-third-writer-trace**.
Implementation commit: **fd105be35a12c4a4ba8b8a72c3f75a4bcf49ff82**.
Base: published c2 head **727fcdc1594e36e6a1ffaabf5569eee89375b871**, including implementation2d26710034dcf695fa7bb2da763f117faf6171db. The pre-created c3 branch's base is preserved; the latest c2 runtime/state/c3 task were cherry-picked. Documentation conflicts were resolved to the exact origin/main handoff. No main rewrite or force push.

## Outcome

v0.1c3 / package **0.1.21** adds the requested diagnostic process-thread Hero Up watch while retaining working c2 A/B overrides. It identifies the first unexplained SurfaceUp-to-WorldUp write in owned multithread hardware fixtures. The actual game writer identity remains **UNKNOWN until the c3 human test**.

Outputs:

- **WallSprint_v0.1c3_third_writer_trace.zip**
- **WallSprint_v0.1c3_source.zip**
- refreshed root **WallSprint.script** / **BUILD_MANIFEST.json**

## Implementation

- `source/src/msm2/orientation_trace.cpp`: primary prearm remains around elapsed0.60. One process-thread snapshot expands the watch at elapsed>=0.85 with an absolute1.50 end. Process candidates cap at128, skipped TID/reason summaries at16, event budget512/session. The helper itself is excluded. No continuously polled enumeration or thread-creation hook.
- Each eligible handle is verified as belonging to this process. Protected fixed-lease DR0/8-byte+DR1/4-byte setup has readback, balanced suspension/resume, partial rollback and retained cleanup. Active foreign DR0/DR1/GD is skipped; foreign DR2/DR3 can coexist and is preserved. A busy primary does not prevent c3 from watching other eligible threads.
- Timeout, lifecycle/identity/guard loss, cancellation, F8/shutdown and failed setup close capture and restore leases. Saved debug state is restored when still owned; foreign takeover is preserved and reported. Immutable tombstones and the pinned VEH remain available for delayed exceptions. No PAGE_GUARD or trace-related page-protection change.
- Each captured Up write receives module/function classification (KNOWN_PATH_A, KNOWN_PATH_B, UNKNOWN_WRITER), bounded validated callers and separate last-A/last-B override timing. Diagnostic lock contention may drop a record but does not suppress A/B commits or stop c3 capture. Drops mark coverage incomplete.
- THIRD_WRITER_EVENT is once/session after both sources commit: prior surface dot>=.90, later dot<=.50, and movement toward World Up. Partial scalar-store sequences can use a surface observation from<=5ms earlier with explicit non-atomic before_semantics. A known function reached through a different caller/thread is not silently excluded as an unexplained path.
- THIRD_WRITER_IDENTITY records executing thread, RIP, module, function bounds/size/SHA and callers. At most16KiB of the one selected function can be written to the PRIVATE local runtime log as THIRD_WRITER_CODE private_do_not_distribute=1. It is never included in the release. Runtime bytes may include an existing owned hook and are not labeled original bytes.
- `immediate_reset_trace.*`, `orientation_trace.hpp`, `telemetry.cpp`: enable this mode only after existing build/proof/admission gates; retain helper ownership of setup/cleanup.
- `third_writer_trace_smoke.cpp` / `test_third_writer.sh`: actual hardware/VEH tests on owned native functions and memory. Existing trace fixture now synchronizes on native-loop readiness instead of a fixed20ms sleep; no assertion or production gate was relaxed.
- Required V01C2_RESULT / THIRD_WRITER_TRACE / V01C3_GATE and runtime evidence mirror are included under root/source. Version, README_TEST, BUILD, WORKLOG, package/release recipes and manifest are updated.

The c2 native_source_override.cpp, compact_map_proof.cpp, reset_copy_source.cpp and stack_source.cpp implementation files are unchanged. Exact exe version/SHA, compact proof, caller/hash, lifecycle/Hero/Transform/native-thread/stack/freshness/kill, Path A Position preservation, Path B first0x24-only row-scale/metadata preservation and lock-free admission remain intact. Retention remains dot>=.98 for300ms or20 distinct native updates.

No third-path patch, direct Actor/Position, adhesion, movement, animation, camera, physics, gravity, parkour, swing or combat write is added.

## Verification

Completed with exit0:

- Windows x64 build with WallSprint warnings as errors.
- Portable ASan/UBSan: **11 PASS**.
- DLL smoke: **4 PASS**.
- Full hook/offline/actual-byte/multithread suite: **32 PASS**, **4 explicit historical private-fixture SKIPs**.
- The actual accepted-build c2 writer444/caller128 regression ran and retained all9 proof pairs and production installer authentication/restoration.
- c3 fixtures installed2+ threads, captured the second-thread unknown function/hash/caller, emitted only one selected event/session, and captured sequential component resets after elapsed1.10. They verified active foreign DR0 refusal, foreign DR2 coexistence, exact previous DR0/1/2/3/6/7 restoration, busy-primary fallback, bad stack refusal, diagnostic contention, timeout/Deinit/identity/F8/shutdown cleanup, and unchanged Position/page protection.
- One early legacy Wine trace run stalled; its standalone retry exposed zero hits with fixed startup timing. Both hardware fixtures now await explicit thread readiness and the complete final run passed.
- Packaging checks cover DLL/version/default INI, PE32+/exports/system imports/no WriteProcessMemory, archive CRC/hash, complete WallSprint source/build recipes and licenses, and raw private-game-input exclusion.
- Fresh standalone source ZIP extraction rebuilt the Windows DLL successfully with private captures absent.

Only known cloud validation summaries are selected for release logs. The private game input, actual raw user log/video and the synthetic trace's full function dump are not packaged.

## Real-game gate and next test

PENDING: actual c3 third writer identity / repetition / clean real-game cleanup. Snapshot coverage does not cover threads created afterward; foreign DR, caps, failed access/setup, drops and sparse observations can still miss a writer. No captured event does not prove absence.

First confirm c2 compact startup proof still admitted with writer444/caller128/pairs9. Then perform **one A Control left-to-right WallRun about1.5–2s**. Require A/B counts>0, MULTITHREAD_UP_WATCH installed>1, THIRD_WRITER_EVENT and THIRD_WRITER_IDENTITY, and cleanup_pending=0. Test E once only if A misses the event. Send the local log privately through the normal ChatGPT handoff, and note flicker/crash/position issues. Do not publish THIRD_WRITER_CODE or raw logs.

Visible flicker may continue because c3 deliberately observes the remaining path. Brief Surface Up is not retained ownership and grants no adhesion permission.

## Persistence

This result and the refreshed packages are committed after the implementation SHA above, then the dedicated branch is pushed and its remote artifact hashes verified. PROJECT_STATE / CODEX_NEXT remain the latest handoff content and were not rewritten by implementation.
