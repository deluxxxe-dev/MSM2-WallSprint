# WallSprint project state

## Current stage — corrected c71 accepted; c72 owner attempt 4 BLOCKED at final canonical record reproduction; representation correction prepared

Branch: `codex/v01c20-edi-value-provenance`.
Do not merge `main` into this development line.

Accepted c66, corrected c67, c68, corrected c69, corrected c70, and corrected c71 owner OFFLINE results are **VERIFIED-STATIC**.

Canonical recent detail:

- `analysis/V01C69_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C70_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C71_OWNER_OFFLINE_RESULT.md`
- `analysis/V01C72_TOOL_PREPARED.md`
- `analysis/V01C72_TOOL_READY.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT.md`
- `analysis/V01C72_LIFECYCLE_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_2.md`
- `analysis/V01C72_PROVENANCE_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_3.md`
- `analysis/V01C72_ORDER_CORRECTION_VALIDATED.md`
- `analysis/V01C72_OWNER_BLOCKED_RESULT_4.md`
- `analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md`

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

Only the 95 records / 99 exact edges are eligible for provisional c72 continuation. The seven non-exact records remain wholly fail-closed.

## C72 owner attempt 1 — BLOCKED at 30720/162472

```text
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

Cause: inherited first-c71-terminal observer was not retired after first authorization. Correction retires it while retaining exact sibling callbacks. Focused regression passed, and later owner runs passed this point.

## C72 owner attempt 2 — BLOCKED at 31744/162472

```text
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: post-c69 arrival lacks c69 proof
```

Cause: a valid c71-only arrival was still sent through historical `mark_post_c69()`. Correction keeps proof layers separate: c71 does not imply current c69; real c69 proof still preserves historical behavior. Focused regression passed, and later owner runs passed this point.

## C72 owner attempt 3 — full replay completed; final stream guard BLOCKED

Observed:

```text
c72 full-replay c71 continuation: 162472/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: c72 live c71 record stream/order differs from accepted checkpoint
```

Before this final guard, c72 had already required the complete c71 terminal domain, canonical per-root record equality, duplicate-root exclusion, the 95-record all-exact domain, the 99-edge domain and empty mismatch counters.

A first correction separated incidental live discovery order from accepted checkpoint serialization order. Owner focused regression passed five checks, including checkpoint-order normalization.

## C72 owner attempt 4 — same final guard after order normalization

After the five-check order correction was owner-validated, the authorized full replay again ended at:

```text
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: c72 live c71 record stream/order differs from accepted checkpoint
```

This guard is after the full historical root loop and the same domain/authentication checks. Therefore attempt 4 does not reopen the direct traversal, the 102/95/99 domain, or per-root accepted record content.

It does prove that the attempt-3 diagnosis was too narrow: ordering alone was not the whole final-comparison issue.

Static diagnosis: per-root authentication uses canonical `c56.encoded()` JSON bytes, while the order adapter's final equality still used raw Python object equality. A live tuple and a checkpoint JSON list can be canonically identical but raw-Python-unequal.

## Fourth correction — prepared, not yet owner-validated

`source/tools/slot_publication_post_c71_stitched_fix.py` now:

- preserves exact per-root `c56.encoded()` authentication;
- preserves duplicate-root fail-closed behavior;
- preserves checkpoint-root-order normalization;
- preserves first-terminal observer retirement;
- preserves c71-only/c69 proof separation;
- changes only the final adapter equality to compare each record through `c56.encoded()` as well;
- keeps accepted c71 record contents and the `102 / 95 / 99` domain unchanged.

The focused regression now has six checks. The new sixth check intentionally uses list-form accepted data and canonically equivalent tuple-form live data; raw Python equality must differ while canonical encoded identity must match.

Expected new PASS:

```text
PASS: c72 final stream comparison uses canonical encoded record identity rather than raw Python list/tuple representation
```

Do not run another full owner replay until all six focused checks pass on the owner machine.

## Reference-MOD review

Owner-supplied reference MOD archives were statically reviewed separately. Durable detail is in `analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md`; reusable findings were synchronized to `MSM2-Wisdoms`.

Most important hint: WallRun has separate config asset families for speed, jump and steering, including `hero_WallRunSteeringSetup_Standard.config`. This remains research guidance only and does not replace current c72 proof work.

## Safety

Runtime remains `v0.1c38`; c53-c72 work is OFFLINE/STATIC.

```text
live_qword_reads = 0
checkpoint_accelerated_resume_used = false
```

C52 remains binding:

`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL`

All publication/allocation/base/range/lifetime/ownership/destination/CFG/EH/production/watch/game gates remain closed. Zero candidates does not prove writer absence. Do not modify SocialFeed or other MOD repositories.

## Current bounded action

Run only:

```text
source/tools/test_slot_publication_post_c71_stitched_fix_quick.py
```

Require all six PASS lines. If the sixth fails, stop and analyze it. If all six pass, record focused validation before authorizing one further corrected c72 owner replay.
