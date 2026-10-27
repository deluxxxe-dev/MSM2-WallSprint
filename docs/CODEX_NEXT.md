# Next task — c57 exact runtime-boundary exit classification

## Status

Accepted owner c56 VERIFIED-STATIC result:

- `analysis/V01C56_OWNER_OFFLINE_RESULT.md`
- `source/analysis/V01C56_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

C56 authenticated the exact pinned image and covered every runtime-function metadata row with deterministic sharding:

```text
runtime_functions_in_metadata = 527161
runtime_rows_covered = 527161
runtime_functions_not_considered = 0
shard_count = 4119
shard_failures = []
regions_modeled = 204770
regions_refused = 322390
regions_excluded = 1
writer_candidates = 0
candidate_sweep_complete = false
writer_enumeration_complete = false
```

This closes only the prior process-global omission of later runtime-metadata rows. It does not close executable gaps, refused regions, alternate/EH entries, indirect targets/writers, deferred producer, or publication/allocation/ownership/lifetime.

The largest c56 refusal classes are:

```text
unsupported_runtime_boundary = 162472
indirect_call = 153992
```

Together they account for about 98.16% of refused metadata rows. C57 addresses only the first class.

C56 shard-local additive summary counters are not globally unique c55 counters. In particular `call_clobber_all_address_origins_remaining=932595` must not be compared numerically with c55's single-process `301194` as a regression/improvement.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, `analysis/V01C56_OWNER_OFFLINE_RESULT.md`, c53-c56 scanner evidence/tools/tests, and relevant Wisdoms references.

## Current bounded task

C57 is **OFFLINE/STATIC diagnostic classification only**.

Determine exactly why the existing c53/c56 per-runtime-row CFG rejects `unsupported_runtime_boundary`, and classify every such outgoing edge on the exact authenticated image. Do **not** yet carry register/address facts across runtime rows and do not change candidate output.

Preferred shape: a separate `v0.1c57` tool/mode or a strictly diagnostic layer over c56 that can be run once on the owner image and emits compact aggregate JSON.

### 1. Preserve all existing semantics

Do not change c53-c56 candidate admission, summary contracts, access semantics, refusal behavior, deferred producer exclusion, external c28 handling, or historical outputs.

No cross-row CFG stitching is authorized in c57.

No runtime-span interior target, fallthrough across a metadata boundary, nearest metadata entry, overlapping metadata row, or metadata-free target becomes an admitted continuation merely because control reaches it.

### 2. Classify every boundary exit exactly

For every c56 row whose first/terminal refusal is `unsupported_runtime_boundary`, recover the exact outgoing successor(s) that crossed the row's half-open runtime span and report bounded deterministic diagnostics.

At minimum record/count:

```text
source_runtime_row_index
source_begin_rva / source_end_rva
source_instruction_rva
source_instruction_sha256 or exact bounded identity
edge_kind
edge_target_rva
target_metadata_relation
```

`edge_kind` must distinguish at least:

```text
ordinary_fallthrough
direct_unconditional_branch
direct_conditional_taken
conditional_fallthrough
call_fallthrough
other_supported_direct_successor
```

Use exact decoder semantics; do not infer edge kind from proximity.

### 3. Target relation must be deterministic and fail-closed

Classify the target at minimum as:

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

If a target is an exact unique runtime begin, report the target row index/begin/end and whether the source and target spans overlap. This is diagnostic only.

If useful, also report exact adjacency (`source_end == target_begin`) separately from admission. Adjacency is never proof of one function or safe fallthrough.

Nearest-entry distances may be diagnostic only and must never affect classification/admission.

### 4. Aggregate diagnostics

Emit compact exact totals including:

```text
repository_result
image_authenticated / image_sha256
boundary_refused_rows_expected = 162472
boundary_refused_rows_classified
boundary_edges_total
edge_kind_counts
target_metadata_relation_counts
exact_runtime_begin_edge_kind_counts
interior_target_edge_kind_counts
metadata_free_RX_edge_kind_counts
ambiguous_or_invalid_target_counts
bounded examples per major class
classification_complete
```

The aggregate must poison/refuse on missing/duplicate source rows, inconsistent row identity/order, stale bytes, wrong image/tool/schema/contract, malformed target relation, or any count mismatch against the accepted c56 boundary-refusal set.

Do not emit full proprietary function bodies. A bounded source instruction identity and small examples are sufficient.

### 5. No positive cross-row proof in c57

C57 must leave:

```text
candidate_sweep_complete = false
writer_enumeration_complete = false
cross_runtime_fact_propagation = false
```

Even if many exits target `exact_unique_runtime_begin`, c57 only establishes the measured distribution needed to design a later bounded stitching proof.

Do not treat:

- `ordinary_fallthrough` into an adjacent runtime row,
- direct branch into a runtime-row interior,
- metadata-free RX target,
- or overlapping metadata

as safe continuation without a separately reviewed contract in a later task.

### 6. Focused owned tests

Add a focused test, preferably:

```bash
python3 source/tools/test_slot_publication_boundary.py
```

Cover at minimum:

- ordinary fallthrough crossing row end;
- unconditional direct branch to exact next runtime begin;
- conditional taken edge to exact runtime begin with in-row fallthrough;
- conditional fallthrough crossing boundary;
- call fallthrough crossing boundary;
- target inside unique runtime span but not begin;
- target in overlapping runtime spans;
- metadata-free RX target;
- unbacked/nonexec/writable/outside-image target;
- deferred producer target;
- exact adjacency reported but never admitted;
- stale/wrong image/schema/tool/row identity refuses;
- duplicate/missing classification refuses aggregate;
- bounded examples do not affect counts;
- historical c38/c53/c54/c55/c56 tests remain unchanged/green;
- candidate counts and all completeness/publication gates remain unchanged.

Synthetic/model success is not real-image evidence.

## Repository result

Emit exactly one:

```text
RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED
```

`READY` means the classifier is deterministic, complete over the accepted c56 `unsupported_runtime_boundary` set, fail-closed and tested. It does not authorize cross-row propagation or prove writer completeness.

If the exact image is unavailable to Codex, publish one owner OFFLINE command that runs the c57 classifier against the exact pinned image and emits compact JSON. No game launch, Update-WS, Overstrike, hardware watch, EXE upload, or runtime log is needed.

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
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If the focused c57 test uses a different exact filename, run/report that exact command instead.

Expected:

- historical c38-c56 behavior remains green and immutable;
- the accepted c56 boundary-refusal set is classified exactly and deterministically;
- no cross-row fact propagation or candidate-semantic broadening occurs;
- no slot qword read, pointer chase, producer traversal, hardware watch, production integration or gameplay change occurs.

## Required outputs / handoff

Update as appropriate:

- `analysis/V01C57_*` evidence/gate/status;
- c57 tool/test files;
- `docs/PROJECT_STATE.md` so accepted c56 owner evidence is recorded and c57 is current;
- `docs/CODEX_RESULT.md`;
- relevant Wisdoms rule if the boundary classification yields a reusable static-analysis rule.

Commit and push the intended branch before claiming completion.

Final report must follow `docs/CODEX_REPORTING.md`: every executed command, expected/actual result, PASS/SKIP/UNAVAILABLE/BLOCKED, every unrun test with reason, branch+SHA, and exact owner action if one remains.
