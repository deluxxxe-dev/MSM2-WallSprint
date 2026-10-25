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

## c19–c23 accepted facts

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
RCX = writer-entry Hero Transform
destination = Hero Transform+0x10C
```

EDI reaching definition:

```text
producer=0x2B2356F
kind=memory_load
root=analyzer globalRoot16
offset=0x0C323788
reaching definitions=1
value provenance exact=1
```

Earlier same-field read:

```text
0x2B23578: mov EAX,[RCX+0x10C]
address=Hero Transform+0x10C
first possible use=0x2B23581
first control dependence=0x2B23583
memory_escape=0
```

c22 exact TEST/JE is disjoint from the tracked selected read:

```text
0x2B235BE test r8b,r8b
R8 tags=0
flags tags=0
0x2B235C1 je consumer tags=0
```

c23 exact nested call:

```text
0x2B235D4 -> 0x2CF7FE0
exact_target=1
selected read reaches boundary in raw GPR0 tag1
```

Raw analyzer GPR indices/origins are not typed ABI declarations.

## c24/c25 accepted runtime-table result

C24 hit an old 262144-entry cap. C25 fully validated the actual authenticated table under finite budgets:

```text
directory_rva=0xC566000
directory_bytes=6325932
entry_bytes=12
derived_entries=527161
metadata_byte_cap=8388608
entry_cap=600000
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
runtime_lookup_same_image=1
runtime_lookup_readable=1
next_runtime_rva=0x2CF8020
missing_runtime_entry=1
PE_runtime_table_valid=1
```

Therefore the strict pinned no-unwind fallback is the active method.

## c26 accepted runtime result — exact CMP

```text
0x2CF7FE0: cmp r8d,0x3FFF
has_memory_operand=0
flags_only=1
GPR_XMM_memory_writes=0
flags_written=CF_PF_AF_ZF_SF_OF
R8 tags=0
CMP flags tags=0
```

## c27 accepted runtime result — exact JA fan-out

```text
0x2CF7FE7: ja
encoding=short_JA_rel8
displacement=42
taken=0x2CF8013
fallthrough=0x2CF7FE9
both forward/in code/in allowance
no runtime-region crossing
control_expanded=1
numerical_outcome=unknown
```

Both outcomes are explored; no numerical branch direction is chosen.

## c28 accepted runtime result — exact MOV classified

Evidence: `analysis/V01C28_RUNTIME_RESULT.md`.

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
instruction_bytes=7
memory_read=1
memory_write=0
memory_RMW=0
pure_load_form=1
flags_preserved=1
full_GPR_write=RAX
SP_write=0
snapshot_stable=1
```

Exact source for the analyzed parent state:

```text
source_rva=0xC2D2580
address_component_tags=0
alias=image only
```

## c29 accepted runtime result — tag0 image proof refused

Evidence: `analysis/V01C29_RUNTIME_RESULT.md`.

The exact source is writable:

```text
section=.data
section_characteristics=0xC0000040
readable=1
writable=1
executable=0
current page=PAGE_READWRITE
```

Therefore the narrow non-writable/tag0 rule correctly refuses before reading the qword:

```text
read_attempts=0
admitted=0
loaded_tags_known=0
raw_value_logged=0
reason=image_load_section_not_readonly
```

This disproves only the tag0/immutable-source route. It does not prove actual taint.

## c30 accepted runtime result — exact unknown-top load modeled

Evidence: `analysis/V01C30_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c30-20261025-133648.log`.

C30 admits only the exact c28/c29 full64 load as a conservative abstract unknown effect:

```text
site_rva=0x2CF7FE9
source_rva=0xC2D2580
width=8
abstract_effect_admitted=1
loaded_may_depend_top=0x7
precise_value_tags_known=0
observed_taint=0
selected_dependence_killed=0
source_value_reads=0
raw_value_logged=0
numeric_interpretation=0
pointer_chase=0
origin=unknown_loaded_qword_not_pointer
flags/other GPR/XMM/SP/memory preserved
```

`0x7` is a may-depend upper bound over selected/postwrite/other-prewrite, not an observation that all categories are present. It keeps selected dependence possible while avoiding any qword value read.

The exact MOV now reports:

```text
semantic_load_modeled=1
reason=target_MOV_exact_source_unknown_may_top_modeled
```

Pre-next-barrier state on the fallthrough path:

```text
RAX/GPR0 tags=0x7
other logged GPR tags=0
logged XMM tags=0
flags_may_depend_tags=0
upper_bound_not_observed=1
```

The walk advances one instruction to the new exact barrier:

```text
barrier_rva=0x2CF7FF0
barrier_instruction=vmovups
reason=target_cfg_memory_operand_refused
path=0
edge_from_rva=0x2CF7FE7
edge_target_rva=0x2CF7FE9
```

Current target state:

```text
TARGET_IDENTITY admitted=0
instructions=4
LEAF_CFG complete=0
return_paths=0
unique_instructions=4
visits=4
final_code_rechecked=0
effect_certificate=0
```

Do not infer VMOVUPS operand classes, vector width, memory direction, address expression, alias class, destination vector register, or AVX upper-lane behavior from the mnemonic/refusal alone.

## Current safety gate

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

C30 runtime safety remained closed in observed WallRun sessions: verified native Deinit, retention pass, hard kills 0, MOD Position writes 0, source-position writes 0, adhesion corrections 0.

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

Shared ordinary mods + SF + WS, shared Overstrike, modded launch and versioned WS/SF runtime logs are accepted.

The prior Overstrike failure was traced to game-root `Check for updates.exe` loading game-root `WINMM.dll`. WS helpers now path-validate and stop that updater during launch/repair, and missing game-root `winmm.dll` is treated as recoverable `.script support` repair state. Do not stop unrelated same-named processes.

## Current stages

1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact writer callsite ✅
7. true writer-entry capture ✅
8. RDX stack-local XYZ correlation ✅
9. c14–c19 writer/callee chain ✅
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
21. classify exact `0x2CF7FF0 VMOVUPS` operand/effect ▶ CURRENT
22. resume full writer XYZ/W causal proof ⏸ after local effect classification
23. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
24. safe writable upstream movement source ⬜ NOT AUTHORIZED
25. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
26. adhesion/contact refinement ⏸ deferred
27. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md`.

The exact next question is the form/effect of `0x2CF7FF0: vmovups`. Classify vector width, register/memory direction, exact operands, effective address/alias classes, and VEX/EVEX upper-lane semantics before adding any new vector-memory rule.

If it is a memory store/RMW or the existing vector tag representation cannot soundly model the architectural destination effect, refuse. If it is an exact pure load and the complete vector overwrite/upper-lane semantics are representable, a narrowly pinned unknown-value may-top rule may be considered without reading/logging the vector contents.

Do not narrow c30's RAX `0x7`, do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.

## c31 prepared — exact VMOVUPS diagnostic

v0.1c31/package0.1.49 classifies only VMOVUPS0x2CF7FF0. Actual operands/width/
direction/address/upper state remain UNKNOWN pending compact VECTOR records.
No new vector semantics: represented XMM128 does not cover whole VEX/YMM/ZMM
upper state, and stores/RMW/masks/merge/modifiers remain refused. Decoder-store
READ discrepancy is reported with encoding/layout-verified direction. Current
parent replay uses saved bytes/owned state only, no data/vector read. C30 RAX7/
no qword reads and all all-path/cap/runtime/ownership/write gates unchanged.
NEXT preserved. Minimal A~3s/end/move/E~3s vector metadata capture required;
analysis/V01C31_VMOVUPS_CLASSIFICATION.md details decision/tests/limits.
