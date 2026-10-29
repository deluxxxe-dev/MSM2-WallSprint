# V01C72 owner OFFLINE blocked result

Date: 2026-10-29 JST
Branch: `codex/v01c20-edi-value-provenance`

## Status

**BLOCKED — not accepted as c72 owner evidence.**

The first c72 owner OFFLINE run stopped fail-closed during the full c71 continuation replay:

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

The PowerShell wrapper then reported `c72 owner offline continuation failed`.

No c72 owner result is accepted from this run.

## Static diagnosis

Accepted c71 is a first-terminal diagnostic: it records exactly one first c70 POST terminal per root. C72 initially reset the c71 capture each time it encountered a c70 POST so that it could reproduce the accepted first terminal before continuation.

The owner run demonstrated a lifecycle form missing from the focused fixture: after the accepted first c71 terminal for a root was authenticated and continuation started, the same root later exposed a distinct c70 POST that was not necessarily already marked as current `tx.post_c71` reachability. The initial c72 implementation attempted to capture/authenticate that later distinct event again as another first c71 terminal. `ProofPool.authenticate_live()` correctly rejected the second authentication for the same accepted root as `duplicate live c71 terminal root`.

This is an observer-lifecycle implementation gap, not evidence that accepted c71 contains duplicate roots. The accepted c71 checkpoint still has exactly 102 unique original roots.

## Correction

Added `source/tools/slot_publication_post_c71_stitched_fix.py`.

After a successful accepted c71 first-terminal authorization, c72 now retires the inherited c71 first-terminal observer for that root:

- exact sibling callbacks from the same authenticated c71 source instruction still use the accepted complete sibling set;
- any later distinct c70 POST is routed to the next c72 unknown boundary (`uncertified_post_c71_runtime_boundary`);
- the later distinct edge is **not** relabeled as current `tx.post_c71` reachability;
- no c71 checkpoint identity/domain or accepted c56-c71 evidence semantics are changed.

Added `source/tools/test_slot_publication_post_c71_stitched_fix_quick.py` to cover the exact owner-observed lifecycle before another long owner replay.

## Safety

The blocked run did not authorize c73 or widen any production/gameplay gate. C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

Do not rerun the owner scan until the dedicated quick regression passes.
