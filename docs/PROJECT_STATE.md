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
first possible use=0x2B23581
first control dependence=0x2B23583
memory_escape=0
```

Disjoint TEST/JE:

```text
0x2B235BE test r8b,r8b
R8 tags=0
flags tags=0
0x2B235C1 je consumer tags=0
```

Nested direct call:

```text
0x2B235D4 -> 0x2CF7FE0
selected read reaches boundary in raw GPR0 tag1
```

Raw analyzer GPR indices/origins are not typed ABI declarations.

### c24-c25 runtime-function table

C25 fully validated the authenticated runtime-function table after c24 exposed an obsolete cap:

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

Therefore the strict pinned no-unwind fallback is legitimate for this exact image.

### c26-c27 control prefix

```text
0x2CF7FE0: cmp r8d,0x3FFF
flags-only
R8 selected-related tags=0
CMP flag tags=0

0x2CF7FE7: ja
taken=0x2CF8013
fallthrough=0x2CF7FE9
both exact, forward, in allowance, no runtime crossing
numerical_outcome=unknown
```

Both outcomes remain mandatory.

### c28-c30 exact qword load

Exact instruction:

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
full64 RAX pure load
flags/SP/other registers unchanged
source_rva=0xC2D2580
address_component_tags=0
alias=image only
```

C29 proved the source is writable `.data` / `PAGE_READWRITE`, so the non-writable/tag0 route is refused before reading the qword.

C30 then admits only this exact load as a conservative abstract unknown effect:

```text
RAX tags <- 0x7
precise_value_tags_known=0
observed_taint=0
selected_dependence_killed=0
source_value_reads=0
raw_value_logged=0
origin=unknown_loaded_qword_not_pointer
```

`0x7` is a may-depend upper bound, not observed taint.

### c31 exact VMOVUPS classification

Evidence: `analysis/V01C31_RUNTIME_RESULT.md`.

```text
0x2CF7FF0: vmovups
encoding=VEX2
VL=256
form=YMM0 <- 32 bytes [RDX]
memory_read=1
memory_write=0
memory_RMW=0
flags_preserved=1
GPR_writes=0
SP_write=0
```

For the analyzed parent state:

```text
base=RDX
index=none
displacement=0
base_tags=0
address_exact=1
class=Hero_Transform_conditional
alias_may_mask=0x1
source_value_reads=0
```

C31 had only a 128-bit vector dependency representation, so the complete YMM destination effect was not representable and it correctly refused.

## c32 accepted runtime result — exact 256-bit dependency model and continuation

Evidence: `analysis/V01C32_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c32-20261025-145028.log`.

The c31 instruction is now semantically modeled under a pinned 256-bit dependency representation:

```text
site=0x2CF7FF0
instruction=vmovups
semantic_load_modeled=1
reason=target_VMOVUPS_exact_YMM0_unknown_halves_modeled
represented_bits=256
represented_banks=16
pinned_YMM0_rule_added=1
generic_YMM_ZMM_rules=0
```

Exact abstract destination effect:

```text
YMM0.low128 tags  <- 0x7
YMM0.high128 tags <- 0x7
above256_known_zero=1
```

The source remains the exact c31 Hero-root `[RDX]` address with address-component tags zero. No vector bytes are read or logged; `0x7` remains a may-depend upper bound, not observed taint.

C30 RAX state is preserved:

```text
RAX tags=0x7
origin=unknown_loaded_qword_not_pointer
```

Pre-next-barrier state:

```text
RAX/GPR0 tags=0x7
other logged GPR tags=0
XMM0/low128 tags=0x7
YMM0/high128 tags=0x7
above256_known_zero=1
flags tags=0
```

The bounded walk advances one instruction to the new exact blocker:

```text
barrier_rva=0x2CF7FF4
barrier_instruction=movsxd
reason=target_cfg_instruction_effect_refused
path=0
edge_from_rva=0x2CF7FE7
edge_target_rva=0x2CF7FE9
```

Current target state:

```text
TARGET_IDENTITY admitted=0
instructions=5
LEAF_CFG complete=0
return_paths=0
unique_instructions=5
visits=5
effect_certificate=0
```

The c32 log does not establish MOVSXD operands, source/destination register, memory use, width, address expression, or exact tag effect. Do not infer these from the mnemonic alone.

## Current safety gate

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

C32 runtime safety remained closed across observed sessions: verified native Deinit, retention pass, hard kills 0, MOD Position writes 0, source-position writes 0, adhesion corrections 0.

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

The prior Overstrike failure was traced to game-root `Check for updates.exe` loading game-root `WINMM.dll`. WS helpers path-validate and stop that updater during launch/repair; missing game-root `winmm.dll` is recoverable `.script support` repair state. Do not stop unrelated same-named processes.

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
23. classify exact `0x2CF7FF4 MOVSXD` operand/effect ▶ CURRENT
24. resume full writer XYZ/W causal proof ⏸ after local effect classification
25. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
26. safe writable upstream movement source ⬜ NOT AUTHORIZED
27. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
28. adhesion/contact refinement ⏸ deferred
29. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md`.

The next bounded question is the exact architectural form/effect of `0x2CF7FF4: movsxd`.

First classify exact instruction length/encoding, source and destination operands, source width, whether memory is involved, memory address/access if present, sign-extension semantics, destination GPR full/partial-write behavior, incoming tags/origins, flags/SP/memory effects, and current-code stability.

If the exact form is representable by the existing GPR dependency model, add only the pinned semantics necessary for that form and continue to the next unsupported instruction. Any unproven memory effect/address, unsupported partial write, stack change, call, branch, runtime crossing, snapshot mismatch or cap exhaustion remains fail-closed.

Preserve c30 RAX `0x7`, c32 YMM0 low/high `0x7`, `above256_known_zero=1`, both c27 JA paths, all ownership gates, and zero gameplay write permission.

Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.
