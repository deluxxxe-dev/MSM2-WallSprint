# V01C67 owner OFFLINE blocked result

Date: 2026-10-28 JST
Branch: `codex/v01c20-edi-value-provenance`

## Owner result

The first real owner/private-image c67 run did **not** produce a c67 JSON. It failed closed with:

```text
POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED: converted c64 POST lacks exact original c65 lower capture
```

This is not evidence corruption and not a writer/publication finding.

## Root cause

**VERIFIED-STATIC:** c66 `View.edge()` intentionally converts an already-observed lower `c64.POST` into c66 `uncertified_post_c65_runtime_boundary` before c66's diagnostic-only `post63_capture` assignment executes. C66 did not need that lower capture for its accepted continuation accounting, so the accepted c66 owner result remains valid.

C67's first implementation assumed that `view.post63_capture` would always already exist when a c66 POST carried `c64.POST` as its cause. The owner data exercised the converted path and exposed that assumption. The synthetic c67 tests covered nested converted c65 authorization provenance, but did not directly exercise c66's own `c64.POST -> c66.POST` conversion path.

## Correction scope

Added:

```text
source/tools/slot_publication_post_c65_boundary_fix.py
```

The correction does **not** modify c66 or accepted c56-c66 evidence. It observes the exact lower c65 edge-entry state immediately before the unchanged c66 edge callback. Only if the actual callback then fails as `c66.POST` caused by `c64.POST` does it reconstruct the omitted lower diagnostic capture and run the unchanged c65 capture/provenance validator against that exact current context. The original refusal is still rethrown.

No target traversal, edge admission, Fact propagation, candidate creation, qword read, pointer chase, native/game probe, gameplay write or safety-gate promotion is added.

## Status

C67 owner artifact remains **NOT ACCEPTED / NOT PRODUCED**.

The prior `TOOL READY` status is superseded until the correction passes focused regression and the owner OFFLINE classifier produces a complete result.

C52 remains binding:

```text
PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL
```
