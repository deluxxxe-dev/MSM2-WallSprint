# Next task — validate c72 checkpoint-order correction with focused regression

## Status

Corrected c71 owner OFFLINE result remains accepted as **VERIFIED-STATIC**.

Three c72 owner OFFLINE attempts are **BLOCKED** historical evidence only. None is accepted c72 evidence.

The third attempt materially advanced to the end of the complete historical replay:

```text
c72 full-replay c71 continuation: 162472/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: c72 live c71 record stream/order differs from accepted checkpoint
```

Canonical detail:

- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_READY.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C72_LIFECYCLE_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_2.md`
- `analysis/V01C72_PROVENANCE_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_3.md`

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

## Third blocker diagnosis

The failing final comparison occurs only after c72 has already proved the complete c71 domain, exact per-root record equality, duplicate-root exclusion, the 95-record preflight domain, the 99-edge domain and empty mismatch counters.

Therefore the observed failure is an order-only permutation of the same unique exact c71 records.

Accepted c71 checkpoint serialization order is order-sensitive because its stream hash is serialized in sequence. Live c72 replay discovery order is incidental and is not a semantic record identity.

## Prepared correction

`source/tools/slot_publication_post_c71_stitched_fix.py` now:

- keeps every existing per-root byte-for-byte authentication;
- keeps duplicate-root refusal fail-closed;
- keeps the prior retired-observer correction;
- keeps the prior c71-only/c69 proof-layer correction;
- records live arrival root order separately;
- presents the already-authenticated records in accepted checkpoint root order for final c71 stream/result reproduction only;
- emits diagnostic fields for live-order match/mismatch;
- does not change record content or the accepted `102 / 95 / 99` domain.

## Current task

Run only:

```text
source/tools/test_slot_publication_post_c71_stitched_fix_quick.py
```

Expected five PASS lines:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
PASS: c72 c71-only arrival does not invent post-c69 reachability
PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior
PASS: c72 exact per-root live records are normalized to accepted checkpoint order without weakening duplicate-root authentication
```

## If focused regression passes

Stop after the five PASS lines and return them to ChatGPT. ChatGPT will record focused validation and then authorize exactly one further full c72 owner replay.

## If focused regression fails

Stop. Do not rerun the full owner scan. Return the exact assertion/error.

## Do not

- Do not weaken per-root c71 record authentication.
- Do not remove duplicate-root refusal.
- Do not change accepted c71 checkpoint contents or identities.
- Do not alter `102 / 95 / 99`.
- Do not continue the six interior records or one metadata-outside-runtime record.
- Do not infer writer absence from zero candidates.
- Do not read slot qwords or chase pointers.
- Do not add native/game probes or gameplay/production writes.
- Do not modify SocialFeed or any other MOD repository.

## Binding safety

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed.
