# WallSprint v0.1c9 real-game runtime result

## Test

- Build: Spider-Man2.exe 2.810.0.0, accepted SHA256.
- Mod: v0.1c9 / package 0.1.27.
- Surface: current large unobstructed A-direction planar wall.
- User visual observation: surface-relative orientation remained active through the full available wall run.

## Result

c9 is a real-game PASS for the current A-wall control case.

Authoritative lifecycle summary:

```text
wallrun_duration_ms=3844
source_entry_elapsed=0.719000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=239
path_b_writes=238
retention_pass=1
max_held_ms=2859
max_held_updates=221
last_actor_up_dot_surface=1
stale_suspend_count=0
stale_resume_count=0
first_hard_kill=none
hard_kill_calls=0
```

The independently verified WallRun lifecycle also closed normally:

```text
Init=1
Entry=1
Update=300
Deinit=1
duration=3.836902 s
end_reason=verified_deinit
```

## Secondary-thread Path B

The c9 fix was exercised heavily in real runtime, not merely incidentally.

```text
PATH_B_SECONDARY_THREAD_SUMMARY
seen=193
admitted=193
overridden=193
rejected=0
unique_threads=16
thread_table_saturated=0
overflow_events=0
```

A final Path B override near elapsed 3.813 s still ran on thread 2564 while the expected native thread remained 36500. It passed the exact caller / current-thread stack / compact-layout / lifecycle / identity / integrity checks and applied Surface Up without migrating native ownership or killing the session.

This confirms the c8 fatal invariant `Path B thread == native WallRun thread` was false in real runtime, and c9's per-call secondary-thread admission is required for stable hold.

## Orientation retention

Retention passed at 250 ms / 20 native updates and continued to Deinit. Final observed Actor Up remained aligned to Surface Up (`dot=1`).

## Position / planar standoff

Position remained read-only:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
```

Across 29 adhesion samples on this wall:

```text
max_abs_error_before=2.98023224e-08 m
target_distance=0.5 m
```

Therefore vanilla WallRun already maintains the ~0.5 m standoff essentially exactly on this planar control wall. There is no current evidence that active normal-direction adhesion correction is needed for this case.

## Gate decision

Orientation lifecycle on the A-direction control wall is now considered PASS through verified Deinit.

Do not immediately add active Position correction. Before introducing new locomotion writes, re-run the same c9 build on a diagonal E-type planar wall to verify the now-stable lifecycle remains surface-normal-generic rather than accidentally A-specific.

If E also reaches full Surface Up, retains to Deinit, and has no hard kill/Position corruption, proceed to the next v0.1 locomotion milestone. Active adhesion should remain optional / deferred unless measured standoff error becomes materially nonzero once movement control changes.

Raw user log/video are private and are not committed.