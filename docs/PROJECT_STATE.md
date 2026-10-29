# WallSprint project state

## Current stage — c72 accepted VERIFIED-STATIC; direct-CFG continuation paused; vanilla WallRun steering config investigation next

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66, corrected c67, c68, corrected c69, corrected c70, corrected c71, and corrected c72 owner OFFLINE results are **VERIFIED-STATIC**.

Canonical recent detail:

- `analysis/V01C69_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C70_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_PREPARED.md`
- `analysis/V01C72_TOOL_READY.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C72_LIFECYCLE_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_2.md`
- `analysis/V01C72_PROVENANCE_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_3.md`
- `analysis/V01C72_ORDER_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_4.md`
- `analysis/V01C72_REPRESENTATION_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_OFFLINE_RESULT.md`
- `analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md`

## Accepted c72 owner artifact

Owner replay completed:

```text
c72 full-replay c71 continuation: 162472/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
C72_REPRFIX_OWNER_PASS
```

Artifact:

```text
MSM2-WS-v0.1c72-post-c71-stitched.json
size = 17148 bytes
SHA256 = 077955562f1cdd9ba966fd1104ecd2b3b258f3aa25384418032fa5099149d32d
repository_result = POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool_version = v0.1c72
schema_version = 1
contract = exact_c71_full_replay_all_successor_continuation_v1
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

Accepted c71 reproduction checks remained exact:

```text
records available/considered = 102/102
all-successor-exact available/preflight = 95/95
exact edges available/considered/preflight = 99/99/99
c71_record_mismatch_counts = {}
```

Final order/representation diagnostics:

```text
c71_live_discovery_order_matches_checkpoint = true
c71_live_discovery_order_mismatch_positions = 0
c71_checkpoint_order_normalized_for_result_reproduction = true
c71_final_stream_comparison_uses_canonical_encoding = true
```

## C72 continuation outcome

```text
c71 boundary instances admitted/refused = 38/64
c71 exact edges admitted/refused = 42/57
post_c71_uncertified_boundary_refusals = 54
root_transactions_attempted/completed/refused = 162472/105829/56643
rows_reached_by_continuation = 1815
address_facts_crossing_runtime_rows = 3251
```

No candidate was produced:

```text
reference_candidate_count = 0
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
new_cross_row_candidate_count = 0
publication_source_kind = unproven
```

Zero candidates does not prove writer absence. It means the bounded c72 direct-CFG continuation did not produce a usable candidate.

Notable full-replay refusal counts include:

```text
indirect_call = 5569
indirect_jump = 258
uncertified_post_c71_runtime_boundary = 54
inside_unique_runtime_span_nonentry = 31932
executable_file_backed_outside_runtime_metadata = 18718
```

These are refusal-event/root counts, not unique function counts or writer evidence.

## Strategy decision after c72

Do **not** automatically create c73 as another direct-CFG boundary-continuation cycle.

The c71 frontier narrowed from 95 eligible records to 54 next post-c71 unknown boundary observations, but all candidate classes remain zero after a long direct-CFG sequence. The replay also retains material indirect-transfer refusal evidence. Therefore the cost/value of continuing the same direct route is now poor enough to pause it.

The next investigation is the independently motivated current-build vanilla WallRun config route established by `analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md`.

Priority assets:

```text
configs/hero/hero_WallRunSteeringSetup_Standard.config
  asset id 948FA063808F7E18
configs/hero/hero_WallRunSteeringSetup_Indoor.config
  asset id 83DF4507531061B1
configs/hero/hero_WallRunConfig.config
  asset id 86DF98B17F4D766D
configs/hero/hero_WallRunSpeedSetup_Standard.config
  asset id 80D25C57AC09BE31
configs/hero/hero_WallRunJumpSetup_Standard.config
  asset id B78A8FA3857CFB17
```

First inspect/decode the current-build vanilla Standard/Indoor steering configs without editing them. If config control is insufficient, compare the higher-level motion/gravity/orientation route suggested by the Scripted Flight reference artifact. Reference MOD offsets/assets remain hints only and are never production authority.

Reopen direct-CFG c73 only if a concrete later question specifically requires the remaining 54 frontier observations.

## Safety

Runtime remains `v0.1c38`; c53-c72 work is OFFLINE/STATIC.

```text
live_qword_reads = 0
game_used = false
hardware_watch_enabled = false
CFG_continuation = false
candidate_sweep_complete = false
writer_enumeration_complete = false
```

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed. Do not modify SocialFeed or other MOD repositories.

## Current bounded action

Follow `docs/CODEX_NEXT.md`: obtain/authenticate and decode the current-build vanilla WallRun steering assets before opening any c73 direct-CFG continuation.
