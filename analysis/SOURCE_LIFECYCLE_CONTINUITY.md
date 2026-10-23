# c5 source lifecycle continuity

## Verified implementation defects and runtime boundary

The four authenticated Win64 thunks invoke the existing native source callback BEFORE the original Init/Entry/Update/Deinit. Update timestamps are intended to advance throughout WallRun; they are not trace-window timestamps. The separate verified worker observer consumes the same hook events through its queue. No new hook, guessed ABI or engine field is needed.

In c4, `lifecycle()` and `event()` share an atomic_flag. A reader holds it while copying a token. If a native Update enters then, event cannot acquire the flag and increments a collision epoch. lifecycle subsequently requires that epoch equal the value saved at Init. Later successful updates refresh updateMs but cannot restore the epoch; return remains false until another Init. The worker sees false/observed=true and permanently closes ownership. This is a definite reader-induced failure mechanism despite a continuously active native Update stream.

An owned-token replay using the EXACT published c4 source/header holds the gate at its reader critical section, injects Update, then100 fresh same-owner Updates. It produces lifecycle=false/observed=true/phase3/freshness<=100ms, recovering only after a clean Init. This proves the mechanism, **not its occurrence in the private real-game run**. That run's exact sub-gate remains UNKNOWN until c5 diagnostics.

A second verified acquisition-order defect: c4 captures now BEFORE reading the token. A concurrent Update crossing a millisecond boundary can publish updateMs>that earlier now; the time-order gate then rejects an actually current token. The native regression constructs this ordering and confirms the old timestamp fails while a post-acquisition clock passes. Truly future timestamps are still refused.

## Narrow repair

Production source lifecycle uses the existing WallSprint AtomicPublication utility: atomic words plus version validation, at most8 bounded coherent-read attempts. Readers take no native writer gate, never modify the native token, and never prevent an event update. The native writer gate still serializes actual native publishers. A genuine event/event collision keeps the epoch invalid until a clean Init, preserving lost-event refusal. Invalid sequence, owner mismatch, disabled lifecycle and Deinit remain failures with explicit LifecycleRead status. A read miss is observed=false, not a fabricated Deinit, and never refreshes publication age.

Every native event still publishes its own GetTickCount64 update time; no worker timestamp is substituted for native freshness. The worker measures now AFTER token acquisition. The100ms native-update and100ms worker-publication limits, owner/generation/session checks and same-native-thread hook admission remain. Sustained missing evidence closes ownership; a genuinely failed session stays blocked until a new verified session. No force-native=true, grace-window widening or stale-timestamp renewal is added.

The worker retains the lifecycle epoch from BEFORE token acquisition. It accounts only for its own session-close increment. A native close racing a later Hero read/worker publication therefore wins; a stale pre-close token cannot adopt the newer close epoch. Native Deinit synchronously invalidates both A/B snapshots before original code runs. A regression injects Deinit during the worker's Hero sample and requires subsequent A/B passthrough.

## Bounded exit and Deinit evidence

NATIVE_LIFECYCLE_EXIT_DIAG is emitted once per entered source session, directly on the worker rather than through the lossy per-call record ring. It names failing_sub_gate and includes verified context, lifecycle return/status/observed, token phase/kind/owner/thread/updates/generation/times, update age, individual fresh checks, context-owner match, sampled Hero/Transform match, transition active and accepted/collision epochs. Logical failure names distinguish freshness, unavailable read, actual native collision, identity and generation conditions. Logging is not added to native callbacks.

The native close callback publishes a fixed-size private notice with token event time and the last coherent source snapshot. At matching verified Deinit the worker emits one ORIENTATION_LIFECYCLE_SUMMARY: native duration, source entry elapsed, first source exit reason, source_active_until_deinit, A/B counts, retained-threshold PASS, maximum held time/distinct updates, last active-source Actor dot and observation timestamp/scope. until_deinit=1 requires an active transition, synchronously open gate and matching coherent active source publication no older than100ms at the authenticated native Deinit, with unchanged session/generation/owner/Hero/Transform. A prior stale/identity/kill exit remains0. A shutdown/new-session fallback has verified_deinit=0 and cannot claim clean lifecycle completion.

Retention PASS remains the existing >=300ms OR>=20 distinct-update gate. A summary is sampled evidence, not proof that every intervening instruction or unobserved frame retained orientation. Maxima survive later losses; last_actor_observation_scope explicitly says active_source_worker_snapshot. Full-game visible hold still requires the human run.

## Tests and unchanged boundaries

`tools/test_source_lifecycle.sh` tests actual production publication for>=4 seconds with4 concurrent OS readers, coherent monotonic updates/identity/time and LastError preservation. It also requires genuine writer collision, invalid owner, Deinit and disable refusal.

`tools/test_native_source.sh` extends the existing real MinHook A/B fixture: >=4-second WallRun, commits continuing well beyond2 seconds, Surface Up/retention for>2 seconds with a concurrent lifecycle reader, then synchronous Deinit refusal and one clean summary. The diagnostic committed stub returns false after1.50 to model expiry; source ownership must continue. Real stale>100ms, future-time order, native collision, Hero identity change and Deinit-during-worker-read negatives are tested; stale sessions cannot silently re-enter after another Update. All data/functions are independently owned.

Exact exe/SHA and owned patches, Path B proof/hash/caller/compact0x24 boundary, Path A Position, row-scale/metadata preservation, raw ABI and lock-free A/B snapshot remain. c4 watch code, thread enumeration/classification and debug restoration are unchanged. Existing4096-write budgets and5-second-from-source-entry bound remain; the requested3–4-second human run is within them. No Actor/Position watcher write, third-writer patch, page protection, adhesion, movement, animation, camera or later system is added.

Real c5 full-WallRun hold and the exact former real-game exit subtype remain PENDING.
