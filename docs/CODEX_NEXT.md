# Next task — c46 watch-enabled pre-access DR6 boundary proof

## Status

C45 is complete as a repository/test task and the owner has run the published native-Windows standalone probe from commit `d48b27acb71024409a55642fbe3226af7494aac6`.

Accepted native result:

- `analysis/V01C45_NATIVE_WINDOWS_RESULT.md`
- `source/analysis/V01C45_NATIVE_WINDOWS_RESULT.md`
- `analysis/V01C45_DR6_LIFECYCLE.md`
- `analysis/V01C45_STATUS.json`
- `analysis/native-probes/c45/README.md`
- `analysis/native-probes/c45/PROBE_MANIFEST.json`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, all c39-c45 retirement/witness evidence, the current continuation-witness source/tests, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted c45 native evidence

Owner command:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Observed native identity/result:

```text
environment                         = native_Windows
mode                                = Position
scenario                            = multiple
cpu_vendor                          = GenuineIntel
mitigation_flags                    = [0,0,0x100]
mitigation_classifier               = ADMIT
contexts                            = 3
watch_context_writes_observed       = 1
outer_hardware_DB_events            = 1
pending before/after final poll     = 0 / 0
canRetire before final poll         = true
active leases after final poll      = false
final poll returned                 = false
RAW_EXACT                           = false
ARCHITECTURAL_CANONICAL             = false
RETIREMENT_STATUS_SAFE_added        = false
result                              = FAIL
```

Final state remained stable across all three readbacks:

```text
Dr0 = saved
Dr1 = saved
Dr2 = synthetic foreign saved value
Dr3 = saved
Dr6 = 0x00000000ffff0ff0
Dr7 = 0x0000000000000410
```

The exact c45 DR6 lifecycle was:

```text
baseline_pre_foreign                 bit11=0 bit16=0 Dr6=0x0
post_foreign_set_readback            bit11=0 bit16=0 Dr6=0x0
post_foreign_resume_saved            bit11=0 bit16=0 Dr6=0x0
post_watch_set_readback_pre_resume   bit11=0 bit16=0 Dr6=0x0
outer_db_handler_entry               bit11=1 bit16=1 Dr6=0xFFFF0FF1
outer_db_handler_after_engine        bit11=1 bit16=1 Dr6=0xFFFF0FF0
post_return_pre_final_poll           bit11=1 bit16=1 Dr6=0xFFFF0FF0
after_final_poll                     bit11=1 bit16=1 Dr6=0xFFFF0FF0
verification_readback_1              bit11=1 bit16=1 Dr6=0xFFFF0FF0
verification_readback_2              bit11=1 bit16=1 Dr6=0xFFFF0FF0
verification_readback_3              bit11=1 bit16=1 Dr6=0xFFFF0FF0
```

At #DB entry B0 was set. The existing engine handler cleared B0 before returning, while bit11/bit16 stayed 1. No later B0/B1 cause remained.

C45 classified the first observed transition for both bit11/bit16 as:

```text
HARDWARE_DB_ENTRY
reason=unique_first_observed_boundary_not_underlying_causality
```

This is a bounded observation label, not proof that the processor itself rather than Windows debug-context delivery caused the transition.

## Documentation/static boundary

Use the current official Intel SDM when accessible, especially the current DR6 section, and current Microsoft `GetThreadContext` / `SetThreadContext` documentation.

Preserve these distinctions:

- Intel defines DR6 as a debug **status** register, while breakpoint address/enables/type/length are controlled through Dr0-Dr3/Dr7.
- RTM bit16 and BLD bit11 have specific status semantics; do not generalize them to all DR6 bits.
- Microsoft thread-context APIs can expose/adjust OS-controlled processor state, but this is not a blanket rule that every DR6 difference is irrelevant.
- Current production ownership uses Dr0/Dr1 plus Dr7 fields and claims owned hardware events from Dr6 B0/B1. Bit11/bit16 do not enable the watch or identify owned Dr0/Dr1 ownership.

Do not change production `restore()` or ownership behavior in c46.

## Current blocker

C45 has one unresolved interval:

```text
post_watch_set_readback_pre_resume
        ↓
worker resumes with owned watch armed
        ↓
first watched access / #DB delivery
        ↓
outer_db_handler_entry
```

The pre-Resume snapshot has bit11/bit16 = 0/0 and the #DB exception context has 1/1. C45 did not observe the worker **after Resume but before the watched access**.

Therefore it cannot distinguish:

1. watch-enabled Resume/context-load/API representation change before any #DB; from
2. Windows/#DB exception-delivery status evolution at the owned event.

C46 must close only this gap. It remains test-only.

## Current bounded Codex task

### 1. Add a deterministic pre-access gate to the owned fixture

Preserve the same worker, owned Dr0/Dr1 watch, foreign Dr2 state, continuation tickets and final poll sequence.

Add a test-only synchronization point that allows this sequence:

1. worker is suspended and the owned watch is installed as today;
2. existing `post_watch_set_readback_pre_resume` is captured;
3. worker is resumed with the watch still armed;
4. worker reaches a deterministic test-owned gate **before any access to the watched address**;
5. coordinator proves no owned hardware #DB has occurred yet;
6. coordinator suspends the gated worker and captures `CONTEXT_DEBUG_REGISTERS`;
7. coordinator resumes the worker and releases the gate;
8. only then may the worker execute the watched access and produce the existing owned #DB.

Name the new observation explicitly, for example:

```text
post_watch_resume_pre_access
```

Requirements:

- no new `SetThreadContext`/DR repair between watch installation and this observation;
- no read/write of the watched target by the gate itself;
- `outer_hardware_DB_events` must still be zero at the pre-access observation;
- prove the gate is reached before `ws_c41_machine` (or equivalent watched-access instruction) touches the watched target;
- preserve current thread/lease/generation identity and timeout bounds;
- if the gate cannot be made unambiguous, fail closed rather than infer ordering.

### 2. Refine the lifecycle classification

Keep all c45 stages and add the new stage.

For bit11 and bit16, classify the first observed transition as one of:

- `SETTHREADCONTEXT_ROUNDTRIP` — changed before initial Resume;
- `WATCH_ENABLED_RESUME_PRE_ACCESS` — 0 before Resume, 1 at the gated pre-access observation with zero #DB events;
- `DEBUG_EXCEPTION_DELIVERY_BOUNDARY` — still 0 at the gated pre-access observation and first 1 in the owned #DB exception context;
- `HANDLER_EDIT` — entry old, after-engine changed;
- `OS_RETURN_POST_CONTEXT` — handler context old, post-return current-thread state changed;
- `UNRESOLVED_OR_MULTIPLE` — ordering/evidence does not uniquely support one boundary.

Do not call `DEBUG_EXCEPTION_DELIVERY_BOUNDARY` direct hardware causality. The evidence is still user-mode Windows exception/context observation.

### 3. Audit exact Intel status semantics for this fixture

For the actual `GenuineIntel` owner profile, explicitly establish:

- the owned event is an ordinary data-breakpoint #DB from B0/B1 and not a bus-lock-detection event;
- Dr7.RTM / advanced-RTM-debug state used by this fixture;
- expected BLD11 and RTM16 status meaning at the event boundary under the current Intel manual;
- whether either field is configuration capable of enabling or resurrecting a hardware watch;
- whether either field participates in production ownership/lease admission;
- whether a pre-event raw `GetThreadContext` value is a valid configuration-restoration target after a completed owned #DB.

If current Intel text plus the Windows observations do not establish a safe interpretation, stop fail closed.

Do not weaken B0-B3, BD, BS, BT, Dr0-Dr3, or any Dr7 control/enable/address requirement.

### 4. Add a narrowly scoped test-only retirement-configuration result only if justified

Do not modify or relabel:

```text
RAW_EXACT
ARCHITECTURAL_CANONICAL
```

They remain historical/auditable and may remain false.

Only if the c46 pre-access observation plus current Intel/Windows documentation prove that bit11/bit16 are post-event status rather than unresolved watch configuration, add a separately named result such as:

```text
RETIREMENT_CONFIGURATION_SAFE
```

This is test-only and owner-configuration-specific. A true value must require at minimum:

- known supported `GenuineIntel` profile;
- unchanged mitigation admission;
- exactly the expected owned #DB event count/identity;
- complete ordered c46 timeline;
- unique accepted transition boundary;
- all continuation tickets acknowledged and pending debt zero;
- `canRetire()` true before final poll;
- active leases false after final poll;
- Dr0-Dr3 exact, including synthetic foreign Dr2;
- Dr7 passes the c44 architectural contract, with every meaningful/OS-controlled control exact;
- no owned B0/B1 cause remains after final poll;
- B2/B3, BD, BS, BT have no unexpected cause;
- DR6 fixed/unknown groups retain c44 fail-closed behavior;
- bit11/bit16 exactly satisfy the documented/observed event-status contract; no blanket mask;
- all final readbacks stable;
- no delayed saved-context debt remains according to the c41 witness mechanism.

A true test-only result is **not** a production retirement certificate and does not authorize exact-slot watch work by itself.

If any requirement is unproved, keep FAIL/REFUSED.

### 5. Preserve all prior counterexamples and behavior

Keep intact:

- c39/c40 delayed saved-CONTEXT resurrection reproduction;
- c41 continuation-ticket/witness ordering;
- close/deadline/kill behavior;
- nested/multiple context accounting;
- context-cap refusal;
- GPR/RFLAGS/RSP/stack/YMM/FXSAVE checks;
- foreign Dr ownership/refusal behavior;
- c42 mitigation classifier;
- c43 explicit restoration diagnostics;
- c44 raw + architectural comparator;
- c45 lifecycle timeline and historical FAIL.

No production code change may be justified merely by a green pure/synthetic test.

### 6. Add focused tests

Add pure/owned tests for the new pre-access phase and classifier.

At minimum cover:

- pre-access gate reached with zero #DB events;
- transition already present at pre-access => `WATCH_ENABLED_RESUME_PRE_ACCESS`;
- pre-access 0 and #DB entry 1 => `DEBUG_EXCEPTION_DELIVERY_BOUNDARY`;
- gate/access ordering ambiguous => refuse;
- extra #DB before gate => refuse;
- handler-only transition classification;
- post-return-only transition classification;
- multiple transitions => refuse;
- incomplete stage => refuse;
- final owned B0/B1 remains => refuse;
- unexpected B2/B3/BD/BS/BT => refuse;
- Dr0-Dr3 mismatch => refuse;
- Dr7 meaningful/control mismatch => refuse;
- foreign Dr2 mismatch => refuse;
- pending/lease/ticket failure => refuse;
- one fully valid synthetic status/configuration case if and only if the comparator is implemented.

Synthetic tests are not native runtime evidence.

### 7. Produce a c46 standalone probe only after owned validation

If the bounded repository work reaches a justified native question, publish only a test-owned artifact under:

```text
analysis/native-probes/c46
```

Prefer the same one-command owner run:

```powershell
.\continuation-witness.exe --position --multiple --json
```

JSON must preserve old raw fields and include:

- the new `post_watch_resume_pre_access` stage;
- proof/no-proof that zero #DB events occurred before that stage;
- bit11/bit16 transition classification;
- RAW_EXACT;
- c44 ARCHITECTURAL_CANONICAL;
- optional c46 RETIREMENT_CONFIGURATION_SAFE;
- all existing mitigation/vendor/lease/pending/ticket/final-readback evidence.

No game is required for this OS/fixture question.

## Forbidden scope

C46 does not authorize:

- production integration of the continuation witness;
- production `restore()`/ownership changes;
- broad DR6 masking/normalization;
- treating all DR6 status as irrelevant;
- the deferred `module_base + 0xC2D2580` exact-slot watch;
- slot qword reads/logging or pointer chasing;
- publication/source-chain interpretation;
- producer `0x351EB00` traversal;
- PAGE_GUARD or broad memory interception;
- Windows mitigation disabling/relaxation;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38 / package0.1.56`. Do not create a game release for c46.

## Required validation

Run from repository root:

```bash
bash source/tools/test_continuation_mitigations.sh
bash source/tools/test_continuation_diagnostics.sh
bash source/tools/test_debug_register_comparison.sh
bash source/tools/test_dr6_lifecycle.sh
bash source/tools/test_continuation_witness.sh
bash source/tools/test_watch_retirement.sh
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

Run any new c46 focused pre-access/status/configuration test explicitly and report its exact command.

Expected repository result:

- c45 native FAIL remains preserved unchanged;
- the watch-enabled Resume-to-first-access interval becomes observable or explicitly remains unresolved;
- no production restoration/watch semantics are changed;
- any new retirement-configuration comparator is narrow, documentation-backed, owner-profile-specific and fail-closed;
- all prior c39-c45 regressions retain their semantics;
- a c46 standalone probe is produced only after owned validation passes.

If any required test is not run, state the exact reason. Do not report unexecuted native-Windows/game tests as PASS.

## Required outputs / handoff

Update as appropriate:

- test-only pre-access/lifecycle diagnostic implementation;
- focused tests;
- `analysis/` with accepted c45 native result and c46 reasoning/evidence boundaries;
- `analysis/native-probes/c46` artifact/README/manifest only after validation;
- `docs/PROJECT_STATE.md` so c45 native lifecycle result is historical and c46 is current;
- `docs/CODEX_RESULT.md`;
- relevant Wisdoms facts only if a reusable Windows/x86 rule is actually established.

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

No Update-WS, game launch, A/E comparison, WallRun capture, offline slot scan or game log is required for c46 repository work.

If a tested c46 standalone artifact is committed and pushed, request the minimum native-Windows standalone command needed to answer the remaining boundary question. Prefer one command and no game launch.
