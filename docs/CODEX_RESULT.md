# Codex result — c72 owner attempts BLOCKED; second provenance correction prepared

2026-10-29 JST. Branch **codex/v01c20-edi-value-provenance**.

## Status

Corrected c71 owner OFFLINE result remains accepted as **VERIFIED-STATIC**.

Both c72 owner OFFLINE attempts are **BLOCKED** and are not accepted c72 evidence.

Canonical detail:

- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_PREPARED.md`
- `analysis/V01C72_TOOL_READY.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C72_LIFECYCLE_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_2.md`

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

Observed:

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

This was an observer-lifecycle bug. The duplicate-root guard correctly stopped the run. The fix retires the inherited first-c71-terminal observer after successful first authorization while preserving exact sibling callbacks.

Owner quick validation passed:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
```

## Second corrected c72 owner attempt

The corrected run passed the first blocker point and reached:

```text
c72 full-replay c71 continuation: 31744/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: post-c69 arrival lacks c69 proof
```

This is a separate c72 layering/provenance bug, not evidence that accepted c69 or c71 artifacts are corrupt.

A valid c71 continuation can arrive with an authenticated `c71_post_boundary` proof while the current path does not itself carry a `c69_post_boundary` proof. Initial c72 still called historical c70 `mark_post_c69()` for that arrival. Historical c70 correctly refused because direct post-c69 reachability was not present on the path.

## Second correction

Updated:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
source/tools/test_slot_publication_post_c71_stitched_fix_quick.py
```

Correction rule:

- a path with actual `c69_post_boundary` proof keeps historical `mark_post_c69()` unchanged;
- a path with `c71_post_boundary` but no c69 proof is not relabeled as current post-c69 reachability;
- c72 still requires the real c71 authorization through `mark_post_c71()`;
- accepted c56-c71 semantics and the `102 / 95 / 99` production domain remain unchanged;
- the first retired-observer correction and duplicate-root guard remain intact.

New focused checks added:

```text
PASS: c72 c71-only arrival does not invent post-c69 reachability
PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior
```

These new lines are prepared but not yet owner-run/accepted. The previous two quick lines must also remain PASS.

## Safety

No slot qword read, pointer chase, game/native probe, gameplay write or production integration was added.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed. Zero candidates must not be interpreted as writer absence.

## Next

Run only the updated c72 quick regression. Do not run the long owner replay unless all four quick checks pass.
