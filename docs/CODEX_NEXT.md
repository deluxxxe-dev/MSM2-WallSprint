# Next task — WallSprint v0.1c15 Writer Branch-Aware RDX Contract

## Base
```text
source branch: codex/v01c14-rdx-source-contract
next branch: codex/v01c15-writer-branch-contract
version target: v0.1c15
```

Read `AGENTS.md`, `docs/DEV_RULES.md`, `docs/PROJECT_STATE.md`, `analysis/V01C14_RUNTIME_RESULT.md`, c14 `docs/CODEX_RESULT.md`, and the c14 RDX contract analyses first.

## Accepted c14 runtime result
A/session1 4453 ms and E/session2 4172 ms both retained Surface Up through verified Deinit, hard kill0, stale0 and Position writes0. Entry-hook and Position-watch cleanup restored normally.

Both sessions now correctly report:
```text
source_capture_ready=1
true_entry_snapshot=1
true_entry_snapshots=8
complete_invocation_maps=8
```

Actual entry/store records again reproduce:
```text
RDX == entry RSP+0x28
[RDX] XYZ == exact executed Transform Position XYZ store bits
entry_store_xyz_match_mask=0x7
source W observed 0
destination W observed 1
```

But c14's independent decoded contract stops here:
```text
writer_complete_decode=1
entry_store_prefix=0
xyz_RDX_0_4_8_paths=0
W_kind=unknown
W_classified=0
first_barrier_rva=0x2CFC91F
first_barrier_instruction=jne
exact_next_producer_rva=0x351EB00
writable_source_proven=0
reason=exact_position_store_sites_mismatch_or_prefix_incomplete
```

Therefore RDX equality is still correlation rather than proven writer-side causality. W source is also unclassified. Do not chase or mutate `0x351EB00` yet.

## Objective
Add the smallest bounded branch-aware proof needed inside authenticated writer `0x2CFC900..0x2CFC9B7` to prove or reject the actual dataflow to exact Position stores:
```text
0x2CFC936 -> +0x30
0x2CFC93C -> +0x34
0x2CFC942 -> +0x38
0x2CFC945 -> +0x3C
```

Prove/reject:
```text
entry [RDX+0] -> +0x30
entry [RDX+4] -> +0x34
entry [RDX+8] -> +0x38
```
starting with exact conditional barrier `0x2CFC91F: jne`.

## Scope rules
- exact build/SHA/writer boundary/current code identity mandatory;
- branch analysis only inside this exact <=183-byte writer;
- explicit strict caps on blocks/edges/branches; runtime observer reports four branches;
- no general whole-engine CFG tracer;
- no recursion/indirect expansion/open-ended fixpoint;
- reject unsupported instructions, unknown memory effects, loops/backedges or uncertain merges rather than guessing;
- a component is proven only if all relevant paths reaching its exact store preserve the authenticated RDX origin, or an independently proven runtime path restriction justifies narrowing;
- if another exact barrier appears, report its RVA/mnemonic and stop.

Emit a bounded contract summary such as:
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

Only set `MOVEMENT_RDX_XYZ_PROOF xyz_causality_proven=1` when the branch-aware proof reaches and authenticates all three exact XYZ stores.

## W
Once exact store `0x2CFC945` is reachable, classify its decoded source conservatively:
```text
constant_one | independent_register | independent_memory | computed | unknown
```
Observed value 1.0 alone is not causal proof and must not be promoted to homogeneous-coordinate semantics.

## Telemetry fix
C14's final `MOVEMENT_LINEAGE_SUMMARY` is correct, but `MOVEMENT_ENTRY_CAPTURE_SUMMARY` is emitted before capture activation and reports zeros. Re-emit/move this summary at diagnostic stop/finalization using the already stored bounded session counts. Do not change capture/watch timing or caps to obtain nonzero output.

## Preserve all protections
Preserve c7 recovery, c8 first hard-kill cause, c9 secondary admission, c10 publication refresh, c11 Position watch/debug restoration, c12 direct call proof, c13 entry hook/pairing, c14 final accounting and accepted orientation behavior.

Authoritative gate remains:
```text
writable_source_proven=0
Position/source/input/movement writes=0
```

## Prohibited in c15
No Actor/Transform Position write, RDX/stack mutation, Path A Position mutation, input rewrite, custom velocity/gravity, active adhesion, producer hook at `0x351EB00`, camera/animation/traversal work, PAGE_GUARD/data-page protection mutation, guessed native ABI/game calls, raw user-log publication or full private game-body distribution.

## PASS
A diagnostic PASS is either:

A. Exact writer contract proven: all relevant store paths authenticated, XYZ causality proven and W independently classified; then a later separate task may investigate `0x351EB00` ownership.

B. A narrower exact local blocker is emitted after branch-aware analysis, with all safety gates preserved.

Do not implement movement automatically after either outcome.

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
