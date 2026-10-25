# Next task — classify exact VMOVUPS at 0x2CF7FF0 after accepted c30

## Status

The v0.1c30 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c30-20261025-133648.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

See `analysis/V01C30_RUNTIME_RESULT.md`.

## Accepted c30 facts

The prerequisite chain remains accepted:

- c25: full authenticated 527161-entry runtime table validation; exact target `0x2CF7FE0` genuinely has no containing runtime-function entry.
- c26: `0x2CF7FE0: cmp r8d,0x3FFF`, flags-only, relevant tags zero.
- c27: exact short `JA` at `0x2CF7FE7`, taken `0x2CF8013`, fallthrough `0x2CF7FE9`; both outcomes required.
- c28: `0x2CF7FE9: mov rax,qword ptr [rcx+0x100]`, full64 pure load from exact image source `0xC2D2580`.
- c29: that source is writable `.data` on PAGE_READWRITE, so tag0/non-writable-image proof is refused before value observation.

C30 admits only this exact already-authenticated load as a conservative abstract unknown-value effect:

```text
site_rva=0x2CF7FE9
source_rva=0xC2D2580
width=8
abstract_effect_admitted=1
RAX tags <- 0x7
precise_value_tags_known=0
observed_taint=0
selected_dependence_killed=0
source_value_reads=0
raw_value_logged=0
numeric_interpretation=0
pointer_chase=0
origin=unknown_loaded_qword_not_pointer
flags/other GPR/XMM/SP/memory preserved
```

`0x7` is the may-depend top over the three existing tracked categories; it is not an observation that all three are present.

The bounded walk therefore advances to a new exact barrier:

```text
barrier_rva=0x2CF7FF0
barrier_instruction=vmovups
reason=target_cfg_memory_operand_refused
path=0
edge_from_rva=0x2CF7FE7
edge_target_rva=0x2CF7FE9
```

Pre-barrier abstract state on this path:

```text
GPR0/RAX tags=0x7
all other logged GPR tags=0
all logged XMM tags=0
flags_may_depend_tags=0
upper_bound_not_observed=1
raw_values=0
```

Current target state:

```text
admitted=0
instructions=4
LEAF_CFG complete=0
return_paths=0
unique_instructions=4
visits=4
final_code_rechecked=0
effect_certificate=0
```

The log does **not** establish the VMOVUPS operand classes, vector width, memory direction, address expression, alias class, destination parent register, upper-lane behavior, or loaded-value provenance. Do not infer these from the mnemonic.

## Current bounded Codex task

Implement the smallest fail-closed extension needed to classify the exact `VMOVUPS` at `0x2CF7FF0` and, only if its exact effect can be modeled conservatively under the existing tag lattice without guessing vector semantics, continue to the next exact barrier.

### 1. Classify the exact VMOVUPS before adding semantics

Emit compact metadata sufficient to establish:

- exact instruction length / encoding family and decoded vector width;
- operand count and source/destination classes;
- exact XMM/YMM register aliases and parent vector register indices;
- exact read/write access for every operand;
- whether memory is read, written, or read-modify-write;
- for a memory operand: base/index/scale/signed displacement/segment/address width;
- exact effective-address provenance from owned symbolic parent state when available;
- possible alias classes against Hero Transform, `Transform+0x10C`, writer source, caller/home stack, callee-private stack, image and unknown ranges;
- incoming selected/postwrite/other-prewrite tags on register/address components;
- exact XMM/YMM destination effect including any VEX/EVEX upper-lane zeroing or preservation required by the decoded form;
- GPR/flags/SP/memory effects;
- current instruction-byte stability.

Do not log raw vector or memory contents. Do not assign semantic ABI/type names.

### 2. Memory store/RMW is an immediate refusal

If the exact VMOVUPS writes memory or performs any read-modify-write behavior, stop at `0x2CF7FF0` with the exact reason.

Do not authorize a store merely because its address appears disjoint from Position.

### 3. Pure load may be modeled only if the vector abstraction is sound

If the exact form is memory -> vector register, first determine whether the current analyzer's vector tag state can conservatively represent the full architectural destination effect.

Only consider admission if all of these are true:

- exact width and destination parent register are known;
- the write is a full architectural overwrite for the represented destination state, or any preserved/zeroed upper lanes can be modeled exactly;
- address expression is exact enough for the pinned rule and no second dereference/pointer chase is involved;
- no memory write/RMW;
- no hidden stack mutation;
- no unsupported mask/merge/broadcast/fault-suppression semantics;
- no lane/upper-half state exists outside the analyzer representation that could retain tracked dependence invisibly.

If the implementation's current XMM-only tag model cannot soundly represent a YMM/upper-lane effect, refuse rather than inventing a whole-register abstraction.

### 4. If admitted as an exact unknown vector load, use may-top rather than tag0

Do **not** attempt another immutable/tag0 proof by habit. For a writable/dynamic or otherwise value-unproven source, a safe exact-load abstraction may use the existing tracked-category top only if the vector destination representation is sound:

```text
destination vector tags <- 0x7   # may upper bound, not observed taint
origin <- unknown loaded vector value from exact source
```

Preserve all unaffected registers/flags/SP/memory exactly. Apply decoded upper-lane zero/preserve semantics exactly; do not generalize to other VMOVUPS sites, widths or encodings.

Do not read vector bytes to narrow tags, and do not interpret them numerically.

If exact address/alias metadata shows a tracked mutable source, `0x7` is still only an upper bound; it does not establish actual dependence. If the source/form cannot be safely bounded by the existing lattice, refuse.

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

After an admitted VMOVUPS, continue only until the next exact unsupported effect. Report its RVA, mnemonic, path/edge, exact refusal reason, and pre-instruction GPR/XMM/flags tag state.

Do not opportunistically add support for the next new instruction in the same task unless it is already covered by an existing admitted register-only rule.

The taken JA path at `0x2CF8013` remains mandatory; one safe fallthrough path never authenticates the target alone.

### 6. Preserve c30 unknown-load semantics exactly

Do not narrow or reinterpret c30's RAX state:

```text
RAX tags=0x7
origin=unknown_loaded_qword_not_pointer
observed_taint=0
selected_dependence_killed=0
```

The new VMOVUPS task must not use RAX's numerical value or treat `0x7` as observed data.

### 7. Existing prohibitions remain

Still refuse on:

- any other unproven memory operation;
- memory write/RMW;
- stack construction/change;
- nested CALL;
- indirect transfer;
- another branch beyond the single authorized JA;
- loop/backedge;
- unsupported partial-register/vector-lane effect;
- unsupported AVX/EVEX mask/merge semantics;
- runtime-region crossing;
- snapshot/current-code mismatch;
- cap exhaustion or unresolved exit.

Still forbidden:

- Position/source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- PAGE_GUARD/data-page mutation;
- raw memory/vector-value logging;
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

### 8. Owned tests / handoff

Add fixtures covering at minimum:

- exact VMOVUPS operand/access/width/encoding classification;
- memory-read vs memory-write/RMW distinction;
- exact base/index/scale/displacement and alias reporting;
- full XMM destination load semantics if that is the actual form and supported;
- YMM/upper-lane refusal when the analyzer representation cannot soundly model it;
- VEX upper-lane zeroing/preservation correctness for any admitted form;
- unknown loaded vector may-top `0x7` only for an exact pinned eligible form;
- no tag0 merely from address tags or image membership;
- no raw vector/memory value observation;
- flags/GPR/SP/memory preservation for an admitted load;
- wrong site/form/width/destination/address refuses;
- store/RMW/masked/merge/broadcast/unsupported encoding refuses;
- next exact barrier propagation;
- one unsafe JA leg refuses whole target;
- c30 unknown-qword top regression;
- c29 writable `.data` tag0 refusal regression;
- c28 MOV classification, c27 JA, c26 CMP, c25 table, c24 no-unwind and c23-c19 regressions.

Run the relevant full build/regression chain. Update `analysis/`, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable Wisdoms facts when appropriate.

If exact VMOVUPS metadata can be derived from the authenticated current-code snapshot, do not request runtime by habit. If parent-state address/tag information or the next barrier genuinely requires live evidence, emit only the minimum VMOVUPS/next-barrier/CFG/tag/safety records and request one shortest A/E-style capture. Commit and push all authorized changes to the current development line.
