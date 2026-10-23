# WallSprint v0.1c5 real-game runtime result

## Scope

This document records the user's private real-game validation of v0.1c5. Raw user log/video are not committed.

Runtime package:

```text
WallSprint v0.1c5
package_version=0.1.23
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
target_build_gate=ACCEPTED
```

Control surface was the large A-direction planar wall. Runtime normal was effectively:

```text
N398 ~= (0,0,+1)
```

## Result: lifecycle-wide orientation hold PASS

The c4 premature source exit is gone.

The real WallRun lasted about 3.85 s. WallSprint source ownership began around 0.687 s and remained active until the verified native Deinit.

Authoritative end-of-session summary:

```text
ORIENTATION_LIFECYCLE_SUMMARY
wallrun_duration_ms=3843
source_entry_elapsed=0.687000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=219
path_b_writes=218
retention_pass=1
max_held_ms=2906
max_held_updates=205
last_actor_up_dot_surface=1
last_actor_observation_valid=1
```

The source exited only on the verified native WallRun Deinit:

```text
NATIVE_SOURCE_EXIT
reason=verified_native_Deinit
duration_since_source_entry=3.156000
applied_writes=437
held_updates=205
held_ms=2906
stability_observed=1
```

The independently verified WallRun lifecycle reported:

```text
duration=3.853269 s
Init=1
Entry=1
Update=264
Deinit=1
end_reason=verified_deinit
```

## Retention threshold

The normal retention gate passed early and remained valid for the rest of source ownership:

```text
ORIENTATION_RETENTION_PASS=1
held_updates=20
held_ms=297
```

Later samples continued with:

```text
Actor Up = Surface Up
actor_up_dot_surface=1
actor_up_dot_world=0
WallSprint=native_source_active
```

through the final active-WallRun observations.

## A/B ownership

Both known orientation source paths remained active beyond the previous ~1.95 s failure point and continued until Deinit:

```text
Path A writes: 219
Path B writes: 218
```

No premature `native_lifecycle_inactive_or_stale` exit occurred.

## Multithread diagnostic status

The bounded hardware watch again enumerated the real process threads:

```text
owner_pid_matches=129
process_threads_seen=128
eligible=127
installed=126
failed=1
cap_exceeded=1
```

The one failed install was an already-suspended thread. Coverage remains bounded/incomplete because of the 128-thread cap, so absence of an UNKNOWN/THIRD writer event is not formal proof of absence. However no unexplained reset survived during the lifecycle-wide retained interval.

## Gate decision

The orientation retention blocker is considered passed on the large planar A control wall:

- Surface-relative rotation: PASS
- Path A source ownership: PASS
- Path B source ownership: PASS
- threshold retention: PASS
- lifecycle-wide retention through verified Deinit: PASS
- no direct Actor Transform spam write: preserved
- no Position modification yet: preserved

This satisfies the project rule that adhesion must remain blocked until retained orientation is proven.

## Next milestone

Authorize the first planar adhesion milestone only.

The next task should:

1. preserve c5 orientation/lifecycle behavior unchanged;
2. authenticate a safe source-level Position path before writing Position;
3. use the already verified planar data `P338` and `N398`;
4. maintain the established standoff target near 0.5 m with bounded wall-normal correction;
5. keep tangent position components unchanged;
6. remain WallRun-lifecycle bounded and fail closed;
7. not add surface-relative locomotion input, animation, camera, gravity replacement, parkour, swing or combat yet.

If a safe source-level Position path cannot be proven, the next build must remain read-only for Position and emit enough evidence for a later implementation rather than falling back to direct Actor teleports.
