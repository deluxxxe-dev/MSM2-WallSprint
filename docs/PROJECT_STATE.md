# WallSprint project state

## c43 current stage — native final-restoration divergence diagnostic

Accepted owner native evidence: analysis/V01C42_NATIVE_WINDOWS_RESULT.md.
The c42 probe at70c55e3 admitted actual known policies[0,0,0x100] and executed
its witness path, then returned FAIL at the combined final lease/DR assertion.
Outer acknowledgment, pending0, canRetire and false final poll completed first.
Whether leases, any individual DR register or both differed remains UNKNOWN.
Do not call that run an environment refusal, native success or production fix.

C43 adds only test-owned before/after final-poll lease/pending/canRetire/poll
observations, exact Dr0/1/2/3/6/7 snapshots/equality/XOR, identity and three
stability readbacks. Missing observations remain null. Final acceptance and all
c39-c42 counterexamples, state/ownership/lifetime/cap/mitigation semantics remain.
The extra pre-poll suspend/read/resume changes scheduling; samples are sequential,
not an atomic history or proof of the original failure's cause. No normalization,
policy change or production restoration change is authorized.

See analysis/V01C43_RESTORATION_DIAGNOSTIC.md, V01C43_GATE.md and V01C43_STATUS.json.
After required validation, the c43 standalone artifact/manifest/licenses go under
analysis/native-probes/c43; c41/c42 artifacts/native evidence remain unchanged.
Only after publication, request one native Windows standalone JSON run. No
Update-WS, game, Overstrike, WallRun, scan or game log is needed. Runtime remains
v0.1c38/package0.1.56; production integration and exact-slot mode stay unauthorized.

## c42 historical stage — classifier admitted, native final restoration failed

C42's evidence-backed per-bit classifier admits CetDynamicApisOutOfProcOnly0x100
because it governs2 unused APIs. Unknown/reserved/relevant bits and unknown queries
still refuse without changing settings. Synthetic classification and owned Wine
validation succeeded. The refreshed owner native run reached the witness path
and failed final restoration; this supersedes the old pending-native status in
historical c42 artifacts/status. Detailed supplied result is preserved unchanged.
The earlier c41 native refusal remains separate: that artifact rejected nonzero
flags before executing the witness. Neither run proves native witness acceptance.

## c41 historical stage — partial success and native refusal

C41 task base4b39a82; current task is c43. C40 completed its review
with the fail-closed outcome; production retirement remains unproven. C41 adds
only an owned test prototype, not production integration or a slot watch.
Current c41 handoff classification: **PARTIAL SUCCESS** — owned Wine feasibility
is established; native policy queries returned[0,0,256] and the witness did not
execute due the old coarse boundary. Native witness verification remains pending.

The15byte MOV [RIP+dedicated marker],imm32 / JMP rel32 original-RIP stub provides
an actual post-return token in the bounded owned Wine fixture. Separate immutable
thread/lease/generation/context tickets preserve nested/multiple debt; close/
deadline/kill refuse final release until all witnesses. Context-cap uncertainty
poisons retirement; code/data stay alive until thread exit. Owned GPR/RFLAGS/RSP,
active-stack, YMM0..15, FXSAVE and debug-state checks pass. This is not arbitrary
process-wide accounting, a full-XSTATE runtime proof or a native-Windows/CET fix.

Details: `analysis/V01C41_CONTINUATION_WITNESS.md`, `V01C41_GATE.md`,
`V01C41_WITNESS_STATUS.json`. The historical c41 standalone command used
`analysis/native-probes/c41/continuation-witness.exe --position --multiple --json`.
Current native follow-up uses c43 above, not this historical artifact. No game
launch/update/slot scan/log is needed. This historical c41 artifact refused
all nonzero native flags; c42 refines only the standalone test boundary. Native evidence and a later
explicit integration task are required before c39-style slot work can resume.
Runtime remains v0.1c38/package0.1.56; original c39/c40 production counterexamples
remain intact. No production watch, source/Position/gameplay or shared-tool change.

## c39 admission audit (2026-10-26; follows accepted c38 scan)

The c39 task's explicit stop-if-unsafe path applied. **No c39 exact-slot watch
was enabled.** Runtime remains v0.1c38/package0.1.56; no c39 game ZIP/script/log.
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

## c40 completed review — bounded retirement not established

The c40 task reviewed the retirement prerequisite exposed by c39: keep cleanup/serialization pending until
an old saved exception CONTEXT can no longer reapply a retired owned DR
configuration, or fail closed if that condition cannot be established.

C40 does **not** authorize the postponed exact-slot RW8/WRITE8 watch, slot-value
reads, pointer chasing, publication-chain interpretation, producer traversal or
any gameplay write. The existing delayed-return owned regression is the primary
acceptance test and must not be removed or weakened.

Standing Codex completion-report requirements are in `docs/CODEX_REPORTING.md`.

C40 owned review result (2026-10-26): **retirement implementation acceptance NOT
MET; section5 fail-closed fallback taken**. Production watch code is unchanged.
The original held-return test now also covers the unchanged timeout and kill
paths and repeated exact DR readbacks; all still permit later resurrection.
Normal close after the owned writer's post-return acknowledgment restores
correctly, but no equivalent verified all-thread production witness exists.
The2 VCH probes register successfully but receive no callback on this owned
Wine hardware path; VCH return-hold coverage is unavailable, not PASS. No API
ordering guarantee or continuation trampoline is assumed/shipped. See
`analysis/V01C40_RETIREMENT_REVIEW.md`, `V01C40_GATE.md` and
`V01C40_RETIREMENT_STATUS.json`. Production retirement remains blocked on a reviewed
post-return witness, not completed by green counterexample tests. Runtime remains
v0.1c38/package0.1.56; c39 exact-slot mode stays unauthorized. No owner update,
game test, offline scan or runtime log requested for this test-only review.

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

## Current evidence direction — retire saved exception CONTEXT before any exact-slot watch

C39 showed that the existing hardware-watch cleanup bookkeeping is not strong
enough to certify phase re-arm/serialization: a delayed saved exception CONTEXT
can restore a retired DR configuration after the coordinator has observed current
thread DR restoration and inactive lease bookkeeping.

C40 stopped without a safe production mechanism. C41 evaluates an owned
post-return continuation witness only. The existing delayed-return regression
remains authoritative for the production counterexample.
The exact game/static slot remains:

```text
module_base + 0xC2D2580
```

but **no watch for that slot is authorized in c41**. After native evidence and
separately authorized production integration, a later task may return to the value-free access
capture. A future writer hit would still be only a runtime writer-site candidate;
no hit would still not prove no writer.

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
21. c39 exact-slot watch admission audit ✅ STOPPED FAIL-CLOSED; retirement prerequisite reproduced
22. c40 saved-exception CONTEXT retirement review ✅ STOPPED FAIL-CLOSED; no production fix
23. c41 owned post-return feasibility ✅ PARTIAL; owned Wine passed, native coarse-boundary REFUSED before witness
24. c42 native mitigation boundary refinement ✅ classifier admitted; actual native final-restoration FAIL
25. c43 final-restoration diagnostic ▶ CURRENT; native cause unresolved
26. exact-slot non-value runtime access-site capture ⏸ after reviewed production retirement integration
27. bounded publication/source chain ⏸ pending writer-site evidence
28. contain `0x2CF7FFB` store destination ⏸ pending base/allocation/lifetime proof
29. resume full writer XYZ/W causal proof ⏸ after store containment
30. caller/storage/producer ownership (`0x351EB00`) ⏸ after writer contract
31. safe writable upstream movement source ⬜ NOT AUTHORIZED
32. surface-relative movement/input rewrite ⬜ NOT AUTHORIZED
33. adhesion/contact refinement ⏸ deferred
34. animation/camera/later traversal ⬜

## Current action

Follow `docs/CODEX_NEXT.md` for c43. Preserve the accepted native c42 FAIL and
exact restoration semantics; finish test-only diagnostic validation and publish
the c43 standalone probe before requesting one native JSON run. No original
cause, Windows canonicalization or production retirement certificate is inferred.
A later separately authorized task must review production integration.

Do not add the exact-slot watch, PAGE_GUARD or broad memory interception merely
to force progress. Do not merge `main`, do not traverse producer `0x351EB00`, and
do not enable gameplay writes.
