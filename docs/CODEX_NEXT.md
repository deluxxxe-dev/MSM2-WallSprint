# Next task — c45 native DR6 status-lifecycle proof

## Status

C44 is complete as a repository/test task and the owner has run the published native-Windows standalone probe.

Accepted native result:

- `analysis/V01C44_NATIVE_WINDOWS_RESULT.md`
- `source/analysis/V01C44_NATIVE_WINDOWS_RESULT.md`
- `analysis/V01C44_DR_CANONICALIZATION.md`
- `analysis/V01C44_STATUS.json`
- `analysis/native-probes/c44/README.md`
- `analysis/native-probes/c44/PROBE_MANIFEST.json`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, all c39-c44 retirement/witness evidence, the current continuation-witness source/tests, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted c44 native evidence

Owner command:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Observed native identity and result:

```text
cpu_vendor                         = GenuineIntel
mitigation_flags                   = [0,0,0x100]
mitigation_classifier              = ADMIT
contexts                           = 3
pending before/after final poll    = 0 / 0
canRetire before final poll        = true
active leases after final poll     = false
final poll returned                = false
Dr0-Dr3 after                      = exact saved values
Dr6 saved                          = 0x0000000000000000
Dr6 after                          = 0x00000000ffff0ff0
Dr7 saved                          = 0x0000000000000010
Dr7 after                          = 0x0000000000000410
post-poll readbacks                = 3 complete, raw stable
RAW_EXACT                          = false
ARCHITECTURAL_CANONICAL            = false
c44 failure reason                 = meaningful_status_or_control_not_exact
```

The c44 Intel comparator accepts the Dr7 `0x400` fixed-one representation difference. The remaining c44-nonfixed Dr6 delta is exactly:

```text
0x10800 = Dr6 bit 11 + Dr6 bit 16
```

The c44 native run is therefore still **FAIL**, not a witness PASS and not a production retirement certificate.

## New primary documentation evidence

ChatGPT independently retrieved the current official Intel-hosted manual after c44 publication:

- Intel® 64 and IA-32 Architectures Software Developer’s Manual, Volume 3B
- Order number `253669-090US`
- February 2026
- Section 20.2.3, Debug Status Register (DR6)

Use the official current Intel publication when accessible and record exact provenance/hash/version. Do not keep the older c44 mirror-delivery limitation as if it were still the best available Intel evidence.

Section 20.2.3 states, in substance:

- DR6 is a **status** register reporting debug conditions sampled at the last debug exception; updates occur when an exception is generated.
- BLD bit 11 is active-low for an OS bus-lock-detection #DB. Other debug exceptions do not modify it. Intel recommends handlers set bit 11 to 1 before return to avoid stale-cause confusion.
- RTM bit 16 is active-low only for the special advanced-RTM-debug case and is set for any other debug exception. Intel recommends returning with bit 16 set.

This is a documentation lead for c45. It does **not** yet authorize masking bits 11/16 or changing production restoration.

## Accepted static implementation boundary

Current production watch behavior must be preserved.

From `source/src/msm2/orientation_trace.cpp`:

- watch ownership is determined from Dr0/Dr1 addresses plus owned Dr7 fields;
- the handler claims the owned hardware event from `Dr6 & 3` (B0/B1) for the two owned slots;
- `restore()` clears/restores owned B0/B1 and may restore the old snapshot under its existing guarded condition;
- Dr6 bits 11/16 are not used to enable a watch or to identify owned Dr0/Dr1 ownership.

This static fact is not by itself proof that bits 11/16 may be ignored in a retirement certificate.

## Current blocker

Determine the exact native-Windows lifecycle of DR6 bits 11 and 16 on this owned hardware-#DB path and decide whether the current pre-event-snapshot equality requirement is testing configuration restoration or incorrectly treating post-event status as persistent configuration.

C45 is still test-only. No production integration is authorized.

## Current bounded Codex task

### 1. Preserve the c44 failure unchanged

Keep the exact c44 native result as historical evidence. Do not relabel it PASS.

Keep reporting all three comparison levels separately if c45 adds another one:

```text
RAW_EXACT
ARCHITECTURAL_CANONICAL   # c44 contract
RETIREMENT_STATUS_SAFE    # only if c45 evidence justifies it
```

Do not weaken or silently replace the c44 comparator.

### 2. Instrument the exact DR6 lifecycle in the owned native fixture

Add test-only observations sufficient to locate when bits 11/16 change. Preserve the same worker/watch/continuation flow.

For the outer hardware-breakpoint context, capture at minimum:

1. `baseline_pre_foreign` — suspended worker debug state before the synthetic foreign Dr2 is installed;
2. `post_foreign_set_readback` — immediately after the synthetic foreign Dr2/Dr7 `SetThreadContext`, before any WallSprint watch is armed;
3. `post_watch_set_readback_pre_resume` — immediately after the owned Dr0/Dr1 watch `SetThreadContext` and readback, while the worker is still suspended and before it can hit the watched access;
4. `outer_db_handler_entry` — `EXCEPTION_POINTERS->ContextRecord` Dr6/Dr7 before the production handler mutates/restores the context;
5. `outer_db_handler_after_engine` — the same saved exception context immediately after the production handler returns, before c41/c45 redirects RIP or raises nested test exceptions;
6. existing post-return/pre-final-poll snapshot after all continuation witnesses are acknowledged;
7. existing final-poll after snapshot and all three stability readbacks.

Report full Dr0/1/2/3/6/7 for the relevant stages, plus explicit bit11/bit16 values and transitions. Keep output bounded to owned test state.

The new observations must not add a repair write merely to make the result pass.

### 3. Distinguish API normalization from debug-event status evolution

Use the timeline to classify the first observed transition of Dr6[11]/Dr6[16] as one of:

- `SETTHREADCONTEXT_ROUNDTRIP` — changed before the worker resumes;
- `HARDWARE_DB_ENTRY` — first changed in the actual owned #DB context;
- `HANDLER_EDIT` — changed by the current production/test handler edits;
- `OS_RETURN_POST_CONTEXT` — handler context shows the old form but the post-return witness/current-thread state shows the new form;
- `UNRESOLVED_OR_MULTIPLE` — evidence does not uniquely identify the boundary.

Do not guess. If observations perturb the mechanism or cannot establish ordering, fail closed and report the limitation.

### 4. Audit DR6 status semantics against the exact fixture

For the owner `GenuineIntel` path, establish from the current Intel manual and actual fixture state:

- whether the event is an ordinary data-breakpoint #DB and not a bus-lock-detection #DB;
- whether advanced RTM debugging is disabled in the fixture (including the relevant Dr7.RTM state); 
- what Intel requires/defines for BLD11 and RTM16 for this event;
- whether either bit can enable or resurrect a hardware breakpoint;
- whether either bit participates in current production ownership/lease admission;
- whether restoring the pre-event raw value is architecturally meaningful after an owned #DB.

Keep B0-B3, BD, BS, BT and every Dr7 control/enable/address field conservative. Do not generalize a conclusion about bits 11/16 to all DR6 status bits.

Unknown vendor or an unsupported feature/profile must refuse. The owner native evidence is Intel-specific; do not create an AMD certificate from it.

### 5. Add a narrowly scoped test-only retirement comparator only if justified

Only if current Intel documentation + static ownership audit + the new native/Wine test model support it, add an explicitly named test-only `RETIREMENT_STATUS_SAFE` comparison.

A candidate true result must require at least:

- known supported `GenuineIntel` profile;
- mitigation admission unchanged;
- all continuation tickets acknowledged and pending debt zero;
- `canRetire()` true before final poll;
- active leases false after final poll;
- Dr0-Dr3 exact, including synthetic foreign Dr2;
- Dr7 passes the c44 architectural contract and all meaningful/OS-controlled fields remain exact;
- no owned B0/B1 cause remains after final poll;
- B2/B3, BD, BS and BT show no unexpected foreign/debug cause in this fixture;
- Dr6 fixed/unknown groups satisfy the c44 fail-closed rules;
- BLD11/RTM16 satisfy the exact documented post-event status contract established by c45, not a blanket mask;
- all three final readbacks are raw-stable and status-safe-stable;
- the lifecycle evidence uniquely supports the claimed status transition.

If any of those are not proven, keep the native result FAIL/REFUSED.

Do not change production `restore()`, ownership logic or equality semantics in c45.

### 6. Add focused tests

Add pure/synthetic tests for the new lifecycle/status classifier. Synthetic tests are not native evidence.

At minimum cover:

- transition at SetThreadContext readback;
- transition only at #DB entry;
- handler restores pre-event value but post-return state changes;
- unresolved/multiple transition => refuse;
- BLD11 wrong for the established event => refuse;
- RTM16 wrong for the established event => refuse;
- owned B0/B1 still set after final poll => refuse;
- foreign/non-owned cause bit appears => refuse;
- Dr7 meaningful mismatch => refuse;
- Dr0-Dr3 mismatch => refuse;
- all required status-safe conditions true;
- incomplete readback/timeline => refuse.

Preserve all c39-c44 regressions.

### 7. Produce c45 standalone probe only after all owned tests pass

If the repository task reaches a justified native question, publish only a test-owned artifact under:

```text
analysis/native-probes/c45
```

Prefer keeping the owner command unchanged:

```powershell
.\continuation-witness.exe --position --multiple --json
```

The JSON must include the phase timeline, first-transition classification, RAW_EXACT, c44 ARCHITECTURAL_CANONICAL, optional c45 RETIREMENT_STATUS_SAFE, and all existing lease/pending/vendor/mitigation/readback evidence.

A c45 test-only status-safe PASS, if achieved, is still not production integration proof.

## Forbidden scope

C45 does not authorize:

- production integration of the continuation witness;
- changing production `restore()` or DR ownership behavior;
- broad masking/normalization of DR6;
- treating all DR6 status as irrelevant;
- the deferred `module_base + 0xC2D2580` exact-slot watch;
- slot qword reads/logging or pointer chasing;
- publication/source-chain interpretation;
- producer `0x351EB00` traversal;
- PAGE_GUARD or broad memory interception;
- Windows mitigation disabling/relaxation;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38 / package0.1.56`. Do not create a game release for c45.

## Required validation

Run from repository root:

```bash
bash source/tools/test_continuation_mitigations.sh
bash source/tools/test_continuation_diagnostics.sh
bash source/tools/test_debug_register_comparison.sh
bash source/tools/test_continuation_witness.sh
bash source/tools/test_watch_retirement.sh
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

Run any new c45 focused lifecycle/status test explicitly and report its exact command.

Expected repository result:

- c44 native FAIL remains preserved;
- current official Intel DR6 status semantics are documented with exact source/version;
- the exact phase where bits 11/16 first differ is observable or explicitly unresolved;
- no production restoration/watch semantics are changed;
- any c45 status-safe comparator is narrow, auditable and fail-closed;
- all prior c39-c44 regressions retain their semantics;
- a c45 standalone probe is produced only after owned tests pass.

If any required test is not run, report the exact reason. Do not report unexecuted native-Windows/game tests as PASS.

## Required outputs / handoff

Update as appropriate:

- test-only continuation/lifecycle diagnostic implementation;
- focused lifecycle/status tests;
- `analysis/` with c44 accepted native result and current Intel documentation proof;
- `analysis/native-probes/c45` artifact/README/manifest only after validation;
- `docs/PROJECT_STATE.md` so c44 native status-bit failure is historical and c45 is current;
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

No Update-WS, game launch, A/E comparison, WallRun capture, offline slot scan or game log is required for c45 repository work.

If a tested c45 standalone artifact is committed and pushed, request the minimum native-Windows standalone command needed to answer the remaining lifecycle question. Prefer one command and no game launch.
