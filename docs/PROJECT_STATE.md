# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Repository / branch policy
- Canonical remote: `https://github.com/deluxxxe-dev/MSM2-WallSprint.git`
- Current implementation line: `codex/v01c20-edi-value-provenance`
- Owner-confirmed organized baseline: `5192f05f32313919fdf4049bd734b971655d163f` (historical baseline, not a pin)
- Continue from later commits on this branch.
- `main` is heavily diverged and must **not** be routinely merged into this branch.

## Exact target / safety
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- WallRun Init `0x14233C0`, Entry `0x14235C0`, Update `0x1424530`, Deinit `0x1416660`
- Preserve exact-build fail-closed behavior, `AGENTS.md` / `docs/DEV_RULES.md`, independent implementation, and no raw user logs/full private game bodies in public artifacts.

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

## c19–c21 accepted facts

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
RCX provenance = writer-entry Hero Transform
destination = Hero Transform+0x10C
```

EDI local reaching definition:

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

Semantic meaning/type remains unknown.

## c22 accepted result — TEST/JE disjoint

```text
0x2B235BE: test r8b,r8b
parent R8 selected/same-field tags=0x0
flags_may_depend_tags=0x0
0x2B235C1: je
consumer_may_depend_tags=0x0
returned_flags_tags=0x0
```

This exact TEST/JE pair is not a carrier of the selected same-field read for the analyzed state.

## c23 accepted result — exact nested direct CALL

```text
0x2B235D4: direct call
target_rva=0x2CF7FE0
exact_target=1
```

Selected `Transform+0x10C` dependence reaches this boundary in raw GPR carrier index 0 (`gpr_tags=0x1`, mem32 Hero Transform+0x10C). This is not a typed ABI declaration.

## c24/c25 accepted runtime-table result

C24 observed 527161 derived PE runtime-function entries against an old 262144-entry diagnostic cap. C25 resolved that ambiguity by validating the complete authenticated table under finite budgets:

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

Therefore the strict pinned no-unwind fallback legitimately starts.

## c26 accepted actual runtime result — exact CMP

Evidence: `analysis/V01C26_RUNTIME_RESULT.md`.

```text
0x2CF7FE0: cmp r8d, 0x3FFF
operand_count=2
has_memory_operand=0
flags_only=1
form_supported=1
GPR_XMM_memory_writes=0
flags_written=CF_PF_AF_ZF_SF_OF
R8 parent_may_depend_tags=0x0
CMP flags_may_depend_tags=0x0
```

Thus the narrow read-only CMP model is accepted for the analyzed state.

The next instruction is:

```text
0x2CF7FE7: ja
condition=CF0_and_ZF0
flags_read=CF_ZF
flags_producer_rva=0x2CF7FE0
consumer_may_depend_tags=0x0
numerical_outcome=unknown
```

## c27 accepted actual runtime result — JA fan-out crossed, MOV memory barrier

Evidence: `analysis/V01C27_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c27-20261025-113718.log`.

The exact pinned JA is now classified and safely expanded without choosing a numerical outcome:

```text
site_rva=0x2CF7FE7
encoding=short_JA_rel8
instruction_bytes=2
displacement=42
edges_exact=1
taken_rva=0x2CF8013
fallthrough_rva=0x2CF7FE9
taken_in_code=1
fallthrough_in_code=1
taken_in_allowance=1
fallthrough_in_allowance=1
taken_forward=1
fallthrough_forward=1
taken_runtime_cross=0
fallthrough_runtime_cross=0
snapshot_stable=1
control_expanded=1
numerical_outcome=unknown
outcome_narrowing=0
rule=pinned_JA_both_forward_paths
```

The all-path proof remains incomplete because the fallthrough leg immediately reaches a memory operand that the current strict leaf model refuses:

```text
barrier_rva=0x2CF7FE9
barrier_instruction=mov
reason=target_cfg_memory_operand_refused
```

Bounded target CFG state:

```text
complete=0
return_paths=0
unique_instructions=3
visits=3
branch_cap=1
path_cap=2
byte_cap=128
instruction_cap=32
visit_cap=64
all_paths_required=1
final_code_rechecked=0
effect_certificate=0
```

Parent selected-read flow therefore advances to:

```text
first_unresolved_rva=0x2CF7FE9
first_unresolved_instruction=mov
returned_gpr_may_depend_mask=0x1
memory_escape=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
writable_source_proven=0
reason=target_cfg_memory_operand_refused
```

Important interpretation:

- c27 proves exact JA geometry and the bounded both-leg fan-out mechanism.
- It does **not** authenticate one leg independently; `return_paths=0` and all-path completion is still required.
- The log does not establish the MOV's exact operands, width, memory read/write direction, address expression, memory region, alias class, or loaded-value provenance.
- Do not infer those facts from `mov` or `target_cfg_memory_operand_refused`.
- The next task is exact MOV classification first; no generic memory model is authorized.

## Current safety gate

```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```

C27 runtime safety remained closed across two WallRun sessions: verified native Deinit, retention pass, hard kills 0, MOD Position writes 0, source-position writes 0, adhesion corrections 0.

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

A 2026-10-25 local failure was traced to game-root `Check for updates.exe` (Overstrike Updater, Tkachov) loading the game-root proxy `WINMM.dll`, preventing Overstrike from deleting/replacing it during `.script support` installation. The WS helper now includes path-validated updater shutdown / launch-time prevention logic. `winmm.dll` absence is treated as a recoverable `.script support` repair state rather than an unconditional preflight failure. Do not stop unrelated same-named processes.

## Current stages

1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact writer callsite ✅
7. true writer-entry capture ✅
8. RDX stack-local XYZ correlation ✅
9. c14 JNE blocker identified ✅
10. c15 writer branch crossing ✅
11. c16 exact call target/boundary/hash ✅
12. c17 call argument provenance ✅
13. c18 actual callee CFG / first real writes ✅
14. c19 exact Transform+0x10C destination ✅
15. local/shared workspace acceptance ✅
16. c20 EDI local value provenance ✅ actual game
17. c21 exact `Transform+0x10C` pre-write read ✅ actual game
18. c22 TEST/JE classification ✅ actual game
19. c23 direct target `0x2CF7FE0` + carrier classification ✅ actual game
20. c24 target identity failure narrowed to PE runtime-table cap ✅ actual game
21. c25 full 527161-entry table validation + genuine missing runtime entry ✅ actual game
22. c26 exact `cmp r8d,0x3FFF` flags-only model ✅ actual game
23. c27 exact `JA` taken/fallthrough classification + bounded both-leg fan-out ✅ actual game
24. classify exact `0x2CF7FE9 MOV` memory operand/effect ▶ CURRENT
25. resume full writer XYZ/W causal proof ⏸ after local effect classification
26. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
27. safe writable upstream movement source ⬜ NOT AUTHORIZED
28. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
29. adhesion/contact refinement ⏸ deferred
30. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md`.

Classify the exact operands/effects of `0x2CF7FE9: mov` before adding any memory semantics. Memory write/RMW remains immediate refusal. A pure read-only load may be modeled only if the source range and loaded-value provenance can be conservatively established from owned symbolic/authenticated state without arbitrary live memory-value observation or guessed aliasing. Address provenance and loaded-value provenance must remain separate.

If and only if that exact MOV is admitted, resume the existing c27 two-path proof. Both JA outcomes remain required and every reachable path must safely end in modeled normal RET before target identity/hash/effects can be admitted.

Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.

Deferred user-facing features remain tracked in `docs/FEATURE_ROADMAP.md`.
