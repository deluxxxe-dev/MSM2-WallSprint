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

This exact CMP is admitted as a read-only flags effect for the analyzed state.

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

Evidence: `analysis/V01C28_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c28-20261025-122112.log`.

Exact fallthrough instruction:

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
instruction_bytes=7
memory_read=1
memory_write=0
memory_RMW=0
pure_load_form=1
flags_preserved=1
full_GPR_write=RAX
partial_GPR_writes=0
XMM_writes=0
SP_write=0
snapshot_stable=1
```

Parent-state source address is exact:

```text
base=RCX
base_tags=0
base_origin_root=16
base_origin_offset=204285056
displacement=256
address_expression_exact=1
same_address_all_samples=1
address_root=16
address_offset=204285312
address_component_tags=0
address_class=image_address_mutability_unproven
alias_may_mask=0x20
image_range_known=1
```

This proves the address class, not loaded-value provenance. Incoming RAX carries selected tag1 before this full-width MOV.

## c29 accepted runtime result — image source is writable, tag0 proof refused

Evidence: `analysis/V01C29_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c29-20261025-130401.log`.

The exact source qword is:

```text
source_rva=0xC2D2580
width=8
```

Authenticated PE section metadata:

```text
section=.data
section_begin_rva=0x9B24000
section_end_rva=0xC565A28
section_characteristics=0xC0000040
readable=1
writable=1
executable=0
loader_ranges_checked=1
loader_overlap=0
policy_passed=0
reason=image_load_section_not_readonly
```

Current mapped page metadata likewise shows a writable image mapping:

```text
allocation_base_matches_image=1
range_in_image=1
region_begin_rva=0xC2D2000
region_bytes=2703360
state=0x1000
 type=0x1000000
protection=0x4  # PAGE_READWRITE
```

Therefore the c29 non-writable/current-stable rule correctly refuses before reading the qword:

```text
read_attempts=0
admitted=0
loaded_tags_known=0
raw_value_logged=0
pointer_chase=0
reason=image_load_section_not_readonly
```

This disproves only the narrow **tag0 via immutable image source** route. It does not prove that the qword actually carries any tracked tag. The sound conservative loaded-value upper bound remains `0x7`.

Target and parent CALL remain fail-closed at the same instruction:

```text
TARGET_IDENTITY admitted=0
barrier_rva=0x2CF7FE9
barrier_instruction=mov
LEAF_CFG complete=0
return_paths=0
unique_instructions=3
visits=3
effect_certificate=0
reason=image_load_section_not_readonly
```

## Current safety gate

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

C29 runtime safety remained closed in the observed WallRun sessions: verified native Deinit, retention pass, hard kills 0, MOD Position writes 0, source-position writes 0, adhesion corrections 0.

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
19. c29 non-writable/tag0 image-load route refused by writable `.data` + PAGE_READWRITE ✅ actual game
20. model exact pinned load conservatively as unknown tag upper bound and continue ▶ CURRENT
21. resume full writer XYZ/W causal proof ⏸ after local effect classification
22. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
23. safe writable upstream movement source ⬜ NOT AUTHORIZED
24. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
25. adhesion/contact refinement ⏸ deferred
26. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md`.

Do **not** retry the c29 immutable-image proof against this exact source: the authenticated `.data` section and current page are writable, so tag0 is not justified.

The next bounded question is whether this **one exact already-classified full64 load** can be admitted only as a conservative abstract effect:

```text
RAX tags <- 0x7   # may-depend upper bound, not observed taint
RAX origin <- unknown loaded value from exact source
all other GPR/XMM tags unchanged
flags unchanged
memory unchanged
SP unchanged
```

No raw qword read/value interpretation is needed. Because `0x7` includes selected tag1, this does **not** claim that the prior selected dependence is killed; it merely avoids inventing precision while allowing the existing bounded c27 two-path proof to advance to the next exact unsupported effect.

Do not generalize this rule to arbitrary memory loads, do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.
