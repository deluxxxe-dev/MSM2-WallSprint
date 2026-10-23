# WallSprint v0.1c18 actual-game runtime result

Status: **Diagnostic PASS / blocker narrowed; no write authorization.**

Actual v0.1c18/package0.1.36 A/E regression passed: A 2984 ms, E 4171 ms; both verified Deinit, retention pass, stale0, hard0 and Position writes0. Final Surface Up dot was 1 / 1.00000012. Both sessions finalized 8 true entry snapshots / 8 complete maps with XYZ mask0x7, source W0, destination W1. Dedicated `log_wallsprint/WallSprint_20261024-051436.log` remained correct with shared fallback disabled.

C18 resolved c17's callee branch-cap blocker. Actual authenticated `0x2B23560..0x2B235E9` local CFG has 5 local branch sites, 4 conditional sites, 8 blocks, 10 edges, branch_cap=5, no backedges or indirect transfers, and `complete_local_cfg=1`.

Reachable write classifications before the exact new blocker:
```text
0x2B23560 width8 -> caller_shadow_or_return_area; source0 Transform0
0x2B23565 width8 -> caller_shadow_or_return_area; source0 Transform0
0x2B2356A width8 -> callee_private_stack; source0 Transform0
0x2B2358E width4 -> hero_transform_possible_alias; address_root=1; offset=268/0x10C; source0 Transform1
```

Exact blocker:
```text
next_barrier_rva=0x2B2358E
next_barrier_instruction=mov
next_barrier_effect=hero_transform_possible_alias
writes_transform_possible_alias=1
source_span_no_write_proven=0
transform_no_write_proven=0
all_relevant_write_aliases_resolved=0
```

The next smallest edge is to resolve only the destination/value/provenance and Position-range relevance of this 4-byte write. CFG coverage itself is no longer the blocker. `[entry RDX] XYZ` continues to match exact Position XYZ bits (`0x7`) but causality remains unproven. `writable_source_proven=0`; no producer/movement/input work is authorized.
