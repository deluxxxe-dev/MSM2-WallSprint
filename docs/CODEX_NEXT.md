# Next task — c50 native mixed-SEARCH identity/cause predicate decomposition

## Status

C49 test-only VCH/search boundary probe is complete in repository and the owner ran the published native-Windows standalone.

Accepted native result:

- `analysis/V01C49_NATIVE_WINDOWS_RESULT.md`
- `source/analysis/V01C49_NATIVE_WINDOWS_RESULT.md`
- `analysis/V01C49_VCH_SEARCH_BRIDGE.md`
- `analysis/V01C49_STATUS.json`
- `analysis/native-probes/c49/README.md`
- `analysis/native-probes/c49/PROBE_MANIFEST.json`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, c48/c49 evidence, current production `orientation_trace.cpp`, c49 probe source/tests, c41-c47 continuation-witness evidence, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted c49 native facts

Owner command:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

Primary result remains:

```text
VCH_SEARCH_CONTINUATION_BRIDGE_FEASIBILITY_FAIL
reason = WS_mixed_SEARCH_identity_or_cause_mismatch
```

Do not relabel that run PASS.

However the same native run establishes these bounded runtime facts:

```text
environment                    = native_Windows
cpu_vendor                     = GenuineIntel
mode/scenario                  = Position/close
mitigation tuple               = [0,0,0x100], ADMIT
WS disposition                 = CONTINUE_SEARCH
downstream disposition         = CONTINUE_EXECUTION
VCH registered                 = true
VCH callback count             = 1
ordered_final_context_observed = true
native_boundary_observed       = true
```

Observed order:

```text
1 WS_entry
2 WS_after_engine
3 reserved debt
4 SEARCH escape
5 downstream_before
6 downstream_final
7 VCH
```

The downstream-final and VCH snapshots have identical thread/context/exception/RIP/EFlags/ContextFlags and Dr0/Dr1/Dr2/Dr3/Dr6/Dr7. Thus native Windows did exercise the stage-2 VCH boundary that Wine could not.

The strict run still poisoned earlier at one aggregate predicate. Existing schema1 combines:

- WS disposition;
- entry Dr6 mixed-cause mask;
- after-engine Dr6 residual mask;
- exact B0-only delta;
- RIP preservation;
- Dr0/Dr1/Dr2/Dr3/Dr7 preservation;
- global `usedLeases==1`;
- `own(after, leases[0])==3`;
- window id;
- reset generation.

The native JSON proves several of these from printed values, but does not expose every internal lease/window conjunct individually. Therefore the exact failing conjunct remains UNKNOWN.

Static inspection identifies one hypothesis only: the test request sets `multithread=true` while the aggregate predicate requires global `usedLeases==1`. Native multithread expansion may make that fixture-only assumption false. Do not accept that hypothesis without direct reporting.

## Current bounded task

C50 is a **test-only predicate-decomposition and lease-set identity task**. It does not authorize the VCH-to-c41 redirection yet and does not modify production behavior.

### 1. Split the c49 aggregate predicate into explicit evidence

Change only the test-owned c49/c50 probe and diagnostics so every conjunct is reported independently before any sticky poison decision.

At minimum report:

```text
ws_search_ok
entry_cause_mask
entry_cause_expected
post_engine_cause_mask
post_engine_cause_expected
cause_delta
cause_delta_expected
rip_preserved
Dr0_preserved
Dr1_preserved
Dr2_preserved
Dr3_preserved
Dr7_preserved
used_leases_total
matching_worker_lease_count
matching_owned_context_lease_count
selected_lease_index
selected_window_id
selected_window_generation
selected_own_mask
expected_window_id
expected_generation
```

Also emit a stable list/summary for every currently published lease that could match the worker/context, including at least lease index, tid, window, `up`, `own(context, lease)` and installed/suspended status where safely observable.

Do not expose unrelated process/game secrets; this is an owned standalone fixture.

### 2. Distinguish global lease count from unique debt identity

Audit the static requirement `usedLeases==1`.

Do not simply delete it to make native pass.

Determine whether the property actually needed for this bounded debt is:

- exactly one global lease; or
- exactly one authenticated matching lease/obligation for the current worker/context/window/generation while unrelated leases may exist.

Use current production lease semantics and the native fixture shape. If multiple candidate owned obligations match the same context, remain fail closed.

If the only failing old conjunct is global `usedLeases==1`, and there is exactly one unambiguous matching owned lease with the expected worker/window/generation/Dr0-Dr1 ownership, add a separately named test-only c50 predicate for **UNIQUE_MATCHING_DEBT_IDENTITY**. Preserve the old c49 aggregate result as historical FAIL; do not rewrite it.

If any ownership/window/generation/cause field genuinely conflicts, report the exact conflict and stop.

### 3. Preserve the already-observed native VCH boundary

The new probe must still observe, without manufacturing it:

```text
WS SEARCH
-> downstream EXECUTION
-> VCH callback
```

Require exact downstream-final to VCH CONTEXT equality as c49 did.

Do not redirect RIP, repair DR state, mask causes, or create a post-return witness in c50.

### 4. Keep debt pending and fail closed

As in c49:

- reserve debt before SEARCH escape;
- request close/deadline/kill while debt remains;
- do not perform legacy live final cleanup while no witness exists;
- retain pending debt until actual worker exit;
- keep code/data alive until worker exit;
- do not infer retirement from VCH observation alone.

### 5. Focused test coverage

Add/update a focused command, preferably:

```bash
bash source/tools/test_vch_search_identity.sh
```

Cover at minimum:

- all c49 aggregate conjuncts serialized individually;
- one unrelated lease plus one unique matching lease;
- multiple matching leases => refuse;
- wrong worker tid => refuse;
- wrong window id => refuse;
- wrong generation => refuse;
- own mask != 3 => refuse;
- cause pattern mismatch => refuse;
- preserved register mismatch => refuse;
- exact unique matching debt => positive synthetic/model result only;
- historical c49 aggregate FAIL representation remains available;
- Wine no-VCH path remains UNAVAILABLE, not PASS;
- c48 mixed SEARCH resurrection counterexample remains reproduced separately.

Synthetic/model success is not native evidence.

### 6. Publish one bounded native probe if needed

If all repository tests pass and the exact native subpredicate remains necessary, publish only a test-owned artifact under:

```text
analysis/native-probes/c50
```

Prefer one owner command:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

The JSON must expose all decomposed predicate fields plus the c49 ordered stages and VCH boundary evidence.

No game, Update-WS, Overstrike, scan, game log or mitigation-setting change is required.

### 7. Decide c50 result

Emit exactly one primary result:

```text
MIXED_SEARCH_IDENTITY_DIAGNOSTIC_PASS
MIXED_SEARCH_IDENTITY_DIAGNOSTIC_FAIL
MIXED_SEARCH_IDENTITY_DIAGNOSTIC_UNAVAILABLE
```

PASS means the exact old failing conjunct is identified and, if the issue is only an over-strong global lease-count assumption, a separately named unique-matching-debt predicate is demonstrated on native Windows without weakening production ownership semantics.

FAIL means a genuine cause/ownership/window/generation ambiguity or contradiction remains.

UNAVAILABLE means the required native boundary/subpredicate cannot be exercised.

Even PASS authorizes only a later explicit VCH-to-c41 bridge feasibility task (c51 or later). It does not authorize production integration or exact-slot watch work.

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
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If the focused c50 command uses a different exact name, run and report that exact command.

## Reporting requirements

Report:

- every executed test/check;
- exact command;
- expected result;
- actual result;
- PASS / SKIP / UNAVAILABLE counts separately;
- every unrun test and reason;
- every decomposed predicate field from repository tests and any native result;
- c50 primary result;
- whether owner native Windows action is required;
- final branch name and commit SHA.

Do not report an unexecuted native case as PASS.

## Forbidden scope

C50 does not authorize:

- production continuation-witness integration;
- VCH RIP redirection / c41 witness bridge;
- production restore/ownership/install/handler/lease/cleanup changes;
- exact-slot watch;
- slot qword reads or pointer chasing;
- producer traversal;
- mitigation changes;
- gameplay/source/Position/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38/package0.1.56`.
