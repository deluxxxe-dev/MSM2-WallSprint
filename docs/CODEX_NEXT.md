# Next task — c41 owned post-return continuation witness feasibility

## Status

C40 completed its review and took the required fail-closed fallback. No bounded production retirement mechanism was established. Production hardware-watch code remains unchanged and the postponed exact-slot watch remains disabled.

Accepted c40 evidence:

- `analysis/V01C40_RETIREMENT_REVIEW.md`
- `analysis/V01C40_GATE.md`
- `analysis/V01C40_RETIREMENT_STATUS.json`
- `docs/CODEX_RESULT.md`

C40 established in the owned Wine fixture that current-thread DR restoration, repeated `GetThreadContext` readbacks, timeout, kill, `inflight=0`, inactive lease bookkeeping and later tombstone recovery do **not** prove that a saved exception CONTEXT has retired. Normal cooperative code running after the actual exception return is a valid witness in the fixture, but no equivalent production witness is available. VCH registration succeeded but its callback was not reached in the tested Wine hardware-breakpoint path, so VCH cannot be promoted to a retirement certificate from current evidence.

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work, read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, all c39/c40 retirement evidence, the existing watch implementation/tests, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Current blocker

The missing fact is a **post-OS-exception-return witness** tied to the exact thread/lease and to every saved exception CONTEXT that could later reapply an owned DR configuration.

C41 must not repeat c40's rejected evidence classes. In particular, do not treat any of these as sufficient by themselves:

```text
positionInflight == 0
cleanup_pending == 0
activeLeases == false
current == -1
finite repeated GetThreadContext readbacks
timeout
kill/shutdown
successful VCH registration
handler/VCH exit counters
later tombstone recovery
```

## Current bounded Codex task

Investigate and, only if supported by evidence, implement a **test-only owned continuation witness** that runs only after the OS has resumed execution from the saved exception CONTEXT.

The primary candidate to evaluate is a minimal context-redirection trampoline in the owned fixture. This is a feasibility/proof task, **not production integration**.

### 1. Test-only scope first

Do not modify the production WallSprint watch engine merely to obtain a green result.

Any continuation/trampoline mechanism must first live in owned test code and operate only on owned threads/functions/memory. It must not touch the game, the static slot, gameplay state, or proprietary function bodies.

If the mechanism cannot be proven safe in the owned fixture, stop fail-closed and document the exact failed guarantee.

### 2. Required continuation property

A successful candidate must establish all of the following:

- the continuation marker is impossible to observe before the OS has actually resumed from the relevant saved exception CONTEXT;
- the marker is bound to the exact thread and watch lease/generation;
- delayed/nested saved contexts cannot be silently collapsed into a single false completion;
- cleanup remains pending until every relevant outstanding context for that lease has an acknowledged post-return witness;
- the witness does not require a second watched-memory access;
- timeout/kill while acknowledgments remain outstanding fails closed rather than declaring final retirement.

A marker written inside VEH/VCH before returning is not sufficient.

### 3. Minimal trampoline candidate

Evaluate a continuation that redirects the saved `RIP` to a minimal owned stub, records a bounded per-context/per-lease completion token, and then transfers control to the original post-exception `RIP`.

Do **not** assume this is safe merely because it works once. Prove or reject the required architectural properties.

Prefer an instruction sequence whose static semantics do not alter GPRs, RFLAGS, stack contents/pointer, vector state or application data except the dedicated owned witness storage. If the implementation requires scratch registers, stack use, code patching, executable allocation, ABI assumptions, unwind assumptions or undocumented behavior, those dependencies must be explicitly proved or the candidate must be rejected.

Account for at least:

- original continuation RIP preservation;
- GPR preservation;
- RFLAGS preservation;
- RSP / stack preservation;
- XMM/YMM/XSTATE non-interference or a justified exact boundary if untouched by construction;
- debug-register restoration semantics;
- thread/lease generation identity;
- multiple pending contexts for one thread/lease;
- nested/re-entrant exception cases within the supported bounded model;
- trampoline/witness storage lifetime until all acknowledgments are retired;
- executable-code identity/lifetime if executable stubs are used;
- CFG/CET/mitigation implications if relevant to the generated or compiled control transfer;
- safe failure when any required guarantee cannot be established.

Do not claim support for architectural state that was not actually preserved/proved.

### 4. Preserve the c39/c40 counterexamples

Keep the existing held-return regression intact. The c41 test must still be able to hold the original exception return past close/timeout/kill.

The key acceptance sequence is:

1. real owned hardware breakpoint occurs;
2. saved exception return is deliberately held;
3. close/timeout/kill is requested;
4. retirement remains pending/refused;
5. release the original saved exception return;
6. the continuation witness runs after that return;
7. only then may that exact pending-context debt be discharged;
8. final restoration remains stable without requiring a later watched hit.

Run this for both reset and Position mode semantics.

### 5. Distinguish feasibility from production authorization

Even if the owned continuation witness succeeds, **do not integrate it into production c39/c40 watch code in this task**.

A c41 success means only that a specific bounded post-return witness mechanism is feasible under the tested owned environment and has a documented preservation contract. A separate next task must review production integration, per-thread lease accounting, actual Windows support and failure policy before c39 slot-watch work can resume.

### 6. Fail-closed outcomes are acceptable

Stop and record the blocker if any of these remain unproven:

- marker ordering relative to actual OS exception return;
- exact thread/lease/context identity;
- full required machine-state preservation;
- nested/multiple-context accounting;
- safe lifetime of trampoline/witness data;
- mitigation/control-transfer compatibility;
- bounded cleanup on timeout/kill/failure.

Do not substitute VCH registration, APC delivery assumptions, repeated snapshots, sleeps or arbitrary delays for a proof.

## Forbidden scope

C41 does not authorize:

- `module_base + 0xC2D2580` RW8/WRITE8 watch;
- slot qword reads/logging or pointer chasing;
- publication/source-chain interpretation;
- producer `0x351EB00` traversal;
- PAGE_GUARD or broad memory interception;
- arbitrary debugger framework expansion;
- Position/source/input/movement/gameplay/adhesion/camera/animation/physics writes;
- weakening debugger/foreign-DR refusal, lease identity, caps, deadlines, restoration or tombstone behavior.

Runtime remains `v0.1c38 / package0.1.56` unless the task produces an independently justified test-only metadata change. Do not create a game release for c41.

## Required validation

At minimum run:

```bash
bash source/tools/test_watch_retirement.sh
bash source/tools/build_windows.sh
bash source/tools/test_hooks_windows.sh
```

If new focused test tooling is added, run its exact command separately and include it in the report.

For a **successful feasibility result**, expected evidence includes:

- held-return close/timeout/kill remains pending before post-return witness;
- a real post-return continuation witness is observed after release;
- exact per-context/per-lease debt is discharged only after that witness;
- retired DR configuration does not resurrect afterward;
- no later watched hit is needed for repair;
- reset and Position restoration policies remain unchanged;
- required machine state is preserved by static proof plus owned runtime checks appropriate to the implemented mechanism;
- existing watch ownership/refusal regressions remain PASS.

For a **fail-closed result**, the report must name the exact missing guarantee and must not claim a retirement fix.

Do not report an unexecuted native-Windows/game test as PASS. If Wine cannot establish a required Windows property, identify a minimal standalone real-Windows owned probe as the next evidence instead of requesting a game test.

## Required outputs / handoff

Update as appropriate:

- owned test implementation only for the continuation witness feasibility work;
- `analysis/` with mechanism, instruction/control-flow contract, evidence class, assumptions, limitations and rejection reasons;
- `docs/PROJECT_STATE.md` to record c40 fail-closed and c41 as the current feasibility stage;
- `docs/CODEX_RESULT.md`;
- relevant Wisdoms facts if a reusable exception-return/continuation rule is established.

Commit and push the intended branch before claiming completion.

The final report must follow `docs/CODEX_REPORTING.md` and include:

- what changed;
- files changed;
- exact tests/checks performed;
- exact commands used;
- expected result;
- actual result;
- unperformed tests and reasons;
- branch and commit SHA;
- whether owner action is required.

## Owner action

No Update-WS, game launch, A/E comparison, WallRun capture, offline slot scan or game runtime log is required for the c41 feasibility work itself.

If the owned environment cannot establish a property that specifically requires native Windows, the only acceptable owner request is a **minimal standalone owned Windows probe** with one exact command and a compact machine-readable result. Do not require launching Spider-Man 2 merely to answer an OS exception-return ordering question.
