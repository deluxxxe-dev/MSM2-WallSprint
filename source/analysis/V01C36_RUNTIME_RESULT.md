# V01C36 runtime result — unresolved exact store remains a hard CFG barrier

Source: owner-supplied `MSM2-WS-v0.1c36-20261026-150111.log`.

## Target identity

The exact admitted build remains unchanged:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
target_build_gate=ACCEPTED
```

## Accepted prefix remains intact

The c26-c35 target prefix remains the same:

```text
0x2CF7FE0: cmp r8d,0x3FFF
0x2CF7FE7: ja 0x2CF8013 / fallthrough 0x2CF7FE9
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
0x2CF7FF0: vmovups ymm0,ymmword ptr [rdx]
0x2CF7FF4: movsxd r8,r8d
0x2CF7FF7: shl r8,6
0x2CF7FFB: vmovups ymmword ptr [r8+rax],ymm0
```

The fallthrough-path pre-store state remains:

```text
RAX tags=0x7
R8 tags=0x0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags tags=0x7
```

These are conservative dependency upper bounds, not observed taint.

## c36 exact store-effect result

C36 verifies the exact existing game-side store and records it as a potential memory escape:

```text
site_rva=0x2CF7FFB
exact_store_identity=1
current_proof=1
memory_write_seen=1
memory_write_width=32
memory_address_tags=0x7
memory_value_low128_tags=0x7
memory_value_high128_tags=0x7
memory_alias_may_mask=0x7F
critical_alias_may_mask=0x3FF
memory_escape=1
first_escape_rva=0x2CF7FFB
pending_unknown_write=1
store_destination_contained=0
control_alias_unexcluded=1
CFG_blocked=1
continuation_allowed=0
later_memory_facts_usable=0
architectural_registers_preserved=1
scope=potential_game_instruction_not_MOD_write
reason=target_store_destination_code_or_control_stack_alias_unexcluded
```

The critical may-alias mask conservatively includes current code/runtime data, control/return stack, caller-home and callee-private stack, Hero/Transform state, `Transform+0x10C`, writer source, Position-related storage and other later memory.

This is uncertainty, not evidence that the store actually overlaps all of those categories.

## Parent propagation corrected

The nested target is still not admitted and no effect certificate is issued, but c36 correctly propagates the unresolved memory side effect before the identity refusal:

```text
MOVEMENT_CALLEE_READ_CALL:
memory_escape=1
first_escape_rva=0x2CF7FFB
pending_unknown_write=1
barrier_rva=0x2CF7FFB
reason=target_store_destination_code_or_control_stack_alias_unexcluded

MOVEMENT_CALLEE_READ_FLOW:
memory_escape=1
first_escape_rva=0x2CF7FFB
first_unresolved_rva=0x2CF7FFB
first_unresolved_instruction=vmovups
writable_source_proven=0
```

Therefore the prior c35 parent `memory_escape=0` was only an artifact of refusing the store before summarizing it; it was not proof that no escape existed.

## CFG / identity result

The bounded target still stops at the store:

```text
TARGET_IDENTITY admitted=0
barrier_rva=0x2CF7FFB
barrier_instruction=vmovups
reason=target_store_destination_code_or_control_stack_alias_unexcluded

LEAF_CFG complete=0
return_paths=0
unique_instructions=7
visits=7
final_code_rechecked=0
effect_certificate=0
```

No next opcode is inferred or manufactured.

The clean JA leg cannot erase the unresolved store effect on the fallthrough leg; both paths remain mandatory for any future complete proof.

## Safety result

Observed c36 sessions remained clean with respect to MOD translation writes:

```text
Position_writes=0
source_position_writes=0
corrections_applied=0
verified_deinit=1
retention_pass=1
first_hard_kill=none
hard_kill_calls=0
```

The VMOVUPS store is an existing game instruction, not a MOD write and not movement-write authorization.

All movement ownership/write gates remain closed:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Producer `0x351EB00` remains deferred.

## Interpretation / next proof class

Repeating the same A/E capture cannot resolve containment. The next useful work must add genuinely new non-value evidence for the two address components of:

```text
[R8 + RAX]
```

Two separable questions are now visible:

1. **R8 offset range on the JA fallthrough path.** Because the path is the unsigned non-JA outcome of `cmp r8d,0x3FFF`, there may be enough authenticated control/data semantics to derive a bounded numeric interval for R8 through `movsxd r8,r8d` and `shl r8,6` without reading a live register value. This must be proved path-sensitively, not assumed from the mnemonic sequence.
2. **RAX base provenance/publication.** RAX comes from the exact writable-image qword load at `[RCX+0x100]` / source RVA `0xC2D2580`. Containment requires non-value proof of what publishes that slot, its allocation/range/lifetime contract, and its disjointness from proof-critical regions. Zero tags, stable sessions, or merely reading a numeric pointer are not ownership/lifetime proof.

Until both components are sufficiently bounded, `0x2CF7FFB` remains the hard CFG barrier.