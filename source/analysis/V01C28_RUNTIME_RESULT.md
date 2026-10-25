# V01C28 actual runtime result — exact MOV form and image-address provenance

## Evidence

Owner-supplied private runtime log:

```text
MSM2-WS-v0.1c28-20261025-122112.log
```

Exact admitted game build remains:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

This document records compact facts only. Do not publish the private raw log or reconstruct the surrounding proprietary target body.

## Accepted exact MOV classification

The c27 fallthrough barrier at `0x2CF7FE9` is now classified:

```text
site_rva=0x2CF7FE9
instruction=mov
instruction_bytes=7
operand_count=2
address_width=8
memory_operands=1
memory_read=1
memory_write=0
memory_RMW=0
memory_access_known=1
pure_load_form=1
flags_preserved=1
full_GPR_writes=0x1
partial_GPR_writes=0
XMM_writes=0
other_register_writes=0
SP_write=0
snapshot_stable=1
semantic_load_modeled=0
```

Exact operands:

```text
operand0: GPR RAX, width=8, write
operand1: memory [RCX+0x100], width=8, read
index=none
scale=1
segment=none
```

Thus the exact decoded form is equivalently:

```text
mov rax, qword ptr [rcx+0x100]
```

This is a full 64-bit GPR overwrite and a pure memory read. It performs no memory write/RMW, no SP mutation, and preserves flags.

## Parent-state address provenance

For the analyzed parent state, the memory base is exact and tag-disjoint:

```text
base=RCX
base_tags=0
base_origin_kind=pointer/affine owned origin
base_origin_root=16
base_origin_offset=204285056
displacement=256
address_expression_exact=1
same_address_all_samples=1
address_root=16
address_offset=204285312
address_component_tags=0
```

The effective address is classified only as:

```text
address_class=image_address_mutability_unproven
alias_may_mask=0x20   # image only
image_range_known=1
current_code_range_known=0
```

This excludes the tracked Hero/Transform+0x10C/source/caller-home/private-stack classes for this analyzed state, but does not prove that the loaded value is immutable or semantically untainted.

## Loaded value remains unresolved

The important c28 boundary is now exact:

```text
readonly_value_proof=not_implemented
loaded_value_tags_known=0
loaded_may_tags_upper_bound=0x7
loaded_value_observed=0
unknown_not_untainted=1
semantics_admitted=0
reason=target_MOV_loaded_value_provenance_unproven
```

Incoming RAX carries selected tag1 before the MOV. Because the MOV is a full 64-bit write, a future admitted load model would replace that abstract RAX value. However c28 does not yet have a sound rule to assign the loaded qword's tags. Therefore it is not valid to kill selected tag1 merely because the source address lies inside the image.

Address provenance and loaded-value provenance remain separate.

## Target / parent status

The target remains unadmitted:

```text
TARGET_IDENTITY admitted=0
barrier_rva=0x2CF7FE9
barrier_instruction=mov
reason=target_MOV_loaded_value_provenance_unproven
LEAF_CFG complete=0
return_paths=0
unique_instructions=3
visits=3
final_code_rechecked=0
effect_certificate=0
```

The parent direct CALL likewise remains unmodeled and preserves the pre-existing conservative selected carrier:

```text
site_rva=0x2B235D4
target_rva=0x2CF7FE0
modeled_states=0
barrier_rva=0x2CF7FE9
barrier_instruction=mov
reason=target_MOV_loaded_value_provenance_unproven
GPR0 tags=0x1
memory_escape=0
```

The accepted c27 JA fan-out remains valid and unchanged:

```text
0x2CF7FE7 JA
taken=0x2CF8013
fallthrough=0x2CF7FE9
both forward/in allowance/no runtime crossing
control_expanded=1
numerical_outcome=unknown
```

## Safety

Two WallRun sessions retained the existing safety boundary:

```text
verified native Deinit=1
retention_pass=1
hard_kill_calls=0
Position_writes=0
source_position_writes=0
adhesion corrections=0
position_proof_admitted=0
```

No movement/source/input/Position write permission is gained.

## Accepted interpretation / next exact question

C28 succeeds at classification and narrows the blocker from generic memory to one exact operation:

> Can the exact image-backed qword at the proven `root16 + 204285312` address be given a sound current-proof loaded-value provenance (for example by proving a non-writable authenticated image section/page plus bounded stable-value snapshot semantics), without treating an image address as automatically immutable and without broadening to a generic memory emulator?

Only if that exact source can receive a justified tag classification may `mov rax,[rcx+0x100]` be modeled. If admitted, RAX is a full-width overwrite, flags remain unchanged, and the existing c27 two-path all-RET proof may resume. Otherwise the exact MOV remains the fail-closed barrier.

Producer `0x351EB00` remains deferred. All ownership/lifetime/exclusivity/writability gates remain zero.
