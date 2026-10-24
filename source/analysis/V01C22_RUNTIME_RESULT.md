# v0.1c22 actual-game runtime result

## Evidence

Owner-supplied private runtime log:

```text
MSM2-WS-v0.1c22-20261025-021717.log
```

Authenticated target remained Spider-Man2.exe 2.810.0.0 with the accepted executable SHA256. The c22 run preserved the existing WallRun lifecycle/orientation baseline and introduced no MOD Position/source/input/movement writes.

## Accepted TEST classification

The c22 pinned diagnostic resolves the previous `0x2B235BE` blocker:

```text
site_rva=0x2B235BE
instruction=test
operand0=r8b
operand1=r8b
width=1 byte
flags_only=1
modeled_operand_writes=0
```

For the analyzed state, both operands' parent `R8` carry no selected same-field taint:

```text
parent_may_depend_tags=0x0
flags_may_depend_tags=0x0
```

The first local flags consumer is:

```text
0x2B235C1: je
consumer_may_depend_tags=0x0
first_possible_dependent_branch_rva=0x0
returned_flags_tags=0x0
```

Therefore this exact `TEST r8b,r8b` / `JE` pair is not a carrier of the selected `0x2B23578` `Transform+0x10C` read value for the analyzed bounded state. This does not prove the entire selected-read effect irrelevant.

## Next exact blocker

After modeling the TEST/JE pair, the selected-read walk advances to:

```text
first_unresolved_rva=0x2B235D4
first_unresolved_instruction=call
reason=read_flow_call_or_control_no_expansion
```

Other compact c22 flow state remains:

```text
complete_local_flow=0
return_paths=1
returned_gpr_may_depend_mask=0x1
returned_xmm_may_depend_mask=0x0
returned_flags_may_depend=0
returned_caller_stack_may_depend=0
memory_escape=0
downstream_XYZ_W_relevance=unknown
cross_invocation_feedback=unknown
effect_certificate=0
writable_source_proven=0
```

The next task is therefore not more TEST modeling. It is the smallest bounded classification of the exact call at `0x2B235D4` needed to decide whether selected-read dependence can escape through that call, return state, memory/alias effects, or downstream control/data flow.

## Safety gate

Still closed:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

Do not traverse producer `0x351EB00`; do not implement movement/adhesion/camera/animation/physics from this result.
