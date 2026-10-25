# Next task — classify exact SHL at 0x2CF7FF7 after accepted c33

## Status

The v0.1c33 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c33-20261025-152750(1).log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

See `analysis/V01C33_RUNTIME_RESULT.md`.

## Accepted c33 facts

Prerequisite chain remains accepted:

- c25: full authenticated 527161-entry runtime-function table validation; exact target `0x2CF7FE0` has no containing runtime-function entry.
- c26: exact `cmp r8d,0x3FFF`, flags-only, relevant tags zero.
- c27: exact short `JA`; both exact forward outcomes remain mandatory.
- c28-c30: exact `mov rax,[rcx+0x100]` is modeled only as an opaque unknown loaded qword with `RAX tags=0x7`, no value read/log/numeric interpretation.
- c31-c32: exact `VEX2.256 VMOVUPS YMM0 <- m256 [RDX]` is modeled under a pinned 256-bit dependency state with `YMM0.low128=0x7`, `YMM0.high128=0x7`, `above256_known_zero=1`, no vector-value reads.

C33 exactly classified and modeled:

```text
site_rva=0x2CF7FF4
instruction=movsxd
bytes=3
encoding=REX_W_opcode63
source=r8d
source_bits=32
source_parent=R8
source_parent_may_tags=0x0
destination=r8
destination_bits=64
full64_replacement=1
memory_read=0
memory_write=0
memory_RMW=0
flags_preserved=1
SP_write=0
vector_writes=0
semantic_modeled=1
result_may_tags=0x0
sign_rule=source_bit31_replicated_to_destination63_32
no_source_numeric_interpretation=1
no_value_reads=1
raw_values=0
reason=target_MOVSXD_pinned_reg32_signextend_full64_modeled
```

The bounded walk advances to:

```text
barrier_rva=0x2CF7FF7
barrier_instruction=shl
reason=target_cfg_instruction_effect_refused
path=0
edge_from_rva=0x2CF7FE7
edge_target_rva=0x2CF7FE9
```

Pre-barrier state:

```text
RAX/GPR0 tags=0x7
R8 tags=0x0
other logged GPR tags=0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags_may_depend_tags=0x0
```

Current target state:

```text
admitted=0
instructions=6
LEAF_CFG complete=0
return_paths=0
unique_instructions=6
visits=6
final_code_rechecked=0
effect_certificate=0
```

The current log establishes only the mnemonic/refusal for `SHL`. Do not infer operand width, destination, count source/count value, flag writes, or memory use from the mnemonic.

## Current bounded Codex task

Implement the smallest fail-closed extension needed to classify the exact `SHL` at `0x2CF7FF7` and, only if its exact data/flag effect is representable under the existing dependency model, continue to the next unsupported boundary.

### 1. Classify exact SHL before adding semantics

Emit compact metadata sufficient to establish:

- exact instruction length and encoding;
- operand count;
- exact destination operand class and width;
- exact shift-count source: implicit 1, immediate, or CL;
- exact encoded count width/value when immediate;
- x86 architectural count masking for the destination width;
- decoded read/write access for each operand;
- whether memory is involved;
- if memory exists: base/index/scale/signed displacement/segment/address width and exact alias/provenance;
- exact destination parent GPR and whether write is full64, zero-extending 32-bit, or partial;
- incoming selected/postwrite/other-prewrite tags on destination and count source;
- exact flags written, preserved, or architecturally undefined for this count class;
- whether flag outputs depend on destination/count tags;
- vector/SP/memory effects;
- current-code/snapshot stability.

Do not log raw numeric register/memory values merely to choose shift behavior. Do not assign semantic ABI/type names.

### 2. Count semantics must be explicit

Do not model a generic SHL rule by mnemonic alone.

For the actual operand width, implement the architectural masking rule exactly. Examples must be derived from the decoded width rather than assumed globally.

If the effective count cannot be proven from the encoding without observing a live numeric register value, do not guess it. A CL-count form must remain fail-closed unless the dependency abstraction can conservatively model all possible masked counts and flag semantics soundly without value observation.

### 3. Flag semantics are part of admission

SHL changes flags. Admission requires a sound dependency treatment for every flag later consumed by the bounded proof.

At minimum distinguish:

- count=0: destination and flags architecturally unchanged;
- count=1: CF and OF have defined shift-specific semantics; SF/ZF/PF derive from result; AF is undefined;
- count>1: CF/result/SF/ZF/PF semantics as architecturally defined for valid masked counts; OF is undefined; AF is undefined;
- counts outside operand width or implementation-defined/undefined result cases must be handled according to x86-64 architectural rules, not by guessed arithmetic.

The analyzer may conservatively mark undefined flags as unknown/top only if that remains sound for all downstream consumers. If the current flag model cannot represent the required uncertainty, refuse.

### 4. Model only the exact pinned form if sound

If the actual SHL is a supported register form and the effective count is statically known from encoding, add only the pinned semantics required at `0x2CF7FF7`.

Dependency propagation should be conservative:

```text
result tags <- destination input tags OR count-source tags when the count source itself can influence the selected outcome
```

For an immediate/implicit constant count, do not add count tags.

Apply exact destination-width semantics:

- 64-bit destination: full64 replacement;
- 32-bit destination: architectural zero-extension to 64-bit parent;
- 16/8-bit destination: partial-write semantics must be represented exactly or refuse.

No stale destination-tag union after a full architectural replacement.

Memory destination/source forms remain refusal unless already covered by a separately proven rule; do not add a generic memory RMW shift model in this task.

### 5. Preserve accepted c30-c33 state

Before the instruction:

```text
RAX tags=0x7
R8 tags=0x0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
other logged GPR/vector tags=0
flags tags=0
```

SHL support must not accidentally narrow/clear unrelated c30/c32 state.

### 6. Resume the c27 all-path walk only after exact admission

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

After an admitted SHL, continue only to the next unsupported instruction.

Report:

- exact RVA/mnemonic;
- path/edge;
- exact refusal reason;
- pre-instruction GPR tags;
- vector low/high state for relevant parents;
- pre/post flag dependency state where modeled.

Do not opportunistically implement the next new instruction unless it is already covered by an existing proven rule.

### 7. Existing refusals remain

Still refuse on:

- memory store/RMW not separately proven;
- unresolved/dynamic memory address;
- stack construction/change;
- nested CALL;
- indirect transfer;
- another branch beyond the authorized JA;
- loop/backedge;
- unsupported partial GPR/vector effect;
- unsupported >256-bit vector use;
- unrepresentable undefined-flag state;
- runtime-region crossing;
- snapshot/code mismatch;
- cap exhaustion or unresolved exit.

### 8. Preserve all safety gates

Still forbidden:

- Position/source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- raw memory/vector/register-value logging for numerical narrowing;
- generic shift/memory/vector semantics;
- guessed ABI/type names;
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

### 9. Owned tests / handoff

Add fixtures covering at minimum:

- exact SHL operand/access/width/count-source classification;
- implicit-1/immediate/CL distinction;
- exact count masking for relevant widths;
- destination full64/zero-ext32/partial-write distinction;
- register-form tag propagation;
- count=0 no-effect behavior;
- count=1 CF/OF and result SF/ZF/PF dependency behavior;
- count>1 undefined OF/AF handling;
- no stale destination tags after full replacement;
- CL-count refusal if numerical count is required and not statically known;
- memory RMW refusal;
- wrong site/width/destination/count/form refusal;
- c33 MOVSXD regression;
- c32 vector256 regression;
- c31 VMOVUPS classification regression;
- c30 qword-top regression;
- c29 writable `.data` tag0 refusal;
- c28 MOV, c27 JA, c26 CMP, c25 runtime table, c24 no-unwind and c23-c19 regressions.

Run the relevant full build/regression chain. Update `analysis/`, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable Wisdoms facts when appropriate.

If exact SHL metadata can be derived from the authenticated current-code snapshot, do not request runtime by habit. If parent-state/count information or the next barrier genuinely requires live evidence, emit only the minimum SHL/flag/next-barrier/CFG/tag/safety records and request one shortest A/E-style capture. Commit and push all authorized changes to the current development line.
