# Reversal START timing review — 2026-10-31 JST

Docs-only review of08028ff2da3891a02f691a9af97452b9bf25c92f on
codex/v01c20-edi-value-provenance. Runtime remains v0.1c38r3/package0.1.59.
This supersedes duration-only solution/acceptance wording; no new tuning.

## Observed, supplied by Chat (not independently synchronized video analysis)

Owner wants START approximately1.1s earlier, not faster rotation/animation.
That number is a requested adjustment, not a measured input delay. The supplied
23.068633s video and r3 log have no established input timestamp or shared clock.
Session1 source exit17:10:57.972Z: bounded_test_window_5seconds,5.000000s since
source entry. Native lifecycle summary17:11:00.241Z: same exit reason and
wallrun_duration_ms7984. Session2 exits17:11:17.830Z at the same5.000000s bound.
Thus the source override CAN end before native WallRun ends. The emitted session1
lines are2.269s apart; buffered worker/log output is not an authenticated video
release time or precise physical Deinit delta. No reversal-causes-timeout claim.
R2 candidate-to-completion302–322ms did NOT include input-to-first-candidate.

## Verified-static caller/path inventory (paths relative to repository)

| Boundary | Actual source/caller and condition |
| --- | --- |
| Native lifecycle | source/src/msm2/hook_thunks.S dispatches ws_source_lifecycle_callback; source/src/NativeHooks/source_lifecycle.cpp:16–47 installs event(), publishes owner/generation/update/thread/time, invokes observer. It carries lifecycle, not directional input. |
| Synchronous close | source/src/NativeHooks/immediate_reset_trace.cpp:14–16,54 registers nativeEvent; phase!=3 calls lifecycleClosed(), native_source_override.cpp:375–378 immediately closes publication. Deinit details drained later. |
| Worker | source/src/msm2/telemetry.cpp:130–142 waits10ms then gathers context/guards, calls native_source::tick and drainLogs. This is scheduling cadence, not a hard10ms latency guarantee. Later telemetry100ms throttle does not throttle this tick. |
| Source admission/entry | source/src/NativeHooks/native_source_override.cpp:396–458 validates lifecycle/identity/current code/thread/normal; ctx.elapsed>=config.delay (INI0.70) permits Transition.enter(). |
| Initial interpolation | source/include/Math/source_orientation.hpp:20–33 sets entryMs=now and interpolates by rotateSeconds (INI0.25); native_source_override.cpp:463,466 does not run reversal observer until p.t>=1. Entry delay/interpolation are initial-session gates, NOT a new0.95s wait on every reversal. |
| Reversal observer | native_source_override.cpp:466–474 passes authenticated Snapshot.position, token.updates, GetTickCount64 and pause-aware headingClock to source/include/Math/reversal_heading.hpp:16–48. |
| Accepted/start | reversal_heading.hpp:45–48: second eligible opposite-sign observation, pendingCount>=2 and span>=33ms sets turning=true/turnStart=clock. This is logical START, not completion. On that call basis was already built from OLD sign (line34); positive angle is produced on a later distinct positive-time turning sample (lines20–32). |
| Worker publication | native_source_override.cpp:473 sets p.basis; line474 emits bounded REVERSAL_HEADING; lines479–482 recheck close/kill epoch then publish coherent identity/basis. Logging precedes this final check: accepted line is NOT proof publication/commit survived a close race. |
| Actual consumers | capturePath():499–526 (A) and captureCopy():529–619 (B), entered by existing orientation/reset-copy hook callbacks. They revalidate publication/lifecycle/current code/stack/thread and commit orientation source rows. Records include native update/A basis/commit QPC; a worker publication is not itself a game write or visible frame. |
| Source exit vs native Deinit | end():140–152 closes override/hard-blocks generation, does not call a game Deinit. drainVerifiedDeinit():313–319 consumes native-close publication, logs NATIVE_SOURCE_CLOSE and verified lifecycle summary. Native WallRun can outlive source exit; straight appearance is not unlimited WS ownership. |

## Conditions delaying START, not turn duration

After entry, first observation establishes position/time/update baseline; it
cannot vote. Each eligible observation needs a distinct native update and positive
time, dt>=33ms and<=250ms. Equal time/update WAITs; regressions/gaps rebase.
Plane-projected displacement must have speed>=2 and normalized dot with SAVED
entry Forward <=-0.8 (or>=+0.8 for the opposite reversal). These are displacement
classification thresholds, not an input edge. Low/zero speed, oblique motion or
current accepted sign clears pending. First new-sign observation is candidate;
second consecutive eligible new-sign observation at least33ms later accepts.
Existing turning ignores retargeting/displacement; completion clears history
before another candidate. Stale handling clears candidates and freezes heading
clock; identity/session/guard failure can close permission. Worker scheduling,
missing fresh updates, direction crossing the threshold, and time until a native
consumer next commits can add delay. None is measured as1.1s by supplied excerpts.
Changing120ms duration changes angle AFTER acceptance; it cannot move that
acceptance earlier. There is no existing1.1s reversal-start delay to subtract.

## Exact timer origin / independence

Transition.enter() at source admission assigns state.transition.entryMs=now
(source_orientation.hpp:25; caller native_source_override.cpp:454–456).
Worker checks now-entryMs>=5000 at native_source_override.cpp:462, before reversal
processing; end() latches HardClosed (line148) and same generation cannot re-enter
(line424). A reversal modifies heading.turnStart/sign only, not transition.entryMs.
Native Init resets generation/session; source timer is NOT anchored to reversal
or native Deinit. Stale resume rebases pausedTransitionMs/heading clock, NOT
entryMs or real5s budget (lines442–447). Stale early returns can delay the worker's
next timeout check; do not generalize supplied exact5.000000 exits into a universal
scheduler guarantee. Five-second bound, all gates and cleanup remain unchanged.

## Existing evidence and decision on diagnostics

REVERSAL_HEADING (64/session) already separates candidate, accepted/logical start,
observation_reset and turn_complete with session/update/ms/active clock, delta,
tangent speed, signs and target/published-basis CANDIDATE. MOVEMENT_DELTA_SAMPLE
is a separate100ms finite-difference diagnostic, not input intent. Source commit
records and actor observations are distinct from animation. NATIVE_SOURCE_EXIT,
NATIVE_SOURCE_CLOSE, stale pairs and ORIENTATION_LIFECYCLE_SUMMARY distinguish
source budget close from native lifecycle. Use native_ms/commit_qpc when available,
not emitted UTC lines as a shared frame clock. Counters/log drops/caps limit proof.

Silent hold/rejection reasons and every observation/first changed worker publication
are NOT exhaustively logged; no exact historical timeline is invented. Existing
records/source semantics suffice for the present supported conclusions: logical
start depends on already changed displacement;5s expiry is independent. Additional
logging of the same authenticated positions could refine detector-only timing but
cannot recover earlier intent. Therefore no runtime instrumentation/version bump
is justified for the decisive missing boundary in THIS review; no new values,
framework or redundant candidate/start event added. Exact publication/frame timing
also remains UNKNOWN where records do not establish it, not promoted to PASS.

## ONE smallest missing fact; no supported behavior patch yet

Need one independently authenticated wall-relative requested-direction/sign signal
that precedes displacement reversal, tied to the SAME session/native update and
monotonic clock as the observer. Its semantics, identity/lifetime and pre-motion
ordering must be evidenced before replacing the current trigger. Current Snapshot
(sdk_bridge.hpp:7) contains position/yaw/axes, not that signal; lifecycle Token
contains no directional intent. Incoming A/B source axes may contain prior WS
output, so they are not an independent earlier-intent channel. Existing F8/F9/F10/
digit marker polling is not authenticated game directional-input evidence.

No smallest behavior change is presently supported that achieves an earlier1.1s
start: guessing an offset, dropping confirmation, lowering thresholds or changing
rotation duration cannot establish the requested causal boundary. This is a
precise evidence blocker, not authorization to discover/capture a new API, request
full raw logs, replay stale duration task, extend/reset budget or start movement.
Chat judges the next bounded task; no owner update/game instructions here.
