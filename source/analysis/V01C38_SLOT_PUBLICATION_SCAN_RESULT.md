# V01C38 owner offline exact-slot scan result

## Evidence

Owner-supplied compact artifact:

```text
MSM2-WS-v0.1c38-slot-publication.json
```

This is an OFFLINE/STATIC report from the exact on-disk executable. The private executable is not committed or uploaded.

Exact target remains:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
slot RVA = 0xC2D2580
```

## Accepted c38 scan facts

The c38 scanner authenticated the image and used schema 2 / tool v0.1c38:

```text
image_authenticated=1
tool_version=v0.1c38
schema_version=2
scan_scope=bounded_offline_direct_and_register_affine_candidates_not_all_writes
slot readable=1
slot writable=1
live_qword_reads=0
pointer_chase=0
```

No local exact-slot candidate was found in the direct or newly admitted local register-affine classes:

```text
reference_candidates=[]
direct_writer_candidates=0
register_affine_writer_candidates=0
writer_candidates=0
```

This is NOT proof of no reader or no writer. Coverage remains explicitly incomplete:

```text
selected_byte_sweep_complete=0
candidate_sweep_complete=0
writer_enumeration_complete=0
publication_source_kind=unproven
allocation_proven=0
range_proven=0
lifetime_proven=0
base_proven=0
destination_contained=0
CFG_continuation=0
reason=xref_coverage_incomplete_indirect_writers_unexcluded
```

## Actual scanner exercise / limits

The new local register-affine representation did execute on the authenticated real image:

```text
register_affine_resolved_operands=1360
cap_state_instructions=64
cap_affine_hops=8
```

The scan reached the global instruction cap:

```text
cap_instructions=5000000
instructions_decoded=5000000
cap_bytes=67108864
bytes_scanned=17859464
decode_gaps=737
indirect_or_unresolved_operands=1278483
```

Primary unresolved categories:

```text
control_flow_boundary_loss=822382
runtime_region_boundary_loss=208548
unsupported_register_mutation=97883
memory_loaded_base=85282
indexed_address=36092
provenance_window_exhausted=20336
partial_register_mutation=4900
segment_unsupported=1985
decode_gap=901
address_outside_image_or_wrap=171
address_size_unsupported=3
```

State resets also show the intentionally aggressive fail-closed policy:

```text
control_flow_boundary_loss=1993440
runtime_region_boundary_loss=200747
provenance_window_exhausted=778
decode_gap=737
```

The dominant limitation is therefore no longer only direct-vs-register addressing. Control/interprocedural lifetime is intentionally discarded. The `memory_loaded_base` class also cannot be solved by simply extending local affine arithmetic without violating the no-pointer-chase rule.

## Accepted external anchor

The c38 report independently revalidated the accepted c28 reader anchor against the current exact image:

```text
site_rva=0x2CF7FE9
width=8
decoded_access=READ
may_write=0
effective_rva=0xC2D2580
address_proof_class=authenticated_external_anchor
origin_root=16
origin_rva=0xC2D2480
local_scanner_coverage=0
```

This confirms current code plus the already-accepted c28 parent-origin evidence. It remains a read-only external anchor, not a locally discovered candidate, writer, publication source or ownership proof.

## Interpretation

C38 succeeded at its bounded question:

1. the new register-affine representation works on the real authenticated image;
2. it found no local exact-slot writer candidate in the admitted classes;
3. it preserved the known c28 reader as a separate authenticated external anchor;
4. it did not overclaim selected-byte or writer completeness.

The result does NOT justify simply increasing arithmetic/dataflow generality. The largest unresolved class is control-flow boundary loss, while a substantial class depends on memory-loaded bases that must remain unknown under the current non-value rules.

A narrower next evidence class is preferable: observe only the instruction site(s) that actually access the exact static slot, without observing the qword value. The current branch already has a guarded hardware-debug-register watch engine with bounded thread enumeration, lease ownership checks, VEH cleanup and exact-build admission. C39 should investigate reusing that infrastructure for the exact 8-byte slot.

The runtime watch must not read/log the slot contents, pointer-chase, infer an allocation, or treat absence of a writer hit as no-writer proof. A short read/write self-check may use the accepted c28 reader as positive control; writer-site capture should then be write-only if the existing engine can safely re-arm without weakening cleanup/ownership rules.

## Existing movement/store safety remains unchanged

C37 relative offset proof remains:

```text
offset_min=0
offset_max_inclusive=0xFFFC0
stride=64
store_width=32
end_extent_exclusive=0xFFFE0
overflow_wrap_excluded=1
offset_proven=1
```

C36 hard barrier remains:

```text
memory_escape=1
pending_unknown_write=1
store_destination_contained=0
control_alias_unexcluded=1
CFG_blocked=1
continuation_allowed=0
```

Safety gates remain:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Producer `0x351EB00` remains deferred.
