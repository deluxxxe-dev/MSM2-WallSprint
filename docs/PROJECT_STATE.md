# WallSprint project state

## Current stage — corrected c71 accepted; c72 owner attempt 3 reached 162472/162472 then BLOCKED on final record order; order correction prepared

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

## C72 owner attempt 1 — BLOCKED

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

Diagnosis: initial c72 failed to retire the inherited first-c71-terminal observer after successful first authorization. A later distinct c70 POST could then be offered as another first c71 terminal. The duplicate-root guard correctly stopped the run.

Correction: retire the first-terminal observer after authorization while preserving exact siblings from the authenticated source. Dedicated quick regression passed.

## C72 owner attempt 2 — BLOCKED

```text
c72 full-replay c71 continuation: 31744/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: post-c69 arrival lacks c69 proof
```

Diagnosis: c72 can have a valid `c71_post_boundary` arrival whose current path does not contain an actual `c69_post_boundary` proof. The implementation still called historical `mark_post_c69()` in that case. Historical c70 correctly refused; c71 authorization must not be projected backward into post-c69 reachability.

Second correction focused validation passed all four checks. C71 proof alone no longer invents current c69 reachability; real c69 proof still preserves historical behavior.

## C72 owner attempt 3 — full replay completed, final order check BLOCKED

Owner observation:

```text
c72 full-replay c71 continuation: 161792/162472
c72 full-replay c71 continuation: 162472/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: c72 live c71 record stream/order differs from accepted checkpoint
```

This is materially later than both prior blockers. The traversal itself reached `162472 / 162472`.

Before this final check, c72 had already required:

- complete accepted c71 terminal domain encountered;
- byte-for-byte live record authentication per `original_root_runtime_row`;
- duplicate-root refusal unchanged;
- accepted 95-record all-successor-exact preflight domain matched;
- accepted 99 exact-edge domain matched;
- c71 mismatch counters empty.

Therefore the observed blocker is an **order-only permutation of the same unique exact per-root c71 records**, not a changed record body/domain. The accepted checkpoint serializes an order-sensitive stream; live c72 replay discovery order is incidental and need not equal that serialization order.

Correction prepared in `source/tools/slot_publication_post_c71_stitched_fix.py`:

- preserve live arrival order as diagnostic metadata;
- store each already-authenticated live record by original root;
- retain duplicate-root fail-closed behavior;
- iterate exact live records in accepted checkpoint root order only for final c71 record-stream/result reproduction;
- do not change accepted c71 record content or the `102 / 95 / 99` domain.

Focused regression now has five checks; the new fifth check covers intentionally permuted live arrival order plus duplicate-root refusal.

C72 is still not an accepted owner result.

## Reference-MOD review

Owner-supplied reference MOD archives were statically reviewed separately while c72 remained the bounded task. Durable findings are recorded in `analysis/REFERENCE_MOD_WALLRUN_REVIEW_20261029.md` and reusable material was copied to `MSM2-Wisdoms`.

Most important hint: WallRun has separate config asset families for speed, jump and steering, including `hero_WallRunSteeringSetup_Standard.config`. This is research guidance only and does not replace the current c72 proof task or authorize third-party code/assets.

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

Run only the focused c72 correction regression:

```text
source/tools/test_slot_publication_post_c71_stitched_fix_quick.py
```

Expected: five PASS lines, including the new checkpoint-order normalization check.

If all five PASS, record focused validation and authorize exactly one further corrected c72 owner OFFLINE replay. Do not rerun the full owner scan before the focused check passes.
