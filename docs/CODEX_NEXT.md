# Next task — c51 native VCH-to-c41 post-return witness bridge feasibility

## Status

C50 repository diagnostics are complete and the owner ran the published native-Windows decomposed probe.

Accepted native result:

- `analysis/V01C50_NATIVE_WINDOWS_RESULT.md`
- `source/analysis/V01C50_NATIVE_WINDOWS_RESULT.md`
- `analysis/V01C50_PREDICATE_DECOMPOSITION.md`
- `analysis/V01C50_STATUS.json`
- `analysis/native-probes/c50/README.md`
- `analysis/native-probes/c50/PROBE_MANIFEST.json`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, all c39-c50 retirement/witness evidence, current production `orientation_trace.cpp`, c41 continuation-witness implementation/tests, c48-c50 SEARCH/VCH harnesses, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted c50 native evidence

Owner command:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

Primary result:

```text
MIXED_SEARCH_IDENTITY_DIAGNOSTIC_PASS
reason = native_only_legacy_global_count_failed_unique_matching_debt_identity_observed
```

Native identity/boundary:

```text
environment                    = native_Windows
cpu_vendor                     = GenuineIntel
mode/scenario                  = Position/close
mitigation tuple               = [0,0,0x100], ADMIT
WS disposition                 = CONTINUE_SEARCH
downstream disposition         = CONTINUE_EXECUTION
VCH registered/callbacks       = true / 1
ordered_final_context_observed = true
native_boundary_observed       = true
```

Observed sequence remained:

```text
1 WS_entry
2 WS_after_engine
3 reserved debt
4 SEARCH escape
5 downstream_before
6 downstream_final
7 VCH
```

The downstream-final and VCH snapshots were identical for thread/context/exception/RIP/EFlags/ContextFlags and Dr0/Dr1/Dr2/Dr3/Dr6/Dr7.

The exact c49 aggregate failure is now identified:

```text
used_leases_total                      = 4
global_used_leases_is_one              = false
legacy_c49_aggregate                   = false
only_global_usedLeases_conjunct_failed = true
legacy_failed_conjuncts                = [global_used_leases_is_one]
```

Every cause/register/window/generation conjunct passed. Worker-specific identity was unique:

```text
matching_worker_lease_count        = 1
matching_owned_context_lease_count = 1
selected_lease_index               = 0
selected_window_id                 = 1
selected_window_generation         = 149
selected_own_mask                  = 3
UNIQUE_MATCHING_DEBT_IDENTITY      = true
c50_diagnostic_admissible          = true
```

All four global lease records showed the same watched addresses/window/generation and `own(context, lease)==3`, but they had different thread IDs. Therefore global lease-table cardinality is not the debt identity for this native multithread-expanded fixture. The matching worker thread + lease/window/generation/context identity is unambiguous.

C50 still deliberately left debt pending and implemented no witness:

```text
VCH_to_witness_binding          = null
witness_acknowledgment          = null
pending_before_witness          = 1
pending_at_worker_exit          = 1
can_retire_before_worker_exit   = false
cleanup_requested_while_pending = true
live_final_cleanup_performed    = false
bridge_implemented              = false
observer_Rip_writes             = 0
observer_DR_repairs             = 0
```

Do not reinterpret c50 as saved-context retirement or production integration.

## Current blocker

C49/c50 now establish on the owner native Windows fixture that a pre-reserved mixed-SEARCH debt can be uniquely authenticated again in a real VCH **after** the downstream owner has finalized the continuation CONTEXT.

The remaining bounded question is whether that exact VCH boundary can redirect only the final continuation RIP through the already-reviewed c41-style post-return witness, obtain an acknowledgment for that exact debt after OS continuation, and then allow debug-configuration cleanup without later owned-slot resurrection.

C51 is test-only feasibility. It is not production integration.

## Current bounded task

### 1. Preserve all historical results and predicates

Do not weaken, replace or relabel:

- c39/c40 delayed saved-CONTEXT resurrection counterexamples;
- c41 continuation-witness token semantics;
- c42 mitigation classifier;
- c43 exact restoration diagnostics;
- c44 raw/canonical comparisons;
- c45/c46 lifecycle observations;
- c47 `RETIREMENT_CONFIGURATION_SAFE` fixture result;
- c48 production integration review FAIL and mixed SEARCH counterexample;
- c49 historical aggregate FAIL plus native VCH boundary observation;
- c50 `MIXED_SEARCH_IDENTITY_DIAGNOSTIC_PASS` and `UNIQUE_MATCHING_DEBT_IDENTITY`.

The old global `usedLeases==1` condition may remain available as historical c49 evidence. Do not silently rewrite historical artifacts.

### 2. Build a test-only VCH-to-c41 bridge for exactly one authenticated debt

Reuse the c50 owned mixed event shape and keep production `orientation_trace.cpp` unchanged.

Required flow:

```text
WS handler authenticates owned B0 obligation
 -> reserve immutable debt before CONTINUE_SEARCH escape
 -> downstream foreign owner clears only B2 and chooses CONTINUE_EXECUTION
 -> real VCH observes exact downstream-final CONTEXT
 -> c50 UNIQUE_MATCHING_DEBT_IDENTITY authenticates the reserved obligation
 -> coordinator prepares/publishes a c41-style continuation stub for that exact original RIP
 -> VCH changes only the final CONTEXT RIP to the prepared stub
 -> VCH returns
 -> OS resumes through the stub
 -> stub writes the exact debt token then direct-jumps to the original RIP
 -> coordinator observes the matching token and may discharge only that debt
```

Requirements:

- reservation remains before SEARCH escape;
- downstream-final to VCH equality must be established before redirection;
- the VCH must not allocate executable memory, format/log, take the diagnostic event lock/ring, or perform unbounded discovery;
- test-only VCH may use the existing bounded harness hold while the coordinator prepares the stub outside callback context;
- stub publication must complete before VCH edits RIP;
- VCH may modify **only RIP** for the explicit bridge; Dr0-Dr3/Dr6/Dr7, flags, stack and downstream foreign changes must remain untouched;
- unknown/multiple worker debt identity, changed final context, missing stub, publication failure, cap failure or unsupported mitigation must poison/refuse;
- one token must acknowledge exactly one thread/creation-epoch/context/exception/lease/window/generation/sequence debt;
- duplicate/stale/wrong-context tokens refuse and must not discharge another debt.

Do not add DR repair, masking or normalization to make the bridge pass.

### 3. Reuse the reviewed c41 continuation form without weakening its guards

Prefer the existing c41 continuation form and preparation checks rather than inventing a new trampoline:

```text
MOV [RIP + marker], imm32/token
JMP rel32 original_RIP
```

Retain all existing range/overflow/current-code/page/mitigation/publication checks. If the current c41 helper cannot safely bind the c50 final RIP without changing its reviewed contract, stop and report the exact blocker.

The owner-native standalone process must query its actual current mitigation state. Do not assume a tuple or disable/relax mitigations.

C47/c50 owner evidence is GenuineIntel with `[0,0,0x100]`, but the live c51 run must report what it actually observes.

### 4. Prove post-return acknowledgment separately from code lifetime

A marker write proves that the redirected saved CONTEXT has resumed through that stub; it does **not** prove the thread has executed the following JMP or that stub storage can be reclaimed.

C51 must therefore keep two states separate:

```text
DEBT_ACKNOWLEDGED
STUB_EXECUTION_QUIESCENT / STORAGE_RECLAIMABLE
```

For this feasibility task:

- the exact token may discharge configuration-retirement debt after it is observed;
- executable stub/marker storage must remain alive and unreused until actual owned worker termination unless a stronger separately tested code-exit witness is added;
- do not claim live-thread stub reuse/reclamation from marker acknowledgment.

### 5. Exercise configuration cleanup only after the exact witness

Use the Position/close shape first; do not broaden the native acceptance matrix merely to increase coverage.

Required ordering:

1. mixed event reaches WS and escapes SEARCH;
2. downstream chooses EXECUTION;
3. VCH authenticates final context and is held;
4. coordinator requests close while pending debt is still 1;
5. no legacy live final cleanup/retirement may occur while the debt is unacknowledged;
6. coordinator publishes the exact continuation stub;
7. VCH redirects only RIP and returns;
8. exact witness token is observed after OS continuation;
9. pending debt becomes 0 for that exact identity only;
10. existing configuration cleanup/final poll may then run;
11. verify inactive lease/current configuration restoration and at least three stable debug-register readbacks;
12. release the worker to terminate; only after actual thread exit may test-owned stub/marker storage be reclaimed.

The positive run must not require a second watched access to clean up or prove retirement.

Preserve synthetic foreign Dr2/control state exactly as prior fixtures require.

### 6. Required failure/refusal coverage

Add a focused command, preferably:

```bash
bash source/tools/test_vch_post_return_witness.sh
```

Cover at minimum:

- reservation before SEARCH escape;
- c50 unique worker debt identity with unrelated global leases;
- multiple matching worker obligations => refuse;
- downstream-final/VCH context mismatch => refuse;
- VCH before downstream-final or duplicate VCH => refuse;
- stub preparation/range/publication failure => sticky refuse;
- VCH edits anything beyond RIP => fail;
- wrong thread/epoch/context/exception/lease/window/generation/sequence token => refuse;
- duplicate/stale token => refuse;
- cleanup request while pending remains pending;
- exact token discharges only the matching debt;
- marker acknowledgment does not mark storage reclaimable;
- worker exit permits terminal test-owned storage cleanup;
- bridge disabled retains the c48 mixed-SEARCH resurrection counterexample;
- Wine callback0 remains UNAVAILABLE, not PASS;
- all c39-c50 regressions remain semantically unchanged.

Synthetic/model PASS is not native bridge evidence.

### 7. Native Windows is required for positive bridge evidence

Wine has historically returned VCH callback0 on this owned path. Preserve it as UNAVAILABLE.

If owned repository validation passes, publish a test-owned standalone under:

```text
analysis/native-probes/c51
```

Prefer the same owner command:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

The JSON must expose at minimum:

- environment / CPU vendor / actual mitigation tuple and admission;
- c50 identity diagnostics and selected debt identity;
- WS/downstream/VCH ordered stages and exact final-context equality;
- debt reserved/published state;
- stub preparation/publication identity, original RIP and redirected RIP;
- exact list of VCH context fields changed, requiring RIP only;
- witness token expected/observed identity;
- pending debt before/after witness;
- cleanup requested before witness;
- final poll/lease state after witness;
- final Dr0/1/2/3/6/7 and three stable readbacks;
- resurrection observed/not observed without a second watched access;
- stub storage retained until worker exit;
- production_integrated=false;
- exact_slot_watch_enabled=false;
- game_used=false.

### 8. Decide c51 result

Emit exactly one primary result:

```text
VCH_POST_RETURN_WITNESS_BRIDGE_FEASIBILITY_PASS
VCH_POST_RETURN_WITNESS_BRIDGE_FEASIBILITY_FAIL
VCH_POST_RETURN_WITNESS_BRIDGE_UNAVAILABLE
```

PASS requires native Windows evidence for the exact bounded Position/close flow: real VCH after downstream-final, c50 unique identity, RIP-only redirection to the reviewed stub, exact post-return token, debt0 only after that token, cleanup afterward, stable restored debug configuration and no later owned-slot resurrection in the bounded readbacks. Stub storage must still be retained until thread exit.

FAIL means the bridge was exercised but violated a required property.

UNAVAILABLE means the required native VCH/stub boundary could not be exercised or a supported prerequisite was unavailable. Do not convert unavailable native execution into PASS.

Even PASS authorizes only a later explicit production-shape design/integration review. It does not authorize changing production behavior or enabling the exact-slot watch.

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

If the focused c51 command uses a different exact name, execute and report that exact command.

## Required outputs / handoff

Update as appropriate:

- test-only bridge implementation and focused tests;
- `analysis/` with accepted c50 native result and c51 evidence/boundaries;
- `analysis/native-probes/c51` only after owned validation;
- `docs/PROJECT_STATE.md` so c50 native PASS is historical and c51 is current;
- `docs/CODEX_RESULT.md`;
- relevant Wisdoms facts if a reusable rule is established.

Commit and push the intended branch before claiming completion.

The final report must follow `docs/CODEX_REPORTING.md` and include:

- exact tests/checks performed;
- exact commands;
- expected result;
- actual result;
- PASS / SKIP / UNAVAILABLE / BLOCKED separately;
- every unperformed test and reason;
- branch and commit SHA;
- whether owner action is required.

## Forbidden scope

C51 does not authorize:

- production continuation-witness integration;
- production `orientation_trace.cpp` behavior changes;
- production restore/ownership/install/handler/lease/cleanup changes;
- live-thread stub reuse/reclamation based only on marker acknowledgment;
- broad DR6 masking/normalization;
- the deferred `module_base + 0xC2D2580` exact-slot watch;
- slot qword reads/logging or pointer chasing;
- publication/source-chain interpretation;
- producer `0x351EB00` traversal;
- PAGE_GUARD or broad memory interception;
- mitigation disabling/relaxation;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38 / package0.1.56`. Do not create a game release for c51.

## Owner action

No owner action is required until the c51 repository task has built, tested, committed and pushed a standalone artifact. If a native question remains, request at most one game-free native Windows command from the published c51 folder. No Update-WS or game launch.
