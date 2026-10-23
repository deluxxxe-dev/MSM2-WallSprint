# Runtime gate — c9 diagonal E-wall regression

## Status

There is currently **no new Codex implementation task**.

The c9 implementation already passed the large A-direction control wall in real runtime. Do not create c10 or modify source code until the same c9 build is tested on a diagonal E-type planar wall.

Current runtime base:

```text
branch: codex/v01c9-secondary-path-b-thread-admission
implementation commit: 0c276126ccac8e3bd2a6dd1d850da63e80bc35ce
published head: b865dabfa81626248ea8a8ae56a1dd1c992d7301
version: v0.1c9 / package 0.1.27
```

Read:

- `analysis/V01C9_RUNTIME_RESULT.md`
- `docs/PROJECT_STATE.md`
- c9 branch `docs/CODEX_RESULT.md`

## Authoritative c9 A-wall PASS

```text
wallrun_duration_ms=3844
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=239
path_b_writes=238
retention_pass=1
max_held_ms=2859
max_held_updates=221
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
```

Secondary-thread Path B admission was exercised heavily:

```text
seen=193
admitted=193
overridden=193
rejected=0
unique_threads=16
```

Position remained read-only. The current A wall showed essentially exact 0.5 m vanilla standoff across 29 samples, so active adhesion correction is not justified yet.

## Required next human test

Use **the same c9 package**, not a new build.

Find/use a diagonal E-type planar wall. Historical E evidence was approximately:

```text
N398 ≈ (-0.707107, 0, +0.707107)
```

The exact angle is not important; it only needs to be clearly non-cardinal so Surface Up is not the A-wall axis.

Perform one continuous WallRun long enough for:

```text
~0.7 s delay
+ ~0.25 s orientation transition
+ some retained Surface Up time
```

A 4 s wall is preferred but not required. A shorter run is acceptable if full transition and retention complete before verified Deinit.

Return the private complete WallSprint log and visual observation.

## E regression PASS criteria

```text
N398 is diagonal / non-cardinal
rotation target follows that N398
ORIENTATION_RETENTION_PASS=1
last_actor_up_dot_surface ≈ 1
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
first_hard_kill=none
Position writes=0
no visible upright flash/snap/crash
```

Secondary-thread Path B counts may vary. If eligible secondary calls occur, they must not kill or migrate native ownership.

## After PASS

Only after the E regression passes should a new implementation task be created.

The next implementation milestone should focus on planar locomotion control / safe movement-path ownership. Active normal-direction adhesion correction remains optional/deferred unless new movement control causes measurable deviation from the ~0.5 m surface standoff.

Do not advance camera, animation, wall jump, corners/ceilings, swing reconnect or combat yet.