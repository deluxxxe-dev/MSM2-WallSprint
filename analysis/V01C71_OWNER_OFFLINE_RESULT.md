# V01C71 corrected owner OFFLINE classification — VERIFIED-STATIC

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Status

Corrected c71 owner OFFLINE classification completed and is accepted as **VERIFIED-STATIC**.

The first c71 owner attempt remains a separate blocked implementation attempt documented in `analysis/V01C71_OWNER_BLOCKED_RESULT.md`.

## Owner evidence received

Owner log:

```text
c70 full-replay c69 continuation: 162472/162472
POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY: .../MSM2-WS-v0.1c71-post-c69-runtime-boundary.json
C71_AUTHENTICATED_PREFIX_CHECKPOINT_READY: .../MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
```

Uploaded corrected owner log:

```text
file: c71_owner_corrected.log
size: 198812 bytes
SHA256: b583c1d01883f0f08fd0011d7e2219473a7d17dc37501c5544e096d1f3ed918c
```

Uploaded c71 authenticated diagnostic checkpoint:

```text
file: MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
size: 1324640 bytes
physical SHA256: 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
checkpoint identity: 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
record count: 102
record stream SHA256: 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
```

The checkpoint embeds the exact c71 result object. Its semantic identity is:

```text
c71 result identity: 18d61be59032514a4c74c9b015f422c379e2c05050e2d5819c1025048a2276d7
classifier identity: 18cba768c4261886b70496e7f73abe3fe5276b44aecadb0d929802ae3dae9fd5
repository_result: POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool: v0.1c71 / schema 1
contract: exact_c70_first_terminal_post_c69_boundary_full_prefix_provenance_diagnostic_v1
```

`c56.atomic_json()` serializes `encoded(value)+"\n"`. From the exact result object embedded in the uploaded checkpoint, the deterministic expected standalone result file is:

```text
MSM2-WS-v0.1c71-post-c69-runtime-boundary.json
expected size: 424663 bytes
expected physical SHA256: db524d81d101da4173bbdda802a6bbe65a0d1801192b37b6261ecf4ce3c37ada
```

The standalone result file bytes were not separately uploaded in this handoff, so that physical SHA is deterministic reconstruction from the authenticated checkpoint result object, not an independent byte-read of the local file. C72 should authenticate the accepted checkpoint physical identity and its embedded result/record identities rather than relying only on the standalone result path.

## Exact c71 domain

Accepted c70 measured exactly 102 `uncertified_post_c69_runtime_boundary` roots. Corrected c71 reproduced and classified all of them:

```text
expected roots = 102
classified roots = 102
boundary instances = 102
unique entry-instance states = 102
unique terminal-context rows = 99
classification_complete = true
terminal_origin_classification_complete = true
```

All 102 terminals are edge-backed:

```text
edge_backed_terminal_instances = 102
transaction_terminal_instances = 0
terminal_event_kind_counts = {edge_callback: 102}
```

## Provenance forms actually observed

The real owner evidence contains all three accepted c70 edge provenance forms:

```text
used_c69_source_on_later_scan = 90
post_c69_reachable_context = 3
retired_c69_first_terminal_observer_distinct_edge = 9
```

The correction added after the first blocked owner run was therefore necessary in real data, not only a synthetic fixture. The 9 retired-observer records preserve:

```text
post_c69 = false
used_c69_source = false
reason = c70_retired_c69_first_terminal_observer_later_distinct_edge
```

They are not relabeled as current post-c69 reachability.

## Successor classification

C71 observed 109 architectural successor edges and zero model-only edges:

```text
post_c69_boundary_edges_total = 109
architectural_post_c69_boundary_edges_total = 109
model_only_post_c69_boundary_edges_total = 0
```

Edge kinds:

```text
ordinary_fallthrough = 36
direct_conditional_taken = 51
conditional_fallthrough = 7
direct_unconditional_branch = 15
call_fallthrough = 0
other_supported_direct_successor = 0
```

Target relations:

```text
exact_unique_runtime_begin = 102
inside_unique_runtime_span_nonentry = 6
executable_file_backed_outside_runtime_metadata = 1
all other relation classes = 0
```

Per-terminal complete successor grouping:

```text
all_successors_exact_runtime_begin = 95
contains_interior_successor = 6
other_relation_set = 1
```

The 95 all-successor-exact terminal instances contain 99 exact architectural edges: 91 have one exact edge and 4 have two exact edges.

The non-continuable diagnostic domain is exactly:

```text
6 terminal instances containing inside_unique_runtime_span_nonentry
1 terminal instance containing executable_file_backed_outside_runtime_metadata
```

No c71 target was traversed.

## Candidate / safety result

C71 remained diagnostic-only:

```text
new_post_c69_boundaries_admitted = 0
new_cross_row_candidate_count = 0
live_qword_reads = 0
c56_through_c70_evidence_read_only = true
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

Historical c70 candidate counts remained zero:

```text
reference_candidate_count = 0
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
```

All completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

Zero candidates does not prove writer absence.

## Accepted conclusion

C71 successfully reduced the next continuation decision to a precise static domain:

```text
102 classified terminal roots
95 all-successor-exact continuation candidates
99 exact architectural edges in those 95 roots
7 terminal roots remain fail-closed without continuation
```

This is static evidence only. It does not prove publication source, writer completeness, allocation, ownership, lifetime, or gameplay writability.

## Next bounded step

C72 may be defined as the minimum full-historical-replay continuation stage through only the 95 c71 terminal instances whose complete architectural successor set is `exact_unique_runtime_begin`.

C72 must authenticate the exact accepted c71 checkpoint physical SHA, checkpoint identity, embedded c71 result identity, classifier identity, record-stream identity, exact 102-record domain, and c70 ancestry. It may preflight only the 99 exact successor edges belonging to those 95 records.

The six interior-containing records and one executable-file-backed-outside-runtime-metadata record remain wholly fail-closed. Admission remains root-transactional: preflight eligibility is not proof that a root will commit.

C72 must stop at the first later unknown boundary and must not widen any publication/ownership/production/game gate by assumption.
