# V01C72 owner OFFLINE attempt 4 — final stream guard still BLOCKED after order normalization

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Status

C72 remains **BLOCKED** and is not accepted owner evidence.

After the five-check checkpoint-order focused regression passed, the owner ran one authorized corrected full c72 replay. It again ended at:

```text
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: c72 live c71 record stream/order differs from accepted checkpoint
c72 orderfix owner offline continuation failed
```

Do not rerun unchanged.

## What this does and does not mean

The failing guard is after the complete historical root loop and after c72 has already required:

- the complete accepted c71 terminal domain;
- exact per-root live authentication against the accepted checkpoint record;
- duplicate-root exclusion;
- the accepted 95-record all-successor-exact preflight domain;
- the accepted 99-edge continuation domain;
- empty c71 mismatch counters.

Therefore this result does **not** show a changed c71 root record, lost root, widened domain, or new traversal failure.

Attempt 4 does show that checkpoint-order normalization alone was insufficient. The earlier description of the blocker as purely an order permutation was too narrow.

## Static diagnosis

Base `ProofPool.authenticate_live()` authenticates each live record with:

```text
c56.encoded(live_record) == c56.encoded(accepted_record)
```

`c56.encoded()` uses canonical JSON serialization (`json.dumps(..., sort_keys=True, separators=(',', ':'))`). Python tuples and lists therefore serialize to the same JSON array representation.

After attempt 3, the fix adapter changed iteration to accepted checkpoint root order, but its final equality implementation still compared raw Python objects:

```text
list(live_records) == list(accepted_records)
```

A live record can consequently pass the binding canonical per-root authentication and still fail raw Python equality when an in-memory tuple corresponds to a list loaded from checkpoint JSON. This is a representation difference, not a semantic record difference under the already-accepted record identity relation.

## Minimal correction prepared

`source/tools/slot_publication_post_c71_stitched_fix.py` now changes only the adapter's final equality relation:

- keep accepted checkpoint root order;
- compare each record using the same `c56.encoded()` canonical bytes already used by per-root authentication;
- retain duplicate-root fail-closed behavior;
- retain first-terminal observer retirement;
- retain c71-only/c69 proof-layer separation;
- retain accepted c71 contents and `102 / 95 / 99` domain unchanged.

No traversal, admission, candidate, value-read, pointer, native/game or gameplay semantics are widened.

## Focused regression prepared

The quick regression now contains a sixth case where:

- accepted checkpoint-form data uses JSON-like lists;
- live in-memory data uses equivalent tuples;
- raw Python equality is intentionally false;
- `c56.encoded()` identity is equal;
- the adapter must accept the canonical identity relation.

Expected new line:

```text
PASS: c72 final stream comparison uses canonical encoded record identity rather than raw Python list/tuple representation
```

This sixth check is prepared but is **not yet owner-observed**.

## Safety

Accepted c71 production domain remains:

```text
records = 102
all-successor-exact = 95
continuable exact edges = 99
non-exact = 7
```

No slot qword read, pointer chase, game/native probe, gameplay write or production integration was added.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.

## Next

Run only `source/tools/test_slot_publication_post_c71_stitched_fix_quick.py` and require all six PASS lines. Do not authorize another full owner replay until the sixth check is owner-observed PASS.
