# V01C70 corrected owner OFFLINE result

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Status

The corrected c70 owner OFFLINE run completed and is accepted as **VERIFIED-STATIC**.

This is not runtime evidence. No game/native probe or gameplay write was used.

The earlier blocked owner attempt remains documented in `analysis/V01C70_OWNER_BLOCKED_RESULT.md`; no artifact from that failed run is accepted.

## Accepted artifact

```text
file: MSM2-WS-v0.1c70-post-c69-stitched.json
physical SHA256: 2780ca89a98ad22d12e2252f53cd202830bc8228edd67d0b0c7fb8eccf7f15ac
size: 15044 bytes
semantic result identity: 9b213db28b05f9d1fc8baefa37f981aa521cd7ee45110a7cfe6b122567f0b9e1
repository_result: POST_C69_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool: v0.1c70 / schema 1
contract: exact_c69_full_replay_all_successor_continuation_v1
```

Exact executable remains authenticated:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

The owner log completed the full c70 replay through `162472/162472` and ended with `POST_C69_RUNTIME_BEGIN_STITCH_SCANNER_READY`.

## Accepted c69 evidence binding

The c70 artifact pins the accepted c69 result/checkpoint exactly:

```text
accepted_c69_result_identity = 08002e4c3aef2c7cfa037040b6f09f7d1b6e247d0b446daea1ffdb2569aa01fb
accepted_c69_physical_sha256 = e57b7f39ea476e356397f944d8aadb14841b27874736a60c1a82b982eb86537f
accepted_c69_classifier_identity = 4f42b0d04346eb5c7b10c8fe0592edd3a3f19fcb463d66a0adf40be01fd25049
accepted_c69_record_set_identity = 20a123c1e83dd6e777b47cb07e86a6a7203acb25e0cede2e86ff956efcfae8ed
accepted_c69_checkpoint_identity = b734f8ab2f0a24e002c8b5c2ed1dbbfe75c9411cc773ec1394a1cf330c9b3a28
accepted_c69_checkpoint_physical_sha256 = d7b8dae21e9e0478e8ea5d8136c7f19156cd5afca2ff80f167dce48a2e61b6b5
accepted_c69_checkpoint_record_stream = 20a123c1e83dd6e777b47cb07e86a6a7203acb25e0cede2e86ff956efcfae8ed
```

C70 explicitly reports:

```text
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

The c69 checkpoint is therefore used as exact evidence/authentication, not as a mutable transaction resume snapshot.

## C69 continuation domain measured by c70

```text
c69 boundary instances available = 191
c69 boundary instances considered = 191
all-successor-exact available = 177
preflight eligible = 177
boundary instances admitted = 70
boundary instances refused = 121

continuable exact edges available = 191
continuable exact edges considered = 191
continuable exact edges preflight eligible = 191
continuable exact edges admitted = 84
continuable exact edges refused = 107
```

Preflight refusal classes remain exactly the accepted c69 non-continuable domain:

```text
inside_unique_runtime_span_nonentry = 13
transaction_terminal = 1
```

The transaction-only terminal remains refused without synthetic instruction/edge.

`c69_record_mismatch_counts` is empty.

The difference between 177 preflight-eligible records and 70 committed admissions is expected fail-closed transaction behavior: eligibility is provisional until the whole root transaction completes. C70 does not retain provisional c69 admission after any later refusal.

## New c70 boundary

C70 stopped at the next unknown post-c69 boundary exactly as intended:

```text
uncertified_post_c69_runtime_boundary = 102 roots
```

This count is the next bounded diagnostic domain. It does not imply that all 107 preflight-eligible-but-uncommitted c69 roots failed for this reason; five were refused by other existing fail-closed conditions after c69 preflight.

C71 must therefore classify only the exact 102 `uncertified_post_c69_runtime_boundary` terminal events, not infer a 107-record boundary domain.

## Replay deltas versus accepted c68

```text
root transactions attempted: 162472 -> 162472
root transactions completed: 105721 -> 105791  (+70)
root transactions refused:   56751  -> 56681   (-70)
rows reached:                 1750   -> 1797    (+47)
address facts crossing rows:  3161   -> 3223    (+62)
```

C70-specific authenticated crossings with facts:

```text
c69_certified_crossings_with_facts = 6
c69_address_facts_crossing_rows = 6
```

Continuation accounting:

```text
continuation_instances_created = 9208
continuation_instances_deduplicated = 115
continuation_instances_refused = 6477
cross_row_cap_refusals = 46
cross_row_cycles_seen = 0
```

## Candidate result

```text
reference_candidate_count = 0
new_cross_row_candidate_count = 0
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
reference_candidates = []
```

Historical c69/c68 candidate totals remain zero.

Zero candidates does **not** prove writer absence.

## Safety

```text
c56_through_c69_evidence_read_only = true
live_qword_reads = 0
pointer_chase = false
game_used = false
production_integrated = false
```

All completeness/publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain false.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

No slot qword read, pointer chase, deferred-producer assumption, native/game probe, gameplay write, production integration, or SocialFeed/other-MOD modification was added.

## Next bounded task

C71 should be diagnostic-only over exactly the 102 accepted c70 `uncertified_post_c69_runtime_boundary` roots.

It should reproduce the exact c70 terminal event under unchanged full historical replay, bind current c69 authorization/provenance and c70 full-prefix state, classify the architectural successor relation using the existing relation vocabulary, and rethrow/refuse without traversal, admission, new Fact, or candidate.

Do not define c72 continuation behavior until the c71 owner classification is analyzed.
