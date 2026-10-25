# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Repository / branch policy
- Canonical remote: `https://github.com/deluxxxe-dev/MSM2-WallSprint.git`
- Current implementation line: `codex/v01c20-edi-value-provenance`
- Historical organized baseline: `5192f05f32313919fdf4049bd734b971655d163f` (not a pin)
- Continue from the latest intended branch history.
- Do **not** routinely merge `main`; it is heavily diverged.

## Exact target / safety
- Spider-Man2.exe `2.810.0.0`
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve exact-build fail-closed behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation, and no raw private game logs/full proprietary bodies in public artifacts.

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
destination = Hero Transform+0x10C
producer of EDI = 0x2B2356F memory load
same-field read = 0x2B23578: mov EAX,[RCX+0x10C]
0x2B235BE test r8b,r8b -> 0x2B235C1 je is tag-disjoint
0x2B235D4 -> 0x2CF7FE0 carries selected read in raw GPR0 tag1
```

Raw analyzer GPR indices/origins are not typed ABI declarations.

### c24-c25 runtime-function basis

```text
directory_rva=0xC566000
directory_bytes=6325932
entry_bytes=12
derived_entries=527161
metadata_bytes_read=6325932
entries_validated=527161
all_entries_read_and_validated=1
table_structurally_valid=1
runtime_lookup_queried=1
```

Exact target:

```text
target_rva=0x2CF7FE0
runtime_lookup_found=0
next_runtime_rva=0x2CF8020
missing_runtime_entry=1
PE_runtime_table_valid=1
```

The strict pinned no-unwind fallback is therefore legitimate for this exact image.

### c26-c27 prefix

```text
0x2CF7FE0: cmp r8d,0x3FFF
flags-only
R8/flag tags=0

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
full64 RAX pure load
source_rva=0xC2D2580
address_component_tags=0
```

C29 proved the source is writable `.data` / `PAGE_READWRITE`, so immutable/tag0 inference is refused. C30 admits only the exact load as an opaque may-top effect:

```text
RAX tags <- 0x7
precise_value_tags_known=0
observed_taint=0
selected_dependence_killed=0
source_value_reads=0
raw_value_logged=0
origin=unknown_loaded_qword_not_pointer
```

### c31-c32 exact VMOVUPS and 256-bit dependency state

Exact instruction:

```text
0x2CF7FF0: vmovups
encoding=VEX2
VL=256
form=YMM0 <- 32 bytes [RDX]
memory_read=1
memory_write=0
memory_RMW=0
```

C31 established the actual 256-bit form and c32 added only the dependency state required for this pinned load:

```text
YMM0.low128 tags  <- 0x7
YMM0.high128 tags <- 0x7
above256_known_zero=1
represented_bits=256
pinned_YMM0_rule_added=1
generic_YMM_ZMM_rules=0
source_value_reads=0
```

C30 RAX `0x7` is preserved.

## c33 accepted runtime result — exact MOVSXD modeled

Evidence: `analysis/V01C33_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c33-20261025-152750(1).log`.

Exact form:

```text
0x2CF7FF4: movsxd r8,r8d
instruction_bytes=3
encoding=REX_W_opcode63
source_bits=32
destination_bits=64
source_parent=R8
destination_parent=R8
full64_replacement=1
memory_read=0
memory_write=0
memory_RMW=0
flags_preserved=1
SP_write=0
vector_writes=0
```

Dependency effect:

```text
incoming R8 tags=0x0
outgoing R8 tags=0x0
sign_rule=source bit31 replicated to bits63:32
no source numeric interpretation
no value reads
raw values=0
semantic_modeled=1
reason=target_MOVSXD_pinned_reg32_signextend_full64_modeled
```

No stale destination-tag union is needed because this is a full 64-bit architectural replacement.

The bounded walk advances to:

```text
barrier_rva=0x2CF7FF7
barrier_instruction=shl
reason=target_cfg_instruction_effect_refused
path=0
edge_from_rva=0x2CF7FE7
edge_target_rva=0x2CF7FE9
```

Pre-barrier state:

```text
RAX/GPR0 tags=0x7
R8 tags=0x0
other logged GPR tags=0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags_may_depend_tags=0x0
```

Current target state:

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

Do not infer SHL operand width, destination, count source/count masking, exact flags written/undefined, or tag effect from the mnemonic alone.

## Current safety gate

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

C33 runtime safety remained closed across observed sessions: verified native Deinit, retention pass, hard kills 0, MOD Position writes 0, source-position writes 0, adhesion corrections 0.

Producer `0x351EB00` remains deferred. No movement, adhesion, camera, animation or physics advancement is authorized.

## Local workspace / shared Overstrike

Canonical workspace:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\
├─ MSM2-WS-v<version>.script
├─ Update-WS.cmd
├─ logs\
├─ _source\
└─ _archive\
```

Shared ordinary mods + SF + WS, shared Overstrike, modded launch and versioned WS/SF runtime logs are accepted. The prior updater lock was game-root `Check for updates.exe` holding the root `WINMM.dll`; path-validated helper handling remains the accepted repair/launch method.

## Current stages

1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact writer callsite ✅
7. true writer-entry capture ✅
8. RDX stack-local XYZ correlation ✅
9. c14-c19 writer/callee chain ✅
10. c20 EDI provenance ✅ actual game
11. c21 same-field read ✅ actual game
12. c22 TEST/JE disjointness ✅ actual game
13. c23 direct nested target/carrier ✅ actual game
14. c24 old PE-table cap diagnosed ✅ actual game
15. c25 full table validation + genuine no-entry ✅ actual game
16. c26 exact CMP effect ✅ actual game
17. c27 exact JA edges + bounded two-path fan-out ✅ actual game
18. c28 exact MOV form/address provenance ✅ actual game
19. c29 writable `.data` tag0 route refused ✅ actual game
20. c30 exact qword unknown-top abstraction + continuation ✅ actual game
21. c31 exact 256-bit VMOVUPS classification ✅ actual game
22. c32 pinned 256-bit vector dependency state + VMOVUPS continuation ✅ actual game
23. c33 exact MOVSXD register sign-extension + continuation ✅ actual game
24. classify exact `0x2CF7FF7 SHL` operand/count/flag effect ▶ CURRENT
25. resume full writer XYZ/W causal proof ⏸ after local effect classification
26. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
27. safe writable upstream movement source ⬜ NOT AUTHORIZED
28. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
29. adhesion/contact refinement ⏸ deferred
30. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md`.

The next bounded question is the exact architectural form/effect of `0x2CF7FF7: shl`.

Classify exact encoding, destination operand/width, count source and x86 count-masking behavior, memory involvement, GPR write width, and all defined/undefined flag effects. Only add a pinned semantic rule if the existing dependency model can conservatively represent both the data result and relevant flags without numerical guessing.

Preserve c30 RAX `0x7`, c32 YMM0 low/high `0x7`, `above256_known_zero=1`, c33 R8 state, both c27 JA paths, runtime/cap/RET constraints, and all ownership/write gates.

Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.

## c34 prepared — exact SHL count/data/flag classification

v0.1c34/package0.1.52 adds pinned SHL0x2CF7FF7 diagnostics and conditional
canonical register32/64 constant-count semantics. Mask63/31 is width-derived;
count0 keeps operand/flags (register32 parent still zero-extends), nonzero
counts retain destination dependence and undefined AF/OF yield aggregate7.
CL/partial/SP/memory RMW refuse, no live numerical count/value observations.
C30 RAX7/c32 low-high7/abovezero/c33 remain unless a legitimate destination
write replaces that register. Both JA legs/RET/current-code/runtime/caps and
all ownership/write gates remain. Actual SHL form/next barrier UNKNOWN, minimum
SHL/FLAGS/OPERAND/ADDRESS plus next-barrier/CFG/state/safety A/E capture pending.
NEXT unchanged. See analysis/V01C34_SHL_FLAGS.md and CODEX_RESULT.md.
