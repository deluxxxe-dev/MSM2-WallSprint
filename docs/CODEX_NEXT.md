# Next task — c59 later entry-instance runtime-boundary classification

## Status

Accepted owner c58 **VERIFIED-STATIC** result:

- `analysis/V01C58_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C58_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

C58 authenticated the exact pinned image and proved the bounded entry-instance mechanism can carry existing c53 image-address facts across some authenticated exact-runtime-begin boundaries without globalizing them:

```text
repository_result = EXACT_RUNTIME_BEGIN_STITCH_SCANNER_READY
exact_runtime_begin_edges_available = 159734
exact_runtime_begin_edges_considered = 147338
exact_runtime_begin_edges_admitted = 1299
rows_reached_by_continuation = 482
address_facts_crossing_runtime_rows = 1686
new_cross_row_candidate_count = 0
roots_completed = 81896 / 162472
root_transactions_refused = 80576
```

Largest c58 root refusal:

```text
uncertified_later_runtime_boundary = 33719
inside_unique_runtime_span_nonentry = 29818
executable_file_backed_outside_runtime_metadata = 12526
indirect_call = 4330
```

The 33719 later-boundary refusals are not authorized continuation edges. C57 authenticated only the accepted first-boundary diagnostic set; a later edge reached inside a non-empty c58 entry instance must not inherit that certificate by assumption.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, `analysis/V01C58_OWNER_OFFLINE_RESULT.md`, c53-c58 scanner evidence/tools/tests, and relevant Wisdoms references.

## Current bounded task

C59 is **OFFLINE/STATIC diagnostic classification only**.

Classify every c58 `uncertified_later_runtime_boundary` refusal reached from an authenticated entry-instance replay. Do **not** admit or stitch any new later boundary in c59. Candidate output and c58 semantics remain unchanged.

### 1. Preserve c58 entry-instance identity

Every diagnostic record must remain bound to the exact c58 context that reached the later boundary. At minimum bind:

```text
original_root_runtime_row
current_entry_instance_row
incoming_state_identity
boundary_path_identity / transition identity
source instruction RVA + exact bounded byte/hash identity
first refusal = uncertified_later_runtime_boundary
```

Do not collapse two different incoming states or predecessor paths into one global row fact. Identical records may be deterministically deduplicated only when their authenticated instance identity is exactly equal.

### 2. Reuse c57 edge semantics, not c57 admission identity

Recover the exact architectural successor(s) from the later boundary instruction using the unchanged c53/c57 decoder and successor semantics.

Classify edge kinds at minimum as:

```text
ordinary_fallthrough
direct_unconditional_branch
direct_conditional_taken
conditional_fallthrough
call_fallthrough
other_supported_direct_successor
```

Classify target relation at minimum as:

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

Use exact current image/runtime metadata. No nearest-entry inference. Adjacency is diagnostic only.

### 3. Authenticate the later-boundary source independently

A record is accepted only when the exact pinned image, c56 manifest, accepted c57 identity, c58 tool/contract/proof identity, source row bytes, entry-instance incoming-state identity, and replayed first-refusal instruction all match.

Stale/missing/corrupt/wrong-image/wrong-tool/wrong-schema/wrong-contract/wrong-state/wrong-path evidence must refuse the aggregate.

Do not assume a c57 first-boundary record authenticates the same row/instruction in a later c58 instance. Prove the later diagnostic source independently from the c58 replay.

### 4. Diagnostic output

Emit compact exact totals including at minimum:

```text
repository_result
image_authenticated / image_sha256
accepted_c57_boundary_identity
accepted_c58 result/proof identity
later_boundary_refused_roots_expected = 33719
later_boundary_refused_roots_classified
later_boundary_instances_total
later_boundary_edges_total
edge_kind_counts
target_metadata_relation_counts
exact_runtime_begin_edge_kind_counts
interior_target_edge_kind_counts
metadata_free_RX_edge_kind_counts
exact_adjacency_counts
unique_source_rows
unique_entry_instance_states
bounded examples per major class
classification_complete
cross_runtime_fact_propagation_unchanged
new_cross_row_candidate_count = 0
candidate_sweep_complete = false
writer_enumeration_complete = false
```

If one refused root contains more than one later-boundary instance, keep root count and instance/edge counts separate. Do not silently equate them.

### 5. No semantic broadening in c59

C59 must not:

- carry facts across a newly classified later boundary;
- admit runtime-interior or metadata-free targets;
- resolve indirect calls/jumps;
- add memory-loaded value/pointer propagation;
- read the slot qword or chase pointers;
- traverse the deferred producer;
- add hardware watch/PAGE_GUARD/runtime instrumentation;
- change production/gameplay behavior.

C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding.

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH completeness gates remain false.

### 6. Focused tests

Add a focused command, preferably:

```bash
python3 source/tools/test_slot_publication_later_boundary.py
```

Cover at minimum:

- later ordinary fallthrough to exact runtime begin is classified but not admitted;
- later conditional taken/fallthrough and direct unconditional edge classification;
- CALL fallthrough classification without new callee semantics;
- exact adjacent vs non-adjacent begin remains diagnostic only;
- runtime-span interior target;
- metadata-free RX target;
- overlap/unbacked/nonexec/outside/deferred target;
- two different c58 incoming states reaching the same row/instruction remain distinct;
- identical authenticated instance records deduplicate deterministically;
- wrong c58 incoming-state/path identity refuses;
- stale source instruction/code bytes refuse;
- missing/duplicate/mismatched root or instance records poison aggregation;
- bounded examples never truncate exact counts;
- c58 candidate output remains unchanged and no new candidate escapes;
- historical c38/c53/c54/c55/c56/c57/c58 tests remain green and unchanged.

Synthetic/model success is not private-image evidence.

## Repository result

Emit exactly one:

```text
LATER_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
LATER_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED
```

`READY` means only that c58 later-boundary refusals can be authenticated and classified deterministically/fail-closed. It does not authorize later-boundary stitching or prove writer/publication completeness.

If the exact image is unavailable in Codex, publish one owner OFFLINE command that reuses the owner's c56 checkpoints, accepted c57 JSON and accepted c58 JSON read-only where possible and emits one compact c59 JSON. No game launch, Update-WS, Overstrike, EXE upload, hardware watch or runtime log.

## Required validation

Run from repository root:

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
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If the focused test uses a different exact filename, run/report that exact command instead.

Expected:

- all historical behavior remains green and immutable;
- c59 classifies the exact accepted later-boundary refusal domain without carrying facts across it;
- no candidate/publication/completeness claim broadening;
- no production/gameplay change.

Report every executed command, expected and actual result, PASS/SKIP/UNAVAILABLE/BLOCKED, every unrun test with reason, branch + commit SHA, verified remote publication, and the exact owner action if private-image classification is needed.
