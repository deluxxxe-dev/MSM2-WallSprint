# Next task — WallSprint v0.1c18 Callee Branch-Aware Alias Contract

## Base
```text
source branch: codex/v01c17-callee-alias-contract
source implementation: d1867bb2de42079a46ceb547f8fccc4d477ae18d
source delivery head: 4f7d1590af744f449d717b8ede30400de0cef820
next branch: codex/v01c18-callee-branch-alias
version target: v0.1c18
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `docs/LOGGING_CONVENTION.md`, `analysis/V01C17_RUNTIME_RESULT.md`, `analysis/V01C17_CALLEE_ALIAS_CONTRACT.md`, `analysis/RDX_XYZ_SOURCE_CONTRACT.md`, and c17 `docs/CODEX_RESULT.md` first.

## Accepted c17 actual-game result
C17 is **Diagnostic PASS / narrower exact callee CFG blocker**.

```text
A/session1 duration=3875 ms
E/session2 duration=4390 ms
both verified Deinit
both retention_pass=1
both stale_suspend_count=0
both hard_kill_calls=0
both Position_writes_by_mod=0
both final Up dot surface=1
```
Both sessions finalized 8 true entry snapshots / 8 complete invocation maps with XYZ mask0x7, source W0 and destination W1.

Dedicated logging is actual-game verified:
```text
game-root\log_wallsprint\WallSprint_YYYYMMDD-HHMMSS.log
shared_logs_fallback=0
```
Preserve it unchanged.

C17 actual writer call arguments:
```text
call_rva=0x2CFC927
arg_rcx_origin=entry_writer_RCX_transform
arg_rdx_origin=entry_writer_RCX_transform
arg_r8_origin=constant_bits_NOT_pointer
arg_r9_origin=unknown_argument
source_span_alias_mask=0x0
transform_alias_mask=0x3
proof_complete=1
```

Authenticated callee:
```text
target=0x2B23560
boundary=0x2B23560..0x2B235E9
hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
```

Exact c17 blocker:
```text
memory_writes_seen=0
source_span_no_write_proven=0
transform_no_write_proven=0
all_relevant_write_aliases_resolved=0
next_barrier_rva=0x2B235C1
next_barrier_instruction=je
next_barrier_effect=indirect_or_branch_cap
reason=callee_indirect_transfer_or_branch_cap_refused
```
`memory_writes_seen=0` means the analysis stopped before write classification; it is not proof of a write-free callee.

## Objective
Resolve only the local control flow inside authenticated 137-byte callee `0x2B23560..0x2B235E9`, beginning with exact blocker `0x2B235C1: je`, so the existing c17 write-address provenance classifier can reach and classify all relevant memory write sites.

Do not inspect/mutate producer `0x351EB00`. Do not implement movement.

## Exact local CFG requirements
Use authenticated complete decode/instruction boundaries. Enumerate local direct Jcc/JMP targets only inside the exact callee. Consider both Jcc successors unless an independently authenticated runtime-path restriction exists.

Do not blindly raise c17's generic `branch_cap=4`. Decode/count the actual local branch sites in this exact 137-byte function and derive a small explicit cap from that count plus at most a tiny safety margin. Emit the actual counts.

No arbitrary pointer chasing, indirect transfer expansion, nested-call expansion, recursion, cross-function traversal or open-ended fixpoint. Conflicting provenance at a merge becomes unknown/refuse. Report any real backedge/indirect/nested-call/unsupported effect exactly.

Emit:
```text
MOVEMENT_CALLEE_CFG_CONTRACT
target_rva=0x2b23560
target_end=0x2b235e9
local_branch_sites=
blocks_total=
edges_total=
blocks_visited=
edges_visited=
paths_examined=
backedges_seen=
indirect_transfers_seen=
nested_calls_seen=
first_unresolved_rva=
first_unresolved_instruction=
reason=
```

## Reuse c17 alias analysis
For every reachable memory write, reuse the existing c17 address provenance model and classify as:
```text
callee_private_stack
caller_shadow_or_return_area
source_span_exact
source_span_possible_alias
hero_transform_exact
hero_transform_possible_alias
rip_relative_global
known_disjoint_object
unknown_pointer
```

Call-time RCX/RDX are Transform-derived. Keep writer-entry RDX source span distinct. R8 is constant bits/non-pointer. R9 remains unknown unless independently proven.

Emit compact metadata per unique reachable write: RVA, width, address class/root/offset, source alias, Transform alias. Do not publish complete private callee bytes.

`source_span_no_write_proven=1` and `transform_no_write_proven=1` require every reachable write to be disjoint from the respective protected object. Any exact alias or unknown address is a blocker.

## Resume writer proof only after alias PASS
Only after:
```text
all_relevant_write_aliases_resolved=1
source_span_no_write_proven=1
transform_no_write_proven=1
```
may writer proof resume from `0x2CFC92C` to exact stores `0x2CFC936/93C/942/945`.

Update CALL_CONTRACT, CALLEE_CFG_CONTRACT, CALLEE_ALIAS_CONTRACT, RDX_BRANCH_CONTRACT, XYZ_PROOF and W_PROOF. Report any later exact writer blocker and stop. W remains independent.

## Runtime observation
Prefer static exact-CFG/address provenance. Only if one exact pointer-root ambiguity remains, add the smallest bounded read-only observation for max8 eligible invocations/session. Do not use before/after equality to rule out transient writes. No PAGE_GUARD, no game-call invocation, no generic tracing.

## Safety boundary
Even a complete writer certificate leaves:
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```
unchanged.

Preserve c7-c17 lifecycle/orientation/stale/hard-kill/secondary/publication-refresh/DR-watch/direct-call/entry-pairing/accounting/branch/call-target/callee-identity/logging/cleanup protections. Preserve `log_wallsprint\WallSprint_YYYYMMDD-HHMMSS.log` with no shared-logs fallback.

## Prohibited in c18
No Actor/Transform Position write, RDX/source/stack mutation, input rewrite, custom movement/velocity/gravity, adhesion correction, upstream producer traversal, camera/animation/traversal, PAGE_GUARD/data-page protection mutation, guessed native game calls, raw log publication, or complete private body distribution.

## Tests
Cover exact local CFG with >4 branches, both-successor reconvergence, function-derived cap, conflicting-root merge refusal, Transform-derived RCX/RDX writes, source-span distinction, private-stack/global safe writes, source/Transform exact aliases, unknown R9 write blocker, local backedge/indirect/nested-call refusal, alias-safe writer continuation, all c17 regressions and logging behavior.

## Human test
If needed after cloud validation:
```text
A/cardinal ~4 s -> fully end WallRun -> move/fast-travel -> E/diagonal ~4 s -> send newest complete file from log_wallsprint
```
No special input. Video only for visible anomaly.

## PASS
A. Exact callee CFG bounded, all reachable write aliases resolved, source/Transform no-write proven, writer proof advances or reports a later exact blocker.

B. One narrower exact callee/write/CFG blocker is reported with all safety gates preserved.

Do not implement movement automatically.

## Required outputs
```text
analysis/V01C17_RUNTIME_RESULT.md
analysis/V01C18_CALLEE_BRANCH_ALIAS.md
analysis/V01C18_GATE.md
analysis/RDX_XYZ_SOURCE_CONTRACT.md
README_TEST.md
docs/PROJECT_STATE.md
docs/CODEX_RESULT.md
```
Package c18 diagnostic release/source per existing rules.