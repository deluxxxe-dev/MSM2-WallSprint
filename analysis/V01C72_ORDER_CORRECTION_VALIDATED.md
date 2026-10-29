# V01C72 checkpoint-order correction validated

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Status

The third c72 correction is **focused-test validated**. C72 is still not an accepted owner result until one further corrected full owner replay reaches READY and its artifact is analyzed.

Historical owner attempt 3 reached the end of the full historical replay:

```text
c72 full-replay c71 continuation: 162472/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: c72 live c71 record stream/order differs from accepted checkpoint
```

Before that final blocker, c72 had already established the complete accepted c71 domain, exact byte-for-byte per-root live record equality, duplicate-root exclusion, the accepted 95 all-successor-exact record domain, the accepted 99 continuation-edge domain, and empty c71 mismatch counters.

Therefore the blocker was limited to ordering of the same unique exact records: accepted c71 checkpoint serialization order is order-sensitive, while c72 live replay discovery order is incidental.

## Corrected rule

`source/tools/slot_publication_post_c71_stitched_fix.py` now preserves all semantic/authentication constraints while normalizing only the final reproduction order:

- authenticate every live c71 record byte-for-byte against the accepted record for its `original_root_runtime_row`;
- retain duplicate-root refusal fail-closed;
- retain the first-terminal observer retirement correction;
- retain the c71-only/c69 proof-layer correction;
- preserve live arrival root order separately as diagnostic metadata;
- after the complete exact per-root domain is proven, iterate the already-authenticated records in accepted checkpoint root order only for the order-sensitive c71 stream/result reproduction;
- do not synthesize, alter, weaken, or replace any accepted c71 record;
- do not change the accepted `102 / 95 / 99` continuation domain.

## Owner focused regression

Observed on the owner machine:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
PASS: c72 c71-only arrival does not invent post-c69 reachability
PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior
PASS: c72 exact per-root live records are normalized to accepted checkpoint order without weakening duplicate-root authentication
```

All five focused checks passed.

## Interpretation

This validates the correction locally without weakening any c71 authentication rule. In particular, the fifth check proves that a permutation of exact accepted per-root records is normalized only after exact identity and duplicate-root checks, so ordering is treated as serialization metadata rather than semantic identity.

The next full owner replay will be the first test of this correction against all `162472` historical roots.

## Safety

No slot qword read, pointer chase, native/game probe, gameplay write, or production integration was added.

Runtime remains `v0.1c38`; c72 remains OFFLINE/STATIC.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.

## Next

Authorize exactly one further corrected c72 owner OFFLINE replay through:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

If it reaches `POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY`, analyze the produced c72 artifact before defining c73 or changing exploration strategy.

If it emits another `POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED`, stop on that exact observed fact and do not rerun unchanged.
