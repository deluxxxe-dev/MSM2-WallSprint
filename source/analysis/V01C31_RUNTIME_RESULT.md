# v0.1c31 actual-game runtime result

Source owner log: `MSM2-WS-v0.1c31-20261025-141246.log`.

## Build identity

Accepted exact build remains unchanged:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

## Accepted c30 continuation remains intact

The exact c30 qword load at `0x2CF7FE9` remains modeled without reading its value:

```text
RAX tags <- 0x7
precise_value_tags_known=0
observed_taint=0
selected_dependence_killed=0
source_value_reads=0
raw_value_logged=0
```

## Exact c31 VMOVUPS classification

The next barrier is now classified exactly:

```text
site_rva=0x2CF7FF0
instruction=vmovups
instruction_bytes=4
encoding=VEX2
vector_bits=256
encoded_bits=256
operands=2
address_bits=8
memory_read=1
memory_write=0
memory_RMW=0
pure_load_form=1
flags_preserved=1
GPR_writes=0
partial_GPR_writes=0
vector_writes=YMM0
SP_write=0
snapshot_stable=1
```

Exact operand form:

```text
YMM0 <- 32 bytes [RDX]
```

Destination:

```text
kind=YMM
register=ymm0
width=32
parent=0
write=1
```

Memory source:

```text
width=32
base=RDX
index=none
scale=1
displacement=0
segment=none
base_tags=0
base_origin_root=1
base_origin_offset=0
```

Current parent-state address classification:

```text
address_exact=1
same_address_all_samples=1
class=Hero_Transform_conditional
root=1
offset=0
component_tags=0
alias_may_mask=0x1   # Hero class only
loaded_may_top=0x7
loaded_tags_known=0
source_value_reads=0
pointer_chase=0
```

## Architectural vector-state blocker

The exact load is not refused because it is a store or RMW. It is refused because the current analyzer does not represent the complete architectural destination state:

```text
decoded_vector_bits=256
represented_bits=128
represented_banks=16
upper_state_represented=0
upper_effect=zero_above_encoded_VL
upper_zero=1
upper_preserved=0
YMM_ZMM_upper_lane_rule_added=0
semantic_load_modeled=0
reason=target_VMOVUPS_upper_vector_state_not_represented
```

Therefore c31 correctly stops at `0x2CF7FF0` rather than silently collapsing a 256-bit YMM write into the existing 128-bit XMM abstraction.

The pre-instruction path state remains conservative:

```text
path=0
RAX/GPR0 tags=0x7
other logged GPR tags=0
logged XMM tags=0
flags_may_depend_tags=0
```

Target status:

```text
TARGET_IDENTITY admitted=0
barrier_rva=0x2CF7FF0
barrier_instruction=vmovups
reason=target_VMOVUPS_upper_vector_state_not_represented
LEAF_CFG complete=0
return_paths=0
unique_instructions=4
visits=4
effect_certificate=0
```

## Safety result

Observed sessions remain read-only for movement/Position proof:

```text
source_position_writes=0
Position_writes=0
adhesion corrections_applied=0
verified_deinit=1
retention_pass=1
first_hard_kill=none
hard_kill_calls=0
```

No ownership/writability permission changes.

## Accepted interpretation

C31 establishes a reusable negative condition: an XMM-only dependency abstraction must not claim sound semantics for a 256-bit YMM destination write when upper-lane dependency state is unrepresented.

The next step is not generic AVX emulation. It is a narrow c32 representation extension sufficient for this exact `YMM0 <- m256 [RDX]` load, with conservative `0x7` dependence for both 128-bit halves and explicit architectural handling of zero-above-VL. Any future vector width or form outside the newly represented state remains fail-closed.
