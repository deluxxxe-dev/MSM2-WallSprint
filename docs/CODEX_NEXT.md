# Next task — WallSprint v0.1c12 Movement Source Lineage / Upstream Translation Ownership

## Base
```text
base branch: codex/v01c11-movement-path-ownership
implementation commit: 2129e69fe003e71bf705fb67f723b447cf3e3764
published c11 head: 9593f0230e220770f2b34faf512ca4108c1825cc
version: v0.1c11 / package 0.1.29
branch: codex/v01c12-movement-source-lineage
```

Read first:
- `analysis/V01C11_RUNTIME_RESULT.md`
- `analysis/POSITION_WRITER_ATTRIBUTION.md`
- `analysis/PATH_A_POSITION_CAUSALITY.md`
- `analysis/MOVEMENT_SOURCE_OWNERSHIP.md`
- `docs/PROJECT_STATE.md`
- c11 `docs/CODEX_RESULT.md`

## Real c11 evidence
Human sequence: A/cardinal then E/diagonal.

Both sessions preserved Surface Up through verified Deinit; Position writes by mod remained zero.

Bounded Position watch captured real Transform Position writers on both runs:
```text
writer_function_rva=0x2CFC900
store RVAs=0x2CFC936/0x2CFC93C/0x2CFC942/0x2CFC945
Transform destinations=+0x30/+0x34/+0x38/+0x3C
```

The caller chain correlated with actual translation on both walls:
```text
caller return RVA=0x351EEB0
caller2 return RVA≈0x351A020
consumer writer=0x2CFC900
```

Path A `0x2CFC580` also touches Position and can produce post-writer equality with its `source+0x30` candidate, but downstream Position writes are observed and exclusive effects are unproven. Path A Position remains denied.

No c11 runtime candidate reached `writable_source_proven=1`.

## Objective
Authenticate the movement-producing source lineage centered on:
```text
0x351EEB0 -> 0x2CFC900
caller2≈0x351A020
```

Determine whether the upstream payload is final target Position, displacement, velocity, desired direction, root-motion translation or another locomotion vector.

The preferred result is a caller-local/native source contract that a later revision can rewrite in SurfaceFrame coordinates without direct Actor Position spam.

**c12 is diagnostic/read-only for movement. Do not rewrite input or Position.**

## Required work

### 1. Preserve c10/c11 orientation and diagnostics
Keep all existing build/lifecycle/orientation guards, c7-c10 safety logic, c11 Position watch and Position_writes_by_mod=0.

### 2. Authenticate the 0x2CFC900 writer function
Establish exact runtime-function boundary, complete decode, current code hash, the four Position stores and their source data lineage.

Emit:
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

Do not commit/log the full private body.

### 3. Prove the 0x351EEB0 callsite
Find the exact call whose return address is `0x351EEB0`, prove target `0x2CFC900`, derive containing caller boundary and attribute the observed caller2 chain.

Emit:
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
For exact Hero-target calls from the verified callsite only, capture bounded read-only entry registers/stack/source values needed to explain Transform +0x30/+0x34/+0x38/+0x3C.

Use complete instruction/data-flow analysis across 0x2CFC900.

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

### 5. Backtrace construction in the caller
Walk backward from the writer-entry payload through the verified caller and classify the nearest meaningful layer:
```text
final_target_position
position_plus_delta
displacement
velocity_times_dt
root_motion_delta
desired_direction_times_speed
unknown
```

Emit:
```text
MOVEMENT_CALLER_SOURCE_PROOF
caller_rva=0x351eeb0
source_storage=
construction_kind=
producer_rva=
producer_caller_rva=
uses_current_position=
uses_dt=
uses_direction_like_vector=
uses_speed_like_scalar=
surface_tangent_correlation=
exclusive_to_hero_translation=
writable_source_proven=
reason=
```

### 6. Runtime source-to-translation correlation
For bounded sampled calls, compare source/predicted XYZ with observed Hero translation and SurfaceFrame components.

Emit:
```text
MOVEMENT_SOURCE_CORRELATION
session=
elapsed=
source_payload=(...)
observed_position_before=(...)
observed_position_after=(...)
predicted_delta=(...)
observed_delta=(...)
delta_error=
delta_normal=
delta_forward=
delta_right=
```

Finite differences are correlation only, not physics velocity.

### 7. Keep Path A Position as comparison only
Do not upgrade `source+0x30` unless caller construction, W, downstream/exclusive effects and intended locomotion ownership are independently proven.

### 8. Prefer a safer upstream tangent vector
Search the authenticated caller/producer chain for displacement, velocity, desired direction or root-motion delta earlier than final Position. For each candidate emit the existing `MOVEMENT_SOURCE_CANDIDATE` fields and set `writable_source_proven=1` only with exact provenance and exclusive intended effect.

If one is proven, document the future write contract but do not write it in c12.

### 9. Optional read-only input correlation only after source proof
No input hook/rewrite. If a direction-like source is authenticated, read-only correlation with already-known control data is allowed; no speculative ABI/game call.

## Safety
No direct Actor/Transform Position write, Path A Position write, 0x351EEB0/0x2CFC900 source write, input rewrite, custom gravity/velocity, active adhesion, PAGE_GUARD/protection mutation, camera/animation/traversal extensions.

If full source lineage cannot be proven, ship diagnostic PASS with the exact missing edge.

## Tests
Add offline/native coverage for callsite verification, source-lineage variants, computed target Position/displacement/velocity*dt patterns, multi-writer/overwrite/W mismatch negatives, wrong identity/lifecycle cases, A/E correlation, and unchanged c10/c11 behavior.

## Human test
Preferred:
```text
1. A/cardinal ~4 s continuous WallRun
2. E/diagonal ~4 s continuous WallRun
```
No special stick pattern unless README_TEST explicitly adds a read-only correlation pattern based on a source c12 actually authenticates.

Return complete private log and note any crash/hitch/freeze or posture regression. No visible movement change is expected.

## PASS
Full PASS requires writer/callsite/source lineage/caller construction and A/E runtime correlation, plus at least one safe upstream source with `writable_source_proven=1`, while Position writes stay zero and orientation remains stable.

Diagnostic PASS is acceptable if the exact remaining blocker is isolated.

Only after a safe source is admitted should the next handoff authorize a minimal surface-relative movement/source rewrite.

## Outputs
```text
analysis/V01C11_RUNTIME_RESULT.md
analysis/MOVEMENT_WRITER_2CFC900.md
analysis/MOVEMENT_CALLER_351EEB0.md
analysis/MOVEMENT_SOURCE_LINEAGE.md
analysis/V01C12_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
WallSprint_v0.1c12_movement_source_lineage.zip
```

Follow `AGENTS.md` / `docs/DEV_RULES.md`. Do not commit raw user logs/video/screenshots or private full captured game-function bytes.
