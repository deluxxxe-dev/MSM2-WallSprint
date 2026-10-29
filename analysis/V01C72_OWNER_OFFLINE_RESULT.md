# V0.1c72 owner OFFLINE result — accepted VERIFIED-STATIC

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Result

The corrected c72 owner full replay completed and emitted:

```text
c72 full-replay c71 continuation: 162472/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
C72_REPRFIX_OWNER_PASS
```

Generated artifact:

```text
MSM2-WS-v0.1c72-post-c71-stitched.json
size = 17148 bytes
SHA256 = 077955562f1cdd9ba966fd1104ecd2b3b258f3aa25384418032fa5099149d32d
```

This c72 result is accepted as **VERIFIED-STATIC**.

## Contract/authentication checks

Observed artifact fields:

```text
repository_result = POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool_version = v0.1c72
schema_version = 1
contract = exact_c71_full_replay_all_successor_continuation_v1
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

Accepted c71 reproduction domain remained exact:

```text
c71 boundary instances available/considered = 102/102
c71 all-successor-exact available/preflight = 95/95
c71 continuable exact edges available/considered/preflight = 99/99/99
c71_record_mismatch_counts = {}
```

Final representation/order correction diagnostics:

```text
c71_live_discovery_order_matches_checkpoint = true
c71_live_discovery_order_mismatch_positions = 0
c71_checkpoint_order_normalized_for_result_reproduction = true
c71_final_stream_comparison_uses_canonical_encoding = true
```

## Continuation outcome

```text
c71 boundary instances admitted = 38
c71 boundary instances refused = 64
c71 exact edges admitted = 42
c71 exact edges refused = 57
post_c71_uncertified_boundary_refusals = 54
```

The previous c71 direct frontier therefore did **not** close to zero. The next direct-CFG unknown boundary frontier is 54 observations. Treat `95 -> 54` as frontier narrowing, not as project completion percentage and not as a one-to-one identity mapping between records.

Historical root accounting:

```text
root_transactions_attempted = 162472
root_transactions_completed = 105829
root_transactions_refused = 56643
rows_reached_by_continuation = 1815
address_facts_crossing_runtime_rows = 3251
```

Notable refusal classes across the full historical root replay include:

```text
indirect_call = 5569
indirect_jump = 258
uncertified_post_c71_runtime_boundary = 54
inside_unique_runtime_span_nonentry = 31932
executable_file_backed_outside_runtime_metadata = 18718
```

These are refusal-event/root counts, not unique function counts and not proof that a writer lies behind any one class. They do establish that indirect transfers remain a material unresolved class outside the current direct-CFG continuation model.

## Candidate result

No publication/writer candidate was found:

```text
reference_candidate_count = 0
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
new_cross_row_candidate_count = 0
publication_source_kind = unproven
```

Zero candidates does **not** prove writer absence from the game. It means no usable candidate was produced by the bounded c72 direct-CFG continuation model.

## Safety

```text
live_qword_reads = 0
game_used = false
hardware_watch_enabled = false
CFG_continuation = false
candidate_sweep_complete = false
writer_enumeration_complete = false
```

Allocation/base/range/lifetime/ownership/destination/production gates remain false. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

No qword read, pointer chase, game/native probe or gameplay write was authorized.

## Strategy decision after c72

Do **not** automatically create c73 as another copy of the same direct-CFG boundary-continuation cycle.

Reasons:

1. The direct frontier narrowed from the c71 eligible 95 records to 54 next unknown boundary observations, but remained nonzero.
2. All candidate classes remain zero after a long sequence of direct-CFG continuation stages.
3. The full replay still contains substantial indirect-transfer refusal evidence (`indirect_call=5569`, `indirect_jump=258`), so the current direct model is not close to an all-route writer enumeration.
4. The reference-MOD review independently established a dedicated WallRun steering config family and a higher-level motion/gravity/orientation route worth testing before paying for another direct-CFG extension.

The next investigation should therefore prioritize the **current-build vanilla WallRun config route** before any c73 direct-CFG continuation:

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

Priority order:

1. obtain/extract the authenticated current-build vanilla `Standard` and `Indoor` steering configs;
2. decode and inventory their fields without editing them;
3. compare speed/jump configs against the already reviewed reference MODs where useful;
4. decide whether one minimal asset-only steering experiment can test wall-relative directional control;
5. only if config control is insufficient, investigate the higher-level current-build motion/gravity/orientation route using reference MOD labels solely as search hints;
6. do not reopen direct-CFG c73 unless a concrete remaining question specifically requires it.

No third-party binary/asset/offset is production authority. Current-build vanilla evidence must be independently authenticated.