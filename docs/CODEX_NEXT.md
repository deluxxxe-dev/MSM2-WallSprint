# Next task — run corrected c72 owner OFFLINE continuation once

## Status

Corrected c71 owner OFFLINE result remains accepted as **VERIFIED-STATIC**.

Both prior c72 owner OFFLINE attempts are **BLOCKED** historical evidence only. Neither is accepted c72 evidence.

The first retired-observer correction and the second c71-only provenance correction have both passed focused owner regressions.

Canonical detail:

- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_READY.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C72_LIFECYCLE_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_2.md`
- `analysis/V01C72_PROVENANCE_CORRECTION_VALIDATED.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

## Accepted c71 basis

```text
MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
physical SHA256 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
checkpoint identity 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
record stream SHA256 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
record count 102
```

Exact c72 continuation domain remains unchanged:

```text
c71 terminal records = 102
all-successor-exact records = 95
continuable exact architectural edges = 99
inside_unique_runtime_span_nonentry records = 6
executable_file_backed_outside_runtime_metadata records = 1
```

Only the 95 all-successor-exact records / 99 exact edges may be provisionally continued.

## Historical blockers

Attempt 1:

```text
30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

Attempt 2, after the first correction:

```text
31744/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: post-c69 arrival lacks c69 proof
```

The first blocker was an inherited first-terminal observer lifecycle bug. The second was a proof-layering bug where a valid c71-only arrival was still sent to historical `mark_post_c69()`.

## Corrected implementation

Run through:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

The wrapper must preserve all of the following:

- authenticate each accepted first c71 terminal only once per root;
- retire the inherited c71 first-terminal observer after successful authorization;
- preserve exact sibling callbacks from the authenticated source instruction;
- later distinct c70 POST events become `uncertified_post_c71_runtime_boundary` without being relabeled as current `tx.post_c71` reachability;
- c71 proof alone does not imply current post-c69 reachability;
- c71-only paths skip historical `mark_post_c69()` and proceed only through actual c71 authorization;
- paths containing a real `c69_post_boundary` proof preserve historical `mark_post_c69()` unchanged;
- duplicate-root authentication remains fail-closed;
- accepted c56-c71 semantics and the `102 / 95 / 99` domain remain unchanged.

## Focused validation accepted

The owner ran the updated quick regression and observed all four required PASS lines:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
PASS: c72 c71-only arrival does not invent post-c69 reachability
PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior
```

## Current task

Run exactly one further corrected c72 owner OFFLINE scan using the fix wrapper against the exact accepted c56-c71 evidence.

Expected successful repository result remains:

```text
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY
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

If this owner run emits `POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED`, stop and analyze only that exact observed fact. Do not rerun or widen semantics by assumption.

## Do not

- Do not weaken historical c69 proof requirements.
- Do not relabel c71-only paths as post-c69.
- Do not weaken duplicate-root authentication.
- Do not alter the accepted 102 / 95 / 99 c71 domain.
- Do not continue the six interior records or one metadata-outside-runtime record.
- Do not define c73 before c72 owner evidence is accepted and analyzed.
- Do not infer writer absence from zero candidates.
- Do not read slot qwords or chase pointers.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or any other MOD repository.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.
