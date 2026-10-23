# Next task — WallSprint v0.1c6 Planar Adhesion / Safe Position-Source Ownership

## Implementation base

Use the published c5 implementation:

```text
base branch: codex/v01c5-sustained-lifecycle
implementation commit: 1cfe3c76a5e09183757f891c5025def860ef4a88
published c5 head: 4917a3af9afb683db7f7b130626f5b25e20aebca
```

A next branch already exists from that exact c5 head:

```text
codex/v01c6-planar-adhesion
```

Read first:

- `analysis/V01C5_RUNTIME_RESULT.md`
- `docs/PROJECT_STATE.md`
- `docs/CODEX_RESULT.md` on `codex/v01c5-sustained-lifecycle`
- existing surface/Path-A/Path-B analysis documents

## Authoritative c5 real-game result

The large unobstructed A-direction control wall produced a lifecycle-wide orientation PASS.

Runtime surface:

```text
N398 ~= (0,0,+1)
```

Authoritative summary:

```text
ORIENTATION_LIFECYCLE_SUMMARY
wallrun_duration_ms=3843
source_entry_elapsed=0.687000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=219
path_b_writes=218
retention_pass=1
max_held_ms=2906
max_held_updates=205
last_actor_up_dot_surface=1
```

The retained threshold passed at:

```text
ORIENTATION_RETENTION_PASS=1
held_updates=20
held_ms=297
```

and remained valid through the real native Deinit.

No premature `native_lifecycle_inactive_or_stale` exit occurred.

This satisfies the project rule blocking adhesion until orientation retention was proven.

## Objective

Implement the first planar adhesion milestone while preserving c5 orientation behavior.

This is **not** surface-relative locomotion yet.

The goal is only to own and stabilize Hero wall-normal standoff on the verified planar WallRun surface.

Verified plane relationship:

```text
N = N398 = WallRun.this + 0x398
P = P338 = WallRun.this + 0x338
Hero-side reference P320 = WallRun.this + 0x320
P320 - P338 ~= 0.5 * N398

distance = dot(HeroPosition - P338, N398)
target_distance ~= 0.5 m
error = target_distance - distance
```

## Critical safety rule

Do **not** add a direct Actor Transform / Position spam writer.

Before modifying Position, independently prove a safe source-level Position path associated with the already-authenticated Path A caller/target.

If that proof cannot be completed, c6 must remain read-only for Position and ship diagnostic evidence rather than guessing.

## Required work

### 1. Preserve c5 orientation unchanged

Do not regress:

- exact exe version/SHA gate;
- verified WallRun lifecycle;
- source-active-until-Deinit behavior;
- Path A orientation ownership;
- Path B compact 3x3 ownership;
- lock-free publication;
- retention metric;
- c4/c5 hardware-watch diagnostics;
- no direct Actor Transform write.

The c6 Position work must be layered on top of c5, not replace its orientation logic.

### 2. Authenticate the Path A Position source/data flow

Current verified Path A caller-local source layout is:

```text
source+0x00 Right Vec4
source+0x10 Up Vec4
source+0x20 Forward Vec4
source+0x30 Position Vec4
```

Orientation currently owns only `[source+0x00, source+0x30)` and deliberately leaves Position unchanged.

Decode/authenticate the exact accepted-build data flow for the Position row from the WallRun caller through the known writer at `0x2CFC580` to the current verified Hero transform.

Required proof should answer:

- exact source Position offsets;
- exact destination Position offsets;
- whether any component is transformed/computed rather than directly copied;
- whether the same caller/target/thread/stack provenance used by Path A orientation applies;
- whether Position metadata/W are preserved;
- whether modifying this caller-local Position changes only the intended Hero transform Position.

Add a startup proof record such as:

```text
PATH_A_POSITION_PROOF
admitted=0|1
caller_verified=
writer_verified=
source_offsets=
destination_offsets=
stack_relation_verified=
target_relation_verified=
position_w_semantics=
reason=
```

Equivalent names are fine.

Do not enable Position writes unless this proof passes on the exact accepted build.

### 3. Add read-only planar standoff telemetry first

For each bounded WallSprint session, sample:

```text
HeroPosition
P338
N398
distance = dot(HeroPosition - P338, N398)
error = 0.5 - distance
```

Validate:

- `N398` finite and approximately unit length;
- `abs(N398.y)` remains appropriate for current planar vertical-wall scope;
- `P338` finite/readable;
- same verified WallRun owner/session/generation;
- same Hero/Transform identity;
- no stale object reads after Deinit.

Bound logging. Do not emit every call unboundedly.

Suggested record:

```text
ADHESION_SAMPLE
session=
elapsed=
distance=
target_distance=0.5
error=
correction_requested=
correction_applied=
position_source_write=0|1
```

### 4. If Position proof passes, implement bounded source-level correction

Correction must be wall-normal only:

```text
correction_vector = scalar_correction * N398
```

Do not intentionally modify tangent components.

Conservative initial control:

```text
target_distance = 0.5 m
deadzone = 0.01 m
max_abs_step_per_qualifying_update = 0.02 m
candidate_step = error * 0.25
applied_step = clamp(candidate_step, -0.02, +0.02)
```

These values may be made constants/configurable for the diagnostic build, but do not silently increase them.

Additional fail-closed bound:

```text
if abs(error) > 0.35 m:
    do not correct
    record out_of_range
```

The goal is smooth standoff stabilization, not snapping the actor to the wall.

### 5. Source write boundary

If Position is proven safe, extend only the authenticated caller-local source write to the Position components that are actually required.

Requirements:

- orientation rows retain c5 semantics;
- Position W/metadata preserved exactly unless static proof establishes the required semantic value;
- tangent displacement introduced by the adhesion code should be near zero within float tolerance;
- no Path B Position write (Path B remains compact orientation-only);
- no direct Actor memory write from the worker/diagnostic thread;
- no page-protection/PAGE_GUARD changes;
- no new game calls merely to set Position.

### 6. Only correct during valid WallSprint ownership

Position correction may run only when all of these hold:

```text
verified WallRun active
matching session / owner / generation
fresh Hero + Transform identity
c5 source lifecycle active
authenticated Path A call
valid P338 / N398
orientation transition has reached t >= 1.0
```

Stop Position ownership on verified Deinit or any existing c5 fail-closed identity/integrity/kill condition.

Do not retain Position control outside the verified WallRun in c6.

### 7. Preserve natural tangential motion

c6 must not implement movement input.

The vanilla WallRun may continue moving the Hero tangentially/up-down according to its own behavior. Adhesion should only correct wall-normal distance.

Log/verify that the Position source change introduced by adhesion satisfies approximately:

```text
dot(delta_position, tangent_right) ~= 0
dot(delta_position, tangent_forward) ~= 0
```

where the tangent basis is derived from the current surface frame.

### 8. Add bounded session summary

Add:

```text
ADHESION_SUMMARY
session=
position_proof_admitted=
samples=
corrections_applied=
max_abs_error_before=
max_abs_error_after=
max_abs_step=
max_abs_tangent_delta=
source_position_writes=
fail_closed_events=
end_reason=
```

Continue the existing `ORIENTATION_LIFECYCLE_SUMMARY` unchanged.

## Human test

Primary c6 test should use the same large A-direction control wall.

Perform one continuous left-to-right WallRun around 3–4 seconds.

Report:

- whether the existing surface-relative rotation still occurs and stays stable;
- whether there is any visible snap, jitter, wall penetration or sudden outward pop;
- crash/position corruption if any.

Return the complete private WallSprint log.

No E/diagonal test is required until the A-wall adhesion path is proven safe.

## PASS criteria

### Minimum diagnostic PASS

If Position writes remain disabled:

```text
PATH_A_POSITION_PROOF provides an exact admitted/rejected reason
P338/N398/distance telemetry is valid for the active session
c5 orientation retention remains PASS through verified Deinit
```

This is acceptable if source Position ownership is not yet proven.

### Full c6 PASS

```text
PATH_A_POSITION_PROOF admitted=1
c5 orientation retention still PASS through verified Deinit
Position writes occur only through the authenticated caller-local source
all applied correction is wall-normal only
max_abs_step <= 0.02 m
no out-of-range snap
no Position metadata corruption
no crash
no wall penetration/outward pop attributable to the mod
ADHESION_SUMMARY end_reason=verified_native_Deinit or equivalent clean lifecycle end
```

A session with naturally tiny error may legitimately apply few/no corrections; in that case static/runtime Position proof plus stable distance telemetry must clearly show why no correction was necessary. Do not add an artificial large displacement merely to force visible movement.

## Explicit non-goals

Do not add in c6:

- surface-relative WASD/input movement;
- custom gravity replacement;
- WallRun extension beyond verified Deinit;
- animation changes;
- camera rotation;
- wall jump / wall-to-wall;
- corner/ceiling traversal;
- parkour;
- swing reconnect;
- combat.

## Required outputs

Update/create:

```text
analysis/V01C5_RESULT.md
analysis/PATH_A_POSITION_PROOF.md
analysis/PLANAR_ADHESION.md
analysis/V01C6_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c6_planar_adhesion.zip
```

Follow `AGENTS.md` / `docs/DEV_RULES.md`.

Do not commit raw user logs/video or private full captured game-function bytes.
