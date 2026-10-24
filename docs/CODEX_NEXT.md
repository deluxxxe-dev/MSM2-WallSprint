# Next task — classify/model exact CMP at 0x2CF7FE0 after accepted c25

## Status

The v0.1c25 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c25-20261025-042121.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

## Accepted c25 facts

The c24 table-cap ambiguity is resolved. The authenticated PE exception/runtime-function directory is structurally valid under the bounded c25 budget:

```text
directory_rva=0xC566000
directory_bytes=6325932
entry_bytes=12
derived_entries=527161
metadata_byte_cap=8388608
entry_cap=600000
byte_cap_exceeded=0
entry_cap_exceeded=0
entry_validation_skipped_by_cap=0
metadata_bytes_read=6325932
entries_validated=527161
all_entries_read_and_validated=1
table_structurally_valid=1
runtime_lookup_queried=1
```

For the exact direct-call target:

```text
target_rva=0x2CF7FE0
runtime_lookup_found=0
runtime_lookup_same_image=1
runtime_lookup_readable=1
next_runtime_rva=0x2CF8020
PE_runtime_table_valid=1
missing_runtime_entry=1
```

Therefore the exact target truly has no containing runtime-function entry under the authenticated current image, and the strict single-target no-unwind fallback legitimately starts.

The fallback obtains a stable bounded snapshot but stops on its first decoded instruction:

```text
method=strict_single_target_no_unwind_linear
scan_bytes=64
snapshot_stable=1
instructions=1
admitted=0
admitted_bytes=0
barrier_rva=0x2CF7FE0
barrier_instruction=cmp
reason=target_leaf_instruction_effect_not_admitted
```

The selected `Transform+0x10C` read dependence still reaches the parent CALL boundary in raw GPR carrier index 0 (`tags=0x1`, mem32 Hero Transform+0x10C). The selected-read flow now stops exactly at the nested target instruction:

```text
complete_local_flow=0
first_unresolved_rva=0x2CF7FE0
first_unresolved_instruction=cmp
returned_gpr_may_depend_mask=0x1
memory_escape=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
effect_certificate=0
writable_source_proven=0
```

See `analysis/V01C25_RUNTIME_RESULT.md`.

## Current bounded Codex task

Implement the smallest fail-closed extension necessary to classify the exact `CMP` instruction at `0x2CF7FE0` and, only if its read-only effects can be proven within the existing strict no-unwind contract, continue the single-target walk to the next exact proof boundary.

### 1. Classify the exact CMP form before adding semantics

Report compact metadata for the exact instruction at `0x2CF7FE0` sufficient to determine:

- operand count;
- operand classes (register / immediate / memory);
- operand widths;
- exact register aliases / parent GPRs when register operands are used;
- immediate value if present as an instruction constant;
- whether any operand is memory-addressed;
- which incoming selected/post-write/other-prewrite tags each read operand may carry;
- flags written by CMP;
- whether CMP writes any GPR/XMM/memory state (it should not, but prove the decoded form rather than assume from mnemonic alone).

Do not assign semantic argument/type names from register indices.

### 2. Add only the exact missing read-only CMP semantics if justified

The existing c23/c24 strict target rule remains the safety boundary. Do not turn this into a generic x86 emulator.

If the exact form is a register-only or register/immediate CMP whose read/flag effects can be modeled exactly under the owned Slice/State rules, add only that narrow form.

Requirements:

- no memory operand may be silently admitted;
- CMP must not be treated as modifying its operands;
- model the flags required by actual downstream consumers conservatively and exactly enough to preserve selected-read provenance;
- for 32/64-bit full-register reads, use exact parent-register tag flow;
- if the actual form is 8/16-bit or another partial-register read, either add a narrowly justified read-only parent-tag model analogous in spirit to c22 (without granting partial-register write semantics), or stop at the exact unsupported form;
- immediate operands carry no selected-read tag by themselves;
- do not infer the numerical branch outcome from tag analysis alone;
- preserve both possible control-flow outcomes whenever a later conditional consumer is encountered unless its dependence can be proven disjoint.

If the actual CMP has a memory operand, unsupported encoding/effect, ambiguous alias, or another form outside this narrow proof, report Outcome C and stop.

### 3. Continue the strict no-unwind identity walk only to the next exact boundary

After CMP is admitted, continue from exact fallthrough under the existing target constraints:

- pinned target `0x2CF7FE0` only;
- <=128 bytes / <=32 instructions;
- same stable guarded snapshot;
- no code crossing `next_runtime_rva=0x2CF8020` or another authenticated runtime region;
- no generic function finder;
- no memory operand unless already explicitly authorized by the strict rule (currently none);
- no stack construction/change;
- no nested CALL/indirect transfer;
- no unsupported partial effect;
- no unmodeled branch.

If the next instruction is a branch, log its exact RVA/mnemonic/condition/flag dependence and stop unless the already-approved strict rule explicitly supports it. Do not broaden c24's linear-leaf contract merely to force completion.

If the walk reaches a plain normal RET with every preceding instruction admitted and no alternate reachable exit, then and only then:

- admit exact begin/end/size/hash;
- recheck runtime-region coverage;
- pass the authenticated span into the existing c23 register-only call-effect sidecar.

### 4. Resume selected-read effect analysis only after target identity passes

If the target becomes authenticated, classify only whether tag `1` is:

- preserved into a returned GPR/XMM/flags state;
- killed/clobbered;
- escaped to caller/home stack or memory;
- blocked at another exact instruction/effect boundary.

Do not broaden into general recursive interprocedural analysis.

Even successful local completion does not settle:

```text
whole writer XYZ/W causality
cross_invocation_feedback
caller/storage ownership
lifetime/exclusivity
writability
```

### 5. Preserve all safety gates

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

### 6. Owned tests / handoff

Add fixtures covering at minimum:

- the exact newly supported CMP operand form;
- selected-tag and disjoint-tag CMP operands;
- immediate operand remains untainted;
- CMP modifies flags only, not operand registers;
- partial-register read handling if implemented;
- memory-operand CMP refusal;
- unsupported width/form refusal;
- next-instruction branch refusal / precise flag-dependence reporting under the unchanged strict linear rule;
- stable RET completion for an admissible linear target;
- no regression of c25 full large-table validation / genuine missing-entry result;
- no regression of c24 no-unwind identity safeguards;
- no regression of c23 call carrier, c22 TEST/JE, c21 same-field read, c20 EDI provenance and c19 Transform protection.

Run the relevant full build/regression chain. Update a successor analysis/result file, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable confirmed/corrected facts in `deluxxxe-dev/MSM2-Wisdoms` when accessible.

If authenticated static/current-code evidence after the CMP fix is sufficient, do not request another runtime test by habit. If runtime capture is genuinely required, emit only the minimum CMP/next-boundary fields and request the shortest A/E-style test necessary. Commit and push all authorized changes to the current development line.