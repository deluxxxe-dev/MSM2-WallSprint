# V01C35 runtime result — second VMOVUPS is an unresolved 256-bit store

## Source

Owner-supplied private runtime log:

```text
MSM2-WS-v0.1c35-20261026-142344.log
```

Target identity remains Spider-Man2.exe `2.810.0.0`, SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`.

Input method was not supplied for this capture. Do not infer controller/keyboard equivalence from motion observations; instruction-level facts below do not require such an inference.

## Accepted c35 result

The c34 barrier at `0x2CF7FFB` is exactly classified as a **store**, not another load:

```text
site_rva=0x2CF7FFB
instruction=vmovups
bytes=6
encoding=VEX3
opcode=0x11
vector_bits=256
memory_width=32
memory_read=0
memory_write=1
memory_RMW=0
direction_verified=1
pure_load_form=0
flags_preserved=1
GPR_writes=0
vector_writes=0
SP_write=0
snapshot_stable=1
current_code_rechecked=1
semantic_modeled=0
reason=target_second_VMOVUPS_store_or_RMW_refused
```

Exact operands in the current abstract parent state:

```text
operand0 = memory [R8 + RAX]
operand0 width=32 bytes
base=R8
base tags=0x0
index=RAX
index tags=0x7
scale=1
displacement=0

operand1 = YMM0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
```

The store preserves architectural vector state; it does not write a vector register.

## Address proof status

The effective address is not proven exact:

```text
address_exact=0
class=indexed_address_unproven
address_component_tags=0x7
alias_may_mask=0x7F
```

The current alias upper bound includes Hero Transform, `Transform+0x10C`, writer source, caller/home stack, callee-private stack, image and unknown ranges. This is an upper bound from unresolved address provenance, not evidence that all aliases occur.

The unresolved component is the c30 `RAX` state. `RAX tags=0x7` remains a conservative may-depend upper bound from the opaque writable-image qword load, not observed selected taint.

Because the destination is unresolved and may include control-/stack-relevant storage, c35 correctly refuses the store. Do not continue the static CFG by pretending that an unknown store has no side effect.

## Current bounded target state

```text
barrier_rva=0x2CF7FFB
barrier_instruction=vmovups
reason=target_second_VMOVUPS_store_or_RMW_refused
path=0
unique_instructions=7
visits=7
return_paths=0
TARGET_IDENTITY admitted=0
```

Pre-barrier state remains:

```text
RAX tags=0x7
R8 tags=0x0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags tags=0x7
```

The parent flow therefore remains unresolved at this site. `memory_escape` is not yet asserted because the store has not been admitted into the effect model; current records still show `memory_escape=0` and `first_escape_rva=0` while the barrier is unresolved.

## Safety

Observed completed sessions retain:

```text
Position_writes=0
source_position_writes=0
corrections_applied=0
verified_deinit=1
retention_pass=1
first_hard_kill=none
hard_kill_calls=0
```

The VMOVUPS store is an existing game instruction discovered by analysis. It is not a new MOD write and does not authorize gameplay/source-position writes.

## Decision / next question

C35 is accepted as a successful diagnostic classification, but not as target continuation.

The next bounded problem is **store-destination/effect containment**, not generic VMOVUPS support:

1. preserve the exact store form `m256 [R8+RAX] <- YMM0`;
2. determine whether the destination can be proven disjoint from current code/control stack and other proof-critical storage using existing symbolic/current-parent evidence without guessing values;
3. if such containment cannot be proven, explicitly summarize the instruction as an unresolved memory escape/barrier rather than silently continuing;
4. only if the complete memory side effect is safely representable may the CFG walk proceed, and any later memory access must respect aliasing against the stored range;
5. do not read/log raw vector or memory values merely to force continuation, do not add generic store semantics, and do not weaken ownership gates.

Producer `0x351EB00` remains deferred. All movement/source/input/adhesion/camera/animation/physics writes remain unauthorized.
