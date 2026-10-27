# V01C61 owner offline result — accepted VERIFIED-STATIC

2026-10-28 JST. Owner supplied `MSM2-WS-v0.1c61-post-c59-runtime-boundary.json` produced from the pinned private `Spider-Man2.exe` with the published c61 offline classifier.

## Classification

**VERIFIED-STATIC** / `POST_C59_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY`.

This is authenticated offline/static diagnostic evidence only. It is not runtime/gameplay evidence and grants no publication, writer, ownership, lifetime, CFG/EH, watch, pointer, qword-read, or production permission.

## Artifact identity

```text
file_sha256 = bcdbc128d89472fa575600de7fea3c11004bb3c0eb21f1090ac0e5ca779e3e23
file_bytes = 151414
tool_version = v0.1c61
schema_version = 1
contract = exact_c60_first_terminal_post_c59_boundary_instance_diagnostic_v1
image_sha256 = c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
classifier_identity = a828ec479e0f3279ddaf117d76ff827652155c357a8415606cbe2924e479d349
authenticated_record_set_sha256 = f5bc25085c62e7f9a32eab74ade7af97e076c9b03c99f796cc06d179ad5f724e
accepted_c60_authentication_identity = 5a71f37023218abc6827dfd22274dad067837021e67ccb8f25d9070202360e0b
accepted_c60_result_identity = 761ffda9b074dc05a8f4df389ded255f654a219ace0ea0364720b81bab50b21a
```

The accepted c57/c58/c59 identities and c56 manifest match the previously accepted chain. `c60_candidate_output_unchanged=true`; candidate output SHA is unchanged.

## Exact owner counts

```text
post_c59_uncertified_boundary_refused_roots_expected = 6566
post_c59_uncertified_boundary_refused_roots_classified = 6566
post_c59_boundary_instances_total = 6566
unique_entry_instance_states = 6566
unique_source_rows = 6467
post_c59_boundary_edges_total = 7206
architectural_post_c59_boundary_edges_total = 7206
model_only_post_c59_boundary_edges_total = 0
classification_complete = true
```

Target relation distribution:

```text
exact_unique_runtime_begin = 6946  # 96.3919% of edges
inside_unique_runtime_span_nonentry = 245  # 3.3999%
executable_file_backed_outside_runtime_metadata = 15  # 0.2082%
all other relation classes = 0
```

Edge kinds:

```text
ordinary_fallthrough = 2487
conditional_fallthrough = 681
direct_conditional_taken = 3492
direct_unconditional_branch = 529
call_fallthrough = 17
other_supported_direct_successor = 0
```

Exact-runtime-begin kinds:

```text
ordinary_fallthrough = 2487
conditional_fallthrough = 681
direct_conditional_taken = 3304
direct_unconditional_branch = 457
call_fallthrough = 17
```

Terminal phases:

```text
same_instance_after_c59_preflight_before_row_fixpoint = 5759
post_c59_reachable_context = 805
post_c59_reachable_dedup_stored_earlier_path = 2
```

No c61 edge was admitted and no new candidate/fact was emitted:

```text
new_post_c59_boundaries_admitted = 0
new_cross_row_candidate_count = 0
cross_runtime_fact_propagation_unchanged = true
c60_candidate_output_unchanged = true
candidate_sweep_complete = false
writer_enumeration_complete = false
```

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false and C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding.

## Interpretation

1. The exact post-c59 unknown wave is fully classified: 6566/6566 roots and instances.
2. The wave is strongly dominated by exact unique runtime beginnings (6946/7206 edges), but this is an **edge distribution**, not an admission count. Multi-successor all-successor semantics mean no safe-root/instance count may be inferred from 6946 alone.
3. The 245 interior and 15 metadata-free RX successors remain explicit fail-closed blockers.
4. `same_instance_after_c59_preflight_before_row_fixpoint` (5759) must not be relabeled as a completed c59 crossing. The c61 implementation deliberately distinguishes local c59 preflight state from authenticated post-c59 arrival/propagation.
5. The 805 true `post_c59_reachable_context` and 2 dedup-stored-earlier-path cases preserve exact arrival/path identity. A later continuation must not borrow certificates across those phase/path distinctions.
6. Zero candidates still does not prove writer absence. Indirect control, EH/alternate entries, executable gaps, unsupported semantics, deferred producer, and completeness/publication/ownership domains remain open.

## Next bounded decision

Proceed to c62 as a separate OFFLINE/STATIC continuation tool. It may use only a fully replayed/authenticated c61 record for the exact current root/entry-instance/state/path/source/terminal-phase context, and only when **every required architectural successor** for that c61 boundary is `exact_unique_runtime_begin` and passes current c60-style target preflight. Interior/free-RX/missing/stale/model-only/mismatched successors abort the whole root. Do not globalize c61 records by row or instruction and do not recursively certify the next unknown wave.
