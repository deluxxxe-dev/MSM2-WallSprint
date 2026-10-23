# Next task — WallSprint v0.1c7 Transient Freshness Recovery

## Implementation base

Use the published c6 implementation:

```text
base branch: codex/v01c6-planar-adhesion
implementation commit: 6ed21d411b8b6868185143d275c2e40486b57781
published c6 head: db874c74cc14f361a2ceb5e15647910e06ebfb84
```

A next branch already exists from that exact c6 head:

```text
codex/v01c7-transient-freshness-recovery
```

Read first:

- `analysis/V01C6_RUNTIME_RESULT.md`
- `docs/PROJECT_STATE.md`
- `docs/CODEX_RESULT.md` on `codex/v01c6-planar-adhesion`
- c5 lifecycle continuity analysis/tests

## Authoritative c6 real-game result

The user visually observed the actor release from surface-relative orientation partway through a long A-wall WallRun. The private runtime log confirms it.

Adhesion was not responsible. c6 stayed Position-read-only:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
mode=position_read_only
```

The exact source-lifecycle exit was:

```text
NATIVE_LIFECYCLE_EXIT_DIAG
session=1
elapsed=2.642367
failing_sub_gate=fresh_update_age
ctx_wallrun_active=1
ctx_wallrun_session=1
lifecycle_read_status=coherent_active
token_phase=3
token_updates=191
token_update_ms=483447203
now_ms=483447453
update_age_ms=250
fresh_phase_ok=1
fresh_owner_ok=1
fresh_updates_ok=1
fresh_time_order_ok=1
fresh_update_age_ok=0
owner_matches_ctx=1
hero_matches=1
transform_matches=1
source_transition_active=1
accepted_event_epoch=0
native_collision_epoch=0
```

Then:

```text
NATIVE_SOURCE_EXIT reason=native_lifecycle_inactive_or_stale
```

The independently verified WallRun remained active and the next verified Update arrived immediately afterward. Final summary:

```text
wallrun_duration_ms=4078
source_exit_reason=native_lifecycle_inactive_or_stale
source_active_until_deinit=0
verified_deinit=1
path_a_writes=132
path_b_writes=130
retention_pass=1
max_held_ms=1422
max_held_updates=114
last_actor_up_dot_surface=1
```

The same WallRun continued roughly 1.4 s after source ownership was permanently closed.

There was no owner/session/Hero/Transform mismatch and no reported native event collision. The runtime evidence therefore demonstrates a transient native Update gap/hitch that exceeds the current 100 ms freshness bound and is incorrectly treated as a permanent session end.

## Objective

Keep freshness fail-closed for each individual source write, but distinguish **transient stale suspension** from **hard lifecycle termination**.

A `fresh_update_age` failure by itself must temporarily suspend A/B source writes, not permanently destroy an otherwise coherent matching verified WallRun session.

When a later fresh verified native Update arrives for the same WallRun session/owner/generation/native thread and all hard identity/integrity gates still pass, WallSprint must resume source ownership without:

- creating a new WallRun session;
- restarting the 0.7 s delay;
- restarting/interpolating orientation from World Up;
- changing Hero Position;
- weakening Deinit/identity/integrity gates.

This task is still orientation/lifecycle only. c6 Position diagnostics may remain read-only.

## Required work

### 1. Split soft stale from hard close

Audit the c5/c6 path that currently maps `fresh_update_age` to `native_lifecycle_inactive_or_stale` and permanent `NATIVE_SOURCE_EXIT`.

Introduce explicit state semantics such as:

```text
ACTIVE
STALE_SUSPENDED
HARD_CLOSED
```

Equivalent implementation is fine.

The exact 100 ms freshness test may remain unchanged initially. The important change is that a time-only freshness failure does not destroy session ownership state.

### 2. While stale, block writes

When `update_age_ms > native_update_freshness_ms`:

- Path A source write must not commit;
- Path B source write must not commit;
- no direct Actor write;
- no Position write;
- no synthetic Update;
- no game call merely to keep the session alive.

Emit bounded evidence once per stale episode, e.g.:

```text
NATIVE_SOURCE_STALE_SUSPEND
session=
elapsed=
update_age_ms=
last_native_update=
owner=
generation=
thread=
actor_up_dot_surface=
```

Do not spam every rejected source call.

### 3. Resume only on a fresh verified native Update

A stale-suspended session may resume only after a real accepted native WallRun Update produces a coherent token satisfying all normal gates for the same ownership identity.

Required resume checks include at least:

```text
same verified WallRun session
same WallRun owner
same generation / accepted event epoch
same expected native thread where required
same Hero
same Transform
fresh phase
fresh time order
fresh update age
no native collision epoch change
no kill/shutdown
current hook/patch integrity still admitted
```

Emit once per recovery:

```text
NATIVE_SOURCE_STALE_RESUME
session=
stale_duration_ms=
previous_update_age_ms=
new_update_age_ms=
native_update=
owner_match=1
hero_match=1
transform_match=1
```

### 4. Do not restart orientation transition

If the orientation transition had already reached `t=1`, recovery must continue with the existing SurfaceFrame orientation target.

Do not rotate from World Up again or reset the 0.7/0.25 timing simply because the stale episode occurred.

If the stale episode occurs before the transition completes, preserve the existing transition state/timing in a way that does not create a visible snap. Document the chosen semantics and test them.

### 5. Keep hard-invalid conditions permanent

The following remain hard close / fail-closed and must not auto-resume inside the same session:

- verified native Deinit;
- WallRun session/owner change;
- generation/accepted event epoch change not explained by the same accepted Update sequence;
- Hero/Transform identity change;
- genuine native event collision / collision epoch invalidation;
- wrong native thread/caller/stack provenance;
- failed exact build/caller/writer/patch integrity;
- kill/F8/shutdown;
- source rollback failure or other existing fatal integrity condition.

A later Update must not revive these hard-closed states.

### 6. Preserve c6 Position work as read-only

Do not advance adhesion Position ownership in c7.

Retain:

```text
PATH_A_POSITION_PROOF admitted=0
ADHESION_SAMPLE read-only
ADHESION_SUMMARY read-only
corrections_applied=0
source_position_writes=0
```

The current real A wall naturally measured about 0.5 m standoff with negligible error, so no artificial displacement is needed.

### 7. Add tests that reproduce the real failure

Add a native lifecycle/source test with a continuous same-owner WallRun sequence that:

1. enters source ownership and reaches full Surface Up;
2. publishes normal Updates;
3. pauses accepted Update publication for roughly 250–300 ms;
4. verifies A/B writes are blocked while stale;
5. publishes the next real same-session native Update;
6. verifies source ownership resumes without a new WallRun session or transition restart;
7. continues to verified Deinit;
8. verifies final `source_active_until_deinit=1` or equivalent successful lifecycle-wide result.

Also test >100 ms stale followed by hard owner/identity/Deinit change and verify it does **not** resume.

### 8. Runtime summary

Extend or preserve the lifecycle summary with bounded stale episode metrics:

```text
ORIENTATION_LIFECYCLE_SUMMARY
...
stale_suspend_count=
stale_resume_count=
max_stale_duration_ms=
source_active_until_deinit=
```

A recovered stale episode must not be reported as `NATIVE_SOURCE_EXIT`.

## Human test

Use the same large A-direction wall.

Perform one continuous WallRun around 4 s. A small frame hitch is acceptable; do not intentionally induce one.

Report:

- whether surface-relative orientation stays visually active for the whole WallRun;
- whether there is any brief snap/upright flash after a hitch;
- crash/Position corruption.

Return the complete private WallSprint log.

## PASS criteria

```text
ORIENTATION_RETENTION_PASS=1
no permanent fresh_update_age exit while same verified WallRun later continues
if stale gap occurs: NATIVE_SOURCE_STALE_SUSPEND then NATIVE_SOURCE_STALE_RESUME
A/B writes resume after fresh accepted Update
no transition restart / no WorldUp snap attributable to recovery
verified Deinit remains the clean final close
source_active_until_deinit=1
Position writes=0
```

If the human run contains no >100 ms gap, normal c5-style full-WallRun hold still must pass. The native test must independently cover the 250–300 ms recovery case.

## Non-goals

Do not add:

- Position correction;
- surface-relative movement input;
- gravity replacement;
- animation;
- camera;
- wall jump;
- corners/ceilings;
- parkour;
- swing reconnect;
- combat.

After real c7 lifecycle-wide retention passes, return to the c6 Position-proof / planar adhesion milestone.

## Required outputs

Update/create:

```text
analysis/V01C6_RESULT.md
analysis/TRANSIENT_FRESHNESS_RECOVERY.md
analysis/V01C7_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c7_transient_freshness_recovery.zip
```

Follow `AGENTS.md` / `docs/DEV_RULES.md`. Do not commit raw user logs/video or private full game-function bytes.