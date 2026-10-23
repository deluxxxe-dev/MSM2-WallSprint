# Codex result — WallSprint v0.1c4

Branch: **codex/v01c4-real-thread-enum-fix**.
Implementation commit: **cfaae52304dfa757f2098eeb63c0168151c5b755**.
Base: published c3 **b05278093b6e0ad92db7dad8c51a986ba16ca872**. The pre-created branch retained that exact implementation. Main handoff commits599f9a5/f1859e1/cd00c92 were cherry-picked; PROJECT_STATE/CODEX_NEXT content is unchanged from that handoff. No main rewrite or force push.

## Outcome and evidence boundary

v0.1c4 / package **0.1.22** repairs demonstrated pre-expansion diagnostic pressure stops and instruments the actual production helper/Toolhelp path. Real c3 A/B overrides and flicker remain authoritative; expanded coverage failed. Its precise real-game stop cause remains **UNKNOWN**, because the supplied summary lacks attempt/exit telemetry. The reproduced mechanism is not proven private-run causality.

The exact published c3 source reproduces capture=false and seen/eligible/installed=0 when its1024-entry common ring fills before expansion. Its poll releases the window before testing>=850ms. Prior c3 fixtures began above threshold and missed this interval. Event-ring/budget exhaustion had equivalent early-close paths. c4 keeps the coordinator alive under record pressure and limits pre-expansion observations to128/512 events, reserving384 for expanded evidence. Drops remain visible and mark coverage incomplete.

## Implementation and changed files

- `source/src/msm2/orientation_trace.cpp`: scheduled/attempt/enum/result and final requested/attempted/completed/failed telemetry; actual Toolhelp entry/owner-match counts, PID/TID/helper exclusion, immediate failed API/error. Verified elapsed>=850ms triggers one pass, with no rescan. Bounded per-reason/error skip summaries retain representative TID, counts and suppressed totals. Primary/install/readback/resume failure stages are retained. Common/event ring pressure and event-budget exhaustion drop records without closing the multithread coordinator.
- `source/src/NativeHooks/immediate_reset_trace.cpp`, `source/include/msm2/orientation_trace.hpp`: pass the failing live-gate name to cleanup. Guard/lifecycle availability/native freshness/generation/owner/Hero/Transform/cancellation/kill/deadline checks still stop capture; invalid windows never reopen to force coverage.
- `immediate_reset_smoke.cpp`, `third_writer_trace_smoke.cpp`, `tools/test_reset_trace.sh`: actual async production helper threshold and real Toolhelp filtering/install on owned OS threads; sparse samples, pre-expansion common-ring/event-budget pressure, OS errors and cleanup. Classification and once/session bounded identity/hash/callers remain.
- Version/header/mod/telemetry, root/source README_TEST, README/BUILD/WORKLOG and packaging recipes updated. Required V01C3_RESULT, REAL_THREAD_ENUM_FIX, V01C4_GATE and authoritative runtime-result mirror are included in root/source analyses.

The native_source_override.cpp, reset_copy_source.cpp, compact_map_proof.cpp, stack_source.cpp and source_lifecycle.cpp implementations are unchanged from c3. Exact executable2.810.0.0/SHA, proof/authentication/current patch ownership, raw Win64 tailjump ABI, lifecycle/Hero/Transform/native-thread/stack/freshness/kill gates, lock-free A/B snapshot, Path A Position and Path B first0x24-only/row-scale/metadata behavior remain intact. No third-path patch, direct Actor/Position watcher write, new hook, page protection/PAGE_GUARD, direct game call or later milestone is added. Foreign DR0/1/GD refusal, DR2/3 coexistence, balanced suspension/resume and exact owned-state restore remain.

## Verification

Completed exit0:

- Windows x64 build with WallSprint warnings as errors.
- Portable ASan/UBSan: **11 PASS**.
- DLL smoke: **4 PASS**.
- Full hooks/offline/actual-byte/native-thread suite: **34 PASS**, **4 explicit historical private-fixture SKIPs**. Final diagnostic aggregation changes were followed by another Windows build and both affected trace suites, all PASS.
- Actual accepted-build writer444/caller128 regression ran: complete9-pair proof, authenticated production installer/owned restoration and negative mutations remain PASS. No original game code executes.
- Production async helper starts near.60, remains unexpanded below.85 and invokes the real enumeration/filter/install once. Fixture observed4 matching process threads,3 eligible/installed and1 excluded helper. Exact host totals vary; assertions require total>1/matches>1/installed>1.
- Guard loss before threshold gives its exact stop reason without enumeration. Sparse polling from below.85 to approximately.93 still expands once after common-ring saturation and128-event pre-expansion cap, then captures a second-thread unknown reset. Pressure does not suppress A/B commit timestamps. Existing foreign-slot/full saved debug state, bad stack, timeout/Deinit/identity/F8/shutdown, Position/page protection checks remain PASS. Invalid OpenThread returns a nonzero OS error.
- Exact published c3 source reproduces the old common-overflow silent stop in a separate owned-memory run.

A fresh standalone source ZIP extraction rebuilt the Windows DLL successfully with private captures absent. Final packaging validation passes archive CRC, current complete source/docs/licenses, nested script contents/hash, manifest gates and private full-game-byte exclusion. Only selected cloud validation summaries are distributed; raw game input and synthetic full-function logs are excluded.

## Outputs and human gate

- **WallSprint_v0.1c4_real_thread_enum_fix.zip**
- **WallSprint_v0.1c4_source.zip**
- refreshed root **WallSprint.script** / **BUILD_MANIFEST.json**

Real c4 process coverage, third-writer identity and retention are **PENDING**. This is an enumeration/diagnostic repair, not a claimed flicker fix. Snapshot coverage excludes later-created threads; caps, foreign slots, access/setup failures and drops can miss a writer.

Confirm startup proof/admission. Perform one left-to-right WallRun around1.5–2s on the same wider near-A wall (N398 approximately -0.113317,0,+0.993559). Minimum: A/B counts>0; EXPAND_ATTEMPT trigger_seen=1; ENUM_RESULT total>1/matches>1; watch attempted=1/seen>1/eligible>0/installed>1; cleanup_pending=0. Unknown event/identity is strong additional success, not the minimum enumeration gate. E only if coverage succeeds but the unknown writer remains uncaptured. Return the complete log privately through normal ChatGPT handoff, plus flicker/crash/position observations. No raw log/video/function capture belongs in Git or release ZIPs.

Retention still requires dot>=.98 for>=300ms OR>=20 distinct native updates after both commits. Third-path modification, adhesion, movement, animation and camera remain blocked.

## Persistence

The implementation is in the exact SHA above. This result, refreshed release artifacts and release hash record are committed afterward on the same branch, then pushed and verified by authenticated Git retrieval. Final user links use that delivery commit: GitHub raw ZIP for direct download and GitHub blob README_TEST for test instructions.
