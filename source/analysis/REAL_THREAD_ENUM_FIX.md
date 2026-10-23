# c4 real process-thread enumeration and expansion

## Evidence and diagnosis

See V01C3_RESULT.md for real coverage failure, reproduced c3 failure mechanism, and unknown exact real-game early-stop cause. Production `immediate_reset_trace.cpp::run` calls `live`, then `trace::pollReset`. In c3, poll cleaned up capture/valid/deadline failures before checking the threshold. Common-record overflow cleared capture even when the verified session remained active; 512-event budget and full event ring had equivalent early-close behavior. Prior fixtures entered already above the threshold.

## State and records

- Begin emits one MULTITHREAD_EXPAND_SCHEDULED and primary setup stage/OS error. Primary prearm stays before enumeration.
- Verified `elapsed>=850ms`, not equality, moves requested/attempted once. Only the coordinator mutates expansion counters; attempted is atomically published for the VEH budget rule.
- MULTITHREAD_EXPAND_ATTEMPT records elapsed, PID, coordinator TID, primary TID and trigger_seen=1 before the snapshot API.
- `expandProcessWatch` is the production function used by native regression. It directly calls CreateToolhelp32Snapshot/Thread32First/Thread32Next, records entries_total and owner_pid_matches, filters current PID, caps process candidates at128 and excludes the helper. It collects a fixed array before suspending anyone. No injected thread list is used.
- MULTITHREAD_ENUM_RESULT reports snapshot/first results, entry/match counts, cap and immediate failed API/error. Normal ERROR_NO_MORE_FILES is accepted. Installation failures retain their stage/OS error before cleanup changes LastError. Logical refusals such as foreign DR use code0; readback ownership mismatch uses ERROR_INVALID_DATA. Resume failure retains its OS error and cleanup lease.
- expand_completed means the single pass returned, including a failed pass. expand_failed indicates enumeration/installation failure, empty eligible set or cap. Foreign-DR skips separately make coverage incomplete. Final summaries expose requested/attempted/completed/failed. rescan_count=0: no rescan or continuous enumeration is added.

## Pressure and termination

For multithread diagnostics only, common/event ring saturation and event-budget exhaustion increment drops without cancelling the coordinator. Before expansion, at most128 of512 events are retained;384 are reserved for expanded coverage. No buffer grows or counters reset. Suppressed records make complete_thread_coverage false. Legacy non-multithread termination remains. Independent A/B timestamps are published before diagnostic locking, unchanged.

Guard/lifecycle/generation/owner/freshness/Hero/Transform failures, F8, cancellation, QPC failure, debug-slot takeover and the absolute1.50 deadline still close the watch. Invalid windows never reopen to force PASS. The helper passes its failing live-gate name to cleanup; lifecycle_unavailable means unavailable evidence, including possible contention, not proven Deinit. Final state explains unattempted stops. No game gate is weakened. Scheduling may still delay polling past deadline; that reports deadline, not fabricated coverage.

## Safety and unchanged behavior

Current-process handle identity, <=128 candidates, foreign DR0/DR1/GD refusal, foreign DR2/DR3 preservation, balanced suspend/resume, guarded setup/readback/rollback, fixed leases, exact owned-state cleanup and delayed-VEH tombstones remain. No log/allocation/SDK/user lock is added inside suspended regions. No page protection/PAGE_GUARD, direct game call, Actor/Position watcher write, new hook, or third-source patch is added. Path A/B source/proof/stack/lifecycle implementation files are unchanged from c3. Classification and once/session bounded private writer capture remain.

## Native regression

`tools/test_reset_trace.sh` runs `immediate_reset_smoke.exe --multithread`: the actual asynchronous production helper starts near.60, remains unexpanded below.85, then invokes real enumeration/filter/install once with multiple owned OS threads. It requires entries_total>1, owner_pid_matches>1, installed>1, helper exclusion and clean restore. Another session loses the observation guard before threshold and must report that exact stop without an attempt.

`tools/test_third_writer.sh` exercises production poll with sparse samples from below.85 to approximately.93. It fills the common ring and pre-expansion event budget first. The watch must survive, enumerate/install and capture a second-thread unknown reset using reserved records. Repeated polls must not repeat enumeration. Existing once/session identity/hash/callers, sequential writes, foreign slots/complete saved debug state, busy primary, contention, bad stack and timeout/Deinit/identity/F8 cleanup checks remain. Invalid OpenThread must return nonzero OS error. Position/page protection stay unchanged.

These are owned Windows fixtures under Wine, not Spider-Man2 proof. Real-game c4 coverage and third-writer identity remain pending.
