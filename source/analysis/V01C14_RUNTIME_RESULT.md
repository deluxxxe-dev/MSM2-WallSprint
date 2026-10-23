# WallSprint v0.1c14 actual-game runtime result

## Verdict

**Diagnostic PASS / exact static blocker identified. Movement/source writes remain unauthorized.**

Human supplied one complete c14 log covering A/cardinal then E/diagonal. The exact target build was accepted:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
v0.1c14 / package 0.1.32
```

No raw user log or private full game-function body is committed here.

## Orientation / lifecycle regression gate

Session 1 / A:
```text
wallrun_duration_ms=4453
source_entry_elapsed=0.719000
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
stale_suspend_count=0
first_hard_kill=none
hard_kill_calls=0
Position_writes=0
```

Session 2 / E:
```text
wallrun_duration_ms=4172
source_entry_elapsed=0.703000
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface=1.00000012
stale_suspend_count=0
first_hard_kill=none
hard_kill_calls=0
Position_writes=0
```

Position watch cleanup restored normally. The c13 entry hook also stopped/restored cleanly in both sessions. No source/Position/input/movement write was performed by the mod.

## c13 entry accounting repair

The final c14 `MOVEMENT_LINEAGE_SUMMARY` is now consistent with the actual captured records in both sessions:

```text
source_capture_ready=1
true_entry_snapshot=1
true_entry_snapshots=8
complete_invocation_maps=8
writable_source_proven=0
Position_writes_by_mod=0
```

So the c13 final-summary underreporting defect is repaired.

A separate telemetry timing issue remains: `MOVEMENT_ENTRY_CAPTURE_SUMMARY` is emitted before the actual entry-capture window activates and therefore reports zero captures for both sessions. This is not evidence that the captures failed; the later real entry records and final `MOVEMENT_LINEAGE_SUMMARY` prove eight snapshots/maps per session. If retained, this summary should be moved/re-emitted at stop/finalization in the next diagnostic revision.

## Repeated actual RDX / stack-local observation

Actual entry snapshots again reproduce the c13 relation on A and E:

```text
RDX == entry RSP + 0x28
pointer candidate 20 == [RDX]
stack candidate 23 == [entry RSP+0x28]
XYZ bits match executed Transform Position XYZ store bits
entry_store_xyz_match_mask=0x7
source W observed as 0
destination Position W observed as 1
```

The stack pointer is TEB-bounded and the pointer bytes equal the sampled stack bytes. This remains strong runtime correlation, not causal write permission.

## Static writer contract blocker

C14's new independent writer contract decoder refuses the real 183-byte writer before it can prove the XYZ path:

```text
MOVEMENT_RDX_STATIC_CONTRACT
admitted_build=1
writer_complete_decode=1
entry_store_prefix=0
xyz_RDX_0_4_8_paths=0
W_kind=unknown
W_classified=0
first_barrier_rva=0x2CFC91F
first_barrier_instruction=jne
caller_prefix=1
caller_RDX_local=0
caller_allocation_owned=0
caller2_relation=unresolved
exact_next_producer_rva=0x351EB00
writable_source_proven=0
reason=exact_position_store_sites_mismatch_or_prefix_incomplete
```

Per invocation the same result is repeated:

```text
MOVEMENT_RDX_XYZ_PROOF
component_paths=unresolved
static_prefix_proven=0
xyz_causality_proven=0
entry_store_xyz_match_mask=0x7
reason=exact_position_store_sites_mismatch_or_prefix_incomplete
```

Therefore equality is observed but the required decoded causal path from entry RDX+0/+4/+8 to Transform +0x30/+0x34/+0x38 is **not yet proven**.

## W proof

Destination W remains observed as `0x3f800000` (1.0), but c14 cannot classify its decoded source because the same writer-prefix proof stops first:

```text
store_rva=0x2CFC945
decoded_site_verified=0
source_kind=unknown
source_value_bits=0x3f800000
destination_value_bits=0x3f800000
causality_proven=0
reason=exact_position_store_sites_mismatch_or_prefix_incomplete
```

Do not call W a homogeneous-coordinate constant until the writer path is independently decoded/proven.

## Storage / producer narrowing

Runtime stack identity is narrowed but ownership is not:

```text
rdx_relation=entry_rsp_plus_0x28_equals_precall_rsp_plus_0x20
stack_object_size=16
current_thread_TEB_bounded=1
pointer_stack_bytes_equal=1
static_RDX_local_assignment=0
caller_owned=0
ownership_exclusive=0
reason=RDX_inherited_or_not_exact_local_next_edge_is_incoming_producer
```

The observed parent function around second return `0x351A020` is bounded as `0x3519E10..0x351A0A4`. Its anchored call at `0x351A01B` has direct target / exact next producer edge `0x351EB00`, but relation to the tiny caller remains unresolved and no causal XYZ constructor has been identified:

```text
producer_relation_to_0x351A020=unresolved
anchored_call_rva=0x351A01B
direct_target_rva=0x351EB00
next_edge_rva=0x351EB00
construction_kind=unknown
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
writable_source_proven=0
```

This producer edge is useful, but it must not be chased as a writable source before the writer-side RDX->Position causal contract is proven.

## Gate

```text
writable_source_proven=0
Position/source/input/movement writes=0
```

C14 is accepted as a diagnostic PASS because it preserved all runtime safety gates, repaired the final c13 entry accounting, reproduced the RDX XYZ relation on both surfaces, and identified a single exact writer-side blocker instead of guessing through it.

## Exact next edge

The next task is **not movement** and not yet the `0x351EB00` producer implementation.

First extend the c14 independent writer contract proof only enough to cross the exact real conditional-control-flow barrier at `0x2CFC91F` and reach the known four Position stores. Prefer a bounded local branch-aware proof for this authenticated 183-byte writer; do not build a general whole-engine CFG tracer.

Required next result:
- prove or reject all relevant branch paths from writer entry to stores `0x2CFC936/93C/942/945`;
- prove/reject `[RDX+0/+4/+8] -> Transform +0x30/+34/+38` causality;
- classify W independently once the store site is reachable;
- keep producer/storage ownership and write permission at 0 unless independently proven;
- re-emit/fix `MOVEMENT_ENTRY_CAPTURE_SUMMARY` at finalization so it reflects the actual session records.
