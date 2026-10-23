# Next task — WallSprint v0.1c18 Callee Branch-Aware Alias Contract

## Base
```text
source branch: codex/v01c17-callee-alias-contract
source implementation: d1867bb2de42079a46ceb547f8fccc4d477ae18d
source delivery head: 4f7d1590af744f449d717b8ede30400de0cef820
next branch: codex/v01c18-callee-branch-alias
version target: v0.1c18
```

Read first:
- `AGENTS.md`
- `docs/DEV_RULES.md`
- `docs/PROJECT_STATE.md`
- `docs/LOGGING_CONVENTION.md`
- `analysis/V01C17_RUNTIME_RESULT.md`
- `analysis/V01C17_CALLEE_ALIAS_CONTRACT.md`
- `analysis/RDX_XYZ_SOURCE_CONTRACT.md`
- c17 `docs/CODEX_RESULT.md`

## Accepted c17 actual-game result
C17 is **Diagnostic PASS / narrower exact callee CFG blocker**.

Regression/logging gate:
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
Both sessions finalized 8 true entry snapshots / 8 complete invocation maps with XYZ mask `0x7`, source W=0, destination W=1.

Dedicated logging is actual-game verified:
```text
game-root\log_wallsprint\WallSprint_YYYYMMDD-HHMMSS.log
shared_logs_fallback=0
```
Preserve this behavior unchanged.

C17 proved the writer call argument contract:
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

Authenticated callee remains:
```text
target=0x2B23560
boundary=0x2B23560..0x2B235E9
hash=2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d
```

Exact c17 blocker:
```text
MOVEMENT_CALLEE_ALIAS_CONTRACT
memory_writes_seen=0
source_span_no_write_proven=0
transform_no_write_proven=0
all_relevant_write_aliases_resolved=0
next_barrier_rva=0x2B235C1
next_barrier_instruction=je
next_barrier_effect=indirect_or_branch_cap
reason=callee_indirect_transfer_or_branch_cap_refused
```
`memory_writes_seen=0` means analysis stopped before relevant write classification; it is not proof of a write-free callee.

## Objective
Resolve **only the local control flow inside authenticated 137-byte callee `0x2B23560..0x2B235E9`, beginning with exact blocker `0x2B235C1: je`,** so the existing c17 write-address provenance classifier can reach and classify all relevant memory write sites.

Do not inspect/mutate upstream producer `0x351EB00`. Do not implement movement.

## 1. Build a bounded exact local CFG for this callee
Use the already authenticated complete decode and instruction boundaries. Enumerate local direct conditional/unconditional branches whose targets remain inside `0x2B23560..0x2B235E9`.

Requirements:
- both Jcc successors must be considered unless an independently authenticated runtime-path restriction exists;
- no arbitrary pointer chasing;
- no indirect transfer expansion;
- no nested-call expansion;
- no recursion;
- no cross-function traversal;
- no backedge/loop guessing: if a real backedge exists, report it exactly unless a finite bounded proof can establish safe termination;
- merges must conservatively combine address provenance; conflicting roots become unknown, not guessed;
- preserve exact callee boundary/hash/current-code gates.

The previous generic `branch_cap=4` must not simply be raised blindly. First decode/count the actual local branch sites and derive a small explicit cap for this exact 137-byte function (for example exact_count plus a tiny safety margin). Emit the actual count and visited blocks/edges.

Suggested summary:
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

## 2. Reuse c17 address-provenance classification on every reachable write
Do not replace the c17 alias model. Feed each reachable callee state through it and classify every reachable memory write as one of:
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

Call-time RCX and RDX are both Transform-derived per c17 actual-game proof. Keep writer-entry RDX source span distinct. R8 is constant bits/non-pointer. R9 remains unknown unless c18 independently proves more.

For every unique write site, emit compact metadata only: RVA, width, address class/root/offset, and whether it may alias source or Transform. Do not publish full private function bytes.

## 3. Preserve exact protected-object requirements
`source_span_no_write_proven=1` only if every reachable write is proven disjoint from `[entryRDX, entryRDX+0x10)`.

`transform_no_write_proven=1` only if every reachable write is proven disjoint from the authenticated Hero Transform protected range needed by the later Position stores.

Because call-time RCX/RDX are Transform-derived, any write through either argument chain must be classified precisely; do not assume it is harmless.

If one exact write aliases Transform/source, or an address root becomes unknown, report that site as the exact blocker and stop writer causality advancement.

## 4. Resume writer proof only after alias PASS
Only if:
```text
all_relevant_write_aliases_resolved=1
source_span_no_write_proven=1
transform_no_write_proven=1
```
may the existing writer proof resume from return `0x2CFC92C` toward exact stores:
```text
0x2CFC936 -> +0x30
0x2CFC93C -> +0x34
0x2CFC942 -> +0x38
0x2CFC945 -> +0x3C
```

Update:
```text
MOVEMENT_WRITER_CALL_CONTRACT
MOVEMENT_CALLEE_CFG_CONTRACT
MOVEMENT_CALLEE_ALIAS_CONTRACT
MOVEMENT_RDX_BRANCH_CONTRACT
MOVEMENT_RDX_XYZ_PROOF
MOVEMENT_POSITION_W_PROOF
```

If a strictly later writer-local instruction becomes the blocker, report its exact RVA/mnemonic/effect and stop. W stays independent.

## 5. Runtime observation only if required for an exact pointer-root ambiguity
Prefer static exact-CFG/address provenance. If one specific call-time pointer relation remains impossible to classify, add only the smallest bounded read-only observation for max 8 eligible invocations/session. No before/after equality may be used to exclude transient writes. No PAGE_GUARD, no callee invocation by the mod, and no generic tracing.

## 6. Safety boundary
Even a complete writer certificate must leave:
```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```
unchanged.

Producer `0x351EB00` remains deferred to a separate later task.

Preserve c7-c17 lifecycle/orientation/stale/hard-kill/secondary/publication-refresh/DR-watch/direct-call/entry-pairing/accounting/branch/call-target/callee-identity/logging/cleanup protections.

Preserve dedicated logging exactly:
```text
log_wallsprint\WallSprint_YYYYMMDD-HHMMSS.log
```
No silent fallback to shared `logs`.

## Prohibited in c18
No Actor/Transform Position write, RDX/source/stack mutation, input rewrite, custom movement/velocity/gravity, adhesion correction, upstream producer traversal, camera/animation/traversal, PAGE_GUARD/data-page protection mutation, guessed native game calls, raw log publication, or full private body distribution.

## Tests
Add owned/offline coverage for:
- exact local Jcc graph with >4 branches;
- both successors and reconvergence;
- exact branch-count-derived cap;
- conflicting root merge -> unknown/refuse;
- Transform-derived RCX/RDX write classification;
- source span kept distinct from call-time RDX;
- private stack/global safe writes;
- exact source/Transform alias write blockers;
- unknown R9-derived write blocker;
- local backedge/indirect/nested-call exact refusal;
- alias-safe callee followed by successful writer continuation in owned fixture;
- all c17 regressions and logging behavior.

## Human test
If actual-game evidence is required after cloud validation:
```text
A/cardinal ~4 s -> fully end WallRun -> move/fast-travel -> E/diagonal ~4 s -> send newest complete file from log_wallsprint
```
No special input. Video only for visible anomaly.

## PASS
A. Exact callee CFG is bounded and all reachable write aliases resolve; source/Transform no-write is proven; writer proof advances or reports a later exact blocker.

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