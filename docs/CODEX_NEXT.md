# Next task — rerun corrected c72 owner OFFLINE continuation once

## Status

Corrected c71 owner OFFLINE result remains accepted as **VERIFIED-STATIC**.

The first c72 owner OFFLINE run is **BLOCKED** historical evidence only. Its observed lifecycle gap is corrected and the dedicated owner-blocker quick regression passed.

Canonical detail:

- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_READY.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C72_LIFECYCLE_CORRECTION_VALIDATED.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

## Accepted c71 basis

```text
MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
physical SHA256 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
checkpoint identity 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
record stream SHA256 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
record count 102
```

Exact c72 continuation domain remains:

```text
c71 terminal records = 102
all-successor-exact records = 95
continuable exact architectural edges = 99
inside_unique_runtime_span_nonentry records = 6
executable_file_backed_outside_runtime_metadata records = 1
```

Only the 95 all-successor-exact records / 99 exact edges may be provisionally continued.

## First owner blocker

Observed:

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

Accepted c71 itself still has exactly 102 unique original roots. The duplicate guard was correct; initial c72 failed to retire the inherited c71 first-terminal observer after the accepted first terminal had been authenticated, so a later distinct c70 POST in the same root could be offered as another first c71 terminal.

## Corrected implementation

Run the owner scan through:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

The correction must preserve these rules:

- authenticate the accepted first c71 terminal once per root;
- retire the inherited first-terminal observer after that authorization;
- preserve exact sibling callbacks from the same authenticated source instruction;
- any later distinct c70 POST becomes `uncertified_post_c71_runtime_boundary`;
- do not relabel that later distinct event as current `tx.post_c71` reachability;
- do not weaken duplicate-root authentication;
- do not change accepted c56-c71 evidence semantics or the 102 / 95 / 99 domain.

## Quick validation accepted

The owner ran:

```text
source/tools/test_slot_publication_post_c71_stitched_fix_quick.py
```

Observed:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
```

The exact first-owner blocker is covered.

## Current task

Run exactly one corrected c72 owner OFFLINE scan using the fix wrapper against the exact accepted c56-c71 evidence.

Expected success identity remains:

```text
repository_result POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
tool v0.1c72 / schema 1
contract exact_c71_full_replay_all_successor_continuation_v1
```

Require:

```text
full_historical_replay_used = true
checkpoint_accelerated_resume_used = false
```

## Required owner-result checks

Before defining c73, verify:

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

If the corrected owner run emits `POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED`, stop and analyze only that exact observed fact. Do not rerun or widen semantics by assumption.

## Do not

- Do not define c73 before corrected c72 owner evidence is analyzed.
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

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.
