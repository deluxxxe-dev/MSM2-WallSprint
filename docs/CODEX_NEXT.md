# Next task — run c72 owner OFFLINE continuation once

## Status

Corrected c71 owner OFFLINE result is accepted as **VERIFIED-STATIC**.

C72 implementation and focused validation are **TOOL READY**.

Canonical detail:

- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_PREPARED.md`
- `analysis/V01C72_TOOL_READY.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

## Accepted c71 basis

```text
MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
physical SHA256 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
checkpoint identity 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
record stream SHA256 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
record count 102
```

Embedded c71 result:

```text
repository_result POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool v0.1c71 / schema 1
contract exact_c70_first_terminal_post_c69_boundary_full_prefix_provenance_diagnostic_v1
semantic result identity 18d61be59032514a4c74c9b015f422c379e2c05050e2d5819c1025048a2276d7
classifier identity 18cba768c4261886b70496e7f73abe3fe5276b44aecadb0d929802ae3dae9fd5
```

Exact executable remains SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`.

## Exact c72 domain

```text
c71 terminal records = 102
all-successor-exact records = 95
continuable exact architectural edges = 99
inside_unique_runtime_span_nonentry records = 6
executable_file_backed_outside_runtime_metadata records = 1
```

Only the 95 all-successor-exact records / 99 exact edges may be provisionally continued.

The seven non-exact records remain wholly fail-closed. No partial arm admission is allowed.

## Focused validation accepted

The owner ran `source/tools/test_slot_publication_post_c71_stitched.py` and all prerequisite c68/c70/c71 suites plus all c72 groups passed.

Required c72 lines observed:

```text
PASS: c72 production contract pins accepted c71 identities and 95/99 continuation domain
PASS: c72 exact c71 terminal continues only after exact live record/all-successor authentication
PASS: c72 later distinct edge stops as uncertified_post_c71_runtime_boundary and rolls back provisional admission
PASS: c72 interior c71 terminal remains diagnostic/refused and is never promoted to an entry
PASS: c72 rejects mutated c71 checkpoint record evidence
PASS: c72 focused full-replay c71 continuation groups complete
```

## Current task

Run exactly one c72 owner OFFLINE scan against the exact accepted c56-c71 evidence.

Tool:

```text
source/tools/slot_publication_post_c71_stitched.py
```

Expected success identity:

```text
repository_result POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool v0.1c72 / schema 1
contract exact_c71_full_replay_all_successor_continuation_v1
```

The run must use full historical replay:

```text
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

It must authenticate and reproduce the exact c71 record stream before continuation and preserve root-transactional rollback.

## Required owner-result checks

After the owner artifact is returned, verify before defining c73:

- c71 records available/considered = 102/102;
- all-successor-exact available/preflight = 95/95;
- exact edges available/considered/preflight = 99/99/99;
- c71 record mismatch counts are empty;
- admitted/refused c71 instance and edge counts;
- next `uncertified_post_c71_runtime_boundary` count;
- root completed/refused counts;
- rows reached and address Facts crossing rows;
- READ/WRITE/RMW/ADDRESS_ONLY candidate counts;
- `live_qword_reads = 0`;
- all safety gates remain false.

Zero candidates does not prove writer absence.

## If blocked

If the owner run emits `POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED`, stop. Record the exact observed blocker and analyze it before changing the classifier or rerunning.

Do not widen semantics by assumption.

## Do not

- Do not define c73 before c72 owner evidence is analyzed.
- Do not continue the six interior records.
- Do not continue the one metadata-outside-runtime record.
- Do not partially admit one arm of a non-exact multi-successor record.
- Do not infer writer absence from zero candidates.
- Do not read slot qwords or chase pointers.
- Do not traverse the deferred producer by assumption.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or any other MOD repository.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed unless later independently proven and explicitly authorized.
