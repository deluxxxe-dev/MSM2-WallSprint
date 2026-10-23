# WallSprint v0.1c15 actual-game runtime result

## Verdict

**Diagnostic PASS / exact next local writer blocker narrowed.**

This is not movement-write authorization.

## Test / regression

```text
WallSprint v0.1c15 / package 0.1.33
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
build gate ACCEPTED
```

A/session1:
```text
wallrun_duration_ms=3860
verified_deinit=1
source_active_until_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
stale_suspend_count=0
hard_kill_calls=0
Position_writes=0
```

E/session2:
```text
N398≈(-0.707107,0,+0.707107)
wallrun_duration_ms=4297
verified_deinit=1
source_active_until_deinit=1
retention_pass=1
last_actor_up_dot_surface≈1.00000012
stale_suspend_count=0
hard_kill_calls=0
Position_writes=0
```

Entry-hook and Position-watch cleanup restored normally.

## Final capture summary repair PASS

Both sessions finalize with:
```text
capture_activated=1
true_entry_snapshots=8
complete_invocation_maps=8
xyz_source_candidate=20
xyz_match_count=8
xyz_match_mask=0x7
xyz_stable_across_invocations=1
source_w_bits=0x0
destination_w_bits=0x3f800000
phase=finalized
summary_final=1
writable_source_proven=0
```

## Writer result

C15 crosses the previous `0x2CFC91F jne` blocker. The next exact local blocker is:
```text
0x2CFC927: call
```

Branch contract:
```text
writer_rva=0x2CFC900
writer_bytes=183
blocks_visited=2
edges_visited=2
blocks_total=7
edges_total=10
branches_seen=4
first_branch_rva=0x2CFC91F
exact_store_sites_verified=1
all_relevant_store_paths_proven=0
proven_component_mask=0x0
next_barrier_rva=0x2CFC927
next_barrier_instruction=call
W_independent=1
reason=unsupported_relevant_writer_instruction_or_memory_effect
```

Actual A/E invocations still reproduce RDX/stack XYZ equality (`entry_store_xyz_match_mask=0x7`), but `xyz_causality_proven=0` remains correct until the call effect is resolved.

Historical source/storage/producer ownership remains unproven; `0x351EB00` stays deferred.

## Gate / next

```text
writable_source_proven=0
Position/source/input/movement writes=0
```

C16 resolves only the writer-local call effect at `0x2CFC927`, then either resumes the bounded writer proof or reports a narrower exact blocker.
