# Next task — c62 c61-authenticated terminal-POST continuation

## Status

Accepted owner c61 **VERIFIED-STATIC** result:

- `analysis/V01C61_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C61_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

Accepted c61 key totals:

```text
repository_result = POST_C59_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
post_c59_uncertified_boundary_refused_roots_expected = 6566
post_c59_uncertified_boundary_refused_roots_classified = 6566
post_c59_boundary_instances_total = 6566
unique_entry_instance_states = 6566
unique_source_rows = 6467
post_c59_boundary_edges_total = 7206
architectural_post_c59_boundary_edges_total = 7206
model_only_post_c59_boundary_edges_total = 0
exact_unique_runtime_begin = 6946
inside_unique_runtime_span_nonentry = 245
executable_file_backed_outside_runtime_metadata = 15
classification_complete = true
new_post_c59_boundaries_admitted = 0
new_cross_row_candidate_count = 0
```

Terminal phases are part of the evidence and must remain distinct:

```text
same_instance_after_c59_preflight_before_row_fixpoint = 5759
post_c59_reachable_context = 805
post_c59_reachable_dedup_stored_earlier_path = 2
```

The `6946` exact value is an **edge count**, not an admissible instance/root count. Multi-successor all-successor semantics remain binding.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, `analysis/V01C61_OWNER_OFFLINE_RESULT.md`, c56-c61 tools/tests/evidence, and relevant Wisdoms references.

## Current bounded task

C62 is **OFFLINE/STATIC address-provenance continuation only**.

Build a separate c62 continuation tool that replays and authenticates the complete accepted c56→c61 evidence chain and may continue across a c61-recorded terminal POST boundary only for the exact current c60/c61 context and only when every required outgoing architectural successor is independently classified `exact_unique_runtime_begin` and passes current immutable/file-backed executable preflight.

Preferred tool/test:

```text
source/tools/slot_publication_post_c59_stitched.py
source/tools/test_slot_publication_post_c59_stitched.py
```

Preferred repository result:

```text
POST_C59_RUNTIME_BEGIN_STITCH_SCANNER_READY
```

Use a separate version/schema (v0.1c62/schema1). Historical tools/results stay read-only.

### 1. Authenticate the complete accepted chain

Require the exact pinned image and independently reconstruct/validate accepted c56→c61 evidence.

Preferred owner inputs are read-only:

- c56 checkpoint directory;
- accepted c57 JSON;
- accepted c58 JSON;
- accepted c59 JSON;
- accepted c60 JSON;
- accepted c61 JSON.

The compact c61 JSON is not itself the per-record certificate pool. Recompute the complete ordered c61 record stream from unchanged c61 logic, validate its aggregate/classifier/record-set identity against the accepted owner c61 result, then use only those authenticated records.

Missing/stale/corrupt/wrong-image/wrong-tool/wrong-schema/wrong-contract/wrong-count/wrong-record-set evidence must refuse. Do not repair or fall back to weaker evidence.

### 2. c61 proof is exact context/phase specific

A c61 authorization must bind at minimum:

```text
original_root_runtime_row
current_entry_instance_row
incoming_state_identity
entry_instance_identity
boundary_path_identity
boundary_transition_identities
source instruction RVA / size / SHA256
source runtime-row/code identity
terminal_phase
post_c59_origin
local_c59_record_identities
c61 record identity / classifier identity
accepted c60 authentication/result identities
```

Never promote a c61 record to row-global or instruction-global authorization. Same row/instruction under another root/state/path/phase/origin is unrelated unless independently recorded.

The three c61 terminal phases are semantically distinct:

- `same_instance_after_c59_preflight_before_row_fixpoint`: local c59 preflight exists, but do **not** fabricate a completed c59 crossing or add a fake c59 proof to the path.
- `post_c59_reachable_context`: require the authenticated c59 arrival/propagation origin represented by c61.
- `post_c59_reachable_dedup_stored_earlier_path`: stored first path and actual post-c59 origin differ; preserve both and never borrow a certificate from one to the other.

### 3. c61 successor admission contract

For the exact current c61 terminal boundary, a successor may be usable only when:

- exact c61 record/context/phase/origin/source identity matches current replay;
- successor is architectural;
- c61 classified that exact successor as `exact_unique_runtime_begin`;
- target runtime row/begin/end is exact and unique;
- source and target spans do not overlap;
- target current code passes the same immutable/file-backed executable preflight used by c60;
- source/target remain outside deferred producer exclusion;
- no evidence/cap/identity/path conflict exists.

Adjacency remains diagnostic only and never substitutes for proof.

Remain refused:

- `inside_unique_runtime_span_nonentry` (accepted c61 total 245 edges);
- `executable_file_backed_outside_runtime_metadata` (15 edges);
- overlap/unbacked/nonexec/outside/deferred targets;
- model-only successors;
- indirect calls/jumps;
- EH/alternate entries not represented by an authenticated edge;
- any later unknown row-external boundary reached after c61-certified continuation unless separately authorized by this exact accepted c61 record set for that exact context.

Use a distinct refusal such as:

```text
uncertified_post_c61_runtime_boundary
```

for the next unknown wave. Do not dynamically create c61/c62 certificates.

### 4. Preserve all-successor fail-closed semantics

A boundary instruction is not partially admissible because one outgoing edge is exact.

Before any c61 edge becomes usable, validate **all required row-external successors** of that exact boundary instruction. If any successor is nonarchitectural, nonexact, stale, mismatched, unsupported, or fails target preflight, abort the whole root transaction and commit no provisional candidate.

Count edge admission separately from boundary-instance admission. Do not interpret `6946 exact edges` as `6946 safe instances`.

### 5. Preserve c53-c60 fact and transaction semantics

Retain unchanged:

- independent EMPTY-root scans;
- c53 must-join and exact GPR image-address Facts only;
- c54/c55 CALL preservation as the only CALL fact-preservation rule;
- RSP kill;
- c57 first-boundary proofs;
- c59 first later-boundary exact-record semantics;
- c60 exact root/row/state/path admission and fail-closed root transaction;
- c61 terminal phase/origin distinction;
- deterministic finite caps;
- candidate commit only from completed roots and only when exact-slot operand depends on an incoming carried Fact.

Boundary/path/certificate metadata must stay separate from address derivation chains.

Do not add memory-loaded values/pointers, slot qword reads, pointer chase, stack/local/return facts, ABI assumptions, immediate-pointer generality, indexed/segment generality, partial-register recovery, EH inference, mitigation changes, or gameplay/production writes.

### 6. Dedup/path/origin safety

Identical state may deduplicate only with the existing deterministic semantics. A c61 certificate must not migrate through different-path same-state dedup.

If a later arrival changes whether a stored context is c61-certified/post-c61 reachable, preserve that provenance explicitly and fail closed if existing processed descendants could otherwise borrow authorization.

For the 2 accepted `post_c59_reachable_dedup_stored_earlier_path` records, focused tests must demonstrate that stored path and authenticated post-c59 origin remain separate and that neither can lend a certificate to the other.

### 7. Required diagnostics

Emit compact exact totals including at minimum:

```text
repository_result
image_authenticated / image_sha256
accepted c57/c58/c59/c60/c61 identities
c61_post_boundary_instances_available = 6566
c61_post_architectural_edges_available = 7206
c61_post_exact_edges_available = 6946
c61_boundary_instances_considered
c61_boundary_instances_admitted
c61_boundary_instances_refused
c61_exact_edges_considered
c61_exact_edges_admitted
c61_exact_edges_refused
c61_terminal_phase_considered/admitted/refused counts
c61_all_successor_preflight_refusal_relation_counts
continuation_instances_created / deduplicated / refused
rows_reached_by_continuation
address_facts_crossing_runtime_rows
c61_certified_crossings_with_facts
c61_address_facts_crossing_rows
call_edges_with_preserved_facts_crossing_rows
root_transactions_attempted / completed / refused
root_refusal_reasons
post_c61_uncertified_boundary_refusals
remaining_boundary_refusal_relation_counts
reference/reader/writer/rmw/address_only candidate counts
new_cross_row_candidate_count
candidate_sweep_complete
writer_enumeration_complete
all publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates
```

Keep count scopes explicit. Historical c60/c61 totals are not automatically comparable with c62 per-instance totals unless domains are identical.

### 8. Completeness and safety remain false

C62 must preserve:

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

C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding. Zero candidate still does not prove no writer.

### 9. Focused tests

Run:

```bash
python3 source/tools/test_slot_publication_post_c59_stitched.py
```

Expected: all focused groups PASS, exit 0.

Cover at minimum:

- exact c61 ordinary fallthrough continuation;
- exact conditional taken/fallthrough and non-adjacent direct branch;
- CALL fallthrough preserves only c54/c55-authorized facts;
- exact root/current-row/state/path/source/phase/origin c61 identity required;
- `same_instance_after_c59_preflight_before_row_fixpoint` does not fabricate a completed c59 crossing;
- `post_c59_reachable_context` requires matching c59 arrival/propagation origin;
- the 2 dedup-stored-earlier-path shape cannot borrow certificates;
- same row/instruction under different state/path/phase/origin refuses;
- one exact + one interior successor aborts whole boundary/root;
- one exact + one metadata-free RX successor aborts whole boundary/root;
- all-exact multi-successor boundary may continue only after complete preflight;
- model-only/overlap/unbacked/nonexec/outside/deferred targets refuse;
- next unknown boundary refuses `uncertified_post_c61_runtime_boundary` (or exact equivalent);
- identical continuation states deduplicate safely; different states remain separate;
- state-changing cycle/cap exhaustion emits no candidate;
- stale/wrong c56-c61 evidence refuses;
- candidate depends on incoming carried Fact and carries complete c57/c59/c61 provenance as applicable;
- factless structural continuation does not fabricate a candidate;
- all historical c38/c53-c61 tests remain green and unchanged.

Synthetic/model success is not private-image evidence.

### 10. Required validation and reporting

Run the existing required regression/build/package commands used by c61, plus the new focused c62 test. At minimum record in `docs/CODEX_RESULT.md`:

```text
python3 source/tools/test_slot_publication_post_c59_stitched.py
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
git diff --check
```

Also run the historical scanner tests c38/c53/c54/c55/c56/c57/c58/c59/c60/c61 directly or through the established nonduplicated regression path; do not double-count hook repetitions.

For every required test/check record:

- exact command;
- expected result;
- actual result;
- exit status / compact PASS-SKIP-UNAVAILABLE-BLOCKED totals where applicable;
- explicit reason for every unrun/UNAVAILABLE item.

Do not report an unexecuted native/game/private-image test as PASS. C62 is offline-only; no game/native probe is required unless implementation unexpectedly changes that scope.

### 11. Repository/documentation output

Update detailed c62 analysis/status/gate docs, `docs/PROJECT_STATE.md`, and `docs/CODEX_RESULT.md`. Preserve the accepted c61 owner result files as immutable history. Update Wisdoms only for genuinely reusable rules/findings and never include private raw artifacts.

Commit and push all intended changes to `codex/v01c20-edi-value-provenance`; verify remote publication. Do not merge `main` or force-push.

User-facing Codex reply must follow `docs/CODEX_REPORTING.md`: status, compact totals, branch/final SHA, and owner action only if required.

## Repository result

Emit exactly one primary state:

```text
POST_C59_RUNTIME_BEGIN_STITCH_SCANNER_READY
```

or

```text
POST_C59_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED
```

READY means the bounded c62 mechanism and validations are complete. It does **not** mean writer enumeration/publication/ownership/lifetime/production is complete.
