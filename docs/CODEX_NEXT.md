# Next task — c44 native debug-register canonicalization proof

## Status

C43 successfully localized the native-Windows final-restoration divergence. The owner ran the c43 standalone probe and the result is preserved in:

- `analysis/V01C43_NATIVE_WINDOWS_RESULT.md`
- `source/analysis/V01C43_NATIVE_WINDOWS_RESULT.md`
- `analysis/V01C43_RESTORATION_DIAGNOSTIC.md`
- `analysis/V01C43_STATUS.json`
- `analysis/native-probes/c43/README.md`
- `analysis/native-probes/c43/PROBE_MANIFEST.json`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, all c39-c43 retirement/witness evidence, the c43 probe source/tests, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted native evidence

The c43 owner command was:

```powershell
.\continuation-witness.exe --position --multiple --json
```

The actual native run reached final restoration with:

```text
mitigation flags                = [0,0,0x100]
mitigation classifier           = ADMIT
contexts                        = 3
pending before final poll       = 0
pending after final poll        = 0
canRetire before final poll     = true
active leases before final poll = true
active leases after final poll  = false
final poll returned             = false
classification                  = DR_ONLY_FAILURE
```

After final poll:

```text
Dr0 saved == after
Dr1 saved == after
Dr2 saved == after
Dr3 saved == after
Dr6 saved 0x0000000000000000
Dr6 after 0x00000000ffff0ff0
Dr7 saved 0x0000000000000010
Dr7 after 0x0000000000000410
```

Therefore:

- lease cleanup succeeded;
- pending continuation debt remained zero;
- the synthetic foreign Dr2 state was preserved exactly;
- Dr0-Dr3 restored exactly;
- exact-unmasked failure is limited to Dr6 delta `0xFFFF0FF0` and Dr7 delta `0x400` in this run;
- the c43 assertion fired before its three verification readbacks, so native stability of the post-poll representation is still unknown.

Do not reinterpret c43 as PASS. Its comparator was intentionally `EXACT_UNMASKED` and returned FAIL.

## Documentation lead, not yet an acceptance rule

Canonical Intel x86-64 documentation describes fixed/reserved values in DR6 and DR7 that appear consistent with the observed native deltas, including the conventional DR6 fixed-one pattern and a fixed-one bit in DR7. Microsoft `SetThreadContext` documentation also states that values in `CONTEXT` which cannot be specified may be silently set to the correct value, including OS-controlled debug-register state.

This is only a lead. C44 must establish the exact applicable architectural/Windows contract before any comparison is relaxed.

Do not assume the owner CPU vendor. The c43 JSON did not report it.

## Current blocker

Determine whether the c43 Dr6/Dr7 differences are exclusively documented fixed/canonicalized representation bits while every architecturally meaningful owned/foreign debug-register bit is restored exactly and stably.

C44 is still test-only. It does not authorize production integration.

## Current bounded Codex task

### 1. Establish the exact architecture contract

Use canonical current documentation and checked SDK definitions. At minimum inspect:

- Intel 64 / IA-32 debug-register definitions for DR6 and DR7;
- AMD64 debug-register definitions if the probe can run on an AMD CPU;
- Microsoft `GetThreadContext` / `SetThreadContext` behavior and `CONTEXT_DEBUG_REGISTERS` contract;
- any Windows rule that silently canonicalizes or OS-controls debug-register bits.

Record exact source/section/version references in `analysis/`.

The probe must report the CPU vendor from CPUID. Do not apply an Intel-only rule to an AMD/unknown vendor. For every supported vendor, derive the rule from that vendor's canonical architecture documentation. Unknown or undocumented vendor/bit behavior must fail closed.

### 2. Classify every compared DR6/DR7 bit

For the detected supported architecture, classify each relevant DR6/DR7 bit as one of:

- `MEANINGFUL_EXACT`: status/control/ownership semantics must match the saved state exactly;
- `FIXED_ONE`: architecture requires the current hardware representation to read as one;
- `FIXED_ZERO`: architecture requires the current hardware representation to read as zero;
- `OS_CONTROLLED_EXACT_OR_REFUSE`: Windows controls it; preserve/verify according to documented semantics or refuse if equivalence cannot be established;
- `UNKNOWN_REFUSE`.

Requirements:

- Dr0-Dr3 remain exact full-width comparisons.
- Do not mask a meaningful breakpoint enable, address, type/length, GD, status, single-step/debug cause, RTM-related or other architecturally meaningful field merely because it differs in a synthetic test.
- Do not treat all reserved bits as don't-care.
- Only bits with a documented required fixed value may be canonicalized.
- Current fixed bits must themselves have the documented fixed value; an unexpected fixed-bit value is a failure.
- Unknown/reserved-without-defined-read-value bits remain fail-closed, not ignored.

### 3. Add a test-only architectural comparison beside raw exact comparison

Keep the raw c43 comparison and raw values in JSON.

Add a separate, explicitly named test-only comparison, for example:

```text
RAW_EXACT
ARCHITECTURAL_CANONICAL
```

The architectural comparison may report equivalent only if:

- Dr0-Dr3 are exact;
- every `MEANINGFUL_EXACT` Dr6/Dr7 bit is exact;
- every `FIXED_ONE` / `FIXED_ZERO` current bit has its documented value;
- any saved/current difference is confined to documented fixed-value representation bits;
- no `UNKNOWN_REFUSE` bit participates;
- lease cleanup, pending debt and `canRetire` conditions remain unchanged.

Do not change production restoration code or production equality semantics in c44.

### 4. Capture post-poll stability before final result

The c43 process aborted before its three verification readbacks. Refactor only the test fixture so the native standalone result can capture the intended bounded post-poll readbacks before emitting the final PASS/FAIL classification.

Requirements:

- preserve the same owned worker, watch, continuation and final poll sequence;
- do not repair or rewrite DR state after final poll merely to obtain agreement;
- capture at least three sequential suspended-thread `GetThreadContext` debug-register readbacks after final poll;
- report raw values for Dr0/1/2/3/6/7 for each readback;
- report both raw-exact and architecture-aware equality for each readback;
- verify the architecture-aware result is stable across the bounded readbacks;
- if the diagnostic readback itself fails, emit partial evidence and fail closed.

Do not claim these sequential readbacks are an atomic execution history.

### 5. Add focused pure tests

Add focused owned tests for the comparison/classification helper.

At minimum cover:

- exact all-register equality;
- the exact observed c43 pair as a synthetic fixture, labeled synthetic;
- a difference only in a documented fixed-one bit;
- wrong value in a fixed-one/fixed-zero bit;
- one meaningful Dr6 status-bit mismatch;
- one meaningful Dr7 control/enable mismatch;
- Dr0-Dr3 mismatch;
- unknown vendor;
- unknown/reserved bit requiring refusal;
- raw exact false while architectural canonical true;
- raw exact false and architectural canonical false.

Synthetic tests are not native runtime evidence.

### 6. Produce a refreshed c44 standalone probe only after owned tests pass

Publish under:

```text
analysis/native-probes/c44
```

The owner command remains:

```powershell
.\continuation-witness.exe --position --multiple --json
```

The JSON must include at least:

- CPU vendor;
- documentation/comparator revision identifier;
- raw mitigation information;
- lease/pending/canRetire/final-poll state;
- raw saved/before/after Dr0/1/2/3/6/7;
- raw exact result;
- per-bit architectural classification/masks for Dr6/Dr7;
- architecture-aware result;
- three post-poll verification readbacks with both comparisons;
- final result/reason.

If architecture-aware equivalence is proven and stable, the standalone **test-only** probe may report an owned witness success under the explicit architecture-aware comparator. It must also state that raw exact equality is false when that remains true. This is not a production retirement certificate.

If the documentation or runtime evidence does not justify canonicalization, retain FAIL/REFUSED and state the precise missing guarantee.

## Forbidden scope

C44 does not authorize:

- production integration of the continuation witness;
- changes to production debug-register restoration behavior;
- broad masking of DR6/DR7;
- treating reserved bits as universally irrelevant;
- the deferred `module_base + 0xC2D2580` RW8/WRITE8 watch;
- slot qword reads/logging or pointer chasing;
- publication/source-chain interpretation;
- producer `0x351EB00` traversal;
- PAGE_GUARD or broad memory interception;
- Windows mitigation disabling/relaxation;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38 / package0.1.56`. Do not create a game release for c44.

## Required validation

Run from repository root:

```bash
bash source/tools/test_continuation_mitigations.sh
bash source/tools/test_continuation_diagnostics.sh
bash source/tools/test_continuation_witness.sh
bash source/tools/test_watch_retirement.sh
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

Run any new c44 focused comparator/canonicalization test explicitly and report its exact command.

Expected repository result:

- the c43 native evidence remains preserved as exact-unmasked FAIL;
- applicable vendor-specific DR6/DR7 fixed/meaningful bit semantics are documented precisely;
- unknown vendor/unknown bits fail closed;
- raw and architecture-aware comparisons are separate and auditable;
- all prior c39-c43 regression semantics remain intact;
- no production watch integration or exact-slot mode is enabled;
- a refreshed c44 standalone artifact is produced only after owned tests pass.

If any required test is not run, state the exact reason. Do not report unexecuted native-Windows/game tests as PASS.

## Required outputs / handoff

Update as appropriate:

- test-only comparator/diagnostic implementation;
- focused comparison tests;
- `analysis/` with architecture/Windows documentation proof and c43 accepted native result;
- `analysis/native-probes/c44` artifact/README/manifest after validation;
- `docs/PROJECT_STATE.md` so c43 native DR-only result is recorded and c44 is current;
- `docs/CODEX_RESULT.md`;
- relevant Wisdoms facts only if a reusable architecture/Windows rule is actually established.

Commit and push the intended branch before claiming completion.

The final report must follow `docs/CODEX_REPORTING.md` and include:

- exact tests/checks performed;
- exact commands;
- expected result;
- actual result;
- PASS/SKIP/UNAVAILABLE separately;
- all unperformed tests and reasons;
- branch and commit SHA;
- whether owner action is required.

## Owner action

No Update-WS, game launch, A/E comparison, WallRun capture, offline slot scan or game log is required for c44 repository work.

After a tested c44 standalone artifact is committed and pushed, request exactly one native-Windows standalone run if needed:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Return its compact JSON/output. Do not require launching Spider-Man 2 for this OS-level question.
