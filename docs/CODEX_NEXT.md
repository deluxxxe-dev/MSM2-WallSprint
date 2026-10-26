# Next task — c38 bounded register-affine exact-slot candidate discovery

## Status

The owner has completed the c37 offline scan requested after the accepted c37 runtime result.

Owner-supplied compact artifact:

```text
MSM2-WS-v0.1c37-slot-publication.json
```

Derived accepted evidence is recorded in:

```text
analysis/V01C37_SLOT_PUBLICATION_SCAN_RESULT.md
```

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before implementation read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, this file, c28/c37 analysis, current `source/tools/slot_publication.py`, and relevant Wisdoms references.

## Accepted c37 scan result

The current tool authenticated the exact supported image and exact writable slot:

```text
slot_rva=0xC2D2580
image_authenticated=1
slot readable=1
slot writable=1
scan_scope=bounded_offline_direct_candidates_not_all_writes
```

Its intentionally narrow direct RIP-relative/absolute candidate class returned:

```text
reference_candidates=[]
direct_writer_candidates=0
candidate_sweep_complete=0
writer_enumeration_complete=0
publication_source_kind=unproven
allocation_proven=0
range_proven=0
lifetime_proven=0
base_proven=0
destination_contained=0
CFG_continuation=0
live_qword_reads=0
pointer_chase=0
reason=xref_coverage_incomplete_indirect_writers_unexcluded
```

Coverage diagnostics:

```text
cap_bytes=67108864
cap_instructions=5000000
bytes_scanned=17859460
decode_gaps=737
indirect_or_unresolved_operands=1279672
```

The deferred runtime region containing producer `0x351EB00` remained excluded.

**Do not interpret zero direct candidates as zero readers/writers.** Candidate coverage and writer enumeration are explicitly incomplete.

## Concrete known limitation / regression anchor

Accepted c28 runtime evidence already proves one exact consumer of the same slot:

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
base=RCX
base_origin_root=16
base_origin_offset=0xC2D2480
displacement=0x100
effective source RVA=0xC2D2580
address_expression_exact=1
address_class=image_address_mutability_unproven
```

The c37 Python scanner deliberately classifies register-base/indexed memory operands as unresolved unless the address is directly RIP-relative/absolute. Therefore the owner result demonstrates exactly why a direct-only scan is insufficient.

This known consumer is an **address-representation anchor**, not a writer/publication proof and not permission to hard-code a candidate result.

## Current bounded Codex task

Extend only the offline/static exact-slot candidate discovery needed to resolve **register-affine exact image addresses** without observing or dereferencing memory values.

The objective is to narrow the 1,279,672 unresolved memory operands into:

1. exact register-affine image-address candidates that can be proven from instruction/address provenance; and
2. still-unresolved operands with an explicit reason.

This task is still candidate discovery. It must not claim all-writer completeness, allocation, lifetime, ownership, store containment, or CFG continuation merely because new candidates are found.

### 1. Add a minimal exact register-address provenance state

Use only narrow full-width address-producing operations that can be proved from static instruction semantics and the authenticated image. Prefer the minimum set actually required by owned fixtures/current code, for example:

- RIP-relative `LEA` producing an exact image address;
- full-width register-to-register propagation of an already exact address;
- checked affine `LEA reg,[known_reg + signed_disp]` with no unresolved index;
- checked full-width add/sub immediate only if required and if no wrap is proven;
- exact absolute address materialization only when its image/relocation semantics are itself authenticated.

Do **not** infer an address value from:

- a memory load (`mov reg,[mem]`) or any loaded qword;
- a call return;
- an indirect transfer;
- a partial-register write;
- unsupported arithmetic;
- guessed ABI/type/pointer semantics.

A memory-loaded register must remain unknown unless a separately accepted non-value proof already supplies an exact address origin. No pointer chasing.

### 2. Bound state lifetime aggressively

This is not a generic symbolic executor.

At minimum:

- invalidate exact register-address state on unsupported/partial writes to that register;
- do not carry unproven state through indirect control transfer;
- do not assume caller/callee preservation from ABI guesses;
- reset or split conservatively at control-flow boundaries unless an existing authenticated direct-edge proof explicitly supplies the state;
- preserve the existing deferred-region exclusion and all scan caps.

If reproducing the c28 `0x2CF7FE9` consumer requires the existing authenticated parent-state proof rather than purely local offline propagation, report that distinction explicitly. It is acceptable to expose an `authenticated_external_anchor` record; it is not acceptable to fake local scanner coverage or hard-code writer completeness.

### 3. Resolve only exact slot overlap

For a memory operand with a proven exact register-affine image address, calculate its full byte interval and report a candidate only when it overlaps the exact 8-byte slot at `0xC2D2580`.

Record compact metadata only, such as:

- site RVA;
- width;
- decoded/encoding-conservative access;
- exact effective RVA;
- address proof class (`direct_rip`, `direct_absolute`, `register_affine_static`, or authenticated external anchor where applicable);
- source kind only when directly decoded without semantic guessing;
- whether it may write.

Do not export instruction bytes, whole function bodies, private file paths, live values, or raw pointer values.

### 4. Improve unresolved diagnostics without claiming completeness

Break the current aggregate unresolved count into useful conservative categories when practical, for example:

- register base with no exact static origin;
- indexed address;
- memory-loaded base provenance;
- unsupported/partial register mutation;
- control-flow boundary loss;
- segment/other unsupported addressing;
- decode gap/cap/excluded region.

These counts are diagnostics, not proof of no alias.

`candidate_sweep_complete` and especially `writer_enumeration_complete` must remain false unless the implementation independently proves the much stronger coverage requirement. A complete pass over selected bytes is still not complete writer enumeration.

### 5. Publication chain remains a separate later question

If the new scan finds candidate writers/readers, do not automatically follow arbitrary calls or create generic cross-image dataflow.

For each candidate, only record enough source/address metadata to decide whether a **future** bounded publication-chain task has a useful exact starting point.

Do not infer:

- allocation base;
- valid extent;
- lifetime;
- uniqueness/ownership;
- pool/array/table semantics;
- critical non-overlap.

Those remain false unless separately proven.

### 6. Existing c37 offset and c36 hard barrier remain unchanged

Accepted relative offset proof remains:

```text
R8D fallthrough=0..0x3FFF
MOVSXD=0..0x3FFF
SHL6 offset=0..0xFFFC0 inclusive
stride=64
32-byte endExtentExclusive=0xFFFE0
overflow_wrap_excluded=1
offset_proven=1
```

Until an allocation/base/extent/lifetime contract is independently established, preserve:

```text
memory_escape=1
pending_unknown_write=1
store_destination_contained=0
control_alias_unexcluded=1
CFG_blocked=1
continuation_allowed=0
```

Do not manufacture the next opcode.

### 7. Safety gates remain closed

Still forbidden:

- MOD Position/source-position/input/movement writes;
- gameplay/adhesion/camera/animation/physics changes;
- live qword/pointer logging or pointer chasing;
- producer `0x351EB00` traversal;
- generic arbitrary memory/dataflow emulation;
- guessed ABI/type/pool/array semantics;
- weakening ownership/lifetime/exclusivity/conflict gates.

Safety state remains:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

## Required owned validation

Add focused fixtures at minimum for:

- RIP-relative LEA -> register -> exact slot read candidate;
- RIP-relative LEA -> register -> exact slot write candidate;
- full-width register copy preserving exact address;
- checked affine LEA displacement producing exact slot overlap;
- non-overlap adjacent address refusal;
- memory load into base register remaining unknown/no pointer chase;
- partial-register write invalidating exact address state;
- unsupported arithmetic invalidation;
- branch/call/indirect boundary conservative invalidation or explicit authenticated-edge handling;
- indexed/segment unresolved classification;
- multiple candidate ambiguity remaining non-ownership;
- decode-gap/cap/excluded-region coverage remaining incomplete;
- deferred `0x351EB00` region still excluded;
- exact SHA and pinned Capstone guards retained;
- c37 direct candidate fixtures and prior c19-c37/native/workspace regressions retained.

If an owned synthetic parent-state fixture represents the c28 exact consumer, ensure it resolves `base+0x100` to the exact slot without loading a pointer value. Do not package proprietary game instruction bodies as fixtures.

## Required outputs / handoff

Update:

- implementation/tests for the bounded offline scanner;
- `analysis/` with the c38 proof/limitations;
- `docs/PROJECT_STATE.md`;
- `docs/CODEX_RESULT.md`;
- reusable Wisdoms facts if appropriate.

Do not overwrite this task with a later milestone until c38 is committed/pushed and its exact next evidence is clear.

After successful push, the expected owner action should be **Update-WS and rerun the offline scanner only**. Do not request another WallRun/A-E game capture unless c38 discovers a genuinely new runtime fact that static analysis cannot establish.

Use a new versioned output name such as:

```text
logs/MSM2-WS-v0.1c38-slot-publication.json
```

The owner should return that compact JSON to ChatGPT. The private executable stays local.