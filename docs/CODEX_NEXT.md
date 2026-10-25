# Next task — prove or refuse exact image-backed load at 0x2CF7FE9 after accepted c28

## Status

The v0.1c28 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c28-20261025-122112.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

See `analysis/V01C28_RUNTIME_RESULT.md`.

## Accepted c28 facts

The prerequisite chain remains accepted:

- c25: authenticated PE runtime-function table fully validated, 527161 entries / 6325932 bytes; actual OS lookup queried; `0x2CF7FE0` has no containing runtime-function entry; next runtime region `0x2CF8020`.
- c26: `0x2CF7FE0: cmp r8d,0x3FFF`, read-only flags effect, relevant R8/flags tags zero.
- c27: `0x2CF7FE7: ja` exact short rel8, displacement 42, taken `0x2CF8013`, fallthrough `0x2CF7FE9`; both forward/in allowance/no runtime crossing; both outcomes explored without numerical narrowing.

C28 classifies the exact fallthrough instruction:

```text
0x2CF7FE9: mov rax, qword ptr [rcx+0x100]
instruction_bytes=7
operand_count=2
memory_read=1
memory_write=0
memory_RMW=0
pure_load_form=1
flags_preserved=1
full_GPR_writes=RAX
partial_GPR_writes=0
XMM_writes=0
SP_write=0
snapshot_stable=1
```

For the analyzed parent state the source address is exact:

```text
base=RCX
base_tags=0
base_origin_root=16
base_origin_offset=204285056
displacement=256
address_expression_exact=1
same_address_all_samples=1
address_root=16
address_offset=204285312
address_component_tags=0
address_class=image_address_mutability_unproven
alias_may_mask=0x20   # image only
image_range_known=1
current_code_range_known=0
```

The loaded value itself remains unresolved:

```text
readonly_value_proof=not_implemented
loaded_value_tags_known=0
loaded_may_tags_upper_bound=0x7
loaded_value_observed=0
unknown_not_untainted=1
semantics_admitted=0
reason=target_MOV_loaded_value_provenance_unproven
```

Incoming RAX carries selected tag1 before this full-width MOV. Do not kill tag1 merely because the address is in the image. Address provenance is not loaded-value provenance.

Current target state:

```text
admitted=0
barrier_rva=0x2CF7FE9
barrier_instruction=mov
LEAF_CFG complete=0
return_paths=0
unique_instructions=3
visits=3
final_code_rechecked=0
effect_certificate=0
writable_source_proven=0
```

## Current bounded Codex task

Implement the smallest fail-closed proof needed to determine whether this **one exact qword image load** can receive a sound loaded-value tag classification. Do not add a generic memory emulator.

### 1. Prove the source region before reading/using its value

For the exact effective image address already proven by c28, classify compact authenticated metadata sufficient to establish current mutability:

- exact image-relative source range and width (8 bytes);
- containing PE section/range and section characteristics;
- whether the section is readable/writable/executable;
- whether all 8 source bytes remain in that same admitted section/range;
- current mapped page allocation base/type/protection for the entire 8-byte range;
- whether any page in the range is writable, copy-on-write, guard, noaccess, or otherwise unsupported;
- whether the address falls in a known loader-mutated table/range that the implementation explicitly treats as unsafe (for example unresolved import/IAT/delay-load/TLS-style mutable metadata), if such ranges are already available from authenticated PE metadata;
- current image identity and range agreement with the exact admitted executable.

Do not infer immutability from an `image_global` label alone.

### 2. Add only a narrow current-proof stable-value rule if justified

A semantic load may be considered only if the exact source range is proven non-writable under both authenticated PE metadata and current mapped-page protection.

If justified, use a bounded stability check of exactly those 8 source bytes without publishing their raw value:

- guarded read of exactly 8 bytes before local effect analysis;
- guarded re-read after the bounded target walk / before final admission;
- equality required;
- report only equality/hash or another non-value compact identity, not the raw qword;
- no pointer chase and no second dereference;
- no numerical interpretation of the loaded bits.

The purpose is provenance/stability, not semantic interpretation.

If the page/section is writable, COW/guarded, crosses an unsupported boundary, cannot be authenticated to the admitted image, or the two bounded reads differ/fail, stop at `0x2CF7FE9`.

Current snapshot stability is not a global lifetime guarantee. State that scope explicitly.

### 3. Assign loaded tags conservatively

Only if the exact source passes the narrow immutable/current-stable rule above may the loaded qword be assigned tag0 for this current target proof.

Rationale must be structural:

- source address carries no relevant tags;
- source is in an authenticated non-writable image range;
- exact 8-byte value is stable across the bounded proof window;
- there is no second dereference or modeled memory write.

Do not use the numerical value itself to decide tags.

If any of these conditions is missing, loaded tags remain unknown upper bound7 and the MOV remains refused.

### 4. If admitted, model only this exact MOV

For this exact full64 GPR load only:

```text
RAX <- loaded tag set (expected 0 only if proven)
all other GPR/XMM tags unchanged
flags unchanged
memory unchanged
SP unchanged
```

Because this is a full 64-bit write, prior RAX selected tag1 is replaced only if the load itself is admitted.

Do not generalize to other memory MOVs, widths, sites, sections, or dynamic image data.

### 5. Resume the existing c27 all-path proof only after the MOV passes

Unchanged caps/contracts:

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

If the MOV is admitted, continue both paths to the next exact unsupported effect. Stop and report that exact RVA/mnemonic/reason rather than opportunistically widening semantics.

Still refuse on:

- any other unproven memory operation;
- stack construction/change;
- nested CALL;
- indirect transfer;
- any additional branch beyond the already authorized pinned JA;
- loop/backedge;
- unsupported partial-register effect;
- runtime-region crossing;
- snapshot/protection/value-stability mismatch;
- cap exhaustion or unresolved exit.

### 6. Identity/effect admission remains all-path only

Target begin/end/hash/effect may be admitted only if every reachable c27 path safely terminates in modeled normal RET and all final code/runtime/source-value checks still pass.

Only then feed the complete all-path effect set to the existing c23 parent CALL sidecar and conservatively merge returned tags/origins.

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
- arbitrary PAGE_GUARD/data-page mutation;
- raw memory-value logging;
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

- exact 8-byte source wholly inside non-writable PE section + non-writable mapped page;
- writable section refusal;
- writable/COW/guard/noaccess mapped-page refusal;
- cross-page/cross-section/overflow refusal;
- wrong image/allocation-base refusal;
- known mutable loader-table/range refusal if such classification is implemented;
- guarded first/second 8-byte read equality and mismatch/failure refusal without raw-value logging;
- tag0 assignment only after all structural/stability gates;
- no tag0 merely from image address or address-component tags0;
- full64 RAX overwrite kills prior selected tag only after admitted load;
- flags/other registers preserved;
- next exact barrier propagation after an admitted load;
- one unsafe JA leg still refuses whole target;
- c28 MOV operand/address regression;
- c27 JA fan-out regression;
- c26 CMP, c25 large-table, c24 no-unwind and c23-c19 provenance/protection regressions.

Run the relevant full build/regression chain. Update `analysis/`, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable Wisdoms facts when appropriate.

If authenticated static/current-code metadata is sufficient to prove/refuse the exact image source and determine the next barrier, do not request a runtime run by habit. If the current page protection/stable-value evidence exists only in the live process, emit the minimum new source-protection/stability/next-barrier records and request one shortest A/E-style capture. Commit and push all authorized changes to the current development line.
