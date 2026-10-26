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

The critical may-mask conservatively includes current code/runtime data, control/return stack, caller-home and callee-private stack, Hero/Transform, `Transform+0x10C`, writer source, Position storage and other later memory. This is uncertainty, not proof of actual overlap. The unresolved effect propagates into parent summaries before target-identity refusal; the prior c35 parent `memory_escape=0` was only an admission artifact.

## c37 accepted result — fallthrough offset proven; base publication still unresolved

Implementation/static proof: `analysis/V01C37_ADDRESS_COMPONENTS.md` and `analysis/V01C37_PUBLICATION_STATUS.json`.

Actual-game confirmation: `analysis/V01C37_RUNTIME_RESULT.md` from owner-supplied `MSM2-WS-v0.1c37-20261026-164009.log`.

The JA fallthrough predicate and exact MOVSXD/SHL sequence prove the relative R8 store offset without observing a live value:

```text
R8D range on fallthrough = 0..0x3FFF
MOVSXD exact nonnegative 64-bit range = 0..0x3FFF
SHL count = 6
offset_min = 0
offset_max_inclusive = 0xFFFC0
offset_stride = 64
store_width = 32
end_extent_exclusive = 0xFFFE0
overflow_wrap_excluded = 1
offset_proven = 1
```

This is range metadata separate from unchanged may-tags.

The RAX source qword remains the writable image slot at `RVA 0xC2D2580`. Runtime confirmed publication/base remained unresolved before the local offline scan.

### c37 gameplay-capture condition

The owner reported an initial failed wall attachment that briefly entered upward/vertical WallRun. Sessions 1-4 are short `Mixed`/`WallRunUp` attempts and are not treated as intended stable samples. Sessions 5 and 6 are usable `WallRunSide` sessions (~3.77 s and ~4.32 s) with verified native Deinit, retention pass, no hard kill, MOD `Position_writes=0`, `source_position_writes=0`, and zero adhesion corrections.

This does not invalidate the c37 address proof because the new OFFSET_RANGE/SLOT_PUBLICATION records are startup/current-code diagnostics and are independent of those motion samples.

## c37 owner offline slot scan — direct-only coverage exhausted, register-based accesses remain unresolved

Evidence: `analysis/V01C37_SLOT_PUBLICATION_SCAN_RESULT.md`, derived from owner-supplied `MSM2-WS-v0.1c37-slot-publication.json`.

The shipped offline scanner authenticated the exact image and slot but found no candidates in its intentionally narrow direct RIP-relative/absolute class:

```text
slot_rva=0xC2D2580
image_authenticated=1
reference_candidates=[]
direct_writer_candidates=0
candidate_sweep_complete=0
writer_enumeration_complete=0
publication_source_kind=unproven
allocation_proven=0
range_proven=0
lifetime_proven=0
base_proven=0
destination_contained=0
CFG_continuation=0
live_qword_reads=0
pointer_chase=0
reason=xref_coverage_incomplete_indirect_writers_unexcluded
```

Coverage diagnostics:

```text
bytes_scanned=17859460
decode_gaps=737
indirect_or_unresolved_operands=1279672
```

Zero direct candidates is not a no-writer/no-reference proof. Accepted c28 already provides a concrete exact register-based consumer that the direct-only scanner class cannot discover by itself:

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
RCX base_origin_root=16
base_origin_offset=0xC2D2480
effective source RVA=0xC2D2580
address_expression_exact=1
```

Therefore the next bounded work is to add only enough non-value static address provenance to classify exact register-affine image-address memory candidates. Do not turn the large unresolved-operand count into generic x86 dataflow or a writer-completeness claim.

## Safety gate

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

The VMOVUPS store is existing game code, not a MOD write and not permission to write movement/Position data. Producer `0x351EB00` remains deferred.

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
18. c37 fallthrough R8 offset range ✅ static + actual diagnostic confirmation
19. c37 exact-slot direct candidate scan ✅ authenticated, inconclusive by design
20. c38 bounded register-affine exact-slot candidate discovery ▶ CURRENT
21. exact qword-slot publication/base provenance ⏸ pending useful writer/publication candidates
22. contain `0x2CF7FFB` store destination ⏸ pending base proof
23. resume full writer XYZ/W causal proof ⏸ after store containment
24. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
25. safe writable upstream movement source ⬜ NOT AUTHORIZED
26. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
27. adhesion/contact refinement ⏸ deferred
28. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md`.

C38 must start from the owner offline result, not repeat c37 gameplay capture or the same direct-only scan. Add only bounded exact register-affine image-address candidate discovery without reading live/static pointer values from memory and without pointer chasing.

Use the accepted c28 exact consumer as an anchor for the address representation, but do not hard-code it into a false coverage/completeness result. If the offline representation cannot reproduce its interprocedural parent provenance, record that exact boundary and keep it as an authenticated external anchor.

Do not infer writer completeness from zero/one/multiple candidates. Do not assume the slot or its value is a pointer, pool, array, allocation or owner. Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.

## c38 prepared — bounded static register-affine exact-slot candidates

v0.1c38/package0.1.56 extends offline scanner only: full64 RIP-LEA/copy/checked
affine LEA, no memory values or ABI. Partial/unsupported/loaded/control/gap/
runtime/window64/hop8 loss, exact8byte slot overlap and unresolved causes. c28
consumer uses explicitly separate accepted-parent/current-code external anchor,
not fake local coverage. All original caps/deferred exclusion; all-writer/
publication/allocation/lifetime/ownership/containment/CFG remain false. Actual
c38 image scan not run in cloud. Native c37 offset/c36 escape unchanged. NEXT
unchanged. Owner action: Update-WS then offline scanner only, return versioned
c38-slot-publication.json; no WallRun/A-E. See V01C38_REGISTER_AFFINE_SCAN.md.
