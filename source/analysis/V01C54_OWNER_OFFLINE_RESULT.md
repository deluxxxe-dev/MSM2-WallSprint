# V01C54 owner offline real-image result

Date: 2026-10-27 JST
Branch: `codex/v01c20-edi-value-provenance`
Evidence class: `VERIFIED-STATIC`
Source artifact: `MSM2-WS-v0.1c54-slot-publication-callee.json`
Artifact SHA-256: `a774df7fb0d409ef3fb7364509247c9193e0e9172fb1662a713424fdf9b2c935`

## Result

The owner executed the published c54 offline scanner against the exact pinned image. The image authenticated successfully:

- `image_authenticated = true`
- `image_sha256 = c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- `slot_rva = 0xC2D2580`
- `repository_result = CALLEE_PRESERVATION_SLOT_SCANNER_READY`
- `evidence = OFFLINE_STATIC`
- `game_used = false`
- `hardware_watch_enabled = false`
- `production_integrated = false`
- `live_qword_reads = 0`
- `pointer_chase = false`

The bounded direct-callee preservation extension produced no admitted real-image summaries:

- `callee_summaries_attempted = 24411`
- `callee_summaries_admitted = 0`
- `callee_summaries_refused = 24411`
- `summary_requests = 44483`
- `summary_cache_hits = 20072`
- `summary_functions_loaded = 1872`
- `summary_instructions_decoded = 74061`
- `summary_work = 602704`
- `summary_cycles_refused = 157`
- `global_summary_budget_exhausted = false`
- `nested_summaries_used = 0`

Accordingly, c54 did not reduce the c53 CALL-clobber blind spot:

- `c53_call_clobber_all_address_origins_baseline = 324714`
- `call_clobber_all_address_origins_remaining = 324714`
- `call_edges_with_any_preserved_origin = 0`
- `preserved_origin_register_facts_across_calls = 0`

The dominant summary refusal is exact callee-entry authentication failure:

- `target_not_exact_authenticated_runtime_entry = 22538` (92.3% of all c54 summary refusals)
- all refusal reasons whose chain contains `target_not_exact_authenticated_runtime_entry` total `23141` (94.8%)

Other direct top-level refusal counts include:

- `unsupported_runtime_boundary = 336`
- `indirect_call = 353`
- `callee_control_cycle = 157`
- `unsupported_instruction_effect = 46`
- `unsupported_callee_memory_addressing = 32`
- `callee_decode_gap = 9`
- `indirect_jump = 7`
- `callee_instruction_cap = 2`
- `callee_depth_cap = 1`

Nested refusal chains are also present and remain fail-closed.

## Candidate / completeness outcome

No local exact-slot candidate was found:

- `reference_candidates = []`
- `reader_candidates = 0`
- `writer_candidates = 0`
- `rmw_candidates = 0`
- `address_only_candidates = 0`

This remains an incomplete zero-observation, not no-writer proof:

- `candidate_sweep_complete = false`
- `writer_enumeration_complete = false`
- `publication_source_kind = unproven`
- `base_proven = false`
- `allocation_proven = false`
- `range_proven = false`
- `lifetime_proven = false`
- `ownership_exclusive = false`
- `destination_contained = false`
- `CFG_continuation = false`
- `alternate_entries_EH_indirect_writers_closed = false`

The main scan again hit the global instruction cap at 5,000,000 decoded instructions. Coverage remains the same c53-scale incomplete sweep: 174,498 regions considered, 65,455 modeled, 109,043 refused, 352,663 runtime functions not considered, and 560,919 unresolved memory operands.

The existing authenticated external c28 reader remains separate and does not count as local CFG coverage.

## Interpretation

VERIFIED-STATIC: c54's never-written direct-callee preservation contract is fail-closed on the real image, but under the current exact-runtime-entry admission rule it admits no summaries at all. Therefore it provides no real-image provenance improvement over c53.

The next bounded static question is not to weaken preservation semantics or assume the ABI. It is to explain and, if possible, safely narrow `target_not_exact_authenticated_runtime_entry` by reconciling authenticated direct CALL targets with runtime-function metadata and/or a separate exact-direct-call-entry normal-CFG proof. Any later work must preserve exact-image authentication, reject ambiguity/overlap, keep indirect/EH closure unsupported, and must not convert metadata-near or heuristic targets into authenticated entries without explicit proof.

C52 production-watch FAIL remains binding. No game/runtime/watch/slot-value/producer traversal or production integration is authorized by this result.
