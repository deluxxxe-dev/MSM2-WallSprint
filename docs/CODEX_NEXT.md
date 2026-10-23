# Next task — WallSprint v0.1c9 Secondary-Thread Path B Admission

## Implementation base

Use published c8 exactly:

```text
base branch: codex/v01c8-kill-cause-attribution
implementation commit: 98c1d45108c89262750616c7258414cf762cac55
published c8 head: 75af734d4d32d5e3a77087cbee6b2247b9617ced
```

Pre-created branch:

```text
codex/v01c9-secondary-path-b-thread-admission
```

Read first:

- `analysis/V01C8_RUNTIME_RESULT.md`
- `analysis/HARD_KILL_ATTRIBUTION.md`
- `analysis/TRANSIENT_FRESHNESS_RECOVERY.md`
- `docs/PROJECT_STATE.md`
- c8 `docs/CODEX_RESULT.md`

## Authoritative c8 real-game result

c8 exact attribution worked. The actor rotated to full Surface Up and retention passed:

```text
ORIENTATION_RETENTION_PASS=1
held_ms=312
held_updates=20
```

At native elapsed `1.750000`, the first hard kill was identified exactly:

```text
NATIVE_SOURCE_HARD_KILL
session=1
elapsed=1.750000
reason=path_b_wrong_thread
origin=path_b
thread_id=35948
native_thread=10680
expected_thread=10680
wallrun_active=1
wallrun_session=1
```

This was not accompanied by any patch, caller, compact proof, observation-plan, Hero/Transform, owner/session/generation/epoch or freshness failure:

```text
input_auth=1
copy_auth=1
compact_proof=1
caller_read_ok=1
caller_patch_match=1
source_failed_gate=none
observation_failed_hook=none
observation_failure_mask=0x0
all four WallRun hook patches read/match
native lifecycle token=coherent_active
```

The secondary-thread Path B entry retained the verified Path B shape:

```text
RCX = current active Hero Transform
entry_return RVA = 0x2CFC84B
entry_source - entry_rsp = 0x28
same WallRun session/owner/generation/accepted epoch
```

Immediately after source hard-close, the independently verified WallRun continued. Final lifecycle:

```text
Init=1
Entry=1
Update=279
Deinit=1
duration=4.405647 s
end_reason=verified_deinit
```

Therefore the current fatal invariant

```text
Path B current thread must equal the verified WallRun/native Update thread
```

is false in real runtime.

All normally logged Path B entries before the failure used thread `10680`. The first observed secondary-thread active-Hero Path B call was thread `35948`. The bounded hardware watch had already stopped at elapsed 1.500 s, so it did not cover this later event.

No c7 stale-suspend episode occurred in the c8 run. Position remained read-only:

```text
position_proof_admitted=0
corrections_applied=0
source_position_writes=0
```

## Objective

Allow a narrowly verified secondary-thread Path B call to apply the already-published WallSprint compact orientation basis without treating thread inequality by itself as a session-fatal condition.

Do **not** globally remove thread/provenance validation. Do **not** migrate the expected native WallRun thread. The new behavior is a per-call Path B admission path only.

This task remains orientation/lifecycle only. No Position correction or movement.

## Required work

### 1. Replace `path_b_wrong_thread` fatal rule with secondary-call classification

Current c8 behavior effectively contains:

```cpp
if (p.thread != GetCurrentThreadId()) {
    kill("path_b_wrong_thread", "path_b");
    return;
}
```

Do not simply delete the condition.

Split Path B calls into:

```text
PRIMARY_PATH_B
SECONDARY_PATH_B_VERIFIED
FOREIGN_OR_INVALID_PATH_B
```

Equivalent naming is fine.

Primary Path B keeps current behavior.

A thread mismatch is not by itself fatal. It starts a secondary-thread verification path.

### 2. Secondary-thread Path B must independently pass all non-thread provenance

A secondary-thread Path B call may override compact orientation only if all of the following are true at that call:

```text
source session is active and not hard-closed
same verified WallRun session
same WallRun owner
same Hero
RCX == same active Hero Transform
same generation
same accepted event epoch
same collision epoch
native lifecycle token is coherent_active
native token is fresh under the existing c7/c8 freshness policy
exact verified Path B caller return RVA 0x2CFC84B
current-thread RSP has valid stack bounds
RDX == current writer-entry RSP + 0x28
compact source span lies inside current thread stack bounds
compact 3x3 proof remains authenticated
source rows are readable/finite/layout-valid
all Path A/B caller/writer/build/source-integrity gates remain healthy
all four WallRun lifecycle patch ownership checks remain healthy
no kill/F8/shutdown/rollback failure
```

Reuse existing production validation functions where possible. Do not create a weaker duplicate validator.

The fact that the call is on another thread must not change `expectedThread` / native lifecycle owner thread stored for the WallRun session.

### 3. Apply only orientation bytes on an admitted secondary call

For `SECONDARY_PATH_B_VERIFIED`:

- use the already-published current WallSprint orientation target/interpolation state;
- write only the proven Path B compact 3x3 range `[RDX+0x00,RDX+0x24)`;
- preserve metadata/scale and all bytes outside the proven range;
- no Position write;
- no direct Actor Transform write;
- no lifecycle token publication from the secondary thread;
- no synthetic native Update;
- do not restart the 0.7 s delay or 0.25 s transition;
- do not change expected native thread identity.

If transition already reached `t=1`, secondary Path B should apply the full current SurfaceFrame basis.

### 4. Invalid secondary calls must not acquire write permission

A secondary-thread call that fails caller/stack/target/layout/lifecycle/identity/integrity validation must not get the WallSprint orientation payload.

Preserve fail-closed semantics for genuine source-integrity corruption.

Differentiate at least:

```text
secondary_wrong_target
secondary_wrong_caller
secondary_stack_relation_failed
secondary_source_span_failed
secondary_token_incoherent
secondary_identity_epoch_mismatch
secondary_source_integrity_lost
```

Use judgment for reject-vs-hard-kill:

- an unrelated/foreign call that simply does not belong to the active Hero/Path B ownership should normally be rejected/passed through without killing the active WallSprint session;
- actual corruption/loss of authenticated source code, owned caller/writer bytes, lifecycle patch ownership, active Hero identity, current session/owner/generation/epoch, collision/rollback state remains hard-close.

Do not turn arbitrary unrelated Path B traffic in the process into a session-fatal event.

### 5. Add bounded secondary-thread telemetry

Emit first/summary evidence such as:

```text
PATH_B_SECONDARY_THREAD_EVENT
session=
elapsed=
thread_id=
expected_thread=
caller_rva=
target_transform=
rdx_minus_rsp=
stack_verified=
compact_verified=
token_status=
identity_verified=
source_up_before=
target_up=
action=override|reject
reason=
```

and final summary:

```text
PATH_B_SECONDARY_THREAD_SUMMARY
seen=
admitted=
overridden=
rejected=
unique_threads=
first_thread=
last_thread=
```

Bound logging. Do not dump private full game-function bytes.

Keep the c8 immutable abnormal first-cause latch. A successfully admitted secondary call must not occupy it.

### 6. Tests must reproduce the c8 real failure exactly

Add a production-policy/native fixture sequence:

1. same owner/session enters WallSprint;
2. Path A + primary Path B transition to full Surface Up;
3. retention threshold passes;
4. a Path B call arrives on another thread;
5. it uses the same active Hero Transform;
6. exact caller is `0x2CFC84B`;
7. current secondary thread has its own valid stack, with `RDX=RSP+0x28` and valid compact source;
8. all source/lifecycle/integrity gates remain healthy;
9. secondary call receives Surface Up orientation;
10. no hard kill occurs;
11. expected native WallRun thread remains unchanged;
12. primary A/B calls continue afterward;
13. verified Deinit cleanly ends the session;
14. final source active-until-Deinit / retention succeeds.

Also add negatives:

- secondary thread + wrong Transform -> no override, no ownership migration;
- secondary thread + wrong caller -> no override;
- secondary thread + bad stack relation -> no override;
- secondary thread + bad compact proof -> fail-closed as appropriate;
- real lifecycle patch/source-integrity mutation -> hard kill remains;
- Hero/Transform/session/owner/generation/epoch change -> no secondary admission;
- stale token -> no secondary store until same existing c7 recovery rules permit active publication again;
- a later valid primary call cannot revive a genuinely hard-closed session.

### 7. Do not regress c7/c8 protections

Keep:

- exact executable/SHA gate;
- c7 stale suspend/resume semantics;
- c8 detailed observation/source integrity status;
- c8 immutable first hard-kill cause;
- Path A expected-thread/caller/stack/Hero/Transform policy unchanged unless direct evidence requires otherwise;
- Path B compact proof and metadata boundary;
- no direct Actor writes;
- no Position writes;
- no PAGE_GUARD/page protection changes;
- no extra game call merely to keep source ownership alive.

### 8. Optional diagnostic extension

The previous hardware watch stopped at elapsed 1.500 s, while the c8 secondary Path B event occurred at 1.750 s.

If safe and low-risk, extend or add a narrowly bounded Path-B-specific diagnostic that can observe secondary-thread events through approximately 2.5 s. This is optional and must not be required for source ownership. Prefer the explicit Path B hook telemetry above over a broad expensive process-wide watch.

### 9. Human test

Same large A-direction wall.

Perform one continuous ~4 s WallRun.

Report:

- rotation begins after ~0.7 s;
- reaches Surface Up;
- stays Surface Up for the entire WallRun;
- any upright flash/snap around ~1.5–2.0 s;
- crash/Position corruption.

Return complete private WallSprint log.

## PASS criteria

```text
ORIENTATION_RETENTION_PASS=1
no NATIVE_SOURCE_EXIT reason=path_b_wrong_thread
if secondary Path B occurs: PATH_B_SECONDARY_THREAD_EVENT action=override
expected native thread remains unchanged
Path A + primary Path B continue after secondary call
verified Deinit is the clean final close
source_active_until_deinit=1 or equivalent
Position writes=0
```

If no secondary-thread event happens in one human run, the automated native test must still cover the c8 sequence exactly. A real no-event run must at least return to c5-style full-WallRun hold.

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
analysis/V01C8_RESULT.md
analysis/SECONDARY_PATH_B_THREAD.md
analysis/V01C9_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c9_secondary_path_b_thread.zip
```

Follow `AGENTS.md` / `docs/DEV_RULES.md`. Do not commit raw user logs/video or private full captured game-function bytes.