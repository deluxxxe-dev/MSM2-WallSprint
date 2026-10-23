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
Session1 naturally exercised the bounded c10 publication-refresh path once and admitted it cleanly. Entry diagnostic / Position-watch cleanup restored normally.

Each session finalized 8 true entry snapshots and 8 complete invocation maps with XYZ mask0x7, source W0 and destination W1.

## c16 writer-call result
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
```

Exact remaining blocker:
```text
effect=source_span_effect_unknown
source_span_no_write_proven=0
source_span_may_write=0
source_span_effect_unknown=1
next_callee_barrier_rva=0x2B23560
next_callee_instruction=mov
reason=callee_memory_write_alias_to_source_stack_or_destination_unresolved
```
The current effect model cannot prove that every callee memory write is disjoint from the 16-byte entry-RDX source stack object and Hero Transform destination. Runtime XYZ equality remains corroboration only; `xyz_causality_proven=0` and W causality remain unproven.

## Safety boundary
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```
Do not inspect or mutate upstream producer `0x351EB00` until the writer-local callee alias contract is resolved.

## Next smallest task
Analyze only callee `0x2B23560..0x2B235E9` enough to classify each memory write relative to the entry-RDX source span and Hero Transform destination. If all relevant writes are proven disjoint, resume the existing writer proof from `0x2CFC92C`; otherwise emit one exact callee write/address-provenance blocker and stop.
