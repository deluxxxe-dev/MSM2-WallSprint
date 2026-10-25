# Next task — add minimal sound 256-bit vector dependency state for exact VMOVUPS after accepted c31

## Status

The v0.1c31 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c31-20261025-141246.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

See `analysis/V01C31_RUNTIME_RESULT.md`.

## Accepted c31 facts

The prerequisite chain remains accepted:

- c25: full authenticated 527161-entry runtime table validation; exact target `0x2CF7FE0` has no containing runtime-function entry.
- c26: exact `cmp r8d,0x3FFF`, flags-only, relevant tags zero.
- c27: exact short `JA` at `0x2CF7FE7`; both exact forward outcomes are mandatory.
- c28-c30: exact `mov rax,[rcx+0x100]` is modeled only as an opaque unknown loaded qword with `RAX tags=0x7`, no qword read/log/numeric interpretation.

C31 exactly classified the next instruction:

```text
site_rva=0x2CF7FF0
instruction=VMOVUPS
bytes=4
encoding=VEX2
vector_bits=256
encoded_bits=256
operands=2
memory_read=1
memory_write=0
memory_RMW=0
pure_load_form=1
flags_preserved=1
GPR_writes=0
SP_write=0
```

Exact operand form:

```text
YMM0 <- 32 bytes [RDX]
```

Memory source for the analyzed parent state:

```text
base=RDX
index=none
scale=1
displacement=0
segment=none
base_tags=0
base_origin_root=1
base_origin_offset=0
address_exact=1
same_address_all_samples=1
class=Hero_Transform_conditional
alias_may_mask=0x1
loaded_may_top=0x7
loaded_tags_known=0
source_value_reads=0
```

Architectural destination metadata:

```text
destination=YMM0
width=256 bits
represented_bits=128
upper_state_represented=0
upper_effect=zero_above_encoded_VL
upper_zero=1
upper_preserved=0
mask=0
broadcast=0
rounding=0
```

Therefore c31 correctly refuses:

```text
semantic_load_modeled=0
reason=target_VMOVUPS_upper_vector_state_not_represented
```

Pre-barrier state remains:

```text
RAX/GPR0 tags=0x7
other logged GPR tags=0
logged XMM tags=0
flags_may_depend_tags=0
```

## Current bounded Codex task

Implement the smallest representation extension needed to model **only this exact authenticated 256-bit YMM load** soundly, then resume the existing bounded two-path walk to the next exact unsupported effect.

This task is not generic AVX/AVX2/AVX-512 emulation.

### 1. Extend vector dependency state only to the required 256-bit width

The current analyzer has 16 XMM dependency banks representing 128 bits each. Add a bounded representation sufficient to distinguish, for vector parents 0-15:

```text
low128 may-depend tags
upper128 may-depend tags
```

Preserve correct aliasing:

- XMMn refers to the low128 portion of YMMn;
- YMMn refers to low128 + upper128;
- an XMM operation must not silently overwrite/preserve the YMM upper half unless the decoded architectural rule says so;
- a YMM full-width write replaces both represented halves.

Do not add generic lane/value simulation. Dependency tags are enough.

### 2. Handle state above 256 bits explicitly

The c31 decoder reports for this exact VEX.256 form:

```text
upper_effect=zero_above_encoded_VL
upper_zero=1
upper_preserved=0
```

Represent this architecturally without pretending to track arbitrary ZMM data.

A minimal acceptable design is either:

- an explicit `above_256_known_zero`/equivalent state for the destination parent; or
- a rule that future >256-bit reads/writes are refused unless their required upper state is independently representable/provable.

Do not silently discard possible tracked dependence above the represented width. Any later EVEX/ZMM-width use must remain fail-closed unless soundly modeled.

### 3. Model only the exact pinned c31 VMOVUPS load

Eligibility remains pinned to all accepted runtime facts:

```text
site=0x2CF7FF0
encoding=VEX2
VL=256
form=YMM0 <- m256 [RDX]
memory_read=1
memory_write=0
memory_RMW=0
address_exact=1
base=RDX
index=none
displacement=0
address_component_tags=0
current alias class=Hero only
mask/broadcast/rounding absent
```

Wrong site, width, vector parent, encoding, direction, address form, mask/modifier or current-parent disagreement must refuse.

### 4. Use conservative unknown-top for both loaded halves

Do not read/log or numerically interpret the 32 source bytes.

For this exact unproven loaded value, assign:

```text
YMM0.low128 tags   <- 0x7
YMM0.upper128 tags <- 0x7
```

`0x7` remains a may-depend upper bound over selected/postwrite/other-prewrite, not observed taint.

Architectural effect for this exact instruction:

```text
YMM0.low128  <- top7
YMM0.high128 <- top7
state above bit255 <- architecturally zero / fail-closed marker as designed
all other vector parents unchanged
all GPR tags unchanged (including RAX=0x7)
flags unchanged
SP unchanged
memory unchanged
```

Do not infer semantic type, matrix/quaternion/vector meaning or pointer identity from `[RDX]`.

### 5. Preserve XMM/YMM write semantics in fixtures

At minimum verify:

- full YMM write replaces low128 and high128 dependency state;
- XMM alias reads see only low128;
- VEX.128 destination semantics are not accidentally treated as legacy SSE preservation if existing code later uses them;
- legacy XMM and VEX XMM upper-half behavior remain distinguishable or unsupported forms refuse;
- no tag leakage between vector parent indices;
- merge/partial writes remain refused unless explicitly modeled.

Do not widen production semantics beyond forms actually needed for sound state maintenance.

### 6. Resume the existing c27 all-path walk after exact admission

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

After this exact VMOVUPS is modeled, continue only to the next unsupported instruction.

Report:

- exact RVA/mnemonic;
- path/edge;
- refusal reason;
- pre-instruction GPR tags;
- vector low128/high128 tags for any relevant parents;
- flags tags.

Do not opportunistically implement the next new instruction in the same task unless it is already covered by an existing proven rule.

### 7. Immediate refusals remain

Still refuse on:

- vector memory store/RMW;
- unsupported masked/merge/broadcast/fault-suppression behavior;
- partial vector effects not represented soundly;
- any >256-bit vector read/write whose required state is not represented/proven;
- any other unproven memory operation;
- stack construction/change;
- nested CALL;
- indirect transfer;
- another branch beyond the authorized JA;
- loop/backedge;
- runtime-region crossing;
- snapshot/code mismatch;
- cap exhaustion or unresolved exit.

### 8. Preserve c30 state and all safety gates

Do not narrow c30:

```text
RAX tags=0x7
origin=unknown_loaded_qword_not_pointer
observed_taint=0
selected_dependence_killed=0
```

Still forbidden:

- Position/source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- raw vector/memory-value logging;
- numerical value interpretation;
- generic vector-memory semantics;
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

- exact pinned c31 YMM0 m256 load admission after 256-bit state exists;
- low128=top7 and high128=top7 without source-value reads;
- full YMM overwrite replaces both prior halves;
- XMM alias reads low128 correctly;
- other vector parents preserved;
- state-above-256 zero/fail-closed behavior;
- wrong site/destination/width/direction/address refuses;
- store/RMW/mask/merge/broadcast/refused forms;
- >256 use refuses unless represented/proven;
- c31 classification regression;
- c30 qword-top regression;
- c29 writable `.data` tag0 refusal;
- c28 MOV, c27 JA, c26 CMP, c25 runtime table, c24 no-unwind, c23-c19 regressions.

Run the relevant full build/regression chain. Update `analysis/`, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable Wisdoms facts when appropriate.

If the exact c31 metadata plus current authenticated snapshot is enough to continue statically, do not request runtime by habit. If the next barrier requires live parent-state evidence, emit only the minimum vector-state/next-barrier/CFG/tag/safety records and request one shortest A/E-style capture. Commit and push all authorized changes to the current development line.
