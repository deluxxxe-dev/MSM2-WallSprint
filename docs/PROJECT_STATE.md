# WallSprint project state

## Current stage — corrected c70 owner OFFLINE result accepted; c71 post-c69 boundary diagnostic next

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66, corrected c67, c68, corrected c69, and corrected c70 owner OFFLINE results are **VERIFIED-STATIC**.

Canonical detail:

- `analysis/V01C66_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C67_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C68_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C69_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C70_CHECKPOINT_SUFFICIENCY_BLOCKED.md`
- `analysis/V01C70_TOOL_READY.md`
- `analysis/V01C70_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C70_OWNER_OFFLINE_RESULT.md`

## Accepted c70 artifact

```text
file: MSM2-WS-v0.1c70-post-c69-stitched.json
physical SHA256: 2780ca89a98ad22d12e2252f53cd202830bc8228edd67d0b0c7fb8eccf7f15ac
size: 15044 bytes
semantic result identity: 9b213db28b05f9d1fc8baefa37f981aa521cd7ee45110a7cfe6b122567f0b9e1
repository_result: POST_C69_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool: v0.1c70 / schema 1
contract: exact_c69_full_replay_all_successor_continuation_v1
```

Exact image remains authenticated:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

C70 used a full historical replay:

```text
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

The accepted c69 result/checkpoint identities and record stream are pinned exactly in the c70 authentication header.

## C70 measured continuation

```text
c69 boundary instances available/considered = 191 / 191
all-successor-exact available/preflight eligible = 177 / 177
c69 boundary instances admitted = 70
c69 boundary instances refused = 121

continuable exact edges available/considered/preflight = 191 / 191 / 191
continuable exact edges admitted = 84
continuable exact edges refused = 107
```

Accepted non-continuable c69 preflight domain remains:

```text
inside_unique_runtime_span_nonentry = 13
transaction_terminal = 1
```

`c69_record_mismatch_counts = {}`.

Admission is root-transaction commit, not preflight permission. Therefore 177 preflight-eligible records need not all be admitted; any later refusal rolls back provisional admission.

## Next unknown domain

C70 measured:

```text
uncertified_post_c69_runtime_boundary = 102 roots
```

This is the exact next diagnostic domain.

Do not infer 107 next-boundary records from the 107 refused exact edges: five preflight-eligible roots failed through other existing fail-closed conditions after c69 preflight.

C71 must classify exactly the 102 c70 post-c69 boundary events and must not traverse them.

## Replay deltas versus accepted c68

```text
root transactions completed: 105721 -> 105791 (+70)
root transactions refused:   56751  -> 56681  (-70)
rows reached:                 1750   -> 1797   (+47)
address facts crossing rows:  3161   -> 3223   (+62)
```

C70-specific fact-carrying crossings:

```text
c69_certified_crossings_with_facts = 6
c69_address_facts_crossing_rows = 6
```

## Candidates and safety

C70 emitted no candidates:

```text
reference_candidate_count = 0
new_cross_row_candidate_count = 0
READ/WRITE/RMW/ADDRESS_ONLY = 0/0/0/0
```

Zero candidates does not prove writer absence.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

The following remain false/unproven:

```text
candidate_sweep_complete
writer_enumeration_complete
allocation_proven
base_proven
range_proven
lifetime_proven
ownership_exclusive
destination_contained
CFG_continuation
alternate_entries_EH_indirect_writers_closed
production_integrated
hardware_watch_enabled
pointer_chase
game_used
```

`live_qword_reads = 0` and c56-through-c69 evidence remained read-only.

Runtime remains `v0.1c38`; c53-c71 work is OFFLINE/STATIC unless separately stated. Do not modify SocialFeed or other MOD repositories.

## Current bounded action

Implement c71 as a diagnostic-only classifier for exactly the 102 accepted c70 `uncertified_post_c69_runtime_boundary` terminal events.

C71 may reproduce and classify the terminal source/successor relationship under unchanged full historical replay. It must not traverse the c71 target, admit a new continuation, create a new Fact/candidate, read the slot qword, chase pointers, use the game, or widen any safety gate.

Do not define c72 continuation behavior until the c71 owner result is analyzed.
