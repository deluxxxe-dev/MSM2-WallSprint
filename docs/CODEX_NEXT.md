# Next task — validate c72 retired-observer lifecycle correction

## Status

Corrected c71 owner OFFLINE result remains accepted as **VERIFIED-STATIC**.

The first c72 owner OFFLINE run is **BLOCKED** and is not accepted c72 evidence.

Canonical detail:

- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_READY.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`; do not merge `main`.

## Accepted c71 basis

```text
MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
physical SHA256 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
checkpoint identity 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
record stream SHA256 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
record count 102
```

Exact c72 continuation domain remains unchanged:

```text
c71 terminal records = 102
all-successor-exact records = 95
continuable exact architectural edges = 99
inside_unique_runtime_span_nonentry records = 6
executable_file_backed_outside_runtime_metadata records = 1
```

## Observed first-owner blocker

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

The duplicate guard was correct. The implementation gap was earlier in the lifecycle: after authenticating the accepted first c71 terminal and beginning continuation, initial c72 could reset the inherited c71 capture and treat a later distinct c70 POST in the same root as another first c71 terminal.

Accepted c71 itself still contains exactly 102 unique original roots.

## Correction

Use:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

The fix introduces an explicit retired-first-terminal-observer state:

- a successful first c71 authorization retires c71's first-terminal observer for that root;
- exact sibling callbacks from the same authenticated c71 source instruction still use the accepted sibling set;
- a later distinct c70 POST becomes `uncertified_post_c71_runtime_boundary`;
- that later distinct event is not relabeled as current `tx.post_c71` reachability;
- accepted c56-c71 evidence semantics and c71 identities are unchanged.

## Current task

Run only:

```text
source/tools/test_slot_publication_post_c71_stitched_fix_quick.py
```

Required PASS lines:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
```

If both pass, ChatGPT may record the correction as validated and authorize exactly one corrected c72 owner run using `slot_publication_post_c71_stitched_fix.py`.

If either fails, stop. Do not run the long owner replay.

## Do not

- Do not weaken or remove duplicate-root authentication.
- Do not relabel a later distinct event as `tx.post_c71` merely to pass the test.
- Do not alter the accepted 102 / 95 / 99 c71 domain.
- Do not continue the six interior records or one metadata-outside-runtime record.
- Do not define c73 before corrected c72 owner evidence is analyzed.
- Do not infer writer absence from zero candidates.
- Do not read slot qwords or chase pointers.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or any other MOD repository.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.
