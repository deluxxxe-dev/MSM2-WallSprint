# Next task — classify exact fallthrough MOV at 0x2CF7FE9 after accepted c27

## Status

The v0.1c27 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c27-20261025-113718.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

See `analysis/V01C27_RUNTIME_RESULT.md`.

## Accepted c27 facts

The c25/c26 prerequisite chain remains accepted:

- authenticated PE runtime-function table fully validated: 527161 entries / 6325932 bytes;
- actual OS lookup queried;
- exact target `0x2CF7FE0` has no containing runtime-function entry;
- pinned no-unwind fallback is active;
- `0x2CF7FE0: cmp r8d,0x3FFF` is a supported read-only flags effect for the analyzed state;
- its relevant R8/flag tags are zero.

C27 classifies the exact `JA` and successfully performs the authorized bounded fan-out:

```text
site_rva=0x2CF7FE7
instruction=ja
encoding=short_JA_rel8
instruction_bytes=2
displacement=42
condition=CF0_and_ZF0
consumer_may_depend_tags=0x0
edges_exact=1
taken_rva=0x2CF8013
fallthrough_rva=0x2CF7FE9
taken_in_code=1
fallthrough_in_code=1
taken_in_allowance=1
fallthrough_in_allowance=1
taken_forward=1
fallthrough_forward=1
taken_runtime_cross=0
fallthrough_runtime_cross=0
snapshot_stable=1
control_expanded=1
numerical_outcome=unknown
outcome_narrowing=0
rule=pinned_JA_both_forward_paths
```

No branch direction is selected. Both outcomes remain possible.

The all-path target proof now fails at the first instruction on the fallthrough leg:

```text
barrier_rva=0x2CF7FE9
barrier_instruction=mov
reason=target_cfg_memory_operand_refused
```

Current CFG summary:

```text
complete=0
return_paths=0
unique_instructions=3
visits=3
branch_cap=1
path_cap=2
byte_cap=128
instruction_cap=32
visit_cap=64
all_paths_required=1
final_code_rechecked=0
effect_certificate=0
```

Parent selected-read flow correspondingly reports:

```text
first_unresolved_rva=0x2CF7FE9
first_unresolved_instruction=mov
returned_gpr_may_depend_mask=0x1
memory_escape=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
writable_source_proven=0
reason=target_cfg_memory_operand_refused
```

The log does **not** establish the exact MOV operands, width, read/write direction, address expression, memory region, alias class, or value provenance. Do not infer them from the mnemonic or refusal reason.

## Current bounded Codex task

Implement the smallest fail-closed extension needed to classify the exact `MOV` at `0x2CF7FE9` and, only if its memory effect can be proven safe under a narrowly justified read-only model, continue the existing two-path target proof to the next exact boundary.

### 1. Classify the exact MOV before adding any semantics

Emit compact diagnostic metadata for `0x2CF7FE9` sufficient to establish:

- operand count;
- source and destination operand classes (GPR/XMM/immediate/memory);
- exact operand widths;
- read/write access for each operand;
- exact register aliases and parent registers;
- for a memory operand: base, index, scale, signed displacement, segment, address width and effective address class/provenance derived from owned symbolic state;
- whether the memory operand is a read, write, or read-modify-write;
- whether the memory range could alias Hero Transform, `Transform+0x10C`, the writer source span, caller/home stack, callee-private stack, image globals/code, or an unknown region;
- incoming selected/post-write/other-prewrite tags on all register/address components;
- whether the operation changes flags;
- exact GPR/XMM/memory writes performed by the decoded form;
- snapshot/current-code stability for the instruction bytes used.

Do not log arbitrary memory contents merely to classify the instruction. Do not assign semantic ABI/type names from register indices.

### 2. Memory write / RMW remains an immediate refusal

If the exact MOV writes memory or has any read-modify-write behavior, stop at `0x2CF7FE9` with the exact reason.

Do not authorize a memory write because the destination appears disjoint from Position. This task is about completing the selected-read target effect proof, not granting new game-state writes.

### 3. Consider only a narrowly proven read-only load

If the exact form is a pure memory-to-register load, do **not** add a generic memory-load emulator.

It may be admitted only if all information needed for conservative tag/effect propagation is already provable from owned state and authenticated region metadata. At minimum require:

- exact address expression and width;
- no memory write;
- no stack-pointer mutation or hidden stack construction;
- no alias ambiguity with tracked mutable ranges that could carry selected/post-write/other-prewrite state;
- no arbitrary pointer chase or second memory dereference;
- no live memory-value observation used as a semantic shortcut;
- a justified source-region/value-provenance rule that can assign the loaded destination tags conservatively without guessing.

Examples of potentially admissible evidence are intentionally narrow: an authenticated immutable/read-only image source whose bytes/value provenance are already covered by the current-code/image proof, or another existing exact owned provenance class that can soundly determine the destination tags. Do not assume that an `image_global` name means immutable; prove the relevant PE protection/range and current snapshot rule.

If the source is dynamic memory, stack/home storage, Transform/hero data, unknown memory, pointer-derived memory with unresolved aliasing, or otherwise cannot receive a sound loaded-value tag classification, stop with Outcome C at this exact MOV.

Address-register tag dependence alone is not equivalent to loaded-value dependence. Keep address provenance and loaded-value provenance separate.

### 4. If admitted, model only the exact MOV form

For an admitted pure load:

- propagate only the exact destination register effect and correct x86 zero/upper-lane semantics for the decoded width;
- preserve all unaffected GPR/XMM state;
- do not invent ABI volatile clobbers;
- preserve flags if the exact MOV does not modify them;
- record whether selected/post-write/other-prewrite tags are introduced, preserved, or killed in the destination;
- record any path-specific effect separately.

Do not generalize this permission to arbitrary MOV memory forms or other sites.

### 5. Resume c27 all-path proof only after the MOV passes

The c27 contract remains unchanged:

```text
pinned target=0x2CF7FE0
branch cap=1
path cap=2
byte cap=128
unique instruction cap=32
visit cap=64
both JA outcomes required
all reachable paths must end in modeled normal RET
```

Continue from `0x2CF7FE9` only if its exact effect is admitted. The taken path at `0x2CF8013` must still be analyzed; no one-leg authentication is allowed.

Every reachable path must still refuse on:

- unsupported memory effect;
- stack construction/change;
- nested CALL;
- indirect transfer;
- another conditional/unconditional branch beyond the single authorized JA;
- loop/backedge;
- unsupported partial-register effect;
- runtime-region crossing;
- unstable/current-code mismatch;
- cap exhaustion or unresolved exit.

If the next exact barrier is reached after an admitted MOV, report its RVA/mnemonic/reason and stop there rather than broadening the model opportunistically.

### 6. Identity/effect admission still requires all outcomes

Target begin/end/size/hash/effect may be admitted only if **all** reachable c27 paths terminate in modeled normal RET and final snapshot/runtime coverage checks pass.

If target identity passes, feed only the fully admitted all-path effect set into the existing c23 parent CALL sidecar and conservatively merge returned GPR/XMM/flags/origins. Path-dependent effects must remain explicit.

Even successful local completion does not establish:

```text
whole writer XYZ/W causality
cross_invocation_feedback
caller/storage ownership
source lifetime/exclusivity
writability
```

### 7. Preserve all safety gates

Still forbidden:

- Position writes;
- source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- guessed ABI/type names;
- arbitrary data-page mutation/PAGE_GUARD shortcuts;
- arbitrary memory-value logging;
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

### 8. Owned tests / handoff

Add fixtures covering at minimum:

- exact MOV operand/access classification;
- pure memory-read vs memory-write/RMW distinction;
- exact base/index/scale/displacement and width reporting;
- address provenance separate from loaded-value provenance;
- unknown/dynamic memory read refusal;
- Transform/source/stack/home alias refusal;
- memory write/RMW refusal;
- authenticated immutable/read-only image load admission only if such a rule is actually implemented and fully proven;
- exact destination zero/upper-lane semantics for any newly supported load width;
- flags preserved by MOV when appropriate;
- selected/post-write/other-prewrite loaded-tag propagation;
- one path failing at the MOV still refuses the entire c27 target;
- c27 exact JA edges/fan-out regression;
- c26 CMP regression;
- c25 full large-table validation / genuine missing-entry regression;
- c24 no-unwind safeguards;
- c23 call carrier, c22 TEST/JE, c21 same-field read, c20 EDI provenance and c19 Transform protection.

Run the relevant full build/regression chain. Update a successor analysis/result file, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable confirmed/corrected facts in `deluxxxe-dev/MSM2-Wisdoms` when accessible.

If authenticated current-code/static evidence after adding MOV diagnostics is sufficient, do not request another runtime test by habit. If actual MOV operand/address metadata is genuinely unavailable outside the guarded live snapshot, emit only the minimal MOV/next-boundary fields and request the shortest A/E-style capture necessary. Commit and push all authorized changes to the current development line.
