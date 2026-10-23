# Next task — WallSprint v0.1c16 Writer Call-Effect Contract

## Base
```text
source branch: codex/v01c15-writer-branch-contract
source implementation: 0efd13924426b1c96247ff35577f7f323c934b97
source delivery head: 8e500b2e5f4967457f8652f46e18eb2c6d4fcb7d
next branch: codex/v01c16-writer-call-effect
version target: v0.1c16
```

Read first:
- `AGENTS.md`
- `docs/DEV_RULES.md`
- `docs/PROJECT_STATE.md`
- `analysis/V01C15_RUNTIME_RESULT.md`
- `analysis/V01C15_WRITER_BRANCH_CONTRACT.md`
- `analysis/RDX_XYZ_SOURCE_CONTRACT.md`
- c15 `docs/CODEX_RESULT.md`

## Human c15 evidence
Actual A/E runtime is accepted as a diagnostic PASS with no write authorization.

Regression gate:
```text
A/session1 wallrun_duration_ms=3860
E/session2 wallrun_duration_ms=4297
both verified Deinit
both retention_pass=1
both hard_kill_calls=0
both stale_suspend_count=0
both Position_writes_by_mod=0
```

Final capture summaries are now correct for each session:
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

C15 crosses the prior local branch barrier `0x2CFC91F jne`. The exact next blocker is:
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
The runtime equality is corroborating evidence only.

Historical producer/storage evidence remains unpromoted:
```text
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
exact_next_producer_rva=0x351EB00
writable_source_proven=0
```

## Objective
Resolve **only the exact call effect at writer RVA `0x2CFC927`** so the bounded writer proof can either continue to the four known Position stores or stop at one still narrower exact blocker.

Do not inspect/mutate the upstream producer `0x351EB00` yet. Do not implement movement.

## 1. Authenticate the exact call instruction and target
Under the existing exact-build / writer-boundary / full-current-code hash gates, decode the instruction at:
```text
0x2CFC927
```
Classify:
```text
direct_call | indirect_call | thunk/wrapper | malformed/unresolved
```
For a direct call, compute and verify the exact in-image target RVA. For an indirect call, identify only the bounded operand form and fail closed unless a stable exact target can be independently authenticated without arbitrary pointer chasing.

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
Do not log the full private callee body.

## 2. Determine only the call effects relevant to the later Position stores
The proof question is narrow:

> Can this call alter the source/provenance needed to prove entry `[RDX+0/+4/+8]` reaches exact stores `0x2CFC936/93C/942`, or alter the independent source feeding W store `0x2CFC945`?

Do not try to recover general callee semantics.

At minimum classify relevant effects on:
- entry RDX pointer identity;
- guarded 16-byte source `[entryRDX, entryRDX+0x10)`;
- registers/lanes that carry or reload XYZ after return;
- any stack slot alias used by the writer after return;
- entry RCX / Hero Transform pointer if the call can affect destination identity;
- W provenance separately.

Use conservative Win64 call-clobber rules for volatile registers, but do not infer memory safety from ABI alone.

## 3. Prefer bounded static effect proof when possible
If the exact callee target is small enough and has a trustworthy runtime-function boundary, add a separate bounded static effect classifier limited to what is necessary to answer the source-memory/provenance question.

Suggested hard limits:
```text
callee bytes <= 1024 unless existing exact boundary is smaller
no recursion
no indirect-call expansion
no arbitrary interprocedural traversal
no open-ended fixpoint
no unknown memory writes promoted as safe
```

You may classify:
```text
source_span_no_write_proven=1
source_span_may_write=1
source_span_effect_unknown=1
```
only from explicit decoded memory effects/provenance. Unknown pointer writes must refuse.

Do not call the helper a movement/transform/physics function without independent evidence.

## 4. If static effect proof is insufficient, add the smallest read-only runtime call observation
Only if needed, instrument the existing authenticated writer invocation around this exact call without changing arguments, return value, flags or native behavior.

Preferred evidence per eligible Hero-targeted invocation:
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
rcx_before/after=
rdx_before/after=
selected_relevant_gpr_before/after=
selected_relevant_xmm_before/after=
return_observed=1
code_integrity_verified=1
```

Keep this bounded to the existing max 8 eligible entry invocations/session. Do not dump arbitrary stack/process memory.

No PAGE_GUARD or data-page protection mutation. Do not invoke the callee from the mod; only observe the game's natural call.

A new exact callee detour/hook is permitted only if there is no safer way to observe the natural call, and only with strict return-address `0x2CFC92C`, active Hero/session/thread/generation/epoch/freshness filters, transparent ABI preservation, exact code ownership/restoration and bounded records. Prefer a writer-local observation if feasible.

## 5. Re-enter the existing branch-aware writer proof after the call
If and only if the call effect is proven compatible with the necessary source/provenance, resume the existing c15 branch proof from return `0x2CFC92C` and require the same exact target stores:
```text
0x2CFC936 -> Transform +0x30
0x2CFC93C -> Transform +0x34
0x2CFC942 -> Transform +0x38
0x2CFC945 -> Transform +0x3C
```

XYZ may become proven only when every relevant path reaching each store has authenticated origin:
```text
entry [RDX+0] -> +0x30
entry [RDX+4] -> +0x34
entry [RDX+8] -> +0x38
```

W remains independent and must be classified from actual decoded provenance.

Emit/update:
```text
MOVEMENT_RDX_BRANCH_CONTRACT
MOVEMENT_RDX_XYZ_PROOF
MOVEMENT_POSITION_W_PROOF
```

If another local instruction becomes the blocker, report its exact RVA/mnemonic/effect and stop. If the call target itself contains the smallest unresolved effect, report the exact callee RVA/mnemonic/memory-effect class and stop.

## 6. Do not advance to producer ownership in c16
Keep these fields historical/unpromoted:
```text
exact_next_producer_rva=0x351EB00
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```

Even a full writer XYZ/W certificate does not prove the upstream source is safe to mutate.

## 7. Preserve all prior protections
Do not regress:
- exact exe/SHA fail-closed gate;
- c7 stale recovery;
- c8 immutable first hard-kill attribution;
- c9 secondary Path B admission;
- c10 bounded publication refresh;
- c11 Position watch / DR cleanup;
- c12 exact direct writer callsite proof;
- c13 true entry / invocation pairing;
- c14 final aggregate accounting;
- c15 branch-aware local CFG proof and finalized capture summary;
- orientation Path A/B behavior;
- `Position_writes_by_mod=0`.

## 8. Safety / prohibited in c16
No:
- Actor/Transform Position write;
- RDX or stack-source mutation;
- Path A Position mutation;
- controller/keyboard input hook or rewrite;
- custom movement/velocity/gravity;
- active adhesion correction;
- producer hook/traversal at `0x351EB00`;
- camera/animation/traversal features;
- PAGE_GUARD/data-page protection mutation;
- guessed native game ABI calls;
- raw user logs or complete private function bodies in GitHub/package.

`writable_source_proven=0` remains unconditional in c16. Do not force it positive.

## Tests
Add owned/offline tests for:
- direct call with irrelevant/no source-span effect and successful continuation;
- call clobbering volatile carrier but writer reloads XYZ from unchanged source => handled correctly;
- call mutating source X/Y/Z => reject affected provenance;
- unknown pointer memory write => fail closed;
- indirect/unresolved target refusal;
- nested local branch + call + reconvergence;
- exact callsite/return mismatch refusal;
- W independent constant/register/memory/computed handling after the call;
- concurrent/nested invocation separation if runtime observation is added;
- transparent call observation ABI/flags/nonvolatiles/XMM/return preservation;
- cleanup/restoration and all c15 regressions.

## Human test
If runtime evidence is required after cloud validation, retain the low-friction sequence:
```text
1. A/cardinal ~4 s
2. fully end WallRun
3. move or fast travel to E
4. E/diagonal ~4 s
5. send one complete WallSprint log
```
No special input. Video only for a visible anomaly.

## PASS
Diagnostic c16 PASS is either:

### A. Writer call effect resolved and writer proof advances
```text
exact call target/form authenticated
relevant call effect classified
all relevant XYZ store paths proven OR a strictly later exact writer blocker reported
W independently classified if reachable
```

### B. Exact call/callee blocker
A precise target/effect/RVA is identified, safety is preserved, and the unresolved scope is narrower than c15.

Do not implement movement automatically under either outcome.

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
