# Next task — authenticate and bound target 0x2CF7FE0 after accepted c23

## Status

The v0.1c23 actual-game A/E capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c23-20261025-025936.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history unless this task deliberately creates and fully hands off a successor branch.

## Accepted c23 facts

The selected-read walk reaches an exact direct call:

```text
site_rva=0x2B235D4
form=direct_call
target_rva=0x2CF7FE0
exact_target=1
```

The selected `Transform+0x10C` read dependence is still present at the call boundary in raw GPR carrier index 0:

```text
gpr=0
gpr_tags=0x1
origin_kind=mem32
root=Hero Transform
offset=0x10C
```

Do not assign semantic ABI/type names to this raw carrier merely from its index.

The exact new blocker is target identity/boundary authentication:

```text
target_begin_rva=0
target_end_rva=0
boundary_verified=0
snapshot_authenticated=0
snapshot_sha256=unavailable
modeled_states=0
reason=read_call_target_boundary_hash_unavailable
```

Thus c23 did **not** expand the target and did not produce an effect certificate.

Current outputs remain:

```text
complete_local_flow=0
first_unresolved_rva=0x2B235D4
first_unresolved_instruction=call
returned_gpr_may_depend_mask=0x1
memory_escape=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
effect_certificate=0
writable_source_proven=0
```

Runtime safety remained clean across two A/E-style sessions: verified native Deinit, retention pass, MOD Position writes 0, hard kills 0.

See `analysis/V01C23_RUNTIME_RESULT.md`.

## Required reading

Before editing code, read:

1. `AGENTS.md`
2. `docs/DEV_RULES.md`
3. `docs/PROJECT_STATE.md`
4. `analysis/V01C23_RUNTIME_RESULT.md`
5. `analysis/V01C23_READ_CALL_FLOW.md`
6. `analysis/V01C22_RUNTIME_RESULT.md`
7. `analysis/V01C21_RUNTIME_RESULT.md`
8. `analysis/V01C20_RUNTIME_RESULT.md`
9. `analysis/RDX_XYZ_SOURCE_CONTRACT.md`
10. relevant `source/src/NativeHooks/rdx_source_contract.cpp/.hpp` code/tests
11. `deluxxxe-dev/MSM2-Wisdoms/docs/WALLSPRINT_INTERNALS.md`
12. `deluxxxe-dev/MSM2-Wisdoms/docs/ENGINE_INTERNALS.md`

Project-local measured evidence remains authoritative over a stale shared summary.

## Current bounded Codex task

Implement the smallest fail-closed extension necessary to determine why the exact target `0x2CF7FE0` lacks c23's required function-boundary/hash identity and, only if provable, authenticate one bounded no-unwind/leaf span sufficiently to resume the existing c23 register-only call-effect analysis.

### 1. Classify the boundary failure exactly

For target `0x2CF7FE0`, report compact metadata sufficient to distinguish at minimum:

- target outside admitted image/code region;
- runtime-function lookup has no entry containing the exact target;
- target falls inside an entry but is not its begin;
- runtime metadata malformed/ambiguous;
- span exceeds configured cap;
- guarded current-code snapshot unavailable/unstable;
- another exact identity refusal.

Do not silently treat missing unwind/runtime-function metadata as a valid function boundary.

Do not publish/reconstruct the whole proprietary target body.

### 2. Optional strict no-unwind leaf admission

Only if the exact direct target is inside the authenticated executable code region and the failure is specifically compatible with a function that has no runtime-function entry, add a **single-target** bounded fallback for `0x2CF7FE0`.

The fallback must fail closed and must not become a generic function finder.

A possible admissible shape is:

- start exactly at `0x2CF7FE0`;
- hard byte/instruction caps no looser than the existing c23 leaf caps unless justified by owned fixtures;
- decode from the exact target with no byte skipping;
- accept only a tiny linear or otherwise fully enumerated leaf whose end can be proven by modeled control flow ending in normal `RET`;
- no indirect transfer;
- no unresolved branch target;
- no nested CALL;
- no recursion;
- no unsupported instruction/effect;
- no unbounded stack-frame construction;
- no code crossing into another authenticated runtime-function region;
- guarded double-read/current-code equality before admitting a hash/span;
- record the exact admitted begin/end/size/hash and why the no-unwind proof is safe.

If any of those conditions fail, stop at the exact reason. Do not infer a function end from the first convenient `RET` unless the bounded control-flow proof establishes that it is the admitted target's complete modeled exit set.

### 3. Resume c23 effect analysis only after identity passes

If the target becomes authenticated under the strict bounded rule, run only the already-designed c23 register-only leaf effect model needed to answer whether tag `1` can:

- return through GPR/XMM/flags;
- be killed/clobbered;
- escape through caller/home stack or another memory effect;
- reach a protected Transform/source/Position alias;
- alter the parent selected-read flow after return.

Do not broaden into a generic recursive interprocedural engine.

Any memory operand, branch/call/stack effect outside the admitted model, alias ambiguity, cap, or unsupported instruction is the next exact blocker.

### 4. Continue only to the next exact proof boundary

Acceptable outcomes:

**A.** `0x2CF7FE0` is authenticated as a strict bounded leaf, its effect is modeled, and the selected-read walk advances to the next exact blocker/return.

**B.** The authenticated leaf proves a narrow selected-read dependence relevant to protected downstream state; record it and stop before ownership/mutation work.

**C.** Boundary/identity cannot be proven safely; report the precise reason and stop without guessing.

Even Outcome A does not settle `cross_invocation_feedback`, whole writer XYZ/W causality, storage ownership, lifetime, exclusivity or writability.

### 5. Preserve existing gates

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

### 6. Tests / handoff

Add owned fixtures for at minimum:

- exact direct target with missing runtime-function entry and safe strict leaf completion;
- missing entry with internal branch that is fully bounded/enumerated if supported, otherwise exact refusal;
- missing entry with nested CALL refusal;
- missing entry with memory/stack effect refusal under the register-only leaf model;
- first-RET-but-more-reachable-code trap proving no naive RET boundary inference;
- target overlapping/crossing an authenticated runtime-function region refusal;
- unstable guarded snapshot refusal;
- no regression of c23 direct-call target/carrier classification;
- no regression of c22 TEST/JE disjoint classification;
- no regression of c21 exact same-field read;
- no regression of c20 EDI value provenance;
- no regression of c19 Transform/Position protection.

Run the relevant full build/regression chain. Update a successor c24 analysis/result file, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable confirmed/corrected facts in `deluxxxe-dev/MSM2-Wisdoms` when accessible.

If authenticated static/current-code evidence suffices, do not request another runtime test merely by habit. If a new runtime capture is genuinely required, emit only the exact new fields needed and request the shortest A/E-style test necessary. Commit and push all authorized changes to the current development line.