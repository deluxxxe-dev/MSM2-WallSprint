# c46 — watch-enabled Resume / pre-access DR6 boundary

2026-10-27 JST, branch codex/v01c20-edi-value-provenance, base5356dd2.
Runtime v0.1c38/package0.1.56; owned standalone only, no game release.

## Accepted c45 native evidence remains FAIL

Owner ran published d48b27acb71024409a55642fbe3226af7494aac6 on native Windows,
GenuineIntel, Position/multiple, admitted actual policies[0,0,256]. Exactly one
watch context write and one outer owned hardware DB were observed. Three
continuation contexts were acknowledged; pending0/canRetire true, false final
poll, inactive leases, exact Dr0-Dr3/foreign Dr2 and three raw-stable readbacks.
Saved Dr6=0/Dr7=0x10 became Dr6=0xFFFF0FF0/Dr7=0x410. RAW_EXACT and c44
ARCHITECTURAL_CANONICAL both failed; no RETIREMENT_STATUS_SAFE was added.

Eleven phases were complete: setup/readbacks through watch-before-Resume had
11/16=0/0; actual DB entry had1/1 and B0, the unchanged engine cleared B0, and
11/16 remained1/1 through post-return/poll/all three readbacks. C45's single
HARDWARE_DB_ENTRY label meant first observed context boundary, not CPU causality.
It did not observe Resume-with-watch-before-first-access. Do not relabel c45 PASS.

The supplied files are intentionally different owner-authored extracts and remain
individually unchanged:

- analysis/V01C45_NATIVE_WINDOWS_RESULT.md SHA256
  324d68a47e1095ee3fc0c6b42b20afed8d38b769d62e63f5d08e858bf2df4ecf.
- source/analysis/V01C45_NATIVE_WINDOWS_RESULT.md compact summary SHA256
  b46fd5ee04e1b9dbda67b3175e8b2be68450a29e2cf588ced2ce964ffb88146b.

## Deterministic owned gate / static ordering proof

The existing ready/go wait, production watch installation/readback and initial
Resume are retained. After go, the same worker records its actual thread and DB
count, publishes gate-reached, signals preAccessReady, then waits at an initially
unsignaled separate preAccessRelease event. Only the coordinator can release it.
Only AFTER that successful bounded wait and captured/release flags does the
worker increment machineEntries and evaluate the target address for the sole
ws_c41_machine call. The unchanged assembly's sole watched store remains intact.

Gate events/atomics are separate globals/kernel event objects, disjoint from the
watched VirtualAlloc allocation. Gate code receives no watched pointer and does
not read/write it. No call into the machine/target evaluation occurs before gate
release. Source control flow plus actual machineEntries0 at capture establishes
the bounded no-access observation; it is not an inferred absence from DB counts.

The coordinator waits for arrival with the existing2000ms bound and verifies:

1. actual arrival worker identity and DB0;
2. DB0 immediately before SuspendThread, whose previous count must be0;
3. once suspended: DB0, machineEntries0, published tickets0, pending0, live owned
   lease/slots and exact actual window lease/generation;
4. release event still unsignaled, capture flag/release flag still false;
5. full Dr0/1/2/3/6/7 via one additional GetThreadContext;
6. DB0 after that read; one original production context-write attempt at both
   watch readback and gate (no additional Set/repair); matched owned slots;
7. ResumeThread previous count1, then publish immutable gate evidence, validate
   it, publish captured/release flags and release the worker to its only access.

Gate capture checks watched address registers including foreign Dr2 exactly,
Dr7 meaningful/OS-controlled fields exactly, unknown/reserved-high fields clear,
Dr7.RTM0 and no B0-B3/BD/BS/BT cause. Fixed representation differences at the gate
are observations only, not permission to relax the unchanged final c44 contract.
Failed/missing/order/counter/identity/read/write-count evidence refuses. Ordinary
capture performs no allocation/formatting/repair while the worker is suspended.
Observation and gate records publish atomically and are immutable thereafter;
partial records are never treated as complete. No assertion/deadline/event cap,
context cap, production lease policy or continuation requirement is weakened.

## Reports / classification

Schema6/taskc46 retains final_restoration(c43), debug_register_comparison(c44)
and dr6_lifecycle(c45-v1, original11 phases/helper/classification). Legacy sequence
numbers have a deliberate gap for the new observation. pre_access_lifecycle is
c46-v1 and contains all12 ordered stages, inserting post_watch_resume_pre_access
between watch-before-Resume and actual outer DB entry. It reports gate proof,
all actual counters/identity/write attempts, full six registers, bit11/16, origins,
sequences and every transition. Cap negatives have eight captured phases and
retain missing poll/readbacks as unknown; they never certify final retirement.

For each bit separately, a complete gate-supported single transition classifies:

- foreign/watch before initial Resume: SETTHREADCONTEXT_ROUNDTRIP;
-0 before Resume and1 at zero-DB pre-access: WATCH_ENABLED_RESUME_PRE_ACCESS;
-0 at zero-DB pre-access and1 at DB entry: DEBUG_EXCEPTION_DELIVERY_BOUNDARY;
- handler only: HANDLER_EDIT; post-return only: OS_RETURN_POST_CONTEXT.

Incomplete/ambiguous/extra-event/multiple/unisolated observations remain
UNRESOLVED_OR_MULTIPLE. Every earliest stage remains visible. For a complete
multiple-change history first_transition_boundary labels only its first observed
boundary; classification/unique remain unresolved/false. No earliest-stage label
can grant acceptance. Unchanged histories are explicit UNCHANGED.

These are Windows suspended-API versus exception-CONTEXT views, not direct
privileged DR sampling. Suspension/readback itself changes timing; another
Resume occurs after pre-access sampling and before the store. Therefore even
DEBUG_EXCEPTION_DELIVERY_BOUNDARY does NOT prove a physical CPU change occurred
at delivery rather than in the intervening OS context-load path. No user-mode
snapshot series supplies a complete atomic hardware history.

## Current Intel / Windows documentation and exact fixture audit

Accepted current primary Intel reference remains owner-retrieved official SDM
Vol3B253669-090US, February2026,20.2.3, supplied for c45; newer evidence supersedes
old mirror-delivery leads. DR6 is last-debug-event STATUS. BLD11 is active-low for
bus-lock-detection DB, other debug exceptions do not modify it, and handlers
should leave/set11. RTM16 is active-low for advanced RTM debug, other DBs set it,
and handlers should leave/set16. These two fields do not enable/type/length a
watch or by themselves resurrect one. This is not a generic all-status mask.

The unchanged fixture uses an aligned plain scalar MOV, no LOCK/XBEGIN/XEND. A
SINGLE_STEP event at its exact exported post-store RIP, matching the worker and
exactly one owned Dr0/Dr1 lease, B0/B1 present and no B2/B3/BD/BS/BT is recorded
as the ordinary owned data DB. Every setup/pre-access/exception/final Dr7.RTM is
checked0; advanced RTM debugging requires Dr7.RTM AND IA32_DEBUGCTL.RTM, so the
conjunction is disabled without a privileged MSR read or setting change.

VERIFIED-STATIC: unchanged production own() uses Dr0/Dr1 addresses and Dr7 fields;
its event claim uses B0/B1. BLD11/RTM16 do not participate in ownership/lease
admission. Existing restore() may assign its old Dr6 snapshot only under its
original guarded condition. No production edit is made. A pre-event API status
snapshot is not independently proved a mandatory configuration-restoration
value after a completed DB. Existing exact/c44 tests still enforce their own
contracts; status/configuration distinction alone does not justify replacing
those contracts or issuing a production certificate.

Current Microsoft vendor-authored API sources were retrieved directly from
MicrosoftDocs/sdk-api/docs, processthreadsapi/nf-processthreadsapi-*.md:

- GetThreadContext:4479 bytes, SHA256
  82be7f41ca82e777e3a731bb9cb5169816bf86d4e6a4fd2bc70b48e3842b9597.
  Running-thread context is invalid; suspend first, select ContextFlags.
- SetThreadContext:4383 bytes, SHA256
  af689d9e657208757e6d1c8b74e7b757c711122c321fe6a3a5a6191619fd1618.
  Some non-specifiable state/global debug enables are silently OS-corrected;
  suspend before setting. No blanket DR6 equivalence is documented.

Exact source URLs/status/hashes are in V01C46_STATUS.json. Independent current
Intel SDM landing/PDF delivery remains proxy403; no PDF hash is invented and an
extraction hash is never called a PDF hash. No current-primary lead is erased
because of this cloud delivery restriction.

## Owned Wine measured trace — not native Windows

All twelve scenarios reached the gate with zero DB/machine/ticket/pending counts,
same identity and one original context-write attempt at watch/gate. Ten complete
runs have12 ordered phases and two cap negatives retain8. Dr6 is0 through the
new pre-access sample; DB entry is0xFFFF0FF1, after-engine/post-return0xFFFF0FF0,
then final poll/three readbacks0. Both bits first change across pre-access-to-DB:
first_transition_boundary=DEBUG_EXCEPTION_DELIVERY_BOUNDARY. Because they change
again after poll, overall classification=UNRESOLVED_OR_MULTIPLE, unique=false.
The c45 coarse view remains original HARDWARE_DB_ENTRY-first/multiple in Wine.
This does not replace native c45's one-transition history or establish native
c46 pre-access values. Original Wine raw-exact success/c44 canonical false remain.

## No RETIREMENT_CONFIGURATION_SAFE result added

The native c46 pre-access state is not yet measured, and the APIs do not certify
physical hardware history. Do NOT introduce configuration-safe true based on a
synthetic case or c45's incomplete interval. The new helper layers only negative
necessary-condition checks over c45's existing necessary-only audit. Even its
true output is not acceptance. RETIREMENT_CONFIGURATION_SAFE_added=false and
RETIREMENT_STATUS_SAFE_added=false remain explicit. No synthetic status-safe
acceptance case is claimed because no such comparator is implemented.

Native still requires original c44 canonical equality/stability plus original
lease/debt/witness/register/stack/vector/lifetime gates, the original c45 timeline
checks and new complete gate/unique-boundary checks. Wine keeps original scoped
RAW_EXACT acceptance, never native/architectural/configuration proof. Unknown/
non-Intel/unsupported prerequisites refuse. No production integration, exact-slot
watch, slot data, producer traversal or new gameplay write is authorized.

## Validation / remaining owner evidence

Focused command: bash source/tools/test_pre_access_lifecycle.sh. Preserve the
unchanged c45 focused command too. Required commands/counts/initial compiler
warning correction, actual Wine observations and remaining unavailable checks
are recorded in CODEX_RESULT and V01C46_STATUS.json. Standalone publication under
analysis/native-probes/c46 is allowed only after all required owned tests pass.
Pin exact tested binary, all code/tool input hashes, licenses and source commit.
Preserve c41-c45 artifacts and distinct supplied owner summaries byte-for-byte.

After tested publication, one native command from that c46 folder is sufficient:

```powershell
.\continuation-witness.exe --position --multiple --json
```

Return compact output even on FAIL. No Update-WS/game/Overstrike/A-E/WallRun/
scan/log or mitigation change. Native acceptance is not predeclared.
