# Next task — bounded read-flow completion past 0x2B235BE after accepted c21

## Status

The v0.1c21 actual-game A/E-style capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c21-20261025-013952.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history unless this task deliberately creates and fully hands off a successor branch.

## Accepted c21 facts

C21 narrows the old `0x2B23578` blocker.

### Exact read address

```text
site_rva=0x2B23578
instruction=mov
operand_class=memory_read
read_width=4
address_class=transform
address_root=writer-entry Hero Transform
address_offset=0x10C
base=RCX
index=none
carrier=EAX
same exact address for analyzed states=1
```

Therefore the earlier read is an exact read of the same `Hero Transform+0x10C` field later written by:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
```

### Local ordering / possible value flow

C21 establishes for the bounded local CFG:

```text
first_possible_use_rva=0x2B23581
control_may_depend=1
first_control_rva=0x2B23583
memory_escape=0
same_field_order_mask includes read-before-write
```

This is a possible local control dependence, not a completed XYZ/W causal proof.

### Current exact blocker

The read-flow walk remains incomplete at:

```text
first_unresolved_rva=0x2B235BE
first_unresolved_instruction=test
reason=read_flow_partial_or_unmodeled_register
```

Current unresolved outputs remain:

```text
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
effect_certificate=0
writable_source_proven=0
```

No Position/source/input/movement writes were introduced. The authenticated target build and normal lifecycle/orientation safety baseline remained intact.

## Required reading

Before editing code, read:

1. `AGENTS.md`
2. `docs/DEV_RULES.md`
3. `docs/PROJECT_STATE.md`
4. `analysis/V01C21_READ_DEPENDENCY.md`
5. `analysis/V01C20_RUNTIME_RESULT.md`
6. `analysis/V01C20_EDI_VALUE_PROVENANCE.md`
7. `analysis/RDX_XYZ_SOURCE_CONTRACT.md`
8. relevant `source/src/NativeHooks/rdx_source_contract.cpp/.hpp` code/tests
9. `deluxxxe-dev/MSM2-Wisdoms/docs/WALLSPRINT_INTERNALS.md`
10. `deluxxxe-dev/MSM2-Wisdoms/docs/ENGINE_INTERNALS.md`

Project-local measured evidence remains authoritative over a stale shared summary.

## Current bounded Codex task

Implement the smallest evidence-preserving extension necessary to continue the c21 selected-read value flow past the exact `0x2B235BE` blocker.

### 1. Classify the exact `test` blocker

For `0x2B235BE`, report compact metadata sufficient to answer:

- exact operands/register widths;
- whether either operand may carry the selected `0x2B23578` read value, the later `0x2B2358E` written value, or another same-field value;
- whether the instruction only updates flags or has any other modeled effect;
- which subsequent local branch/use consumes those flags, if representable within the existing bounded CFG.

Do not publish/reconstruct the whole proprietary native function body.

### 2. Add only the missing semantics

If `test` is the only missing local semantic needed, implement its exact bounded taint/flag semantics for the operand forms actually required by owned fixtures/current authenticated code.

Do not add a generic CPU emulator. Preserve fail-closed behavior for partial-register, unsupported, conflicting, indexed, or unmodeled cases.

### 3. Continue only until the next exact proof boundary

After modeling the blocker, determine whether the selected read can affect any state relevant to the writer-call contract:

- returned GPR/XMM state;
- returned flags if observable by caller flow;
- caller/home stack state;
- later same-field reload/new-field-value carrier;
- a memory escape;
- downstream branch path that changes XYZ/W-relevant behavior.

If the local normal-return flow completes, state that explicitly. If it reaches another unsupported instruction/call/alias/cap, stop at that exact point.

Do not infer cross-invocation persistence from local completion. Treat `cross_invocation_feedback` as a separate proof obligation.

### 4. Preserve existing gates

Still forbidden:

- Position writes;
- source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- guessed game ABI/type names;
- data-page mutation/PAGE_GUARD shortcuts;
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

### 5. Desired bounded outcomes

**A.** Local selected-read flow completes and is proven irrelevant to protected downstream XYZ/W state for the modeled scope. Resume the existing protected writer/effect proof only until the next exact blocker.

**B.** Local flow proves a narrow dependence relevant to downstream XYZ/W. Record the causal relation and stop before ownership/mutation work.

**C.** Another exact unsupported/ambiguous instruction, call, alias, cap, or cross-invocation question remains. Report that precise blocker without guessing.

Even Outcome A does not authorize producer `0x351EB00` research or movement writes unless the complete writer contract later passes and a separate task explicitly advances ownership research.

## Tests / handoff

Add owned fixtures for at minimum:

- `test reg,reg` carrying selected-read taint into flags/branch;
- `test reg,reg` with killed/dead taint;
- unrelated `test` operand proving disjointness;
- partial-register/unmodeled refusal;
- branch consuming tainted flags;
- branch consuming untainted flags;
- no regression of c21 same-field read address/order proof;
- no regression of c20 EDI value proof;
- no regression of c19 Transform/Position protection.

Run the relevant full build/regression chain. Update `analysis/V01C21_RUNTIME_RESULT.md` (or successor c22 result file), `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable confirmed/corrected facts in `deluxxxe-dev/MSM2-Wisdoms` when accessible.

If another runtime capture is required, emit only the exact new fields needed and request the shortest A/E test necessary. Commit and push all authorized changes to the current development line.