# Next task — WallSprint v0.1c15 Writer Branch-Aware RDX Contract

## Base
```text
source branch: codex/v01c14-rdx-source-contract
next branch: codex/v01c15-writer-branch-contract
version target: v0.1c15
```

Read first:
- `AGENTS.md`
- `docs/DEV_RULES.md`
- `docs/PROJECT_STATE.md`
- `analysis/V01C14_RUNTIME_RESULT.md`
- `analysis/RDX_XYZ_SOURCE_CONTRACT.md`
- `analysis/RDX_CALLER_PRODUCER.md`
- c14 `docs/CODEX_RESULT.md`

## Human c14 evidence
Actual A/E runtime is accepted as a diagnostic PASS with no movement-write authorization.

Regression gate:
```text
A/session1 wallrun_duration_ms=4453
E/session2 wallrun_duration_ms=4172
both verified Deinit
both retention_pass=1
both hard_kill_calls=0
both Position_writes_by_mod=0
both stale_suspend_count=0
```

The c13 final-summary defect is repaired: each real c14 session reports:
```text
source_capture_ready=1
true_entry_snapshot=1
true_entry_snapshots=8
complete_invocation_maps=8
```

Actual entry/store records still reproduce:
```text
RDX == entry RSP+0x28
[RDX] XYZ == executed Transform Position XYZ store bits
entry_store_xyz_match_mask=0x7
source W observed 0
destination W observed 1
```

But this remains correlation because the new static writer proof stops before the four known stores.

Exact blocker:
```text
MOVEMENT_RDX_STATIC_CONTRACT
writer_complete_decode=1
entry_store_prefix=0
xyz_RDX_0_4_8_paths=0
W_kind=unknown
W_classified=0
first_barrier_rva=0x2CFC91F
first_barrier_instruction=jne
caller_prefix=1
caller_RDX_local=0
caller_allocation_owned=0
caller2_relation=unresolved
exact_next_producer_rva=0x351EB00
writable_source_proven=0
reason=exact_position_store_sites_mismatch_or_prefix_incomplete
```

Per actual invocation:
```text
MOVEMENT_RDX_XYZ_PROOF
component_paths=unresolved
static_prefix_proven=0
xyz_causality_proven=0
entry_store_xyz_match_mask=0x7
```

W is also not decoded/proven because the same writer-prefix proof stops first.

Storage is narrowed at runtime to:
```text
RDX == entry RSP+0x28 == precall RSP+0x20
16-byte TEB-bounded stack object
pointer_stack_bytes_equal=1
caller_owned=0
ownership_exclusive=0
```

The parent around observed second return `0x351A020` has anchored call `0x351A01B` with direct target / exact next producer edge `0x351EB00`, but construction semantics and ownership remain unknown.

## Objective
Resolve **only the writer-side control-flow/dataflow blocker**. Prove or reject the exact RDX XYZ causal path through the authenticated 183-byte writer to the known Position stores, with W classified independently.

Do not chase or mutate `0x351EB00` yet. Do not implement movement.

## 1. Add bounded branch-aware proof for the exact writer
The c14 straight-line contract stops at exact real barrier:
```text
0x2CFC91F: jne
```

Extend the independent c14 `rdx_source_contract` proof narrowly enough to reason about the authenticated writer `0x2CFC900..0x2CFC9B7` and its local conditional control flow.

Requirements:
- exact accepted game build / SHA / writer boundary / full current code identity remain mandatory;
- no general whole-engine CFG tracer;
- bound the proof to this exact <=183-byte function;
- bound blocks/edges/branch count explicitly (the runtime static observer reports four branches in this function);
- no recursion, indirect call expansion, arbitrary interprocedural traversal or open-ended fixpoint;
- reject loops/backedges unless independently finite and explicitly supported;
- reject unknown memory effects or unsupported instructions rather than guessing;
- merge provenance conservatively: a component is proven only if every relevant path reaching its store has the same authenticated RDX origin, or if runtime/path evidence independently restricts the applicable path without weakening identity gates.

If supporting the exact `jne` exposes another unsupported local edge, report the next exact RVA/mnemonic and stop. Do not silently broaden scope.

## 2. Prove the known store sites, not nearby inferred stores
The exact target stores remain:
```text
0x2CFC936 -> Transform +0x30
0x2CFC93C -> Transform +0x34
0x2CFC942 -> Transform +0x38
0x2CFC945 -> Transform +0x3C
```

Require these exact RVAs and destination offsets under the authenticated function body.

For XYZ, prove/reject:
```text
entry [RDX+0] -> +0x30
entry [RDX+4] -> +0x34
entry [RDX+8] -> +0x38
```

The c13/c14 `mask=0x7` runtime equality is corroborating evidence only, never a substitute for the decoded path.

Emit a compact path result, e.g.:
```text
MOVEMENT_RDX_BRANCH_CONTRACT
writer_rva=0x2cfc900
blocks_visited=
edges_visited=
branches_seen=
first_branch_rva=0x2cfc91f
all_relevant_store_paths_proven=0|1
x_path=
y_path=
z_path=
next_barrier_rva=
next_barrier_instruction=
reason=
```

Update `MOVEMENT_RDX_XYZ_PROOF` so `xyz_causality_proven=1` only when the branch-aware proof actually reaches and authenticates the three exact stores.

## 3. Classify W independently once its exact store is reachable
For exact store `0x2CFC945`, classify the executed source as one of:
```text
constant_one
independent_register
independent_memory
computed
unknown
```

Require decoded source provenance. Observed destination bits `0x3f800000` alone are not enough.

Keep:
```text
homogeneous_coordinate_semantics=unknown
```
unless independently proven; a numeric 1.0 is not by itself an engine semantic label.

## 4. Preserve producer/storage boundary
Do not add a new producer hook and do not follow `0x351EB00` beyond already recorded bounded metadata in this task.

Keep these historical c14 facts visible but unpromoted:
```text
exact_next_producer_rva=0x351EB00
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
writable_source_proven=0
```

Only after writer XYZ/W causality is proven should the next task decide whether to inspect the producer edge.

## 5. Fix capture-summary timing without changing capture policy
C14 final `MOVEMENT_LINEAGE_SUMMARY` is correct, but `MOVEMENT_ENTRY_CAPTURE_SUMMARY` is emitted before the entry window activates and therefore reports zeros.

Re-emit/move the bounded summary at diagnostic stop/finalization so the final summary reflects the same actual session counts already stored by the entry aggregator.

Do not change the 8-entry/session cap, watch timing, entry-hook install policy, hardware watch policy, or freshness gates just to make the summary nonzero.

## 6. Preserve all existing protections
Do not regress:
- exact exe version/SHA fail-closed gate;
- c7 stale suspend/resume;
- c8 immutable first hard-kill cause;
- c9 secondary Path B admission;
- c10 bounded publication refresh;
- c11 Position hardware watch/cleanup/debug-register preservation;
- c12 exact direct callsite proof;
- c13 true entry hook and invocation pairing;
- c14 corrected final entry accounting;
- orientation Path A/B behavior;
- `Position_writes_by_mod=0`.

## 7. Safety / prohibited in c15
No:
- Actor/Transform Position write;
- RDX/stack source mutation;
- Path A Position mutation;
- controller/keyboard input hook or rewrite;
- custom movement/velocity/gravity;
- active adhesion correction;
- new producer hook at `0x351EB00`;
- camera/animation/traversal features;
- PAGE_GUARD/data-page protection mutation;
- guessed native ABI/game calls;
- raw user log or complete private game-function body in GitHub/package.

`writable_source_proven=0` remains unconditional in c15 unless the task unexpectedly proves the entire producer/lifetime/downstream/exclusive-effect gate too; do not attempt to force that outcome.

## Tests
Add owned/offline tests for:
- exact conditional branch with both successors reconverging on identical RDX provenance;
- one successor mutating X/Y/Z => reject that component;
- one successor bypassing a store => correct relevant-path semantics;
- nested conditional branches within strict block/edge caps;
- loop/backedge refusal;
- unsupported/indirect/control-flow refusal with exact first barrier;
- exact store-RVA mismatch refusal;
- W constant-one vs independent/computed source classification;
- final capture-summary timing/count correctness;
- all c14/c13 orientation/watch/cleanup regressions unchanged.

## Human test
If actual-game evidence is still required after cloud validation, use the same low-friction sequence:
```text
1. A/cardinal ~4 s
2. fully end WallRun
3. move or fast travel to E
4. E/diagonal ~4 s
5. send one complete WallSprint log
```
No special input pattern. Video only for a visible anomaly.

## PASS
C15 diagnostic PASS requires one of:

### A. Writer contract proven
```text
all_relevant_store_paths_proven=1
xyz_causality_proven=1
exact XYZ stores authenticated
W source independently classified/proven
```
Then the next task may investigate `0x351EB00` producer/storage ownership. Still do not implement movement automatically.

### B. Exact next local blocker
If the branch-aware proof still cannot reach the exact stores, report one precise next local writer RVA/mnemonic/edge and keep all permissions at zero. This is still a diagnostic PASS if safety/regressions are preserved and the blocker is narrower than c14.

## Required outputs
Update/create:
```text
analysis/V01C14_RUNTIME_RESULT.md
analysis/RDX_XYZ_SOURCE_CONTRACT.md
analysis/V01C15_WRITER_BRANCH_CONTRACT.md
analysis/V01C15_GATE.md
README_TEST.md
docs/PROJECT_STATE.md
docs/CODEX_RESULT.md
```

Package the c15 diagnostic release/source per existing project rules.
