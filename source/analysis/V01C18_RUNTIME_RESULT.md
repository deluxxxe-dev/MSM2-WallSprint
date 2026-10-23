# WallSprint v0.1c18 actual-game runtime result

Status: **Diagnostic PASS / blocker narrowed; no write authorization.**

## Build/logging
- v0.1c18 / package 0.1.36
- Spider-Man2.exe 2.810.0.0
- SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`
- exact-build gate accepted
- dedicated log path/naming remained correct: `log_wallsprint/WallSprint_20261024-051436.log`, shared fallback disabled.

## A/E regression
Session 1 / A:
- wallrun_duration_ms=2984
- verified_deinit=1
- retention_pass=1
- last_actor_up_dot_surface=1
- stale_suspend/resume=0
- first_hard_kill=none; hard_kill_calls=0
- Position writes by mod=0

Session 2 / E:
- wallrun_duration_ms=4171
- verified_deinit=1
- retention_pass=1
- last_actor_up_dot_surface=1.00000012
- stale_suspend/resume=0
- first_hard_kill=none; hard_kill_calls=0
- Position writes by mod=0

Both sessions finalized 8 true entry snapshots / 8 complete invocation maps and retained XYZ match mask 0x7, source W=0, destination W=1.

## c18 callee CFG result
Authenticated callee remains `0x2B23560..0x2B235E9`.

Actual bounded CFG metadata:
```text
local_branch_sites=5
conditional_branch_sites=4
branch_cap=5
branch_resource_ceiling=8
path_cap_per_call=16
state_cap_per_call=31
blocks_total=8
edges_total=10
blocks_visited=4
edges_visited=6
backedges_seen=0
indirect_transfers_seen=0
nested_calls_seen=2
complete_local_cfg=1
```
C17's generic branch-cap blocker is resolved.

## Reachable write classification
C18 reached real callee writes. Prefix classifications:
```text
0x2B23560 width8 -> caller_shadow_or_return_area, source=0, Transform=0
0x2B23565 width8 -> caller_shadow_or_return_area, source=0, Transform=0
0x2B2356A width8 -> callee_private_stack, source=0, Transform=0
0x2B2358E width4 -> hero_transform_possible_alias, address_root=1, address_offset=268 (0x10C), source=0, Transform=1
```
Summary at the exact blocker:
```text
memory_writes_seen=4
writes_source_exact=0
writes_source_possible_alias=0
writes_transform_exact=0
writes_transform_possible_alias=1
writes_unknown=0
source_span_no_write_proven=0
transform_no_write_proven=0
all_relevant_write_aliases_resolved=0
next_barrier_rva=0x2B2358E
next_barrier_instruction=mov
next_barrier_effect=hero_transform_possible_alias
reason=callee_write_not_proven_disjoint_from_both_protected_objects
```
The exact next problem is no longer CFG coverage. It is the provenance/effect of the single 4-byte write at `0x2B2358E`, currently represented as Transform-root-relative offset 268 / 0x10C but only `possible_alias`, not exact/harmless.

## Writer/upstream gate
Per sampled invocations, `[entry RDX] XYZ` still equals executed Position XYZ (`mask=0x7`), but `xyz_causality_proven=0` because the callee write contract is not complete. W remains unproven.

Keep unchanged:
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```
Do not inspect/mutate producer `0x351EB00` yet.

## Next smallest edge
Resolve only `0x2B2358E`:
1. authenticate its exact decoded destination operand and value source;
2. map `address_root=1` to the exact call-time argument/provenance path;
3. determine whether the write is exactly Hero Transform +0x10C or only a possible alias;
4. determine whether +0x10C overlaps any protected Transform range or affects later Position-store destination/provenance;
5. if harmless/disjoint is proven, resume the existing c18 alias proof past this site; otherwise record the exact semantic/provenance blocker.

No movement implementation is authorized by this result.
