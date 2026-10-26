# Next task — c40 bounded saved-exception CONTEXT retirement

## Status

C39 stopped fail-closed before adding the exact-slot watch. The owned Wine DR/VEH regression reproduced a retirement gap: reset and Position cleanup can report no active lease / no pending cleanup / inflight0 while a delayed exception return still reapplies the retired DR configuration from its saved exception CONTEXT.

Accepted evidence:

- `analysis/V01C39_WATCH_RETIREMENT.md`
- `analysis/V01C39_GATE.md`
- `analysis/V01C39_WATCH_STATUS.json`
- `docs/CODEX_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before implementation read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, this file, the c39 retirement evidence above, the existing hardware-watch/reset/Position implementation, and relevant references in `deluxxxe-dev/MSM2-Wisdoms` when accessible.

## Evidence boundary

The following is VERIFIED in the owned Wine fixture, not in the actual game or real Windows:

1. A real hardware breakpoint enters the existing production VEH handler.
2. The production handler finishes and its existing inflight bookkeeping can reach zero.
3. Before OS exception return is allowed to complete, the coordinator can restore current thread DR state, close the lease, and report cleanup complete.
4. Releasing the delayed exception return can then reapply the old DR configuration stored in the saved exception CONTEXT.
5. A later hit can recover through the existing tombstone path, but that later hit is not a bounded retirement guarantee during silence.

Do not promote this fixture result to a claim that the game or real Windows has exhibited the race. It is sufficient to reject the existing cleanup bookkeeping as a certificate for c39 phase transition / serialization.

## Current blocker

The existing engine does not currently prove that every saved exception CONTEXT capable of reapplying an owned debug-register configuration has retired before the configuration is considered fully closed.

Therefore these conditions are NOT sufficient retirement proof by themselves:

```text
cleanup_pending=0
activeLeases=false
current=-1
positionInflight=0
```

A check performed only inside or immediately before returning from VEH is also not sufficient, because the OS exception-return step occurs afterward.

## Current bounded Codex task

Design and implement the minimum bounded mechanism needed to retire owned saved exception CONTEXT state safely.

The mechanism must keep cleanup / serialization logically pending until an old saved exception CONTEXT can no longer reapply a retired owned DR configuration, or must fail closed if that condition cannot be established.

Do not assume a particular Windows/Wine API guarantee unless it is supported by the existing implementation, documentation available to the environment, or a focused owned test. Record any OS-behavior assumptions explicitly.

### 1. Preserve ownership and restoration semantics

Do not weaken or bypass:

- debugger-present refusal;
- foreign DR ownership/refusal;
- exact saved debug-register restoration policy for Position;
- existing legacy reset DR6 policy;
- thread suspend/resume error handling;
- lease identity checks;
- tombstone safety behavior;
- event/window/thread/deadline caps;
- timeout/kill/failure cleanup.

Do not forge thread CONTEXT, lease state, inflight state, or exception state in production code merely to make the regression pass.

### 2. Keep scope limited to retirement

This task does NOT authorize the postponed c39 exact-slot watch.

Do not add or enable:

- the `module_base + 0xC2D2580` RW8/WRITE8 watch mode;
- slot qword reads/logging;
- pointer chasing;
- new value/GPR/XMM/stack capture aimed at the slot;
- PAGE_GUARD or broad memory interception;
- arbitrary detours/debugger framework expansion;
- publication-chain interpretation;
- producer `0x351EB00` traversal;
- Position/source/input/movement/gameplay/adhesion/camera/animation/physics writes.

The c39 non-value slot-watch acceptance matrix remains deferred until this retirement prerequisite is proven.

### 3. Use the existing delayed-return regression as the primary acceptance test

Preserve and extend `source/src/NativeHooks/watch_retirement_smoke.cpp` and its test path rather than deleting, bypassing, weakening, or timing-out the counterexample.

Required acceptance behavior for both existing reset and Position modes:

- the first real owned hardware-breakpoint event still occurs;
- the test can still hold OS exception return after the production handler has returned;
- while a saved exception CONTEXT can still reapply an old owned DR configuration, production cleanup/serialization must not claim final retirement;
- after the new bounded retirement condition is satisfied, releasing/finishing the held return must not resurrect a retired active watch configuration;
- saved foreign/owned DR state must be restored according to the existing mode-specific policy;
- no new capture is required to repair an already-declared-retired configuration;
- failure to establish retirement within the bounded contract must close/refuse safely rather than silently declaring success.

Do not replace the delayed-return test with a weaker immediate-return test.

### 4. Add focused failure-path coverage

Add the minimum owned tests needed to cover the new retirement mechanism, including where applicable:

- normal close after a completed exception return;
- close while an exception return is deliberately held;
- timeout/failure while retirement is pending;
- kill/forced close path if the existing engine exposes one in owned tests;
- reset mode restoration policy;
- Position exact saved-DR restoration policy;
- foreign DR/debugger refusal unchanged;
- no overlap/regression in existing watch ownership tests;
- repeated arm/close cycles if necessary to prove stale state does not survive into a later lease.

Do not weaken existing assertions, deadlines, caps, or cleanup checks to obtain PASS.

### 5. Fail closed if bounded retirement cannot be proven

If a safe bounded mechanism cannot be established without relying on an unverified OS guarantee or weakening the current ownership model:

- do not approximate;
- do not add the c39 slot mode;
- document the exact unresolved retirement condition;
- keep runtime at v0.1c38/package0.1.56 unless an independently justified test-only version/documentation change requires otherwise;
- update the handoff with the precise next evidence needed.

## Unchanged proof gates

The accepted c38/c37/c36 state remains unchanged:

```text
slot_rva=0xC2D2580
writer_candidates=0
writer_enumeration_complete=0
publication_source_kind=unproven
allocation_proven=0
range_proven=0
lifetime_proven=0
base_proven=0
destination_contained=0
CFG_continuation=0
```

Zero local writers is not no-writer proof. The c28 external reader remains a reader only.

Safety remains:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

## Required validation

At minimum run:

```bash
bash source/tools/test_watch_retirement.sh
```

Also run the normal mandatory Windows/native regression path required by the repository for any production hardware-watch change. If the implementation changes only test/documentation code, still run the focused retirement test and the smallest repository-mandated regression set that covers the touched watch code.

Expected result:

- delayed-return regression no longer permits final cleanup/retirement while an old saved exception CONTEXT can resurrect the retired owned watch;
- after bounded retirement completes, the old configuration does not reappear;
- reset and Position restoration semantics remain unchanged;
- existing watch ownership/refusal regressions pass;
- no c39 slot-watch functionality is enabled.

If any required test is not run, state the exact reason. Do not report an unexecuted test as PASS.

## Required outputs / handoff

Update as appropriate:

- the minimum production/test implementation needed for bounded retirement;
- `analysis/` with the retirement mechanism, evidence class, assumptions and limitations;
- `docs/PROJECT_STATE.md` so c39 is recorded as stopped fail-closed and c40 retirement is the current stage;
- `docs/CODEX_RESULT.md`;
- relevant `deluxxxe-dev/MSM2-Wisdoms` reusable facts if a general watch/retirement rule is established.

Commit and push the intended branch before claiming completion.

The final Codex report must include, concisely:

- what changed;
- files changed;
- exact tests/checks performed;
- exact commands used;
- expected result for each relevant test;
- actual result for each relevant test;
- any unperformed test and the reason;
- branch and commit SHA;
- whether any owner action is required.

## Owner action for this task

No Update-WS, game launch, A/E comparison, WallRun capture, offline slot scan, or runtime log is requested merely to develop/test this owned retirement prerequisite.

Only request owner-side testing later if the implementation produces a specific real-Windows/game question that cannot be answered by the owned regression and repository tests.
