# r4 — normal orientation lifetime follows authenticated native session

2026-10-31 JST. Baseline18c563a, branch codex/v01c20-edi-value-provenance.
Runtime v0.1c38r4 / package0.1.60. Prior START/duration tasks are not replayed.

Supplied r3 runtime evidence: V01C38R3_REPEATED_REVERSAL_RUNTIME.md. Session2
THREE completed reversals and session4 TWO, no stale episodes in those sessions,
then exactly5s source expiry. Session1 separately had10 stale pairs/max1385ms.
The missing video is not synchronized; reversal count is not shown as the cause
of closure. Earlier input-triggered START remains a separate later task.

## Static dependency review and minimum policy change

Transition.enter (Math/source_orientation.hpp:20–25) establishes original entry;
its sample clamps progress, with no5s certification. Native source tick's explicit
now-entryMs>=5000 branch was the sole normal total-runtime expiry. Removed only
that branch. Subsequent fresh authenticated same-session updates can republish
current orientation, no reversal-renewal/re-entry/generation reset. Original
entry, initial delay/interpolation and heading timing/duration are unchanged.

Actual A/B consumers capturePath/captureCopy recheck current code, immutable caller,
source/target/stack identity, native owner/thread/generation/permission epoch and
fresh100ms publication/token before store. No admission certificate or publication
TTL relying solely on5s was found. In particular, orientation_source.cpp
finish() only closes its bounded recording window; authenticated() checks owned
code/admission/kill, not recording-window age. orientation_source_thunk.S tests
ws_source_recording OR ws_native_source_enabled, so a completed diagnostic
window does not suppress normal source capture. reset_copy_source.cpp's
compactProofAuthenticated() rechecks admitted/current caller ownership without
5s expiry. Diagnostic window exhaustion does not grant or revoke source lifetime. Current-build/source proof scope is not expanded;
this is NOT a new whole-game ownership certificate. C52 remains FAIL.

Preserve suspend/recovery on missing/stale native updates, hard close on invalid
owner/generation/Hero/Transform/surface/code/Deinit/disable/F8. HardClosed same
generation still cannot enter; new genuine session resets state normally. Stale
virtual interpolation/heading clocks do not manufacture native freshness. Watch/
helper lifetime and independently bounded diagnostic windows remain unchanged.

Independent4096-per-path write/attempt budgets remain exactly unchanged. This
patch removes total TIME expiry, not all safety quotas: it cannot promise unlimited
stores or arbitrary reversal counts beyond exhausted independent write budgets.
No increase/reset-on-reversal is authorized. A quota rejection remains rejection.
At typical rates this quota can still become a later practical limit; changing it
requires separate review rather than calling it a diagnostic counter and removing it.

## Bounded long-session diagnostics

Existing heading64, movement40, plane64 and stale-pair16 quotas remain. Actor
observations previously emitted continuously even after a retention pass; now
cap emitted actor records at256/session, keeping retention/state validation on
every existing call. Periodic A/B counter snapshots now cap64/session. Neither
quota closes or gates source publication/stores. Fixed ring1024 and A/B write/
rejection log budgets retained; no dynamic per-session allocation added.
Package manifest now declares no total expiry, native-session policy and caps.
No shared updater/environment/profile/SF change; no new hook/watch/API/write type.

## Validation design

Existing native smoke fixture adds actual31.1s wall-clock same-session run,
NO backdated source entry or per-reversal generation reset. It uses synthetic
owned lifecycle events with actual GetTickCount64 timestamps and actual owned
MinHook A/B stores on fresh16ms worker
cadence: initial straight travel beyond5s, then at least20 completed alternating
reversals, still active beyond30s with original entry/session/generation. All
Position/W/compact metadata contracts remain in existing assertions. Check
heading64/actor256/movement40 caps, remaining path budgets, then actual Deinit,
closed-generation refusal, clean next session and disable passthrough. Retained
fault/stale/code/owner/normal/refusal suites remain. These are synthetic/Wine
fixtures, not owner real-game acceptance or arbitrary lifetime certification.

Exact commands/outcomes and publication are in CODEX_RESULT. This lifetime task
is committed/pushed before the separately authorized INPUT inventory begins.
