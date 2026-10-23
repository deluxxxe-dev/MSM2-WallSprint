# Next task — WallSprint v0.1c17 Callee Alias Contract

## Base
```text
source branch: codex/v01c16-writer-call-effect
source implementation: 0a00bdeee05ffdae593d6963abd5c24424a2d8ef
source delivery head: 8f66b15a109e503cdfc115eb5a48ebadd7bc4113
next branch: codex/v01c17-callee-alias-contract
version target: v0.1c17
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `analysis/V01C16_RUNTIME_RESULT.md`, `analysis/V01C16_WRITER_CALL_EFFECT.md`, `analysis/RDX_XYZ_SOURCE_CONTRACT.md`, and c16 `docs/CODEX_RESULT.md` first.

## Accepted c16 actual-game result
Regression gate passed:
```text
A/session1 3875 ms
E/session2 4203 ms
both verified Deinit / retention pass / stale0 / hard0 / Position writes0
A Up dot surface=1
E Up dot surface=0.99999994
```
Both sessions finalized 8 true entries / 8 complete maps with XYZ mask0x7, source W0 and destination W1.

Authenticated writer call:
```text
writer=0x2CFC900..0x2CFC9B7
call=0x2CFC927
return=0x2CFC92C
kind=direct_call
target=0x2B23560
boundary=0x2B23560..0x2B235E9
hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
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
Actual `[entryRDX] XYZ == Position XYZ` remains corroboration only; `xyz_causality_proven=0`.

## Objective
Resolve only memory-write aliasing inside authenticated callee `0x2B23560..0x2B235E9` as used by writer call `0x2CFC927`.

For every relevant callee memory write, prove or reject disjointness from:
1. `[entryRDX, entryRDX+0x10)` — the 16-byte source object / writer-entry stack object;
2. the Hero Transform destination object/range needed by exact Position stores.

If all relevant writes are proven disjoint, resume the existing writer proof from `0x2CFC92C`. Otherwise report one exact callee RVA/mnemonic/address-provenance blocker and stop.

Do not inspect/mutate upstream producer `0x351EB00`. Do not implement movement.

## Required implementation
### Exact callee identity
Preserve exact executable/SHA, writer hash/current bytes, exact call/return, target boundary and target hash. Any mismatch fails closed. Do not log/package full private callee bytes.

### Bounded address provenance
Extend the c16 effect classifier only enough to classify effective addresses of callee memory writes. Track exact/simple provenance from call-time arguments, preserved aliases, callee RSP/allocation, RIP-relative addresses, LEA/MOV/add/sub/copy chains and bounded local branches.

Classify writes as:
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
Unknown/conflicting pointer provenance fails closed. No arbitrary interprocedural traversal, recursion, nested-call expansion or open-ended fixpoint. If a nested call/indirect/unsupported effect is encountered, emit its exact blocker.

### Map call-time arguments
At `0x2CFC927`, prove provenance of RCX/RDX/R8/R9 and needed stack args. Keep writer-entry RDX distinct from call-time RDX.

Emit:
```text
MOVEMENT_WRITER_CALL_ARGUMENT_CONTRACT
...
```
with argument origins, source/Transform alias masks, proof completeness and reason.

### Alias summary
Emit:
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

Only set source/Transform no-write when every relevant executable write is proven disjoint.

### Optional minimal runtime observation
Only if static caller-to-callee pointer identity remains ambiguous, observe the natural call for the existing max 8 eligible invocations/session. Allowed: call-time RCX/RDX/R8/R9, source address, Transform address, RSP relation, source before/after bits, exact pointer relations, return/code integrity. Runtime equality must not be used to rule out transient writes. No PAGE_GUARD and never invoke the callee from the mod.

### Resume writer proof only after alias PASS
Require:
```text
all_relevant_write_aliases_resolved=1
source_span_no_write_proven=1
transform_no_write_proven=1
```
before resuming from `0x2CFC92C` to exact stores `0x2CFC936/93C/942/945`. Update CALL_CONTRACT, BRANCH_CONTRACT, XYZ_PROOF and W_PROOF. Report the next exact writer blocker if one appears. W remains independent.

## Safety boundary
Even if writer causality completes:
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```
must remain. Producer `0x351EB00` is a later separate task.

Preserve all c7-c16 lifecycle/orientation/stale/hard-kill/secondary/publication-refresh/DR-watch/direct-call/entry-pairing/accounting/branch/call-target/integrity/cleanup protections.

No Position/RDX/source/input/movement/velocity/gravity/adhesion/camera/animation writes, producer traversal, PAGE_GUARD, guessed native game calls, raw user-log publication, or full private game-body distribution.

## Tests
Cover private-stack disjoint writes, source exact/possible alias, Transform exact/possible alias, argument-pointer copy chains, RIP-global writes, branch reconvergence/conflicting provenance, shadow/return area, unknown pointer, nested-call/indirect exact refusal, and owned writer continuation after an alias-safe call. Preserve all c16 regressions.

## Human test
If needed after cloud validation:
```text
A/cardinal ~4 s -> fully end WallRun -> move/fast-travel -> E/diagonal ~4 s -> one complete WallSprint.log
```
No special input; video only for visible anomaly.

## PASS
A. All callee write aliases resolved, source/Transform no-write proven, writer proof advances or reports a strictly later exact writer blocker.

B. One narrower exact callee address-provenance blocker is reported with all safety/regression gates preserved.

Do not implement movement automatically.

## Required outputs
```text
analysis/V01C16_RUNTIME_RESULT.md
analysis/V01C17_CALLEE_ALIAS_CONTRACT.md
analysis/V01C17_GATE.md
analysis/RDX_XYZ_SOURCE_CONTRACT.md
README_TEST.md
docs/PROJECT_STATE.md
docs/CODEX_RESULT.md
```
Package c17 diagnostic release/source per existing rules.
