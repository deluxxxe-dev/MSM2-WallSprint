# Next task — c48 production continuation-witness integration review

## Status

C47 is complete as a repository/test task and the owner has now run the published native-Windows standalone probe.

Accepted native result:

- `analysis/V01C47_NATIVE_WINDOWS_RESULT.md`
- `source/analysis/V01C47_NATIVE_WINDOWS_RESULT.md`
- `analysis/V01C47_CONFIGURATION_RETIREMENT.md`
- `analysis/V01C47_STATUS.json`
- `analysis/native-probes/c47/README.md`
- `analysis/native-probes/c47/PROBE_MANIFEST.json`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, all c39-c47 retirement/witness evidence, the current production watch implementation, the c41-c47 standalone implementation/tests, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted c47 native evidence

Owner command:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Observed native identity/result:

```text
result                         = OWNED_WITNESS_CONFIGURATION_PASS
environment                    = native_Windows
mode / scenario                = Position / multiple
cpu_vendor                     = GenuineIntel
mitigation_flags               = [0,0,0x100]
mitigation_classifier          = ADMIT
contexts                       = 3
watch_context_writes_observed  = 1
outer_hardware_DB_events       = 1
RAW_EXACT                      = false
ARCHITECTURAL_CANONICAL        = false
RETIREMENT_CONFIGURATION_SAFE  = true
acceptance_comparison          = RETIREMENT_CONFIGURATION_SAFE
exact_slot_watch_enabled       = false
production_integrated          = false
game_used                      = false
native_candidate_probe_passed  = true
retirement_accepted            = true
```

The c47 predicate was `c47-v1` / profile
`Intel_SDM253669_090US_DR6_owner_c46_Position_multiple_v1`. It accepted the exact owner-native c46 activated-state history, not a generic DR6 mask.

Additional native facts:

```text
ticket_count                       = 3
all tickets acknowledged           = true
all tickets current_code           = true
same worker/lease/generation       = true
pending before/after final poll    = 0 / 0
canRetire before final poll        = true
active leases after final poll     = false
final poll returned                = false
total context write attempts       = 2
final-poll restore write attempts  = 1
unscoped context write attempts    = 0
failed context write attempts      = 0
```

Dr0/Dr1/Dr3 restored exactly and the synthetic foreign Dr2 was preserved exactly. All three final readbacks were complete/stable. Historical `RAW_EXACT=false` and c44 `ARCHITECTURAL_CANONICAL=false` remain preserved and were not relabeled.

The c46 pre-access timeline was reproduced: Dr6[11,16] changed 0/0 -> 1/1 after watch-enabled Resume but before watched access, machine entry, ticket creation, pending debt or owned #DB. C47 accepts only the exact measured full-status/configuration profile plus all original ownership/ticket/lifetime checks.

## What c47 proves — and what it does not

### Accepted

The owned standalone candidate can, on the measured native-Windows/GenuineIntel/Position-multiple profile, distinguish breakpoint configuration retirement from the observed DR6 status representation and can safely retire that **owned fixture** according to `RETIREMENT_CONFIGURATION_SAFE/c47-v1`.

This closes the specific c41-c47 standalone feasibility chain.

### Still unproven

C47 is explicitly not production integration. It does not prove:

- that every production saved exception `CONTEXT` is discovered/registered;
- arbitrary game-thread and exception interleavings;
- nested/reentrant production VEH cases beyond the owned fixture;
- production thread exit while a ticket or saved context remains outstanding;
- deinit/kill/timeout/module-unload lifetime across all pending continuations;
- a process-wide bounded context cap/accounting rule;
- safe executable-stub/code/data lifetime in the real game process;
- mitigation/CET/CFG compatibility of the actual game process merely because the standalone process passed;
- full production XSTATE/architectural preservation for arbitrary game threads;
- permission to enable the deferred exact-slot watch at `module_base + 0xC2D2580`.

The original c39/c40 delayed saved-`CONTEXT` resurrection counterexample remains authoritative until a production-shape integration proof closes these gaps.

## Current blocker

Determine whether the c41-c47 continuation-witness mechanism can be integrated into the **existing production watch engine** with complete, bounded and fail-closed saved-context accounting, without changing gameplay behavior and without enabling the deferred exact-slot watch.

C48 is a **production-integration review**, not production integration itself.

## Current bounded Codex task

### 1. Audit the exact production exception lifecycle

Trace the current production watch implementation from watch installation through every owned/foreign/mixed exception path and every cleanup/retirement path.

At minimum map:

- watch install and immutable lease publication;
- suspend/GetThreadContext/SetThreadContext/readback/Resume ordering;
- VEH entry and ownership matching;
- normal owned hit;
- closed/deadline/kill hit;
- event-cap/budget/lock/ring-overflow paths;
- mixed identity / foreign ownership loss;
- reset and Position modes;
- multithread expansion;
- final cleanup / pollReset / deinit;
- timeout/kill while handlers or saved contexts exist;
- thread exit / invalid handle / Resume failure;
- nested or repeated owned exceptions before an earlier saved context has fully returned;
- any path that can return `EXCEPTION_CONTINUE_EXECUTION` with a saved `CONTEXT` capable of reapplying owned DR state.

Produce a table of every such return path, the exact lease/thread/window identity available there, and whether a continuation ticket could be registered **before** that saved context becomes a future retirement debt.

Do not infer completeness from the c47 fixture. Prove it from current production code plus bounded owned tests, or report the first exact blocker.

### 2. Define a production-safe ticket/accounting contract

Design, but do not yet integrate, the minimum production contract needed to discharge the c39/c40 counterexample.

It must specify at minimum:

- immutable ticket identity: thread + lease/window generation + context sequence;
- the exact moment a ticket is created relative to saved-`CONTEXT` mutation/return;
- how nested/multiple outstanding contexts on one thread are represented;
- maximum bounded tickets per thread/process and what happens on cap uncertainty;
- no heap allocation, loader interaction, logging, user locks or unbounded work in the exception path;
- how ticket storage is published atomically and read by cleanup;
- how code/data lifetime is retained until every possible continuation witness is discharged;
- how thread exit discharges or poisons outstanding debt;
- how deinit/kill/timeout behaves when debt remains;
- how module unload/process teardown avoids executing freed witness code;
- how stale lease/window generations are rejected;
- how mixed/foreign DR ownership remains fail-closed;
- how context-registration failure poisons retirement rather than silently dropping debt.

If any required production path cannot obtain a unique immutable identity or cannot register debt without unsafe work in VEH context, the review must fail closed.

### 3. Audit executable continuation-stub feasibility in the real production process

The c41-c47 standalone stub is not automatically a production-game certificate.

Review the exact production requirements for:

- executable memory ownership and lifetime;
- W^X / page protection transitions if any;
- CFG/CET/shadow-stack/EH-continuation interactions;
- current game-process mitigation queries required before use;
- whether the current 15-byte continuation form is position/range safe for arbitrary production thread RIPs;
- whether the continuation preserves every required GPR/RFLAGS/RSP/stack/vector/XSTATE property for production use;
- instruction-cache coherency and publication ordering;
- abrupt thread/process/module teardown.

Do not assume the actual game-process mitigation tuple equals the standalone `[0,0,0x100]` result. Define a runtime admission rule that would query the real process and fail closed on unknown/relevant settings.

Do not disable or relax Windows mitigations.

### 4. Build a test-only production-shape accounting model/harness

If the static review yields a coherent bounded contract, add test-only owned coverage that models the **production shapes**, not only the original c41 worker.

At minimum cover:

- one context / one thread;
- multiple saved contexts on one thread with out-of-order scheduling;
- multiple threads, each with independent debt;
- delayed return after cleanup request;
- close/deadline/kill while debt remains;
- nested/reentrant owned exception registration;
- lease/window generation reuse attempt;
- context/ticket cap exhaustion;
- thread exit with zero debt;
- thread exit with unresolved debt;
- deinit/module-lifetime boundary with outstanding debt;
- mixed/foreign ownership;
- failed ticket publication / unknown identity;
- stale/duplicate witness token;
- witness for one context must not discharge another;
- all debt discharged then cleanup allowed;
- original c39/c40 delayed-return resurrection must remain reproduced when the witness mechanism is absent/disabled.

Prefer reusing production structs/helpers where safe so the harness cannot drift into an unrelated abstract model. If exact production logic cannot be exercised without modifying production behavior, document that limitation explicitly.

Synthetic/model PASS is not a production certificate.

### 5. Decide the c48 review result — do not integrate yet

Emit one of:

```text
PRODUCTION_WITNESS_INTEGRATION_REVIEW_PASS
PRODUCTION_WITNESS_INTEGRATION_REVIEW_FAIL
```

A PASS requires a complete mapped return-path inventory, a bounded no-drop ticket/accounting contract, safe executable/code-data lifetime design, real-process mitigation admission design, and passing production-shape owned tests.

A PASS authorizes only a later explicit implementation task (c49 or later). It does **not** authorize modifying production code inside c48 and does not authorize the exact-slot watch.

A FAIL must state the first exact missing guarantee and preserve all current production behavior unchanged.

### 6. Preserve every historical predicate/result

Do not weaken, replace or relabel:

- c39/c40 delayed saved-context resurrection counterexamples;
- c41 continuation witness semantics;
- c42 mitigation classifier;
- c43 exact restoration diagnostic;
- c44 `RAW_EXACT` / `ARCHITECTURAL_CANONICAL` reporting;
- c45/c46 lifecycle observations;
- c47 `RETIREMENT_CONFIGURATION_SAFE` bounded owner predicate.

C47 native PASS remains fixture-specific. Do not turn it into a generic DR6 ignore rule.

### 7. Forbidden scope

C48 does not authorize:

- production continuation-witness integration;
- changing production `restore()`, ownership, install, handler, lease or cleanup semantics;
- enabling any new hardware watch mode;
- the deferred `module_base + 0xC2D2580` exact-slot RW8/WRITE8 watch;
- slot qword reads/logging or pointer chasing;
- publication/source-chain interpretation;
- producer `0x351EB00` traversal;
- PAGE_GUARD or broad memory interception;
- mitigation disabling/relaxation;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38 / package0.1.56`. Do not create a game release for c48.

## Required validation

Run from repository root:

```bash
bash source/tools/test_continuation_mitigations.sh
bash source/tools/test_continuation_diagnostics.sh
bash source/tools/test_debug_register_comparison.sh
bash source/tools/test_dr6_lifecycle.sh
bash source/tools/test_pre_access_lifecycle.sh
bash source/tools/test_retirement_configuration.sh
bash source/tools/test_continuation_witness.sh
bash source/tools/test_watch_retirement.sh
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

Run every new c48 production-shape/accounting test explicitly and report the exact command.

Expected repository result:

- c47 native PASS is preserved as bounded owner-fixture evidence;
- all production exception-return paths are inventoried or the first exact coverage blocker is identified;
- a bounded no-drop ticket/lifetime/admission design exists or the review fails closed;
- c39/c40 counterexamples remain intact;
- no production behavior or exact-slot watch is changed;
- no game release is produced.

If any required test is not run, state the exact reason. Do not report unexecuted native-Windows/game tests as PASS.

## Required outputs / handoff

Update as appropriate:

- `analysis/V01C48_PRODUCTION_WITNESS_REVIEW.md`;
- `analysis/V01C48_GATE.md`;
- `analysis/V01C48_STATUS.json`;
- test-only production-shape accounting helpers/tests if justified;
- `docs/PROJECT_STATE.md` so c47 native PASS is historical/accepted and c48 is current;
- `docs/CODEX_RESULT.md`;
- relevant Wisdoms documentation only for truly reusable general debugging/safety rules.

Do **not** create `analysis/native-probes/c48` unless the review discovers a specific OS-level native question that cannot be answered from c41-c47 evidence and owned repository tests. If such a probe is necessary, explain the exact missing guarantee first and keep it game-free.

Commit and push the intended branch before claiming completion.

The final report must follow `docs/CODEX_REPORTING.md` and include:

- exact tests/checks performed;
- exact commands;
- expected result;
- actual result;
- PASS / SKIP / UNAVAILABLE separately;
- all unperformed tests and reasons;
- branch and commit SHA;
- exact c48 review result;
- whether owner action is required.

## Owner action

No Update-WS, game launch, A/E comparison, WallRun capture, offline slot scan or game log is required for the c48 review by default.

If c48 reaches `PRODUCTION_WITNESS_INTEGRATION_REVIEW_PASS`, stop there and return the report. A later explicit task will decide whether to implement production integration. Do not proceed directly to exact-slot work.
