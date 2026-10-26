# Next task — classify exact VMOVUPS at 0x2CF7FFB after accepted c34

## Status

The v0.1c34 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c34-20261026-090013.log
```

Test environment note: this capture was performed remotely with keyboard/mouse controls. Keep gameplay-motion observations labeled as that input condition. Do not infer controller equivalence from this capture.

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

See `analysis/V01C34_RUNTIME_RESULT.md`.

## Accepted c34 facts

Prerequisite chain remains accepted:

- c25: exact target `0x2CF7FE0` has no containing runtime-function entry after full 527161-entry table validation.
- c26: `cmp r8d,0x3FFF`, relevant tags zero.
- c27: exact short JA, both exact forward outcomes mandatory.
- c28-c30: exact qword load modeled only as opaque `RAX tags=0x7`, no value observation.
- c31-c32: exact first VMOVUPS at `0x2CF7FF0` is `VEX2.256 YMM0 <- m256 [RDX]`, represented as low128/top7 + high128/top7 + above256 known zero.
- c33: `0x2CF7FF4: movsxd r8,r8d`, full64 sign extension, result tags zero.

C34 exactly classified and modeled:

```text
site_rva=0x2CF7FF7
instruction=shl
bytes=4
encoding=group2_C0_C1_slash4_imm8
destination=R8
destination_bits=64
count_source=immediate
encoded_count=6
count_mask=0x3F
effective_count=6
memory_read=0
memory_write=0
memory_RMW=0
incoming R8 tags=0x0
count tags=0x0
result R8 tags=0x0
semantic_modeled=1
```

Flag abstraction after the count-6 shift:

```text
CF defined
SF/ZF/PF defined
AF undefined
OF undefined for count>1
defined_flag_may_tags=0x0
pre_flags_tags=0x0
post_flags_tags=0x7
aggregate_undefined_top7=1
```

`post_flags_tags=0x7` is conservative uncertainty from undefined flags, not observed taint.

The bounded walk advances to:

```text
barrier_rva=0x2CF7FFB
barrier_instruction=vmovups
reason=target_cfg_memory_operand_refused
path=0
edge_from_rva=0x2CF7FE7
edge_target_rva=0x2CF7FE9
unique_instructions=7
visits=7
return_paths=0
```

Pre-barrier abstract state:

```text
RAX/GPR0 tags=0x7
R8 tags=0x0
other logged GPR tags=0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags_may_depend_tags=0x7
```

Do **not** infer that the new VMOVUPS matches `0x2CF7FF0` merely because the mnemonic is the same.

## Current bounded Codex task

Classify the exact architectural form/effect of `0x2CF7FFB: vmovups`. Only if its complete effect is already representable by the existing dependency state, add the smallest pinned rule and resume the current c27 bounded all-path walk to the next unsupported boundary.

### 1. Classify exact VMOVUPS before adding semantics

Emit compact metadata sufficient to establish:

- exact instruction length and VEX/EVEX/legacy encoding;
- exact vector length / encoded width;
- exact operand count;
- source and destination operand classes/registers;
- load vs store direction;
- exact memory width;
- decoded operand read/write access;
- memory read/write/RMW status;
- base/index/scale/signed displacement/segment/address width;
- current symbolic address provenance and exactness when available;
- aliases against Hero Transform, `Transform+0x10C`, writer source, caller/home stack, callee-private stack, image and unknown ranges;
- incoming selected/postwrite/other-prewrite tags for address components and any vector source;
- exact destination vector parent and architectural write width;
- XMM/YMM alias behavior and upper-lane zero/preserve semantics;
- mask/merge/zero-mask/broadcast/rounding/fault-suppression state;
- flags/GPR/SP/other-memory effects;
- current-code/snapshot stability.

Do not read/log vector or memory values and do not assign semantic type names.

### 2. Reuse c32 representation only when exact form permits

Existing represented vector state is limited to:

```text
16 vector parents
low128 may-depend tags
high128 may-depend tags
above256 known-zero/fail-closed knowledge
```

If `0x2CF7FFB` is a full-width XMM/YMM register destination load whose complete architectural effect is soundly expressible in that model, a pinned site-specific rule may reuse it.

Do not add generic AVX/AVX2/AVX-512 emulation.

### 3. Immediate refusal cases

Refuse without continuation on any of the following unless already proven by an existing exact rule:

- vector memory store or RMW;
- unresolved/dynamic address or unsupported pointer chase;
- unsupported partial/merge/masked vector write;
- broadcast/rounding/fault-suppression semantics not represented;
- any >256-bit state requirement not independently representable/proven;
- unsupported VEX/EVEX upper-lane behavior;
- stack construction/change;
- nested CALL or indirect transfer;
- another branch beyond the authorized c27 JA;
- loop/backedge;
- runtime-region crossing;
- code/snapshot mismatch;
- cap exhaustion or unresolved exit.

### 4. Preserve current state

Before the new VMOVUPS:

```text
RAX tags=0x7
R8 tags=0x0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags tags=0x7
```

Do not accidentally narrow or clear unrelated state. The SHL-generated flag top must remain until a later instruction has a proven architectural flag effect.

### 5. Bounded walk contracts remain unchanged

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

After an admitted exact VMOVUPS, continue only to the next unsupported instruction. Do not opportunistically implement the next new opcode unless already covered by an existing proven rule.

Report the new barrier RVA/mnemonic/path/reason plus relevant GPR/vector low-high/above256/flags state.

### 6. Safety gates remain closed

Still forbidden:

- Position/source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- raw memory/vector/register-value logging for numerical narrowing;
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

### 7. Owned tests / handoff

Add fixtures covering at minimum:

- exact site/encoding/vector width/operand direction classification;
- memory width/access/address/alias diagnostics;
- compatible full XMM/YMM load semantics if that is the actual form;
- exact XMM/YMM alias and upper-lane effect;
- preserved unrelated RAX/R8/flags/vector-parent state;
- wrong site/direction/width/destination/address refuses;
- store/RMW/masked/partial/wider unsupported forms refuse;
- c34 SHL data/flag regression;
- c33 MOVSXD regression;
- c32 vector256 regression;
- c31 first VMOVUPS regression;
- c30 qword-top regression;
- c29-c19 prior proof regressions.

Run the relevant full build/regression chain. Update `analysis/`, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable Wisdoms facts when appropriate.

If exact form plus current parent state can be derived from authenticated current-code evidence, do not request runtime by habit. If live parent/address/next-barrier evidence is genuinely required, emit only the minimum VMOVUPS/operand/address/vector-state/next-barrier/CFG/tag/safety records and request one shortest capture. Commit and push all authorized changes to the current development line.