# Next task — WallSprint v0.1c17 Callee Alias Contract

## Base
```text
source branch: codex/v01c16-writer-call-effect
source implementation: 0a00bdeee05ffdae593d6963abd5c24424a2d8ef
source delivery head: 8f66b15a109e503cdfc115eb5a48ebadd7bc4113
next branch: codex/v01c17-callee-alias-contract
version target: v0.1c17
```

Read first:
- `AGENTS.md`
- `docs/DEV_RULES.md`
- `docs/PROJECT_STATE.md`
- `analysis/V01C16_RUNTIME_RESULT.md`
- `analysis/V01C16_WRITER_CALL_EFFECT.md`
- `analysis/RDX_XYZ_SOURCE_CONTRACT.md`
- c16 `docs/CODEX_RESULT.md`

## Accepted c16 actual-game evidence
C16 is an actual-game **Diagnostic PASS / narrower blocker**.

Regression gate:
```text
A/session1 wallrun_duration_ms=3875
E/session2 wallrun_duration_ms=4203
both verified Deinit
both retention_pass=1
both stale_suspend_count=0
both hard_kill_calls=0
both Position_writes_by_mod=0
A final Up dot surface=1
E final Up dot surface=0.99999994
```

Each session finalized:
```text
true_entry_snapshots=8
complete_invocation_maps=8
xyz_match_count=8
xyz_match_mask=0x7
xyz_stable_across_invocations=1
source W=0
destination W=1
summary_final=1
```

Exact writer-local call is now authenticated:
```text
writer=0x2CFC900..0x2CFC9B7
call_rva=0x2CFC927
return_rva=0x2CFC92C
call_kind=direct_call
target_rva=0x2B23560
target_boundary=0x2B23560..0x2B235E9
target_code_hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
exact_build_verified=1
current_code_verified=1
target_boundary_verified=1
target_complete_decode=1
```

Exact c16 blocker:
```text
effect=source_span_effect_unknown
source_span_no_write_proven=0
source_span_may_write=0
source_span_effect_unknown=1
next_callee_barrier_rva=0x2B23560
next_callee_instruction=mov
reason=callee_memory_write_alias_to_source_stack_or_destination_unresolved
```

Per actual invocation, `[entry RDX] XYZ` still matches exact Position XYZ stores (`mask=0x7`), but this remains corroboration only. `xyz_causality_proven=0` because c16 cannot prove the callee's memory writes are disjoint from the source stack object or Hero Transform destination.

Historical upstream fields stay unpromoted:
```text
exact_next_producer_rva=0x351EB00
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
writable_source_proven=0
```

## Objective
Resolve **only the memory-alias effect of authenticated callee `0x2B23560..0x2B235E9`** as used by writer call `0x2CFC927`.

The proof question is:

> For every callee memory write that can execute on the relevant writer path, can its effective address be proven disjoint from (a) the 16-byte entry-RDX source object and (b) the Hero Transform destination object needed by the later Position stores?

If yes, feed that result back into the existing c16 call contract and resume the c15 branch proof from `0x2CFC92C`. If no, emit one exact callee write/address-provenance blocker and stop.

Do not inspect or mutate `0x351EB00` in c17. Do not implement movement.

## 1. Preserve exact callee identity
Require all existing c16 gates before alias proof:
```text
exact executable version/SHA
writer boundary/hash/current bytes
exact call RVA/return RVA
exact direct target 0x2B23560
runtime-function boundary 0x2B23560..0x2B235E9
current target hash 2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
complete decode
```
Any mismatch fails closed.

Do not log/package the full private callee body.

## 2. Add bounded address-provenance analysis for callee memory writes
Extend the independent c16 effect classifier only enough to classify effective-address provenance for memory **writes** in this 137-byte callee.

Track addresses conservatively from:
- callee entry GPR arguments / preserved aliases;
- callee RSP and explicit stack allocation;
- exact caller/writer pre-call provenance where available;
- RIP-relative globals as a distinct non-stack/non-Transform region when exact;
- simple LEA/add/sub/copy chains that remain exact;
- local conditional control flow only within the authenticated callee boundary.

Classify each relevant write as one of:
```text
callee_private_stack
caller_shadow_or_return_area
source_span_exact
source_span_possible_alias
hero_transform_exact
hero_transform_possible_alias
rip_relative_global
known_disjoint_object
unknown_pointer
```

Unknown/conflicting pointer provenance must refuse. Do not infer disjointness merely because a pointer value differs in one snapshot.

Use strict caps appropriate to the exact 137-byte function. No recursion, arbitrary interprocedural traversal, nested-call expansion or open-ended fixpoint. If the callee contains a nested call/indirect transfer/unsupported effect, report that exact RVA/mnemonic as the new blocker rather than broadening scope.

## 3. Map writer pre-call arguments into callee entry provenance
At `0x2CFC927`, determine the provenance of call-time RCX/RDX/R8/R9 and any stack argument slots needed by the callee effect analysis.

Keep entry-writer RDX source identity distinct from **call-time RDX**, because Win64 volatile registers may have been repurposed before the call.

Emit compact evidence such as:
```text
MOVEMENT_WRITER_CALL_ARGUMENT_CONTRACT
call_rva=0x2cfc927
arg_rcx_origin=
arg_rdx_origin=
arg_r8_origin=
arg_r9_origin=
source_span_alias_mask=
transform_alias_mask=
proof_complete=0|1
reason=
```

Do not assign semantic names to arguments beyond proven provenance.

## 4. Prove or reject write disjointness
Emit a bounded summary:
```text
MOVEMENT_CALLEE_ALIAS_CONTRACT
target_rva=0x2b23560
target_end=0x2b235e9
memory_writes_seen=
writes_proven_private_stack=
writes_proven_global_disjoint=
writes_proven_other_disjoint=
writes_source_exact=
writes_source_possible_alias=
writes_transform_exact=
writes_transform_possible_alias=
writes_unknown=
source_span_no_write_proven=0|1
transform_no_write_proven=0|1
all_relevant_write_aliases_resolved=0|1
next_barrier_rva=
next_barrier_instruction=
next_barrier_effect=
reason=
```

`source_span_no_write_proven=1` requires every executable relevant callee write to be proven disjoint from `[entryRDX, entryRDX+0x10)` for the authenticated writer invocation contract. `transform_no_write_proven=1` similarly requires disjointness from the Hero Transform object/range needed for exact Position stores.

A write to either protected object is not automatically a crash/safety failure; classify it precisely and stop the causal proof unless its effect is independently understood. Do not hide it.

## 5. Smallest runtime observation only if static pointer provenance needs concrete call-time identity
If static caller-to-callee argument provenance is insufficient, add only a bounded read-only observation of the game's natural `0x2CFC927` call for the existing max 8 eligible invocations/session.

Allowed observations:
```text
call-time RCX/RDX/R8/R9 raw pointer values
entry-RDX source span address
Hero Transform address
callee RSP / caller RSP relation
source bytes before/after
exact relevant pointer/address relations
return observed
code-integrity status
```

Do not use before/after equality as proof that no transient write occurred. Runtime values may establish pointer alias/disjoint relations; decoded callee write provenance must still establish the memory-effect contract.

No PAGE_GUARD/data-page protection changes and no invocation of the callee by the mod.

## 6. Resume writer proof only after callee alias PASS
Only if:
```text
all_relevant_write_aliases_resolved=1
source_span_no_write_proven=1
transform_no_write_proven=1
```
may c17 feed a compatible call effect into the existing branch-aware writer proof at return `0x2CFC92C`.

Then require exact stores:
```text
0x2CFC936 -> +0x30
0x2CFC93C -> +0x34
0x2CFC942 -> +0x38
0x2CFC945 -> +0x3C
```

Update:
```text
MOVEMENT_WRITER_CALL_CONTRACT
MOVEMENT_RDX_BRANCH_CONTRACT
MOVEMENT_RDX_XYZ_PROOF
MOVEMENT_POSITION_W_PROOF
```

If the proof reaches another exact writer blocker after `0x2CFC92C`, report it and stop. W remains independent and requires decoded provenance.

## 7. Preserve upstream safety boundary
Even if writer XYZ/W causality becomes complete in c17:
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```
remain unchanged. A later separate task may inspect `0x351EB00`; c17 must not.

## 8. Preserve all prior protections
Do not regress c7-c16 exact-build/lifecycle/orientation/stale recovery/hard-kill attribution/secondary admission/publication refresh/Position DR watch/direct writer call/entry pairing/final accounting/branch proof/call-target identity and cleanup.

Keep `Position_writes_by_mod=0`.

## 9. Prohibited in c17
No Actor/Transform Position write, RDX/stack-source mutation, input rewrite, custom movement/velocity/gravity, active adhesion, upstream producer hook/traversal, camera/animation/traversal work, PAGE_GUARD/data-page protection mutation, guessed game ABI calls, raw user-log publication or complete private game-body distribution.

## Tests
Add owned/offline tests for:
- callee private-stack write proven disjoint;
- exact source-span write => reject;
- exact Transform write => reject/classify;
- argument pointer copied through LEA/MOV and then written;
- RIP-relative global write proven separate;
- two branches reconverging with same address provenance;
- conflicting address provenance => unknown/refuse;
- caller shadow/home/return-area write handling;
- unknown pointer write exact blocker;
- nested call/indirect transfer exact blocker;
- writer proof resumes after alias-safe call and reaches exact XYZ/W stores in owned fixture;
- all c16 regressions and cleanup unchanged.

## Human test
If actual-game evidence remains necessary after cloud validation, use the same low-friction test:
```text
A/cardinal ~4 s
-> fully end WallRun
-> move or fast-travel
-> E/diagonal ~4 s
-> send one complete WallSprint.log
```
No special input. Video only for visible anomaly.

## PASS
Diagnostic c17 PASS is either:

### A. Callee alias resolved
All relevant callee writes are classified relative to source/Transform, source/Transform no-write is proven, and writer proof advances to exact stores or a strictly later exact writer blocker.

### B. Narrower callee blocker
One exact callee RVA/mnemonic/address-provenance ambiguity is identified with all regressions/safety gates preserved.

Do not implement movement automatically.

## Required outputs
Update/create:
```text
analysis/V01C16_RUNTIME_RESULT.md
analysis/V01C17_CALLEE_ALIAS_CONTRACT.md
analysis/V01C17_GATE.md
analysis/RDX_XYZ_SOURCE_CONTRACT.md
README_TEST.md
docs/PROJECT_STATE.md
docs/CODEX_RESULT.md
```
Package c17 diagnostic release/source per existing project rules.
