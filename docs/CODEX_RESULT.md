# Codex result — c72 owner attempts BLOCKED; second provenance correction validated

2026-10-29 JST. Branch **codex/v01c20-edi-value-provenance**.

## Status

Corrected c71 owner OFFLINE result remains accepted as **VERIFIED-STATIC**.

Both c72 owner OFFLINE attempts remain **BLOCKED** historical evidence only. Neither is accepted c72 evidence.

The second c72 provenance correction is now focused-test validated and one further corrected owner replay is authorized.

Canonical detail:

- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_PREPARED.md`
- `analysis/V01C72_TOOL_READY.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C72_LIFECYCLE_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_2.md`
- `analysis/V01C72_PROVENANCE_CORRECTION_VALIDATED.md`

## Accepted c71 basis

```text
MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
physical SHA256 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
checkpoint identity 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
record count 102
record stream SHA256 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
```

Exact c72 bounded domain remains:

```text
c71 terminal records = 102
all-successor-exact records = 95
continuable exact edges = 99
interior-containing records = 6
metadata-outside-runtime records = 1
```

## Historical owner attempts

Attempt 1:

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

Correction: retire the inherited c71 first-terminal observer after successful first authorization while preserving exact sibling callbacks.

Attempt 2, after the first correction:

```text
c72 full-replay c71 continuation: 31744/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: post-c69 arrival lacks c69 proof
```

Correction: preserve proof-layer semantics. A valid c71-only arrival is not relabeled as current post-c69 reachability; historical `mark_post_c69()` runs only when an actual `c69_post_boundary` proof is present.

## Updated focused validation

The owner ran:

```text
source/tools/test_slot_publication_post_c71_stitched_fix_quick.py
```

Observed:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
PASS: c72 c71-only arrival does not invent post-c69 reachability
PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior
```

All four checks passed. Accepted c56-c71 evidence semantics and the `102 / 95 / 99` production domain remain unchanged.

## Safety

No slot qword read, pointer chase, game/native probe, gameplay write or production integration was added.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed. Zero candidates must not be interpreted as writer absence.

## Next

Run exactly one further corrected c72 owner OFFLINE continuation through:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

If READY, analyze the owner artifact before defining c73. If BLOCKED, stop and analyze the exact observed blocker before rerunning or widening anything.
