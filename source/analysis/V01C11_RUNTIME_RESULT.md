# WallSprint v0.1c11 real-runtime result

Human sequence: A/cardinal then E/diagonal. Raw user log/video are private and not committed.

## Orientation regression

A:
```text
N398≈(+1,0,0)
wallrun_duration_ms=4125
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
max_held_ms=3157
max_held_updates=185
last_actor_up_dot_surface=1
hard_kill_calls=0
Position_writes_by_mod=0
```

E:
```text
N398≈(-0.7071068,0,+0.7071068)
wallrun_duration_ms=4063
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
max_held_ms=3093
max_held_updates=189
last_actor_up_dot_surface≈0.99999994
hard_kill_calls=0
Position_writes_by_mod=0
```

## Position writer attribution

Both Position windows:
```text
watch_armed=1
threads_seen=128
threads_installed=126
unique_writers=14
total_write_events=256
cleanup_restored=1
cleanup_pending=0
```

Main Transform Position writer family:
```text
writer_function_rva=0x2CFC900
store RVAs=0x2CFC936/0x2CFC93C/0x2CFC942/0x2CFC945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
```

Movement-producing caller chain observed on A and E:
```text
caller return=0x351EEB0
caller2 return≈0x351A020
consumer=0x2CFC900
```

The 0x351EEB0 path changes real Hero XYZ and correlates with surface-tangent movement. A samples have near-zero normal delta with Y/Z tangent motion. E finite-difference samples remain predominantly tangent with only small normal drift.

## Path A Position

Path A 0x2CFC580 also touches the Position row and can produce post-writer equality with caller-local source+0x30, but it remains denied:
```text
writer_verified=1
caller_verified=1
stack_relation_verified=1
post_writer_match=1
position_w_observed=1
exclusive_effects_proven=0
write_permission=0
reason=whole_function_CFG_caller_construction_W_and_exclusive_effects_not_proven
```

Downstream Position writers are observed, so Path A source equality is not locomotion ownership proof.

## Gate

c11 = DIAGNOSTIC PASS.

No real candidate reached `writable_source_proven=1`.

Next target: authenticate the source lineage feeding `0x351EEB0 -> 0x2CFC900` and classify it as target Position / displacement / velocity / direction / root motion or unknown. Remain movement-read-only until a source write contract is proven.
