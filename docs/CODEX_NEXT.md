# Next task — prove store-address components after accepted c36 hard barrier

## Status

The v0.1c36 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c36-20261026-150111.log
```

Input device is intentionally not part of this instruction-level conclusion unless explicitly needed for later motion/input analysis.

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

See `analysis/V01C36_RUNTIME_RESULT.md`.

## Accepted c36 facts

Prerequisite chain remains accepted:

- c25: exact target `0x2CF7FE0` has no containing runtime-function entry after full 527161-entry validation.
- c26: exact `cmp r8d,0x3FFF`.
- c27: exact short `JA`; both forward outcomes remain mandatory.
- c28-c30: exact qword load at `0x2CF7FE9` is modeled only as opaque `RAX tags=0x7`; its source is the writable-image qword at RVA `0xC2D2580`; no raw value observation.
- c31-c32: exact `YMM0 <- m256 [RDX]`, represented as low/high128 top7 and above256 known zero.
- c33: `movsxd r8,r8d`, full64 result tags zero.
- c34: `shl r8,6`, result tags zero; aggregate flags top7 due undefined AF/OF.
- c35: exact existing game-side store `m256 [R8+RAX] <- YMM0` at `0x2CF7FFB`.

C36 now establishes the unresolved store as a real abstract memory escape:

```text
memory_write_seen=1
memory_write_width=32
memory_address_tags=0x7
memory_value_low128_tags=0x7
memory_value_high128_tags=0x7
memory_alias_may_mask=0x7F
critical_alias_may_mask=0x3FF
memory_escape=1
first_escape_rva=0x2CF7FFB
pending_unknown_write=1
store_destination_contained=0
control_alias_unexcluded=1
CFG_blocked=1
continuation_allowed=0
later_memory_facts_usable=0
architectural_registers_preserved=1
reason=target_store_destination_code_or_control_stack_alias_unexcluded
```

The parent READ/CALL summaries now also carry `memory_escape=1`, `first_escape_rva=0x2CF7FFB` and `pending_unknown_write=1` before target-identity refusal.

The target remains incomplete:

```text
barrier_rva=0x2CF7FFB
barrier_instruction=vmovups
LEAF_CFG complete=0
return_paths=0
unique_instructions=7
visits=7
effect_certificate=0
```

No current log can reveal a safe next instruction because unresolved control-stack/code alias makes continuation unsound.

## Current bounded Codex task

Do **not** repeat the c36 store summary or request another identical A/E capture. The next question is whether genuinely new non-value evidence can decompose the destination address:

```text
[R8 + RAX]
```

into a bounded offset plus a proven base/publication contract.

This task has two independent proof tracks. Either may succeed while the other remains a blocker.

### 1. Prove the fallthrough-path R8 numeric range without observing a live value

Use the already-authenticated instruction/control sequence only:

```text
0x2CF7FE0: cmp r8d,0x3FFF
0x2CF7FE7: ja  taken=0x2CF8013, fallthrough=0x2CF7FE9
...
0x2CF7FF4: movsxd r8,r8d
0x2CF7FF7: shl r8,6
0x2CF7FFB: store [R8+RAX]
```

Prove the exact x86 unsigned predicate for the **fallthrough path** of `JA` from the accepted CMP flags. Do not infer a path from runtime frequency; derive the predicate architecturally.

If sound, carry an interval/range fact separately from dependency tags:

- exact pre-MOVSXD range of `R8D` on this path;
- effect of sign extension on that proven range;
- exact non-overflowing 64-bit effect of `SHL 6` on that range;
- resulting byte-offset interval and stride/alignment if provable;
- exact inclusive/exclusive extent needed for a 32-byte store.

The implementation must derive all bounds itself. Do not hard-code the expected maximum from this document as an assumption.

Keep dependency tags unchanged; this is a numeric **path predicate/range proof**, not tag narrowing.

If the current path abstraction cannot soundly retain a numeric predicate through these instructions, add only the minimum pinned path-range representation required for this sequence. Do not add a generic symbolic arithmetic engine.

### 2. Prove or refuse RAX base provenance from the exact qword publication slot

RAX is fully replaced by:

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
exact source RVA=0xC2D2580
section=.data
current page=PAGE_READWRITE
```

C29 already disproved immutable/tag0 treatment. C30 intentionally treats the loaded value as opaque may-top.

Now investigate the **slot's publication/writer provenance**, not its live numeric value.

Allowed bounded work:

- exact authenticated xref/reference discovery for RVA `0xC2D2580`;
- exact direct writers/readers to this slot from authenticated code;
- loader/relocation/import/TLS/load-config metadata only when it directly concerns this exact slot;
- a bounded writer/publication chain only as far as needed to identify how the slot receives its value and whether that source has a provable allocation/base/range/lifetime contract;
- static/current-code metadata that proves all writes to the exact slot if such completeness can itself be justified.

Do **not**:

- log/read the live qword merely to obtain a numeric pointer;
- pointer-chase from a live value;
- assume the slot contains a pointer, pool base, array base or allocation merely because `[R8+RAX]` later forms an address;
- infer ownership from a writable page or stable repeated value;
- traverse the separately deferred movement producer `0x351EB00`;
- add a generic cross-image dataflow engine just to force progress.

If exact-slot writers cannot be enumerated completely, or their source/lifetime cannot be established, report the precise missing proof and stop this track fail-closed.

### 3. Keep base and offset proofs separate

A proven R8 offset range is useful even if RAX remains opaque.

A proven RAX publication/base contract is useful even if R8 range remains unresolved.

Do not combine them into `store_destination_contained=1` unless both facts are independently established and the resulting full 32-byte destination interval is proven disjoint from all critical categories:

- current code / runtime-control data;
- control/return stack;
- caller-home and callee-private stack;
- Hero Transform / `Transform+0x10C`;
- writer source / Position-related tracked storage;
- any later-memory region required by the bounded proof.

If a base allocation/region is eventually proven, its valid extent must cover the entire derived offset range **plus the 32-byte write width**. Prove arithmetic without overflow/wrap.

### 4. Do not resume CFG unless containment becomes exact

If either component remains unresolved, preserve c36:

```text
memory_escape=1
pending_unknown_write=1
store_destination_contained=0
control_alias_unexcluded=1
CFG_blocked=1
continuation_allowed=0
```

Do not manufacture the next opcode.

If, and only if, full destination containment becomes proven, record the exact proof basis and then allow the existing bounded walk to continue to the next unsupported instruction. Do not opportunistically implement that next instruction in the same task unless an existing accepted rule already covers it.

### 5. Both JA paths remain mandatory

The path-range proof applies specifically to the `JA` fallthrough leg. Keep the taken leg intact and merge all effect state conservatively.

A clean taken path cannot erase the store/escape from the fallthrough path.

No effect certificate may be issued while unresolved memory escape/control alias remains.

### 6. Existing safety gates remain closed

Still forbidden:

- MOD Position/source-position/input/movement writes;
- gameplay/adhesion/camera/animation/physics changes;
- raw live pointer/value logging for ownership narrowing;
- producer `0x351EB00` traversal;
- generic arbitrary memory/store emulation;
- guessed ABI/type names;
- weakening caller-owned/lifetime/exclusivity requirements.

Safety state remains:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

### 7. Required diagnostics / tests

Add compact diagnostics that distinguish:

- branch-derived numeric range vs dependency tags;
- R8D fallthrough predicate and propagated MOVSXD/SHL offset range;
- arithmetic overflow/wrap proof status;
- exact qword-slot reference/writer enumeration status;
- completeness of exact-slot writer discovery;
- publication source kind without guessed semantic names;
- allocation/region/lifetime proof status if found;
- base-range proof vs mere numeric-address observation;
- final destination containment status and exact reason when false.

Owned fixtures must cover at minimum:

- JA taken/fallthrough unsigned predicate correctness;
- boundary values around `0x3FFF/0x4000` in owned synthetic states;
- MOVSXD and SHL interval propagation with no overflow;
- 32-byte end-extent calculation;
- clean separation of range metadata from may-tags;
- exact-slot direct writer/reference discovery fixtures;
- multiple writer ambiguity refuses;
- indirect/unproven writer refuses;
- incomplete xref coverage refuses;
- allocation/range/lifetime missing proof refuses;
- base proven but offset unknown refuses;
- offset proven but base unknown refuses;
- c36 store-escape regression and c19-c35 prior regressions.

Run the relevant full build/regression chain. Update `analysis/`, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md` and reusable Wisdoms facts when appropriate.

If this task can be resolved entirely from authenticated current executable/static metadata, do not request runtime by habit. If a genuinely new runtime observation class becomes necessary, first define exactly what non-value fact it would establish and why existing static/current evidence cannot establish it. Do not request another identical movement capture.