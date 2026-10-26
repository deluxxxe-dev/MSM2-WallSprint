# c40 — bounded retirement review: implementation acceptance NOT MET

Base2b8124b, branch codex/v01c20-edi-value-provenance. Read current AGENTS,
manual/rules/workspace/state/reporting/NEXT, c39 evidence, actual reset/Position
watch/helper/test code and relevant Wisdoms references. No main merge/reset.

**Outcome: CODEX_NEXT section5 fail-closed fallback.** A safe general bounded
retirement mechanism is not established. Production watch source remains
unchanged, with its known c39 limitation. Do not interpret green regression
records as a c40 fix. Runtime stays v0.1c38/package0.1.56; no c40 game artifact,
slot mode or retirement certificate is issued. The positive c40 acceptance
condition (no false final retirement / no later resurrection) remains NOT MET.

## What the extended owned execution establishes

Scope is owned native functions/memory/threads under the configured Wine10 watch
test environment, not actual game/real-Windows evidence. The original c39 held-
return counterexample and both mode-specific restoration assertions remain.
No production CONTEXT, lease flags or exception state is forged. No original
2000ms fixture wait, production window/deadline, event/thread/window cap or prior
assertion is increased/removed. Extra cases extend the same fixture:

| Case (reset and Position separately) | Check | Actual owned result |
| --- | --- | --- |
| Normal, acknowledged return | Writer's done event occurs after store/exception return; close and3 readbacks restore exact saved DR state without another watched access | PASS,2 records total |
| Held return, explicit close | Original real event; inflight0; real close; exact instantaneous restoration; release hold | Retired DR0/DR1 reappear; original c39 counterexample preserved |
| Held return, unchanged deadline | Wait until actual production deadline then pollReset(true), reason=deadline | Same resurrection; timeout is not a retirement witness |
| Held return, kill | Actual trace::kill then pollReset(true), reason=kill_or_shutdown | Same resurrection; kill closes capture but is not OS return acknowledgment |
| Repeated readback while return held | Three additional real suspend/GetThreadContext/resume samples all match saved state | Still resurrects afterward in all held cases; finite readback repetitions are insufficient |
| Later matching hit | Old immutable tombstone restores slots, records no new event; Position exact DR6 vs legacy reset status policy preserved | PASS as recovery, not repair-free bounded retirement |
| VCH capability probe | Register real AddVectoredContinueHandler, observe callback vs owned writer completion following a real VEH-handled hardware event | Registration succeeds, callback0 in both modes; UNAVAILABLE, not a passing VCH retirement test |

Focused result: **14 PASS records,2 UNAVAILABLE records**. Of the14,2 are normal
acknowledged closure and12 preserve3 held-return scenarios plus later recovery
in2 modes. These14 are not14 passing c40-fix requirements.

The VCH exploration initially assumed dispatch and failed its new2000ms wait.
The current probe explicitly races callback-entry vs writer-done events and
asserts the observed callback0/real-hit1/inflight0 result before reporting
UNAVAILABLE and normally restoring/tearing down. It does not synthesize dispatch,
extend a wait, remove the original held-VEH test, or label unexecuted VCH return-
hold coverage PASS. If a future environment actually invokes VCH, the same probe
holds its return and exercises the preserved counterexample path; that branch
is unexecuted here and must not be reported as verified.

## Candidate mechanisms and exact unresolved condition

**REJECTED by owned execution:** extra GetThreadContext readbacks, timeout/kill,
inflight0, lease bookkeeping0 and later-hit tombstones as a post-return witness.
Restoring *current* DRs is not retirement of a still-pending saved CONTEXT.

**Confirmed only in the owned fixture:** cooperative writer acknowledgment after
the actual exception return permits ordinary exact restoration. This handshake
is test code after the owned store, not a production event covering all game
threads/possible pending exception contexts. Importing the fixture's done event
as a production fact would invent the missing witness.

**UNKNOWN / not usable here:** VCH dispatch and post-OS-return ordering for this
hardware-exception path. In the actual owned probe dispatch is unavailable;
successful registration is not a callback/retirement guarantee. This does not
establish that VCH cannot work on all Wine versions or actual Windows.

Microsoft Learn requests returned proxy403, but the canonical MicrosoftDocs
sources were then read successfully through raw.githubusercontent.com. The
documentation does not supply the missing general post-return certificate:

- AddVectoredContinueHandler documents registration/ordering/removal, not an
  acknowledgment after all pending OS CONTEXT applications have completed.
- GetThreadContext documents selective ContextFlags and suspending a running
  thread first. It does not turn a current snapshot into saved-context retirement.
- QueueUserAPC requires an alertable target for ordinary delivery. The existing
  game-wide watch has no verified bounded all-thread alertable-wait contract.
- QueueUserAPC2 special callbacks can interrupt user-mode execution and nest;
  delivery is not synchronized with the interrupted thread. A callback may
  therefore run while a saved exception return is still pending. It is not by
  itself a post-return acknowledgment; no game APC is queued in this task.
- WaitForSingleObject establishes only the specified object's signaled state.
  The owned done event has a known post-store/return source; no analogous
  verified production signal is manufactured from it.
- RtlRestoreContext restores the specified caller context and describes special
  unwind/long-jump cases. It does not certify retirement of pre-existing saved
  contexts or an injected continuation's full preservation contract.

Sources: MicrosoftDocs/sdk-api, branch docs, sdk-api-src/content/errhandlingapi/
nf-errhandlingapi-addvectoredcontinuehandler.md; processthreadsapi/
nf-processthreadsapi-{getthreadcontext,queueuserapc,queueuserapc2}.md; synchapi/
nf-synchapi-waitforsingleobject.md; winnt/nf-winnt-rtlrestorecontext.md. Also
MicrosoftDocs/win32, docs/desktop-src/Debug/vectored-exception-handling.md
(handler ordering before stack unwinding is not post-return acknowledgment).
These are API contract inspections, not new real-Windows runtime measurements.

No existing authenticated mechanism exposes a per-thread post-OS-exception-
return acknowledgment covering every saved CONTEXT that can reapply an owned
DR configuration. Adding a counter/check at VEH/VCH exit repeats the interval
before OS return. Redirecting saved RIP to a new continuation/stack trampoline,
adding undocumented native-thread operations, or assuming APC delivery would
need independently supported dispatch/restoration/unwind/register/flags/XSTATE/
thread-lifetime/foreign-DR guarantees. None was verified here; these alternatives
were NOT implemented/tested or described as impossible. No arbitrary detour,
broad debugger or CONTEXT patch is shipped merely to make a fixture green.

Keeping all affected live-thread leases pending until a proven witness would be
honest fail-closed behavior, but clearing that debt on inflight0/current-readback
would be unsound. No generally available bounded discharge event has been
established. This task therefore does not retrofit a guessed debt-discharge rule
or weaken existing repeated-arm/cleanup contracts. Legacy cleanup remains a known
limitation; future exact-slot admission remains closed.

Precise next evidence: a supported and owned-tested post-return witness with
per-thread/lease identity, accounting for nested/delayed contexts and preserving
all existing debug restoration/foreign-state rules. Tests must hold the original
return past close/timeout/kill, observe honest pending/refusal, then establish
retirement without requiring a second watched access. A supplied API ordering
contract or a separately reviewed minimal continuation mechanism is needed;
another identical game log or slot scan cannot answer this prerequisite.

## Commands, required validation and limitations

Run from repository root (Bash with configured toolchains):

```bash
bash source/tools/test_watch_retirement.sh
bash source/tools/build_windows.sh
bash source/tools/test.sh
bash source/tools/smoke_windows.sh
bash source/tools/test_hooks_windows.sh
```

Exact expected/actual exits/counts and independent Git/package checks are in
docs/CODEX_RESULT.md and V01C40_RETIREMENT_STATUS.json. The full path retains
foreign-slot/ownership/refusal/suspend/resume/caps/repeated-window tests, plus
all previous native/scanner/workspace regressions. Production debugger-refusal
code is unchanged; no live attached-debugger experiment is claimed.

Unperformed: a successful new retirement mechanism's acceptance tests (no such
mechanism established/shipped); VCH return-hold2 cases (callback not invoked);
optional private-body positive replays (5 private inputs absent); actual Windows/
game execution (no such environment/game here, not needed for the explicit
fallback); new exact-slot tests (expressly unauthorized/deferred). No unexecuted
test is PASS. Existing package/ZIP/script outputs remain the c38 originals.

All c37 offset/c36 escape/CFG/publication/allocation/lifetime/ownership gates
and producer0x351EB00 deferral remain unchanged. No slot value/pointer chase,
new game data capture, gameplay/source/Position writes, SF/shared-tool/updater
change. Preserve task/AGENTS and the five original untracked artifacts.
No owner Update-WS/game/offline scan/runtime log is requested for this audit.
