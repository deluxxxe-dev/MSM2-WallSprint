# WallSprint project state

## Current stage — corrected c71 accepted; two c72 owner attempts BLOCKED; second correction quick PASS; owner rerun next

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

The corrected replay passed the first blocker and reached:

```text
c72 full-replay c71 continuation: 31744/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: post-c69 arrival lacks c69 proof
```

Diagnosis: c72 can have a valid `c71_post_boundary` arrival whose current path does not contain an actual `c69_post_boundary` proof. The implementation still called historical `mark_post_c69()` in that case. Historical c70 correctly refused; c71 authorization must not be projected backward into post-c69 reachability.

## Second correction — focused validated

The same wrapper now preserves proof layers exactly:

- c71 proof alone does not imply current post-c69 reachability;
- c71-only paths skip historical `mark_post_c69()` and proceed only through actual c71 authorization;
- paths with real `c69_post_boundary` proof retain historical `mark_post_c69()` behavior unchanged;
- the first retired-observer correction and duplicate-root guard remain intact;
- accepted c56-c71 semantics and the `102 / 95 / 99` domain are unchanged.

Owner quick regression passed all four checks:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
PASS: c72 c71-only arrival does not invent post-c69 reachability
PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior
```

C72 is still not an accepted owner result until the next corrected owner replay completes and its artifact is analyzed.

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

Run exactly one further corrected c72 owner OFFLINE continuation through `source/tools/slot_publication_post_c71_stitched_fix.py` against accepted c56-c71 evidence.

If READY, analyze c71 admission/refusal counts, next-boundary count, candidates and safety before defining c73.

If BLOCKED, stop and analyze that exact observed fact before changing anything or rerunning.
