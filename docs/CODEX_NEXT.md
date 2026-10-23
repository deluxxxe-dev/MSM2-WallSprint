# Next task — WallSprint v0.1c4 Real-Game Thread Enumeration / Watch Expansion Fix

## Implementation base

Use the published c3 implementation, not old main source:

```text
base branch: codex/v01c3-third-writer-trace
implementation commit: fd105be35a12c4a4ba8b8a72c3f75a4bcf49ff82
published c3 head: b05278093b6e0ad92db7dad8c51a986ba16ca872
```

A next branch has already been created from that exact c3 head:

```text
codex/v01c4-real-thread-enum-fix
```

Read first:

- `analysis/V01C3_RUNTIME_RESULT.md`
- `docs/PROJECT_STATE.md`
- `docs/CODEX_RESULT.md` on `codex/v01c3-third-writer-trace`

## Authoritative c3 runtime result

The user tested c3 on a wider planar wall facing approximately the same direction as prior A Control. Runtime measured:

```text
N398 ~= (-0.113317, 0, +0.993559)
```

so treat it as a near-+Z planar control, not as exact +Z.

Existing c2/c3 orientation behavior remains alive:

```text
PATH_A_OVERRIDE_COUNT > 0
PATH_B_OVERRIDE_COUNT > 0
PATH_A_STATE_LOCK_CONTENTION=0
```

Path A and Path B both write interpolated surface-relative Up. The known WorldUp reset/flicker remains. No retention PASS occurred.

The c3 diagnostic failure is specific and authoritative:

```text
RESET_TRACE_MODE ... primary_first=1 multithread=1 expand_elapsed=0.85 max_threads=128
orientation_trace_arm ... primary_native_thread=23668 ... primary_only=0 expand_to_process_threads_at=0.85
...
MULTITHREAD_UP_WATCH session=1 phase=stopped
process_threads_seen=0
eligible=0
installed=0
skipped_foreign_dr=0
failed=0
complete_thread_coverage=0
rescan_count=0
```

The primary thread hardware watch still worked and captured known Path A events, proving the base watch mechanism itself was active.

No `UNKNOWN_WRITER`, `THIRD_WRITER_EVENT`, or `THIRD_WRITER_IDENTITY` was captured because no additional game thread was actually watched.

Cloud/native c3 fixtures reported 2+ watched test threads, so the next task is to find the discrepancy in the real production enumeration/expansion path. Do not interpret `process_threads_seen=0` as evidence that no third writer exists.

## Objective

Repair the real-game multithread watch expansion so it actually enumerates current Spider-Man2 process threads and installs the bounded Hero Up watch on safely eligible threads.

This is still diagnostic-first.

**Do not patch a third writer/source path in c4.**

Preserve working Path A and Path B overrides. No adhesion, movement, animation, camera, gravity, parkour, swing or combat.

## Required work

### 1. Trace the production expansion state machine

Find exactly why real c3 reached the stopped summary without any enumeration:

```text
process_threads_seen=0
eligible=0
installed=0
rescan_count=0
```

Do not guess that Toolhelp/OpenThread failed: the c3 log contains no sufficient attempt/error telemetry.

Instrument the actual production path with one bounded state-transition record per stage, e.g.:

```text
MULTITHREAD_EXPAND_ATTEMPT
session=
elapsed=
process_id=
coordinator_tid=
primary_tid=
trigger_seen=1

MULTITHREAD_ENUM_RESULT
snapshot_created=0|1
thread32first_ok=0|1
entries_total=
owner_pid_matches=
cap_reached=0|1
win32_error_stage=
win32_error_code=
```

Use equivalent names if clearer. The critical requirement is that a failed attempt has an explicit stage and OS error code, while a successful attempt shows how many entries were actually iterated and how many belonged to this process.

### 2. Make elapsed>=0.85 expansion impossible to silently skip

The production coordinator must perform at most one initial expansion when the verified active session crosses the configured threshold.

Do not depend on hitting an exact elapsed value. If the worker samples jump from below to above 0.85, expansion must still run once.

Do not let trace-stop/cleanup race ahead of an unattempted expansion while the verified session is still inside the intended diagnostic window.

Track explicit state such as:

```text
expand_requested
expand_attempted
expand_completed
expand_failed
```

and include it in the final summary.

### 3. Use the same enumeration function in production and test

The Windows native regression must exercise the exact production process-thread enumeration and filtering code, not a synthetic list injected around it.

Test with several real owned process threads and verify:

```text
entries_total > 1
owner_pid_matches > 1
installed > 1
```

Also test the production threshold/state-machine transition that calls that enumeration function.

### 4. Preserve safe debug-register handling

Keep c3 rules:

- only current process threads;
- cap <=128;
- do not overwrite foreign/in-use DR0/DR1/GD;
- preserve foreign DR2/DR3;
- balanced suspend/resume;
- exact owned-state restore;
- no PAGE_GUARD/page-protection changes;
- no direct game calls;
- no Actor/Position writes from the diagnostic watcher;
- helper/coordinator exclusion remains explicit and logged.

Log bounded per-reason skip/failure summaries. Never dump unbounded thread lists.

### 5. One bounded rescan only after a real first attempt

If retained from c3, the optional rescan must only occur after `expand_attempted=1` and must increment `rescan_count` visibly.

Do not use continuous polling.

### 6. Keep writer classification and third-writer capture

Once additional threads are truly watched, retain c3 classification:

```text
KNOWN_PATH_A
KNOWN_PATH_B
UNKNOWN_WRITER
```

and the existing once/session:

```text
THIRD_WRITER_EVENT
THIRD_WRITER_IDENTITY
```

If c4 captures the unknown writer after fixing coverage, record it, but **do not patch it in c4**. The next handoff will characterize/own it separately.

### 7. Keep A/B behavior unchanged

Do not alter the c2/c3 orientation interpolation or source semantics merely to improve the diagnostic signal.

Preserve:

- exact exe version/SHA gate;
- compact Path B proof;
- Path A Position preservation;
- Path B first 0x24-only boundary / metadata preservation;
- lock-free A/B snapshot;
- lifecycle/Hero/Transform/thread/stack/freshness/kill gates;
- no direct Actor spam writes.

## Required startup/runtime evidence

Startup still needs the known proof/admission PASS.

For the human c4 run, the new hard diagnostic gate is:

```text
MULTITHREAD_EXPAND_ATTEMPT ... trigger_seen=1
MULTITHREAD_ENUM_RESULT ... entries_total>1 owner_pid_matches>1
MULTITHREAD_UP_WATCH ... process_threads_seen>1 eligible>0 installed>1
```

If this fails, the log must contain the exact failed stage / Win32 error code.

A `THIRD_WRITER_EVENT` is strong success but is not required to prove the enumeration fix itself.

## Human test

Primary test can use the same wider near-A wall from the c3 run:

- one left-to-right WallRun, approximately 1.5–2 s;
- report visible flicker/crash/position issues;
- return the complete private WallSprint log.

Only test E if multithread coverage is successfully established but the third writer is still not captured on the control wall.

## PASS criteria

Minimum c4 PASS:

```text
c2/c3 startup proof still PASS
Path A override count > 0
Path B override count > 0
expand_attempted=1
process_threads_seen > 1
eligible > 0
installed > 1
cleanup_pending=0
```

Strong PASS:

```text
UNKNOWN_WRITER / THIRD_WRITER_EVENT captured
THIRD_WRITER_IDENTITY captured
```

Do not claim third-writer absence if coverage remains incomplete.

## Required outputs

Update/create:

```text
analysis/V01C3_RESULT.md
analysis/REAL_THREAD_ENUM_FIX.md
analysis/V01C4_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c4_real_thread_enum_fix.zip
```

Follow `AGENTS.md` / `docs/DEV_RULES.md`. Do not commit raw user logs/video or private full game-function captures.
