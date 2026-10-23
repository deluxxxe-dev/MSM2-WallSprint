# v0.1c6 real-game runtime result

Test package: `WallSprint v0.1c6 / package 0.1.24 Planar Adhesion Diagnostics`

User report: the surface-relative orientation appeared to release partway through the long A-wall run.

## Result

The report is confirmed by the private runtime log.

The c6 adhesion milestone itself remained read-only:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
mode=position_read_only
```

Therefore the visible release was not caused by Position correction or adhesion writes.

## Exact regression

The verified WallRun remained active, same session/owner/Hero/Transform, but the native-source lifecycle permanently exited on the freshness sub-gate:

```text
NATIVE_LIFECYCLE_EXIT_DIAG
session=1
elapsed=2.642367
failing_sub_gate=fresh_update_age
ctx_wallrun_active=1
ctx_wallrun_session=1
ctx_wallrun_owner=0x1d30038ed80
lifecycle_read_status=coherent_active
token_phase=3
token_last_kind=2
token_owner=0x1d30038ed80
token_updates=191
token_update_ms=483447203
now_ms=483447453
update_age_ms=250
fresh_phase_ok=1
fresh_owner_ok=1
fresh_updates_ok=1
fresh_time_order_ok=1
fresh_update_age_ok=0
owner_matches_ctx=1
hero_matches=1
transform_matches=1
source_transition_active=1
accepted_event_epoch=0
native_collision_epoch=0
```

Immediately afterward:

```text
NATIVE_SOURCE_EXIT reason=native_lifecycle_inactive_or_stale
```

Yet the independently verified WallRun hook continued and reached real Deinit later.

Final summary:

```text
wallrun_duration_ms=4078
source_entry_elapsed=0.703000
source_exit_reason=native_lifecycle_inactive_or_stale
source_active_until_deinit=0
verified_deinit=1
path_a_writes=132
path_b_writes=130
retention_pass=1
max_held_ms=1422
max_held_updates=114
last_actor_up_dot_surface=1
```

The full WallRun lasted about 4.09 s, so source ownership ended about 1.4 s before verified Deinit.

## Timing clue

The last active source observations were followed by a roughly 250 ms native-update freshness gap. No owner/session/Hero/Transform mismatch and no native collision were reported. The next verified WallRun Update arrived immediately after the source had already fail-closed.

This demonstrates that the current `native_update_freshness_ms=100` contract can classify a transient native Update stall/hitch as a permanent lifecycle end even while the same verified WallRun session later continues.

## Next direction

Do not advance Position writes or adhesion correction yet.

Change freshness handling so a transient `fresh_update_age` failure suspends source writes but does not permanently destroy an otherwise coherent matching WallRun session. A later fresh verified native Update for the same session/owner/generation/thread must be able to resume ownership without restarting the interpolation or creating a new WallRun session.

Permanent close remains required for verified Deinit, owner/session/generation change, Hero/Transform identity failure, native event collision/epoch failure, integrity/patch failure, kill/F8/shutdown, or other demonstrated hard-invalid conditions.

The 100 ms freshness check should continue to block individual writes while stale; the required change is the permanence/recovery semantics, not simply removing freshness validation.

After this regression is repaired and real-game lifecycle-wide retention passes again, resume the c6 Position-proof / planar adhesion work.