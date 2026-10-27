# c58 owner offline stitched scan result

2026-10-27 JST. Evidence class: **VERIFIED-STATIC**. Exact pinned image authenticated:
`c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`.
Tool/schema: `v0.1c58` / schema1. Repository result:
`EXACT_RUNTIME_BEGIN_STITCH_SCANNER_READY`.

## Accepted real-image facts

- accepted c57 boundary identity: `b990e0d13ed735418fda2669a8e54fec4f7e0b5647d846a4fbaf99eb80639158`
- c56 checkpoints and c57 JSON were reused read-only.
- `cross_runtime_fact_propagation=true`, scoped only to completed offline entry instances.
- exact-runtime-begin edges available: 159734
- considered: 147338 (92.24% of available)
- admitted: 1299 (0.81% of available; 0.88% of considered)
- refused: 146039
- continuation instances created/deduplicated/refused: 6808 / 11 / 5499
- cross-row cap refusals: 34; cycles seen: 0
- rows reached by continuation: 482
- address facts crossing runtime rows: 1686
- CALL edges with preserved facts crossing rows: 0
- new cross-row exact-slot candidates: 0
- all reader/writer/RMW/address-only candidate counts remain 0
- historical c56 candidate counts remain unchanged.

Root transactions: 162472 attempted, 81896 completed (50.41%), 80576 refused. Dominant root refusal reasons:

- `uncertified_later_runtime_boundary`: 33719 (41.85% of refused roots)
- `inside_unique_runtime_span_nonentry`: 29818 (37.01%)
- `executable_file_backed_outside_runtime_metadata`: 12526 (15.55%)
- `indirect_call`: 4330 (5.37%)
- smaller: indirect_jump119, system_or_unsupported_transfer27, summary_cap_refused31, decode_gap3, instruction_cap3.

`remaining_boundary_refusal_relation_counts` are attempted-cross-edge counts inside aborted roots, not a complete writer domain: exact unique runtime begin67515, interior31384, metadata-free RX18680.

## Interpretation

This is positive mechanism evidence: existing c53 image-address Facts can survive authenticated runtime-row boundaries without being globalized to the target row. It is not writer/publication proof. No new exact-slot candidate was observed.

The dominant newly exposed blocker is not the first c57 boundary anymore. It is a **later runtime boundary reached from a c58 continuation instance that lacks an independently authenticated continuation-edge certificate**. c58 intentionally refuses such edges as `uncertified_later_runtime_boundary`.

Do not infer that those 33719 roots are safe to continue. First classify the exact later-boundary edge kinds/target relations and bind them to source row, incoming-state identity and immutable instruction identity. Reuse c57 edge semantics but do not assume that a c57 first-boundary record covers a later entry-instance boundary.

## Safety boundaries retained

`candidate_sweep_complete=false`, `writer_enumeration_complete=false`, `publication_source_kind=unproven`, allocation/base/range/lifetime/ownership/destination/CFG/EH gates false. `live_qword_reads=0`, `pointer_chase=false`, hardware watch/game/production integration false. C52 production VCH review FAIL remains binding. Producer traversal remains excluded.

## Recommended next task

c59 should be diagnostic-only: classify every `uncertified_later_runtime_boundary` encountered by c58 completed/aborted entry-instance replay, with exact source instance identity, architectural edge kind and target metadata relation. No new stitching in c59. This mirrors the c57 classify-before-admit pattern and avoids silently generalizing first-boundary evidence to later continuation contexts.
