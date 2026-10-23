# Next task — WallSprint v0.1c5 Sustained Native Lifecycle / Full-WallRun Orientation Hold

## Implementation base

Use the published c4 implementation:

```text
base branch: codex/v01c4-real-thread-enum-fix
implementation commit: cfaae52304dfa757f2098eeb63c0168151c5b755
published c4 head: 6bf7646d2aebdfda9c285b9a3ca4108760d6baa1
```

A next branch already exists from that exact c4 head:

```text
codex/v01c5-sustained-lifecycle
```

Read first:

- `analysis/V01C4_RUNTIME_RESULT.md`
- `docs/PROJECT_STATE.md`
- `docs/CODEX_RESULT.md` on `codex/v01c4-real-thread-enum-fix`

## Authoritative c4 result

The large A-direction control wall produced the first real-game orientation retention PASS.

```text
ORIENTATION_RETENTION_PASS=1
held_ms=329
held_updates=20
```

The same sequence later remained Surface Up for about 55 distinct updates / ~969 ms.

Path A and Path B both committed valid SurfaceFrame orientation. The user reported the prior rapid flicker was no longer visually apparent.

c4's production thread enumeration also succeeded:

```text
owner_pid_matches=129
process_threads_seen=128
eligible=127
installed=126
failed=1
cap_exceeded=1
cleanup_pending=0
```

No THIRD_WRITER_EVENT was captured during the successful retained interval.

## New demonstrated failure

The source ownership ended early even though verified WallRun remained active.

Last successful commits near elapsed 1.953 s:

```text
PATH_A_OVERRIDE_COUNT=69
PATH_B_OVERRIDE_COUNT=68
actor_up ~= Surface Up
```

Then:

```text
NATIVE_SOURCE_EXIT
session=2
reason=native_lifecycle_inactive_or_stale
duration_since_source_entry=1.235000
applied_writes=135
held_updates=55
held_ms=985
stability_observed=1
```

But the independently verified WallRun lifecycle continued until real Deinit at about 3.900 s total duration.

By approximately WallRun elapsed 2.339 s:

```text
automatic_wallrun=Active
WallSprint=inactive
Actor Up ~= World Up
```

Therefore the immediate blocker is a false/early native-source lifecycle inactive/stale decision.

Do not resume speculative third-writer patching. Preserve the diagnostic capability, but fix lifecycle continuity first.

## Objective

Keep the already-working Path A + Path B SurfaceFrame ownership alive continuously from the existing transition until the verified WallRun Deinit.

This task is still orientation-only.

No adhesion, movement, animation, camera, gravity, parkour, swing or combat.

## Required work

### 1. Break down `native_lifecycle_inactive_or_stale`

The current reason collapses several checks into one. Add a bounded once-per-exit diagnostic that records the exact failing sub-gate, without weakening any gate first.

At minimum capture:

```text
NATIVE_LIFECYCLE_EXIT_DIAG
session=
elapsed=
ctx_wallrun_active=
ctx_wallrun_session=
ctx_wallrun_owner=
lifecycle_return_native=
token_observed=
token_phase=
token_last_kind=
token_owner=
token_thread=
token_updates=
token_init_ms=
token_update_ms=
now_ms=
update_age_ms=
fresh_phase_ok=
fresh_owner_ok=
fresh_updates_ok=
fresh_time_order_ok=
fresh_update_age_ok=
owner_matches_ctx=
hero_matches=
transform_matches=
source_transition_active=
```

Equivalent names are fine. The critical requirement is that the next real-game log must say exactly which condition caused the premature exit.

### 2. Inspect and test source lifecycle publication

Trace the data flow in `source_lifecycle.*` and the verified WallRun lifecycle hooks.

Determine why the native-source token becomes inactive/stale around ~1.95 s while:

```text
verified WallRun Update continues
same WallRun owner continues
Hero / Transform remain unchanged
verified Deinit has not occurred
```

Add native tests reproducing a continuous multi-second WallRun token/update sequence. Do not rely only on short <2 s fixtures.

### 3. Repair only the demonstrated continuity bug

Once the failing sub-gate is proven, fix it narrowly.

Do not simply remove the 100 ms freshness requirement, ignore owner/thread identity, or force `native=true` while WallRun is active.

Preserve fail-closed behavior for:

- verified Deinit;
- Hero/Transform identity change;
- owner/session/generation change;
- exact executable / patch identity failure;
- native thread identity failure where genuinely required;
- stale publication that is actually stale;
- kill/F8/shutdown.

If the source lifecycle's own update timestamp is not meant to advance for the whole WallRun, redesign that specific freshness contract so it uses a verified signal that does advance for the whole WallRun. Document why the replacement is semantically valid.

### 4. Keep A/B ownership active through verified Deinit

Desired sequence:

```text
WallRun Init/Entry
~0.7 s: WallSprint transition starts
~0.95 s: Surface Up reached
A/B commits continue for the remainder of active WallRun
verified Deinit occurs
source ownership ends cleanly
```

Do not end ownership merely because the diagnostic multithread trace window closes.

The c4 hardware watcher may remain bounded and diagnostic-only; orientation ownership itself must not depend on that watch being active.

### 5. Preserve c4 behavior and safety

Keep unchanged unless directly required by the proven lifecycle fix:

- exact build/SHA gate;
- Path B compact proof and caller/writer identity;
- Path A Position preservation;
- Path B `[RDX,RDX+0x24)` boundary / metadata preservation;
- lock-free A/B snapshot;
- no direct Actor Transform spam writes;
- c4 thread enumeration / writer diagnostics;
- clean debug-register restoration;
- no PAGE_GUARD/page-protection changes.

### 6. Improve runtime evidence after retention PASS

Continue the existing retention metric, but also add a lifecycle-wide summary at verified Deinit:

```text
ORIENTATION_LIFECYCLE_SUMMARY
session=
wallrun_duration_ms=
source_entry_elapsed=
source_exit_reason=
source_active_until_deinit=0|1
path_a_writes=
path_b_writes=
retention_pass=0|1
max_held_ms=
max_held_updates=
last_actor_up_dot_surface=
```

The summary must distinguish a clean verified Deinit from an early stale/inactive exit.

## Human test

Use the same large unobstructed A-direction wall that allowed the ~3.9 s c4 WallRun.

Perform one continuous left-to-right WallRun of roughly 3–4 s.

Report:

- whether the actor rotates to Surface Up;
- whether it stays rotated for the whole WallRun instead of returning upright around ~2 s;
- crash/position corruption if any.

Return the complete private WallSprint log.

No E test is needed for c5 unless the A run passes and there is a specific reason to regression-check angle handling.

## PASS criteria

Minimum diagnostic success if not fixed yet:

```text
NATIVE_LIFECYCLE_EXIT_DIAG identifies one exact failing sub-gate
```

Primary c5 success:

```text
Path A writes continue beyond elapsed 2.0 s
Path B writes continue beyond elapsed 2.0 s
ORIENTATION_RETENTION_PASS=1
no premature native_lifecycle_inactive_or_stale exit
source_active_until_deinit=1
verified WallRun Deinit cleanly ends ownership
```

Strong success:

- Actor Up remains `dot(ActorUp,N398) >= 0.98` from full transition through the last active-WallRun observations;
- no rapid flicker;
- no unexplained third-writer reset;
- no crash, Position corruption or metadata corruption.

Only after this lifecycle-wide orientation hold passes should the next handoff define the first adhesion milestone.

## Required outputs

Update/create:

```text
analysis/V01C4_RESULT.md
analysis/SOURCE_LIFECYCLE_CONTINUITY.md
analysis/V01C5_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c5_sustained_lifecycle.zip
```

Follow `AGENTS.md` / `docs/DEV_RULES.md`.

Do not commit raw user logs/video or private full captured game functions.
