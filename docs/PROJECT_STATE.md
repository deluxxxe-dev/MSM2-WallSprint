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

### c26-c34 accepted target prefix

```text
0x2CF7FE0: cmp r8d,0x3FFF
0x2CF7FE7: ja 0x2CF8013 / fallthrough 0x2CF7FE9
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
0x2CF7FF0: vmovups ymm0,ymmword ptr [rdx]
0x2CF7FF4: movsxd r8,r8d
0x2CF7FF7: shl r8,6
```

Both JA outcomes remain mandatory.

Accepted abstract state before `0x2CF7FFB`:

```text
RAX tags=0x7
R8 tags=0x0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags tags=0x7
```

Important interpretation:

- c30 `RAX=0x7` is a may-depend upper bound from an opaque writable `.data` qword load, not observed taint.
- c32 YMM0 low/high `0x7` is conservative unknown loaded-vector dependence, not observed taint.
- c34 flags `0x7` comes from undefined AF/OF after `shl r8,6`, not observed selected dependence.

## c35 accepted runtime result — second VMOVUPS is a store

Evidence: `analysis/V01C35_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c35-20261026-142344.log`.

Input method was not supplied for this capture. Do not infer device equivalence from motion observations; instruction-level facts below do not depend on input device.

Exact instruction:

```text
0x2CF7FFB: vmovups
bytes=6
encoding=VEX3
opcode=0x11
vector_bits=256
memory_width=32
memory_read=0
memory_write=1
memory_RMW=0
flags_preserved=1
GPR_writes=0
vector_writes=0
SP_write=0
```

Exact decoded operands for the current abstract state:

```text
m256 [R8 + RAX] <- YMM0
base R8 tags=0x0
index RAX tags=0x7
scale=1
displacement=0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
```

Current destination proof:

```text
address_exact=0
class=indexed_address_unproven
address_component_tags=0x7
alias_may_mask=0x7F
```

The alias upper bound includes Hero Transform, `Transform+0x10C`, writer source, caller/home stack, callee-private stack, image and unknown ranges. This is uncertainty, not proof that every alias occurs.

C35 correctly refuses the store:

```text
reason=target_second_VMOVUPS_store_or_RMW_refused
semantic_modeled=0
barrier_rva=0x2CF7FFB
unique_instructions=7
visits=7
return_paths=0
TARGET_IDENTITY admitted=0
```

Do not continue the CFG by treating an unresolved store as side-effect-free. The current parent summary still reports `memory_escape=0` only because the store has not been admitted/modelled; that is not proof of no escape.

## Safety gate

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Completed c35 sessions retained verified native Deinit, retention pass, hard kills 0, MOD Position writes 0, source-position writes 0 and adhesion corrections 0.

The discovered VMOVUPS store is an existing game instruction, not a new MOD write and not permission to write movement/Position data.

Producer `0x351EB00` remains deferred.

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
16. c34 SHL data/flags + continuation ✅
17. c35 second VMOVUPS exact store classification ✅ actual game
18. prove/contain `0x2CF7FFB` store destination/effect ▶ CURRENT
19. resume full writer XYZ/W causal proof ⏸ after store effect is safely bounded
20. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
21. safe writable upstream movement source ⬜ NOT AUTHORIZED
22. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
23. adhesion/contact refinement ⏸ deferred
24. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md`.

C36 must treat `0x2CF7FFB` as an exact game-side `m256 [R8+RAX] <- YMM0` store and answer whether that side effect can be safely contained in the proof.

Do not add generic store semantics. First determine whether the destination can be proven disjoint from current code/control stack and proof-critical movement/Transform storage using existing symbolic/current-parent evidence without guessing raw values.

If containment is not proven, explicitly model/report an unresolved memory escape/barrier and stop. If a narrow store summary is sound, preserve register/vector/flag state, propagate the memory-effect state through branch/return merges, and continue only until the next instruction whose correctness could depend on aliasing with that store.

Any later memory access must prove disjointness from the stored range before using stale memory assumptions.

Preserve both c27 JA paths, current-code/runtime/cap contracts and all ownership/write gates. Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.


## c36 prepared — unresolved exact store/escape is a hard CFG blocker

v0.1c36/package0.1.54 verifies the exact VEX3.256 R8+RAX/YMM0 store and records
32byte/address/value-half may tags, alias/critical masks, escape1/first_escape/
pending under current code proof. Parent READ/CALL no longer hides this as
escape0 solely because identity is refused. Code/control-stack alias cannot be
excluded from opaque RAX/R8 provenance; no continuation or memory map is added.
Registers/vector/flags preserved, clean-path union cannot erase escape, all
current-code/runtime/caps/ownership/write gates remain. New non-value provenance/
region/range/lifetime/disjointness proof is required; identical A/E recapture
only validates summary/safety, not containment. NEXT unchanged. See
V01C36_STORE_CONTAINMENT.md / CODEX_RESULT.md.
