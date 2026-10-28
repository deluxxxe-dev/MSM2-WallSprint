# Next task — c65 post-c63 runtime-boundary diagnostic classification

## Status

Accepted owner c64 **VERIFIED-STATIC** result:

- `analysis/V01C64_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C64_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

Accepted c64 key totals:

```text
repository_result = POST_C61_RUNTIME_BEGIN_STITCH_SCANNER_READY
root_transactions_attempted = 162472
root_transactions_completed = 105083
root_transactions_refused = 57389
c63_boundary_instances_considered = 2923
c63_boundary_instances_admitted = 1150
c63_boundary_instances_refused = 1773
c63_exact_edges_considered = 3049
c63_exact_edges_admitted = 1346
c63_exact_edges_refused = 1703
rows_reached_by_continuation = 1549
address_facts_crossing_runtime_rows = 2881
post_c63_uncertified_boundary_refusals = 998
new_cross_row_candidate_count = 0
```

The new bounded unknown wave is exactly the `998` roots whose first terminal refusal after c63-authenticated continuation is `uncertified_post_c63_runtime_boundary`.

`docs/PROJECT_STATE.md` may still describe c64 owner metrics as pending. For this bounded task the accepted owner result above and the two V01C64 owner result files supersede only that pending c64 distribution. Historical c56-c63 facts and all safety gates remain unchanged.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, `analysis/V01C64_OWNER_OFFLINE_RESULT.md`, c56-c64 tools/tests/evidence, and relevant Wisdoms references.

## Current bounded task

C65 is **OFFLINE/STATIC DIAGNOSTIC ONLY**.

Build a separate c65 diagnostic classifier for the exact first terminal `uncertified_post_c63_runtime_boundary` instance from each accepted c64 refused root. Classify that boundary and its outgoing successors; admit no new edge, carry no additional Fact, and emit no new candidate.

Preferred tool/test:

```text
source/tools/slot_publication_post_c63_boundary.py
source/tools/test_slot_publication_post_c63_boundary.py
```

Preferred repository result:

```text
POST_C63_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
```

Use a separate v0.1c65/schema1 mode. Historical tools/results remain read-only.

### 1. Authenticate the complete accepted chain

Require the exact pinned image and independently validate/reconstruct the accepted c56→c64 chain.

Preferred read-only owner inputs:

- c56 checkpoint directory;
- accepted c57 JSON;
- accepted c58 JSON;
- accepted c59 JSON;
- accepted c60 JSON;
- accepted c61 JSON;
- accepted c62 JSON;
- accepted c63 JSON;
- accepted c64 JSON.

Replay unchanged c64 transaction semantics sufficiently to reproduce the accepted owner c64 result and validate supplied c64 schema/contract/tool/image/result/counts before using any terminal context diagnostically. Missing/stale/corrupt/wrong evidence must refuse; never repair evidence.

### 2. Exact diagnostic domain

Expected root domain:

```text
post_c63_uncertified_boundary_refused_roots_expected = 998
```

Record only the first terminal `uncertified_post_c63_runtime_boundary` reached under unchanged c64 semantics for that original root. Do not explore past the refusal.

Require exactly one authenticated diagnostic record per expected refused root unless deterministic replay proves a stricter root/instance distinction; then report both scopes explicitly. Missing/duplicate/out-of-order/wrong-root/wrong-state/wrong-path/wrong-source/wrong-provenance records poison the aggregate.

### 3. Bind exact c64 context

At minimum bind each record to:

```text
original_root_runtime_row
current_entry_instance_row
incoming_state_identity
entry_instance_identity
boundary_path_identity / transition identities
source instruction RVA / size / SHA256
source runtime-row identity / code identity
accepted c63 classifier/record-set/result identities
accepted c64 authentication/result identity
c63 terminal phase/reason/provenance carried by c64
current c64 continuation/full-prefix identity needed to distinguish this exact instance
```

Never promote a record to row-global or instruction-global proof. Preserve c63 phase/reason/provenance distinctions, including converted c60-POST context; do not borrow earlier c61 authorization.

### 4. Classify successors only

Use the existing architectural successor and target-relation vocabulary unchanged. Preserve architectural vs model-only labeling and exact adjacency as diagnostics only.

Do **not** admit any c65 edge, continue into targets, alter c64 facts/counters/candidates, or create recursive certificates.

### 5. Preserve historical semantics and gates

Keep unchanged independent EMPTY-root scans; c53 must/GPR Facts; c54/c55 CALL preservation and RSP kill; c57/c59/c61/c63 diagnostic domains; c60/c62/c64 exact context/path/full-prefix admission; finite caps; whole-root fail-closed transactions; and candidate commit only from completed roots and incoming carried Facts.

No memory-loaded values/pointers, slot qword reads, pointer chase, stack/local/return facts, ABI assumptions, EH/alternate-entry inference, mitigation changes, producer traversal, production/gameplay writes, native probe, or game run.

All completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false. C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding. Zero candidates is not writer absence.

## Required diagnostics

Emit deterministic classifier and ordered-record-set identities plus exact counts for:

```text
repository_result
image_authenticated / image_sha256
accepted c57-c64 identities
post_c63_uncertified_boundary_refused_roots_expected = 998
post_c63_uncertified_boundary_refused_roots_classified
post_c63_boundary_instances_total
unique_entry_instance_states
unique_source_rows
post_c63_boundary_edges_total
architectural_post_c63_boundary_edges_total
model_only_post_c63_boundary_edges_total
edge_kind_counts
target_metadata_relation_counts
exact_runtime_begin_edge_kind_counts
interior_target_edge_kind_counts
metadata_free_RX_edge_kind_counts
exact_adjacency_counts
terminal/provenance phase/reason counts where meaningful
classification_complete
new_post_c63_boundaries_admitted = 0
cross_runtime_fact_propagation_unchanged = true
c64_candidate_output_unchanged = true
c64_candidate_output_sha256
new_cross_row_candidate_count = 0
```

## Required tests and reporting

Focused command:

```bash
python3 source/tools/test_slot_publication_post_c63_boundary.py
```

Expected: all focused groups PASS, exit 0. Cover exact 998-root domain selection, exact c64 prefix/state/path/source/provenance binding, successor kinds/relations, architectural/model-only distinction, duplicate/missing/stale/wrong evidence poisoning, unchanged c64 result/candidate hash, no edge/fact/candidate admission, and all gates false.

Then run the same regression/build/package suite used by c64. Record in `docs/CODEX_RESULT.md`:

- every test/check actually run;
- exact command;
- expected result;
- actual result;
- branch/commit and push/publication verification;
- every unrun test and exact reason.

Update `docs/PROJECT_STATE.md` to record accepted c64 owner VERIFIED-STATIC totals and c65 TOOL READY status. Keep user-facing report minimal per `docs/CODEX_REPORTING.md`.

## Owner handoff

After implementation is pushed and independently verified, provide one owner OFFLINE command only. Reuse existing c56 checkpoints and accepted c57-c64 JSONs read-only and output:

```text
logs/MSM2-WS-v0.1c65-post-c63-runtime-boundary.json
```

No Update-WS, game, Overstrike, runtime capture, native probe, evidence editing, or regeneration unless a separately proven stale/missing input exists.
