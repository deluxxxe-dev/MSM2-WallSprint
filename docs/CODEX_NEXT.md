# Next task — c52 production-shape VCH witness integration review

## Status

C51 repository work is complete and the owner ran the published native-Windows standalone probe.

Accepted native result:

- `analysis/V01C51_NATIVE_WINDOWS_RESULT.md`
- `source/analysis/V01C51_NATIVE_WINDOWS_RESULT.md`
- `analysis/V01C51_VCH_POST_RETURN_WITNESS.md`
- `analysis/V01C51_STATUS.json`
- `analysis/native-probes/c51/README.md`
- `analysis/native-probes/c51/PROBE_MANIFEST.json`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, all c39-c51 retirement/witness evidence, current production `orientation_trace.cpp`, c48 production review, c49-c51 SEARCH/VCH harnesses, the reviewed c41 stub helpers, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted c51 native evidence

Owner command:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

Primary result:

```text
VCH_POST_RETURN_WITNESS_BRIDGE_FEASIBILITY_PASS
reason = native_exact_one_debt_post_return_token_then_configuration_cleanup
```

Bounded native facts:

```text
environment                    = native_Windows
cpu_vendor                     = GenuineIntel
mode/scenario                  = Position/close
mitigation tuple               = [0,0,0x100], ADMIT
WS disposition                 = CONTINUE_SEARCH
downstream disposition         = CONTINUE_EXECUTION
VCH registered/callbacks       = true / 1
c50 unique matching debt       = true
VCH changed context fields     = [Rip] only
exact witness token            = observed
pending debt                   = 1 -> 0 after token only
cleanup requested before token = true
live final cleanup             = after token only
second watched accesses        = 0
resurrection after cleanup     = false
machine state preserved        = true
```

The reviewed c41 continuation form was prepared/published outside VCH, then the authenticated VCH redirected only RIP. The exact token acknowledged only the bound thread/creation-epoch/context/exception/lease/window/generation/sequence/original-RIP debt. Final cleanup and three stable debug-register readbacks followed without a second watched access.

Historical RAW_EXACT/c44 canonical comparisons remain false. The separate c51 same-event configuration contract passed; no DR repair/masking/normalization was introduced. Test-owned stub/marker storage remained allocated until actual worker exit. Marker acknowledgment did not by itself authorize storage reclamation.

C51 is **not production integration**. It proves one owned Position/close debt shape only.

## Current blocker

Determine whether the c51 bridge primitive can be turned into a complete, bounded, no-drop **production-shape design** covering every production saved-context obligation before any production code is modified.

C48 previously failed because modified saved contexts could escape through `CONTINUE_SEARCH`, accounting was incomplete, code lifetime was unresolved, and the c41 fixture assumptions did not cover production. C49-c51 close one specific SEARCH/VCH/one-debt path, but do not establish all-path registration, concurrent debt bounds, callback ordering guarantees, executable-stub availability, module lifetime, or actual game-process admission.

C52 is a review/design task. **Do not integrate into production in c52.**

## Current bounded task

### 1. Re-audit every production exception/return path using c51 as a primitive

Start from the c48 return-path inventory and current `orientation_trace.cpp`. For every path that can carry or later apply an owned saved `CONTEXT`, state:

- entry/terminal disposition (`CONTINUE_SEARCH` / `CONTINUE_EXECUTION`);
- whether production mutates owned Dr0/Dr1/Dr6/Dr7 fields before returning;
- exact thread/lease/window/generation identities available;
- whether one or multiple lease obligations can be present;
- earliest safe debt-reservation point;
- whether the final continuation context is later observable/authenticatable in VCH;
- whether a prepared continuation target can be available before VCH returns;
- what cleanup/deinit/thread-exit behavior applies while debt is outstanding.

Include normal owned hit, mixed cause, partial ownership, closed/deadline/kill, cap/budget/lock/ring-drop, reset/Position, multithread expansion, nested/reentrant handler entry, stale/tombstone lease, thread exit/invalid handle/resume failure, and all c48 S/C rows.

A diagnostic/event drop is never permission to drop return debt.

If any reachable production obligation cannot be reserved/authenticated without ambiguity, identify the first exact blocker and fail closed.

### 2. Define a bounded production debt ledger contract

Design, but do not integrate, a fixed-capacity pre-reserved ledger whose states distinguish at least:

```text
FREE
RESERVED
PUBLISHED
FINAL_CONTEXT_BOUND
RIP_REDIRECTED
DEBT_ACKNOWLEDGED
EXECUTION_QUIESCENT_OR_THREAD_EXIT
RECLAIMABLE
UNKNOWN_POISON
```

Requirements:

- stable thread handle/creation epoch, lease/window/session/generation, context sequence and obligation set;
- reserve before a saved context can escape WS or cleanup can observe zero;
- reserved-but-not-yet-published entries count as debt;
- nested/multiple debts on one thread are independent;
- multiple threads are independent;
- mixed contexts can bind all relevant owned lease obligations without collapsing identities;
- fixed process/thread caps are proven before arming; exhaustion/publication uncertainty becomes sticky `UNKNOWN_POISON` and blocks retirement/re-arm;
- no heap allocation, loader operation, formatting/logging, user lock, diagnostic ring/event lock or unbounded work in VEH/VCH registration/binding;
- duplicate/stale/wrong identity/token cannot discharge another debt;
- timeout/kill/deinit cannot silently clear debt;
- actual thread exit may terminate execution debt only through a retained signaled thread handle/epoch and must still respect readers/code lifetime;
- marker acknowledgment and stub-code quiescence remain separate.

Do not use global `usedLeases==1`; preserve c50 unique worker/obligation identity semantics and refuse ambiguous matches.

### 3. Audit production VCH ordering/coverage as an explicit admission requirement

C51 proves one real native VCH observation, not universal callback ordering.

Using current Microsoft documentation plus bounded owned tests where possible, determine:

- whether every relevant production continuation path is documented/observed to dispatch VCH after downstream finalization;
- how multiple VCH registrations are ordered;
- whether another VCH can run after WallSprint and alter the redirected context;
- whether WallSprint can establish a supported last/terminal mutation boundary, or must refuse when that cannot be guaranteed;
- what the VCH return value does/does not guarantee for subsequent handlers;
- how VCH registration/removal and DLL/module unload interact with outstanding callbacks/debts.

Do not infer "last callback" from one native c51 run. If the API contract cannot guarantee a required ordering property, state whether a safe fail-closed admission rule exists. If no observable/admissible rule can close it, this is a review blocker.

### 4. Audit production stub availability without blocking/allocation inside VCH

C51 deliberately held VCH while a coordinator prepared the stub. That is allowed only in the test fixture and is not automatically production-safe.

Design or reject a production-ready preparation strategy that ensures a fully validated continuation target is already published before VCH must return.

Audit at minimum:

- whether original RIP is known early enough to prepare outside VCH;
- preallocated executable/marker pools and W^X transitions;
- signed-32-bit reach for both marker store and original-RIP jump;
- coverage for all admitted production RIP/module ranges;
- finite slot capacity and per-debt ownership;
- current-code/target revalidation at bind time;
- instruction-cache publication ordering;
- no allocation/Capstone/page-protection transition while VCH is on the critical path unless explicitly proven safe/supported;
- failure after a debt exists must poison/retain debt, not drop it;
- no live slot reuse merely because marker was observed.

If arbitrary admitted production RIPs cannot be served by a bounded prevalidated pool, define the exact admission/refusal boundary rather than assuming near allocation will succeed later.

### 5. Audit actual game-process admission and architectural preservation design

Do not assume standalone `[0,0,0x100]` equals the game process.

Define the production admission checks required before the first witness-capable watch is armed:

- actual process DynamicCode/CFG/UserShadowStack and any additional relevant continuation/EH policies;
- supported CPU/OS/XSTATE prerequisites;
- debugger/foreign-DR ownership rules;
- exact-build/current-code identity;
- required GPR/RFLAGS/RSP/stack/vector/XSTATE preservation scope;
- module/VEH/stub code/data residency and unload/deinit quiescence.

Unknown/relevant mitigation or unsupported XSTATE/lifetime condition must refuse. Do not disable/relax mitigations.

### 6. Build a test-only production-shape ledger/bridge review harness only if the design is coherent

Prefer reusing production structs/helpers and the c51 bridge helpers without modifying production behavior.

Cover at minimum:

- one EXECUTION debt and one mixed SEARCH debt;
- multiple outstanding contexts on one thread, including out-of-order return/witness;
- multiple threads with independent debts;
- nested/reentrant registration;
- multiple owned lease obligations in one saved context;
- unrelated global leases with one unique current obligation;
- close/deadline/kill while debt remains;
- context/ticket/stub pool cap exhaustion;
- reservation/publication failure and sticky poison;
- VCH missing, duplicate, reordered, or final-context mismatch;
- another simulated later VCH mutation => refuse unless a documented terminal boundary exists;
- wrong/stale/duplicate token;
- acknowledgment of A must not discharge B;
- marker observed while stub execution is not quiescent;
- worker exit with/without debt;
- deinit/module-lifetime boundary with debt/stub readers;
- all debt acknowledged plus quiescence/thread-exit => reclaimable;
- original c39/c40/c48 counterexamples remain reproduced when witness support is absent/disabled.

Pure/model PASS is not production or native-game evidence.

### 7. Decide c52 review result — no production integration yet

Emit exactly one:

```text
PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_PASS
PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL
```

PASS requires all of:

- complete production return-path/debt inventory;
- bounded no-drop ledger contract;
- supported VCH ordering/coverage admission design;
- bounded no-block/no-allocation VCH stub-availability design;
- explicit game-process mitigation/XSTATE/lifetime admission design;
- passing production-shape owned/model tests with all historical counterexamples preserved;
- no unresolved path that can silently escape or discharge debt.

PASS authorizes only a later explicit production implementation task (c53 or later). It does not modify production code in c52 and does not authorize exact-slot watch work.

FAIL must state the first exact missing guarantee and leave production unchanged. A failed review is a valid completion result.

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
bash source/tools/test_production_witness_review.sh
bash source/tools/test_vch_search_bridge.sh
bash source/tools/test_vch_search_identity.sh
bash source/tools/test_vch_post_return_witness.sh
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If c52 adds a focused review/harness command, execute and report that exact command separately.

Expected result:

- c51 native PASS is preserved as bounded one-debt evidence;
- production remains unchanged;
- c39/c40/c48 counterexamples remain intact;
- all unresolved production assumptions are either closed by a bounded auditable contract or cause review FAIL;
- no exact-slot/gameplay work is enabled.

## Required outputs / handoff

Update as appropriate:

- `analysis/V01C52_*` review/gate/status evidence;
- any test-only production-shape model/harness and focused tests;
- `docs/PROJECT_STATE.md` so c51 native PASS is accepted and c52 is current;
- `docs/CODEX_RESULT.md`;
- relevant `MSM2-Wisdoms` reusable rules, especially the c51 distinction between exact post-return debt acknowledgment and later code/storage quiescence.

Commit and push the intended branch before claiming completion.

The final report must follow `docs/CODEX_REPORTING.md` and include:

- every executed test/check;
- exact command;
- expected result;
- actual result;
- PASS / SKIP / UNAVAILABLE / BLOCKED separately;
- every unperformed test and reason;
- branch and commit SHA;
- whether owner action is required.

## Forbidden scope

C52 does not authorize:

- production continuation-witness integration;
- production `orientation_trace.cpp` behavior changes;
- production restore/ownership/install/handler/lease/cleanup changes;
- broad DR6 masking/normalization;
- live-thread stub reuse without a separate quiescence proof;
- the deferred `module_base + 0xC2D2580` exact-slot watch;
- slot qword reads/logging or pointer chasing;
- publication/source-chain interpretation;
- producer `0x351EB00` traversal;
- PAGE_GUARD or broad memory interception;
- mitigation disabling/relaxation;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38 / package0.1.56`. Do not create a game release for c52.

## Owner action

No owner action is required for the c52 repository review unless Codex establishes a narrowly defined native OS-level question that cannot be answered from the accepted c51 evidence. Do not request Update-WS or game launch for review-only work.