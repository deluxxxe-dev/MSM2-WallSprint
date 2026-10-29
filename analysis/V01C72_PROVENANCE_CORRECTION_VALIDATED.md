# V01C72 second provenance correction validated

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Status

The second c72 correction is **focused-test validated**. C72 is still not an accepted owner result.

Two prior owner attempts remain BLOCKED historical evidence:

```text
30720/162472 -> duplicate live c71 terminal root
31744/162472 -> post-c69 arrival lacks c69 proof
```

The second blocker was a c72 layering error: a path carrying a valid `c71_post_boundary` proof but no actual `c69_post_boundary` proof was still passed to historical `mark_post_c69()`. Historical c70 correctly refused that projection.

## Corrected rule

`source/tools/slot_publication_post_c71_stitched_fix.py` now preserves the proof layers:

- `c71_post_boundary` alone does not imply current post-c69 reachability;
- if the current path has no `c69_post_boundary`, c72 skips historical `mark_post_c69()` and authorizes only via the real c71 proof;
- if an actual `c69_post_boundary` proof is present, historical `mark_post_c69()` behavior is preserved unchanged;
- the retired-first-terminal observer correction and duplicate-root guard remain intact;
- accepted c56-c71 evidence identities and the c72 `102 / 95 / 99` domain are unchanged.

## Owner quick regression

Observed on the owner machine:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
PASS: c72 c71-only arrival does not invent post-c69 reachability
PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior
```

All four focused checks passed.

## Safety

No slot qword read, pointer chase, native/game probe, gameplay write or production integration was added.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.

## Next

Authorize exactly one further corrected c72 owner OFFLINE replay through `source/tools/slot_publication_post_c71_stitched_fix.py`.

If READY, analyze the owner artifact before defining c73. If BLOCKED, stop on that exact observed blocker and do not rerun by assumption.
