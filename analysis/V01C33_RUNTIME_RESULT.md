# v0.1c33 actual-game runtime result

Accepted owner-supplied log: `MSM2-WS-v0.1c33-20261025-152750(1).log`.

## Exact build

```text
Spider-Man2.exe version=2.810.0.0
SHA256=c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
SDK=SM2ScriptTemplate 999d0c7437720be5378049455209e69447f4a930
```

## Accepted c33 MOVSXD result

The c32 vector state remains accepted:

```text
0x2CF7FF0 VMOVUPS
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
RAX tags=0x7
```

The next instruction is now exactly classified and modeled:

```text
site_rva=0x2CF7FF4
instruction=movsxd
bytes=3
encoding=REX_W_opcode63
operands=2
source=r8d
source_bits=32
source_parent=R8
source_parent_may_tags=0x0
destination=r8
destination_bits=64
destination_parent=R8
full64_replacement=1
register_form_supported=1
memory_read=0
memory_write=0
memory_RMW=0
flags_preserved=1
SP_write=0
vector_writes=0
semantic_modeled=1
sign_rule=source_bit31_replicated_to_destination63_32
result_may_tags=0x0
no_source_numeric_interpretation=1
no_value_reads=1
raw_values=0
reason=target_MOVSXD_pinned_reg32_signextend_full64_modeled
```

No memory-address semantics are involved for the actual form. The diagnostic address record correctly remains unclassified because the actual source is a register, not memory.

This is a full 64-bit replacement of R8 from the sign-extended R8D value. Since incoming R8 tags are zero, outgoing R8 tags remain zero. No stale destination-tag union is required.

## New exact barrier

The bounded walk advances one instruction to:

```text
barrier_rva=0x2CF7FF7
barrier_instruction=shl
reason=target_cfg_instruction_effect_refused
path=0
edge_from_rva=0x2CF7FE7
edge_target_rva=0x2CF7FE9
```

Pre-barrier dependency state retains:

```text
RAX/GPR0 tags=0x7
R8 tags=0x0
other logged GPR tags=0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags_may_depend_tags=0x0
```

Current target status:

```text
TARGET_IDENTITY admitted=0
instructions=6
LEAF_CFG complete=0
return_paths=0
unique_instructions=6
visits=6
final_code_rechecked=0
effect_certificate=0
first_unresolved_rva=0x2CF7FF7
first_unresolved_instruction=shl
```

The c33 log does not establish the exact SHL operands, shift-count source, operand width, destination register, count semantics, or flag dependency/effect. Do not infer them from the mnemonic.

## Runtime safety

Observed WallRun sessions remained inside the existing safety boundary:

```text
Position_writes=0
source_position_writes=0
adhesion corrections_applied=0
verified_deinit=1
retention_pass=1
hard_kill_calls=0
```

No movement/source/input/adhesion/camera/animation/physics write permission is added.

## Decision

c33 is accepted for the bounded question. The exact `MOVSXD r8,r8d` effect is modeled soundly and the next blocker is `0x2CF7FF7: shl`.

Next work should classify the exact SHL form and only model it if operand-width, count semantics, destination dependency propagation, and flag effects can be represented exactly under the existing lattice. Preserve c30 RAX top7, c32 YMM0 low/high top7, both c27 JA legs, all runtime caps, and all ownership/write gates.
