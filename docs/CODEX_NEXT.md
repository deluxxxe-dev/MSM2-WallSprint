# Next task — bounded exact JA fan-out after accepted c26

## Status

The v0.1c26 actual-game capture is accepted for the current bounded diagnostic question.

Source log supplied by the owner:

```text
MSM2-WS-v0.1c26-20261025-054449.log
```

Do not merge `main`. Continue from the latest `codex/v01c20-edi-value-provenance` history.

See `analysis/V01C26_RUNTIME_RESULT.md`.

## Accepted c26 facts

The c25 large-table result remains accepted: the authenticated PE runtime-function table is structurally valid with 527161 entries, the actual OS lookup was queried, and exact target `0x2CF7FE0` has no containing runtime-function entry. The strict pinned no-unwind fallback is therefore the active method.

C26 classifies the exact first target instruction:

```text
0x2CF7FE0: cmp r8d, 0x3FFF
operand_count=2
has_memory_operand=0
flags_only=1
form_supported=1
GPR_XMM_memory_writes=0
flags_written=CF_PF_AF_ZF_SF_OF
```

For the analyzed selected-read state:

```text
R8 parent_may_depend_tags=0x0
CMP flags_may_depend_tags=0x0
```

Thus CMP is admitted as a read-only flag effect and is disjoint from selected tag 1.

The strict walk advances exactly one instruction and now stops at:

```text
0x2CF7FE7: ja
condition=CF0_and_ZF0
flags_read=CF_ZF
flags_producer_rva=0x2CF7FE0
flags_dependency_known=1
consumer_may_depend_tags=0x0
control_expanded=0
numerical_outcome=unknown
both_outcomes_unresolved=1
reason=target_leaf_control_or_nested_call_refused
```

Target identity remains unadmitted:

```text
admitted=0
boundary_verified=0
snapshot_authenticated=0
effect_certificate=0
first_unresolved_rva=0x2CF7FE7
first_unresolved_instruction=ja
writable_source_proven=0
```

Do **not** infer the numerical branch direction merely because its flags are disjoint from selected tag 1.

## Current bounded Codex task

Implement the smallest fail-closed extension needed to classify and, only if safe, enumerate the exact `JA` at `0x2CF7FE7` under the pinned no-unwind target proof.

### 1. Classify the exact branch edges first

Before changing control-flow semantics, report compact metadata for the exact branch sufficient to establish:

- encoded conditional branch form and width;
- exact taken target RVA;
- exact fallthrough RVA;
- whether either edge leaves the authenticated code region;
- whether either edge reaches/crosses `next_runtime_rva=0x2CF8020` or another runtime-function region;
- forward/backward relation for each edge;
- condition and exact flags read;
- exact producer of those flags;
- selected/post-write/other-prewrite tag dependence of the condition;
- snapshot stability/current-code pin for the bytes used to derive both edges.

Do not infer semantic function names or ABI types.

### 2. Add only a narrow tag-disjoint conditional-branch fan-out if justified

The existing strict no-unwind contract remains the safety boundary. Do not turn this into a generic CFG emulator.

The exact `JA` may be expanded only if all of the following are proven:

- its condition dependence is known and disjoint from selected/post-write/other-prewrite tags relevant to the tracked read state;
- both taken and fallthrough edges are exact;
- both stay inside the pinned target code allowance and before any authenticated runtime-region boundary;
- no backward edge / loop is introduced;
- branch/path/instruction budgets are explicit and finite;
- both outcomes are explored; no numerical outcome is guessed or selected;
- path state is cloned and merged conservatively.

A suggested tight initial budget is one conditional fan-out, at most two live paths, still bounded by the existing <=128 target bytes and <=32 decoded instructions. Increase only if the actual exact structure proves that the tighter budget is insufficient and a finite justification is documented.

### 3. Preserve the existing leaf prohibitions on every branch leg

Each reachable leg must still stop on the first exact unsupported effect. Continue to refuse:

- memory operand/effect not already explicitly authorized by the strict leaf model;
- stack construction/change;
- nested CALL;
- indirect jump/call/return-like transfer other than a modeled normal RET;
- unsupported partial-register effect;
- self-modifying/current-code instability;
- runtime-region crossing;
- loop/backedge;
- unbounded or unresolved control flow.

Do not weaken these merely to complete target identity.

### 4. Identity/effect admission requires all reachable outcomes

Do not authenticate the target from one branch leg.

Only if **every** reachable path under the admitted exact branch fan-out:

- stays within all caps;
- contains only admitted effects;
- terminates in a plain modeled normal RET;
- has no alternate unresolved exit;
- passes final snapshot/runtime-region coverage checks;

may the target begin/end/size/hash be admitted.

If multiple RET paths exist, merge returned GPR/XMM/flags effects conservatively. A selected tag is considered preserved/escaped/clobbered only according to the union required by all reachable paths; do not choose the most convenient path.

If path effects differ, report that difference explicitly rather than collapsing it to a single deterministic result.

### 5. Resume parent CALL effect analysis only after full target identity passes

If and only if target identity is admitted, feed the complete admitted reachable effect set into the existing c23 call-effect sidecar and classify whether selected tag 1 is:

- preserved into returned GPR/XMM/flags state;
- killed/clobbered;
- escaped to caller/home stack or memory;
- path-dependent;
- blocked by another exact instruction/effect boundary.

Do not broaden into recursive interprocedural analysis.

Even successful local target completion does not settle:

```text
whole writer XYZ/W causality
cross_invocation_feedback
caller/storage ownership
lifetime/exclusivity
writability
```

### 6. Preserve all safety gates

Still forbidden:

- Position writes;
- source-position writes;
- input/movement/adhesion/camera/animation/physics changes;
- producer `0x351EB00` traversal;
- guessed ABI/type names;
- arbitrary data-page mutation/PAGE_GUARD shortcuts;
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

- exact conditional edge decoding for short/near Jcc forms actually supported by the implementation;
- taken/fallthrough RVA calculation and overflow/range refusal;
- current-code/snapshot pin for edge decoding;
- tag-disjoint branch expansion with both outcomes explored;
- selected-tag-dependent branch refusal unless explicitly supported by a future task;
- backward-edge/loop refusal;
- runtime-region crossing refusal on either edge;
- path/instruction/byte cap refusal;
- one leg hitting memory/stack/call/indirect/unsupported effect -> whole identity refusal;
- two safe RET legs with identical effects;
- two safe RET legs with differing returned-tag effects -> conservative/path-dependent result;
- no one-leg-only authentication;
- c26 CMP regression (`cmp r8d,0x3FFF`, flags-only, tag0 flags);
- c25 full large-table validation / genuine missing-entry regression;
- c24 no-unwind safeguards;
- c23 call carrier, c22 TEST/JE, c21 same-field read, c20 EDI provenance and c19 Transform protection.

Run the relevant full build/regression chain. Update a successor analysis/result file, `docs/PROJECT_STATE.md`, `docs/CODEX_RESULT.md`, and reusable confirmed/corrected facts in `deluxxxe-dev/MSM2-Wisdoms` when accessible.

If authenticated current-code evidence after the branch extension is sufficient, do not request another runtime test by habit. If actual branch target/path bytes are unavailable outside the live guarded snapshot, emit only the minimum branch-edge/next-barrier fields and request the shortest A/E-style runtime test necessary. Commit and push all authorized changes to the current development line.
