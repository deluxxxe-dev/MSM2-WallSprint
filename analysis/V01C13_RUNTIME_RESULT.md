# WallSprint v0.1c13 actual-game runtime result

## Verdict
Diagnostic PASS. Human A -> E run used one continuous log with a fast-travel transition between surfaces. WallRun sessions remained cleanly separable; the fast travel did not invalidate the gate.

## Build / package
- WallSprint v0.1c13 / package 0.1.31
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- exact target gate accepted

## Orientation regression gate
Session 1 / A-cardinal:
- wallrun_duration_ms=3875
- source_active_until_deinit=1
- verified_deinit=1
- retention_pass=1
- last_actor_up_dot_surface=1
- stale_suspend_count=0
- first_hard_kill=none
- hard_kill_calls=0
- Position_writes_by_mod=0

Session 2 / E-diagonal:
- N398≈(-0.7071068,0,+0.7071068)
- wallrun_duration_ms=4281
- source_active_until_deinit=1
- verified_deinit=1
- retention_pass=1
- last_actor_up_dot_surface=1
- stale_suspend_count=0
- first_hard_kill=none
- hard_kill_calls=0
- Position_writes_by_mod=0

No visible anomaly was reported by the user. Video was intentionally omitted under the current log-first test policy.

## True entry snapshots
The c13 entry diagnostic gate admitted and installed successfully for the exact writer `0x2CFC900..0x2CFC9B7` and restored its owned patch cleanly after each session.

Actual game captured 16 true writer-entry snapshots total:
- session 1: invocation_id 1..8
- session 2: invocation_id 9..16

Every captured snapshot had:
- return_rva=`0x351EEB0`
- observed caller2 return=`0x351A020`
- RCX = active Hero Transform
- entry_target_verified=1
- code_integrity_verified=1
- true_entry_snapshot=1
- position_before_read=1
- no source mutation

The observed second-return chain remained runtime-stack-verified but static relation unresolved.

## Invocation-linked store map
For each sampled invocation, c13 paired the same invocation to all four Transform Position component stores:
- +0x30 / store `0x2CFC936`
- +0x34 / store `0x2CFC93C`
- +0x38 / store `0x2CFC942`
- +0x3C / store `0x2CFC945`

Each complete invocation map recorded `store_30_seen=1`, `store_34_seen=1`, `store_38_seen=1`, `store_3c_seen=1`, with the observed Position delta remaining surface-tangent as expected.

## Strong entry source candidate
The same candidate pattern repeated across all 16 invocations on both A and E:

- entry `RDX = RSP + 0x28`
- candidate_id=20: pointer read from RDX
- candidate_id=23: stack snapshot at entry RSP+0x28
- both expose the same 16 bytes
- XYZ exactly match the three executed Transform Position XYZ stores on every sampled invocation
- source W is consistently 0 while destination Position W is 1
- `exact_store_match_mask=0x7`, observed store mask `0xF`

This strongly narrows the source to a caller-local stack Vec3/Vec4-like target-position payload whose XYZ feed the writer. The W lane is not copied from the candidate and appears to be generated or supplied independently by the writer path. This is correlation plus exact store-bit equality, not yet a complete causal/write contract.

The current c13 candidate aggregation flags still report `stable_across_invocations=0`, `stable_across_surfaces=0`, `entry_pointer_candidate_mask=0`, and `source_stable=0` because they require a full four-component match; the persistent XYZ-only `0x7` match is therefore underrepresented in summary telemetry.

## Telemetry inconsistency
`MOVEMENT_LINEAGE_SUMMARY` still ends with `true_entry_snapshot=0` and `source_capture_ready=0` even though 8 true snapshots were captured per session and `MOVEMENT_ENTRY_DIAGNOSTIC_ACTIVE capture_ready=1` was reached. Treat this as a c13 aggregation/reporting defect, not evidence that entry capture failed.

## Remaining blocker
`writable_source_proven=0` remains authoritative.

Still unproven:
1. exact writer dataflow proving `[RDX+0/+4/+8] -> Transform +0x30/+0x34/+0x38` and the origin/meaning of destination W=1;
2. caller-side construction and lifetime of the stack-local payload at `RSP+0x28`;
3. producer provenance through the observed caller2 path around `0x351A020`;
4. downstream conflicts / overwrite behavior and exclusive intended translation effect;
5. typed semantics (final target position vs another representation).

Do not infer velocity, displacement, desired direction, or root motion from these samples. Do not authorize movement/source writes yet.

## Next edge
The smallest next diagnostic is to authenticate the RDX stack-local XYZ source contract and trace who constructs it before `0x351EEAB -> 0x2CFC900`, while separately proving W behavior. Also repair c13 aggregate telemetry so true-entry and XYZ-only stable-candidate evidence are reported correctly.
