# WallSprint v0.1c10 real-runtime result

## Status

**PASS for planar orientation stability on both tested cardinal/diagonal walls.**

Human observation: the A→E test sequence visually maintained the surface-relative posture through both runs. The tester separately noted two visual-alignment issues (body lean relative to the apparent ground plane and excessive-looking foot-to-wall standoff). Those are recorded as deferred visual-alignment work; they are not evidence of orientation lifecycle failure.

Runtime log: private user log `WallSprint(20261023-104426).log` was analyzed; the raw log/video/screenshots are not committed.

Build admission:

```text
WallSprint v0.1c10
package_version=0.1.28
Spider-Man2.exe 2.810.0.0
exe SHA256=c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
target_build_gate=ACCEPTED
```

## Session 1 — diagonal planar wall

Observed surface normal:

```text
N398 ≈ (-0.70710677, 0, +0.70710677)
```

Retention/lifecycle:

```text
wallrun_duration_ms=4172
source_entry_elapsed=0.719000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=221
path_b_writes=220
retention_pass=1
max_held_ms=3187
max_held_updates=204
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
```

Secondary Path B:

```text
seen=102
admitted=102
overridden=102
rejected=0
unique_threads=16
```

No `actor_up=(0,1,0)` observation and no `secondary_source_publication_changed` rejection appeared in this run after the orientation transition.

Position remained read-only:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
max_abs_error_before=0.0268774629 m
```

## Session 2 — cardinal planar wall

Final Surface Up was cardinal:

```text
actor_up=(0,0,1)
actor_up_dot_surface=1
```

Retention/lifecycle:

```text
wallrun_duration_ms=3421
source_entry_elapsed=0.718000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=151
path_b_writes=152
retention_pass=1
max_held_ms=2453
max_held_updates=140
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
```

Secondary Path B:

```text
seen=161
admitted=161
overridden=161
rejected=0
unique_threads=16
```

No WorldUp actor observation or publication-change reject was logged.

Position remained read-only and the measured standoff was almost exactly 0.5 m:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
max_abs_error_before=1.52587891e-05 m
```

## c10 race-fix interpretation

The c9 E-wall failure mode is absent in this human c10 run:

```text
secondary_source_publication_changed matches=0
actor_up=(0,1,0) matches=0
secondary rejects=0 on both sessions
```

However, the new c10 bounded publication-refresh path itself did **not** naturally fire in this log:

```text
secondary_publication_refresh_seen=0
secondary_publication_refresh_admitted=0
PATH_B_SECONDARY_PUBLICATION_REFRESH_EVENT matches=0
```

Therefore:

- real A/E orientation stability is accepted;
- the demonstrated c9 leak did not recur;
- deterministic c10 native/synthetic fixtures remain the evidence that the refresh branch itself handles the race;
- do not claim a naturally occurring real-game refresh was observed in this run.

This is sufficient to close the planar orientation-stability milestone and proceed to movement-path discovery. If the publication race recurs later, the c10 telemetry must be re-evaluated.

## Deferred visual-alignment backlog

The user supplied screenshots showing two visible issues:

1. the running body appears leaned toward the apparent ground side compared with normal ground-running animation;
2. feet/body appear farther from the wall than visually expected, giving a floating impression even though the current diagnostic root standoff is around 0.5 m.

Do **not** conflate these with orientation lifecycle stability. Defer them until after movement-path ownership is established, then perform a dedicated visual-alignment pass covering root/surface offset and animation/root lean/contact appearance.

No active adhesion/Position correction is authorized by this result.

## Next milestone

Proceed to **surface-relative movement path discovery / safe movement ownership**.

The next task should remain read-only/diagnostic unless it can independently authenticate a safe caller-local/native movement source. Do not introduce direct Actor Position spam, gravity replacement, camera, animation, wall jump, corners/ceilings, swing reconnect or combat yet.
