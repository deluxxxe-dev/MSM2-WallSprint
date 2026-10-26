# Next task — c47 test-only retirement-configuration acceptance proof

## Status

C46 is complete as a repository/test task and the owner ran the published native-Windows standalone probe.

Accepted native result:

- `analysis/V01C46_NATIVE_WINDOWS_RESULT.md`
- `source/analysis/V01C46_NATIVE_WINDOWS_RESULT.md`
- `analysis/V01C46_PRE_ACCESS_BOUNDARY.md`
- `analysis/V01C46_STATUS.json`
- `analysis/native-probes/c46/README.md`
- `analysis/native-probes/c46/PROBE_MANIFEST.json`

Continue only on `codex/v01c20-edi-value-provenance`. Do not merge `main`.

Before work read `AGENTS.md`, `docs/DEVELOPMENT_MANUAL.md`, `docs/DEV_RULES.md`, `docs/LOCAL_WORKSPACE.md`, `docs/PROJECT_STATE.md`, `docs/CODEX_REPORTING.md`, this file, all c39-c46 retirement/witness evidence, the current continuation-witness source/tests, and relevant `deluxxxe-dev/MSM2-Wisdoms` references.

## Accepted c46 native evidence

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
RETIREMENT_CONFIGURATION_SAFE_added = false
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

The c46 pre-access gate proved all of the following at the new observation:

```text
gate reached/captured               = true
release unsignaled at capture       = true
active lease / owned slots          = true / true
DB_events_at_arrival                = 0
DB_events_before_suspend            = 0
DB_events_at_capture                = 0
DB_events_after_read                = 0
machine_entries_at_capture          = 0
tickets_at_capture                  = 0
pending_at_capture                  = 0
context_writes_at_watch             = 1
context_writes_at_capture           = 1
```

The exact native transition was:

```text
post_watch_set_readback_pre_resume   bit11=0 bit16=0 Dr6=0x0000000000000000
post_watch_resume_pre_access         bit11=1 bit16=1 Dr6=0x00000000ffff0ff0
outer_db_handler_entry               bit11=1 bit16=1 Dr6=0x00000000ffff0ff1
outer_db_handler_after_engine        bit11=1 bit16=1 Dr6=0x00000000ffff0ff0
post_return_pre_final_poll           bit11=1 bit16=1 Dr6=0x00000000ffff0ff0
after_final_poll                     bit11=1 bit16=1 Dr6=0x00000000ffff0ff0
verification_readback_1..3           bit11=1 bit16=1 Dr6=0x00000000ffff0ff0
```

Both bit11 and bit16 therefore classify as:

```text
WATCH_ENABLED_RESUME_PRE_ACCESS
```

The transition occurs after the watch-enabled worker is resumed but before any watched access, machine entry, continuation ticket, pending debt, or owned #DB event. This supersedes c45's earlier first-observed `HARDWARE_DB_ENTRY` boundary. Do not claim the exact kernel/processor mechanism; c46 proves the bounded user-mode observation order, not internal causality.

## Documentation/static boundary

Use the current official Intel SDM and current Microsoft thread-context documentation when accessible.

Preserve these facts/distinctions:

- Intel defines DR6 as a **debug status register**.
- Intel defines DR7 plus Dr0-Dr3 as the breakpoint control/address configuration.
- DR6 BLD11 and RTM16 have specific status semantics and are not breakpoint address/enable/type/length fields.
- Microsoft documents that some thread-context values that cannot be specified are silently set to the correct value, including OS-controlled debug-register state; this is not a blanket permission to ignore DR differences.
- Current production ownership is based on Dr0/Dr1 addresses plus owned Dr7 fields and claims the owned event through DR6 B0/B1.
- Production does not use DR6 bit11/bit16 to enable a watch or identify the owned Dr0/Dr1 slots.

C46 now provides native evidence that the bit11/16 representation changes **before the watched access/#DB**, so the owned event itself is not a necessary cause of this transition. That still does not prove a general Windows/Intel rule for arbitrary CPUs/processes/threads.

## Current blocker

Define whether the exact owner-native c46 state is safe as a **watch-configuration retirement** even though historical raw DR6 equality and the deliberately conservative c44 architectural comparator remain false.

C47 may add one separately named, test-only predicate. It must not relabel or weaken the historical comparators.

This task is still not production integration.

## Current bounded Codex task

### 1. Preserve all historical comparisons/results unchanged

Keep reporting:

```text
RAW_EXACT
ARCHITECTURAL_CANONICAL
```

For the accepted c46 owner result both remain false. Do not modify their definitions to make c47 green.

Preserve c44/c45/c46 native FAIL evidence unchanged.

### 2. Implement a narrowly scoped test-only `RETIREMENT_CONFIGURATION_SAFE` predicate only if justified

Review current Intel and Microsoft documentation plus the exact c46 native timeline. If that evidence is sufficient, add a **separate** test-only predicate named exactly:

```text
RETIREMENT_CONFIGURATION_SAFE
```

Its meaning is only: the owned hardware-watch **configuration** is retired and foreign configuration preserved under this exact supported owner profile, while DR6 bit11/bit16 are accepted only through the separately proven c46 status/representation contract.

A true result must require every item below. Missing/unknown data refuses.

#### Environment/profile

- `environment=native_Windows` for native acceptance;
- `cpu_vendor=GenuineIntel`;
- the exact supported Intel documentation/profile is identified;
- c42 mitigation classifier remains admitted;
- no debugger/unsupported mitigation/profile boundary is crossed;
- unknown vendor or unsupported feature combination refuses.

#### c46 ordering proof

Require the complete ordered c46 lifecycle and all pre-access gate facts:

- `post_watch_set_readback_pre_resume` captured;
- `post_watch_resume_pre_access` captured;
- release was unsignaled at capture;
- same worker/thread/lease/generation identity;
- active lease and owned slots were present at capture;
- zero owned #DB events at arrival/before-suspend/capture/after-read;
- zero machine entries at capture;
- zero continuation tickets and pending debt at capture;
- exactly one original watch context write;
- no intervening SetThreadContext/repair write;
- bit11 and bit16 each have exactly one accepted first transition `0 -> 1` at `WATCH_ENABLED_RESUME_PRE_ACCESS`;
- no later reversal/drift in the native lifecycle/readbacks.

Do not accept `HARDWARE_DB_ENTRY`, ambiguous/multiple, handler-only, or post-return-only lifecycle classifications for this c47 predicate.

#### Actual watch configuration retirement

Require:

- all continuation tickets acknowledged;
- pending context debt zero;
- `canRetire()` true before final poll;
- final poll has the expected false return;
- active leases false after final poll;
- Dr0 exact saved value;
- Dr1 exact saved value;
- Dr2 exact synthetic foreign saved value;
- Dr3 exact saved value;
- Dr7 satisfies the c44 vendor-specific contract;
- all owned Dr7 local-enable/type/length fields are retired to their saved state;
- no global/foreign Dr7 control is lost or silently accepted;
- no owned B0/B1 cause remains after final poll;
- B2/B3, BD, BS and BT show no unexpected cause;
- all c44 DR6 fixed/unknown-group fail-closed checks continue to pass;
- final three readbacks are complete and stable.

#### Bit11/bit16 special contract

Do **not** mask bit11/bit16 generically.

For this exact c47 predicate, accept their raw saved/current difference only when all of the following hold:

- saved pre-watch representation is `bit11=0, bit16=0` as actually observed;
- immediate pre-Resume watch readback remains `0/0`;
- first post-Resume/pre-access readback is `1/1` with all c46 zero-event/gate proof above;
- final state and all three verification readbacks remain `1/1`;
- current Intel documentation classifies these fields as DR6 status fields, not breakpoint configuration fields;
- neither bit participates in the current production watch enable/address/type/length ownership identity;
- no BLD/RTM event interpretation inconsistent with this fixture is present.

Any different values, transition order, profile, event/cause, missing readback, or instability must refuse.

This is a positive match for one measured lifecycle, not a universal `ignore 0x10800` mask.

### 3. Keep raw evidence visible and auditable

Root JSON for a c47 standalone run must expose all three results independently:

```text
RAW_EXACT
ARCHITECTURAL_CANONICAL
RETIREMENT_CONFIGURATION_SAFE
```

If the c46-style native state is accepted by the new predicate, the expected relationship is allowed to be:

```text
RAW_EXACT=false
ARCHITECTURAL_CANONICAL=false
RETIREMENT_CONFIGURATION_SAFE=true
```

That is intentional and must be explained in the JSON/report. It means the exact raw status representation differs while the narrowly defined watch configuration is retired; it does **not** mean the older comparators were wrong.

### 4. Add focused fail-closed tests

Add a pure/owned test helper for the c47 predicate. Synthetic tests are not native evidence.

At minimum test:

- exact accepted c46 lifecycle/configuration fixture => true;
- bit11 transition at SetThreadContext roundtrip => false;
- bit16 transition at SetThreadContext roundtrip => false;
- transition first at #DB entry => false for this narrow c47 predicate;
- multiple/reversed/drifting transition => false;
- extra #DB before pre-access gate => false;
- machine entry before capture => false;
- extra context write/repair => false;
- missing pre-access gate proof => false;
- Dr0 mismatch => false;
- Dr1 mismatch => false;
- foreign Dr2 mismatch => false;
- Dr3 mismatch => false;
- Dr7 owned enable/type/length mismatch => false;
- Dr7 foreign/global mismatch => false;
- owned B0 or B1 remains after final poll => false;
- unexpected B2/B3/BD/BS/BT => false;
- unknown DR6 group => false;
- bit11 or bit16 final value not 1 => false;
- one final readback missing => false;
- final readback drift => false;
- pending debt nonzero => false;
- active lease remains => false;
- `canRetire()` false => false;
- continuation ticket unacknowledged => false;
- unknown/non-Intel vendor => false;
- mitigation refusal => false.

Preserve every c39-c46 regression.

### 5. Update only the test-owned standalone path

If the predicate is justified and all owned tests pass, publish a c47 standalone under:

```text
analysis/native-probes/c47
```

Prefer the same owner command:

```powershell
.\continuation-witness.exe --position --multiple --json
```

If the new predicate passes natively, use an explicit test-only result name such as:

```text
OWNED_WITNESS_CONFIGURATION_PASS
```

Do not reuse a generic production-sounding PASS without the qualification. JSON must include:

- raw mitigation/vendor fields;
- complete c46 lifecycle/pre-access gate evidence;
- RAW_EXACT;
- ARCHITECTURAL_CANONICAL;
- RETIREMENT_CONFIGURATION_SAFE;
- final Dr0/1/2/3/6/7 and all three readbacks;
- pending/lease/ticket/canRetire/final-poll state;
- `production_integrated=false`;
- `exact_slot_watch_enabled=false`;
- `game_used=false`.

### 6. Explicitly audit production relevance without integrating it

Document which c47 conditions already exist in production and which are test-fixture-only or still missing for arbitrary production threads.

At minimum distinguish:

- VERIFIED-RUNTIME owner-native c46 facts;
- VERIFIED-STATIC production ownership/restore facts;
- test-fixture-only synchronization/identity facts;
- UNKNOWN requirements for production-wide application.

Do not modify production `restore()`, ownership, watch installation, lease lifecycle, exception handler behavior, or equality semantics in c47.

A native c47 configuration-safe PASS would authorize only a **later explicit production-integration review task**. It does not authorize c39 exact-slot work directly.

## Forbidden scope

C47 does not authorize:

- production continuation-witness integration;
- production `restore()`/ownership/equality changes;
- broad DR6 masking or normalization;
- treating all DR6 status as irrelevant;
- the deferred `module_base + 0xC2D2580` exact-slot watch;
- slot qword reads/logging or pointer chasing;
- publication/source-chain interpretation;
- producer `0x351EB00` traversal;
- PAGE_GUARD or broad memory interception;
- Windows mitigation disabling/relaxation;
- gameplay/source/Position/input/movement/adhesion/camera/animation/physics writes.

Runtime remains `v0.1c38 / package0.1.56`. Do not create a game release for c47.

## Required validation

Run from repository root:

```bash
bash source/tools/test_continuation_mitigations.sh
bash source/tools/test_continuation_diagnostics.sh
bash source/tools/test_debug_register_comparison.sh
bash source/tools/test_dr6_lifecycle.sh
bash source/tools/test_pre_access_lifecycle.sh
bash source/tools/test_continuation_witness.sh
bash source/tools/test_watch_retirement.sh
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
python3 source/tools/package.py
```

Run the exact new c47 retirement-configuration test command separately and report it.

Expected repository result:

- c46 native FAIL remains preserved unchanged;
- raw/c44 comparisons remain unchanged and auditable;
- a c47 predicate exists only if documentation + native ordering + static ownership support it;
- the predicate accepts only the exact narrow measured configuration/status lifecycle and fails closed otherwise;
- all c39-c46 regressions retain their semantics;
- no production integration or exact-slot work is enabled;
- a c47 standalone artifact is produced only after owned validation passes.

If the evidence is not sufficient to justify the predicate, stop fail closed and document the precise missing guarantee instead of forcing a PASS.

If any required test is not run, report the exact reason. Do not report unexecuted native-Windows/game tests as PASS.

## Required outputs / handoff

Update as appropriate:

- test-only c47 retirement-configuration comparator/predicate;
- focused tests;
- `analysis/` with c46 accepted native result and c47 evidence boundary;
- `analysis/native-probes/c47` artifact/README/manifest only after validation;
- `docs/PROJECT_STATE.md` so c46 native pre-access result is historical and c47 is current;
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

No Update-WS, game launch, A/E comparison, WallRun capture, offline slot scan or game log is required for c47 repository work.

If a tested c47 standalone artifact is committed and pushed, request at most one game-free native-Windows standalone run:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Return only its compact output/JSON. A native `OWNED_WITNESS_CONFIGURATION_PASS` would still be test-only and would lead to a separate production-integration review task, not directly to exact-slot watch work.
