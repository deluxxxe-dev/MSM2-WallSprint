# WallSprint v0.1c12 actual-game runtime result

## Verdict

**Diagnostic PASS / exact missing edge.**

The real A/cardinal and E/diagonal c12 runs preserve c10/c11 orientation and reproduce the same movement-lineage blocker on both surfaces. No writable movement source is admitted.

Target/build was the accepted Spider-Man2.exe 2.810.0.0 / SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9` with `WallSprint v0.1c12 package_version=0.1.30`.

## A/cardinal result

```text
wallrun_duration_ms=4266
source_entry_elapsed=0.719000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
Position_writes_by_mod=0
```

Position watch:

```text
watch_armed=1
threads_seen=128
threads_installed=126
unique_writers=14
total_write_events=256
dropped=0
cleanup_restored=1
cleanup_pending=0
```

c12 lineage:

```text
MOVEMENT_WRITER_PROOF writer_rva=0x2cfc900
function_begin=0x2cfc900
function_end=0x2cfc9b7
function_size=183
complete_decode=1
lineage_complete=0
source_map_supported_prefix=0
first_unsupported_offset=0xa
unsupported=35
writable_source_proven=0
```

Exact direct callsite:

```text
return_rva=0x351eeb0
call_rva=0x351eeab
target_rva=0x2cfc900
direct_call_verified=1
caller_function_begin=0x351eea7
caller_function_end=0x351eec5
caller2_return_rva=0x351a020
caller2_direct_relation_verified=0
caller2_reason=direct_target_mismatch
```

Final lineage gate:

```text
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

## E/diagonal result

Verified surface normal:

```text
N398 ≈ (-0.7071068, 0, +0.7071068)
```

Lifecycle:

```text
wallrun_duration_ms=4297
source_entry_elapsed=0.703000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface=1.00000012
first_hard_kill=none
hard_kill_calls=0
Position_writes_by_mod=0
```

A single native Update gap occurred near the end:

```text
stale_suspend_count=1
stale_resume_count=1
max_stale_duration_ms=72
native_gap_ms=172
transition_restarted=0
```

This recovered normally with Surface Up already held and is not treated as a regression.

The same c12 blocker reproduced:

```text
first_unsupported_offset=0xa
source_capture_ready=0
eligible_hits=0
attempts=0
complete_writer_CFG=0
source_prefix_proven=0
direct_call_verified=1
caller2_direct_relation_verified=0
true_entry_snapshot=0
writable_source_proven=0
```

The measured real translation chain still appears during the watch. On the diagonal run, writer `0x2CFC900` with return `0x351EEB0` / second return `0x351A020` wrote Hero Position components at `0x2CFC936/93C/942/945`; the per-component X/Z writes decompose into the expected diagonal SurfaceFrame relationship. This is correlation only, not a writable ownership contract.

## Interpretation

A and E produce the same startup writer proof, exact direct callsite proof, and the same missing lineage edge. Therefore the blocker is not cardinal-vs-diagonal surface behavior. It is the current diagnostic's inability to reconstruct the writer's true entry-time source state and complete semantic producer chain.

Do not infer velocity/displacement/direction/root-motion semantics from arithmetic resemblance. `writable_source_proven=0` remains authoritative.

## Next smallest diagnostic

Create c13 as a read-only **writer entry snapshot / invocation correlation** task for verified writer `0x2CFC900`.

The smallest missing evidence is explicitly `true_entry_snapshot=0`. Capture exact entry-time GPR/XMM/stack argument state only for the active verified Hero Transform, pair each bounded writer invocation with the existing Position hardware-write events on the same thread/target, and identify which entry register/pointer/stack source actually feeds the four Position stores.

Do not add movement/input/Position writes. Do not weaken c12 static proof. If no stable entry source can be authenticated, report the exact next producer edge instead.
