# WallSprint v0.1c9 diagonal E-wall runtime result

Human observation: visually, the actor appeared to maintain the wall-relative orientation through the run.

Runtime package: v0.1c9 / package 0.1.27.

## Diagonal surface confirmation

The tested wall was genuinely diagonal/non-cardinal:

```text
N398 ≈ (-0.70710677, 0, +0.70710677)
```

The orientation target followed that normal. Path A/B repeatedly applied:

```text
target_up ≈ (-0.70710671, 0, +0.70710683)
actor_up_dot_surface = 1
```

So this is not an A-axis-only success.

## Lifecycle result

Final lifecycle summary:

```text
wallrun_duration_ms=4157
source_entry_elapsed=0.703000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=231
path_b_writes=228
retention_pass=1
max_held_ms=1391
max_held_updates=90
last_actor_up_dot_surface=1
stale_suspend_count=0
stale_resume_count=0
first_hard_kill=none
hard_kill_calls=0
```

Therefore the c9 orientation lifecycle remains active through verified Deinit on a diagonal wall.

## Important residual issue: two very short WorldUp flashes

The run was not perfectly continuous internally. There were exactly two read-only actor observations with:

```text
actor_up=(0,1,0)
actor_up_dot_surface=0
actor_up_dot_world=1
```

They occurred near:

```text
elapsed ≈ 2.34 s
elapsed ≈ 3.53 s
```

Each was recovered on the next normal Path A/common-source override, roughly 10–30 ms later, which explains why the user did not notice them visually.

Both flashes correlate exactly with the only two rejected secondary-thread Path B calls in the run:

```text
action=reject
reason=secondary_source_publication_changed
```

Secondary summary:

```text
seen=112
admitted=110
overridden=110
rejected=2
unique_threads=16
```

The first rejection was followed about 10 ms later by the first WorldUp observation. The second rejection was followed about 10 ms later by the second WorldUp observation.

This strongly indicates a normal lock-free publication race: the secondary Path B call starts from a valid active `Published p`, but a primary worker publication advances `publishedMs` during validation. c9 then rejects because the final publication no longer exactly matches the initial snapshot, leaving that vanilla Path B call free to write WorldUp for one brief interval.

Do not weaken session/owner/Hero/Transform/generation/lifecycle/epoch/code-integrity validation. The narrow next target is to distinguish an identity-preserving publication refresh from a real ownership change.

## Standoff / adhesion observation

Position remained read-only:

```text
position_source_write=0
corrections_applied=0
```

Unlike the A wall, diagonal-wall samples showed small standoff variation. Examples ranged from about 0.479 m to 0.511 m around the 0.5 m target. This is still modest, but it means adhesion should remain observational for now rather than being declared unnecessary globally.

## Gate decision

E-wall orientation generality: PASS.

Perfect continuous orientation: NOT YET PASS because of the two one-observation WorldUp flashes.

Next implementation: c10 secondary-publication race repair. Do not start surface-relative movement until this race no longer produces WorldUp observations in the same diagonal regression.
