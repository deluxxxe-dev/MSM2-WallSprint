# WallSprint v0.1c13 actual-game runtime result

Diagnostic PASS. Human test used one combined log: A/cardinal WallRun, full exit, fast travel, then E/diagonal WallRun. Sessions remained cleanly separable, so fast travel did not reduce evidence quality.

## Orientation / lifecycle
A/session1: 3875 ms, verified Deinit, source active until Deinit, retention pass, final Up dot surface=1, stale0, hard kill0, Position writes0.

E/session2: 4281 ms, verified Deinit, source active until Deinit, retention pass, final Up dot surface=1, stale0, hard kill0, Position writes0.

## c13 entry evidence
Actual game captured 16 true writer-entry snapshots total (8 A + 8 E) for the verified `0x351EEAB -> 0x2CFC900` Hero Transform call. Every snapshot had `true_entry_snapshot=1`, exact target/code verification, return `0x351EEB0`, observed second return `0x351A020`, and no source mutation.

Every sampled invocation paired to all four Position stores `0x2CFC936/93C/942/945` in the same invocation.

## Strong source candidate
Across all 16 invocations:
```text
RDX = entry RSP + 0x28
candidate20 = [RDX]
candidate23 = stack bytes at RSP+0x28
XYZ exactly match executed Position XYZ store bits
source W=0
Position W=1
exact_store_match_mask=0x7
observed_store_mask=0xF
```
This strongly identifies a caller-local stack payload feeding Position XYZ, but not yet a writable contract. W causality, typed semantics, producer/lifetime, downstream conflicts and exclusive intended translation effect remain unproven.

## Telemetry defect
Final c13 `MOVEMENT_LINEAGE_SUMMARY true_entry_snapshot=0/source_capture_ready=0` contradicts the captured records; candidate stability also stays 0 because aggregation requires W to match. Treat these as reporting defects, not capture failure.

## Gate
`writable_source_proven=0` remains authoritative. No movement/source/input writes are authorized.

Next edge: authenticate the exact RDX XYZ dataflow, independently classify W, and trace construction/lifetime of the `RSP+0x28` source through the caller/observed `0x351A020` producer path. See `codex/v01c14-rdx-source-contract`.
