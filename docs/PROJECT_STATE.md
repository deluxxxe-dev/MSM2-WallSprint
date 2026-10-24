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

## c24 accepted result — old runtime-table cap refusal

C24 observed 527161 derived PE runtime-function entries against its old 262144-entry diagnostic cap. This was accepted only as a cap refusal, not malformed metadata or missing-entry proof.

## c25 accepted result — full table valid, target genuinely has no runtime entry

Evidence: `analysis/V01C25_RUNTIME_RESULT.md`.

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

## c26 accepted actual runtime result — exact CMP crossed, JA is next barrier

Evidence: `analysis/V01C26_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c26-20261025-054449.log`.

Exact first target instruction:

```text
0x2CF7FE0: cmp r8d, 0x3FFF
operand_count=2
has_memory_operand=0
flags_only=1
form_supported=1
GPR_XMM_memory_writes=0
flags_written=CF_PF_AF_ZF_SF_OF
```

For the analyzed selected-read state:

```text
R8 parent_may_depend_tags=0x0
CMP flags_may_depend_tags=0x0
```

Thus the c26 narrow CMP model is accepted: it is a read-only flags effect and does not carry selected tag 1 into flags for this state.

The strict walk now stops at the next exact instruction:

```text
0x2CF7FE7: ja
condition=CF0_and_ZF0
flags_read=CF_ZF
flags_producer_rva=0x2CF7FE0
flags_dependency_known=1
consumer_may_depend_tags=0x0
control_expanded=0
numerical_outcome=unknown
both_outcomes_unresolved=1
reason=target_leaf_control_or_nested_call_refused
```

Target identity/effect remains unadmitted:

```text
admitted=0
boundary_verified=0
snapshot_authenticated=0
effect_certificate=0
first_unresolved_rva=0x2CF7FE7
first_unresolved_instruction=ja
writable_source_proven=0
```

Interpretation: JA control dependence is proven disjoint from selected tag 1 for this analyzed state, but the numerical outcome is unknown. Do not follow only one edge. A successor proof must classify exact taken/fallthrough edges and, if authorized by a narrow bounded rule, enumerate both and merge their effects conservatively.

## Current safety gate

```text
writable_source_proven=0
Position/source/input/movement writes=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```

C26 runtime safety remained closed across two WallRun sessions: verified native Deinit, retention pass, hard kills 0, MOD Position writes 0, source-position writes 0, adhesion corrections 0.

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

A 2026-10-25 local failure was traced to game-root `Check for updates.exe` (Overstrike Updater, Tkachov) loading the game-root proxy `WINMM.dll`, thereby preventing Overstrike from deleting/replacing it during `.script support` installation. The WS helper now includes path-validated updater shutdown / launch-time prevention logic. `winmm.dll` absence is treated as a recoverable `.script support` repair state rather than an unconditional preflight failure. Do not stop unrelated same-named processes.

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
22. c26 exact `cmp r8d,0x3FFF` flags-only model + next `JA` barrier ✅ actual game
23. bounded exact `0x2CF7FE7 JA` edge classification / both-leg proof ▶ CURRENT
24. resume full writer XYZ/W causal proof ⏸ after local effect classification
25. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
26. safe writable upstream movement source ⬜ NOT AUTHORIZED
27. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
28. adhesion/contact refinement ⏸ deferred
29. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md`.

Classify exact taken and fallthrough edges of `0x2CF7FE7 JA`. Only add a narrowly bounded tag-disjoint conditional fan-out if both edges are exact, forward/acyclic, remain inside the pinned allowed code region and runtime-region boundary, and every reachable path remains within the existing no-memory/no-stack/no-call/no-unsupported-effect contract. Both outcomes must be explored; numerical direction must not be guessed. Target identity is admitted only if every reachable path terminates in modeled normal RET with all effects/caps satisfied.

Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.

Deferred user-facing features remain tracked in `docs/FEATURE_ROADMAP.md`.
