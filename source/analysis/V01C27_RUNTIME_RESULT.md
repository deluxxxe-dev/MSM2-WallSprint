# V0.1c27 actual runtime result — exact JA fan-out accepted, fallthrough MOV memory barrier

## Evidence

Owner-supplied private runtime log:

```text
MSM2-WS-v0.1c27-20261025-113718.log
```

Exact admitted executable identity remains unchanged:

```text
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

No raw private game body or memory dump is reproduced here.

## Accepted c27 result

The c25/c26 prerequisite chain remains valid: the authenticated PE runtime-function table has 527161 fully validated entries, OS lookup was actually queried, exact target `0x2CF7FE0` has no containing runtime-function entry, and the pinned no-unwind proof starts from the accepted `cmp r8d,0x3FFF` whose relevant tags are zero for the analyzed state.

C27 successfully classifies and expands the exact branch at `0x2CF7FE7`:

```text
site_rva=0x2CF7FE7
instruction=ja
encoding=short_JA_rel8
instruction_bytes=2
displacement_bytes=1
displacement=42
condition=CF0_and_ZF0
flags_read=CF_ZF
flags_producer_rva=0x2CF7FE0
consumer_may_depend_tags=0x0
edges_exact=1
taken_rva=0x2CF8013
fallthrough_rva=0x2CF7FE9
taken_in_code=1
fallthrough_in_code=1
taken_in_allowance=1
fallthrough_in_allowance=1
taken_forward=1
fallthrough_forward=1
taken_runtime_cross=0
fallthrough_runtime_cross=0
snapshot_stable=1
control_expanded=1
numerical_outcome=unknown
outcome_narrowing=0
rule=pinned_JA_both_forward_paths
```

This accepts the c27 narrow two-path fan-out mechanism for this exact branch. It does **not** select a numerical outcome; both edges remain semantically possible.

## New exact blocker

Target identity still does not complete. The bounded CFG stops immediately on the fallthrough leg at:

```text
barrier_rva=0x2CF7FE9
barrier_instruction=mov
reason=target_cfg_memory_operand_refused
```

The aggregate bounded CFG state is:

```text
complete=0
return_paths=0
unique_instructions=3
visits=3
branch_cap=1
path_cap=2
byte_cap=128
instruction_cap=32
visit_cap=64
all_paths_required=1
final_code_rechecked=0
effect_certificate=0
```

Therefore c27 proves the branch geometry and safe fan-out, but no path reaches an admitted RET and no target begin/end/hash/effect is authenticated yet.

The selected-read parent flow correspondingly advances its first unresolved point from the JA to the MOV:

```text
first_unresolved_rva=0x2CF7FE9
first_unresolved_instruction=mov
returned_gpr_may_depend_mask=0x1
memory_escape=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
effect_certificate=0
writable_source_proven=0
reason=target_cfg_memory_operand_refused
```

The parent CALL still carries selected tag 1 in raw GPR carrier index 0. Do not assign a semantic ABI/type name to that index.

## Interpretation

The c26 JA barrier is resolved. The exact next question is no longer control-flow fan-out; it is the operand/effect form of `0x2CF7FE9: mov` on the fallthrough leg.

The current log does not provide sufficient compact metadata to identify that MOV's exact source/destination operands, width, address base/index/displacement, read/write direction, memory region class, or tag/alias implications. Do not infer any of those from the mnemonic or the generic refusal reason.

A successor may first add diagnostic-only operand/address classification for this exact MOV. Any semantic admission must remain narrower than a generic memory model. In particular, a memory write, unknown alias, dynamic/unclassified load, selected-tag escape, stack mutation, or other unsupported effect remains an exact fail-closed boundary. A read-only memory load may be considered only if its address and loaded-value provenance can be conservatively established under existing owned state without observing/guessing arbitrary memory values.

The taken leg `0x2CF8013` is not sufficient by itself to authenticate anything. `return_paths=0` and all-path admission remains required.

## Runtime safety

Two WallRun sessions remained within the existing safety envelope.

Session 1:

```text
verified_Deinit=1
retention_pass=1
first_hard_kill=none
hard_kill_calls=0
Position_writes=0
source_position_writes=0
corrections_applied=0
```

Session 2:

```text
verified_Deinit=1
retention_pass=1
first_hard_kill=none
hard_kill_calls=0
Position_writes=0
source_position_writes=0
corrections_applied=0
```

No movement-source write authorization follows from c27.

## Accepted conclusion / next boundary

Accepted:

```text
CMP 0x2CF7FE0 crossed
JA 0x2CF7FE7 exact edges classified
both forward edges safely enumerated
fallthrough=0x2CF7FE9
taken=0x2CF8013
JA control expanded without numerical outcome choice
```

Still unaccepted:

```text
target identity/span/hash/effect
MOV 0x2CF7FE9 operand/effect semantics
any complete return path
whole writer XYZ/W causality
cross-invocation feedback
caller/storage ownership
source lifetime/exclusivity
writability
```

Safety remains:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Producer `0x351EB00` remains deferred.
