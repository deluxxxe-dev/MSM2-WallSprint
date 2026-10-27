# c53 owner offline real-image result — authenticated CFG scan, no local exact-slot candidate, incomplete coverage

Date: 2026-10-27 JST
Branch: `codex/v01c20-edi-value-provenance`
Scanner repository head before owner run: `337f95378c03c9ba7be2712a5125e0d039f9d2d5`
Evidence class: `VERIFIED-STATIC` (owner offline scan of exact authenticated on-disk image)

## Primary classification

The owner ran the published c53 offline CFG-aware scanner against the exact pinned executable. The scan completed and emitted schema1/tool v0.1c53 JSON.

```text
repository_result = CFG_SLOT_PUBLICATION_SCANNER_READY
image_authenticated = true
image_sha256 = c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
slot_rva = 0xC2D2580
hardware_watch_enabled = false
game_used = false
production_integrated = false
live_qword_reads = 0
pointer_chase = false
```

The real image produced no local exact-slot candidate in the admitted CFG/address-provenance language:

```text
reference_candidates = []
reader_candidates = 0
writer_candidates = 0
rmw_candidates = 0
address_only_candidates = 0
```

This is **not** proof that no writer exists. All completeness/publication/ownership gates remain false:

```text
candidate_sweep_complete = false
writer_enumeration_complete = false
publication_source_kind = unproven
base_proven = false
allocation_proven = false
range_proven = false
lifetime_proven = false
ownership_exclusive = false
destination_contained = false
CFG_continuation = false
alternate_entries_EH_indirect_writers_closed = false
```

Reason reported by the scanner:

```text
bounded_normal_CFG_candidates_not_writer_or_publication_completeness
```

## Accepted external reader remains separate

The accepted c28 reader was revalidated independently and remained outside local CFG coverage:

```text
site_rva = 0x2CF7FE9
origin_rva = 0xC2D2480
effective_rva = 0xC2D2580
operation = READ
may_write = false
address_proof_class = authenticated_external_anchor
local_scanner_coverage = false
```

It remains reader evidence only; it does not seed the new CFG analysis or prove a writer/publication source.

## Real-image coverage

The scanner materially reduced the old c38 control-flow blind spot, but did not close it.

```text
runtime_functions_in_metadata = 527161
runtime_functions_not_considered = 352663
regions_considered = 174498
regions_modeled = 65455
regions_refused = 109043
instructions_decoded = 5000000
blocks_traversed = 332865
edges_traversed = 3716504
states_discovered = 3687652
worklist_iterations = 1795614
unique_joins = 57703
state_revisits = 313561
loop_back_edges = 8119
previously_control_lost_edges_safely_modeled = 10471
control_edges_without_agreed_origin = 179106
```

The global instruction cap was reached:

```text
global_instructions_cap = 1
instructions_decoded = cap 5000000
```

Therefore the candidate sweep cannot be promoted to complete.

## Dominant remaining unresolved address-provenance classes

Total unresolved memory operands:

```text
560919
```

Main categories:

```text
call_clobber_all_address_origins = 324714
register_base_without_exact_origin = 118219
unsupported_register_mutation = 62079
memory_loaded_base = 20287
cfg_merge_unknown = 16749
indexed_address = 14775
partial_register_mutation = 1838
segment_unsupported = 1852
cfg_merge_conflict = 391
address_outside_image_or_wrap = 15
```

Refused control regions include:

```text
indirect_call = 38801
indirect_jump = 1014
system_or_unsupported_transfer = 10
unsupported_runtime_boundary = 68899
```

The largest new bounded blind spot is now `call_clobber_all_address_origins`: c53 intentionally invalidates every register-origin fact across every direct CALL because it makes no ABI or callee-preservation assumption.

## Deferred/safety boundaries preserved

The deferred producer remained excluded before decode:

```text
deferred_producer_rva = 0x351EB00
excluded_deferred_regions = [[0x351EB00, 0x351EB17]]
```

No slot qword value was read or logged, no pointer chase occurred, hardware watch remained disabled, and c52 production-watch FAIL remains binding.

## Interpretation

The c53 owner scan answers the bounded question successfully:

1. the exact image is authenticated;
2. the CFG must-analysis runs on the real image and safely models 10,471 control edges that the c38 local scanner would have discarded;
3. no local exact-slot candidate appears in the admitted language;
4. the scan remains materially incomplete because of global caps, unconsidered runtime functions, refused regions and unresolved provenance classes.

The correct conclusion is therefore **no local candidate observed under current bounded CFG provenance**, not **no writer exists**.

## Recommended next bounded evidence class

A narrower c54 should target the dominant `call_clobber_all_address_origins` class with conservative **static direct-callee preservation summaries** only.

Any summary must be proven from authenticated code and must fail closed unless the complete callee/control-flow domain is bounded. No ABI preservation assumption is allowed. A safe first form is a per-register preservation summary that may preserve an incoming exact address origin across a direct CALL only when the callee and any summarized nested direct callees are fully decoded/closed under strict caps, all reachable exits are normal returns, and that register is proven never written on any reachable path. Indirect/system/unresolved control, recursion/cycles beyond the bounded summary contract, decode gaps, cap exhaustion, unsupported register effects, or any write to the register must kill the fact.

Even if this finds a writer later, publication/allocation/base/lifetime/ownership/containment remain separate unproven questions.
