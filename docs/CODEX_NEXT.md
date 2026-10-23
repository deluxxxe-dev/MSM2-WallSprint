# Next task — WallSprint v0.1c8 Exact Hard-Kill Attribution / Observation Guard Decoupling

## Implementation base

Use published c7:

```text
base branch: codex/v01c7-transient-freshness-recovery
implementation commit: 93d7887e8ba8cb04e38473a0a821077ee8fc9efb
published c7 head: 466f0ab237faccd8c769d17fedd66a42138a7829
```

Pre-created branch:

```text
codex/v01c8-kill-cause-attribution
```

Read first:

- `analysis/V01C7_RUNTIME_RESULT.md`
- `analysis/TRANSIENT_FRESHNESS_RECOVERY.md`
- `docs/PROJECT_STATE.md`
- c7 `docs/CODEX_RESULT.md`

## Authoritative c7 real-game failure

The user reported no visible surface-relative rotation; the actor stayed effectively vanilla.

The source override actually started but was hard-closed after only two Path A commits:

```text
elapsed=0.703  Path A write #1  t=0.000
elapsed=0.719  Path A write #2  t=0.064
NATIVE_SOURCE_EXIT reason=disabled_kill_or_patch_gate
```

The verified WallRun continued after that close. This happened before any meaningful c7 stale-suspend/resume episode.

Immediately after the first Path A write, the bounded reset/orientation watch stopped with:

```text
stop_reason=observation_guards_lost
```

Do not assume that trace stop itself caused the kill. It is only temporal evidence until the first kill caller is identified.

## Code-level issue already identified

The c7 telemetry worker contains a broad coupling:

```cpp
if(!msm2::hooks::observationPlanAuthenticated()) {
    msm2::reset_trace::kill();
    msm2::trace::kill();
    msm2::input::kill();
    msm2::native_source::kill();
}
```

and later:

```cpp
msm2::native_source::tick(...,
    msm2::hooks::observationPlanAuthenticated() &&
    msm2::input::authenticated());
```

`tick()` itself hard-kills if that combined boolean is false.

`observationPlanAuthenticated()` currently collapses multiple conditions:

```text
verifiedAvailable
msm2::source::fieldPlanEnabled
WallRun Init enabled/readable/patch bytes match
WallRun Entry enabled/readable/patch bytes match
WallRun Update enabled/readable/patch bytes match
WallRun Deinit enabled/readable/patch bytes match
```

There are also independent `kill()` sites in Path A/B entry validation. Therefore the generic runtime reason `disabled_kill_or_patch_gate` is insufficient to establish the first cause.

## Objective

First attribute the exact first hard-kill source. Then repair only a proven false coupling or false integrity failure while preserving every genuine fail-closed condition.

This task is orientation/lifecycle only. Do not progress adhesion Position writes.

## Required work

### 1. Replace anonymous hard kill with a first-cause latch

Add an internal kill reason/context API so every hard-kill call has an explicit reason. Preserve one immutable first cause per process/session and bounded later counters if useful.

Required categories should distinguish at least:

```text
worker_observation_plan_lost
worker_input_auth_lost
path_a_wrong_thread
path_a_known_caller_mismatch
path_a_hero_transform_mismatch
path_a_current_guards_lost
path_a_stack_or_source_layout_failed
path_a_rollback_failed
path_b_wrong_thread
path_b_compact_or_guard_lost
path_b_hero_transform_mismatch
path_b_stack_or_source_layout_failed
path_b_rollback_failed
entry_token_incoherent
token_identity_epoch_mismatch
F8_or_script_stop
verified_Deinit
```

Equivalent names are fine, but the next real log must identify the exact first hard-kill caller and condition.

Emit once per first kill, e.g.:

```text
NATIVE_SOURCE_HARD_KILL
session=
elapsed=
reason=
origin=worker|path_a|path_b|entry_token|shutdown
thread_id=
wallrun_active=
wallrun_session=
owner=
hero=
transform=
native_update=
```

Do not log private raw game-function bytes.

### 2. Decompose `observationPlanAuthenticated()`

Add a side-effect-free detailed status structure/function. It must report, at minimum:

```text
verified_available
field_plan_enabled
init_enabled
init_read_ok
init_patch_match
entry_enabled
entry_read_ok
entry_patch_match
update_enabled
update_read_ok
update_patch_match
deinit_enabled
deinit_read_ok
deinit_patch_match
```

When a state first changes from all-good to bad, emit a bounded record such as:

```text
OBSERVATION_PLAN_AUTH_FAILURE ...
```

Include expected-vs-current mismatch identity in a safe form such as first differing byte offset / hashes; do not dump full functions.

### 3. Separate diagnostic viability from source ownership where proven safe

Do not make a blanket change before the detailed cause exists.

If the real/synthetic reproduced failure is only an observation/diagnostic condition that is not semantically necessary for already-authenticated Path A/B source ownership, stop the diagnostic component without killing `native_source`.

Examples that may be decouplable only if proven:

- diagnostic field-plan state used only for observation;
- bounded reset trace/watch availability;
- diagnostic trace cleanup/coverage state.

Conditions that must remain hard kill include actual modification/loss of any code/patch/caller/writer identity that Path A/B ownership depends on, wrong native thread, wrong stack provenance, Hero/Transform/session/owner/epoch mismatch, genuine collision, rollback failure, build mismatch, F8/shutdown.

### 4. Preserve c7 stale suspend/resume

Do not regress the c7 behavior:

```text
>100 ms time-only freshness gap
-> block A/B writes
-> STALE_SUSPENDED
-> later real same-identity Update
-> resume without session/transition restart
```

Its synthetic 250–300 ms recovery tests must still pass.

### 5. Add the missing real-failure-style test

Create a test that reproduces the worker/observation guard transition near first source activation (~0.7 s) while Path A/B are beginning interpolation.

It must prove one of two outcomes:

A. If the changed guard is diagnostic-only:

```text
diagnostic shuts down
native source remains alive
Path A/B continue
transition reaches t=1
```

B. If a real source-integrity patch is changed:

```text
first hard-kill reason identifies exact patch/integrity failure
A/B stop immediately
no auto-resume
```

Also test all four WallRun hook patch mismatches independently so the log/status says which hook failed.

### 6. Preserve safety / scope

Keep unchanged unless directly required by the proven fix:

- exact executable/SHA admission;
- c7 transient freshness recovery;
- Path A caller/writer/stack/thread/Hero/Transform gates;
- Path B compact proof and metadata boundary;
- no direct Actor Transform writes;
- Position proof remains not admitted/read-only;
- adhesion corrections = 0;
- no PAGE_GUARD/page-protection changes;
- no extra game call merely to keep ownership alive.

### 7. Human runtime evidence

Use the same large A-direction wall for one continuous ~4 s WallRun.

Required observations:

- does actor start rotating after ~0.7 s;
- does it reach Surface Up;
- does it remain there until verified Deinit;
- any upright flash/snap/crash.

Return the complete private log.

The log must include the new first-cause and observation-plan diagnostics even if no failure occurs (an all-good startup/summary record is sufficient for the no-failure case).

## PASS criteria

Primary success:

```text
Path A/B continue beyond the first ~0.7 s transition
ORIENTATION_RETENTION_PASS=1
no anonymous disabled_kill_or_patch_gate close
verified Deinit is final clean close
source_active_until_deinit=1
Position writes=0
```

If a hard kill still occurs, c8 is at least a diagnostic success only if:

```text
NATIVE_SOURCE_HARD_KILL identifies exact first cause
OBSERVATION_PLAN_AUTH_FAILURE identifies exact failed sub-gate/patch
```

and no broad unsupported weakening is applied.

## Non-goals

Do not add:

- Position correction / adhesion writes;
- surface-relative movement input;
- gravity replacement;
- animation;
- camera;
- wall jump;
- corners/ceilings;
- parkour;
- swing reconnect;
- combat.

## Required outputs

Update/create:

```text
analysis/V01C7_RESULT.md
analysis/HARD_KILL_ATTRIBUTION.md
analysis/V01C8_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c8_kill_cause_attribution.zip
```

Follow `AGENTS.md` / `docs/DEV_RULES.md`. Do not commit raw user logs/video or private full captured game-function bytes.