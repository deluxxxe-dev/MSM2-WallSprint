# WallSprint project state

## Current stage — corrected c71 accepted; c72 owner attempt 4 BLOCKED at final canonical record reproduction; representation correction focused PASS; owner replay next

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66, corrected c67, c68, corrected c69, corrected c70, and corrected c71 owner OFFLINE results are **VERIFIED-STATIC**.

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
- `analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md`

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

Only the 95 records / 99 exact edges are eligible for provisional c72 continuation. The seven non-exact records remain wholly fail-closed.

## C72 owner history

Attempt 1 blocked at `30720 / 162472` on duplicate live c71 terminal root. The first-terminal observer retirement correction is focused-test validated and later owner attempts passed this point.

Attempt 2 blocked at `31744 / 162472` on `post-c69 arrival lacks c69 proof`. The proof-layer correction is focused-test validated and later owner attempts passed this point.

Attempt 3 completed `162472 / 162472` and blocked only on final accepted-c71 stream/order reproduction. Checkpoint-order normalization was added and focused-test validated.

Attempt 4 again completed the traversal/domain work and blocked at the same final reproduction guard. Static diagnosis showed that per-root authentication used canonical `c56.encoded()` equality while the final ordered adapter still used raw Python object equality, allowing canonically identical tuple/list representations to fail only at the last check.

## Fourth correction — owner focused validation PASS

`source/tools/slot_publication_post_c71_stitched_fix.py` now preserves all previous corrections and changes only the final ordered-record equality to compare canonical `c56.encoded()` bytes record-by-record.

Owner observed all six focused PASS lines:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
PASS: c72 c71-only arrival does not invent post-c69 reachability
PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior
PASS: c72 exact per-root live records are normalized to accepted checkpoint order without weakening duplicate-root authentication
PASS: c72 final stream comparison uses canonical encoded record identity rather than raw Python list/tuple representation
```

This validates the representation correction only at focused-test level. C72 remains unaccepted until one corrected full owner replay reaches READY and its output artifact is analyzed.

## Reference-MOD review

Owner-supplied reference MOD archives were statically reviewed separately. Durable detail is in `analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md`; reusable findings were synchronized to `MSM2-Wisdoms`.

Most important hint: WallRun has separate config asset families for speed, jump and steering, including `hero_WallRunSteeringSetup_Standard.config`. This remains research guidance only and does not replace current c72 proof work.

## Safety

Runtime remains `v0.1c38`; c53-c72 work is OFFLINE/STATIC.

```text
live_qword_reads = 0
checkpoint_accelerated_resume_used = false
```

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed. Zero candidates does not prove writer absence. Do not modify SocialFeed or other MOD repositories.

## Current bounded action

Run exactly one further corrected c72 owner OFFLINE continuation through:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

against the accepted c56-c71 evidence.

If READY, analyze the generated c72 artifact before defining c73 or changing exploration strategy.

If BLOCKED, stop and analyze that exact observed fact before changing anything or rerunning.
