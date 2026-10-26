# Next task — c49 VCH bridge feasibility for modified CONTINUE_SEARCH contexts

## Status

C48 production continuation-witness integration review is complete and failed closed.

Accepted c48 result:

```text
PRODUCTION_WITNESS_INTEGRATION_REVIEW_FAIL
```

Accepted implementation/report head before this task:

```text
branch = codex/v01c20-edi-value-provenance
commit = 3fc10cd567ee569102295ee6176df866f5891b7b
```

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, all c39-c48 retirement/witness evidence, current production `orientation_trace.cpp`, current c40 VCH probe, c41-c47 continuation-witness implementation/tests, c48 production-review harness, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted c48 blocker

C48 established a new owned Wine counterexample in addition to c39/c40.

A real mixed hardware debug event can contain:

```text
owned B0 + foreign B2
```

Current production code may:

1. authenticate and claim owned B0;
2. clear B0 in the saved exception `CONTEXT`;
3. leave foreign B2 for another owner;
4. return `EXCEPTION_CONTINUE_SEARCH`;
5. allow a following handler to eventually return `EXCEPTION_CONTINUE_EXECUTION`;
6. later apply that saved `CONTEXT` after WallSprint coordinator cleanup has restored the thread's current DRs and marked the lease inactive.

Across reset/Position x close/deadline/kill, c48 reproduced owned-slot resurrection after the downstream handler released the saved context. Two foreign-only unchanged-context controls did not resurrect on that Wine path and remain separate evidence.

Therefore the first missing production guarantee is not merely "create a ticket on CONTINUE_EXECUTION". A modified saved `CONTEXT` can escape WallSprint through `CONTINUE_SEARCH` and still become a future return debt.

## Why entry reservation alone is insufficient

A future design may need to reserve debt before such a saved context escapes, but reservation by itself does not provide a post-return witness.

After WallSprint returns `CONTINUE_SEARCH`, a downstream VEH/SEH owner may inspect or modify the same exception `CONTEXT` before choosing continuation. Redirecting RIP to a WallSprint witness before passing the context downstream could alter foreign-handler semantics and is not authorized by current evidence.

C49 therefore asks a narrower question first:

> Is there a supported, observable Windows continuation boundary after the downstream handler has chosen continuation and finalized the context, but before the OS resumes the interrupted thread, where a previously reserved owned debt can be bound to the final context and redirected to the already-reviewed c41-style post-return witness?

The primary candidate is a vectored continue handler (VCH), because Windows exposes `AddVectoredContinueHandler`. Documentation confirms registration/ordering controls for VCH, but the exact hardware-#DB dispatch/order/final-context behavior needed here is not sufficiently established by documentation alone.

Historical c40 evidence must remain intact:

- Wine VCH registration succeeded;
- callback count remained zero on the owned hardware-watch path;
- those cases are `UNAVAILABLE`, not PASS and not proof that VCH cannot work on native Windows.

## Current bounded task

C49 is a **test-only VCH/search-continuation feasibility task**. It does not authorize production integration.

### 1. Audit the exact VCH contract and preserve uncertainty

Review current Microsoft documentation/source for:

- `AddVectoredContinueHandler` / `RemoveVectoredContinueHandler`;
- vectored exception/continue ordering;
- `PVECTORED_EXCEPTION_HANDLER` context mutation/return semantics;
- DLL unload/registered-handler lifetime;
- any documented relationship between a downstream handler's `EXCEPTION_CONTINUE_EXECUTION` decision and subsequent VCH dispatch.

Record exact source/version/provenance.

Do not infer undocumented ordering. In particular, documentation that a VCH can be registered is not proof that it runs for this hardware-#DB path, sees the downstream-finalized context, or is the last callback before OS context application.

If documentation alone cannot establish those facts, mark them runtime questions rather than assumptions.

### 2. Build a test-owned mixed SEARCH -> downstream EXECUTION -> VCH probe

Reuse the c48 real hardware-debug shape as closely as practical while keeping production source unchanged.

The owned test sequence must include:

1. an owned hardware watch corresponding to B0;
2. a synthetic foreign hardware watch corresponding to B2;
3. a WallSprint-like first handler that runs the unmodified production engine, claims/clears only B0, preserves B2 and returns `CONTINUE_SEARCH`;
4. a separate following foreign owner that handles B2, performs only its own documented/test-owned cause cleanup, and returns `CONTINUE_EXECUTION`;
5. a real VCH registered through `AddVectoredContinueHandler`;
6. bounded sequence/thread/context identity capture for all three stages.

The VCH observation must prove or refuse all of:

- VCH callback was actually invoked for this hardware-#DB continuation;
- it ran after the downstream foreign handler's final context mutation/decision;
- it sees the expected owned Dr0/Dr1 configuration still present before final OS resume;
- owned B0 and foreign B2 cause handling matches the expected post-handler state;
- no unrelated observer rewrites the context merely to force the ordering result;
- same thread and same exception/debt identity can be bound without ambiguity.

If VCH is not invoked, order is ambiguous, the downstream context cannot be authenticated, or observation itself changes handler semantics, fail closed.

### 3. Test a VCH-to-c41 continuation bridge only if stage 2 succeeds

Only if the VCH boundary is observed and authenticated, build a test-only bridge using the already-reviewed c41 continuation-witness concept.

The required flow is:

```text
WS handler sees owned obligation
    -> reserve immutable debt before CONTINUE_SEARCH escape
    -> downstream foreign handler finalizes context and chooses CONTINUE_EXECUTION
    -> VCH authenticates the matching reserved debt/final context
    -> VCH binds that final context to a prepared c41-style continuation witness
    -> VCH returns
    -> OS resumes through witness
    -> witness token acknowledges that exact context only
    -> original continuation resumes
```

Requirements:

- reservation must precede escape from the WS handler;
- VCH binding must be allocation-free/bounded and must not use the diagnostic event lock/ring;
- nested/multiple debts must remain independently identified;
- one token cannot discharge another context/thread/lease/generation;
- downstream handler modifications must be preserved except for the explicitly reviewed continuation redirection required by the test bridge;
- no DR repair/masking may be added merely to make the bridge pass;
- marker observation does not prove stub code exit/reclamation; retain c48 lifetime distinction;
- executable code/data must remain alive until the owned worker exits in this feasibility task unless a stronger separately tested code-exit witness is provided;
- any missing reservation, VCH callback, identity, prepared stub, publication, cap or lifetime condition must poison/refuse rather than drop debt.

Do not copy c47's cooperative fixture assumption that every return path directly produces `CONTINUE_EXECUTION` from the WallSprint handler.

### 4. Exercise close / deadline / kill while the VCH/final context is held

For reset and Position where practical, cover the c48 mixed B0+B2 shape under:

- explicit close;
- unchanged deadline;
- kill/shutdown.

At minimum prove this order:

1. mixed event reaches WS and escapes via SEARCH;
2. downstream handler selects EXECUTION;
3. VCH has authenticated/published the matching continuation debt and is deliberately held before return;
4. coordinator cleanup is requested while debt is still outstanding;
5. retirement/re-arm is refused or remains pending honestly;
6. VCH is released;
7. the post-return witness for that exact context is observed;
8. cleanup can then complete without later owned-slot resurrection and without needing a second watched access.

When the bridge is disabled, preserve the c48 mixed-SEARCH resurrection counterexample unchanged.

### 5. Preserve foreign-only and unrelated controls

Keep at least these controls separate:

- foreign B2-only unchanged-context SEARCH;
- owned normal EXECUTION path from c39/c40/c41-c47;
- mixed B0+B2 SEARCH path with bridge disabled;
- unknown/stale lease identity;
- VCH registered but not dispatched;
- downstream handler changes/invalidates expected identity;
- nested/multiple reservations;
- cap exhaustion/publication failure;
- duplicate/stale/wrong-context token.

A foreign-only negative observation must not be promoted into a general safety rule.

### 6. Native Windows is required for positive VCH evidence

The historical Wine environment did not dispatch the c40 VCH callback for the owned hardware path. Preserve that as `UNAVAILABLE`.

C49 may use Wine for regression/counterexample/synthetic/model coverage, but a positive VCH dispatch/order/bridge result requires an owned native-Windows standalone run.

If owned tests/build pass and a bounded native question remains, publish only a test-owned artifact under:

```text
analysis/native-probes/c49
```

Prefer one owner command:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

The exact CLI may differ only if the implementation has a strong reason; document it in the c49 README.

The compact JSON must expose at minimum:

- environment / CPU vendor / mitigation query state;
- WS handler disposition;
- downstream handler disposition;
- VCH registration success;
- VCH callback count;
- ordered WS -> downstream -> VCH sequence identities;
- relevant before/downstream/VCH Dr0/1/2/3/6/7 state;
- reserved debt identity;
- VCH-to-witness binding result;
- witness acknowledgment identity;
- pending debt before/after witness;
- close/deadline/kill scenario result as applicable;
- resurrection observed/not observed after final cleanup;
- production_integrated=false;
- exact_slot_watch_enabled=false;
- game_used=false.

Do not request the game for this OS/exception-dispatch question.

### 7. Decide the c49 result

Emit exactly one primary result:

```text
VCH_SEARCH_CONTINUATION_BRIDGE_FEASIBILITY_PASS
VCH_SEARCH_CONTINUATION_BRIDGE_FEASIBILITY_FAIL
VCH_SEARCH_CONTINUATION_BRIDGE_UNAVAILABLE
```

A PASS requires native-Windows runtime evidence that the VCH boundary is actually invoked for the owned mixed hardware-#DB path, is ordered after the downstream handler's final mutation/continuation decision, can bind the exact pre-reserved debt to the final context, and can drive the c41-style post-return witness without later resurrection in the bounded owned cases.

A FAIL means the mechanism was exercised and violated a required property.

UNAVAILABLE means the required native callback/boundary could not be exercised or the environment cannot answer the question. Do not convert UNAVAILABLE into PASS or general rejection.

Even PASS authorizes only a later explicit production-design/integration review. It does not authorize modifying production code in c49 and does not authorize the deferred exact-slot watch.

## Required focused tests

Add a focused c49 test command, preferably:

```bash
bash source/tools/test_vch_search_bridge.sh
```

Cover at minimum:

- reservation before SEARCH escape;
- downstream EXECUTION ordering;
- VCH-after-downstream model;
- VCH-before/downstream or ambiguous ordering => refuse;
- no VCH callback => unavailable/refuse for positive certificate;
- mismatched context/thread/lease/generation => refuse;
- nested/multiple reservation identity;
- duplicate/stale token => refuse;
- cap/publication failure => sticky poison;
- close/deadline/kill pending until exact witness;
- bridge-disabled c48 resurrection remains reproduced;
- foreign-only control remains separate;
- marker observed but stub code exit not proven => code storage not reclaimable.

Pure/synthetic ordering tests are not native VCH evidence.

## Required regression validation

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
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If the exact focused command differs, run and report the actual command.

Expected repository result:

- c48 FAIL remains preserved unchanged;
- c39/c40 EXECUTION counterexamples remain;
- c48 mixed SEARCH counterexamples remain when the c49 bridge is absent/disabled;
- c47 bounded native configuration predicate remains unchanged;
- no production watch behavior changes;
- native VCH evidence is requested only if the test-owned implementation reaches a precise remaining runtime question;
- no exact-slot watch or gameplay work.

## Forbidden scope

C49 does not authorize:

- production continuation-witness/VCH integration;
- modifying production `orientation_trace.cpp` exception disposition, restore, lease, install or cleanup behavior;
- mutating a context before `CONTINUE_SEARCH` merely to force the downstream path through WallSprint code;
- broad DR6 masking/normalization;
- disabling/relaxing Windows mitigations;
- the deferred `module_base + 0xC2D2580` exact-slot RW8/WRITE8 watch;
- slot qword reads/logging or pointer chasing;
- publication/source-chain interpretation;
- producer `0x351EB00` traversal;
- PAGE_GUARD/broad interception;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38 / package0.1.56`. Do not create a game release for c49.

## Required outputs / handoff

Update as appropriate:

- test-only VCH/search bridge implementation and focused tests;
- `analysis/V01C49_*.md` / status/gate evidence;
- `analysis/native-probes/c49` only if owned validation passes and native evidence is required;
- `docs/PROJECT_STATE.md` so c48 is historical and c49 is current;
- `docs/CODEX_RESULT.md`;
- Wisdoms only if c49 establishes or rejects a reusable Windows exception-dispatch rule.

Commit and push the intended branch before claiming completion.

Final report must follow `docs/CODEX_REPORTING.md` and include:

- exact checks/tests performed;
- exact commands;
- expected result;
- actual result;
- PASS / SKIP / UNAVAILABLE separately;
- unperformed tests and reasons;
- c49 primary result;
- branch and commit SHA;
- whether owner native action is required.

## Owner action

No Update-WS, game launch, A/E comparison, WallRun capture, offline slot scan or game log is required for repository work.

If and only if Codex publishes a tested `analysis/native-probes/c49` artifact and reports that native Windows evidence is required, request the single documented standalone command from that folder and return the compact JSON/output.
