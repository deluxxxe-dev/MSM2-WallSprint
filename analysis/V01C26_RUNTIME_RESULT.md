# v0.1c26 actual runtime result — exact CMP admitted, next barrier JA

## Source / scope

Accepted owner-supplied private runtime log:

```text
MSM2-WS-v0.1c26-20261025-054449.log
```

Build identity in the log:

```text
WallSprint v0.1c26
package_version=0.1.44
Spider-Man2.exe 2.810.0.0
exe SHA256=c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

This result answers only the bounded c26 question at the exact no-unwind target `0x2CF7FE0`. It does not authorize movement/source writes or settle caller ownership, lifetime, exclusivity, XYZ/W causality, or producer `0x351EB00`.

## Accepted result

The c25 runtime-table conclusion remains intact:

```text
target_rva=0x2CF7FE0
runtime_lookup_queried=1
runtime_lookup_found=0
PE_runtime_table_valid=1
PE_runtime_entries=527161
missing_runtime_entry=1
next_runtime_rva=0x2CF8020
snapshot_stable=1
```

The exact first target instruction is now classified:

```text
site_rva=0x2CF7FE0
instruction=cmp
operand_count=2
has_memory_operand=0
flags_only=1
form_supported=1
GPR_XMM_memory_writes=0
flags_written=CF_PF_AF_ZF_SF_OF
```

Operands:

```text
operand 0: register r8d, width=4, parent=r8
operand 1: immediate 16383 (0x3FFF), width=4
```

For the analyzed selected-read state, R8 is disjoint from the selected/post-write/other-prewrite tags:

```text
r8 parent_may_depend_tags=0x0
CMP flags_may_depend_tags=0x0
```

Therefore c26 successfully models the exact CMP as a read-only flags effect. CMP does not write GPR/XMM/memory state and does not carry selected tag 1 into flags for this state.

## New exact blocker

After admitting CMP, the strict no-unwind walk advances to the next instruction:

```text
barrier_rva=0x2CF7FE7
barrier_instruction=ja
reason=target_leaf_control_or_nested_call_refused
instructions=2
admitted=0
boundary_verified=0
snapshot_authenticated=0
effect_certificate=0
```

The conditional branch record is:

```text
site_rva=0x2CF7FE7
instruction=ja
conditional=1
condition=CF0_and_ZF0
flags_read=CF_ZF
flags_producer_rva=0x2CF7FE0
flags_dependency_known=1
consumer_may_depend_tags=0x0
control_expanded=0
numerical_outcome=unknown
both_outcomes_unresolved=1
rule=strict_linear_no_branch
```

Important interpretation:

- The exact branch condition is proven disjoint from selected tag 1 for this analyzed state.
- That does **not** prove which numerical branch outcome occurs.
- It also does **not** permit following only fallthrough or only taken edge, because downstream selected-tag effects may differ by path.
- Target identity/effect remains unadmitted until every reachable path admitted by a future bounded branch rule is proven safe.

## Selected-read flow state

The parent selected-read flow now stops at JA rather than CMP:

```text
first_unresolved_rva=0x2CF7FE7
first_unresolved_instruction=ja
returned_gpr_may_depend_mask=0x1
memory_escape=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
effect_certificate=0
writable_source_proven=0
```

The raw selected carrier at the parent CALL remains GPR index 0 with tag 1 and origin `Hero Transform+0x10C`; this is an analyzer carrier fact, not a typed ABI argument.

## Runtime safety

Two WallRun sessions completed with the write gate closed.

Session 1:

```text
verified_deinit=1
retention_pass=1
path_a_writes=165
path_b_writes=164
first_hard_kill=none
hard_kill_calls=0
Position_writes=0
source_position_writes=0
position_proof_admitted=0
corrections_applied=0
```

Session 2:

```text
verified_deinit=1
retention_pass=1
path_a_writes=215
path_b_writes=213
first_hard_kill=none
hard_kill_calls=0
Position_writes=0
source_position_writes=0
position_proof_admitted=0
corrections_applied=0
```

All ownership/writability gates remain closed:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
```

## Next bounded question

The next smallest proof step is not to infer the JA outcome. It is to classify its exact taken target and fallthrough from the same guarded snapshot, then decide whether a narrowly bounded, tag-disjoint conditional-branch fan-out can be admitted without weakening the existing no-memory/no-stack/no-call/no-runtime-region-crossing contract.

A safe successor may enumerate **both** outcomes only if:

- branch target and fallthrough are exact and remain inside the pinned allowed code region;
- no edge reaches/crosses `next_runtime_rva=0x2CF8020` or another authenticated runtime region;
- branch is forward/acyclic under a strict bounded CFG budget;
- all reachable instructions on both legs satisfy the existing leaf-effect rules;
- every reachable exit is a modeled normal RET;
- path effects are merged conservatively, never by selecting a guessed outcome.

If either edge reaches memory/stack construction, nested call/indirect transfer, unsupported effect, loop/backedge, cap, runtime-region crossing, or another unresolved branch outside the narrow rule, stop at that exact boundary.

Producer `0x351EB00` remains deferred. No gameplay write is authorized.
