# V01C72 owner attempt 3 — full replay completed, final record-order check BLOCKED

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Owner observation

The third corrected c72 owner OFFLINE replay passed both prior blockers and completed the full historical root replay:

```text
c72 full-replay c71 continuation: 161792/162472
c72 full-replay c71 continuation: 162472/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: c72 live c71 record stream/order differs from accepted checkpoint
```

C72 is still **not accepted**. No c72 result artifact should be treated as READY evidence from this attempt.

## What this attempt proves

The final order check occurs only after c72 has already required all of the following:

- all `162472 / 162472` historical roots were replayed;
- the complete accepted c71 terminal domain was encountered (`considered == expected_ids`);
- every live c71 terminal record was authenticated byte-for-byte against the accepted checkpoint record for its `original_root_runtime_row`;
- duplicate live c71 roots remained fail-closed;
- the c71 all-successor exact preflight record domain matched the accepted `95` records;
- the continuable exact edge domain matched the accepted `99` edges;
- c71 mismatch counters remained empty.

Therefore this blocker does **not** indicate a changed c71 record body, missing root, duplicate root, or changed `102 / 95 / 99` domain. Given the preceding guards, the failing list comparison can only represent a permutation of the same unique exact per-root records.

This full owner attempt also validates both earlier c72 corrections against the complete replay path:

1. the retired first-c71-terminal observer correction passed the historical `30720` blocker;
2. the c71-only / c69 proof-layer correction passed the historical `31744` blocker;
3. no later traversal blocker occurred before `162472 / 162472`.

## Diagnosis

Accepted c71 binds an order-sensitive serialized record stream in its checkpoint/result. C72 separately replays the same roots and appends authenticated records in live discovery order.

The original c72 finalization incorrectly treated these two notions as identical:

```text
accepted checkpoint serialization order
==
live c72 replay discovery order
```

That equality is stronger than the semantic proof needed for continuation. Each c71 record is already uniquely keyed by `original_root_runtime_row` and authenticated exactly before it is appended. The full domain and duplicate-root guards are separately enforced.

The correct rule is:

1. authenticate every live record exactly against the accepted record for the same original root;
2. require the complete unique accepted domain;
3. preserve live arrival order only as diagnostic metadata;
4. for reproduction of c71's order-sensitive record-stream/result bytes, reorder the already-authenticated live records into the accepted checkpoint root order;
5. do not weaken any record, root, edge, provenance, or duplicate guard.

## Correction prepared

`source/tools/slot_publication_post_c71_stitched_fix.py` now adds a bounded checkpoint-order adapter for `ProofPool.live_records`.

The adapter:

- stores each authenticated record by `original_root_runtime_row`;
- separately records live arrival root order;
- rejects duplicate roots;
- iterates records in the accepted checkpoint root order for the existing final list/result reproduction checks;
- emits c72 diagnostic fields for whether live discovery order matched the checkpoint and how many positions differed.

No accepted c71 record content is changed or synthesized.

Focused regression coverage was extended in:

`source/tools/test_slot_publication_post_c71_stitched_fix_quick.py`

with an intentionally permuted live arrival order that must normalize to the accepted order while duplicate roots still refuse.

## Safety

No slot qword read, pointer chase, native/game probe, gameplay write, production integration, c69 proof relaxation, c71 relabeling, or domain widening was added.

Accepted c71 remains:

```text
records = 102
all-successor-exact records = 95
continuable exact edges = 99
non-exact records = 7
```

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.

## Next

Run the focused c72 fix regression first. Do **not** immediately rerun the full owner scan.

If all five focused checks PASS, record the correction as focused validated and authorize exactly one further c72 owner replay. If the focused test fails, stop and analyze that exact failure.
