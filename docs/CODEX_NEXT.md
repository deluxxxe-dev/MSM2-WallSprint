# Next task — WallSprint v0.1c12 Movement Source Lineage / Upstream Translation Ownership

## Implementation base

Use c11 delivery head exactly:

```text
base branch: codex/v01c11-movement-path-ownership
implementation commit: 2129e69fe003e71bf705fb67f723b447cf3e3764
published c11 head: 9593f0230e220770f2b34faf512ca4108c1825cc
version: v0.1c11 / package 0.1.29
```

Pre-created branch:

```text
codex/v01c12-movement-source-lineage
```

Read first:

- `analysis/V01C11_RUNTIME_RESULT.md`
- `analysis/POSITION_WRITER_ATTRIBUTION.md`
- `analysis/PATH_A_POSITION_CAUSALITY.md`
- `analysis/MOVEMENT_SOURCE_OWNERSHIP.md`
- `docs/PROJECT_STATE.md`
- c11 branch `docs/CODEX_RESULT.md`

## Authoritative c11 human result

Human sequence was A/cardinal first, then E/diagonal.

Both sessions preserved orientation through verified Deinit with no Position writes by the mod.

```text
A: wallrun_duration_ms=4125, retention_pass=1, last_actor_up_dot_surface=1, hard_kill_calls=0
E: wallrun_duration_ms=4063, retention_pass=1, last_actor_up_dot_surface≈0.99999994, hard_kill_calls=0
```

Both Position-watch windows:

```text
watch_armed=1
threads_seen=128
threads_installed=126
unique_writers=14
total_write_events=256
cleanup_restored=1
cleanup_pending=0
Position_writes_by_mod=0
```

## New runtime evidence

A real Transform Position writer family was captured:

```text
writer_function_rva=0x2CFC900
store RVAs=0x2CFC936 / 0x2CFC93C / 0x2CFC942 / 0x2CFC945
Transform destinations=+0x30 / +0x34 / +0x38 / +0x3C
```

The caller chain that correlated with actual translation on both A and E was:

```text
caller return RVA=0x351EEB0
caller2 return RVA=0x351A020
consumer writer=0x2CFC900
```

Examples:

- A: the 0x351EEB0 path changed Y/Z while X (surface normal) stayed essentially fixed.
- E: the same path changed X/Y/Z; combined worker finite-difference deltas remained predominantly surface-tangent.

Path A `0x2CFC580` also touches Position and can produce a full-row post-writer match with its caller-local `source+0x30`, but c11 observed downstream Position writes and did not prove exclusive effects. Therefore Path A Position remains denied.

No real runtime candidate reached `writable_source_proven=1`.

## Objective

Trace and authenticate the source lineage that feeds the movement-producing call chain centered on:

```text
0x351EEB0 -> 0x2CFC900
caller2 ≈ 0x351A020
```

Determine whether the upstream payload represents:

- final/target Position;
- displacement;
- velocity;
- desired movement direction;
- root-motion translation;
- another locomotion vector.

The preferred outcome is a safe caller-local/native source contract that a later revision can rewrite in SurfaceFrame coordinates without direct Actor Position spam.

**c12 remains movement-read-only unless an upstream source contract is independently proven. Do not implement player input rewriting yet.**

## Required work

### 1. Preserve c11/c10 behavior

Do not regress:

- exact exe/SHA admission;
- c7 stale recovery;
- c8 first hard-kill attribution;
- c9 secondary Path B admission;
- c10 bounded publication refresh;
- c11 Position watch and movement-delta diagnostics;
- A/B Surface Up orientation;
- Position writes by mod = 0.

### 2. Authenticate writer 0x2CFC900 as a concrete runtime function

Use the accepted build and runtime function metadata to establish the exact function boundary containing `0x2CFC900`.

At minimum record:

```text
MOVEMENT_WRITER_PROOF
writer_rva=0x2cfc900
function_begin=
function_end=
function_size=
complete_decode=
current_code_hash=
position_store_offsets=
position_destination_offsets=
entry_target_relation=
reason=
```

The c11 runtime hash may be used as evidence, but do not treat one runtime hash alone as proof of exclusive semantics. Do not log or commit a full private function body.

### 3. Prove the 0x351EEB0 callsite/caller relation

Identify the exact call instruction whose return address is `0x351EEB0`, prove that it targets `0x2CFC900`, and derive the containing caller function boundary.

Also attribute the observed `caller2≈0x351A020` chain.

Emit bounded evidence:

```text
MOVEMENT_CALLSITE_PROOF
return_rva=0x351eeb0
call_rva=
target_rva=0x2cfc900
direct_call_verified=
caller_function_begin=
caller_function_end=
caller_hash=
caller2_return_rva=
reason=
```

### 4. Capture writer-entry source provenance

For Hero-target calls from exactly the verified `0x351EEB0` callsite, capture only bounded read-only entry data needed to explain the four Position stores.

Determine which entry registers / stack locations / pointed source values ultimately feed:

```text
Transform+0x30
Transform+0x34
Transform+0x38
Transform+0x3C
```

Use complete instruction/data-flow analysis across the 0x2CFC900 function, not just scalar store observation.

Emit:

```text
MOVEMENT_WRITER_SOURCE_MAP
caller_rva=0x351eeb0
source_component_x=
source_component_y=
source_component_z=
source_component_w=
source_kind=register|stack|pointer|computed|unknown
source_span=
source_readable=
source_identity_stable=
post_store_match=
position_w_semantics_observed=
```

No source modification in this task.

### 5. Backtrace source construction in the 0x351EEB0 caller

Once the writer-entry source is known, walk backward through the verified caller to find where that payload is constructed.

Classify the nearest safe semantic layer:

```text
final_target_position
position_plus_delta
displacement
velocity_times_dt
root_motion_delta
desired_direction_times_speed
unknown
```

If the caller is too complex for a full general CFG proof, use bounded runtime correlation plus explicit unsupported edges rather than guessing.

Emit:

```text
MOVEMENT_CALLER_SOURCE_PROOF
caller_rva=0x351eeb0
source_storage=
construction_kind=
producer_rva=
producer_caller_rva=
uses_current_position=0|1
uses_dt=0|1
uses_direction_like_vector=0|1
uses_speed_like_scalar=0|1
surface_tangent_correlation=
exclusive_to_hero_translation=
writable_source_proven=0|1
reason=
```

### 6. Correlate with A and E translation

Keep c11 finite-difference samples and add a bounded correlation record for the 0x351EEB0 source payload.

For each sampled call, compare the predicted/derived XYZ update with observed Hero translation and SurfaceFrame decomposition:

```text
MOVEMENT_SOURCE_CORRELATION
session=
elapsed=
source_before=(...)
source_after_or_payload=(...)
observed_position_before=(...)
observed_position_after=(...)
predicted_delta=(...)
observed_delta=(...)
delta_error=
delta_normal=
delta_forward=
delta_right=
```

Do not claim physics velocity from finite differences alone.

### 7. Distinguish Path A Position from actual locomotion ownership

Continue to observe Path A Position events only as comparison.

The expected c11 pattern is:

```text
Path A may copy/match Position row
then other Position writers can update/overwrite translation
```

Do not upgrade Path A `source+0x30` unless c12 independently proves caller construction, W behavior, downstream/exclusive effects and that it is the intended movement-control point.

### 8. Search for a safer upstream tangent vector

From the authenticated `0x351EEB0` caller and its producer chain, prefer a source that is earlier than final target Position if available:

- tangent displacement;
- velocity;
- desired direction;
- root-motion delta.

For each candidate emit:

```text
MOVEMENT_SOURCE_CANDIDATE
candidate_id=
producer_rva=
consumer_rva=
source_kind=
storage_or_stack_location=
hero_target_verified=
wallrun_owned=
correlates_with_translation=
surface_tangent_relation=
writable_source_proven=
reason=
```

If one reaches `writable_source_proven=1`, document the exact future write contract but still do not perform the write in c12.

### 9. Optional input observation only after movement source is known

Do not hook/rewrite input.

If an authenticated movement source exposes a direction-like vector, c12 may add **read-only** correlation against already-known game/control fields or existing runtime data, but must not invent an input ABI or add a speculative game call.

### 10. Safety

No:

- direct Actor/Transform Position writes;
- Path A Position writes;
- 0x351EEB0/0x2CFC900 source writes;
- input rewriting;
- custom velocity/gravity;
- active adhesion correction;
- PAGE_GUARD/protection mutation;
- camera/animation/wall jump/corners/ceilings/parkour/swing/combat.

If exact source lineage cannot be proven, ship a diagnostic PASS with the precise missing edge.

## Tests

Add offline/native tests for:

- direct callsite/return verification;
- complete 0x2CFC900-like scalar Position-store source lineage;
- source pointer/register/stack variants;
- computed target Position / displacement / velocity-times-dt patterns;
- partial/multi-writer/downstream-overwrite negative cases;
- W mismatch and metadata preservation;
- wrong Hero/Transform/caller/thread/session/generation/Deinit/stale cases;
- A/E SurfaceFrame correlation;
- unchanged c10/c11 orientation and Position-write-zero guarantees.

No private full game-function bodies in the repository/package.

## Human test

Use the c12 package on the same planar walls.

Preferred sequence:

```text
1. A/cardinal continuous WallRun ~4 s
2. E/diagonal continuous WallRun ~4 s
```

No special stick pattern is required unless README_TEST explicitly introduces a read-only correlation pattern based on a source that c12 actually authenticates.

Report:

- posture/orientation still visually stable;
- any hitch/freeze/crash;
- complete private WallSprint log.

No visible movement behavior change is expected.

## PASS criteria

Full c12 PASS:

```text
c11/c10 orientation preserved through verified Deinit
Position writes by mod=0
0x2CFC900 writer boundary/store map authenticated
0x351EEB0 direct callsite -> 0x2CFC900 proven
entry source lineage for XYZ/W identified
caller-side source construction classified
runtime source-to-translation correlation validated on A and E
one safe upstream movement source reaches writable_source_proven=1
no crash/hard kill/DR corruption
```

Diagnostic PASS is acceptable if the exact remaining proof blocker is isolated.

Only after a safe source is admitted should the following task implement a minimal surface-relative movement/source rewrite.

## Required outputs

Update/create:

```text
analysis/V01C11_RUNTIME_RESULT.md
analysis/MOVEMENT_WRITER_2CFC900.md
analysis/MOVEMENT_CALLER_351EEB0.md
analysis/MOVEMENT_SOURCE_LINEAGE.md
analysis/V01C12_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c12_movement_source_lineage.zip
```

Follow `AGENTS.md` / `docs/DEV_RULES.md`. Do not commit raw user logs/video/screenshots or private full captured game-function bytes.
