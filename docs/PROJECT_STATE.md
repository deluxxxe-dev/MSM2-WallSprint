# WallSprint project state

## c39 admission audit (2026-10-26; follows accepted c38 scan)

The current NEXT's explicit stop-if-unsafe path applies. **No c39 exact-slot watch
is enabled.** Runtime remains v0.1c38/package0.1.56; no c39 game ZIP/script/log.
See `analysis/V01C39_WATCH_RETIREMENT.md`, `V01C39_GATE.md` and
`V01C39_WATCH_STATUS.json` for the reproducible prerequisite failure.

Owned real-thread Wine DR/VEH tests reproduce reset and Position cleanup reporting
pending0/inflight0/no active leases, with exact saved debug registers restored at
that instant, followed by a delayed exception return re-enabling the retired
watch from its saved CONTEXT. A subsequent hit recovers through immutable lease
tombstones, but that is not bounded cleanup during silence. Real-game/Windows
manifestation is UNKNOWN. Existing production code is unchanged; normal prior
cleanup tests passing does not close this newly tested exception-return interval.

Do not treat existing cleanup bookkeeping as a c39 phase-rearm/serialization
certificate. A bounded exception-context retirement mechanism must be reviewed
and proven before adding a separate value-free single8byte watch/event path and
unique current-code trigger identity. Existing value/GPR/XMM capture must never
be repointed at the static slot. Full positive c39 access-capture acceptance
matrix remains unimplemented; this is the explicitly permitted fail-closed audit
outcome. No new owner offline/game test is needed for the test-only changes.
C38 accepted scan/c28 reader, c37 range, c36 escape/CFG barrier, all publication/
ownership/write gates and deferred producer0x351EB00 remain unchanged below.

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

The top values are conservative uncertainty, not observed taint.

## c36 accepted runtime result — unresolved store escape is a hard CFG barrier

Evidence: `analysis/V01C36_RUNTIME_RESULT.md`.

Exact existing game-side store:

```text
site_rva=0x2CF7FFB
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

The critical may-mask is uncertainty, not proof of actual overlap. The unresolved effect propagates into parent summaries before target-identity refusal.

## c37 accepted result — fallthrough offset proven; base publication unresolved

Evidence: `analysis/V01C37_ADDRESS_COMPONENTS.md`, `analysis/V01C37_RUNTIME_RESULT.md`.

The exact branch/MOVSXD/SHL sequence proves the relative R8 store offset without observing a live value:

```text
R8D fallthrough range = 0..0x3FFF
MOVSXD exact range = 0..0x3FFF
SHL count = 6
offset_min = 0
offset_max_inclusive = 0xFFFC0
offset_stride = 64
store_width = 32
end_extent_exclusive = 0xFFFE0
overflow_wrap_excluded = 1
offset_proven = 1
```

This is range metadata separate from may-tags.

The RAX source qword remains the writable image slot at `RVA 0xC2D2580`.

### c37 gameplay-capture condition

The owner reported an initial failed wall attachment. Sessions 1-4 were short `Mixed`/`WallRunUp`; sessions 5-6 were usable `WallRunSide` sessions with verified native Deinit, retention pass, no hard kill, MOD Position/source-position writes 0 and zero adhesion corrections. This does not affect the startup/current-code address proof.

## c37 owner offline slot scan — direct-only coverage inconclusive

Evidence: `analysis/V01C37_SLOT_PUBLICATION_SCAN_RESULT.md`.

The direct RIP-relative/absolute class returned no local candidates, but coverage and writer enumeration remained incomplete. Accepted c28 already proves one exact register-based consumer:

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
RCX base_origin_offset=0xC2D2480
+0x100 -> slot RVA 0xC2D2580
```

Therefore zero direct candidates was never a no-reader/no-writer result.

## c38 accepted owner scan — register-affine representation works, no local slot writer found

Evidence: `analysis/V01C38_SLOT_PUBLICATION_SCAN_RESULT.md`, derived from owner-supplied `MSM2-WS-v0.1c38-slot-publication.json`.

Exact-image/tool identity:

```text
image_authenticated=1
tool_version=v0.1c38
schema_version=2
scan_scope=bounded_offline_direct_and_register_affine_candidates_not_all_writes
slot_rva=0xC2D2580
slot readable=1
slot writable=1
```

Local exact-slot results:

```text
reference_candidates=[]
direct_writer_candidates=0
register_affine_writer_candidates=0
writer_candidates=0
register_affine_resolved_operands=1360
```

The 1360 resolved register-affine operands prove the new local representation actually executed on the authenticated real image. They did not overlap the target slot.

Coverage remains incomplete:

```text
selected_byte_sweep_complete=0
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

Actual coverage diagnostics:

```text
instructions_decoded=5000000
cap_instructions=5000000
bytes_scanned=17859464
decode_gaps=737
indirect_or_unresolved_operands=1278483
control_flow_boundary_loss=822382
runtime_region_boundary_loss=208548
unsupported_register_mutation=97883
memory_loaded_base=85282
indexed_address=36092
provenance_window_exhausted=20336
partial_register_mutation=4900
segment_unsupported=1985
```

The largest unresolved class is intentional control-flow boundary loss. `memory_loaded_base` is also substantial and cannot be converted into a pointer origin without violating the no-value/no-pointer-chase rule.

The accepted c28 reader anchor was independently revalidated against current exact code:

```text
site_rva=0x2CF7FE9
effective_rva=0xC2D2580
may_write=0
address_proof_class=authenticated_external_anchor
origin_rva=0xC2D2480
local_scanner_coverage=0
```

Therefore zero local writer candidates still does not prove no writer.

## Next evidence direction — exact-slot non-value runtime access-site capture

The branch already contains a guarded hardware debug-register watch engine used by reset/Position diagnostics, including thread snapshots, per-thread DR lease ownership, foreign-DR refusal, VEH handling and restore/cleanup. C39 should investigate reusing that existing mechanism rather than broadening offline analysis into generic cross-control-flow or pointer dataflow.

The new diagnostic target is the static exact 8-byte slot:

```text
module_base + 0xC2D2580
```

The watch must record instruction identity/access metadata only. It must not read/log the watched qword contents or pointer-chase. A bounded read/write phase may use the accepted c28 reader `0x2CF7FE9` as a positive control; after that, write-only observation is preferred if the existing hardware engine can safely re-arm while preserving ownership/cleanup rules.

A writer hit would be a runtime writer-site candidate, not all-writer/publication/allocation/lifetime/ownership proof. No hit is not proof of no writer.

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
19. c37 direct exact-slot scan ✅ authenticated, incomplete by design
20. c38 register-affine exact-slot scan ✅ actual image, 1360 affine operands resolved, no local slot writer
21. c39 exact-slot non-value runtime access-site capture ▶ CURRENT
22. bounded publication/source chain ⏸ pending writer-site evidence
23. contain `0x2CF7FFB` store destination ⏸ pending base/allocation/lifetime proof
24. resume full writer XYZ/W causal proof ⏸ after store containment
25. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
26. safe writable upstream movement source ⬜ NOT AUTHORIZED
27. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
28. adhesion/contact refinement ⏸ deferred
29. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md`.

C39 must first inspect and preserve the existing hardware-watch ownership/cleanup contract. Reuse it only if the exact static slot can be watched without reading the slot value and without interfering unsafely with existing reset/Position windows.

Do not add PAGE_GUARD or broad memory interception merely to force progress. Do not infer writer completeness from a bounded runtime window. Do not merge `main`, do not traverse producer `0x351EB00`, and do not enable gameplay writes.
