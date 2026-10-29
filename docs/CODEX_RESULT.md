# Codex result — c72 first owner BLOCKED; lifecycle correction quick PASS

2026-10-29 JST. Branch **codex/v01c20-edi-value-provenance**.

## Status

Corrected c71 owner OFFLINE result remains accepted as **VERIFIED-STATIC**.

The first c72 owner OFFLINE run is **BLOCKED** and is not accepted c72 evidence. Its observed lifecycle implementation gap has been corrected, and the dedicated owner-blocker quick regression passed.

Canonical detail:

- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_PREPARED.md`
- `analysis/V01C72_TOOL_READY.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C72_LIFECYCLE_CORRECTION_VALIDATED.md`

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

## First c72 owner attempt

Observed owner progress:

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

This does not indicate duplicate accepted c71 records. Accepted c71 still contains one first terminal for each of 102 unique roots.

The failure was an observer-lifecycle bug: after c72 authenticated a root's accepted first c71 terminal and began continuation, the inherited c71 first-terminal observer could be reset. A later distinct c70 POST in that same root could then be offered to the accepted-c71 authenticator again. The duplicate-root guard correctly stopped the scan.

## Correction

Corrected wrapper:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

The correction retires the inherited first-terminal observer after successful first authorization while preserving exact sibling callbacks from the authenticated source. Any later distinct c70 POST becomes the next `uncertified_post_c71_runtime_boundary` without being relabeled as current `tx.post_c71` reachability.

Duplicate-root authentication remains fail-closed. Accepted c56-c71 semantics and the 102 / 95 / 99 domain are unchanged.

## Quick validation

The owner ran:

```text
source/tools/test_slot_publication_post_c71_stitched_fix_quick.py
```

Observed:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
```

The exact first-owner blocker is therefore covered by focused validation. C72 is still not accepted until one corrected owner replay completes and its artifact is analyzed.

## Safety

No slot qword read, pointer chase, game/native probe, gameplay write or production integration was added.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed. Zero candidates must not be interpreted as writer absence.

## Next

Run exactly one corrected c72 owner OFFLINE continuation through:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

If READY, analyze the owner artifact before defining c73. If BLOCKED, analyze the exact blocker before rerunning or widening anything.
