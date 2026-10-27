# Next task — c61 post-c59 runtime-boundary diagnostic classification

## Status

Accepted owner c60 **VERIFIED-STATIC** result:

- `analysis/V01C60_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C60_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

Accepted c60 key totals:

```text
repository_result = LATER_RUNTIME_BEGIN_STITCH_SCANNER_READY
root_transactions_attempted = 162472
root_transactions_completed = 100729
root_transactions_refused = 61743
c59_boundary_instances_considered = 33719
c59_boundary_instances_admitted = 18833
c59_boundary_instances_refused = 14886
c59_exact_edges_considered = 28201
c59_exact_edges_admitted = 20570
c59_exact_edges_refused = 7631
rows_reached_by_continuation = 794
address_facts_crossing_runtime_rows = 2062
post_c59_uncertified_boundary_refusals = 6566
new_cross_row_candidate_count = 0
```

The new bounded unknown wave is exactly the `6566` roots whose first terminal refusal after a c59-certified crossing is `uncertified_post_c59_runtime_boundary`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, `analysis/V01C60_OWNER_OFFLINE_RESULT.md`, c56-c60 tools/tests/evidence, and relevant Wisdoms references.

## Current bounded task

C61 is **OFFLINE/STATIC DIAGNOSTIC ONLY**.

Build a separate c61 diagnostic classifier for the exact first terminal `uncertified_post_c59_runtime_boundary` instance from each accepted c60 refused root. Classify that boundary and its outgoing successors; admit no new edge, carry no additional fact, and emit no new candidate.

Preferred tool: `source/tools/slot_publication_post_c59_boundary.py` with focused test `source/tools/test_slot_publication_post_c59_boundary.py`.

### 1. Authenticate the complete accepted chain

Require the exact pinned image and independently validate/reconstruct the accepted c56→c60 chain.

Preferred read-only owner inputs:

- c56 checkpoint directory;
- accepted c57 JSON;
- accepted c58 JSON;
- accepted c59 JSON;
- accepted c60 JSON.

The c61 tool must not trust compact examples as a certificate pool. Replay the exact c60 transaction semantics needed to reproduce the owner c60 result and verify supplied c60 identity/counts before using any c60 context diagnostically.

Missing/stale/corrupt/wrong-image/wrong-tool/wrong-schema/wrong-contract/wrong-count/wrong-record-set evidence must refuse. Never repair or silently weaken supplied evidence.

### 2. Exact diagnostic domain

The expected root domain is:

```text
post_c59_uncertified_boundary_refused_roots_expected = 6566
```

Record only the first terminal `uncertified_post_c59_runtime_boundary` encountered under the unchanged c60 transaction for that original root. C60 currently aborts the root at that terminal refusal; do not explore beyond it.

Require exactly one authenticated diagnostic record per expected refused root unless deterministic replay proves a stricter one-to-many entry-instance distinction, in which case report root count and instance count separately and preserve exact ordering.

Any missing/duplicate/out-of-order/wrong-root/wrong-entry-state/wrong-path/wrong-source record poisons the diagnostic aggregate.

### 3. Bind records to exact entry-instance/path context

At minimum bind each record to:

```text
original_root_runtime_row
current_entry_instance_row
incoming_state_identity
boundary_path_identity
boundary_transition_identities
source instruction RVA / size / SHA256
source runtime-row identity / code identity
accepted c60 authentication/result identity
accepted c59 record/classifier identities already present in the path
```

Never promote a record to a row-global or instruction-global certificate. Same row/instruction reached under another state/root/path remains unrelated unless independently recorded.

### 4. Classify successors only

Use the existing c57 architectural successor and target-relation vocabulary unchanged:

```text
ordinary_fallthrough
direct_unconditional_branch
direct_conditional_taken
conditional_fallthrough
call_fallthrough
other_supported_direct_successor
```

and:

```text
exact_unique_runtime_begin
inside_unique_runtime_span_nonentry
overlapping_multiple_runtime_spans
executable_file_backed_outside_runtime_metadata
executable_not_fully_file_backed
non_executable_or_writable_section
outside_image
deferred_producer_excluded
```

Preserve architectural vs model-only labeling and exact adjacency as a diagnostic only. Adjacency is never a proof substitute.

Do **not** admit any c61 edge, continue into any classified target, or change c60 candidate/fact output.

### 5. Preserve c60 semantics and output

C61 must leave unchanged:

- c53 must/fact semantics;
- c54/c55 CALL preservation and RSP kill;
- c57 first-boundary proofs;
- c59 exact later-boundary records;
- c60 exact entry-instance/path admission;
- all c60 root completion/refusal/candidate totals;
- all finite caps and fail-closed behavior.

No memory-loaded values/pointers, slot qword reads, pointer chase, stack/local/return facts, ABI assumptions, immediate-pointer generality, indexed/segment generality, partial-register recovery, EH/alternate-entry inference, or dynamic recursive certificate generation.

### 6. Required diagnostics

Emit compact exact totals including at minimum:

```text
repository_result
image_authenticated / image_sha256
accepted c57/c58/c59/c60 identities
post_c59_uncertified_boundary_refused_roots_expected = 6566
post_c59_uncertified_boundary_refused_roots_classified
post_c59_boundary_instances_total
unique_entry_instance_states
unique_source_rows
post_c59_boundary_edges_total
architectural_post_c59_boundary_edges_total
model_only_post_c59_boundary_edges_total
edge_kind_counts
target_metadata_relation_counts
exact_runtime_begin_edge_kind_counts
interior_target_edge_kind_counts
metadata_free_RX_edge_kind_counts
exact_adjacency_counts
classification_complete
new_post_c59_boundaries_admitted = 0
cross_runtime_fact_propagation_unchanged = true
c60_candidate_output_unchanged = true
c60_candidate_output_sha256
new_cross_row_candidate_count = 0
```

If useful, record entry-seed fact counts, but label them explicitly as entry-seed state rather than source-instruction must-state.

Publish a deterministic classifier identity plus authenticated ordered record-set SHA-256.

### 7. Safety / completeness remain false

Explicitly preserve:

```text
candidate_sweep_complete = false
writer_enumeration_complete = false
publication_source_kind = unproven
allocation_proven = false
base_proven = false
range_proven = false
lifetime_proven = false
ownership_exclusive = false
destination_contained = false
CFG_continuation = false
alternate_entries_EH_indirect_writers_closed = false
production_integrated = false
hardware_watch_enabled = false
live_qword_reads = 0
pointer_chase = false
game_used = false
```

C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding. Zero candidates or a dominant exact-target class is not writer absence/publication proof.

### 8. Focused tests

Run:

```bash
python3 source/tools/test_slot_publication_post_c59_boundary.py
```

Expected: all focused groups PASS, exit 0.

Cover at minimum:

- only exact c60 terminal POST refusals are recorded;
- pre-c59 `uncertified_later_runtime_boundary` is not relabeled;
- exact root/current-row/state/path/source identity binding;
- same row/instruction with different state/path remains distinct/refuses borrowing;
- ordinary, conditional taken/fallthrough, direct branch and CALL-fallthrough successor classification;
- architectural/model-only distinction;
- all target relation classes and exact adjacency diagnostics;
- duplicate/missing/out-of-order/stale records poison aggregation;
- stale/wrong c56-c60 evidence refuses;
- no new edge/fact/candidate admission;
- c60 result/candidate hash unchanged;
- all gates remain false.

### 9. Required validation

Run these exact checks:

```bash
python3 source/tools/test_slot_publication.py
python3 source/tools/test_static_access_v2.py
python3 source/tools/test_slot_publication_cfg.py
python3 source/tools/test_slot_publication_callee.py
python3 source/tools/test_slot_publication_direct_entry.py
python3 source/tools/test_slot_publication_sharded.py
python3 source/tools/test_slot_publication_boundary.py
python3 source/tools/test_slot_publication_stitched.py
python3 source/tools/test_slot_publication_later_boundary.py
python3 source/tools/test_slot_publication_later_stitched.py
python3 source/tools/test_slot_publication_post_c59_boundary.py
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

Expected: historical semantics remain green, focused c61 PASS, no new BLOCKED regression. Existing optional SKIP/UNAVAILABLE cases may remain only with their exact historical reason.

Record in GitHub for every check: command, expected result, actual result, and reason for any unrun test. Do not paste the long table into the owner-facing Codex reply; follow `docs/CODEX_REPORTING.md`.

### 10. Repository result

Emit exactly one:

```text
POST_C59_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
POST_C59_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED
```

`READY` means the bounded diagnostic classifier/tool is implemented and validated. It does not mean actual private-image counts are known in cloud.

### 11. Required repository outputs

Update/create:

- c61 tool + focused test;
- `analysis/V01C61_POST_C59_RUNTIME_BOUNDARY.md`;
- `source/analysis/V01C61_POST_C59_RUNTIME_BOUNDARY.md`;
- compact status/gate evidence if useful;
- `docs/PROJECT_STATE.md` with accepted c60 owner result and c61 current stage;
- `docs/CODEX_RESULT.md`;
- Wisdoms only if a reusable rule is newly established.

Do not rewrite historical owner-result files.

Commit, push, and verify publication on `codex/v01c20-edi-value-provenance`.

### 12. Owner action if private image is unavailable to Codex

Provide one OFFLINE-only owner command that consumes the existing c56 checkpoint directory plus accepted c57/c58/c59/c60 JSON read-only and writes only the compact c61 JSON under WS logs.

No Update-WS, game launch, Overstrike action, EXE upload, runtime log, hardware watch, qword read, or pointer chase is required for c61.
