# Next task — bounded 0x2B23578 read-dependency classification after c20

## Status

The v0.1c20 actual-game A/E capture is accepted and recorded in:

- `analysis/V01C20_RUNTIME_RESULT.md`

C20 answered the immediate EDI question at `0x2B2358E`:

```text
0x2B2358E: mov dword ptr [RCX+0x10C], EDI
EDI local producer: 0x2B2356F
producer kind: memory_load
origin: exact executable-image RIP-relative global (c20 root16)
image-relative offset: 0x0C323788
```

The local EDI reaching-definition contract succeeded for the analyzed authenticated call input, but the whole callee/writer effect contract did not. The next exact blocker is the earlier decoded read at:

```text
0x2B23578
```

with current conservative reason:

```text
earlier_read_provenance_dependency
exact_transform_write_may_change_a_decoded_read_carrier
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history unless this task deliberately creates and fully hands off a successor branch.

## Required reading

Before editing code, read:

1. `AGENTS.md`
2. `docs/DEV_RULES.md`
3. `docs/PROJECT_STATE.md`
4. `analysis/V01C20_RUNTIME_RESULT.md`
5. `analysis/V01C20_EDI_VALUE_PROVENANCE.md`
6. `analysis/V01C19_TRANSFORM_WRITE_CONTRACT.md`
7. `analysis/RDX_XYZ_SOURCE_CONTRACT.md`
8. `analysis/ENGINE_INTERNALS_REFERENCE.md`
9. the relevant `source/src/NativeHooks/rdx_source_contract.cpp/.hpp` implementation/tests
10. `deluxxxe-dev/MSM2-Wisdoms` technical references when accessible

Project-local measured evidence remains authoritative over a stale shared summary.

## Accepted c20 facts

Keep these facts separate:

### EDI local value proof — accepted

```text
producer_rva=0x2B2356F
producer_kind=memory_load
origin_kind=memory_load
root=16 = analyzer globalRoot / exact RIP-relative image address
origin offset=0x0C323788
reaching definitions=1
all local reaching states examined=1
same local value origin=1
value provenance exact=1
```

This is a static value-provenance result authenticated by the actual-game current callee identity. It does **not** give a semantic engine field name to the image-global value.

### Whole writer/effect proof — still blocked

```text
all_writer_call_states_examined=0
relevant_transform_effects_resolved=0
transform_position_protected_range_unmodified=0
source_span_no_write_proven=0
XYZ/W causal proof incomplete
```

The current dependency site `0x2B23578` occurs before the exact write at `0x2B2358E`. The existing analyzer conservatively refuses because the write may alter a decoded read carrier. Do not resolve this by assumption in either direction.

## Current bounded Codex task

Implement the smallest evidence-preserving diagnostic/static-analysis extension necessary to classify the `0x2B23578` dependency precisely.

### 1. Report the exact read

For the authenticated current callee, emit compact metadata for the blocker at `0x2B23578`:

- mnemonic/operand class without publishing the full private function body;
- read width;
- exact address provenance root/offset when modeled;
- whether it is Transform/source/stack/image-global/other/unknown;
- whether it overlaps `Transform+[0x10C,0x110)`;
- the value carrier/register produced by the read when representable.

Do not infer semantic type names from address shape.

### 2. Prove ordering rather than treating all dependencies alike

The dependency is currently an **earlier read** relative to the write at `0x2B2358E`.

Extend the bounded local model only enough to distinguish, on every relevant local path:

```text
read-before-write
read-after-write
conflicting/branch-dependent order
unresolved order
```

A read-before-write cannot be relabeled automatically as irrelevant: if its value feeds post-callee state or if the persisted field can feed later invocations, that still needs an explicit model. Conversely, do not claim causal dependence merely because the conservative c19/c20 gate stopped there.

### 3. Track only the necessary value flow

Determine whether the exact `0x2B23578` read can affect state that survives the callee and is relevant to the downstream writer XYZ/W proof.

If necessary, distinguish:

- same-invocation pre-write read value;
- the new value written at `0x2B2358E`;
- any later reload of the same field;
- a possible prior-invocation -> next-invocation feedback relation.

Do not add a generic interprocedural engine model or unbounded fixpoint. If cross-invocation persistence cannot be proven with existing evidence, report that exact blocker instead.

### 4. Preserve all existing gates

Keep fail-closed behavior and all current authenticated-build/current-code/boundary/hash checks.

Still forbidden:

- Position writes;
- source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- guessed game calls or ABI types;
- PAGE_GUARD/data-page mutation as a shortcut;
- weakening ownership/lifetime/exclusivity requirements.

Current safety state remains:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

### 5. Desired outcome

The task should end in one of these bounded outcomes:

**A.** The `0x2B23578` read is exactly classified and proven not to make the native `+0x10C` write relevant to downstream XYZ/W for the modeled scope; resume the existing full protected-effect/writer path until the next exact blocker or XYZ/W completion.

**B.** The read/value flow is exactly classified as relevant; record the narrow causal relationship and the next required proof.

**C.** The analyzer reaches a narrower unsupported/ambiguous point; report that exact RVA/operand/order/value blocker without guessing.

Even Outcome A does not authorize `0x351EB00` or movement writes unless the complete writer contract subsequently passes and a later task explicitly advances ownership research.

## Tests / handoff

Add owned fixtures for at minimum:

- exact same-field read-before-write with no later use;
- read-before-write whose value survives and affects a protected downstream carrier;
- read-after-write;
- branch paths with differing order;
- exact disjoint image/global/stack/source reads;
- unknown/indexed/conflicting address refusal;
- no regression of c20 EDI local value proof;
- no regression of c19 Transform/Position protection.

Run the relevant full build/regression chain, update `docs/CODEX_RESULT.md`, project state/analysis as authorized by this task, commit and push. If a runtime capture is still required after the static/fixture work, specify only the exact new log fields and the shortest A/E test needed.

Synchronize reusable confirmed/corrected engine facts to `deluxxxe-dev/MSM2-Wisdoms` when accessible; keep detailed proof in this repository.