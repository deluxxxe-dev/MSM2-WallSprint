# Next task — c42 native mitigation boundary refinement for continuation witness

## Status

C41 is a **PARTIAL SUCCESS** only. The owned Wine continuation witness is feasible, but production integration remains unauthorized.

The owner has now run the c41 standalone probe on native Windows from commit `ded0ce052d2c39e5640d1ac5b79be0b85046ccac`:

```json
{"schema":1,"task":"c41","result":"REFUSED","reason":"mitigation_active_outside_supported_boundary","mitigation_query_known":[true,true,true],"mitigation_flags":[0,0,256],"game_used":false}
```

Accepted evidence:

- `analysis/V01C41_CONTINUATION_WITNESS.md`
- `analysis/V01C41_WITNESS_STATUS.json`
- `analysis/V01C41_NATIVE_WINDOWS_RESULT.md`
- `analysis/native-probes/c41/README.md`
- `analysis/native-probes/c41/PROBE_MANIFEST.json`
- `docs/CODEX_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, all c39-c41 retirement/witness evidence, the c41 probe source/tests, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Evidence boundary

### VERIFIED-RUNTIME — native Windows

The c41 probe queried three policies successfully and returned:

```text
ProcessDynamicCodePolicy        = 0
ProcessControlFlowGuardPolicy   = 0
ProcessUserShadowStackPolicy    = 256 / 0x100
```

The current c41 probe rejects **any nonzero policy DWORD**, so the continuation-witness path did not execute on this native run. Do not call this a witness PASS or witness FAIL.

### VERIFIED-STATIC — current c41 source

`continuation_witness_smoke.cpp` queries policies in this order:

1. `ProcessDynamicCodePolicy`
2. `ProcessControlFlowGuardPolicy`
3. `ProcessUserShadowStackPolicy`

and refuses when any returned DWORD is nonzero.

### VERIFIED-DOCUMENTATION

Microsoft documents `PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY` bit `0x100` as `CetDynamicApisOutOfProcOnly`. Its documented purpose concerns the process boundary for specific CET dynamic-code APIs. The current c41 witness does not call those APIs.

This does **not** automatically prove that the witness is safe under the observed native configuration. It only proves that the existing all-nonzero refusal is coarser than the documented per-bit policy semantics.

## Current blocker

Determine whether `0x100` is materially relevant to the exact c41 continuation witness, and define a defensible per-bit native mitigation admission policy.

Do not disable, relax or modify Windows mitigation settings to make the probe run.

## Current bounded Codex task

### 1. Review the exact mitigation-policy bits

For each bit in the queried policy structures that the c41 mechanism could encounter, classify it as one of:

- `RELEVANT_REFUSE`: documented or owned-tested to affect this exact mechanism;
- `IRRELEVANT_TO_C41_CANDIDATE`: documented to govern behavior/API paths that this candidate does not use;
- `UNKNOWN_REFUSE`: insufficient evidence, therefore fail closed.

At minimum cover:

- `PROCESS_MITIGATION_DYNAMIC_CODE_POLICY` bits;
- `PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY` bits;
- `PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY` bits, including `0x100 CetDynamicApisOutOfProcOnly`;
- reserved/unknown bits.

Use canonical Microsoft documentation or checked SDK definitions. Do not infer a bit's semantics from its numeric value alone.

### 2. Verify exact c41 mechanism dependencies

Statically confirm whether the c41 candidate uses or depends on:

- `SetProcessDynamicEHContinuationTargets`;
- `SetProcessDynamicEnforcedCetCompatibleRanges`;
- `SetThreadContext` or `RtlRestoreContext` for candidate execution;
- indirect JMP/CALL or RET in the generated stub;
- CFG call-target registration;
- CET-compatible image metadata;
- EH continuation metadata;
- dynamic-code policy opt-out APIs;
- any other API directly governed by an observed mitigation bit.

Do not broaden this into a general CET/CFG implementation project.

### 3. Refine only the test/probe admission classifier if justified

If documentation plus static mechanism review supports it, replace the current `any nonzero DWORD => REFUSED` rule with a precise per-bit classifier.

Requirements:

- `0x100` may be admitted only if its documented restriction is demonstrably irrelevant to the exact APIs/control transfer used by this candidate;
- all unknown/reserved bits must refuse;
- materially relevant enabled bits must refuse;
- no Windows mitigation may be disabled or changed;
- no production WallSprint watch code may be modified;
- JSON must report raw flags plus the per-policy/per-bit admission decision so the next native run is auditable.

If `0x100` cannot be safely classified, stop fail closed and document the missing guarantee instead of changing the classifier.

### 4. Add owned classifier tests

Add focused tests for the pure classification logic using synthetic policy DWORD inputs. These tests are only parser/classifier tests; they are not native mitigation runtime evidence.

At minimum test:

- all-zero accepted boundary;
- the observed native tuple `[0,0,0x100]`;
- each materially relevant known bit individually;
- multiple known bits;
- unknown/reserved bits;
- query-unknown on native Windows remains REFUSED.

Do not forge `GetProcessMitigationPolicy` runtime output in the actual standalone native execution path.

### 5. Preserve c41 witness behavior

The existing owned continuation-witness tests must remain intact:

- close/deadline/kill pending before acknowledgment;
- nested and multiple context accounting;
- context-cap poison/refusal;
- exact thread/lease/generation/context identity;
- GPR/RFLAGS/RSP/stack/YMM/FXSAVE checks;
- debug-register restoration checks;
- no later watched access required for repair;
- code/data lifetime until worker termination.

The original c39/c40 delayed-return counterexample must also remain intact.

### 6. Produce a refreshed standalone native probe only after tests pass

If the observed `[0,0,0x100]` tuple is admitted by the justified classifier, rebuild and refresh the standalone c41/c42 probe artifact and manifest for one additional owner-side native Windows run.

The requested native command must remain a single game-free command equivalent to:

```powershell
.\continuation-witness.exe --position --multiple --json
```

The resulting JSON must distinguish `OWNED_WITNESS_PASS`, `REFUSED`, and `FAIL`, and include raw mitigation flags plus classifier interpretation.

Do not request Spider-Man 2, Overstrike, Update-WS, WallRun capture, or a game log for this OS-level question.

## Forbidden scope

C42 does not authorize:

- production integration of the continuation witness;
- the deferred `module_base + 0xC2D2580` RW8/WRITE8 watch;
- slot qword reads/logging or pointer chasing;
- publication/source-chain interpretation;
- producer `0x351EB00` traversal;
- PAGE_GUARD or broad memory interception;
- mitigation disabling/relaxation;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38 / package0.1.56`. Do not create a game release for c42.

## Required validation

Run from repository root:

```bash
bash source/tools/test_continuation_witness.sh
bash source/tools/test_watch_retirement.sh
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If a new focused mitigation-classifier test script is added, run it explicitly and report its exact command separately.

Expected result for a successful c42 implementation:

- the mitigation classifier has explicit per-bit evidence-backed decisions;
- unknown/reserved/relevant enabled protections still fail closed;
- the observed native tuple `[0,0,0x100]` is either admitted with documented justification or remains REFUSED with a precise documented reason;
- all c41 witness and c39/c40 retirement regressions retain their prior semantics;
- no production watch integration or slot-watch functionality is enabled;
- a refreshed standalone probe is produced only if its admission boundary is justified.

If any required test is not run, report the exact reason. Do not report unexecuted native Windows/game tests as PASS.

## Required outputs / handoff

Update as appropriate:

- focused test/probe classifier implementation only;
- `analysis/` with per-bit mitigation table, documentation sources, static dependencies, evidence classes and remaining unknowns;
- `analysis/V01C41_NATIVE_WINDOWS_RESULT.md` or a new c42 result file without erasing the original native evidence;
- standalone probe README/manifest if a refreshed probe is produced;
- `docs/PROJECT_STATE.md` so c41 partial success/native refusal and c42 current stage are explicit;
- `docs/CODEX_RESULT.md`;
- relevant Wisdoms facts only if a reusable general rule is actually established.

Commit and push the intended branch before claiming completion.

The final report must follow `docs/CODEX_REPORTING.md` and include:

- what changed;
- files changed;
- exact tests/checks performed;
- exact commands used;
- expected result;
- actual result;
- PASS/SKIP/UNAVAILABLE counts separately;
- unperformed tests and reasons;
- branch and commit SHA;
- whether one additional owner-side native standalone probe run is required.

## Owner action

No owner action is required while Codex performs c42 classification/refinement.

Only after a refreshed probe is committed and pushed may the owner be asked to run the one standalone native Windows command. Do not ask for a game launch or Update-WS.
