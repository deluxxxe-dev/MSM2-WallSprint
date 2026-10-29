# WallSprint project state

## Current stage — corrected c71 accepted; c72 owner attempts BLOCKED; second focused correction next

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

## First c72 owner attempt — BLOCKED

Observed:

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

Accepted c71 still contains exactly 102 unique original roots. Initial c72 failed to retire the inherited first-c71-terminal observer after authorization. The duplicate-root guard correctly stopped a later distinct c70 POST from being accepted as a second first terminal.

Correction wrapper:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
```

Dedicated quick regression passed:

```text
PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability
PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source
```

## Corrected c72 owner attempt — second BLOCKED

The corrected replay passed the first blocker point and reached:

```text
c72 full-replay c71 continuation: 30720/162472
c72 full-replay c71 continuation: 31744/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: post-c69 arrival lacks c69 proof
```

This second run is also **BLOCKED**, not accepted c72 evidence.

Static diagnosis: c72 can have an authenticated `c71_post_boundary` arrival whose current path does not itself contain a `c69_post_boundary` proof. Initial c72 still called historical c70 `mark_post_c69()` after such an arrival. C70 correctly refused because direct post-c69 reachability had not been proven on that path.

This is not evidence that accepted c69/c71 artifacts are corrupt. It is a c72 layering/provenance bug: a later c71 authorization was being projected backward into the older c69 reachability layer.

## Second c72 correction

The same fix wrapper now preserves historical c69 semantics exactly:

- if a c72 live path contains an actual `c69_post_boundary` proof, historical `mark_post_c69()` runs unchanged;
- if it contains a `c71_post_boundary` proof but no c69 proof, c72 does not invent current post-c69 reachability;
- c72 still requires and records the actual c71 proof through `mark_post_c71()`;
- the accepted c56-c71 evidence and the `102 / 95 / 99` c72 domain are unchanged.

Dedicated quick coverage has been added but is not yet owner-run/accepted:

```text
PASS: c72 c71-only arrival does not invent post-c69 reachability
PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior
```

The previous two retired-observer quick checks must remain PASS as well.

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

Run only the updated dedicated c72 correction quick regression.

If all four quick lines pass, record the second correction as validated and authorize exactly one further corrected c72 owner OFFLINE run through `source/tools/slot_publication_post_c71_stitched_fix.py`.

If any quick check fails, stop and analyze that focused failure. Do not run the long owner scan and do not define c73.
