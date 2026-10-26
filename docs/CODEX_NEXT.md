# Next task — contain exact VMOVUPS store at 0x2CF7FFB after accepted c35

## Status

The v0.1c35 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c35-20261026-142344.log
```

Input method was not supplied. Do not infer keyboard/mouse/controller equivalence from motion observations; the instruction-level result below is independent of input device.

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

See `analysis/V01C35_RUNTIME_RESULT.md`.

## Accepted c35 facts

Prerequisite chain remains accepted:

- c25: exact target `0x2CF7FE0` has no containing runtime-function entry after full 527161-entry validation.
- c26: `cmp r8d,0x3FFF`, relevant tags zero.
- c27: exact short JA; both exact forward outcomes remain mandatory.
- c28-c30: exact qword load is modeled only as opaque `RAX tags=0x7`, no raw value observation.
- c31-c32: exact first VMOVUPS `YMM0 <- m256 [RDX]`, with low/high128 top7 and above256 known zero.
- c33: `movsxd r8,r8d`, full64 result tags zero.
- c34: `shl r8,6`, result R8 tags zero; undefined AF/OF produce conservative aggregate flags top7.

C35 exactly classified the second VMOVUPS at `0x2CF7FFB` as a store:

```text
site_rva=0x2CF7FFB
instruction=vmovups
bytes=6
encoding=VEX3
opcode=0x11
vector_bits=256
memory_width=32
memory_read=0
memory_write=1
memory_RMW=0
direction_verified=1
flags_preserved=1
GPR_writes=0
vector_writes=0
SP_write=0
```

Current decoded operands/state:

```text
m256 [R8 + RAX] <- YMM0
R8 tags=0x0
RAX tags=0x7
scale=1
displacement=0
YMM0.low128 tags=0x7
YMM0.high128 tags=0x7
above256_known_zero=1
flags tags=0x7
```

Current address proof:

```text
address_exact=0
class=indexed_address_unproven
address_component_tags=0x7
alias_may_mask=0x7F
```

The alias upper bound includes Hero Transform, `Transform+0x10C`, writer source, caller/home stack, callee-private stack, image and unknown ranges. This is conservative uncertainty, not evidence that every alias occurs.

C35 therefore correctly stops:

```text
reason=target_second_VMOVUPS_store_or_RMW_refused
semantic_modeled=0
barrier_rva=0x2CF7FFB
unique_instructions=7
visits=7
return_paths=0
```

The existing parent summary still says `memory_escape=0` only because this store has not been admitted into the effect model. Do not interpret that as proof that no escape exists.

## Current bounded Codex task

Implement the smallest fail-closed extension needed to answer whether the exact existing game-side store at `0x2CF7FFB` can be safely represented/contained in the dependency proof.

This is **not** permission for MOD memory writes and is **not** a request for generic AVX store emulation.

### 1. Preserve the exact store identity

Re-verify from authenticated current code:

```text
VEX3.256 VMOVUPS
m256 [R8 + RAX] <- YMM0
32-byte memory write only
no memory read/RMW
no GPR/vector/SP/flag write
```

Retain the existing decoder-direction conflict diagnostics and encoding-resolved direction. Any different form remains refusal.

### 2. Determine whether destination containment is provable without guessing values

Using only existing symbolic/current-parent evidence and bounded metadata, determine whether the 32-byte destination can be proven disjoint from every proof-critical category that would make blind continuation unsound, including at minimum:

- current target/code bytes and runtime-control data;
- caller/home stack and return/control stack storage;
- callee-private stack;
- Hero Transform / `Transform+0x10C`;
- writer source / Position-related tracked storage;
- any other memory region whose later load/store in the bounded target could alias the write.

Do not infer disjointness merely from zero tags. Do not convert the opaque c30 RAX loaded qword into a pointer/type by assumption.

Do not read/log raw memory/vector/register values merely to force a destination address. No pointer chase or ABI/type naming.

If existing non-value provenance cannot exclude critical aliases, say so explicitly.

### 3. Add only a sound pinned memory-effect summary

If the exact destination remains unresolved, the safe result is an explicit unresolved store/escape summary such as:

```text
memory_write_seen=1
memory_write_width=32
memory_address_tags=0x7
memory_value_low128_tags=0x7
memory_value_high128_tags=0x7
memory_alias_may_mask=<current conservative mask>
memory_escape=1
first_escape_rva=0x2CF7FFB
store_destination_contained=0
```

Names may match existing project conventions, but semantics must be equivalent.

`0x7` remains a may-depend upper bound, not observed taint.

A store summary must not pretend that destination bytes are available for later exact reads unless a real tracked memory-state model proves them.

### 4. Continue only when control/memory soundness permits

If a narrow store-effect summary is sound enough to continue register/control analysis, preserve unchanged architectural register state:

```text
RAX=0x7
R8=0x0
YMM0.low128=0x7
YMM0.high128=0x7
above256_known_zero=1
flags=0x7
```

But after the store, set an explicit pending/unknown-memory-write state.

Any later memory read/write whose address may alias this 32-byte store must refuse unless disjointness is proven exactly. Do not use stale pre-store memory assumptions across a possible alias.

If the unresolved destination may include current code/control stack such that even static fallthrough/RET cannot be trusted, do **not** continue the CFG. Report that as the current hard blocker.

If continuation is sound, stop at the next unsupported instruction and report its exact RVA/mnemonic/path/reason plus the pending store effect.

### 5. Both JA paths and merges remain mandatory

The store is on the c27 fallthrough path. Preserve both exact JA outcomes.

When merging paths/returns, memory escape/write uncertainty must union conservatively. A clean taken path must not erase a store/escape seen on the fallthrough path.

Do not issue an effect certificate while unresolved memory escape/control alias remains.

### 6. Existing refusals and safety remain

Still refuse on:

- any other unproven memory store/RMW;
- unresolved code/control-stack alias that invalidates CFG continuation;
- stack mutation not fully represented;
- nested CALL/indirect transfer;
- another branch beyond authorized JA;
- loop/backedge;
- unsupported vector/partial/masked/wider semantics;
- runtime-region crossing;
- code/snapshot mismatch;
- cap exhaustion.

Still forbidden:

- MOD Position/source-position/input/movement writes;
- gameplay/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- raw value logging/numerical narrowing intended to guess ownership;
- generic memory/store semantics;
- weakening caller-owned/lifetime/exclusivity gates.

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

- exact VEX3.256 store identity/direction/width;
- `[R8+RAX]` component tags/origins/aliases;
- 32-byte store-value low/high dependency summary;
- unresolved destination produces conservative memory-escape state;
- clean register/vector/flags state remains architecturally unchanged by store;
- path merge does not erase escape from one branch;
- subsequent potentially aliasing memory read/write refuses;
- provably disjoint later memory access may proceed only if existing model truly proves disjointness;
- possible code/control-stack alias prevents unsafe CFG continuation;
- wrong site/direction/width/form refuses;
- c34 SHL, c33 MOVSXD, c32 vector256, c31 first VMOVUPS, c30 qword-top and earlier regressions.

Run the relevant full build/regression chain. Update `analysis/`, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable Wisdoms facts when appropriate.

If the current accepted c35 evidence already proves that containment is impossible without a new observation class, do not manufacture progress. Encode the fail-closed result and identify the minimal next evidence needed. Commit and push all authorized changes to the current development line.
