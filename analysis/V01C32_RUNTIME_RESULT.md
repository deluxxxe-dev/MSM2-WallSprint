# v0.1c32 actual runtime result

Source evidence supplied by owner:

```text
MSM2-WS-v0.1c32-20261025-145028.log
```

Exact build remains accepted:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
SDK SM2ScriptTemplate 999d0c7437720be5378049455209e69447f4a930
```

## Accepted c32 result

The exact c31 instruction remains:

```text
0x2CF7FF0: vmovups
VEX2
VL=256
YMM0 <- 32 bytes [RDX]
memory_read=1
memory_write=0
memory_RMW=0
flags_preserved=1
SP_write=0
```

C32 extends only the dependency representation required for this exact instruction:

```text
represented_bits=256
represented_banks=16
YMM0.low128 may tags  <- 0x7
YMM0.high128 may tags <- 0x7
above256_known_zero=1
```

The source address remains exact for the analyzed parent state:

```text
base=RDX
index=none
displacement=0
address component tags=0
class=Hero_Transform_conditional
alias mask=Hero only
```

No vector bytes are read or logged. `0x7` is a may-depend upper bound, not observed taint.

The instruction is now semantically modeled:

```text
semantic_load_modeled=1
reason=target_VMOVUPS_exact_YMM0_unknown_halves_modeled
pinned_YMM0_rule_added=1
generic_YMM_ZMM_rules=0
```

C30 state is preserved:

```text
RAX tags=0x7
origin=unknown_loaded_qword_not_pointer
```

## New exact blocker

The bounded walk advances one instruction to:

```text
barrier_rva=0x2CF7FF4
barrier_instruction=movsxd
reason=target_cfg_instruction_effect_refused
path=0
edge_from_rva=0x2CF7FE7
edge_target_rva=0x2CF7FE9
```

Pre-barrier state:

```text
GPR0/RAX tags=0x7
other logged GPR tags=0
XMM0 low128 tags=0x7
YMM0 high128 tags=0x7
above256_known_zero=1
flags tags=0
```

Target remains incomplete:

```text
TARGET_IDENTITY admitted=0
instructions=5
LEAF_CFG complete=0
return_paths=0
unique_instructions=5
visits=5
effect_certificate=0
```

The c32 log does not establish MOVSXD operands, source/destination registers, whether a memory operand is present, access width, address expression, or exact dependency effect. Do not infer them from the mnemonic.

## Safety

Observed WallRun sessions remain closed:

```text
Position_writes=0
source_position_writes=0
adhesion corrections_applied=0
verified native Deinit=1
retention_pass=1
hard_kill_calls=0
```

No Position/source/input/movement/adhesion/camera/animation/physics write permission follows from c32.

## Next bounded question

Classify exact `0x2CF7FF4: movsxd` before adding semantics. Determine exact operands/access/width, memory direction/address if any, sign-extension destination effect, GPR partial/full-write semantics, tags/origins, flags/SP/memory effects and current-code stability. If representable under existing GPR tag state, model only the exact pinned form and continue to the next unsupported effect; otherwise fail closed.
