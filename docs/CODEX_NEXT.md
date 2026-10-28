# Next task — c67 converted-c64 lower-capture correction validation

## Status

Accepted owner c66 result remains **VERIFIED-STATIC**.

The first owner c67 OFFLINE run failed closed with:

```text
POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED: converted c64 POST lacks exact original c65 lower capture
```

Canonical blocker record:

- `analysis/V01C67_OWNER_BLOCKED_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

## Verified root cause

C66 converts an actual lower `c64.POST` into c66 `uncertified_post_c65_runtime_boundary` before c66 stores the diagnostic-only lower c65 capture. C67 incorrectly assumed that lower capture would already be present.

Accepted c66 code/evidence is not wrong and must remain read-only.

## Current bounded task

Validate the c67-only correction:

```text
source/tools/slot_publication_post_c65_boundary_fix.py
```

The fix may only reconstruct the omitted lower diagnostic capture from the exact same c66 edge invocation:

1. snapshot the exact c65 lower edge-entry state before the unchanged c66 edge callback;
2. call the unchanged c66 implementation;
3. only if it actually returns `c66.POST` caused by `c64.POST`, bind that exact lower snapshot to the current source;
4. run the unchanged c65 `capture_for`/provenance validation;
5. continue c67 diagnostic capture and rethrow the original refusal.

It must not modify c66, accepted c56-c66 artifacts, traversal/admission semantics, Facts, candidates or safety gates.

## Required validation

Run the c67 focused regression under the correction layer, then rerun the exact owner OFFLINE classifier against accepted c56-c66 evidence.

Success requires:

```text
POST_C65_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
```

and output:

```text
logs/MSM2-WS-v0.1c67-post-c65-runtime-boundary.json
```

If a new blocker appears, stop and preserve evidence. Do not weaken provenance checks.

## After owner result

ChatGPT must analyze the exact c67 JSON first, persist the VERIFIED-STATIC result if valid, and only then define c68.

C52 remains binding:

```text
PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL
```

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed. Zero candidates never proves writer absence. Do not modify other MSM2 MOD repositories.
