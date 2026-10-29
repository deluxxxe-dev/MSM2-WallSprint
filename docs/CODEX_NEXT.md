# Next task — validate c72 c71-only arrival provenance correction

## Status

Corrected c71 owner OFFLINE result remains accepted as **VERIFIED-STATIC**.

Both c72 owner OFFLINE attempts are **BLOCKED** historical evidence only. Neither is accepted c72 evidence.

Canonical detail:

- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_READY.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C72_LIFECYCLE_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_2.md`

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

Only the 95 all-successor-exact records / 99 exact edges may be provisionally continued.

## First owner blocker

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

The inherited c71 first-terminal observer was not retired after successful first authorization. This was corrected in:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

Accepted focused proof:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
```

## Second owner blocker

The corrected owner replay passed the first blocker point and reached:

```text
c72 full-replay c71 continuation: 31744/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: post-c69 arrival lacks c69 proof
```

The blocker is not a reason to weaken c70. Historical c70 is correct to require actual `c69_post_boundary` proof before marking current post-c69 reachability.

The c72 bug was calling historical `mark_post_c69()` after a valid c71-authorized arrival whose current path had `c71_post_boundary` proof but no actual c69 proof. That would incorrectly project later c71 authorization backward into c69 reachability.

## Second correction

Continue to use:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

The wrapper must enforce:

- c71 proof alone does not imply current post-c69 reachability;
- when the live path contains `c71_post_boundary` and no `c69_post_boundary`, skip historical `mark_post_c69()` and proceed only through c72's actual `mark_post_c71()` authorization;
- when the live path contains an actual `c69_post_boundary`, preserve historical `mark_post_c69()` unchanged;
- do not alter accepted c56-c71 evidence semantics or identities;
- do not alter the exact c72 `102 / 95 / 99` domain;
- keep the first lifecycle correction and duplicate-root guard intact.

## Current task

Run only the updated quick regression:

```text
source/tools/test_slot_publication_post_c71_stitched_fix_quick.py
```

Required PASS lines are all four:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
PASS: c72 c71-only arrival does not invent post-c69 reachability
PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior
```

If all four pass, ChatGPT may record the second correction as validated and authorize exactly one further corrected c72 owner run using `slot_publication_post_c71_stitched_fix.py`.

If any fails, stop. Do not run the long owner replay.

## If a later owner run is BLOCKED

Stop and analyze only the exact observed blocker. Do not rerun or widen semantics by assumption.

## Do not

- Do not weaken historical c69 proof requirements.
- Do not relabel c71-only paths as post-c69.
- Do not weaken duplicate-root authentication.
- Do not alter the accepted 102 / 95 / 99 c71 domain.
- Do not continue the six interior records or one metadata-outside-runtime record.
- Do not define c73 before c72 owner evidence is accepted and analyzed.
- Do not infer writer absence from zero candidates.
- Do not read slot qwords or chase pointers.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or any other MOD repository.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.
