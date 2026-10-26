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

### c19-c25 foundation

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
EDI producer=0x2B2356F memory load
same-field read=0x2B23578: mov EAX,[RCX+0x10C]
0x2B235BE test r8b,r8b -> 0x2B235C1 je tag-disjoint
0x2B235D4 -> 0x2CF7FE0 carries selected read in raw GPR0 tag1
```

C25 authenticated all 527161 runtime-function entries and proved target `0x2CF7FE0` has no containing runtime entry; strict pinned no-unwind analysis is legitimate for this exact image.

### c26-c35 accepted target prefix

```text
0x2CF7FE0: cmp r8d,0x3FFF
0x2CF7FE7: ja 0x2CF8013 / fallthrough 0x2CF7FE9
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
0x2CF7FF0: vmovups ymm0,ymmword ptr [rdx]
0x2CF7FF4: movsxd r8,r8d
0x2CF7FF7: shl r8,6
0x2CF7FFB: vmovups ymmword ptr [r8+rax],ymm0
```

Both JA outcomes remain mandatory.

Fallthrough pre-store abstract state:

```text
RAX tags=0x7
R8 tags=0x0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags tags=0x7
```

Interpretation:
- `RAX=0x7` is may-top from the opaque writable `.data` qword load, not observed taint.
- YMM0 low/high `0x7` are conservative loaded-vector dependence, not observed taint.
- flags `0x7` comes from undefined AF/OF after `shl r8,6`.

## c36 accepted runtime result — unresolved store escape is a hard CFG barrier

Evidence: `analysis/V01C36_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c36-20261026-150111.log`.

C36 confirms the exact existing game-side store and its unresolved effect summary:

```text
site_rva=0x2CF7FFB
exact_store_identity=1
current_proof=1
memory_write_seen=1
memory_write_width=32
memory_address_tags=0x7
memory_value_low128_tags=0x7
memory_value_high128_tags=0x7
memory_alias_may_mask=0x7F
critical_alias_may_mask=0x3FF
memory_escape=1
first_escape_rva=0x2CF7FFB
pending_unknown_write=1
store_destination_contained=0
control_alias_unexcluded=1
CFG_blocked=1
continuation_allowed=0
later_memory_facts_usable=0
architectural_registers_preserved=1
reason=target_store_destination_code_or_control_stack_alias_unexcluded
```

The critical may-mask conservatively includes current code/runtime data, control/return stack, caller-home and callee-private stack, Hero/Transform, `Transform+0x10C`, writer source, Position storage and other later memory. This is uncertainty, not proof of actual overlap.

The unresolved effect now propagates correctly into the parent summaries before target-identity refusal:

```text
READ_CALL memory_escape=1
READ_CALL first_escape_rva=0x2CF7FFB
READ_CALL pending_unknown_write=1

READ_FLOW memory_escape=1
READ_FLOW first_escape_rva=0x2CF7FFB
READ_FLOW first_unresolved_rva=0x2CF7FFB
```

The prior c35 parent `memory_escape=0` was an admission artifact, not proof of no escape.

Target remains incomplete:

```text
TARGET_IDENTITY admitted=0
barrier_rva=0x2CF7FFB
barrier_instruction=vmovups
reason=target_store_destination_code_or_control_stack_alias_unexcluded
LEAF_CFG complete=0
return_paths=0
unique_instructions=7
visits=7
effect_certificate=0
```

No next opcode is inferred. A clean JA path cannot erase an unresolved store on the fallthrough path.

## Current address-containment decomposition

The hard blocker is now specifically the destination:

```text
[R8 + RAX]
```

The next useful evidence must separate two facts rather than repeat the same runtime capture:

1. **Fallthrough R8 offset range.** The branch is the non-JA outcome of unsigned `cmp r8d,0x3FFF`; c37 should prove path-sensitively whether this implies `r8d <= 0x3FFF`, then carry that exact range through `movsxd` and `shl 6` without live numeric observation.
2. **RAX base publication/provenance.** RAX is loaded from the exact writable-image qword at source RVA `0xC2D2580`. c37 should find authenticated non-value evidence for writers/publication/allocation/range/lifetime of that slot. Zero tags, stable sessions, or a numeric pointer value alone are insufficient.

If the first proof succeeds, the expected mathematical maximum must be derived by the implementation, not assumed. If both proofs eventually establish an allocation base plus a bounded offset and 32-byte store extent, only then may critical non-overlap/containment be considered.

## Safety gate

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Observed c36 sessions retained verified native Deinit, retention pass, hard kills 0, MOD Position writes 0, source-position writes 0 and adhesion corrections 0.

The VMOVUPS store is existing game code, not a MOD write and not permission to write movement/Position data.

Producer `0x351EB00` remains deferred.

## Current stages

1. lifecycle / SurfaceFrame ✅
2. Path A/B orientation and stabilization ✅
3. planar A/E orientation ✅
4. Position writer attribution ✅
5. SurfaceFrame translation decomposition ✅
6. exact writer callsite / writer-entry source ✅
7. RDX stack-local XYZ correlation ✅
8. c14-c23 writer/callee/provenance chain ✅
9. c24-c25 runtime table/no-unwind basis ✅
10. c26 CMP ✅
11. c27 JA two-path fan-out ✅
12. c28-c30 qword load + unknown-top continuation ✅
13. c31-c32 first VMOVUPS + 256-bit state ✅
14. c33 MOVSXD ✅
15. c34 SHL data/flags ✅
16. c35 second VMOVUPS exact store ✅
17. c36 unresolved store escape / hard control-alias barrier ✅ actual game
18. prove fallthrough offset range + exact qword-slot base publication ▶ CURRENT
19. contain `0x2CF7FFB` store destination ⏸ pending both address components
20. resume full writer XYZ/W causal proof ⏸ after store containment
21. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
22. safe writable upstream movement source ⬜ NOT AUTHORIZED
23. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
24. adhesion/contact refinement ⏸ deferred
25. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md`.

C37 must not repeat c36 A/E metadata as if that could solve containment. It should add only new authenticated non-value proof for the two address components: path-sensitive R8 range from the accepted CMP/JA/MOVSXD/SHL sequence, and exact source/publication analysis for the qword slot at RVA `0xC2D2580` that supplies RAX.

Do not observe/log a live numeric pointer merely to narrow aliases. Do not assume the slot is a pointer, pool, array, allocation or owner until its writer/publication chain proves that. Exact-slot xref/write discovery is allowed; generic arbitrary memory emulation is not.

Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.

## c37 prepared — offset range proven independently of opaque base

v0.1c37/package0.1.55 derives fallthrough unsigned R8D interval through exact
MOVSXD/SHL6: offsets0..0xFFFC0/stride64/endExclusive0xFFFE0 for32bytes, checked
nonwrap, separate from unchanged tags. Other R8 writes invalidate it. Publication
slot0xC2D2580 writers/source/allocation/extent/lifetime remain unproven; no actual
authenticated EXE in cloud, counts unknown/not scanned. Bounded hash-guarded
offline exact-slot candidate tool included; no all-writer/ownership claim and
deferred producer excluded. Base+offset/control containment false; c36 escape1/
pending/hard CFG stop and all safety gates preserved. No repeated A/E capture.
NEXT unchanged; see V01C37_ADDRESS_COMPONENTS.md / PUBLICATION_STATUS / CODEX_RESULT.
