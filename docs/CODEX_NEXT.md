# Next task — WallSprint v0.1c19 Exact Transform-Alias Write Contract

## Base
```text
source branch: codex/v01c18-callee-branch-alias
source implementation: 19aa3012959bf7538df3917278dd4a97551123dd
source delivery head before human result: 3ffc19b5b3b3f42c0fcbf62679fe6d9e44f92dfb
next branch: codex/v01c19-transform-write-contract
version target: v0.1c19
```

Read first:
- `AGENTS.md`
- `docs/DEV_RULES.md`
- `docs/PROJECT_STATE.md`
- `docs/LOGGING_CONVENTION.md`
- `analysis/V01C18_RUNTIME_RESULT.md`
- `analysis/V01C18_CALLEE_BRANCH_ALIAS.md`
- `analysis/RDX_XYZ_SOURCE_CONTRACT.md`
- c18 `docs/CODEX_RESULT.md`

## Accepted c18 actual-game result
C18 is **Diagnostic PASS / narrower exact write blocker**.

Regression/logging gate:
```text
A/session1 duration=2984 ms
E/session2 duration=4171 ms
both verified Deinit
both retention_pass=1
both stale_suspend_count=0
both hard_kill_calls=0
both Position_writes_by_mod=0
A final Up dot surface=1
E final Up dot surface=1.00000012
```
Both sessions finalized 8 true entries / 8 complete maps with XYZ mask `0x7`, source W=0, destination W=1. Dedicated `log_wallsprint/WallSprint_YYYYMMDD-HHMMSS.log` remains verified with no shared fallback.

Authenticated call/callee remains:
```text
writer call=0x2CFC927
return=0x2CFC92C
target=0x2B23560
boundary=0x2B23560..0x2B235E9
hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
```
Call-time RCX and RDX are Transform-derived; writer-entry RDX Position source is distinct. R8 is constant/non-pointer; R9 remains unknown.

C18 resolved the c17 branch-cap problem:
```text
local_branch_sites=5
conditional_branch_sites=4
branch_cap=5
blocks_total=8
edges_total=10
complete_local_cfg=1
backedges_seen=0
indirect_transfers_seen=0
```

Actual reachable write classifications before the new blocker:
```text
0x2B23560 width8 -> caller_shadow_or_return_area, no source/Transform alias
0x2B23565 width8 -> caller_shadow_or_return_area, no source/Transform alias
0x2B2356A width8 -> callee_private_stack, no source/Transform alias
0x2B2358E width4 -> hero_transform_possible_alias, address_root=1, address_offset=268 (0x10C), source=0, Transform=1
```
Current exact blocker:
```text
next_barrier_rva=0x2B2358E
next_barrier_instruction=mov
next_barrier_effect=hero_transform_possible_alias
writes_transform_possible_alias=1
source_span_no_write_proven=0
transform_no_write_proven=0
all_relevant_write_aliases_resolved=0
reason=callee_write_not_proven_disjoint_from_both_protected_objects
```

## Objective
Resolve **only the exact 4-byte write at callee RVA `0x2B2358E`**.

Answer:
1. What is the exact decoded destination operand and its base/index/displacement?
2. What does `address_root=1` mean on every reaching state, and is the destination exactly Hero Transform + `0x10C` or merely a possible alias?
3. What is the exact value source written by this `mov`?
4. Does the 4-byte destination overlap the protected Transform range relevant to later Position stores (`+0x30..+0x3F`) or any carrier/provenance needed by writer continuation?
5. Can this write be proven irrelevant/disjoint to Position XYZ/W causality, or is it a real Transform-side dependency that must be modeled?

Do not inspect/mutate upstream producer `0x351EB00`. Do not implement movement.

## 1. Authenticate the exact instruction
Under all existing exact exe/SHA, writer, call, callee boundary/hash/current-code gates, decode `0x2B2358E` exactly.

Emit compact metadata only, e.g.:
```text
MOVEMENT_CALLEE_WRITE_CONTRACT
site_rva=0x2b2358e
mnemonic=mov
width=4
dst_base=
dst_index=
dst_scale=
dst_disp=
dst_provenance=
value_source_kind=
value_source_register_or_memory=
value_source_provenance=
reaching_state_count=
all_reaching_states_same_destination=0|1
all_reaching_states_same_value_origin=0|1
```
Do not publish the full private callee body.

## 2. Resolve address_root=1 precisely
Use the c18 CFG states and c17 call-argument provenance. The current compact result says root1 +268 / 0x10C may alias Transform. Do not assume root1 identity from numbering alone; prove it from the actual provenance representation.

For every reaching path to `0x2B2358E`:
- identify the exact pointer root;
- preserve any MOV/LEA/add/sub/copy chain;
- detect conflicting roots/offsets;
- report whether the effective address is exactly Transform+0x10C, another exact object, or still ambiguous.

If any path differs, keep `possible_alias` and report the exact path/provenance conflict.

## 3. Compare against the protected Transform ranges
The Position writer later stores:
```text
+0x30 X
+0x34 Y
+0x38 Z
+0x3C W
```
The c19 question is not whether Transform is touched anywhere; it is whether this exact +0x10C candidate write can alter the destination identity, source carriers, Position rows, or any state needed to prove entry-RDX XYZ reaches the later Position stores.

Explicitly classify:
```text
write_overlaps_position_range=0|1
write_overlaps_transform_identity_or_pointer_carrier=0|1
write_affects_writer_reload_provenance=0|1|unknown
write_effect_relevant_to_xyz=0|1|unknown
write_effect_relevant_to_w=0|1|unknown
```
Do not infer semantics for Transform+0x10C beyond what is proven.

## 4. Determine the written value origin
Track the source of the `mov` value only enough to decide causality relevance. It may be constant/register/memory/computed/unknown. No semantic label without proof.

If the write is exactly Transform+0x10C but provably outside all Position/provenance ranges, classify it as a Transform write that is **non-overlapping and irrelevant to this narrow Position proof**, rather than pretending the callee is Transform-write-free.

This means c19 may allow writer proof continuation even with a Transform write, but only if the exact effect is proven non-overlapping/non-causal for XYZ/W.

## 5. Resume the c18 alias proof only if this write is safely modeled
If and only if all paths to `0x2B2358E` have exact destination/value provenance and the write is proven irrelevant to protected Position/provenance, feed a modeled safe effect back into the c18 callee analysis and continue to the next reachable write/control effect.

Then either:
- finish all callee relevant effects and resume writer proof from `0x2CFC92C`, or
- report the next exact blocker inside the callee/writer.

Do not blanket-set `transform_no_write_proven=1` if a real Transform write exists. Instead introduce/retain a stricter notion such as `transform_position_protected_range_unmodified=1` / `relevant_transform_effects_resolved=1` and make writer continuation depend on the narrow protection actually required.

Keep existing telemetry backward-compatible where possible and document the distinction clearly.

## 6. Runtime observation only if static provenance cannot distinguish exact pointer identity
Prefer static proof. If a single exact pointer-root ambiguity remains, add only the smallest bounded read-only observation of natural game execution for max 8 eligible invocations/session. Allowed observations: effective destination address at this site, Transform address, source-span address, value bits, return/session/thread identity, code integrity. Do not mutate the instruction or value. Before/after equality alone cannot prove no transient effect.

No PAGE_GUARD/data-page protection mutation and no manual callee invocation.

## 7. Preserve safety boundary
Even if c19 fully clears this write and completes writer XYZ/W causality:
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```
must remain unchanged.

Producer `0x351EB00` is still a later separate task.

Preserve all c7-c18 lifecycle/orientation/stale/hard-kill/secondary/publication-refresh/DR-watch/direct-call/entry-pairing/accounting/callee identity/CFG/logging/cleanup protections.

## Tests
Add owned/offline tests for:
- exact Transform+0x10C write outside Position +0x30..0x3F and safe continuation;
- exact write overlapping +0x30/+0x34/+0x38/+0x3C => reject;
- same site with conflicting reaching roots => possible alias/refuse;
- same root but conflicting displacement => refuse;
- value constant/register/memory/computed/unknown classification;
- write affecting a later reload carrier even without direct Position overlap => reject;
- modeled irrelevant Transform write followed by later safe callee writes and successful writer continuation;
- exact later blocker reporting;
- all c18 regressions and dedicated logging unchanged.

## Human test
If actual-game evidence is required after cloud validation:
```text
A/cardinal ~4 s -> fully end WallRun -> move/fast-travel -> E/diagonal ~4 s -> send newest complete timestamped file from log_wallsprint
```
No special input. Video only for visible anomaly.

## PASS
A. `0x2B2358E` destination/value/effect is exact and irrelevant to protected Position/provenance; analysis advances to the next exact blocker or completes callee/writer causality.

B. One exact unresolved/relevant effect at `0x2B2358E` is identified more precisely than c18, with all safety gates preserved.

Do not implement movement automatically.

## Required outputs
Update/create:
```text
analysis/V01C18_RUNTIME_RESULT.md
analysis/V01C19_TRANSFORM_WRITE_CONTRACT.md
analysis/V01C19_GATE.md
analysis/RDX_XYZ_SOURCE_CONTRACT.md
README_TEST.md
docs/PROJECT_STATE.md
docs/CODEX_RESULT.md
```
Package c19 diagnostic release/source per existing rules.
