# WallSprint v0.1c12 actual-game runtime result

## Verdict
**Diagnostic PASS / exact missing edge.**

Real A/cardinal and E/diagonal c12 runs preserve orientation and reproduce the same movement-lineage blocker on both surfaces. No writable movement source is admitted.

Target/build: Spider-Man2.exe 2.810.0.0, SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`, WallSprint v0.1c12 / package 0.1.30.

## A/cardinal
```text
wallrun_duration_ms=4266
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
Position_writes_by_mod=0
```

## E/diagonal
```text
N398≈(-0.7071068,0,+0.7071068)
wallrun_duration_ms=4297
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface=1.00000012
first_hard_kill=none
hard_kill_calls=0
Position_writes_by_mod=0
```

E had one late native Update gap (`native_gap_ms=172`) with a 72 ms stale suspend/resume; it resumed normally with coherent identity and is not a regression.

## Shared real c12 lineage result
```text
MOVEMENT_WRITER_PROOF
writer_rva=0x2cfc900
function_begin=0x2cfc900
function_end=0x2cfc9b7
function_size=183
complete_decode=1
lineage_complete=0
source_map_supported_prefix=0
first_unsupported_offset=0xa
unsupported=35
writable_source_proven=0

MOVEMENT_CALLSITE_PROOF
return_rva=0x351eeb0
call_rva=0x351eeab
target_rva=0x2cfc900
direct_call_verified=1
caller_function_begin=0x351eea7
caller_function_end=0x351eec5
caller2_return_rva=0x351a020
caller2_direct_relation_verified=0
caller2_reason=direct_target_mismatch

MOVEMENT_LINEAGE_SUMMARY
source_capture_ready=0
eligible_hits=0
attempts=0
complete_writer_CFG=0
source_prefix_proven=0
direct_call_verified=1
caller2_direct_relation_verified=0
true_entry_snapshot=0
writable_source_proven=0
reason=entry_snapshot_typed_caller_W_and_exclusive_effects_unproven
```

The real Position writer chain remains visible on E: `0x2CFC900` with return `0x351EEB0` / observed second return `0x351A020` writes Hero Transform +0x30/+0x34/+0x38/+0x3C, and the diagonal component writes decompose into the expected SurfaceFrame relation. This is correlation, not a writable contract.

## Next smallest edge
The smallest missing evidence is `true_entry_snapshot=0` for verified writer `0x2CFC900`. c13 should capture the actual entry-time state only for active verified Hero-targeted invocations, pair it to existing Position hardware-write events by invocation, and identify the exact entry register/pointer/stack source feeding the Position stores.

Keep movement and Position read-only. Do not weaken c12 proof or infer typed velocity/displacement semantics from resemblance.

## c13 implementation handoff

C13 implements bounded true entry-time capture and explicit invocation/store correlation in response to this exact blocker. No measured c12 fact above is reclassified as a c13 game result. See MOVEMENT_WRITER_ENTRY_SNAPSHOT.md and V01C13_GATE.md; actual c13 A/E is pending.
