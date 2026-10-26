# Next task — c43 native final-restoration divergence diagnostic

## Status

C42 refined the standalone mitigation classifier and the owner ran the refreshed native-Windows probe from commit `70c55e3caec4ca10d0e995b8de4ba13b1b089aa2`.

The observed native mitigation tuple `[0,0,0x100]` was admitted. The continuation-witness path therefore executed, but the run failed at the final combined restoration assertion:

```text
c41 owned assertion at line 166: !activeLeases()&&same(debug(),saved)
```

Exact native result and evidence boundary:

- `analysis/V01C42_NATIVE_WINDOWS_RESULT.md`
- `analysis/V01C42_MITIGATION_BOUNDARY.md`
- `analysis/V01C42_STATUS.json`
- `analysis/native-probes/c42/README.md`
- `docs/CODEX_RESULT.md`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, all c39-c42 retirement/witness evidence, the continuation witness source/tests, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted evidence

### VERIFIED-RUNTIME — native Windows

The c42 standalone command was:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Observed classifier state:

```text
ProcessDynamicCodePolicy      = 0
ProcessControlFlowGuardPolicy = 0
ProcessUserShadowStackPolicy  = 0x100 CetDynamicApisOutOfProcOnly
classifier decision           = ADMIT
```

The run reached the final assertion after the witness path had executed. It did not stop at mitigation admission.

Because the failing assertion appears only after the preceding assertions in the same source sequence, the following completed before the failure:

- the outer witness became acknowledged;
- pending context debt reached zero;
- `canRetire()` returned true;
- `pollReset(false, "owned_c41_post_return_witness")` returned false as expected by the fixture.

The final combined predicate was false:

```text
!activeLeases() && same(debug(), saved)
```

### VERIFIED-STATIC — tested source

`same()` compares exactly:

```text
Dr0 Dr1 Dr2 Dr3 Dr6 Dr7
```

The current JSON does not identify whether the failed side is:

- retained `activeLeases()` state;
- one or more debug-register mismatches;
- both.

Do not infer the cause.

## Current blocker

Characterize the native-Windows final-restoration divergence without changing production behavior or weakening any restoration requirement.

C43 is a diagnostic task only.

## Current bounded Codex task

### 1. Split the opaque final assertion into test-only diagnostics

Preserve the same test-owned flow and acceptance requirement, but make the standalone probe report the two sides separately.

At minimum record in compact JSON:

- `active_leases_before_final_poll`;
- `active_leases_after_final_poll`;
- the final `pollReset` return value;
- `pending_contexts_before_final_poll` and after poll;
- whether `canRetire()` was true before final poll;
- per-register equality between the saved snapshot and current debug state for:
  - `Dr0`
  - `Dr1`
  - `Dr2`
  - `Dr3`
  - `Dr6`
  - `Dr7`;
- enough bounded register diagnostics to identify which register differs without dumping unrelated process/game data;
- exact mode/scenario/thread/lease/generation/context-count identity already used by the fixture.

Capture debug state immediately before final poll and immediately after final poll where safe in the existing owned fixture, so the first divergence boundary can be identified.

Do not silently normalize or mask a mismatch in c43.

### 2. Preserve the current acceptance rule

A diagnostic run must still be considered a failure unless both conditions hold:

```text
activeLeases() == false
same(debug(), saved) == true
```

Do not turn the native c42 failure into PASS by splitting the assertion.

If exact restoration proves too strict because Windows documents canonicalization of a specific debug-register field, only record that as a hypothesis/evidence candidate in c43. Do not change `same()` semantics or production restoration policy in this task.

### 3. Preserve c39-c42 behavior

Keep all existing owned tests and counterexamples intact:

- c39/c40 delayed saved-CONTEXT resurrection reproduction;
- c41 post-return witness ordering and per-context debt;
- close/deadline/kill behavior;
- nested/multiple context accounting;
- context-cap refusal;
- GPR/RFLAGS/RSP/stack/YMM/FXSAVE checks;
- foreign DR ownership/refusal semantics;
- c42 mitigation classifier decisions and unknown/reserved fail-closed behavior.

Do not weaken deadlines, caps, assertions, or ownership checks.

### 4. Add focused diagnostic regression coverage

Owned Wine tests should prove the new diagnostic output itself is correct on a known passing restoration path.

Where practical, add test-only negative coverage for the diagnostic serializer/comparison logic without forging production watch state.

The diagnostic must distinguish at least:

- lease-only failure;
- DR-only failure;
- combined failure;
- all-restored success.

Synthetic comparison/unit inputs may be used for serializer/comparator tests, but must be clearly separated from real native/Wine watch execution.

### 5. Produce a refreshed standalone c43 native probe

After all required owned tests pass, publish a refreshed test-only artifact under:

```text
analysis/native-probes/c43
```

The owner command should remain one game-free command:

```powershell
.\continuation-witness.exe --position --multiple --json
```

The result JSON must expose the final-restoration diagnostic fields even when the run fails.

No Windows mitigation setting may be disabled or changed.

### 6. Fail closed

If the diagnostic itself cannot safely obtain the required before/after state without perturbing the mechanism, stop and document that exact limitation.

Do not guess whether Windows, Wine or WallSprint caused the c42 mismatch.

## Forbidden scope

C43 does not authorize:

- production integration of the continuation witness;
- changes to production debug-register restoration semantics;
- weakening `same()` or lease cleanup requirements;
- the deferred `module_base + 0xC2D2580` RW8/WRITE8 watch;
- slot qword reads/logging or pointer chasing;
- publication/source-chain interpretation;
- producer `0x351EB00` traversal;
- PAGE_GUARD or broad memory interception;
- mitigation disabling/relaxation;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38 / package0.1.56`. Do not create a game release for c43.

## Required validation

Run from repository root:

```bash
bash source/tools/test_continuation_mitigations.sh
bash source/tools/test_continuation_witness.sh
bash source/tools/test_watch_retirement.sh
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

If a new focused diagnostic test script is added, run its exact command separately and report it.

Expected result for c43 repository work:

- the previous opaque final assertion is represented by explicit test-only diagnostic fields;
- Wine passing behavior remains passing;
- the exact native c42 result remains recorded as a failure, not reclassified;
- all prior ownership/restoration/mitigation/counterexample semantics remain intact;
- no production integration or slot-watch functionality is enabled;
- a c43 standalone artifact is produced only after owned tests pass.

If any required test is not run, state the exact reason. Do not report unexecuted native-Windows/game tests as PASS.

## Required outputs / handoff

Update as appropriate:

- test-only witness/diagnostic implementation;
- focused diagnostic tests;
- `analysis/` with c42 native evidence, c43 diagnostic contract and limitations;
- `analysis/native-probes/c43` artifact/README/manifest if owned validation passes;
- `docs/PROJECT_STATE.md` so c42 native execution failure is recorded and c43 is current;
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
- whether owner action is required.

## Owner action

No Update-WS, game launch, A/E comparison, WallRun capture, offline slot scan or game runtime log is required for c43 repository work.

After a tested c43 standalone artifact is committed and pushed, request exactly one native-Windows standalone command if needed:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Return only its compact JSON/output. Do not require launching Spider-Man 2 for this OS-level diagnostic.
