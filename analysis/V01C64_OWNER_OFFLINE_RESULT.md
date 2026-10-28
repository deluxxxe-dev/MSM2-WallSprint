# V01C64 owner offline result — accepted VERIFIED-STATIC

2026-10-28 JST. Owner supplied `MSM2-WS-v0.1c64-post-c61-stitched.json` from the pinned private image after c64 publication.

## Classification

**VERIFIED-STATIC** / `POST_C61_RUNTIME_BEGIN_STITCH_SCANNER_READY`.

This is authenticated OFFLINE/STATIC continuation evidence only. It is not runtime/gameplay evidence and grants no writer, publication, ownership, lifetime, CFG/EH, watch, pointer, qword-read, or production permission.

## Artifact identity

```text
file_sha256 = 21b7bacf285e2ccb68fd2cf086dfe7209783282c1c54f07053597fb460ca680a
file_bytes = 11303
tool_version = v0.1c64
schema_version = 1
image_sha256 = c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
accepted_c63_result_identity = 559e4935c8c6af7c4974d7b16703f8177cb0636a538a602ddb56bafb8c397343
```

The accepted c57-c63 identities in the supplied artifact match the established chain, and `c56_through_c63_evidence_read_only = true`.

## Accepted c64 totals

```text
root_transactions_attempted = 162472
root_transactions_completed = 105083
root_transactions_refused = 57389

c63_boundary_instances_available = 2923
c63_boundary_instances_considered = 2923
c63_boundary_instances_admitted = 1150
c63_boundary_instances_refused = 1773

c63_exact_edges_available = 3049
c63_exact_edges_considered = 3049
c63_exact_edges_admitted = 1346
c63_exact_edges_refused = 1703

rows_reached_by_continuation = 1549
address_facts_crossing_runtime_rows = 2881
c63_certified_crossings_with_facts = 3
c63_address_facts_crossing_rows = 4
post_c63_uncertified_boundary_refusals = 998
new_cross_row_candidate_count = 0
reference_candidate_count = 0
writer_candidates = 0
```

Phase/reason admission remains provenance-specific. Of 2923 authenticated c63 instances, 1150 were admitted under all-successor fail-closed semantics. The 3049 exact value remains an edge count, not an instance count.

Compared with accepted c62, completed roots increase 103933 -> 105083 (+1150), reached rows 1317 -> 1549 (+232), and crossing address Facts 2635 -> 2881 (+246). The next bounded unknown wave is reduced from 2923 post-c61 refusals to exactly 998 `uncertified_post_c63_runtime_boundary` root refusals.

The accepted c63 preflight relation blockers remain visible (109 interior, 9 metadata-free RX); downstream historical refusal classes remain separate and must not be subtracted as a writer census.

## Safety interpretation

No new slot candidate was emitted. `candidate_sweep_complete = false` and `writer_enumeration_complete = false`; publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false. C52 `PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` remains binding.

Zero candidates is not evidence of writer absence. The c64 result proves only that the authenticated continuation mechanism safely advanced 1150 of the c63 boundary instances and bounded the next unknown diagnostic wave to 998 roots.

## Next bounded step

Create a separate c65 OFFLINE/STATIC diagnostic classifier for exactly the first terminal `uncertified_post_c63_runtime_boundary` instance in each of the 998 accepted c64 refused roots. Classify successors only; admit no edge, carry no new Fact, emit no candidate, and preserve complete c64 output/evidence read-only. Exact root/state/path/source and c63 phase/reason/provenance remain required.
