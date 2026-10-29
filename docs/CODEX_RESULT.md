# Codex result — c72 third owner attempt completed full replay; final order check BLOCKED; correction prepared

2026-10-29 JST. Branch **codex/v01c20-edi-value-provenance**.

## Status

Corrected c71 owner OFFLINE result remains accepted as **VERIFIED-STATIC**.

Three c72 owner OFFLINE attempts remain **BLOCKED** historical evidence only. No c72 result is accepted yet.

Third owner observation:

```text
c72 full-replay c71 continuation: 161792/162472
c72 full-replay c71 continuation: 162472/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: c72 live c71 record stream/order differs from accepted checkpoint
```

Unlike the prior blockers at 30720 and 31744, this attempt completed all 162472 historical roots and failed only during final accepted-c71 reproduction checks.

Canonical detail:

- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_2.md`
- `analysis/V01C72_PROVENANCE_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_3.md`

## What passed before the third blocker

The original c72 finalization reaches the failing order check only after proving:

```text
complete accepted c71 terminal record domain
exact byte-for-byte live record match per original root
no duplicate live root
the accepted 95 all-successor-exact record domain
the accepted 99 exact continuation-edge domain
empty c71 mismatch counters
```

Therefore the observed failure is order-only: the same unique accepted records arrived in a different live replay order than the checkpoint serialization order.

This attempt also exercises both earlier corrections through the complete replay, so the previous 30720 lifecycle blocker and 31744 proof-layer blocker are passed by real owner data.

## Prepared correction

Changed:

- `source/tools/slot_publication_post_c71_stitched_fix.py`
- `source/tools/test_slot_publication_post_c71_stitched_fix_quick.py`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_3.md`
- `docs/PROJECT_STATE.md`
- `docs/CODEX_NEXT.md`
- `docs/CODEX_RESULT.md`

Correction behavior:

- retain exact per-root c71 record authentication;
- retain duplicate-root fail-closed guard;
- retain the first-terminal observer retirement rule;
- retain the c71-only/c69 proof-layer rule;
- preserve live discovery order as diagnostic metadata only;
- reorder already-authenticated live records by accepted checkpoint root order solely for order-sensitive c71 record-stream/result reproduction;
- do not synthesize or modify any accepted c71 record.

A new focused regression intentionally presents the exact accepted records in a different live order, requires checkpoint-order iteration, and verifies that duplicate roots still refuse.

## Test status

The updated focused regression is prepared but has **not yet been observed on the owner machine** after this correction.

Expected fifth PASS:

```text
PASS: c72 exact per-root live records are normalized to accepted checkpoint order without weakening duplicate-root authentication
```

Do not authorize another full c72 owner replay until all five focused checks pass.

## Safety

Accepted c71 production domain remains unchanged:

```text
records = 102
all-successor-exact = 95
continuable exact edges = 99
non-exact = 7
```

No slot qword read, pointer chase, game/native probe, gameplay write or production integration was added.

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.

## Next

Run only the focused quick regression. If all five PASS, return the output to ChatGPT for validation and authorization of one further full c72 owner replay.
