# WallSprint v0.1c3 real-game runtime result

## Human test

- Package: `v0.1c3 / 0.1.21 Third Reset Writer Multithread Trace`
- User tested on a wider planar wall facing approximately the same direction as prior A Control.
- This run's measured normal was `N398 ~= (-0.113317, 0, +0.993559)`, so it is close to +Z but not exactly the previous cardinal A normal.

## Startup / existing orientation paths

Target build gate passed. c2 compact proof and dual-source admission remained enabled.

Path A and Path B both committed in the real game. Representative early session values:

```text
PATH_A_OVERRIDE_COUNT > 0
PATH_B_OVERRIDE_COUNT > 0
PATH_A_STATE_LOCK_CONTENTION=0

NATIVE_SOURCE_OVERRIDE ... target_up=(-0.00552367,0.99495095,0.10021003)
PATH_B_COMPACT3X3_OVERRIDE ... applied_up=(-0.00552367,0.99495024,0.10020995)
```

As interpolation advances, Path A/B continue producing increasingly surface-relative Up values. The known flicker/reset behavior remains; several later Path A entries arrive with original WorldUp and are converted again (`LATE_COMMON_WORLDUP_OVERRIDE=1`).

No `ORIENTATION_RETENTION_PASS=1` was observed.

## c3 diagnostic failure

The new multithread watcher did **not** actually enumerate or arm any additional process threads in the real game.

Runtime summary:

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

The primary native thread watch itself did remain functional and captured only the already-known Path A writer (`0x2CFC580 / caller 0x2CFC8F3`).

No `UNKNOWN_WRITER`, `THIRD_WRITER_EVENT`, `THIRD_WRITER_IDENTITY`, or `NEW_RESET_AFTER_DUAL_OVERRIDE` evidence was captured.

Therefore this run does **not** disprove the third-writer hypothesis. The c3 real-game blocker is the process-thread enumeration/expansion path itself.

## Important distinction

Cloud/native fixtures reported that c3 can install 2+ hardware watches on owned test threads, but the actual game run reached `process_threads_seen=0` with no explicit enumeration failure reason in the log. The next task must reproduce/fix the real-game enumeration/activation path rather than change orientation semantics.

## Next target

v0.1c4 should preserve all c3/c2 A/B behavior and repair/diagnose real-game process-thread enumeration so the bounded Hero Up watch truly covers additional game threads.

Required next-run evidence before claiming success:

```text
MULTITHREAD_UP_WATCH phase=expand_attempt
process_threads_seen > 1
eligible > 0
installed > 1
```

If enumeration fails, emit exact bounded diagnostics including snapshot/open result and OS error code. Only after multithread coverage is real should the project continue to third-writer identity capture.

Do not patch a third path, and do not advance to adhesion/movement/camera yet.
