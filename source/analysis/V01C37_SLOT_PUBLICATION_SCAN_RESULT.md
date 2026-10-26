# V01C37 owner offline exact-slot scan result

## Evidence

Owner-supplied compact output:

```text
MSM2-WS-v0.1c37-slot-publication.json
```

The report was produced locally by the shipped `source/tools/slot_publication.py` against the exact on-disk game executable. No executable bytes, private paths, live qword value, or pointer chase are included here.

## VERIFIED-STATIC/OFFLINE facts

The scanner authenticated the exact supported image and exact slot:

```text
slot_rva=0xC2D2580
image_authenticated=1
slot readable=1
slot writable=1
scan_scope=bounded_offline_direct_candidates_not_all_writes
```

The direct-only candidate sweep returned:

```text
reference_candidates=[]
direct_writer_candidates=0
candidate_sweep_complete=0
writer_enumeration_complete=0
publication_source_kind=unproven
allocation_proven=0
range_proven=0
lifetime_proven=0
base_proven=0
destination_contained=0
CFG_continuation=0
live_qword_reads=0
pointer_chase=0
reason=xref_coverage_incomplete_indirect_writers_unexcluded
```

Coverage diagnostics:

```text
cap_bytes=67108864
cap_instructions=5000000
bytes_scanned=17859460
decode_gaps=737
indirect_or_unresolved_operands=1279672
```

The deferred runtime region containing producer `0x351EB00` remained excluded as designed.

## Interpretation

`direct_writer_candidates=0` is **not** a no-writer proof. The scan itself says candidate coverage and writer enumeration are incomplete, with 1,279,672 register-based/indexed/otherwise unresolved memory operands and 737 decode gaps.

This limitation is concrete rather than hypothetical: accepted c28 evidence already proves an exact slot consumer at:

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
```

For the authenticated parent state c28 proves:

```text
RCX base_origin_root=16
base_origin_offset=0xC2D2480
displacement=0x100
effective source RVA=0xC2D2580
address_expression_exact=1
address_class=image_address_mutability_unproven
```

The c37 Python scanner intentionally rejects register-base memory operands unless their address is directly RIP-relative/absolute, so the known exact consumer is outside its direct-only candidate class. Therefore the zero-candidate result cannot narrow publication/writer existence.

## Next bounded question

The next useful step is not another WallRun capture and not a live qword read. Extend only the offline/static candidate discovery needed to resolve **exact register-affine image-address memory operands** when their address provenance can be established without dereferencing memory values.

The implementation should remain candidate discovery, not ownership proof. It may support narrowly bounded exact address-producing forms such as RIP-relative address materialization and checked full-width register/affine propagation, but must invalidate provenance at unsupported/partial writes and must never treat a memory-loaded register as a pointer merely because it is later used as an address.

The already-proven `0x2CF7FE9` consumer is the regression anchor: the new representation should be able to express why this access resolves to the slot from authenticated existing provenance, or explicitly document the remaining boundary if the offline tool cannot reproduce that interprocedural parent state. Do not hard-code a fake all-code/all-writer claim to make the anchor pass.

If new register-affine candidates are found, classify only site/width/access/address-proof/source-kind metadata and follow a publication chain only as far as exact static provenance remains sound. If no candidates are found, retain the precise unresolved classes and fail closed.

## Safety / unchanged gates

The accepted c37 R8 offset proof remains:

```text
offset=0..0xFFFC0 inclusive
stride=64
32-byte endExtentExclusive=0xFFFE0
offset_proven=1
```

The base half remains unresolved, so c36 remains in force:

```text
memory_escape=1
pending_unknown_write=1
store_destination_contained=0
control_alias_unexcluded=1
CFG_blocked=1
continuation_allowed=0
```

And gameplay/write authorization remains closed:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Do not traverse deferred producer `0x351EB00`, do not merge `main`, and do not infer allocation/pool/array/owner semantics from an address pattern.