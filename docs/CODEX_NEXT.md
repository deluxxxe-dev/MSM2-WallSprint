# Next task — WallSprint v0.1c16 Writer Call-Effect Contract

## Base
```text
source branch: codex/v01c15-writer-branch-contract
source implementation: 0efd13924426b1c96247ff35577f7f323c934b97
source delivery head: 8e500b2e5f4967457f8652f46e18eb2c6d4fcb7d
next branch: codex/v01c16-writer-call-effect
version target: v0.1c16
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `analysis/V01C15_RUNTIME_RESULT.md`, `analysis/V01C15_WRITER_BRANCH_CONTRACT.md`, `analysis/RDX_XYZ_SOURCE_CONTRACT.md`, and c15 `docs/CODEX_RESULT.md` first.

## Accepted c15 runtime
A/session1 3860 ms and E/session2 4297 ms both retained Surface Up through verified Deinit, with hard kill0, stale0, Position writes0 and clean entry/watch restoration.

Finalized capture summaries are now correct in both sessions:
```text
capture_activated=1
true_entry_snapshots=8
complete_invocation_maps=8
xyz_source_candidate=20
xyz_match_count=8
xyz_match_mask=0x7
xyz_stable_across_invocations=1
source_w_bits=0x0
destination_w_bits=0x3f800000
phase=finalized
summary_final=1
```

C15 crosses the prior local branch barrier `0x2CFC91F jne`. The next exact blocker is:
```text
MOVEMENT_RDX_BRANCH_CONTRACT
first_branch_rva=0x2CFC91F
exact_store_sites_verified=1
all_relevant_store_paths_proven=0
proven_component_mask=0x0
next_barrier_rva=0x2CFC927
next_barrier_instruction=call
W_independent=1
reason=unsupported_relevant_writer_instruction_or_memory_effect
```

Per actual invocation:
```text
RDX == entry RSP+0x28
entry_store_xyz_match_mask=0x7
xyz_causality_proven=0
source W observed 0
destination W observed 1
```
The equality is corroboration only.

Historical source/storage/producer fields remain unpromoted:
```text
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
exact_next_producer_rva=0x351EB00
writable_source_proven=0
```

## Objective
Resolve **only the exact call effect at writer RVA `0x2CFC927`** so the bounded writer proof can either continue to exact Position stores or stop at a narrower exact blocker.

Do not inspect/mutate upstream producer `0x351EB00` yet. Do not implement movement.

## Exact call authentication
Under exact build/SHA/writer boundary/current-code identity, decode `0x2CFC927` and classify its form:
```text
direct_call | indirect_call | thunk/wrapper | unresolved
```
For a direct call, authenticate its exact in-image target and runtime-function boundary/hash. For indirect calls, fail closed unless the stable target can be established without arbitrary pointer chasing.

Emit:
```text
MOVEMENT_WRITER_CALL_CONTRACT
writer_rva=0x2cfc900
call_rva=0x2cfc927
call_kind=
target_rva=
target_boundary_begin=
target_boundary_end=
target_code_hash=
return_rva=0x2cfc92c
exact_build_verified=
current_code_verified=
relevant_to_xyz_paths=
reason=
```
Do not publish the full private callee body.

## Relevant effect contract only
Determine whether the natural game call can affect provenance needed for:
```text
entry [RDX+0] -> Transform +0x30
entry [RDX+4] -> Transform +0x34
entry [RDX+8] -> Transform +0x38
```
and independently the source of `0x2CFC945 -> +0x3C`.

Classify effects on:
- entry RDX pointer identity;
- guarded 16-byte `[entryRDX, entryRDX+0x10)` source span;
- registers/lanes used after return to carry/reload XYZ;
- relevant stack aliases;
- entry RCX/Hero Transform destination identity;
- W provenance separately.

Use conservative Win64 volatile-register rules, but never infer memory safety from ABI alone.

## Bounded static proof first
If the authenticated callee is suitably bounded, implement a separate conservative effect classifier only as far as needed to answer the source-span/provenance question. Suggested cap: <=1024 bytes unless the exact runtime boundary is smaller. No recursion, no indirect-call expansion, no arbitrary interprocedural traversal, no open-ended fixpoint. Unknown memory writes refuse.

Classify only with explicit evidence:
```text
source_span_no_write_proven
source_span_may_write
source_span_effect_unknown
```
Do not assign gameplay semantics to the helper.

## Minimal runtime observation if static proof is insufficient
Only if necessary, observe the game's natural call without changing arguments/return/flags/native behavior. Keep the existing 8 eligible invocations/session cap.

Preferred bounded record:
```text
MOVEMENT_WRITER_CALL_OBSERVATION
session=
invocation_id=
thread_id=
call_rva=0x2cfc927
target_rva=
entry_rdx=
source_before_bits=(4x32)
source_after_bits=(4x32)
source_span_unchanged=0|1
selected_relevant_gpr_before/after=
selected_relevant_xmm_before/after=
return_observed=1
code_integrity_verified=1
```

No arbitrary memory dump, PAGE_GUARD, or game call from the mod. A new exact callee diagnostic hook is allowed only if no safer writer-local observation works; then require return address `0x2CFC92C`, active Hero/session/thread/generation/epoch/freshness, exact code ownership/restoration and transparent ABI preservation.

## Resume writer proof only after call effect is resolved
If the call is proven compatible with the needed provenance, resume c15 branch-aware proof at return `0x2CFC92C` and require exact stores:
```text
0x2CFC936 -> +0x30
0x2CFC93C -> +0x34
0x2CFC942 -> +0x38
0x2CFC945 -> +0x3C
```
XYZ is proven only if every relevant store-reaching path retains authenticated entry-RDX origin. W remains independent and must be classified from actual decoded provenance.

If another writer instruction blocks, report exact RVA/mnemonic/effect and stop. If the call target itself is the smallest unresolved scope, report the exact callee barrier/effect and stop.

## Preserve / prohibit
Preserve all c7-c15 lifecycle/orientation/refresh/DR-watch/direct-call/entry-pairing/accounting/branch-proof gates and `Position_writes_by_mod=0`.

No Actor/Transform Position write, RDX/stack-source mutation, Path A Position mutation, input rewrite, custom movement/velocity/gravity, active adhesion, producer hook/traversal at `0x351EB00`, camera/animation/traversal work, PAGE_GUARD/data-page protection mutation, guessed native game calls, raw user logs or full private game bodies.

`writable_source_proven=0` remains unconditional in c16.

## Tests
Cover direct call/no source effect, volatile clobber followed by safe reload, source XYZ mutation rejection, unknown memory write refusal, indirect target refusal, branch+call reconvergence, callsite/return mismatch, independent W classifications, transparent observation ABI/cleanup, and all c15 regressions.

## Human test
If actual-game evidence is still required:
```text
A/cardinal ~4 s -> fully end -> move/fast travel -> E/diagonal ~4 s -> one complete WallSprint log
```
No special input. Video only for a visible anomaly.

## PASS
Diagnostic PASS is either:
- exact call target/effect resolved and writer proof advances to exact stores or a strictly later exact writer blocker; or
- a precise call/callee effect blocker narrower than c15, with safety preserved.

Do not implement movement automatically.

## Required outputs
Update/create:
```text
analysis/V01C15_RUNTIME_RESULT.md
analysis/V01C16_WRITER_CALL_EFFECT.md
analysis/V01C16_GATE.md
analysis/RDX_XYZ_SOURCE_CONTRACT.md
README_TEST.md
docs/PROJECT_STATE.md
docs/CODEX_RESULT.md
```
Package the c16 diagnostic release/source per existing project rules.
