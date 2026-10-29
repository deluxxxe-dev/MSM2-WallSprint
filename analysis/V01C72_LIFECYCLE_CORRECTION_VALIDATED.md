# V01C72 lifecycle correction validated

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Status

The first c72 owner OFFLINE run remains BLOCKED historical evidence:

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

Static diagnosis: accepted c71 still contains exactly 102 unique original roots. The duplicate guard fired because initial c72 reset the inherited c71 first-terminal capture after authenticating the accepted first c71 terminal, so a later distinct c70 POST in the same root could be presented as another first c71 terminal.

This is an observer-lifecycle implementation gap, not evidence of duplicate accepted c71 records.

## Correction

Correction wrapper:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

Rules preserved by the correction:

- authenticate the accepted first c71 terminal exactly once per root;
- retire the inherited first-terminal observer after successful first authorization;
- still allow exact sibling callbacks from the same authenticated c71 source instruction;
- route any later distinct c70 POST to `uncertified_post_c71_runtime_boundary`;
- do not relabel the later distinct event as current `tx.post_c71` reachability;
- keep duplicate-root authentication fail-closed;
- keep accepted c56-c71 evidence semantics and the 102 / 95 / 99 continuation domain unchanged.

## Owner quick validation

The owner ran:

```text
source/tools/test_slot_publication_post_c71_stitched_fix_quick.py
```

Observed:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
```

Therefore the exact first-owner blocker is covered by a focused regression.

This validation does **not** accept c72 owner evidence. One corrected owner OFFLINE run is still required.

## Safety

No accepted c56-c71 evidence semantics changed. No slot qword read, pointer chase, native/game probe, gameplay write or production integration was added. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`
