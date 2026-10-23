# Next task — WallSprint v0.1c14 RDX Stack-Local Source Contract

## Base
```text
source branch: codex/v01c13-writer-entry-snapshot
next branch: codex/v01c14-rdx-source-contract
version target: v0.1c14
```

Read first:
- `AGENTS.md`
- `docs/DEV_RULES.md`
- `docs/PROJECT_STATE.md`
- `analysis/V01C13_RUNTIME_RESULT.md`
- c13 `docs/CODEX_RESULT.md`
- `analysis/MOVEMENT_WRITER_ENTRY_SNAPSHOT.md`
- `analysis/MOVEMENT_ENTRY_SOURCE_CANDIDATES.md`

## Human c13 evidence
Combined A -> fast travel -> E log is accepted. Fast travel did not impair session separation.

Both sessions preserved orientation through verified Deinit with hard kill 0 and Position writes 0.

Actual game captured 16 true writer-entry snapshots, 8 per surface. Every sampled invocation was linked to all four `0x2CFC900` Position stores.

The repeated source pattern is:
```text
entry RDX = entry RSP + 0x28
candidate_id=20: [RDX] 16-byte guarded read
candidate_id=23: entry stack bytes at RSP+0x28
XYZ exactly equal the executed Transform Position XYZ store bits
source W=0
Transform Position W=1
exact_store_match_mask=0x7
observed_store_mask=0xF
```
This pattern repeats for all 16 A/E invocations. It is strong source correlation, not yet write permission.

Observed runtime chain:
```text
0x351EEAB -> 0x2CFC900
return 0x351EEB0
caller2 observed 0x351A020
runtime stack relation verified
caller2 static relation unresolved
```

## Objective
Authenticate the exact contract of the RDX stack-local XYZ payload and the caller-side producer that constructs it. Determine whether it is final target Position XYZ or another representation. Separately prove how destination W becomes 1. Keep movement diagnostic/read-only.

## Required work

### 1. Repair c13 aggregate telemetry
Actual records have `true_entry_snapshot=1` and capture active, but final `MOVEMENT_LINEAGE_SUMMARY` incorrectly reports `true_entry_snapshot=0/source_capture_ready=0`.

Fix summary accounting without weakening any gate.

Also add XYZ-aware candidate aggregation so a persistent `mask=0x7` source is not reported as unstable merely because W is independently generated.

Emit bounded summaries such as:
```text
MOVEMENT_ENTRY_CAPTURE_SUMMARY
session=
true_entry_snapshots=
complete_invocation_maps=
xyz_source_candidate=
xyz_match_count=
xyz_match_mask=
source_w_pattern=
destination_w_pattern=
```

### 2. Prove writer XYZ dataflow from RDX
Using the authenticated current writer body and c13 true-entry records, prove or reject the exact dataflow:
```text
[RDX+0] -> Transform +0x30
[RDX+4] -> Transform +0x34
[RDX+8] -> Transform +0x38
```
Do not infer from equality alone. Follow the actual decoded writer instructions, including the previously unsupported edge around writer+0xA.

Emit:
```text
MOVEMENT_RDX_XYZ_PROOF
writer_rva=0x2cfc900
source_register=RDX
source_offsets=0,4,8
position_offsets=0x30,0x34,0x38
component_paths=
xyz_causality_proven=0|1
reason=
```

Do not log or publish the full private writer body.

### 3. Prove W behavior separately
Determine the executed source for Transform `+0x3C` and why actual c13 samples show source candidate W=0 while destination W=1.

Classify conservatively:
```text
constant_one
independent_register
independent_memory
computed
unknown
```

Emit:
```text
MOVEMENT_POSITION_W_PROOF
store_rva=0x2cfc945
source_kind=
source_value_bits=
destination_value_bits=
causality_proven=
reason=
```

XYZ proof must not fail merely because W is a homogeneous-coordinate constant, if that is independently established.

### 4. Authenticate caller-local storage identity
C13 observed `RDX == entry RSP+0x28` on every sampled call. Prove the relationship at the exact callsite and determine whether this is a caller-owned stack local, argument home/temporary storage, or another bounded stack object.

Require exact callsite/current-code identity and per-call stack bounds.

Emit:
```text
MOVEMENT_RDX_STORAGE_PROOF
call_rva=0x351eeab
rdx_relation=
stack_object_begin=
stack_object_size=
lifetime_scope=
caller_owned=
reason=
```

### 5. Trace who constructs XYZ before the call
Trace backward only as far as necessary to identify the writes producing the RDX/RSP+0x28 XYZ bytes before `0x351EEAB`.

The observed second return `0x351A020` is a runtime investigation anchor, not an assumed direct caller relation. Resolve whether it reaches the tiny caller through direct call, wrapper, tail jump, indirect dispatch, or another exact mechanism.

Prefer a bounded static/dataflow proof. If actual runtime evidence is required, add the smallest read-only producer diagnostic around the exact stack-local writes; no broad function tracing.

Emit:
```text
MOVEMENT_RDX_PRODUCER_PROOF
storage=entry_rsp_plus_0x28
producer_rva=
producer_function_begin=
producer_relation_to_0x351A020=
construction_kind=final_target_position|position_plus_delta|other|unknown
uses_current_position=
uses_dt=
uses_direction_like_vector=
uses_speed_like_scalar=
reason=
```

Do not call it velocity/displacement/root motion/direction from arithmetic resemblance alone.

### 6. Lifetime / downstream / exclusive-effect gate
Before any future write permission, establish:
- source bytes are caller-owned for the relevant invocation and not shared with unrelated systems;
- changing XYZ at that source would feed only the intended Hero translation path for the admitted invocation;
- no later writer systematically replaces the intended result before it can affect locomotion;
- source lifetime covers the proposed future mutation point;
- session/generation/Hero/Transform/thread/freshness/code-integrity gates can be preserved.

If any edge remains unknown, keep `writable_source_proven=0` and report the exact missing edge.

### 7. Preserve all existing protections
Do not change the accepted c10/c11 orientation ownership logic, c7 recovery, c8 kill attribution, c9 secondary admission, c10 publication refresh, or c13 entry-hook cleanup semantics except where needed to fix diagnostic accounting.

## Safety / prohibited in c14
No:
- Actor/Transform Position write;
- RDX source mutation;
- Path A Position mutation;
- controller/keyboard input hook or rewrite;
- custom movement/velocity/gravity;
- active adhesion correction;
- animation/camera/traversal features;
- PAGE_GUARD or data-page protection mutation;
- guessed native ABI/game calls;
- raw user logs or full private game bodies in GitHub/package.

`Position_writes_by_mod=0` remains mandatory.

## Tests
Add focused owned/offline tests for:
- XYZ source with independent W constant;
- RDX==RSP+0x28 stack-local lifetime;
- direct/wrapper/tail/indirect producer relations;
- wrong caller/target/session/generation/thread/stale/code-integrity negatives;
- nested/concurrent invocation separation;
- telemetry summary accurately reflecting captured snapshots and XYZ-only stable matches;
- unchanged A/B orientation and cleanup regressions.

## Human test
If c14 still requires runtime evidence, keep the low-friction standard:
```text
1. A/cardinal ~4 s
2. fully end WallRun
3. move or fast travel to E
4. E/diagonal ~4 s
5. send one complete WallSprint log
```
Video only if the user sees a visible anomaly or if a later targeted reproduction is specifically requested.

## PASS
A diagnostic PASS should at minimum establish:
- actual XYZ causality from the RDX stack-local source into the three Position XYZ stores;
- W source/behavior classified independently;
- exact stack-local storage identity/lifetime narrowed;
- producer relation narrowed to a concrete bounded edge;
- c13 telemetry inconsistency repaired;
- orientation/cleanup/Position-write safety unchanged.

Only set `writable_source_proven=1` if producer provenance, source lifetime, XYZ/W semantics, downstream conflicts and exclusive intended translation effect are all independently proven. Even then, do not silently implement movement: create a separate minimal rewrite task first.

## Outputs
Update/add:
```text
analysis/V01C13_RUNTIME_RESULT.md
analysis/RDX_XYZ_SOURCE_CONTRACT.md
analysis/RDX_CALLER_PRODUCER.md
analysis/V01C14_GATE.md
README_TEST.md
docs/PROJECT_STATE.md
docs/CODEX_RESULT.md
```
Package the c14 diagnostic release and source ZIP per existing project rules.
