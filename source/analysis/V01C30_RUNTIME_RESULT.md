# v0.1c30 actual-game runtime result

Source: owner-supplied private log `MSM2-WS-v0.1c30-20261025-133648.log`.

## Build identity

```text
WallSprint v0.1c30
package_version=0.1.48
Spider-Man2.exe=2.810.0.0
exe_sha256=c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
```

## Accepted c30 result

C30's exact pinned unknown-load abstraction worked as intended at the accepted c28/c29 site:

```text
site_rva=0x2CF7FE9
source_rva=0xC2D2580
width=8
abstract_effect_admitted=1
loaded_may_depend_top=0x7
precise_value_tags_known=0
observed_taint=0
selected_dependence_killed=0
source_value_reads=0
raw_value_logged=0
numeric_interpretation=0
pointer_chase=0
origin=unknown_loaded_qword_not_pointer
flags_other_GPR_XMM_SP_memory_preserved=1
reason=exact_source_unknown_may_top_current_proof_no_value_read
```

The instruction record correspondingly reports:

```text
0x2CF7FE9: mov rax,qword ptr [rcx+0x100]
semantic_load_modeled=1
reason=target_MOV_exact_source_unknown_may_top_modeled
```

This does **not** prove the writable `.data` value carries all tracked dependencies. `0x7` is a may-depend upper bound over selected/postwrite/other-prewrite. It deliberately preserves selected dependence as possible and uses no qword value observation.

The prior c29 immutable/tag0 route remains refused because the exact source is still in writable `.data` on PAGE_READWRITE. C30 does not weaken or retry that rule.

## New exact barrier

The bounded target walk advanced by one instruction and now stops at:

```text
barrier_rva=0x2CF7FF0
barrier_instruction=vmovups
reason=target_cfg_memory_operand_refused
path=0
edge_from_rva=0x2CF7FE7
edge_target_rva=0x2CF7FE9
```

Pre-instruction abstract state on that path:

```text
GPR0/RAX tags=0x7
all other logged GPR tags=0
all logged XMM tags=0
flags_may_depend_tags=0
upper_bound_not_observed=1
raw_values=0
```

Target state:

```text
TARGET_IDENTITY admitted=0
instructions=4
barrier_rva=0x2CF7FF0
barrier_instruction=vmovups
LEAF_CFG complete=0
return_paths=0
unique_instructions=4
visits=4
final_code_rechecked=0
effect_certificate=0
```

The parent direct CALL remains unadmitted and now reports the same `0x2CF7FF0` barrier. Do not infer VMOVUPS operands, vector width, memory direction, address, alias class, or upper-lane semantics from the mnemonic/refusal alone.

## Safety

Observed WallRun sessions remained fail-closed for movement writes:

```text
source_position_writes=0
Position_writes=0
adhesion corrections_applied=0
verified_deinit=1
retention_pass=1
first_hard_kill=none
hard_kill_calls=0
```

`writable_source_proven`, ownership/exclusivity/lifetime/conflict gates remain zero. No gameplay write permission follows from c30.

## Accepted interpretation

- c30 validates the narrow conservative principle: when one exact full-width load is already authenticated but its writable-source value provenance cannot be made precise, a true may-lattice top can model the abstract destination effect without reading the value, provided the rule is pinned to that exact form/source and preserves every possible tracked category.
- This is not generic memory-load permission and not observed taint.
- The exact next blocker is VMOVUPS at `0x2CF7FF0`.
- Next work must classify VMOVUPS exactly before adding any vector-memory semantics.
