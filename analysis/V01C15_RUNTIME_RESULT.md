# WallSprint v0.1c15 actual-game runtime result

## Verdict

**Diagnostic PASS / exact next local writer blocker narrowed.**

This is not movement-write authorization.

## Test

One complete actual-game log, ordinary input:

```text
A/cardinal WallRun -> fully end -> move/fast-travel -> E/diagonal WallRun
WallSprint v0.1c15 / package 0.1.33
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
build gate ACCEPTED
```

## Regression / lifecycle gate

A/session1:
```text
wallrun_duration_ms=3860
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
stale_suspend_count=0
stale_resume_count=0
first_hard_kill=none
hard_kill_calls=0
Position_writes=0
```

E/session2:
```text
N398≈(-0.707107,0,+0.707107)
wallrun_duration_ms=4297
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface≈1.00000012
stale_suspend_count=0
stale_resume_count=0
first_hard_kill=none
hard_kill_calls=0
Position_writes=0
```

Entry hook and Position watch cleanup restored in both sessions.

## Capture-summary repair PASS

Finalized summaries:

```text
true_entry_snapshots=8/session
complete_invocation_maps=8/session
xyz_source_candidate=20
xyz_match_count=8
xyz_match_mask=0x7
xyz_stable_across_invocations=1
source_w_bits=0x0
destination_w_bits=0x3f800000
phase=finalized summary_final=1
writable_source_proven=0
```

## Writer branch-contract result

C15 crosses the c14 `0x2CFC91F jne` barrier. The next exact local blocker is:

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

Runtime continues to show exact XYZ mask0x7 on every sampled invocation, but `xyz_causality_proven=0`; equality is not promoted over the unresolved call effect.

Storage/producer ownership remains unproven. `0x351EB00` stays deferred.

## Gate / next

```text
writable_source_proven=0
Position/source/input/movement writes=0
```

C16 resolves only the effect contract of writer call `0x2CFC927` before any producer or movement work.
