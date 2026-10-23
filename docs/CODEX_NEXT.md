# Next task — WallSprint v0.1c11 Movement Path Ownership / Translation Writer Attribution

## Implementation base

Use c10 delivery head exactly:

```text
base branch: codex/v01c10-secondary-publication-race
implementation commit: 609fdf935e40947d7aad29fea4bed5f2cf3d0e2e
published c10 head: 6246fd6d2e52cf81e18e6ff8ffd4d8ee4d863aa1
version: v0.1c10 / package 0.1.28
```

Pre-created branch:

```text
codex/v01c11-movement-path-ownership
```

Read first:

- `analysis/V01C10_RUNTIME_RESULT.md`
- `analysis/PATH_A_POSITION_PROOF.md`
- `analysis/PLANAR_ADHESION.md`
- `docs/PROJECT_STATE.md`
- c10 branch `docs/CODEX_RESULT.md`

## Authoritative c10 human result

Two real WallRun sessions completed cleanly through verified Deinit.

Diagonal session:

```text
N398 ≈ (-0.70710677, 0, +0.70710677)
wallrun_duration_ms=4172
source_active_until_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
hard_kill_calls=0
secondary seen/admitted/overridden/rejected=102/102/102/0
```

Cardinal session:

```text
wallrun_duration_ms=3421
source_active_until_deinit=1
retention_pass=1
last_actor_up_dot_surface=1
hard_kill_calls=0
secondary seen/admitted/overridden/rejected=161/161/161/0
```

Across the c10 human log:

```text
post-transition actor_up=(0,1,0) observations=0
secondary_source_publication_changed=0
Position writes by mod=0
```

The c10 bounded publication-refresh branch did not naturally fire in that log; deterministic c10 fixtures are the evidence for the actual race branch. This does not block movement-path discovery.

## Deferred visual alignment

The user has visually identified:

- excessive apparent wall standoff / floating feet;
- body/root lean relative to the apparent surface compared with normal ground running.

Do **not** solve these in c11. Record them as a later visual-alignment/contact pass. c11 is about movement ownership only.

## Objective

Identify and authenticate the native/caller-local data path that actually advances Hero translation during verified WallRun, so the next revision can implement surface-relative movement upstream and safely.

**c11 is diagnostic/read-only with respect to movement and Position.**

Do not directly write Actor Transform Position, do not alter the candidate Path A Position row, and do not add a custom velocity/gravity system in this task.

The preferred outcome is a proof-quality answer to:

```text
Which writer(s) update Hero Transform Position during WallRun?
What caller/source data produces those updates?
Is Path A source+0x30 actually causal for Position?
If not, what safer upstream velocity/displacement source should c12 own?
```

## Required work

### 1. Preserve c10 orientation exactly

Do not regress:

- exact build/SHA gate;
- c7 stale suspend/resume;
- c8 first hard-kill attribution;
- c9 secondary-thread Path B admission;
- c10 bounded publication refresh/revalidation;
- Path A/B orientation behavior;
- no direct Actor Transform writes;
- Position writes remain zero.

The c11 diagnostics must not change the c10 Surface Up result.

### 2. Add bounded Hero Position writer observation

Observe writes to the current verified Hero Transform Position row:

```text
Transform + 0x30 .. +0x3F
```

The read layout already used by the SDK establishes XYZ at `+0x30/+0x34/+0x38`; c11 must also observe/read `+0x3C` only as bounded read-only diagnostic data, without assuming its semantic meaning in advance.

Use hardware data breakpoints / existing watch infrastructure rather than PAGE_GUARD or page-protection mutation.

Coordinate with the existing Up writer watch so the two diagnostics do not fight over DR0-DR3. Preferred policy:

- keep the existing early orientation watch unchanged;
- after full Surface Up/retention and after the old bounded Up watch has ended, arm a separate bounded Position watch;
- use at most the available hardware slots and preserve/restore foreign DR state exactly;
- no unbounded rescans or persistent process-wide debugging.

A reasonable real window is roughly `t >= 1.55 s` for ~0.4–0.8 s, but derive the exact window from existing coordinator timing and tests rather than hardcoding an unsafe overlap.

### 3. Attribute actual Position writers

For each unique writer identity, capture bounded evidence such as:

```text
POSITION_WRITE_EVENT
session=
elapsed=
thread_id=
rip/module_rva=
writer_function_rva=
caller_rva=
caller2_rva=
target_transform=
slot_mask=
position_before=(x,y,z,w?)
position_after=(x,y,z,w?)
delta=(dx,dy,dz)
delta_normal=
delta_surface_forward=
delta_surface_right=
relative_us_from_last_path_a=
relative_us_from_last_path_b=
classification=
```

Do not spam per-frame logs. Keep a strict event cap per unique writer and aggregate hit counts.

Add a final summary:

```text
POSITION_WRITER_SUMMARY
session=
watch_armed=
threads_seen=
threads_installed=
unique_writers=
total_write_events=
writer_1_rva=/hits=
writer_2_rva=/hits=
...
unknown_writer_events=
cleanup_restored=
```

### 4. Decompose translation in SurfaceFrame

For observed consecutive Hero Position samples, compute:

```text
delta = P_now - P_prev
normal_component = dot(delta, SurfaceUp/N398)
forward_component = dot(delta, SurfaceForward)
right_component = dot(delta, SurfaceRight)
```

Also record `dt` and a finite-difference velocity estimate for correlation only.

This is diagnostic decomposition, not a physics claim.

Add bounded samples such as:

```text
MOVEMENT_DELTA_SAMPLE
session=
native_update=
dt_ms=
delta_world=(...)
delta_normal=
delta_forward=
delta_right=
speed_estimate=
N398=(...)
```

### 5. Revisit Path A source+0x30 Position causality

c6 established only a candidate source row and pre-writer equality; it did **not** prove causal copy or exclusive effects.

Use c11 writer-watch/runtime evidence plus complete bounded static inspection to answer whether the authenticated Path A writer/caller actually propagates:

```text
source+0x30/+0x34/+0x38/+0x3C
```

to the verified Hero Transform Position row.

If Path A is observed as a Position writer, prove at minimum:

- exact destination offsets/instructions;
- exact source lineage for all Position components actually written;
- caller-local source provenance still matches authenticated stack relation;
- post-writer target XYZ equals the expected source/result when that writer executes;
- behavior of the fourth component is observed and documented, not guessed;
- any calls/branches that can alter or duplicate Position effect are accounted for;
- whether other Position writers immediately overwrite the result;
- whether modifying this source can reasonably be isolated to intended Hero translation rather than other transform metadata.

Do not set `admitted=1` from simple source/Hero equality alone.

Emit:

```text
PATH_A_POSITION_CAUSALITY_PROOF
admitted=0|1
writer_verified=
caller_verified=
stack_relation_verified=
source_offsets=
destination_offsets=
post_writer_match=
position_w_observed=
downstream_overwrite_observed=
exclusive_effects_proven=
reason=
```

### 6. Prefer a safer upstream movement source if one exists

Analyze the authenticated WallRun Entry/Update and the newly identified Position writer/caller chain for a caller-local or native source representing:

- displacement;
- velocity;
- desired movement direction;
- root-motion translation;
- another tangent-motion vector upstream of final Transform Position.

A safe upstream vector is preferable to manually incrementing Transform Position.

If a candidate is found, document exact provenance and runtime correlation, but **do not modify it in c11**.

Emit one bounded candidate table:

```text
MOVEMENT_SOURCE_CANDIDATE
candidate_id=
producer_rva=
consumer_rva=
source_kind=position|displacement|velocity|direction|root_motion|unknown
storage_or_stack_location=
wallrun_owned=0|1
hero_target_verified=0|1
correlates_with_translation=0|1
surface_tangent_relation=
writable_source_proven=0|1
reason=
```

If no safe upstream candidate can be authenticated, say so explicitly.

### 7. No input rewrite yet

Do not hook or rewrite controller/keyboard input in c11.

You may document already-visible input/control accesses in the authenticated WallRun code if static/runtime evidence supports them, but do not add a speculative input ABI or new game call.

The next task will map input into SurfaceFrame only after c11 establishes a safe movement source.

### 8. Safety / fail-closed requirements

No:

- direct Actor Position write;
- Path A Position modification;
- active adhesion correction;
- custom gravity;
- movement injection;
- PAGE_GUARD/protection changes;
- camera/animation/wall jump/corners/ceilings/parkour/swing/combat.

If writer attribution cannot be made safe with current hardware-watch capacity or thread coverage, fail closed and ship the strongest bounded diagnostic result instead of broadening to invasive instrumentation.

### 9. Tests

Add offline/native tests for:

- Position watch arm/restore and foreign DR preservation;
- writer identity aggregation/event caps;
- SurfaceFrame delta decomposition on cardinal and diagonal normals;
- Path A Position causal-positive synthetic fixture;
- computed/partial/multi-writer/overwrite negative fixtures;
- fourth-component mismatch;
- wrong caller/target/thread/stack/session/generation/Deinit/stale cases;
- c10 orientation regression unchanged while Position watch is inactive and while it is active after the Up watch window.

Do not ship private full game-function bodies.

## Human test

Use the c11 package on the same large planar walls.

Preferred sequence:

```text
1. one continuous ~4 s cardinal/A-style WallRun
2. one continuous ~4 s diagonal/E-style WallRun
```

No special stick pattern is required for c11 unless README_TEST explicitly adds one based on an authenticated input observation discovered during implementation.

Report:

- whether c10 surface-relative posture still holds visually;
- crash/hitch/freeze if any;
- full private WallSprint log.

The important c11 evidence is diagnostic; no visible movement change is expected.

## PASS criteria

Full c11 proof PASS:

```text
c10 orientation still holds to verified Deinit
Position writes by mod=0
bounded Position watch arms and restores cleanly
actual Hero Position writer identity/identities captured
SurfaceFrame translation decomposition valid on cardinal and diagonal runs
PATH_A_POSITION_CAUSALITY_PROOF has an evidence-based admitted/rejected result
at least one safe upstream movement source is authenticated, OR exact blocker is identified
no hard kill/crash/foreign DR corruption
```

A diagnostic PASS is acceptable if no writable movement source is admitted, provided the task identifies the exact missing proof needed for the next step.

Do **not** proceed to actual surface-relative input rewriting inside c11.

## Required outputs

Update/create:

```text
analysis/V01C10_RUNTIME_RESULT.md
analysis/POSITION_WRITER_ATTRIBUTION.md
analysis/PATH_A_POSITION_CAUSALITY.md
analysis/MOVEMENT_SOURCE_OWNERSHIP.md
analysis/V01C11_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c11_movement_path_ownership.zip
```

Follow `AGENTS.md` / `docs/DEV_RULES.md`. Do not commit raw user logs/video/screenshots or private full captured game-function bytes.