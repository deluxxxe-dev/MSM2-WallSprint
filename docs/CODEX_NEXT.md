# Next task — WallSprint v0.1c20 EDI Value Provenance

## Base
```text
source branch: codex/v01c19-transform-write-contract
source implementation: b3185cd240a16446a2424ecab9fa0c2605cb00e8
next branch: codex/v01c20-edi-value-provenance
version target: v0.1c20
```

Read first:
- `AGENTS.md`
- `docs/DEV_RULES.md`
- `docs/PROJECT_STATE.md`
- `docs/LOGGING_CONVENTION.md`
- `analysis/V01C19_RUNTIME_RESULT.md`
- `analysis/V01C19_TRANSFORM_WRITE_CONTRACT.md`
- `analysis/RDX_XYZ_SOURCE_CONTRACT.md`
- c19 `docs/CODEX_RESULT.md`

## Accepted c19 actual-game result
C19 is **Diagnostic PASS / exact value-provenance blocker identified**.

Regression gate:
```text
A/session1 duration=3844 ms
E/session2 duration=4094 ms
both verified Deinit
both retention_pass=1
both stale_suspend_count=0
both hard_kill_calls=0
both Position_writes_by_mod=0
both final Up dot surface=1
```
Both sessions finalized 8 true entry snapshots / 8 complete invocation maps with XYZ mask `0x7`, source W=0 and destination W=1.

C19 proved the exact destination of the former c18 alias:
```text
site_rva=0x2B2358E
instruction=mov
width=4
destination=[RCX+0x10C]
dst provenance=entry_writer_RCX_transform
address_class=hero_transform_exact
may_alias_source=0
may_alias_transform=1
write_overlaps_position_range=0
write_overlaps_transform_identity_or_pointer_carrier=0
```

The new exact blocker is the value source:
```text
value_source_register_or_memory=edi
value_source_kind=unknown
value_source_provenance_exact=0
all_reaching_states_same_value_origin=0
all_reaching_states_examined=0
write_affects_writer_reload_provenance=unknown
write_effect_relevant_to_xyz=unknown
write_effect_relevant_to_w=unknown
reason=exact_write_value_unknown_computed_or_reaching_origins_conflict
```

## Objective
Resolve **only the value provenance of EDI at `0x2B2358E` and its relevance to later Position/source-carrier reads**.

Answer:
1. Which exact local instruction(s) produce the EDI value reaching `0x2B2358E`?
2. Are all reaching paths covered, and do they have one stable origin?
3. Is the value constant, copied from an entry register, loaded from memory, or computed?
4. Does the Transform+0x10C write influence any later read/carrier needed for Position XYZ/W causality?
5. If irrelevant, can the write be safely modeled and writer proof resume past `0x2CFC92C`?

Do not traverse or mutate upstream producer `0x351EB00`. Do not implement movement.

## Required proof
Use the authenticated c19 callee CFG and exact current-code/hash gates. Trace EDI backward only within the exact authenticated callee and only as far as required to establish its reaching definitions. Preserve separate states across branches; conflicting origins remain unresolved.

Emit compact metadata, for example:
```text
MOVEMENT_CALLEE_VALUE_CONTRACT
site_rva=0x2b2358e
value_register=edi
reaching_definition_count=
producer_rva=
producer_kind=constant|register_copy|memory_load|computed|unknown
producer_root=
producer_offset=
all_reaching_states_examined=0|1
all_reaching_states_same_value_origin=0|1
later_relevant_read_count=
first_dependency_rva=
write_effect_relevant_to_xyz=0|1|unknown
write_effect_relevant_to_w=0|1|unknown
reason=
```

Do not invent semantics for Transform+0x10C or EDI.

## Safe continuation
Only if EDI provenance is exact on all reaching paths and the Transform+0x10C write is proven non-causal to Position/source-carrier reads may c20 mark:
```text
transform_position_protected_range_unmodified=1
relevant_transform_effects_resolved=1
```
and continue the existing writer XYZ/W proof.

If another exact callee/writer instruction becomes the blocker, report that exact RVA/mnemonic/effect and stop.

## Safety boundary
Keep unchanged:
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```
No Actor/Transform Position writes, source mutation, input rewrite, movement/velocity/gravity/adhesion, camera/animation/traversal, PAGE_GUARD, data-page protection mutation, guessed game calls, or producer `0x351EB00` traversal.

Preserve dedicated logging:
```text
log_wallsprint\WallSprint_YYYYMMDD-HHMMSS.log
```
with no fallback to shared `logs`.

## Release naming
The user-facing install ZIP must be named exactly:
```text
MSM2-WS-v0.1c20.zip
```
Do not append task descriptions such as `edi_value_provenance` or any other explanatory suffix. Follow `docs/DEV_RULES.md` for future versions as well.

## Human test
If actual-game evidence is required:
```text
A/cardinal ~4 s -> fully end WallRun -> move/fast-travel -> E/diagonal ~4 s -> send newest complete timestamped file from log_wallsprint
```
No special input. Video only for a visible anomaly.

## PASS
A. EDI reaching definitions and relevance are exact; the +0x10C write is safely modeled and writer proof advances/completes.

B. One narrower exact value/dependency blocker is identified with all safety gates preserved.

Do not implement movement automatically.

## Required outputs
Update/create:
```text
analysis/V01C19_RUNTIME_RESULT.md
analysis/V01C20_EDI_VALUE_PROVENANCE.md
analysis/V01C20_GATE.md
analysis/RDX_XYZ_SOURCE_CONTRACT.md
README_TEST.md
docs/PROJECT_STATE.md
docs/CODEX_RESULT.md
```
Package the user-facing install release as `MSM2-WS-v0.1c20.zip`.