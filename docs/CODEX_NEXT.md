# Next task — c67 first terminal post-c65 boundary diagnostic

## Status

Accepted owner c66 result is **VERIFIED-STATIC**.

Canonical result:

- `analysis/V01C66_OWNER_OFFLINE_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

Accepted c66 key totals:

```text
repository_result = POST_C63_RUNTIME_BEGIN_STITCH_SCANNER_READY
root attempts = 162472
completed/refused = 105541 / 56931
c65 instances available/considered/admitted/refused = 998 / 998 / 458 / 540
c65 exact edges available/considered/admitted/refused = 1021 / 1021 / 513 / 508
rows reached = 1683
address Facts crossing rows = 3044
c65 useful crossings/Facts = 8 / 11
post_c65_uncertified_boundary_refusals = 458
new candidates = 0
```

The 1021 exact value is an edge count, not instance/root permission. C66 admitted c65 records only for complete successful root transactions. Zero candidates remains incomplete evidence.

## Current bounded task

C67 is **OFFLINE/STATIC boundary classification only**.

Build a separate c67 diagnostic that reconstructs/authenticates the complete accepted c56→c66 chain and classifies only the first original terminal `uncertified_post_c65_runtime_boundary` for the exact 458 refused roots exposed by accepted c66.

Preferred tool/test:

```text
source/tools/slot_publication_post_c65_boundary.py
source/tools/test_slot_publication_post_c65_boundary.py
```

Preferred repository result:

```text
POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
```

Use `v0.1c67` / schema 1. Historical c38-c66 tools/results remain read-only except normal test-registration/docs integration explicitly required by this task.

### 1. Authenticate the accepted c56→c66 chain

Require the exact pinned image and replay/validate:

- c56 checkpoint directory;
- accepted c57 JSON;
- accepted c58 JSON;
- accepted c59 JSON;
- accepted c60 JSON;
- accepted c61 JSON;
- accepted c62 JSON;
- accepted c63 JSON;
- accepted c64 JSON;
- accepted c65 JSON;
- accepted c66 JSON.

The c66 compact JSON is not a row-global certificate pool. Missing/stale/corrupt/wrong-image/wrong-tool/wrong-schema/wrong-contract/wrong-count/wrong-chain evidence must refuse.

Authenticate the c66 owner artifact including:

```text
physical SHA256 = eb4e44fddb67dafd64700e19e846173e60cbaddce95860686e24e543e650cc1c
accepted c65 result identity = dd4ff3451faa9735f88756e16222a36fc80cafbf51f113bc12f6f06b6dc62780
post_c65_uncertified_boundary_refusals = 458
candidate counts = 0
all safety gates false
```

Do not treat physical-file SHA as a substitute for semantic replay; both identities and full accounting must match.

### 2. Exact diagnostic domain

The complete c67 domain is the exact 458 original roots whose accepted c66 root transaction ends with:

```text
uncertified_post_c65_runtime_boundary
```

Capture only the first actual owned terminal row-external boundary for each such root. Preserve exact:

- original root;
- current runtime row / entry instance;
- incoming abstract state;
- path and transition identities;
- source instruction bytes/hash and current row identity;
- current c66 continuation/full-prefix identity;
- carried c65 proof identity and c65 terminal phase/reason/provenance;
- any actual post-c65 arrival/origin information;
- underlying refusal/cause when the terminal is a conversion of an earlier refusal;
- accepted c56→c66 chain identities.

Do not infer reachability from exception context or stale cached observer state. Source-local preflight is not a completed crossing. Dedup-stored earlier path is not current arrival provenance. Converted earlier refusal is not natural post-c65 reachability and may not borrow earlier c63/c65 authorization.

If a terminal refusal cannot be tied to an exact unique source edge/instruction in the current root context, fail the aggregate rather than inventing a boundary record.

### 3. Successor classification only

For every captured terminal instruction, classify all supported row-external successor callbacks with the unchanged c57 target-relation vocabulary:

- `exact_unique_runtime_begin`
- `inside_unique_runtime_span_nonentry`
- `overlapping_multiple_runtime_spans`
- `executable_file_backed_outside_runtime_metadata`
- `executable_not_fully_file_backed`
- `non_executable_or_writable_section`
- `outside_image`
- `deferred_producer_excluded`

Preserve architectural vs model-only distinction and edge kind.

C67 must **not**:

- traverse a classified target;
- admit a new edge;
- propagate any new Fact;
- create any candidate;
- infer a function end or nearest metadata entry;
- turn an interior/free-RX target into an entry;
- dynamically issue a c65/c66/c67 continuation certificate.

### 4. Complete-domain replay and identities

Require exactly 458 classified roots/instances or block. Validate ordered record completeness, uniqueness and deterministic identity. Missing, duplicate, out-of-order, stale or mismatching records must fail closed.

Publish deterministic classifier/header/ordered-record-set identities so a later continuation task can authenticate exact c67 records without treating examples as a certificate pool.

Report at minimum:

- expected/classified roots;
- unique entry-instance states and source rows;
- boundary instance count;
- architectural/model-only edge counts;
- target relation counts;
- edge kinds;
- exact-target adjacency diagnostic only;
- terminal phase/reason/provenance counts based on what is actually observed;
- zero new admission/Fact/candidate counts;
- complete-domain flag;
- all safety gates false.

Do not assume c67 phase counts in advance. Preserve actual provenance distinctions found by replay.

### 5. Preserve safety and historical semantics

All accepted c56-c66 analysis semantics remain unchanged. C52

```text
PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL
```

remains binding.

Do not add memory-loaded values/pointers, slot qword reads, pointer chase, stack/local/return facts, ABI assumptions, EH inference, mitigation changes, producer traversal, gameplay/production writes, native probes, game use, PAGE_GUARD or hardware watch.

`candidate_sweep_complete`, `writer_enumeration_complete`, publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false. Zero candidates never proves writer absence.

## Required tests

Focused command:

```bash
python3 source/tools/test_slot_publication_post_c65_boundary.py
```

Cover at minimum:

- exact first post-c65 terminal capture;
- ordinary/direct/conditional/CALL-fallthrough edge vocabulary;
- multi-successor architectural classification without target traversal;
- interior and metadata-free RX relation preservation;
- exact root/state/path/source/c66-prefix/c65-provenance binding;
- stale/mismatched phase/reason/origin/provenance refusal;
- dedup stored-path vs actual post-c65 arrival separation;
- converted underlying refusal provenance if present in owned fixtures;
- missing/duplicate/out-of-order record failure;
- invalidation-only terminal without exact source edge blocks;
- c66 candidate bytes/counts unchanged;
- zero new edge/Fact/candidate admission and all safety gates false.

Then run the existing slot-publication regression suite through c67 and the same build/test/smoke/hooks/package validation used for c66. Register the focused c67 test in the mandatory hooks suite without removing any historical test.

## Documentation

Update:

- `docs/PROJECT_STATE.md` with c67 TOOL READY after implementation validation;
- `docs/CODEX_RESULT.md` with exact commands/results, unrun reasons and publication state;
- a concise `analysis/V01C67_TOOL_READY.md`.

Do not alter accepted c66 owner evidence.

## Owner handoff after c67 TOOL READY

After implementation is pushed and verified, provide one owner OFFLINE command only. Reuse c56 checkpoints plus accepted c57-c66 JSONs read-only and output:

```text
logs/MSM2-WS-v0.1c67-post-c65-runtime-boundary.json
```

No Update-WS, game, Overstrike, runtime capture, native probe, evidence editing or regeneration unless an exact input is separately proven missing/stale.
