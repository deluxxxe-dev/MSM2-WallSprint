# Next task — bounded call-effect classification at 0x2B235D4 after accepted c22

## Status

The v0.1c22 actual-game A/E capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c22-20261025-021717.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history unless this task deliberately creates and fully hands off a successor branch.

## Accepted c22 facts

The previous `0x2B235BE` blocker is resolved for the authenticated analyzed state:

```text
0x2B235BE: test r8b,r8b
flags_only=1
parent R8 selected/same-field tags=0x0
flags_may_depend_tags=0x0
0x2B235C1: je
consumer_may_depend_tags=0x0
first_possible_dependent_branch_rva=0x0
returned_flags_tags=0x0
```

Therefore this exact TEST/JE pair does not carry the selected `0x2B23578` `Transform+0x10C` read value for the analyzed bounded state.

The selected-read walk now stops at:

```text
first_unresolved_rva=0x2B235D4
first_unresolved_instruction=call
reason=read_flow_call_or_control_no_expansion
```

Current unresolved outputs remain:

```text
complete_local_flow=0
returned_gpr_may_depend_mask=0x1
returned_xmm_may_depend_mask=0x0
returned_flags_may_depend=0
returned_caller_stack_may_depend=0
memory_escape=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
effect_certificate=0
writable_source_proven=0
```

See `analysis/V01C22_RUNTIME_RESULT.md`.

## Required reading

Before editing code, read:

1. `AGENTS.md`
2. `docs/DEV_RULES.md`
3. `docs/PROJECT_STATE.md`
4. `analysis/V01C22_RUNTIME_RESULT.md`
5. `analysis/V01C22_READ_TEST_FLOW.md`
6. `analysis/V01C21_RUNTIME_RESULT.md`
7. `analysis/V01C20_RUNTIME_RESULT.md`
8. `analysis/RDX_XYZ_SOURCE_CONTRACT.md`
9. relevant `source/src/NativeHooks/rdx_source_contract.cpp/.hpp` code/tests
10. `deluxxxe-dev/MSM2-Wisdoms/docs/WALLSPRINT_INTERNALS.md`
11. `deluxxxe-dev/MSM2-Wisdoms/docs/ENGINE_INTERNALS.md`

Project-local measured evidence remains authoritative over a stale shared summary.

## Current bounded Codex task

Implement the smallest evidence-preserving extension necessary to classify the exact call at `0x2B235D4` and continue the selected `Transform+0x10C` read-flow only until the next exact proof boundary.

### 1. Classify the exact callsite

For `0x2B235D4`, report compact metadata sufficient to answer:

- direct/indirect call form;
- exact target RVA if statically resolvable;
- authenticated target function boundary/hash if available under existing capture rules;
- call arguments at the analyzed state and which may carry tag `1` selected-read, tag `2` post-write same-field, or tag `4` other-prewrite same-field dependence;
- whether any argument may alias Hero Transform, the writer-entry RDX source span, caller/home stack, or another protected region;
- which volatile/nonvolatile return/clobber carriers matter to the selected-read walk.

Do not publish/reconstruct full proprietary native function bodies.

### 2. Expand only if required and bounded

If the call target is exact, authenticated, locally bounded, and expansion is necessary to decide selected-read effect, add one narrowly scoped nested-call effect analysis for this call only.

Required questions are limited to whether selected-read dependence can:

- return through GPR/XMM/flags;
- write/escape through caller/home stack or memory;
- reach the same Transform field or protected Position/source regions through a proven alias;
- alter a downstream branch/data path relevant to the writer XYZ/W contract.

Do not create a generic recursive interprocedural engine. Preserve strict caps and fail closed at unsupported instructions, aliases, indirect targets, recursion, nested calls beyond the explicitly admitted depth, or incomplete code identity.

### 3. Continue only to the next exact boundary

After the `0x2B235D4` call is classified, continue the selected-read walk only as far as the existing bounded flow safely permits.

Possible acceptable outcomes:

**A.** Call is proven disjoint from selected-read effect for the modeled scope; continue to the next exact blocker/return.

**B.** Call carries a narrow selected-read dependence relevant to protected downstream state; record the exact causal relation and stop before ownership/mutation work.

**C.** Exact target/alias/effect cannot be bounded; report the precise blocker and stop without guessing.

Even local completion does not settle `cross_invocation_feedback` or authorize ownership research/mutation.

### 4. Preserve existing gates

Still forbidden:

- Position writes;
- source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- guessed ABI/type names;
- arbitrary data-page mutation/PAGE_GUARD shortcuts;
- weakening ownership/lifetime/exclusivity requirements.

Safety state remains:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

### 5. Tests / handoff

Add owned fixtures covering at minimum:

- direct-call selected-read taint in an argument;
- direct-call disjoint arguments;
- return GPR carrying selected-read taint;
- clobbered/dead selected-read carrier;
- caller/home-stack memory escape;
- protected Transform/source-span alias refusal;
- unsupported/indirect/nested-call refusal;
- no regression of c22 TEST/JE disjoint classification;
- no regression of c21 exact same-field read;
- no regression of c20 EDI value provenance;
- no regression of c19 Transform/Position protection.

Run the relevant full build/regression chain. Update a successor analysis/result file, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable confirmed/corrected facts in `deluxxxe-dev/MSM2-Wisdoms` when accessible.

If current authenticated static/captured evidence suffices, do not request another runtime test merely by habit. If a new runtime capture is genuinely required, emit only the exact new fields needed and request the shortest A/E-style test necessary. Commit and push all authorized changes to the current development line.
