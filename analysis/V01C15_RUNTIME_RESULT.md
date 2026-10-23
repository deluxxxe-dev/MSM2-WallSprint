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

Entry hook and Position watch cleanup restored in both sessions; no crash/hard kill/regression was observed in the supplied evidence.

## Capture-summary repair PASS

C15 fixes the c14 early-zero summary defect. Finalized summaries are real and consistent with the per-invocation records:

```text
session1: capture_activated=1 true_entry_snapshots=8 complete_invocation_maps=8
session2: capture_activated=1 true_entry_snapshots=8 complete_invocation_maps=8
xyz_source_candidate=20
xyz_match_count=8
xyz_match_mask=0x7
xyz_stable_across_invocations=1
source_w_pattern=constant_observed source_w_bits=0x0
destination_w_pattern=constant_observed destination_w_bits=0x3f800000
phase=finalized summary_final=1
writable_source_proven=0
```

## Writer branch-contract result

C15 successfully crosses the c14 local `0x2CFC91F jne` barrier. The next exact local blocker is now:

```text
0x2CFC927: call
```

Startup proof:

```text
MOVEMENT_RDX_BRANCH_CONTRACT
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
x_path=unproven
y_path=unproven
z_path=unproven
proven_component_mask=0x0
next_barrier_rva=0x2CFC927
next_barrier_instruction=call
W_independent=1
reason=unsupported_relevant_writer_instruction_or_memory_effect
```

The associated static contract reports the same blocker and retains:

```text
writer_complete_decode=1
xyz_RDX_0_4_8_paths=0
W_kind=unknown
W_classified=0
exact_next_producer_rva=0x351EB00
writable_source_proven=0
```

## Runtime corroboration remains strong but noncausal

All sampled A/E invocations continue to show:

```text
RDX == entry RSP+0x28
[RDX] XYZ == executed Transform Position XYZ bits
entry_store_xyz_match_mask=0x7
source W observed 0
destination W observed 1
```

Per-invocation proof correctly remains:

```text
xyz_causality_proven=0
component_paths=unresolved
reason=unsupported_relevant_writer_instruction_or_memory_effect
```

The call barrier therefore cannot be bypassed merely because the values match at runtime.

## Storage / producer boundary unchanged

The existing bounded evidence still says:

```text
RDX relation = entry RSP+0x28 = precall RSP+0x20
16-byte current-thread TEB-bounded stack object
pointer_stack_bytes_equal=1
static_RDX_local_assignment=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
exact next producer edge metadata=0x351EB00
```

Do not advance to the producer edge yet. The writer-local call at `0x2CFC927` is now the smallest unresolved causal edge.

## Decision

C15 satisfies diagnostic outcome B: safety/regressions are preserved and the exact local blocker is narrower than c14.

Authoritative gate remains:

```text
writable_source_proven=0
Position/source/input/movement writes=0
```

## Next

C16 should resolve only the effect contract of the exact writer call at `0x2CFC927`: authenticate its target, determine whether it can alter the entry-RDX XYZ source or the relevant reaching provenance, and either continue the bounded writer proof to the exact Position stores or report the next narrower local/callee blocker. No movement mutation and no `0x351EB00` producer work yet.
