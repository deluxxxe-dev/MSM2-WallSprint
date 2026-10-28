# Next task — c64 c63-authenticated post-c61 continuation

## Status

Accepted owner c63 **VERIFIED-STATIC** result:

- `analysis/V01C63_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C63_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

Accepted c63 key totals:

```text
repository_result = POST_C61_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
schema_version = 2
post_c61_uncertified_boundary_refused_roots_expected = 2923
post_c61_uncertified_boundary_refused_roots_classified = 2923
post_c61_boundary_instances_total = 2923
unique_entry_instance_states = 2923
unique_source_rows = 2880
post_c61_boundary_edges_total = 3167
architectural_post_c61_boundary_edges_total = 3167
model_only_post_c61_boundary_edges_total = 0
exact_unique_runtime_begin = 3049
inside_unique_runtime_span_nonentry = 109
executable_file_backed_outside_runtime_metadata = 9
classification_complete = true
new_post_c61_boundaries_admitted = 0
new_cross_row_candidate_count = 0
```

Terminal provenance remains part of the proof and must stay distinct:

```text
terminal_phase_counts:
  same_instance_after_c61_preflight_before_row_fixpoint = 2679
  post_c61_reachable_context = 226
  post_c61_reachable_dedup_stored_earlier_path = 3
  prior_c61_authorization_converted_c60_post = 15

terminal_reason_counts:
  used_c61_source_later_instruction = 2679
  current_context_post_c61_reachable = 229
  prior_c61_authorization_converted_context_match_failure = 15
```

The `3049` exact value is an **edge count**, not an admissible instance/root count. Multi-successor all-successor semantics remain binding.

`docs/PROJECT_STATE.md` still describes the pre-owner-rerun c63 pending state; for this bounded task the accepted owner result above and the two V01C63 owner result files supersede only that pending c63 distribution. Historical c56-c62 facts and all safety gates remain unchanged.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, `analysis/V01C63_OWNER_OFFLINE_RESULT.md`, c56-c63 tools/tests/evidence, and relevant Wisdoms references.

## Current bounded task

C64 is **OFFLINE/STATIC address-provenance continuation only**.

Build a separate c64 continuation tool that reconstructs/authenticates the complete accepted c56→c63 chain and may continue across a c63-recorded terminal boundary only for the exact current context and only when every required outgoing architectural successor is independently classified `exact_unique_runtime_begin` and passes the existing immutable/file-backed executable preflight.

Preferred tool/test:

```text
source/tools/slot_publication_post_c61_stitched.py
source/tools/test_slot_publication_post_c61_stitched.py
```

Preferred repository result:

```text
POST_C61_RUNTIME_BEGIN_STITCH_SCANNER_READY
```

Use a separate v0.1c64/schema1 mode. Historical tools/results remain read-only.

### 1. Authenticate the complete accepted chain

Require the exact pinned image and independently reconstruct/validate accepted c56→c63 evidence.

Preferred owner inputs are read-only:

- c56 checkpoint directory;
- accepted c57 JSON;
- accepted c58 JSON;
- accepted c59 JSON;
- accepted c60 JSON;
- accepted c61 JSON;
- accepted c62 JSON;
- accepted c63 JSON.

The compact c63 JSON is not itself the per-record certificate pool. Recompute the complete ordered c63 record stream from unchanged c63 logic, validate aggregate/classifier/record-set identity against the accepted owner c63 result, then use only those authenticated records.

Missing/stale/corrupt/wrong-image/wrong-tool/wrong-schema/wrong-contract/wrong-count/wrong-record-set evidence must refuse. Never repair or fall back to weaker evidence.

### 2. c63 proof is exact context and provenance specific

A c63 authorization must bind at minimum:

```text
original_root_runtime_row
current_entry_instance_row
incoming_state_identity
entry_instance_identity
boundary_path_identity / transition identities
source instruction RVA / size / SHA256
source runtime-row/code identity
terminal_phase
terminal_reason
terminal_provenance
post_c59_origin / post_c61_origin where present
c61 authorization identity and phase
accepted c62 authentication/result identities
c63 record/classifier identity
```

Never promote a c63 record to row-global or instruction-global authorization.

For the 15 `prior_c61_authorization_converted_c60_post` / `prior_c61_authorization_converted_context_match_failure` records, preserve the exact converted underlying refusal, failed match conjunct and scan/path provenance. Do not relabel them as post-c61 reachable and do not borrow the prior c61 certificate. Any c64 permission comes only from the exact authenticated c63 record for the current context.

### 3. c63 successor admission contract

For the exact current c63 terminal boundary, a successor may be usable only when:

- exact c63 root/state/path/source/phase/reason/provenance identity matches current replay;
- successor is architectural;
- c63 classified that exact successor as `exact_unique_runtime_begin`;
- target runtime row/begin/end is exact and unique;
- source and target spans do not overlap;
- target current code passes unchanged immutable/file-backed executable preflight;
- source/target remain outside deferred producer exclusion;
- no evidence/cap/identity/path/provenance conflict exists.

Remain refused: the accepted c63 109 interior edges, 9 metadata-free RX edges, model-only/missing/stale/unsupported targets, indirect transfers, and any next unknown row-external boundary not explicitly authorized by the accepted c63 record set.

Use a distinct next-wave refusal such as:

```text
uncertified_post_c63_runtime_boundary
```

Do not dynamically create c63/c64 certificates.

### 4. Preserve all-successor fail-closed semantics

Before any c63 edge becomes usable, validate **all required row-external successors** of that exact boundary instruction. If any successor is nonarchitectural, nonexact, stale, mismatched, unsupported, or fails target preflight, abort the whole root transaction and commit no provisional candidate.

Count edge admission separately from boundary-instance admission. Never interpret `3049 exact edges` as `3049 safe instances`.

### 5. Preserve historical fact/transaction semantics

Retain unchanged:

- independent EMPTY-root scans;
- c53 must-join and exact GPR image-address Facts only;
- c54/c55 CALL preservation as the only CALL fact-preservation rule;
- RSP kill;
- c57/c59/c61/c63 diagnostic domains;
- c60/c62 exact context/path/full-prefix admission;
- c63 conversion-provenance distinction;
- deterministic finite caps;
- candidate commit only from completed roots and incoming carried Facts;
- whole-root fail-closed rollback.

Do not add memory-loaded values/pointers, slot qword reads, pointer chase, stack/local/return facts, ABI assumptions, EH inference, mitigation changes, producer traversal, gameplay/production writes, native probes, or game use.

### 6. Diagnostics and safety

Emit exact counts for c63 instances/edges considered/admitted/refused, phase/reason counts, preflight refusals, continuation instances, rows/facts reached, root transactions, next-wave refusals, candidates, and all existing false gates. Preserve explicit count scopes.

All completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false. C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding. Zero candidates never proves writer absence.

## Required tests and reporting

Focused command:

```bash
python3 source/tools/test_slot_publication_post_c61_stitched.py
```

Expected: all focused groups PASS, exit 0.

Cover at minimum exact ordinary/conditional/direct/CALL-fallthrough continuation, all-successor rollback, all four c63 terminal phases, all three terminal reasons, the 15 converted-context shape without borrowed c61 authorization, stale/mismatched phase/reason/provenance refusal, different-path same-state dedup refusal, next unknown wave fail-closed, unchanged historical candidate output, and all gates false.

Then run the same regression/build/package suite required by the published c63 repair task. Record in `docs/CODEX_RESULT.md`:

- exact commands executed;
- expected results;
- actual results;
- branch/commit and push/publication verification;
- every unrun test and its reason.

## Owner handoff

After implementation is pushed and independently verified, provide one owner OFFLINE command only. Reuse existing c56 checkpoints and accepted c57-c63 JSONs read-only. Output a compact c64 JSON under `logs/`. No Update-WS, game, Overstrike, runtime log, native probe, evidence editing, or regeneration unless a separately proven stale/missing input exists.
