# WallSprint project state

## Goal
Convert vanilla WallRun into surface-relative locomotion. Do not advance movement rewrite, adhesion, or camera until runtime ownership gates authorize them.

## Repository / branch policy
- Canonical remote: `https://github.com/deluxxxe-dev/MSM2-WallSprint.git`
- Current implementation line: `codex/v01c20-edi-value-provenance`
- Owner-confirmed organized baseline: `5192f05f32313919fdf4049bd734b971655d163f`
- Continue from later commits on this branch; do not reset to the baseline.
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

Runtime source relation remains `RDX=entry RSP+0x28`; sampled `[RDX] XYZ` equals exact executed Position XYZ bits. This remains correlation until the writer/effect/ownership chain completes.

## c19/c20/c21 accepted results

C19 established:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
RCX provenance = writer-entry Hero Transform
destination = Hero Transform +0x10C
```

C20 established the bounded local reaching definition for EDI:

```text
producer RVA = 0x2B2356F
producer kind = memory_load
origin kind = memory_load
root = analyzer-internal globalRoot 16
offset = 0x0C323788
reaching definitions = 1
all local reaching states examined = 1
same local value origin = 1
value provenance exact = 1
```

C21 classified the earlier exact same-field read:

```text
0x2B23578: mov EAX,[RCX+0x10C]
address = Hero Transform+0x10C
first possible use = 0x2B23581
first control dependence = 0x2B23583
memory_escape = 0
```

Semantic meaning of the memory origin, EDI, and `Transform+0x10C` remains unknown.

## Accepted c22 runtime result — TEST/JE resolved

Evidence: `analysis/V01C22_RUNTIME_RESULT.md`.

```text
0x2B235BE: test r8b,r8b
parent R8 selected/same-field tags = 0x0
flags_may_depend_tags = 0x0
0x2B235C1: je
consumer_may_depend_tags = 0x0
first_possible_dependent_branch_rva = 0x0
returned_flags_tags = 0x0
```

Thus that exact TEST/JE pair is not a carrier of the selected same-field read for the analyzed state.

## Accepted c23 runtime result — direct CALL target identified

Evidence: `analysis/V01C23_RUNTIME_RESULT.md`.

```text
0x2B235D4: direct call
target_rva=0x2CF7FE0
exact_target=1
```

Selected `Transform+0x10C` dependence reaches the call boundary in raw GPR carrier index 0 (`gpr_tags=0x1`, mem32 Hero Transform+0x10C). Target boundary/hash was unavailable, so no effect analysis was admitted.

## Accepted c24 runtime result — PE runtime-table cap blocker

Evidence: `analysis/V01C24_RUNTIME_RESULT.md`.

C24 reported `PE_runtime_entries=527161` against its old 262144-entry diagnostic cap. This established only a cap refusal, not table corruption or target no-entry status.

## Accepted c25 runtime result — table valid, target truly lacks runtime entry

Evidence: `analysis/V01C25_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c25-20261025-042121.log`.

C25 fully validates the exact PE exception/runtime-function table under an explicit bounded budget:

```text
directory_rva=0xC566000
directory_bytes=6325932
entry_bytes=12
derived_entries=527161
metadata_byte_cap=8388608
entry_cap=600000
byte_cap_exceeded=0
entry_cap_exceeded=0
entry_validation_skipped_by_cap=0
metadata_bytes_read=6325932
entries_validated=527161
all_entries_read_and_validated=1
table_structurally_valid=1
runtime_lookup_queried=1
```

Thus c24's old-cap refusal is resolved as a budget issue rather than malformed metadata.

For exact target `0x2CF7FE0`, the fully validated table and actual OS lookup agree:

```text
runtime_lookup_found=0
runtime_lookup_same_image=1
runtime_lookup_readable=1
next_runtime_rva=0x2CF8020
missing_runtime_entry=1
PE_runtime_table_valid=1
```

Therefore the strict pinned no-unwind fallback legitimately starts. It immediately stops at the first decoded target instruction:

```text
method=strict_single_target_no_unwind_linear
scan_bytes=64
snapshot_stable=1
instructions=1
admitted=0
admitted_bytes=0
barrier_rva=0x2CF7FE0
barrier_instruction=cmp
reason=target_leaf_instruction_effect_not_admitted
```

No target begin/end/hash/effect is admitted yet. The selected-read flow's exact blocker has therefore advanced from metadata to the nested target instruction itself:

```text
complete_local_flow=0
first_unresolved_rva=0x2CF7FE0
first_unresolved_instruction=cmp
returned_gpr_may_depend_mask=0x1
memory_escape=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
effect_certificate=0
writable_source_proven=0
```

Do not infer CMP operands, width, branch outcome, ABI meaning or semantic target identity from the mnemonic alone.

Runtime safety remained closed across two A/E-style sessions: verified native Deinit, retention pass, hard kills 0, MOD Position writes 0, source-position writes 0.

## Current safety gate

```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```

Producer `0x351EB00` remains deferred. No movement, adhesion, camera, animation or physics advancement is authorized.

## Local workspace — accepted on real PC

Canonical workspace:

```text
<game>\Mods Library\Mods-Developing\MSM2-WS\
├─ MSM2-WS-v<version>.script
├─ Update-WS.cmd
├─ logs\
├─ _source\
└─ _archive\
```

Shared ordinary mods + SF + WS, shared Overstrike, modded launch and versioned WS/SF runtime logs are accepted. See `analysis/LOCAL_WORKSPACE_RUNTIME_ACCEPTANCE_20261024.md` and `docs/LOCAL_WORKSPACE.md`.

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
17. c21 exact `Transform+0x10C` pre-write read classification ✅ actual game
18. c22 `0x2B235BE TEST r8b,r8b` / `0x2B235C1 JE` classification ✅ actual game
19. c23 `0x2B235D4` direct target `0x2CF7FE0` + carrier classification ✅ actual game
20. c24 target identity failure narrowed to PE runtime-table validation cap ✅ actual game
21. c25 full 527161-entry table validation + genuine missing runtime entry ✅ actual game
22. classify/model exact `0x2CF7FE0 CMP` under strict no-unwind rule ▶ CURRENT
23. resume full writer XYZ/W causal proof ⏸ after local effect classification
24. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
25. safe writable upstream movement source ⬜ NOT AUTHORIZED
26. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
27. adhesion/contact refinement ⏸ deferred
28. animation/camera/later traversal ⬜

## Current action

Classify the exact `CMP` operands/width/effects at `0x2CF7FE0` without guessing from the mnemonic. Add only the smallest read-only flag semantics needed if the exact form can be proven safe under the existing strict single-target no-unwind model. Any memory operand, unsupported partial form, branch/call/stack effect, alias ambiguity or cap remains an exact fail-closed boundary. If the target eventually reaches a fully modeled normal RET under the existing 128-byte/32-instruction rule, only then admit begin/end/hash and resume the c23 call-effect sidecar.

Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.

Deferred user-facing features are tracked in `docs/FEATURE_ROADMAP.md` and remain intentionally out of scope until the movement ownership foundation is safe.

## C26 prepared diagnostic handoff

v0.1c26/package0.1.44 describes exact first CMP0x2CF7FE0 before any extension:
operand classes/widths/aliases/parents/constants/access and decoded flags effects.
Only guarded full32/64-bit GPR reg/reg or reg/imm flags-only forms are modeled;
8/16/memory/unknown forms refuse. Stable-prefix parent tag/next Jcc condition
metadata never grants target identity: all branches remain strict linear refusals.
Full current snapshot/128bytes/32instructions/normal RET/coverage checks required
before hash and c23 effects. Six owned groups added to identity suite; c25 large
metadata and c19-c24/native/workspace contracts retained. Actual CMP form/next
boundary remains PENDING new minimal diagnostic, not proven by fixture tests.
See analysis/V01C26_CMP_FLAGS.md / V01C26_GATE.md and docs/CODEX_RESULT.md.
CODEX_NEXT remains the task authority. All Position/source/input/movement writes
and writable/ownership/exclusivity gates remain0; producer0x351EB00 deferred.
