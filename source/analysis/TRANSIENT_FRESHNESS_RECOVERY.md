# Transient freshness recovery — c7

## Evidence and state

VERIFIED runtime fact: c6 observed a250ms-old coherent active native Update for unchanged session/owner/Hero/Transform and epochs, permanently closed source ownership, then saw that same WallRun continue. The scheduler/hitch cause is UNKNOWN. The implementation is orientation-only, based on published c6 db874c7 and the authoritative main handoff.

Worker-owned states are Dormant, Active, StaleSuspended and HardClosed. Transition.active means retained transition ownership, not permission to write. During suspension both the atomic open gate and active publication are closed, active() is false, and original native calls pass through. The transition/target, session, original entry time, proof state, cumulative writes and retention maxima remain. A stale episode emits no NATIVE_SOURCE_EXIT and does not finalize read-only adhesion diagnostics.

## Freshness and recovery

The100ms native and worker limits are unchanged. Each admitted A/B entry acquires the actual production lifecycle publication, then samples time, validates owner/generation/init/native thread/accepted epoch/collision epoch, and checks age. This is repeated at the final source-store barrier. A recently refreshed worker snapshot cannot extend a native token's lease. A bounded publication read miss rejects that call; sustained unavailable evidence still hard-closes. No synthetic Update or keepalive call is added.

A stale entry publishes a bounded atomic signal (token/session/detection clock), without locking worker state or emitting per-call stale logs. The worker can record an episode even if a real Update arrives before its next poll. Signals are tied to session/owner/generation/epoch/thread and handled native-update index; unrelated or already-handled signals cannot create new episodes. Concurrent publishers may skip according to the existing nonwaiting AtomicPublication contract; write denial does not depend on signal delivery.

Resume requires a later accepted Update (lastKind2, updates strictly larger than the suspended index), fresh phase/time/age and the same verified context session, native owner/generation/init/thread/accepted epoch, no collision, same fresh Hero/Transform, unchanged owned surface and current caller/writer/lifecycle-patch/compact-proof authentication. A refreshed worker clock or reread of the old token cannot resume. Kill/shutdown and native close win.

The worker pins its permission epoch at session creation. Soft suspension alone advances it with compare-exchange; unexplained epoch changes hard-close, never become newly adopted permission. Thus a close racing suspension/publication cannot be used to reopen source ownership. Verified Deinit is drained before publication and again on an unexpected epoch after snapshot sampling. The final publication barrier checks that epoch. Deinit summary until1 requires a fresh matching active source/native token at the actual close.

## Transition semantics

At t1 recovery directly republishes the existing fixed SurfaceFrame target, using the original entry quaternion/target/session. Before t1, pause at the **last worker-published interpolation progress**, keeping that quaternion and original transition.entryMs. Resume sets a virtual-clock offset so the first output is the same progress, then advances with subsequent real time. This avoids counting unobserved stale time as interpolation progress or returning to WorldUp. The real5s-from-original-entry cap and per-path4096 store cap remain. Retention's current held interval resets at suspension; prior PASS/maxima survive and stale time is not counted as uninterrupted hold.

While writes are denied, vanilla may itself move orientation. c7 does not promise to eliminate every visual flash during a hitch: recovery deliberately restores the owned pose; actual visual behavior requires the human run.

## Hard close and scope

Verified Deinit, inactive/unverified context, changed session/owner/generation/init/accepted or collision epoch/native thread/Hero/Transform, surface invalidation, integrity/build loss, kill and rollback failure never resume inside the failed session. A new verified session may initialize a new state. Fatal owned hot-entry provenance/thread/identity/integrity faults latch kill and require restart. Known caller-body addresses with a wrong return are hard faults for the owned target; unrelated callers/other targets remain outside admission and pass through, because the common engine writer serves other code.

The lifecycle publisher/observer and source-store helper/assembly ABI are unchanged. No arbitrary engine caller is globally rewritten. A48/B36 bytes remain the only source writes; Position16/compact metadata stay untouched. P338 diagnostics remain read-only/capped64; PATH_A_POSITION_PROOF admitted0, corrections/source Position writes0. No new hook, native game setter, Actor write, page protection, movement or later roadmap work.

## Evidence bounds

Once per episode NATIVE_SOURCE_STALE_SUSPEND includes IDs, last update/ms/age, last observed Actor Up dot/validity, progress and episode index. NATIVE_SOURCE_STALE_RESUME includes duration/previous age/new age/new update/native gap/match flags and transition_restarted0. First16 pairs/session are logged; full counters/max remain in ORIENTATION_LIFECYCLE_SUMMARY. Rejected hot calls only increment counts. Duration starts at last accepted update timestamp+100ms (lease expiration), not at a potentially delayed worker poll; native_gap_ms gives the full last-old-to-new update interval. previous_update_age_ms is the maximum observed age before recovery; last Actor dot is explicitly a prior guarded observation, not a stale-time sample.

## Tests and limits

The actual production lifecycle/worker implementation and owned MinHook A/B callers reproduce a real >=270ms pause measured by GetTickCount64 at full pose. Both paths reject before the worker notices and while suspended, then resume on real native event2 without changing target/entry/session; retention and clean verified-Deinit until1 follow. A worker refreshed near the native deadline still cannot permit a stale source call, and its hot signal survives Update-before-worker recovery. Partial interpolation first output equals its pre-gap basis and later progresses to1. Eighteen episodes exercise the16-pair bound. Eleven stale-to-hard controls cover session, owner, Hero, Transform, Deinit, real native collision, kill, integrity, wrong native Update thread, generation and permission epoch; no revived source stores. Known wrong caller/stack and wrong hot thread also latch close; unrelated caller passes through.

Fixtures own all buffers/caller/writer functions and never execute captured game code. Init is backdated800ms to enter the configured delay; the270ms gap uses Sleep1 yields until the actual production clock has advanced sufficiently. Separate production lifecycle stress runs >=4 real seconds with4 readers. These are synthetic correctness checks, not real c7 game PASS. Exact accepted B writer/caller proof regressions still run privately without distributing bytes. Actual A Position whole-path proof and c7 visual flash/hold/crash/Position review remain pending.

Fixture admission now publishes normal Updates until full interpolation instead of assuming the first tick was admitted before a fixed Sleep. Waiting across freshness/sample boundaries uses the actual GetTickCount64 elapsed time; this avoids host/Wine timer granularity producing a nominal110ms sleep that misses a100ms gate. Production bounds/assertions are unchanged.

Final cloud validation: Windows build with warnings-as-errors, portable ASan/UBSan12 PASS, DLL smoke4 PASS, full hooks39 PASS /4 historical private-input SKIPs. Actual accepted B444/caller128 proof and production installer regressions ran. Final owned continuous recovery session4101ms native elapsed, A174/B174, one suspend/resume, maximum expired-lease duration174ms (full native gap274ms), retention PASS, clean verified-Deinit until1 and last Up dot1. Normal cadence session4110ms, A188/B188, held3036ms/188 updates, until1. Native Init is backdated800ms; the separate publication stress covers>=4 actual seconds. These are owned fixture results, not real game c7 evidence.
