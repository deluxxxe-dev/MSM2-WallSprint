# WallSprint v0.1c4 real-game runtime result

## Human observation

Tested on a still wider planar wall aligned with the original A direction. The prior rapid alternating/flicker between vanilla WallRun orientation and surface-relative orientation was no longer visually apparent.

## Build / startup

- package: v0.1c4 / 0.1.22
- Spider-Man2.exe 2.810.0.0
- accepted executable SHA256: c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
- target build gate: ACCEPTED
- Path B compact proof/admission remained valid.

## c4 thread enumeration fix — PASS

The production Toolhelp path now actually enumerates and installs hardware watches in the real game:

```text
MULTITHREAD_ENUM_RESULT
snapshot_created=1
thread32first_ok=1
entries_total=6495
owner_pid_matches=129
cap_reached=1
win32_error_stage=none
win32_error_code=0

MULTITHREAD_UP_WATCH phase=armed
process_threads_seen=128
eligible=127
installed=126
failed=1
cap_exceeded=1
coordinator_self_excluded=1
```

One thread was skipped as already suspended. Cleanup restored all 126 owned watch leases with cleanup_pending=0.

No THIRD_WRITER_EVENT / THIRD_WRITER_IDENTITY was emitted in this run.

## First real orientation-retention PASS

The control wall measured essentially exact +Z during the successful session:

```text
N398 ~= (0,0,+1)
```

Path A and Path B both converged to Surface Up and Actor Up remained surface-relative continuously long enough to pass the existing gate:

```text
ORIENTATION_RETENTION_PASS=1
held_ms=329
held_updates=20
```

The same continuous sequence later reached at least 55 distinct held updates / about 969 ms without a WorldUp flicker.

This is the first authoritative real-game retention PASS.

## New blocker: native-source lifecycle exits while WallRun remains active

The successful source ownership did not persist for the full WallRun.

Last successful A/B commits occurred around elapsed 1.953 s:

```text
PATH_A_OVERRIDE_COUNT=69
PATH_B_OVERRIDE_COUNT=68
actor_up=(0,0,1)
stability_observed=1
```

Then WallSprint ended itself with:

```text
NATIVE_SOURCE_EXIT
session=2
reason=native_lifecycle_inactive_or_stale
duration_since_source_entry=1.235000
applied_writes=135
held_updates=55
held_ms=985
stability_observed=1
```

However the independently verified WallRun lifecycle remained active afterward. The same WallRun did not Deinit until about 3.900 s total duration.

By about WallRun elapsed 2.339 s, worker observation showed Actor Up back at World Up while `automatic_wallrun=Active`.

Therefore the current remaining orientation blocker is no longer the previously hypothesized mandatory third writer race. The demonstrated blocker is a false/early native-source lifecycle inactive/stale gate that stops A/B ownership during an otherwise valid continuing WallRun.

Do not delete the third-writer diagnostic capability; it remains useful if a true unexplained reset returns after lifecycle continuity is repaired.

## Interpretation

Current sequence:

```text
WallRun starts
  -> 0.7 s transition starts
  -> Path A + Path B rotate toward Surface Up
  -> Surface Up reaches full target
  -> retention PASS (>=329 ms / >=20 updates)
  -> remains stable for roughly 1 s
  -> native source lifecycle reports inactive/stale around 1.95 s
  -> A/B commits stop
  -> actor later returns to World Up while WallRun itself continues
  -> verified WallRun Deinit around 3.9 s
```

## Next task

v0.1c5 should diagnose the exact sub-gate behind `native_lifecycle_inactive_or_stale` and repair only that demonstrated continuity failure.

The goal is to keep the already-working A/B SurfaceFrame ownership active from the transition through the verified WallRun Deinit, without weakening executable identity, caller identity, Hero/Transform identity, native-thread, stack, freshness, or kill safety indiscriminately.

Adhesion/movement remain blocked until lifecycle-wide orientation ownership is demonstrated on the wide A control wall.
