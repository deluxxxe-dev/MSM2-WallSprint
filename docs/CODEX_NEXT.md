# Next task — conservatively model exact writable-image qword load after accepted c29

## Status

The v0.1c29 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c29-20261025-130401.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

See `analysis/V01C29_RUNTIME_RESULT.md`.

## Accepted c29 facts

The prerequisite chain remains accepted:

- c25: complete authenticated PE runtime-function table validation, 527161 entries / 6325932 bytes, queried OS lookup, exact target `0x2CF7FE0` has no containing runtime-function entry, next runtime region `0x2CF8020`.
- c26: exact `0x2CF7FE0: cmp r8d,0x3FFF` is a supported read-only flags effect for the analyzed state; relevant R8/flags tags zero.
- c27: exact short `JA` at `0x2CF7FE7`, displacement 42, taken `0x2CF8013`, fallthrough `0x2CF7FE9`; both forward/in allowance/no runtime crossing; both outcomes explored without numerical narrowing.
- c28: exact fallthrough instruction is `mov rax,qword ptr [rcx+0x100]`, a 7-byte pure 8-byte memory read, no memory write/RMW, full64 RAX write, flags/SP/other registers unchanged; source address exact for the analyzed parent state and image-only alias class.

C29 tested the narrow tag0/non-writable image-source route for the exact qword source:

```text
source_rva=0xC2D2580
width=8
section=.data
section_begin_rva=0x9B24000
section_end_rva=0xC565A28
section_characteristics=0xC0000040
readable=1
writable=1
executable=0
loader_overlap=0
```

Current mapped page:

```text
allocation_base_matches_image=1
range_in_image=1
region_begin_rva=0xC2D2000
region_bytes=2703360
state=0x1000
 type=0x1000000
protection=0x4  # PAGE_READWRITE
```

Therefore:

```text
policy_passed=0
reason=image_load_section_not_readonly
read_attempts=0
admitted=0
loaded_tags_known=0
raw_value_logged=0
```

The implementation correctly refused before reading the qword. Do not retry or weaken this exact tag0 proof: the source is structurally writable.

Current target status remains:

```text
admitted=0
barrier_rva=0x2CF7FE9
barrier_instruction=mov
LEAF_CFG complete=0
return_paths=0
unique_instructions=3
visits=3
effect_certificate=0
reason=image_load_section_not_readonly
```

## Current bounded Codex task

Implement the smallest sound extension needed to model **only this exact already-classified full64 load** as a conservative unknown-value effect, then resume the existing c27 all-path proof to the next exact unsupported boundary.

The goal is not to prove the loaded value is tag0. The goal is to preserve soundness by carrying the worst-case tracked dependency upper bound instead of refusing solely because the writable global's precise value provenance is unknown.

### 1. Admit only the exact pinned form and source

Eligibility must remain pinned to all accepted actual facts:

```text
site=0x2CF7FE9
instruction=MOV
form=RAX <- qword ptr [RCX+0x100]
width=8
memory_read=1
memory_write=0
memory_RMW=0
full64_RAX_write=1
flags_preserved=1
SP_write=0
source_rva=0xC2D2580
address_expression_exact=1
same_address_all_parent_samples=1
address_component_tags=0
alias=image-only for the analyzed parent state
```

Recheck the current instruction/source metadata before using the rule. Any form/address/width/access/current-parent disagreement must still refuse.

Do not make the rule apply to another MOV, another source RVA, another width, another image address, indexed/segmented forms, stores/RMW, partial registers, XMM loads, or dynamic pointer-chased memory.

### 2. Model loaded dependence as an explicit upper bound, not observed taint

For this exact pinned read only, assign:

```text
loaded_may_depend_tags = 0x7
```

where bits1/2/4 remain the existing selected/postwrite/other-prewrite categories.

This means **may depend on any tracked category**. It does not mean all three dependencies were observed or proven present.

Do not read or log the qword to choose a narrower tag set. Do not use numerical value equality, pointer interpretation, section contents, or runtime value samples to reduce `0x7`.

The abstract effect is exactly:

```text
RAX tags <- 0x7
RAX origin <- unknown_loaded_qword_from_exact_source
all other GPR tags unchanged
all XMM tags unchanged
flags unchanged
memory unchanged
SP unchanged
```

Because the MOV is a full64 write, prior RAX abstract contents are replaced by the unknown-loaded abstract value. However selected tag1 remains possible because `0x7` includes bit1. Do not describe this as killing selected dependence.

### 3. Separate target-effect soundness from value provenance precision

The successor proof may admit this instruction's **abstract effect** even though exact loaded-value provenance is unresolved, because the tag set is a conservative upper bound.

Required invariant:

> Every actual combination of tracked selected/postwrite/other-prewrite dependence for this loaded value must be a subset of `0x7`.

Do not infer semantic type, pointer identity, constantness, immutability, lifetime, or ownership from this abstraction.

If the implementation cannot guarantee that its tag lattice treats `0x7` as a true may-depend top element for these three tracked categories, stop and document why rather than adding the rule.

### 4. Resume the existing c27 two-path target walk

After the exact MOV is abstractly admitted, resume the unchanged bounded proof from the next instruction on its fallthrough path and continue the taken path at `0x2CF8013`.

Unchanged caps/contracts:

```text
pinned target=0x2CF7FE0
branch cap=1
path cap=2
byte cap=128
unique instruction cap=32
visit cap=64
both JA outcomes required
all reachable paths required
```

Stop at the first exact unsupported effect on either path and report:

- RVA;
- mnemonic;
- path/edge;
- exact refusal reason;
- current selected/postwrite/other-prewrite tag state at that boundary.

Do not opportunistically broaden support for the next instruction in the same task unless it is already covered by an existing admitted register-only rule.

### 5. Existing prohibitions remain

Still refuse on:

- any other memory operation not already explicitly admitted;
- memory write/RMW;
- stack construction/change;
- nested CALL;
- indirect transfer;
- another conditional/unconditional branch beyond the already authorized pinned JA;
- loop/backedge;
- unsupported partial-register effect;
- runtime-region crossing;
- unstable/current-code mismatch;
- cap exhaustion or unresolved exit.

The writable source page itself is not modified, guarded, watched or interpreted by the MOD.

### 6. Identity/effect handling

If all reachable paths eventually terminate in modeled normal RET with all existing final snapshot/runtime coverage checks passing, target begin/end/hash may be admitted under the conservative abstract effect model.

Returned dependency summaries must retain every possible tag implied by `0x7` and subsequent operations. Do not prefer a cleaner path or narrower tag result.

If the two paths have different return effects, preserve path dependence explicitly and merge by union.

Successful target identity/effect classification still does **not** establish:

```text
whole writer XYZ/W causality
cross_invocation_feedback
caller/storage ownership
source lifetime/exclusivity
writability
```

No gameplay write permission follows from local target completion.

### 7. Preserve all safety gates

Still forbidden:

- Position writes;
- source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- guessed ABI/type names;
- PAGE_GUARD/data-page mutation;
- raw memory-value logging;
- numerical interpretation of `0xC2D2580`;
- generic memory-load semantics;
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

- exact pinned full64 MOV + exact source accepts the unknown-top rule;
- loaded tags become exactly the lattice top `0x7` regardless of incoming RAX tags;
- previous RAX selected tag is replaced structurally, but selected remains possible through `0x7`;
- no numerical qword read/log needed for abstract admission;
- wrong source RVA/form/width/register/address expression refuses;
- store/RMW/partial/XMM/indexed/segmented/second-dereference forms refuse;
- address-component tags nonzero or conflicting parent source refuses if required by the current pinned-source contract;
- flags/other GPR/XMM/SP/memory preserved;
- next exact barrier is reported after this load rather than widened automatically;
- one unsafe JA leg refuses whole target;
- conservative union of path-dependent return tags;
- c29 writable `.data` tag0 refusal regression;
- c28 MOV classification regression;
- c27 JA fan-out regression;
- c26 CMP, c25 large-table, c24 no-unwind and c23-c19 provenance/protection regressions.

Run the relevant full build/regression chain. Update `analysis/`, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable Wisdoms facts when appropriate.

If the accepted c29 metadata and current authenticated code are sufficient to execute the abstract continuation statically/current-snapshot-side, do not request another runtime run by habit. If the next exact barrier can only be learned in the guarded live process, emit only the minimum next-barrier/CFG/tag/safety records and request one shortest A/E-style capture. Commit and push all authorized changes to the current development line.
