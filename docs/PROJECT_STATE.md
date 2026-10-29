# WallSprint project state

## Current stage — corrected c71 accepted; first c72 owner BLOCKED; lifecycle correction quick PASS; corrected owner rerun next

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

## Accepted c71 basis

```text
MSM2-WS-v0.1c71-authenticated-prefix-checkpoint.json
physical SHA256 25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926
checkpoint identity 4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e
record stream SHA256 6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8
record count 102
```

Embedded c71 result:

```text
repository_result POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY
tool v0.1c71 / schema 1
semantic result identity 18d61be59032514a4c74c9b015f422c379e2c05050e2d5819c1025048a2276d7
classifier identity 18cba768c4261886b70496e7f73abe3fe5276b44aecadb0d929802ae3dae9fd5
```

Exact image remains:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

## Exact c72 continuation domain

```text
c71 terminal records = 102
all-successor-exact records = 95
continuable exact architectural edges = 99
inside_unique_runtime_span_nonentry records = 6
executable_file_backed_outside_runtime_metadata records = 1
```

Only the 95 records / 99 exact edges are eligible for provisional c72 continuation. The seven non-exact records remain wholly fail-closed.

## First c72 owner attempt

Observed:

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

This run is **BLOCKED**, not accepted c72 evidence.

Static diagnosis: accepted c71 still contains exactly 102 unique original roots. Initial c72 authenticated the accepted first c71 terminal, then later reset the inherited c71 capture. A later distinct c70 POST in the same root could therefore be presented as another first c71 terminal. The duplicate-root guard correctly stopped the scan.

## Lifecycle correction

Use:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

The correction:

- retires c71's inherited first-terminal observer after the accepted first terminal is authenticated;
- preserves exact sibling callbacks from the same authenticated c71 source instruction;
- routes any later distinct c70 POST to `uncertified_post_c71_runtime_boundary`;
- does not relabel the later event as current `tx.post_c71` reachability;
- preserves duplicate-root authentication and accepted c56-c71 semantics;
- does not change the 102 / 95 / 99 domain.

Dedicated owner quick regression passed:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
```

The exact first-owner blocker is therefore covered. C72 is still not an accepted owner result until the corrected owner scan completes and its artifact is analyzed.

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

Run exactly one corrected c72 owner OFFLINE continuation using `source/tools/slot_publication_post_c71_stitched_fix.py` against the accepted c56-c71 evidence.

If READY, analyze the resulting c71 admission/refusal counts, next-boundary count, candidates and safety before defining c73.

If BLOCKED, stop and analyze that exact observed fact before changing anything or rerunning.
