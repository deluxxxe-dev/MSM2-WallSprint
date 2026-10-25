# Next task — classify exact MOVSXD at 0x2CF7FF4 after accepted c32

## Status

The v0.1c32 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c32-20261025-145028.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

See `analysis/V01C32_RUNTIME_RESULT.md`.

## Accepted c32 facts

The prerequisite chain remains accepted:

- c25: full authenticated 527161-entry runtime table validation; exact target `0x2CF7FE0` has no containing runtime-function entry.
- c26: exact `cmp r8d,0x3FFF`, flags-only, relevant tags zero.
- c27: exact short `JA` at `0x2CF7FE7`; both forward outcomes remain mandatory.
- c28-c30: exact `mov rax,[rcx+0x100]` is modeled only as an opaque unknown loaded qword with `RAX tags=0x7`, no qword read/log/numeric interpretation.
- c31: exact `0x2CF7FF0` is `VEX2.256 VMOVUPS YMM0 <- m256 [RDX]`, pure load, exact Hero-root address, no store/RMW.

C32 adds only the vector dependency representation required for this pinned load:

```text
represented_bits=256
represented_banks=16
YMM0.low128 tags  <- 0x7
YMM0.high128 tags <- 0x7
above256_known_zero=1
pinned_YMM0_rule_added=1
generic_YMM_ZMM_rules=0
source_value_reads=0
raw_values=0
observed_taint=0
```

The VMOVUPS is now semantically modeled:

```text
semantic_load_modeled=1
reason=target_VMOVUPS_exact_YMM0_unknown_halves_modeled
```

C30 state remains:

```text
RAX tags=0x7
origin=unknown_loaded_qword_not_pointer
```

The bounded walk advances to:

```text
barrier_rva=0x2CF7FF4
barrier_instruction=movsxd
reason=target_cfg_instruction_effect_refused
path=0
edge_from_rva=0x2CF7FE7
edge_target_rva=0x2CF7FE9
```

Pre-barrier abstract state:

```text
GPR0/RAX tags=0x7
other logged GPR tags=0
XMM0/low128 tags=0x7
YMM0/high128 tags=0x7
above256_known_zero=1
flags_may_depend_tags=0
```

Current target state:

```text
admitted=0
instructions=5
LEAF_CFG complete=0
return_paths=0
unique_instructions=5
visits=5
final_code_rechecked=0
effect_certificate=0
```

The current log establishes only the mnemonic/refusal for `MOVSXD`. Do not infer its operands or memory use from the mnemonic.

## Current bounded Codex task

Implement the smallest fail-closed extension needed to classify the exact `MOVSXD` at `0x2CF7FF4` and, only if its exact effect is representable under the existing dependency model, continue to the next unsupported boundary.

### 1. Classify the exact MOVSXD before adding semantics

Emit compact metadata sufficient to establish:

- exact instruction length / encoding;
- operand count;
- exact source and destination operand classes;
- exact source width;
- exact destination GPR and architectural write width;
- decoded read/write access for all operands;
- whether a memory operand exists;
- if memory exists: base/index/scale/signed displacement/segment/address width;
- exact effective-address provenance from the owned symbolic parent state when available;
- alias classes against Hero Transform, `Transform+0x10C`, writer source, caller/home stack, callee-private stack, image and unknown ranges;
- incoming selected/postwrite/other-prewrite tags on source/address components;
- exact sign-extension effect into the destination GPR;
- whether the destination write is a full 64-bit architectural replacement or any partial-state rule is required;
- flags/GPR/vector/SP/memory effects;
- current instruction-byte stability.

Do not log raw source memory values and do not assign semantic ABI/type names.

### 2. Memory behavior must remain explicit

If the source is memory, classify it as a read only before considering admission.

Any store/RMW, unresolved address, second dereference/pointer chase, stack mutation, or unsupported alias/address state remains immediate refusal.

Do not assign tag0 merely from address provenance. If loaded-value provenance is unknown, use only a sound may-depend upper bound where the exact load can be represented safely.

### 3. Model only the exact pinned form if sound

If the exact MOVSXD is representable, add only the narrow semantics required for this site.

For a register source:

- destination tags must be derived conservatively from the exact source parent tags;
- preserve correct width/sign-extension semantics;
- do not retain unrelated prior destination tags after a full architectural replacement.

For a memory source:

- destination loaded tags may use the tracked top `0x7` only if the exact read/address form is pinned and no more precise provenance is safely established;
- no raw value read/log/numeric interpretation is required or permitted for narrowing;
- origin should remain an opaque exact-source load unless independently proven otherwise.

Do not generalize MOVSXD handling to arbitrary sites/forms in this task.

### 4. Preserve current c30/c32 state

Before the instruction, retain:

```text
RAX tags=0x7
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
other logged GPR/vector tags=0
flags tags=0
```

The MOVSXD implementation must not accidentally narrow or clear unrelated c30/c32 state.

### 5. Resume the existing c27 all-path walk only after exact admission

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

After an admitted MOVSXD, continue only to the next unsupported instruction.

Report:

- exact RVA/mnemonic;
- path/edge;
- refusal reason;
- pre-instruction GPR tags;
- vector low128/high128 tags for relevant parents;
- flags tags.

Do not opportunistically add support for the next new instruction in the same task unless it is already covered by an existing proven rule.

### 6. Existing refusals remain

Still refuse on:

- memory store/RMW;
- unresolved or dynamic memory address not covered by the pinned rule;
- stack construction/change;
- nested CALL;
- indirect transfer;
- another branch beyond the authorized JA;
- loop/backedge;
- unsupported partial GPR/vector effect;
- unsupported >256-bit vector use;
- runtime-region crossing;
- snapshot/code mismatch;
- cap exhaustion or unresolved exit.

### 7. Preserve all safety gates

Still forbidden:

- Position/source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- raw memory/vector-value logging;
- numerical source interpretation;
- generic memory/vector semantics;
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

### 8. Owned tests / handoff

Add fixtures covering at minimum:

- exact MOVSXD operand/access/width classification;
- register-source sign-extension propagation if that is the actual form;
- memory-source read/address/alias classification if that is the actual form;
- exact full64 destination replacement semantics;
- no stale destination-tag retention after full replacement;
- unknown exact memory load may-top only if needed and safely pinned;
- wrong site/source width/destination/form/address refuses;
- store/RMW/stack/partial/unsupported forms refuse;
- c32 YMM0 low/high state remains unchanged when MOVSXD does not affect it;
- c32 vector256 regression;
- c31 VMOVUPS classification regression;
- c30 qword-top regression;
- c29 writable `.data` tag0 refusal;
- c28 MOV, c27 JA, c26 CMP, c25 runtime table, c24 no-unwind and c23-c19 regressions.

Run the relevant full build/regression chain. Update `analysis/`, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable Wisdoms facts when appropriate.

If the exact MOVSXD metadata can be derived from the authenticated current-code snapshot, do not request runtime by habit. If parent-state address/tag information or the next barrier genuinely requires live evidence, emit only the minimum MOVSXD/next-barrier/CFG/tag/safety records and request one shortest A/E-style capture. Commit and push all authorized changes to the current development line.
