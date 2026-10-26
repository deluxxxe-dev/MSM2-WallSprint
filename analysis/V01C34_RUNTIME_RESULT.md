# v0.1c34 actual-game runtime result

## Evidence

Owner-supplied private runtime log:

```text
MSM2-WS-v0.1c34-20261026-090013.log
```

Test-environment note from the owner: this capture was performed through a remote environment using keyboard/mouse controls rather than the prior local/controller-style interaction. Treat movement-shape observations as a different input condition. The instruction-level target classification below does not depend on controller-vs-keyboard semantics.

Target identity remains:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

## Accepted SHL result

The c33 barrier at `0x2CF7FF7` is exactly:

```text
0x2CF7FF7: shl r8, 6
bytes=4
encoding=group2_C0_C1_slash4_imm8
destination_bits=64
destination_parent=R8
count_source=immediate
encoded_count=6
count_mask=0x3F
effective_count=6
count_class=multiple_within_width
```

Exact architectural/abstract effects reported by c34:

```text
memory_read=0
memory_write=0
memory_RMW=0
GPR_writes=R8 only
partial_GPR_writes=0
vector_writes=0
SP_write=0
incoming R8 tags=0x0
count tags=0x0
result R8 tags=0x0
semantic_modeled=1
no_value_reads=1
raw_values=0
```

The shift therefore preserves zero dependency tags in R8 without observing a numeric register value.

## Flag effect

For effective count 6:

```text
CF: defined from last shifted-out bit
SF/ZF/PF: defined from result
AF: architecturally undefined
OF: architecturally undefined for count > 1
```

The defined flag dependencies derive from input R8 tags and are zero here. Because the analyzer uses one aggregate flag dependency lattice, undefined AF/OF conservatively inject top:

```text
pre_flags_tags=0x0
defined_flag_may_tags=0x0
post_flags_tags=0x7
aggregate_undefined_top7=1
numerical_flag_values=unknown
```

`0x7` is a may-depend/undefined upper bound, not observed taint.

## New exact barrier

The bounded c27 two-path walk advances one instruction to:

```text
barrier_rva=0x2CF7FFB
barrier_instruction=vmovups
reason=target_cfg_memory_operand_refused
path=0
edge_from_rva=0x2CF7FE7
edge_target_rva=0x2CF7FE9
```

Current state immediately before the new barrier includes:

```text
RAX/GPR0 tags=0x7
R8 tags=0x0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags_may_depend_tags=0x7
```

Target status:

```text
TARGET_IDENTITY admitted=0
LEAF_CFG complete=0
return_paths=0
unique_instructions=7
visits=7
effect_certificate=0
```

Do not reuse the earlier `0x2CF7FF0` VMOVUPS semantics merely because the mnemonic matches. `0x2CF7FFB` is a new site and its exact encoding, width, operands, direction, memory access and alias/provenance remain unproven in c34.

## Runtime safety

Observed completed sessions in this remote/keyboard-mouse capture retain the safety boundary. For example sessions 2 and 4 report verified native Deinit, retention pass, hard kills 0, `Position_writes=0`, `source_position_writes=0`, and `corrections_applied=0`.

No new gameplay translation/source/input write is authorized.

## Decision

Accept c34 as successful bounded progress:

1. pinned SHL form/effective count established;
2. data dependency effect soundly modeled;
3. undefined flag uncertainty retained conservatively;
4. walk advances to exact new blocker `0x2CF7FFB: vmovups`;
5. ownership/writability/effect certificate gates remain closed.

Next task: classify the exact VMOVUPS at `0x2CF7FFB` independently, then reuse existing c32 vector state only if the exact architectural form is compatible.