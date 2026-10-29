# WallSprint project state

## Current stage — corrected c71 accepted; first c72 owner run BLOCKED; lifecycle correction quick-test next

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

## C71 measured continuation domain

```text
terminal records = 102
all-successor-exact records = 95
continuable exact architectural edges = 99
inside_unique_runtime_span_nonentry records = 6
executable_file_backed_outside_runtime_metadata records = 1
```

Only the 95/99 exact domain is eligible for c72 continuation. The seven non-exact records remain fail-closed.

## First c72 owner attempt

The owner ran the TOOL READY c72 full historical replay. It reached:

```text
c72 full-replay c71 continuation: 30720/162472
POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED: duplicate live c71 terminal root
```

This run is **BLOCKED**, not accepted c72 evidence.

Static diagnosis: accepted c71 records one first c70 POST per root. After c72 authenticated that first terminal and started continuation, the same root could later expose a distinct c70 POST that was not already represented by the current `tx.post_c71` set. Initial c72 reset the inherited c71 capture and incorrectly attempted to authenticate that later distinct event as a second c71 first terminal for the same root. The duplicate guard correctly stopped the run.

This does not imply duplicate roots in the accepted c71 checkpoint. It is an observer-lifecycle implementation gap.

## C72 lifecycle correction

Added:

```text
source/tools/slot_publication_post_c71_stitched_fix.py
source/tools/test_slot_publication_post_c71_stitched_fix_quick.py
```

Correction rule:

- after successful authentication of the accepted first c71 terminal, retire the inherited c71 first-terminal observer for that root;
- exact sibling callbacks from the same authenticated source instruction remain eligible for the already-preflighted sibling set;
- any later distinct c70 POST becomes the next `uncertified_post_c71_runtime_boundary`;
- do not relabel that later event as current `tx.post_c71` reachability;
- do not change accepted c56-c71 evidence semantics or the 102 / 95 / 99 domain.

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

Run only the dedicated c72 lifecycle quick regression.

If it passes, record the correction as validated and rerun exactly one corrected c72 owner OFFLINE continuation using the fix wrapper.

If it fails, stop and analyze the focused failure. Do not rerun the long owner scan and do not define c73.
