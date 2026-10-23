# Next task — WallSprint v0.1c13 Writer Entry Snapshot

## Base
```text
base branch: codex/v01c12-movement-source-lineage
implementation: 4c84d634df4d629b237d4cc311d5e5cd6de4398c
published head: c966d233787504dda88f65d1825c5c1c408a2d67
version: v0.1c12 / package 0.1.30
branch: codex/v01c13-writer-entry-snapshot
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `analysis/V01C12_RUNTIME_RESULT.md`, c12 `docs/CODEX_RESULT.md`, `analysis/MOVEMENT_WRITER_2CFC900.md`, and `analysis/MOVEMENT_CALLER_351EEB0.md`.

## Authoritative real result
c12 A/cardinal and E/diagonal both preserve Surface Up through verified Deinit with hard kill 0 and `Position_writes_by_mod=0`.

Verified chain:
```text
writer 0x2CFC900..0x2CFC9B7
stores 0x2CFC936/93C/942/945 -> Transform +0x30/+0x34/+0x38/+0x3C
exact direct call 0x351EEAB -> 0x2CFC900
return 0x351EEB0
observed second return 0x351A020
```

Both surfaces end at the same blocker:
```text
first_unsupported_offset=0xA
complete_writer_CFG=0
source_prefix_proven=0
source_capture_ready=0
eligible_hits=0
attempts=0
true_entry_snapshot=0
caller2_direct_relation_verified=0
writable_source_proven=0
```

## Objective
Resolve the smallest missing edge: capture **true entry-time state** for verified Hero-targeted invocations of writer `0x2CFC900`, then pair each invocation with the existing Position-store hardware-watch events.

Answer only:
1. Which entry register/pointer/stack value supplies values later written to +0x30/+0x34/+0x38/+0x3C?
2. Is that source stable on A and E?
3. What exact upstream producer edge remains if not writable?

c13 remains read-only for movement and Position.

## Implementation requirements
- Preserve every c12 orientation/lifecycle/watch/build gate unchanged.
- Add the smallest exact-build diagnostic that observes `0x2CFC900` at actual entry before its body mutates source registers.
- Preferred: verified diagnostic detour/raw thunk at exact writer boundary/hash, transparent to ABI/flags/arguments/return. No typed game signature. Fail closed if transparency cannot be proven.
- Filter to active verified WallRun + current verified Hero Transform. Generic Transform calls must not enter the evidence set.
- Hard-cap eligible snapshots, e.g. <=32/session.
- Capture only bounded GPR/XMM state, return address, RSP and a small stack-argument window. No arbitrary memory dump or full game body.
- Guarded reads only; no PAGE_GUARD/protection changes or broad pointer chasing.

Emit:
```text
MOVEMENT_WRITER_ENTRY_SNAPSHOT
session= invocation_id= thread_id= elapsed=
target_transform= return_rva= caller2_observed_rva=
rcx= rdx= r8= r9= rsp=
selected_gpr_state= selected_xmm_state= stack_arg_snapshot=
entry_target_verified= code_integrity_verified=
```

## Pair exact invocation to Position stores
Use an explicit per-thread/invocation token so an entry snapshot pairs only with its own `POSITION_WRITE_EVENT`s.

Emit:
```text
MOVEMENT_WRITER_INVOCATION_MAP
session= invocation_id= thread_id= target_transform=
store_30_seen= store_34_seen= store_38_seen= store_3c_seen=
position_before=(...) position_after=(...)
entry_source_match_mask= entry_pointer_candidate_mask=
source_stable=
delta_normal= delta_forward= delta_right=
reason=
```

## Entry source candidates
Test only bounded candidates from GPR values, XMM lanes, readable 16-byte argument pointers, and bounded stack args.

Emit:
```text
MOVEMENT_ENTRY_SOURCE_CANDIDATE
session= invocation_id= candidate_id=
origin=GPR|XMM|pointer|stack
origin_register_or_offset=
value4=(...)
relationship=current_position|final_position|delta_like|partial|none
exact_store_match_mask=
stable_across_invocations=
stable_across_surfaces=
typed_semantics=unknown
writable_source_proven=0
reason=
```

`delta_like` is correlation only. Do not rename it velocity/displacement/root-motion without independent proof.

## Caller2 relation
c12 proved direct `0x351EEAB -> 0x2CFC900`, but second return `0x351A020` failed a *direct* static relation check. Classify runtime relation conservatively as direct/indirect/wrapper/unresolved using stack/return evidence; do not force direct E8.

## Write gate
Do not set `writable_source_proven=1` unless real evidence proves exact source storage/range, true pre-mutation values, causal feed including W, Hero/session/thread lifetime, stable producer provenance, no relevant downstream conflict, and exclusive intended translation effect. Otherwise keep 0 and name the next exact edge.

## No scope expansion
No Position/source modification, input hook/rewrite, custom movement/gravity, active adhesion, camera/animation/traversal work, PAGE_GUARD mutation or unbounded tracing. Do not expand into a general CFG project merely to force success.

## Tests
Cover transparent entry thunk state preservation, exact build/writer gate, Hero filtering, invocation pairing, concurrent/reentrant calls, wrong lifecycle identity, guarded read refusal, candidate false-positive rejection, caller2 relation classification, c12 regressions, and cleanup/unhook.

## Human test
```text
A/cardinal ~4 s
E/diagonal ~4 s
```
Ordinary input. No visible movement change expected. Full private logs. Video optional backup; logs authoritative.

## Required outputs
```text
analysis/V01C12_RUNTIME_RESULT.md
analysis/MOVEMENT_WRITER_ENTRY_SNAPSHOT.md
analysis/MOVEMENT_ENTRY_SOURCE_CANDIDATES.md
analysis/V01C13_GATE.md
README_TEST.md
docs/PROJECT_STATE.md
docs/CODEX_RESULT.md
WallSprint_v0.1c13_writer_entry_snapshot.zip
```
