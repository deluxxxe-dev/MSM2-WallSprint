# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, camera, animation or physics until runtime ownership gates authorize them.

## Repository / branch policy
- Canonical remote: `https://github.com/deluxxxe-dev/MSM2-WallSprint.git`
- Current implementation line: `codex/v01c20-edi-value-provenance`
- Historical organized baseline: `5192f05f32313919fdf4049bd734b971655d163f` (not a pin)
- Continue from latest intended branch history.
- Do **not** routinely merge `main`; it is heavily diverged.

## Exact target
- Spider-Man2.exe `2.810.0.0`
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`

## Verified movement chain

```text
writer=0x2CFC900..0x2CFC9B7
stores=0x2CFC936/0x2CFC93C/0x2CFC942/0x2CFC945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
exact callsite=0x351EEAB -> 0x2CFC900
return=0x351EEB0
observed second return=0x351A020
```

Runtime source relation remains `RDX=entry RSP+0x28`; sampled `[RDX] XYZ` equals exact executed Position XYZ bits. This remains correlation until writer/effect/ownership proof completes.

## Accepted reverse-engineering chain

### c19-c23

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
RCX = writer-entry Hero Transform
producer of EDI = 0x2B2356F memory load
same-field read = 0x2B23578: mov EAX,[RCX+0x10C]
0x2B235BE test r8b,r8b -> 0x2B235C1 je tag-disjoint
0x2B235D4 -> 0x2CF7FE0 carries selected read in raw GPR0 tag1
```

Raw analyzer GPR indices/origins are not typed ABI declarations.

### c24-c25 runtime-function basis

```text
directory_rva=0xC566000
directory_bytes=6325932
entry_bytes=12
derived_entries=527161
entries_validated=527161
all_entries_read_and_validated=1
table_structurally_valid=1
runtime_lookup_queried=1
target_rva=0x2CF7FE0
runtime_lookup_found=0
next_runtime_rva=0x2CF8020
missing_runtime_entry=1
PE_runtime_table_valid=1
```

The strict pinned no-unwind fallback is legitimate for this exact image.

### c26-c27 control prefix

```text
0x2CF7FE0: cmp r8d,0x3FFF
flags-only; R8/flag tags=0

0x2CF7FE7: ja
taken=0x2CF8013
fallthrough=0x2CF7FE9
both exact, forward, in allowance, no runtime crossing
numerical_outcome=unknown
```

Both outcomes remain mandatory.

### c28-c30 qword load

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
```

Source is writable `.data` / `PAGE_READWRITE`; immutable/tag0 inference is refused. Exact load is modeled only as opaque may-top:

```text
RAX tags <- 0x7
precise_value_tags_known=0
observed_taint=0
selected_dependence_killed=0
source_value_reads=0
origin=unknown_loaded_qword_not_pointer
```

### c31-c32 first VMOVUPS

```text
0x2CF7FF0: vmovups ymm0, ymmword ptr [rdx]
VEX2.256
memory pure load
```

Pinned 256-bit dependency state:

```text
YMM0.low128 tags  <- 0x7
YMM0.high128 tags <- 0x7
above256_known_zero=1
generic_YMM_ZMM_rules=0
source_value_reads=0
```

### c33 MOVSXD

```text
0x2CF7FF4: movsxd r8,r8d
32->64 sign extension
full64 replacement
memory/flags/SP/vector effects=0
incoming R8 tags=0
outgoing R8 tags=0
```

### c34 accepted runtime result — SHL modeled

Evidence: `analysis/V01C34_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c34-20261026-090013.log`.

Test environment: remote environment with keyboard/mouse controls. Treat movement-shape observations as a separate input condition from prior controller/local captures. Instruction-level classification and safety gates below are not inferred from controller semantics.

Exact instruction:

```text
0x2CF7FF7: shl r8, 6
bytes=4
encoding=group2_C0_C1_slash4_imm8
destination_bits=64
count_source=immediate
encoded_count=6
count_mask=0x3F
effective_count=6
```

Dependency/data effect:

```text
incoming R8 tags=0x0
count tags=0x0
result R8 tags=0x0
memory_read/write/RMW=0
vector_writes=0
SP_write=0
semantic_modeled=1
no_value_reads=1
```

Flag effect for count 6:

```text
CF defined from last shifted-out bit
SF/ZF/PF defined from result
AF undefined
OF undefined for count>1
defined_flag_may_tags=0x0
pre_flags_tags=0x0
post_flags_tags=0x7
aggregate_undefined_top7=1
```

`post_flags_tags=0x7` is conservative undefined-state top, not observed taint.

The bounded walk advances to a **new** site:

```text
barrier_rva=0x2CF7FFB
barrier_instruction=vmovups
reason=target_cfg_memory_operand_refused
path=0
unique_instructions=7
visits=7
return_paths=0
```

Pre-barrier abstract state:

```text
RAX/GPR0 tags=0x7
R8 tags=0x0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags_may_depend_tags=0x7
```

Do **not** reuse the earlier `0x2CF7FF0` VMOVUPS rule by mnemonic alone. `0x2CF7FFB` requires independent exact encoding/operand/direction/memory classification.

## Safety gate

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Completed c34 sessions in the remote/keyboard-mouse capture retained verified native Deinit, retention pass, hard kills 0, MOD Position writes 0, source-position writes 0, and adhesion corrections 0.

Producer `0x351EB00` remains deferred. No movement-source write is authorized.

## Current stages

1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact writer callsite ✅
7. true writer-entry capture ✅
8. RDX stack-local XYZ correlation ✅
9. c14-c23 writer/callee/provenance chain ✅
10. c24-c25 runtime table/no-unwind basis ✅
11. c26 CMP ✅
12. c27 JA two-path fan-out ✅
13. c28-c30 qword load + unknown-top continuation ✅
14. c31-c32 first VMOVUPS + 256-bit state ✅
15. c33 MOVSXD ✅
16. c34 SHL data/flags + continuation ✅ actual game
17. classify exact `0x2CF7FFB VMOVUPS` ▶ CURRENT
18. resume full writer XYZ/W causal proof ⏸ after local effect classification
19. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
20. safe writable upstream movement source ⬜ NOT AUTHORIZED
21. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
22. adhesion/contact refinement ⏸ deferred
23. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md`.

C35 must classify `0x2CF7FFB: vmovups` independently: exact encoding/vector width, source/destination operands, load/store direction, memory width/access, address/provenance/aliases, vector parent/write width, mask/broadcast/rounding/upper-lane semantics, incoming tags and current-code stability.

Only if the exact form is soundly representable by the existing c32 low/high128/above256 model may a pinned rule be added and the walk continue. Store/RMW, unsupported partial/masked/wider effects, unresolved memory, stack/call/extra branch/runtime crossing/snapshot mismatch remain fail-closed.

Preserve RAX `0x7`, R8 `0`, YMM0 low/high `0x7`, above256 zero knowledge, flags `0x7`, both c27 JA paths and all ownership/write gates.

Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.

## c35 prepared — second VMOVUPS direction/address/upper-state contract

v0.1c35/package0.1.53 classifies only VMOVUPS0x2CF7FFB independently. Conditional
canonical VEX2 full XMM/YMM load from current STILL-proven Hero RDX+0 can reuse
c32 dependency state: low7, high0 for128 or7 for256, above256zero. Prior MOVSXD/
SHL replacing source origin invalidates stale entry-Hero permission even at tags0.
Stores/RMW/dynamic/EVEX/masked/wider/other forms refuse. Flags7/RAX7/R8zero and
unrelated parents remain. No values/new hooks/producer/gameplay writes or safety
relaxation. Both JA outcomes/current-code/runtime/caps/RET retained; actual form/
next barrier UNKNOWN pending minimum VECTOR2 metadata A/E capture, input method
noted. NEXT unchanged. See V01C35_SECOND_VMOVUPS.md / CODEX_RESULT.md.
