# V0.1c72 corrected owner OFFLINE result — second BLOCKED observation

2026-10-29 JST. Branch `codex/v01c20-edi-value-provenance`.

## Status

The corrected c72 owner OFFLINE run is **BLOCKED** and is not accepted c72 evidence.

Accepted c71 owner evidence remains unchanged and **VERIFIED-STATIC**.

## Observed owner result

The corrected owner replay passed the first historical blocker point and reached:

```text
c72 full-replay c71 continuation: 30720/162472
c72 full-replay c71 continuation: 31744/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: post-c69 arrival lacks c69 proof
```

No c72 result should be accepted from this run.

## Static diagnosis

The first c72 lifecycle correction successfully moved beyond the prior `duplicate live c71 terminal root` blocker.

The second blocker is a different layering problem. C72's later-stage continuation can contain an authenticated `c71_post_boundary` proof even when the current live path does **not** itself contain a `c69_post_boundary` proof. Initial c72 manually called historical c70 `mark_post_c69()` after creating the c71-authorized destination context. Historical c70 correctly requires an actual c69 proof before marking current post-c69 reachability and therefore stopped with:

```text
post-c69 arrival lacks c69 proof
```

This does not prove stale/corrupt c69 or c71 evidence. It shows that c72 attempted to project a later c71 authorization backward into the older c69 reachability layer.

## Correction rule

Update only the c72 correction wrapper:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

For `mark_post_c69(key,path)` during c72 replay:

- if the path contains an actual `c69_post_boundary` proof, preserve historical c70 `mark_post_c69()` behavior unchanged;
- if the path contains `c71_post_boundary` proof but no `c69_post_boundary` proof, do **not** mark current post-c69 reachability;
- continue to require c72's later `mark_post_c71()` authorization from the actual c71 proof;
- do not alter accepted c56-c71 evidence semantics or identities;
- do not change the accepted c72 production domain `102 / 95 / 99`.

This is a fail-closed provenance correction, not a relaxation: c71-only evidence is no longer mislabeled as direct c69 reachability.

## Required focused validation before another owner run

The dedicated quick regression must prove both:

```text
PASS: c72 c71-only arrival does not invent post-c69 reachability
PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior
```

The prior retired-observer quick checks must also continue to pass.

Do not run the full owner replay until all four quick checks pass.

## Safety

No slot qword read, pointer chase, game/native probe, gameplay write or production integration is authorized or added by this correction.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.
