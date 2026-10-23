# WallSprint v0.1c11 real-runtime result

## Scope

Private human test on Spider-Man2.exe 2.810.0.0 / SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`.

Sequence reported by user: A/cardinal wall first, then E/diagonal wall. Raw user log/video are not committed.

## Orientation regression

Both sessions retained c10 orientation behavior through verified native Deinit.

### Session 1 — A/cardinal

```text
N398 ≈ (+1,0,0)
wallrun_duration_ms=4125
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=201
path_b_writes=200
retention_pass=1
max_held_ms=3157
max_held_updates=185
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
Position_writes_by_mod=0
```

### Session 2 — E/diagonal

```text
N398 ≈ (-0.7071068,0,+0.7071068)
wallrun_duration_ms=4063
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=204
path_b_writes=204
retention_pass=1
max_held_ms=3093
max_held_updates=189
last_actor_up_dot_surface≈0.99999994
first_hard_kill=none
hard_kill_calls=0
Position_writes_by_mod=0
```

## Position writer watch

The bounded Position watch armed and restored on both sessions:

```text
watch_armed=1
threads_seen=128
threads_installed=126
unique_writers=14
total_write_events=256
unknown_writer_events=220
cleanup_restored=1
cleanup_pending=0
Position_writes_by_mod=0
```

The dominant Transform Position writer family observed during the window is:

```text
writer_function_rva=0x2CFC900
store RVAs=0x2CFC936 / 0x2CFC93C / 0x2CFC942 / 0x2CFC945
Transform destinations=+0x30 / +0x34 / +0x38 / +0x3C
```

The concrete call chain that correlated with real translation on both A and E was:

```text
caller return RVA=0x351EEB0
caller2 return RVA=0x351A020
consumer writer=0x2CFC900
```

On the A wall this call changed tangent Y/Z components while the wall-normal X component remained essentially unchanged. On E it changed X/Y/Z, and the combined finite-difference motion remained predominantly surface-tangent even though individual scalar stores naturally project onto both normal and tangent axes.

Examples from the worker finite-difference samples:

```text
A: delta_normal≈0, delta_forward≈1.6..2.5, delta_right≈-0.3..-0.5 per ~109-110 ms sample
E: delta_normal typically near 0 (small ± drift), delta_forward≈1.5..2.25, delta_right≈-0.3..-0.48 per ~109-110 ms sample
```

This strongly identifies the 0x351EEB0 -> 0x2CFC900 path as part of the actual locomotion Position update chain.

## Path A Position result

Path A writer 0x2CFC580 is also observed touching the Position row and can produce post-writer equality with the caller-local `source+0x30` candidate. However it is **not admitted as the locomotion ownership point**:

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

Runtime also observed downstream Position writes after the Path A candidate. Therefore source equality alone is insufficient and modifying Path A Position remains unauthorized.

## Safe movement source status

No `writable_source_proven=1` candidate occurred in the real log.

The current strongest candidate is the runtime chain around `0x351EEB0 -> 0x2CFC900`, but c11 only proves writer identity and runtime correlation. Upstream source lineage, caller-local provenance, exact input/displacement/target-position semantics and exclusive effects remain unknown.

## Gate decision

**c11 diagnostic PASS.**

Passed:

- c10 orientation preserved on A and E through verified Deinit;
- bounded Position watch armed/restored cleanly;
- real Hero Position writers captured;
- movement delta decomposition works on cardinal and diagonal surfaces;
- Path A Position gets an evidence-based rejected result;
- exact next investigation target identified.

Not passed / not yet authorized:

- safe writable movement source;
- input rewriting;
- movement injection;
- direct Position modification;
- visual standoff/contact adjustment.

## Next task

Trace and authenticate the movement-producing caller/source chain centered on:

```text
0x351EEB0 -> 0x2CFC900
caller2 ≈ 0x351A020
```

Determine the exact entry/caller-local source that supplies the target Position/displacement and whether a more upstream tangent displacement, velocity, desired-direction or root-motion vector can be owned safely. Remain read-only until a source-level write contract is proven.
