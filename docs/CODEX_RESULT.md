# Codex result — c72 owner OFFLINE READY and accepted VERIFIED-STATIC

2026-10-29 JST. Branch **codex/v01c20-edi-value-provenance**.

## STATUS

success

Corrected c72 owner OFFLINE full replay completed and is accepted as **VERIFIED-STATIC**.

## RESULT

Owner observed:

```text
c72 full-replay c71 continuation: 162472/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
C72_REPRFIX_OWNER_PASS
```

Artifact:

```text
MSM2-WS-v0.1c72-post-c71-stitched.json
size 17148 bytes
SHA256 077955562f1cdd9ba966fd1104ecd2b3b258f3aa25384418032fa5099149d32d
```

Contract/authentication:

```text
repository_result = POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool_version = v0.1c72
schema_version = 1
contract = exact_c71_full_replay_all_successor_continuation_v1
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
c71 records available/considered = 102/102
all-successor-exact available/preflight = 95/95
exact edges available/considered/preflight = 99/99/99
c71_record_mismatch_counts = {}
```

Continuation result:

```text
c71 boundary instances admitted/refused = 38/64
c71 exact edges admitted/refused = 42/57
post_c71_uncertified_boundary_refusals = 54
roots attempted/completed/refused = 162472/105829/56643
rows reached = 1815
address facts crossing rows = 3251
```

Candidate result:

```text
reference_candidate_count = 0
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
new_cross_row_candidate_count = 0
publication_source_kind = unproven
```

No writer absence inference is authorized from the zero-candidate result.

Notable refusal-event/root counts include:

```text
indirect_call = 5569
indirect_jump = 258
uncertified_post_c71_runtime_boundary = 54
```

These counts are not unique function counts or writer evidence; they show unresolved route classes remain outside the current direct-CFG model.

## SAFETY

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

No qword read, pointer chase, game/native probe, gameplay write or production integration was performed.

## CHANGED

- `analysis/V01C72_OWNER_OFFLINE_RESULT.md`
- `docs/PROJECT_STATE.md`
- `docs/CODEX_NEXT.md`
- `docs/CODEX_RESULT.md`

## NEXT

Do **not** open c73 as another automatic direct-CFG continuation.

The current bounded task is to obtain/authenticate and decode the current-build vanilla WallRun steering configs, starting with:

```text
configs/hero/hero_WallRunSteeringSetup_Standard.config
asset id 948FA063808F7E18

configs/hero/hero_WallRunSteeringSetup_Indoor.config
asset id 83DF4507531061B1
```

If those assets are unavailable in the execution environment, stop and request exactly those owner-supplied vanilla files. Do not substitute reference-MOD assets.
