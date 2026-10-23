# Next task — WallSprint v0.1c13 Writer Entry Snapshot / Invocation Correlation

## Base
Use the completed c12 delivery exactly:

```text
base branch: codex/v01c12-movement-source-lineage
implementation commit: 4c84d634df4d629b237d4cc311d5e5cd6de4398c
published head: c966d233787504dda88f65d1825c5c1c408a2d67
version: v0.1c12 / package 0.1.30
```

Work on the pre-created branch:

```text
codex/v01c13-writer-entry-snapshot
```

Read first:
- `AGENTS.md`
- `docs/DEV_RULES.md`
- `docs/PROJECT_STATE.md`
- `analysis/V01C12_RUNTIME_RESULT.md`
- `analysis/MOVEMENT_WRITER_2CFC900.md`
- `analysis/MOVEMENT_CALLER_351EEB0.md`
- c12 `docs/CODEX_RESULT.md`

## Authoritative c12 real result
The actual A/E run is a **diagnostic PASS / exact missing edge**, not a writable-source admission.

Both surfaces preserved orientation/lifecycle through verified Deinit with Position writes by the mod = 0.

A/cardinal:
```text
wallrun_duration_ms=4266
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
hard_kill_calls=0
```

E/diagonal:
```text
N398≈(-0.7071068,0,+0.7071068)
wallrun_duration_ms=4297
source_active_until_deinit=1
verified_deinit=1
retention_pass=1
last_actor_up_dot_surface≈1
hard_kill_calls=0
```

E contained one bounded 72 ms stale suspend/resume caused by a ~172 ms native Update gap; it resumed with coherent identity and is not a blocker.

The exact direct callsite is real and authenticated:

```text
return_rva=0x351EEB0
call_rva=0x351EEAB
target_rva=0x2CFC900
direct_call_verified=1
```

The real writer remains:

```text
function=0x2CFC900..0x2CFC9B7
stores=0x2CFC936 / 0x2CFC93C / 0x2CFC942 / 0x2CFC945
Hero Transform destinations=+0x30 / +0x34 / +0x38 / +0x3C
```

But both A and E ended with the same c12 blocker:

```text
first_unsupported_offset=0xA
complete_writer_CFG=0
source_prefix_proven=0
source_capture_ready=0
eligible_hits=0
attempts=0
true_entry_snapshot=0
caller2_direct_relation_verified=0
writable_source_proven=0
reason=entry_snapshot_typed_caller_W_and_exclusive_effects_unproven
```

Do not infer typed velocity/displacement/direction/root-motion semantics from the observed arithmetic or Position deltas.

## Objective
Resolve the smallest missing evidence edge: **obtain a true entry-time snapshot for verified Hero-targeted invocations of writer `0x2CFC900`, and correlate that exact invocation with the existing Position-store hardware-watch events.**

This task remains diagnostic/read-only with respect to movement and Position.

The preferred result is an evidence-quality answer to:

```text
At writer 0x2CFC900 entry, which register / pointer / stack value actually supplies the values later written to Hero Transform +0x30/+0x34/+0x38/+0x3C?
Is that entry source stable across A/cardinal and E/diagonal WallRun?
What is the exact next upstream producer edge if it is not yet writable?
```

## 1. Preserve c12 behavior exactly
Do not regress:
- exact exe version/SHA build gate;
- c7 stale suspend/resume;
- c8 first hard-kill attribution;
- c9 secondary-thread Path B admission;
- c10 bounded publication refresh;
- c11 Position hardware watch / cleanup / DR preservation;
- c12 direct callsite proof and read-only lineage diagnostics;
- orientation Path A/B behavior;
- `Position_writes_by_mod=0`.

Do not alter visual wall standoff/root lean in c13.

## 2. Add a bounded true-entry diagnostic for 0x2CFC900
The c12 blocker explicitly contains `true_entry_snapshot=0`. Add the smallest exact-build diagnostic capable of observing the writer at its actual entry **before its body mutates source registers**.

Preferred approach: an exact-build, verified-boundary diagnostic detour/raw thunk at `0x2CFC900`, only after:
- target build/SHA accepted;
- runtime function begin exactly `0x2CFC900`;
- expected current runtime hash/boundary accepted from c12 startup proof;
- active verified WallRun session exists;
- current target is the verified active Hero Transform.

The diagnostic must preserve the real function's ABI/flags/register state exactly and continue through the original/trampoline without changing arguments or return value. Use existing raw-thunk/MinHook patterns where appropriate; do not invent a typed game signature.

If a detour cannot be made provably transparent under the existing project rules, fail closed and implement the closest equivalent bounded entry observation without enabling movement writes.

## 3. Capture only bounded entry state
For eligible Hero-targeted invocations, record a compact snapshot sufficient to recover caller-supplied inputs:

```text
MOVEMENT_WRITER_ENTRY_SNAPSHOT
session=
invocation_id=
thread_id=
elapsed=
writer_rva=0x2cfc900
target_transform=
return_rva=
caller2_observed_rva=
rcx=
rdx=
r8=
r9=
rsp=
selected_gpr_state=...
selected_xmm_state=...
stack_arg_snapshot=bounded
entry_target_verified=1
code_integrity_verified=1
```

Capture only what is needed for source attribution. Hard-cap records, e.g. <=32 eligible invocations/session and a small bounded stack-argument window. Do not dump arbitrary process memory or full functions.

Capture XMM/GPR values in a raw, semantics-neutral way. Do not call a value velocity/delta/direction until independently proven.

## 4. Pair entry snapshots with actual Position stores
Correlate each eligible entry snapshot to the existing `POSITION_WRITE_EVENT` records from writer `0x2CFC900` on the same:
- WallRun session;
- thread;
- Hero Transform target;
- writer invocation.

Use an explicit invocation token/counter so events from unrelated generic Transform calls cannot be mixed.

Emit:

```text
MOVEMENT_WRITER_INVOCATION_MAP
session=
invocation_id=
thread_id=
target_transform=
entry_return_rva=
store_30_seen=0|1
store_34_seen=0|1
store_38_seen=0|1
store_3c_seen=0|1
position_before=(...)
position_after=(...)
entry_source_match_mask=
entry_pointer_candidate_mask=
source_stable=0|1
surface_normal_component=
surface_forward_component=
surface_right_component=
reason=
```

Do not require all generic calls to match; only the verified active Hero target matters.

## 5. Identify entry-source candidates conservatively
For each entry snapshot, test bounded candidate origins from:
- direct GPR values;
- XMM lanes;
- readable 16-byte structures pointed to by argument GPRs;
- bounded caller stack arguments / saved values.

Use guarded reads only. No protection changes. No broad pointer chasing.

Compare candidate values against:
- Hero Position before this invocation;
- exact values written by the four Position stores;
- net invocation delta;
- SurfaceFrame decomposition.

Emit evidence such as:

```text
MOVEMENT_ENTRY_SOURCE_CANDIDATE
session=
invocation_id=
candidate_id=
origin=GPR|XMM|pointer|stack
origin_register_or_offset=
read_valid=
value4=(...)
relationship=current_position|final_position|delta_like|partial|none
exact_store_match_mask=
stable_across_invocations=0|1
stable_across_surfaces=0|1
typed_semantics=unknown
writable_source_proven=0
reason=
```

`delta_like` is a correlation label only. It must not be promoted to engine displacement/velocity.

## 6. Reconcile the caller relation
c12 proved exact direct `0x351EEAB -> 0x2CFC900`, but the observed second return `0x351A020` did not pass the c12 *direct* caller2 relation check (`direct_target_mismatch`).

Use the true entry snapshot's stack/return chain only to classify the real relation conservatively:
- direct caller;
- indirect caller;
- wrapper/thunk;
- unrelated/unresolved.

Do not require caller2 to be a direct E8 if real runtime evidence shows an indirect/wrapper path.

Emit:

```text
MOVEMENT_CALLER_CHAIN_RUNTIME
session=
invocation_id=
return_rva=0x351eeb0
caller2_observed_rva=
relation=direct|indirect|wrapper|unresolved
static_direct_verified=0|1
runtime_stack_verified=0|1
reason=
```

## 7. Do not broaden the static analyzer merely to force success
You may add support for a specific c12 unsupported instruction only if its semantics are independently complete and useful, but the primary c13 purpose is the true entry snapshot.

Do not turn the bounded analyzer into an open-ended whole-engine CFG project in this task.

`writable_source_proven=1` is allowed only if the actual-game evidence independently proves:
- exact source storage/range;
- source values before writer mutation;
- exact causal feed to intended Position components including W behavior;
- active Hero/session/thread/owner lifetime;
- stable producer provenance;
- no immediate conflicting/downstream overwrite relevant to the proposed source;
- exclusive intended Hero translation effect.

Otherwise keep it 0 and state the next exact producer edge.

## 8. Safety constraints
No:
- direct Actor/Transform Position write;
- modification of `0x2CFC900` arguments/source values;
- Path A Position write;
- input/controller hook or rewrite;
- custom velocity/gravity;
- active adhesion;
- camera/animation/wall jump/corners/ceilings/parkour/swing/combat;
- PAGE_GUARD/protection mutation;
- unbounded process-wide tracing.

Any entry detour is diagnostic-only and must be byte/build scoped, transparent, removable on failure, and covered by regression tests.

## 9. Tests
Add deterministic tests for:
- exact writer boundary/hash gate;
- transparent entry thunk preserving integer args, XMM args, flags/nonvolatiles and return value;
- Hero target filter vs unrelated Transform calls;
- invocation token pairing with four stores;
- nested/reentrant or concurrent writer calls on multiple threads;
- wrong target/session/generation/stale/Deinit cases;
- bounded stack snapshot and unreadable/guard/noaccess pointer refusal;
- candidate exact-match / partial-match / false-positive rejection;
- caller2 direct vs indirect/wrapper classification;
- c12 orientation and Position-watch regression unchanged;
- exact cleanup/unhook on shutdown/failure.

Do not package private full game-function bodies.

## Human test
If c13 implementation passes its offline/native suite, package a read-only runtime build.

Preferred human sequence:
```text
1. A/cardinal planar WallRun ~4 s
2. E/diagonal planar WallRun ~4 s
```

Ordinary input; no special stick pattern. No visible movement change expected.

Return full private logs. Video remains optional diagnostic backup; logs are authoritative for this task.

## PASS criteria
Diagnostic c13 PASS:
```text
c12 orientation remains stable to verified Deinit
Position_writes_by_mod=0
entry diagnostic installs only on the exact accepted build/writer
actual Hero-targeted 0x2CFC900 entry snapshots captured
entry snapshots are paired to real Position stores by invocation
at least one candidate source relation is identified, OR the exact next producer edge is established
cleanup/unhook/debug state restored
no hard kill/crash/hitch introduced by diagnostic
```

Do not automatically implement surface-relative movement after c13. First adjudicate the real A/E log and create a separate minimal proof/write task only if the source contract is actually admitted.

## Required outputs
Update/create:
```text
analysis/V01C12_RUNTIME_RESULT.md
analysis/MOVEMENT_WRITER_ENTRY_SNAPSHOT.md
analysis/MOVEMENT_ENTRY_SOURCE_CANDIDATES.md
analysis/V01C13_GATE.md
README_TEST.md
docs/PROJECT_STATE.md
docs/CODEX_RESULT.md
```

Package:
```text
WallSprint_v0.1c13_writer_entry_snapshot.zip
```
