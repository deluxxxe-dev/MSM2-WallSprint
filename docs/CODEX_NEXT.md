# Next task — c60 authenticated later-runtime-begin entry-instance continuation

## Status

Accepted owner c59 **VERIFIED-STATIC** result:

- `analysis/V01C59_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C59_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

C59 authenticated and completely classified the accepted c58 first-terminal `uncertified_later_runtime_boundary` domain:

```text
repository_result = LATER_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
later_boundary_refused_roots_expected = 33719
later_boundary_refused_roots_classified = 33719
later_boundary_instances_total = 33719
later_boundary_edges_total = 35921
architectural_later_boundary_edges_total = 35921
model_only_later_boundary_edges_total = 0
classification_complete = true
```

Target distribution:

```text
exact_unique_runtime_begin = 28201        # 78.5084%
inside_unique_runtime_span_nonentry = 1566 # 4.3596%
executable_file_backed_outside_runtime_metadata = 6154 # 17.1320%
all overlap/unbacked/nonexec/outside/deferred classes = 0
```

C59 also reports `later_boundary_instances_with_empty_entry_seed=33582` and `later_boundary_instances_in_original_root=33582`. These are **entry-seed** diagnostics before the row-local must-fixpoint, not the state at the refusing instruction. Do not infer that those boundary instructions carry no address facts.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, `analysis/V01C59_OWNER_OFFLINE_RESULT.md`, c53-c59 scanner evidence/tools/tests, and relevant Wisdoms references.

## Current bounded task

C60 is **OFFLINE/STATIC address-provenance continuation only**.

Extend the c58 entry-instance scanner so it may continue across a c59-authenticated later boundary only when the exact current c58 entry-instance identity and the exact c59 diagnostic record independently authorize an architectural `exact_unique_runtime_begin` successor.

Do not generalize later-boundary admission dynamically. C59 certifies only the measured first terminal later-boundary record for each accepted c58 root/instance. Any new boundary reached after a c59-certified continuation remains uncertified and must fail closed.

Preferred shape: a separate `v0.1c60` tool, e.g. `slot_publication_later_stitched.py`, reusing historical modules read-only.

### 1. Authenticate all input evidence before admission

Require the exact pinned image and independently reconstruct/validate the accepted c56/c57/c58/c59 chain.

Prefer read-only owner inputs:

- c56 checkpoint directory;
- accepted c57 JSON;
- accepted c58 JSON;
- accepted c59 JSON.

The compact c59 JSON does not contain every diagnostic record. Recompute the complete c59 ordered record stream from unchanged c59 logic, verify its aggregate/record-set/classifier identities against the accepted owner c59 JSON, then use only those recomputed authenticated records as c60 proof material.

Missing/stale/corrupt/wrong-image/wrong-tool/wrong-schema/wrong-contract/wrong-count/wrong-record-set evidence must refuse. Do not repair or fall back to a weaker mode when owner evidence was supplied.

### 2. c59 proof is entry-instance-specific, never row-global

A c59 later-boundary certificate must remain keyed to the exact context that produced it. At minimum bind:

```text
original_root_runtime_row
current_entry_instance_row
incoming_state_identity
boundary_path_identity / transition identities
source instruction RVA/size/hash
c59 record identity / classifier identity
```

Do not convert a c59 record into a global certificate for that runtime row or instruction.

If the same source row/instruction is reached under a different root, incoming state or predecessor path, that other context is not admitted unless it has its own independently authenticated c59 record.

The historical c57 first-boundary row proofs remain separate and unchanged.

### 3. Admitted c59 later-edge contract

For the exact current c58/c60 entry instance, a c59 later successor may be admitted only when:

- the source context exactly matches one authenticated c59 record;
- source row/code/instruction identity is current and exact;
- the successor is architectural;
- c59 classified that exact successor as `exact_unique_runtime_begin`;
- target row index/begin/end is exact and unique;
- source and target spans do not overlap;
- target code passes the same immutable/file-backed executable preflight already used by c58;
- source/target are outside the deferred producer exclusion;
- no evidence/cap/identity failure exists.

Adjacency is not required for an authenticated direct control transfer and never substitutes for proof.

The following remain refused:

- `inside_unique_runtime_span_nonentry`;
- `executable_file_backed_outside_runtime_metadata`;
- overlap/unbacked/nonexec/outside/deferred targets;
- model-only successors;
- indirect calls/jumps;
- EH/alternate entries not represented by an authenticated admitted edge;
- any boundary reached after the c59-certified first later boundary unless separately present in the authenticated c59 record set for that exact current instance.

Use a distinct refusal such as `uncertified_post_c59_runtime_boundary` for the next unknown boundary wave.

### 4. Preserve all-successor fail-closed semantics

A multi-successor later-boundary instruction is not partially safe merely because one outgoing edge is exact.

If any required row-external successor of that instruction is nonarchitectural, nonexact, stale, unmatched, unsupported or otherwise refused, abort the entire root transaction exactly as c58 does. No candidate may escape from a partially admitted boundary.

Count per-edge and per-boundary-instance admission separately so the 28,201 exact edge total is never mistaken for 28,201 safely admitted boundary instances.

### 5. Entry-instance and fact semantics stay c58-exact

Retain c58 semantics unchanged:

- independent EMPTY-root scans remain separate;
- different incoming carried states never merge into a fabricated global fact;
- identical continuation states may deduplicate deterministically;
- c53 must-join semantics remain exact inside each instance;
- only existing c53 GPR image-address Facts are propagated;
- c54/c55 CALL preservation remains the only CALL fact-preservation rule;
- RSP remains killed;
- boundary-path proof metadata stays separate from the address derivation chain;
- candidates are emitted only from completed root transactions and only when the exact-slot operand depends on an incoming carried fact, as in c58.

Do not add memory-loaded values/pointers, slot qword reads, pointer chase, stack/local/return facts, ABI assumptions, immediate-pointer generality, indexed/segment generality or partial-register recovery.

### 6. Bounds / next unknown wave

Keep deterministic finite caps. The existing c58 limits are the preferred starting point unless a stricter reviewed reason requires a separate c60 cap set.

State-changing cycles or cap exhaustion refuse the whole affected root transaction with zero provisional candidates.

If a c59-admitted continuation reaches another row-external boundary that is not authenticated for that exact current entry instance, refuse it. Do not recursively infer that because its target is an exact runtime begin it is safe.

The c60 output must expose the next refusal distribution so a later task can diagnose it rather than silently broadening again.

### 7. Required diagnostics

Emit compact exact totals including at minimum:

```text
repository_result
image_authenticated / image_sha256
accepted c57/c58/c59 identities
c59_later_boundary_instances_available = 33719
c59_later_architectural_edges_available = 35921
c59_later_exact_edges_available = 28201
c59_boundary_instances_considered
c59_boundary_instances_admitted
c59_boundary_instances_refused
c59_exact_edges_considered
c59_exact_edges_admitted
c59_exact_edges_refused
continuation_instances_created / deduplicated / refused
rows_reached_by_continuation
address_facts_crossing_runtime_rows
c59_certified_crossings_with_facts
call_edges_with_preserved_facts_crossing_rows
root_transactions_attempted / completed / refused
root_refusal_reasons
post_c59_uncertified_boundary_refusals
remaining_boundary_refusal_relation_counts
reference/reader/writer/rmw/address_only candidate counts
new_cross_row_candidate_count
candidate_sweep_complete
writer_enumeration_complete
all publication/allocation/base/range/lifetime/ownership/destination/CFG/EH gates
```

Keep count scopes explicit. Historical c58/c59 totals are not automatically comparable with new c60 per-instance totals unless the domain is identical.

### 8. Completeness and safety remain false

C60 must preserve:

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

Even if c60 finds zero new candidate, do not claim no writer. Interior targets, metadata-free RX, indirect control, EH/alternate entries, executable gaps, unsupported semantics and deferred producer remain open.

C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding.

### 9. Focused tests

Add a focused command, preferably:

```bash
python3 source/tools/test_slot_publication_later_stitched.py
```

Cover at minimum:

- exact c59 later ordinary fallthrough carries a proven fact;
- exact c59 later conditional taken/fallthrough;
- exact c59 later non-adjacent direct branch;
- CALL fallthrough uses only c54/c55 preserved facts;
- c59 proof must match exact root/current-row/incoming-state/path/source identity;
- same row/instruction under a different state/path refuses;
- c59 record is never promoted to a row-global certificate;
- one exact and one interior/free-RX successor aborts the whole boundary/root;
- all-exact multi-successor boundary may continue when every edge is authenticated;
- runtime-interior/free-RX/overlap/unbacked/nonexec/outside/deferred/model-only targets refuse;
- an uncertified boundary after one c59 continuation refuses as `uncertified_post_c59_runtime_boundary` (or exact equivalent);
- independent EMPTY-root behavior remains separate;
- identical continuation states deduplicate; different states stay separate;
- state-changing cycle/cap failure emits no candidate;
- stale/wrong c56/c57/c58/c59 evidence refuses;
- safe multi-boundary exact-slot candidate is reported with both first-boundary and c59-later boundary provenance;
- candidate not dependent on incoming carried fact is not promoted;
- historical c38/c53/c54/c55/c56/c57/c58/c59 tests remain green and unchanged.

Synthetic/model success is not private-image evidence.

## Repository result

Emit exactly one:

```text
LATER_RUNTIME_BEGIN_STITCH_SCANNER_READY
LATER_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED
```

`READY` means only that c59-authenticated later exact-runtime-begin continuation is bounded, entry-instance-specific, fail-closed and tested. It does not mean the private image has been rescanned, that a writer exists/does not exist, or that publication/production is solved.

If the exact image is unavailable to Codex, publish one owner OFFLINE command that reuses the owner's c56 checkpoints plus accepted c57/c58/c59 JSON read-only where possible and emits one compact c60 JSON. No game launch, Update-WS, Overstrike, EXE upload, hardware watch or runtime log.

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
python3 source/tools/test_slot_publication_later_stitched.py
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If the focused c60 test uses a different exact filename, run/report that exact command instead.

Expected:

- historical c38-c59 behavior remains green and immutable;
- only exact current-instance c59 `exact_unique_runtime_begin` edges can become the new later continuation class;
- no row-globalization of c59 records;
- all-successor fail-closed semantics are preserved;
- a new uncertified post-c59 boundary remains a refusal;
- no value/pointer semantic broadening;
- no production/gameplay behavior change;
- all completeness/publication/ownership/lifetime gates remain false.

## Required outputs

Update/create:

- separate c60 offline tool/mode and focused tests;
- `analysis/V01C60_*` and source mirror as appropriate;
- `docs/PROJECT_STATE.md`: record accepted c58 and c59 owner VERIFIED-STATIC results as historical and c60 as current;
- `docs/CODEX_RESULT.md`;
- Wisdoms only if a reusable multi-boundary entry-instance must-analysis rule is established;
- commit and push the implementation branch, then verify remote publication.

Do not overwrite `analysis/V01C58_OWNER_OFFLINE_RESULT.md` or `analysis/V01C59_OWNER_OFFLINE_RESULT.md`.

Final report must follow `docs/CODEX_REPORTING.md`: every executed command, expected/actual result, PASS/SKIP/UNAVAILABLE/BLOCKED, every unrun test with reason, branch + commit SHA, publication verification, and exact owner action if a private-image scan is needed.
