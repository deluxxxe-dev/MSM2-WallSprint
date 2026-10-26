# c49 — mixed SEARCH / downstream EXECUTION / VCH boundary feasibility

2026-10-27 JST. Base53ad63810fc04255ebcaaff7880481b063764684 on
codex/v01c20-edi-value-provenance; no main merge/reset. Test-only.

**VCH_SEARCH_CONTINUATION_BRIDGE_UNAVAILABLE**. Six owned Wine runs reach
WS SEARCH and downstream EXECUTION with exact reserved context identity; the
real VCH registration succeeds but callback count is zero in every run.
Native Windows is not available in cloud. Neither registration nor successful
model checks constitute native dispatch/order, binding, witness or retirement
evidence. This does not reject VCH on native Windows or erase c40 UNAVAILABLE.

## Current primary contract and exact limits

Read current MicrosoftDocs/sdk-api docs commit
656e9656bfaef34dd29cd7ee9e1146aec17e924c, these files under sdk-api-src/content:

- errhandlingapi/nf-errhandlingapi-addvectoredcontinuehandler.md
- errhandlingapi/nf-errhandlingapi-removevectoredcontinuehandler.md
- winnt/nc-winnt-pvectored_exception_handler.md
- errhandlingapi/nf-errhandlingapi-addvectoredexceptionhandler.md

Also MicrosoftDocs/win32 docs commit ca8274decead49403d70ceeed4af8d4ca74eaf11,
desktop-src/Debug/vectored-exception-handling.md. All five current raw-primary
requests returned200, then each byte hash was checked against its commit-pinned
URL. Exact URLs/bytes/SHA256/retrieval date are in V01C49_STATUS.json. API files
carry ms.date2018-12-05; concept page2025-07-14. No manual body is redistributed.

**VERIFIED-STATIC contract inspection:** AddVCH documents first/last registration
order and allows a later first registration to supersede the current first.
RemoveVCH unregisters the specified handle; it supplies no documented all-pending-
context/stub lifetime join. Registered VEH/VCH callbacks remain registered if
their DLL unloads, which can cause errors. PVECTORED_EXCEPTION_HANDLER receives
EXCEPTION_POINTERS and documents SEARCH versus EXECUTION returns. It advises
against allocating or acquiring synchronization objects in the handler. The
general vectored page places vectored exception handling after debugger first
chance and before stack unwind; this is not a post-return witness contract.

**UNKNOWN:** none of those inspected texts precisely guarantees this mixed
hardware-DB VEH SEARCH -> following owner's EXECUTION -> VCH callback, identical
downstream-final CONTEXT, last-mutation ordering, or later OS resumption through
a redirected IP. Those remain runtime questions, not inferences from an API
name/registration. The c49 VCH itself does not rewrite any context and returns
SEARCH so it does not claim ownership of other VCH callbacks. One owned run
could not prove immutable ordering against future third-party registrations.

## Actual stage-2 probe and observation boundaries

New vch_search_bridge_smoke.cpp includes the unchanged orientation_trace.cpp in
its test translation unit, intercepting only installation to observe the original
handler's real result. It runs a single owned worker, retained kernel thread
handle plus GetThreadTimes creation epoch, an allocated4096-byte synthetic hero,
and a real overlapping Dr0/B0 plus foreign Dr2/B2 write watch. No game code/data.
No new source is linked into WallSprint.dll; no production API or header changes.

Ordered sequence (independent monotonic order numbers in JSON):

1. Capture original WS-entry CONTEXT at actual mixed B0+B2 DB.
2. Call unchanged production handler; capture exact after-engine state. Require
   SEARCH, B0 cleared, B2 retained, Dr0-Dr3/Dr7/RIP preserved and both owned slots.
3. Reserve immutable debt with worker/retained creation epoch, lease1,
   generation149, sequence1, CONTEXT and exception addresses and original RIP.
   Publish pending1 before allowing SEARCH to escape the wrapper. No RIP edit.
4. A separately registered following foreign owner authenticates exactly the
   reserved addresses, identity, order and the entire unchanged CONTEXT bytes.
   Its first test-owned hold lets the coordinator observe the reservation and
   request close, actual unchanged deadline, or kill without final polling.
5. That foreign owner clears **only its own Dr6.B2**, snapshots the final context,
   publishes its chosen EXECUTION disposition, and returns EXECUTION. A complete
   CONTEXT byte comparison checks that no other field changed.
6. A real last-registered AddVCH observer, if actually dispatched, captures thread,
   sequence, addresses and CONTEXT. It requires a matching reserved debt, order
   after the foreign final snapshot, chosen EXECUTION, exact entire downstream
   context, armed owned configuration and no residual B0-B3/BD/BS/BT. Mismatch,
   duplicate/additional event or ambiguity poisons/refuses. It can then be held
   on a separate bounded event before returning, without modifying CONTEXT.

The holds use existing2000ms owned-fixture waits; they are deliberate test
scheduling, not production-safe callback design. No wait/cap/deadline was enlarged.
All capture/reservation has fixed storage/lock-free atomic publication, no heap,
formatting or diagnostic event lock/ring in those observer sections. The original
engine's existing diagnostic behavior is unmodified. No allocation or prepared
executable stub exists in this observation-only probe. Callback-to-ticket binding
is not implemented or inferred from observing the addresses.

After releasing the foreign hold, the coordinator waits for actual VCH entry OR
the worker's real post-store completion. Wine takes the latter branch. It checks
callback0, pending1, active leases and the still-owned slots. No final poll or
live-thread retirement is allowed while this debt lacks an exact witness.
The worker then actually exits; the retained handle must be signaled before
trace::stop, handler deregistration and owned-data release. The report preserves
pending_at_worker_exit=1 rather than fabricating a marker or pending_after=0.
This is terminal-worker teardown, **not** a post-witness cleanup success.

JSON reports unknown/unexecuted VCH context, witness binding, acknowledgment,
post-witness pending and resurrection-after-final-cleanup as null. In particular,
still-armed slots after return are not called resurrection: this probe intentionally
did not perform the earlier cleanup. The unchanged c48 regression independently
tests actual cleanup-before-return and reproduces resurrection six times.

### Measured scope and primary decision

**VERIFIED-RUNTIME (owned Wine only):** reset/Position x close/deadline/kill all
reach order1 WS entry,2 WS-after,3 reservation,4 SEARCH escape,5 downstream-before,
6 downstream-final. WS clears B0 only; foreign clears B2 only. Actual policies
[0,0,0] admit under unchanged c42 classifier; CPU GenuineIntel. Same context,
exception, thread epoch/lease/generation/RIP remain bound within each process.
Each real AddVCH succeeds, callback0, reserved debt1 survives every cleanup
request until actual owned thread termination. No VCH-stage hold was exercised.

**UNAVAILABLE:** native VCH callback/order/final-context identity and stage3
post-return witness. Section3 of CODEX_NEXT permits a VCH-to-c41 bridge only
after stage2 succeeds. It did not succeed in the available environment; therefore
we do not build an unvalidated conditional redirection or modify saved RIP before
SEARCH merely to force progress. Primary UNAVAILABLE is not FEASIBILITY_FAIL:
the required callback/boundary could not be exercised, rather than an observed
native mechanism violating a tested property.

## Synthetic contract tests — separate evidence, not production design

The new header vch_search_bridge_model.hpp and vch_search_bridge_tests.cpp are
explicitly sequential pure test models, not an atomic concurrent production
ledger or OS dispatch simulator. They express required refusal/accounting
conditions using four never-reused slots. Reservation counts debt immediately,
including before publication/binding. Missing/invalid/duplicate reservation,
capacity, preparation/publication, changed final-context identity or wrong order
causes sticky poison. No close/deadline/kill method can clear debt. Exact
identity/token acknowledgment is required; marker alone never frees code storage.

Twelve ASan/UBSan model PASS observations cover reserve-before-SEARCH, downstream
EXECUTION, VCH-after-final model, reversed/ambiguous/no callback, context/exception/
thread/epoch/lease/generation/sequence mismatch, invalid/stale/duplicate identity,
nested/multiple and another-thread independent tickets, stale/duplicate/reused
token, cap/publication/preparation failure, pending close/deadline/kill and
marker-versus-code-exit lifetime. The byte/current-code/stub/OS/concurrency
premises are deliberately not synthesized as native evidence. These tests cannot
establish the c48 all-production coverage/credit/lifetime contract.

New command `bash source/tools/test_vch_search_bridge.sh` executes those12 model
observations, six real mixed-path Wine processes (6 UNAVAILABLE) and one independent
strict JSON validation PASS. It also calls the unchanged original c48 harness
(6 mixed counterexamples +2 foreign-only controls +1 parser PASS), so the focused
command totals22 PASS/0 SKIP/6 UNAVAILABLE. The9 c48 observations are not new
bridge evidence and are not added twice in the full hook chain. Existing c48
also remains directly runnable as its original separate command. Existing c39/c40 normal EXECUTION/VCH tests remain byte-identical:
their14 PASS/2 UNAVAILABLE do not become new VCH success. C41-c47 tests/helpers,
native evidence/binaries and c47 exact fixture-only predicate remain unchanged.

## Bounded owner handoff — stage2 question only

After owned validation passes, publish the exact tested PE at
analysis/native-probes/c49/continuation-witness.exe with licenses, hashes,
source-input provenance and README. It is the observation-only executable above,
not a game MOD or positive bridge. From that folder, one native PowerShell command:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

Return its compact output, including callback count, stages, debt, policy state
and reason. Native unknown/refused prerequisites exit77, observed contract failure
exit3; no mitigation setter/bypass. Unavailable or boundary-only observation may
exit0; **exit0 alone is not a feasibility PASS**. A real callback plus matched
downstream final context reports native_boundary_observed=true but keeps primary
UNAVAILABLE/bridge_implemented=false. That evidence is for a later stage3 design;
this binary cannot claim a post-return witness it does not implement.

No owner Update-WS/game/Overstrike/A-E/WallRun/offline scan/log is requested. Runtime
v0.1c38/package0.1.56 remains unchanged. Production restoration/ownership/handler/
lease/cleanup, c39/c40 and c48 counterexamples, c47 predicate, exact-slot watch
and all gameplay/write gates are unchanged. Full tested commands/counts and
explicit not-run native bridge matrix are in CODEX_RESULT and V01C49_STATUS.
