# Next task — c63 post-c61 runtime-boundary diagnostic classification

## Status

Accepted owner c62 **VERIFIED-STATIC** result:

- `analysis/V01C62_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C62_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

Accepted c62 key totals:

```text
repository_result = POST_C59_RUNTIME_BEGIN_STITCH_SCANNER_READY
root_transactions_attempted = 162472
root_transactions_completed = 103933
root_transactions_refused = 58539
c61_boundary_instances_considered = 6566
c61_boundary_instances_admitted = 3204
c61_boundary_instances_refused = 3362
c61_exact_edges_considered = 6946
c61_exact_edges_admitted = 3765
c61_exact_edges_refused = 3181
rows_reached_by_continuation = 1317
address_facts_crossing_runtime_rows = 2635
post_c61_uncertified_boundary_refusals = 2923
new_cross_row_candidate_count = 0
```

The new bounded unknown wave is exactly the `2923` roots whose first terminal refusal after c61-authenticated continuation is `uncertified_post_c61_runtime_boundary`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, `analysis/V01C62_OWNER_OFFLINE_RESULT.md`, c56-c62 tools/tests/evidence, and relevant Wisdoms references.

## Current bounded task

C63 is **OFFLINE/STATIC DIAGNOSTIC ONLY**.

Build a separate c63 diagnostic classifier for the exact first terminal `uncertified_post_c61_runtime_boundary` instance from each accepted c62 refused root. Classify that boundary and its outgoing successors; admit no new edge, carry no additional fact, and emit no new candidate.

Preferred tool/test:

```text
source/tools/slot_publication_post_c61_boundary.py
source/tools/test_slot_publication_post_c61_boundary.py
```

Preferred repository result:

```text
POST_C61_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
```

Use a separate v0.1c63/schema1 mode. Historical tools/results remain read-only.

### 1. Authenticate the complete accepted chain

Require the exact pinned image and independently validate/reconstruct the accepted c56→c62 chain.

Preferred read-only owner inputs:

- c56 checkpoint directory;
- accepted c57 JSON;
- accepted c58 JSON;
- accepted c59 JSON;
- accepted c60 JSON;
- accepted c61 JSON;
- accepted c62 JSON.

Do not trust compact examples as a certificate pool. Replay the exact c62 transaction semantics needed to reproduce the accepted owner c62 result and verify supplied c62 schema/contract/tool/image/result/counts before using any c62 terminal context diagnostically.

Missing/stale/corrupt/wrong-image/wrong-tool/wrong-schema/wrong-contract/wrong-count/wrong-record-set evidence must refuse. Never repair or silently weaken supplied evidence.

### 2. Exact diagnostic domain

Expected root domain:

```text
post_c61_uncertified_boundary_refused_roots_expected = 2923
```

Record only the first terminal `uncertified_post_c61_runtime_boundary` encountered under unchanged c62 semantics for that original root. C62 aborts at that terminal refusal; do not explore beyond it.

Require exactly one authenticated diagnostic record per expected refused root unless deterministic replay proves a stricter one-to-many entry-instance distinction; then report roots and instances separately with deterministic ordering.

Any missing/duplicate/out-of-order/wrong-root/wrong-state/wrong-path/wrong-source/wrong-prefix/wrong-phase/origin record poisons the aggregate.

### 3. Bind exact c62 context

At minimum bind each record to:

```text
original_root_runtime_row
current_entry_instance_row
incoming_state_identity
entry_instance_identity
boundary_path_identity / transition identities
source instruction RVA / size / SHA256
source runtime-row identity / code identity
accepted c61 classifier/record-set/result identities
accepted c62 authentication/result identity
c61 terminal phase/origin/full-prefix provenance carried by c62
```

Never promote a record to row-global or instruction-global proof. Same source row/instruction under another root/state/path/prefix/phase/origin remains unrelated unless independently recorded.

Preserve the three historical c61 phase distinctions; do not relabel source-local preflight as a completed crossing and do not let dedup-stored earlier paths borrow post-arrival certificates.

### 4. Classify successors only

Use the existing architectural successor and target-relation vocabulary unchanged:

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

Preserve architectural vs model-only labeling and exact adjacency as diagnostics only. Do **not** admit any c63 edge, continue into targets, alter c62 facts/counters/candidates, or create recursive certificates.

### 5. Preserve c62 output and all historical semantics

Keep unchanged:

- independent EMPTY-root scans;
- c53 must/GPR Fact semantics;
- c54/c55 CALL preservation and RSP kill;
- c57/c59/c61 diagnostic proof domains;
- c60/c62 exact instance/path/full-prefix admission;
- finite caps and whole-root fail-closed transactions;
- candidate commit only from completed roots and incoming carried Facts.

No memory-loaded values/pointers, slot qword reads, pointer chase, stack/local/return facts, ABI assumptions, immediate-pointer generality, indexed/segment generality, partial-register recovery, EH/alternate-entry inference, mitigation changes, producer traversal, production/gameplay writes, native probe, or game run.

### 6. Required diagnostics

Emit compact exact totals including at minimum:

```text
repository_result
image_authenticated / image_sha256
accepted c57/c58/c59/c60/c61/c62 identities
post_c61_uncertified_boundary_refused_roots_expected = 2923
post_c61_uncertified_boundary_refused_roots_classified
post_c61_boundary_instances_total
unique_entry_instance_states
unique_source_rows
post_c61_boundary_edges_total
architectural_post_c61_boundary_edges_total
model_only_post_c61_boundary_edges_total
edge_kind_counts
target_metadata_relation_counts
exact_runtime_begin_edge_kind_counts
interior_target_edge_kind_counts
metadata_free_RX_edge_kind_counts
exact_adjacency_counts
terminal/provenance phase counts where meaningful
classification_complete
new_post_c61_boundaries_admitted = 0
cross_runtime_fact_propagation_unchanged = true
c62_candidate_output_unchanged = true
c62_candidate_output_sha256
new_cross_row_candidate_count = 0
```

Publish deterministic classifier identity plus authenticated ordered record-set SHA-256. Keep root/instance/edge scopes explicit.

### 7. Safety/completeness remain false

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

## Required tests and reporting

Focused test:

```bash
python3 source/tools/test_slot_publication_post_c61_boundary.py
```

Expected: every focused group PASS, exit 0. Cover exact domain selection, pre-existing earlier refusal not relabeled, root/state/path/full-prefix/phase/origin identity binding, successor kinds/relations, architectural/model-only distinction, duplicate/missing/stale/wrong evidence poisoning, unchanged c62 result/candidate hash, no edge/fact/candidate admission, and all gates false.

Regression/validation from repository root with the pinned analysis Python environment:

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
python3 source/tools/test_slot_publication_post_c59_stitched.py
python3 source/tools/test_slot_publication_post_c61_boundary.py
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
git diff --check
```

Expected: focused c63 and all required historical owned tests PASS/exit0 except already-known optional SKIP/UNAVAILABLE cases, with **BLOCKED0**. Do not call unexecuted native/game tests PASS.

`docs/CODEX_RESULT.md` and c63 STATUS/analysis must record exact commands, expected results, actual results, branch/final commit/push verification, and every unrun test with its reason. User-facing reply must remain minimal per `docs/CODEX_REPORTING.md`.

If actual private-image evidence is unavailable in cloud, record c63 distribution UNKNOWN/null and provide one owner OFFLINE command that reuses existing c56 checkpoints+c57-c62 JSON read-only. The owner should return only the compact c63 JSON. No Update-WS/game/Overstrike/EXE upload/runtime log/native probe.

## Repository outputs

Add/update only c63-scoped tool/test/evidence/status/gate analysis plus required project/result/package metadata. Preserve historical owner reports and source bytes unless the bounded task requires a reviewed c63 integration point. Do not merge `main`.
