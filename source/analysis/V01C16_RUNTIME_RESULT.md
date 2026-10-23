# WallSprint v0.1c16 actual-game runtime result

## Verdict
**Diagnostic PASS / exact callee alias blocker identified.** No movement-write authorization.

Actual private combined A -> full WallRun end -> move/fast-travel -> E log used v0.1c16 / package 0.1.34 on the accepted Spider-Man2.exe 2.810.0.0 SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`.

## Regression gate
```text
A/session1 wallrun_duration_ms=3875
E/session2 wallrun_duration_ms=4203
both verified_Deinit=1
both source_active_until_deinit=1
both retention_pass=1
A final actor Up dot surface=1
E final actor Up dot surface=0.99999994
both stale_suspend_count=0
both hard_kill_calls=0
both Position_writes_by_mod=0
```
Session1 also naturally exercised the bounded c10 publication-refresh path once and admitted it cleanly. Position watch/entry diagnostic cleanup restored normally.

Final capture accounting on each session:
```text
capture_activated=1
true_entry_snapshots=8
complete_invocation_maps=8
xyz_match_count=8
xyz_match_mask=0x7
xyz_stable_across_invocations=1
source_w_bits=0x0
destination_w_bits=0x3f800000
summary_final=1
```

## c16 writer-call result
The exact writer call is authenticated:
```text
writer=0x2CFC900..0x2CFC9B7
call_rva=0x2CFC927
return_rva=0x2CFC92C
call_kind=direct_call
target_rva=0x2B23560
target_boundary=0x2B23560..0x2B235E9
target_code_hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
exact_build_verified=1
current_code_verified=1
target_boundary_verified=1
target_complete_decode=1
relevant_to_xyz_paths=1
```

Exact remaining effect blocker:
```text
effect=source_span_effect_unknown
source_span_no_write_proven=0
source_span_may_write=0
source_span_effect_unknown=1
next_callee_barrier_rva=0x2B23560
next_callee_instruction=mov
reason=callee_memory_write_alias_to_source_stack_or_destination_unresolved
```
The callee is small and completely decoded, but the current effect model cannot prove that its memory writes are disjoint from the 16-byte entry-RDX source stack object and Hero Transform destination. Volatile RDX/RCX identity is conservatively invalidated across the call and writer proof cannot resume yet.

All sampled A/E invocations still corroborate `RDX == entry RSP+0x28`, `entry_store_xyz_match_mask=0x7`, source W=0 and destination W=1, but `xyz_causality_proven=0` / W causality remain unproven.

## Safety boundary
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```
Do not investigate or mutate upstream producer `0x351EB00` until this writer-local callee alias contract is resolved.

## c17 handoff consumption

C17 consumes this compact result and pins its exact measured callee boundary/hash. No private full body/raw log is requested or published; new address classifications await actual c17 A/E.
