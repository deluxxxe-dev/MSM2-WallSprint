# WallSprint v0.1c2 real-game runtime result

## Human observation

Visible change is now confirmed. During WallRun, normal world-up orientation and the intended surface-relative orientation switch rapidly back and forth.

User supplied video/screenshot and the authoritative `WallSprint(20261023-040924).log`.

## Startup gate

v0.1c2 startup proof succeeds on the accepted build:

```text
WallSprint v0.1c2 package_version=0.1.20
PATH_B_COMPACT3X3_PROOF admitted=1
caller_verified=1
writer_verified=1
rdx_to_rcx_map_verified=1
decoded_prefix_bytes=444
caller_decoded_bytes=128
pairs=9
```

Therefore the c1 decoder blocker is fixed.

## Dual-source writes are real

Path A and Path B both commit non-world-up SurfaceFrame bases in the running game.

Representative A-wall sequence:

```text
PATH_A / NATIVE_SOURCE_OVERRIDE applied_up -> (0,0,1)
PATH_B_COMPACT3X3_OVERRIDE applied_up -> (0,0,1)
NATIVE_SOURCE_ACTOR_OBSERVATION actor_up=(0,0,1)
```

Path B preserves the 0x24-byte write boundary and metadata gate.

The same behavior reproduces on the diagonal E wall. Representative E target:

```text
N398 ~= (-0.707107, 0, +0.707107)
PATH_B_COMPACT3X3_OVERRIDE applied_up ~= (-0.707107, 0, +0.707107)
actor_up_dot_surface ~= 1
```

## Critical failure: Actor Up is reset by an unobserved writer

After successful A/B Surface Up commits, the next worker observation repeatedly flips back to World Up:

```text
actor_up=(0,0,1)      actor_up_dot_surface=1
...
actor_up=(0,1,0)      actor_up_dot_surface=0
```

This occurs on both A and E.

Examples from session 1 show Surface Up held for only a few native updates (often ~0-5 distinct updates / <= ~31 ms) before a sudden World Up observation. Retention never reaches the required 300 ms / 20-update gate.

No `ORIENTATION_RETENTION_PASS=1` is present.

## Why the current watch misses the reset

The bounded hardware Up watch reports:

```text
threads_enumerated=0
installed=1
complete_thread_coverage=0
primary_native_thread=8968
primary_only=1
```

Observed primary-thread writer events during the active window are the already-known path-A common writer `0x2CFC580 / caller 0x2CFC8F3`, and those events write the requested Surface Up.

At the moments where worker observation jumps back to World Up, no matching hardware writer event is captured. Therefore the strongest current hypothesis is a third orientation writer executing on another game thread (or otherwise outside the primary-only hardware watch coverage).

This is stronger than the prior c2 decision-tree expectation because `NEW_RESET_AFTER_DUAL_OVERRIDE` cannot fire when the actual write occurs outside the armed thread set.

## Additional evidence

Path A lock contention is fixed:

```text
PATH_A_STATE_LOCK_CONTENTION=0
```

Late path-A WorldUp source invocations are detected and converted to Surface Up (`LATE_COMMON_WORLDUP_OVERRIDE=1`), yet Actor Up may still remain or return to World Up immediately afterward. Thus late path-A calls are not sufficient to explain the flicker.

Path B original source remains WorldUp-oriented, but the exact compact source is replaced with Surface Up and the actor is observed Surface-relative immediately after those commits. Path B therefore is not the surviving reset owner in these samples.

## Current conclusion

v0.1c2 establishes:

```text
startup compact proof              PASS
Path A SurfaceFrame override       PASS
Path B compact SurfaceFrame write  PASS
visible Surface-relative rotation  PASS (intermittent)
retained orientation               FAIL
unknown third reset owner          CURRENT BLOCKER
```

Do not add adhesion/movement/camera.

## Next diagnostic

v0.1c3 should preserve the working c2 A/B overrides and expand the Up write-watch to bounded multi-thread coverage long enough to capture the first SurfaceUp -> WorldUp transition after both overrides are active.

Required capture for the first unexplained reset:

```text
THIRD_WRITER_EVENT
thread_id=
rip=
writer_rva=
caller=
caller_rva=
caller2=
caller2_rva=
up_before=
up_after=
relative_us_from_last_path_a_override=
relative_us_from_last_path_b_override=
```

Do not write to the third path in c3. Characterize it read-only first.
