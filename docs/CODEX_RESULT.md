# Codex result — WallSprint v0.1c7

Branch: **codex/v01c7-transient-freshness-recovery**.
Implementation commit: **93d7887e8ba8cb04e38473a0a821077ee8fc9efb**.
Base: published c6 **db874c74cc14f361a2ceb5e15647910e06ebfb84**. The pre-created task branch was used; main handoff9f2cd28/f1f3816/0a72022 was imported as3e03fdb/093075a/3ec0c55. `docs/PROJECT_STATE.md` / `docs/CODEX_NEXT.md` remain byte-identical to main's handoff. No main rewrite or force push.

## Outcome

**v0.1c7 / package0.1.25 implements transient freshness suspension/recovery.** Native Update age>100ms blocks each A/B source commit without permanently destroying an otherwise matching WallRun transition. A later fresh accepted native Update for the same verified identity resumes the retained target/progress. Hard invalidations remain permanent within the failed session. **Real-game c7 lifecycle-wide hold and visual recovery are PENDING.** Position remains read-only; no adhesion correction or later milestone is added.

The measured c6 regression is authoritative: same session/owner/Hero/Transform and coherent epochs, native age250ms at elapsed2.642367, permanent stale exit, yet verified WallRun continued~1.4s to4078ms Deinit. Corrections/source Position writes were0. Historical c5 normal-cadence full hold3843ms remains verified; it does not establish c7 hitch recovery.

## Changes and files

- `source/src/NativeHooks/native_source_override.cpp`: explicit Dormant/Active/StaleSuspended/HardClosed semantics, retained transition with separate atomic write permission, pinned permission/accepted epochs and native thread. Soft suspension alone advances permission epoch by compare-exchange; unexplained native close cannot be adopted as permission to reopen. Freshness is checked from the production lifecycle publication at each owned hot entry and again at the pre-store barrier, alongside worker age. A stale hot token can signal the worker without locks/log spam so an Update-before-worker poll still gets episode evidence.
- Resume requires a strictly later real accepted Update, matching verified context session/owner/generation/init/thread/accepted epoch/no collision/Hero/Transform/surface, fresh phase/time/age, current caller/writer/lifecycle-patch/compact proof, no kill. Worker clock renewal alone cannot resume. Full t1 directly continues SurfaceFrame. Partial interpolation freezes at the last published progress and resumes its virtual clock from that same quaternion; original entry/session/delay/target and real5s budget remain.
- Bounded first16 SUSPEND/RESUME pairs per session include old/new native age, IDs, prior Actor dot/validity and progress; lifecycle summary has all suspension/resume counts and maximum stale duration. Duration begins at last native Update+100ms (lease expiry); native_gap_ms records the whole update interval. The current retention interval resets during suspension while historical PASS/maxima survive. Recovered stale episodes never emit NATIVE_SOURCE_EXIT or prematurely finalize read-only adhesion diagnostics.
- Hard failures include Deinit, unverified/inactive context, unexplained session/generation/owner/init/thread/epoch changes, Hero/Transform/surface change, native collision, integrity, kill and rollback failure. Known owned caller-body return mismatches, stack provenance/thread/identity/integrity faults latch kill; unrelated callers/other targets remain outside admission and pass through. Verified Deinit is drained again if it races worker snapshot sampling; final summary until1 also requires fresh matching native/source evidence at actual close.
- `source/src/msm2/native_source_override_smoke.cpp`: production lifecycle/worker plus real owned MinHook A/B writers, continuous~4.1s sequence with real>=270ms Update gap, hot and suspended write denial, same target/session/entry recovery, final clean Deinit; partial transition continuity; missed-worker signal;18 episodes/16-pair cap;11 stale-to-hard controls; known wrong caller/stack/thread negatives and unrelated-caller passthrough. Tests never execute game bytes.
- `analysis/V01C6_RESULT.md`, `TRANSIENT_FRESHNESS_RECOVERY.md`, `V01C7_GATE.md` and source mirrors, authoritative c6 runtime mirror, root/source `README_TEST.md`, source README/BUILD/WORKLOG/version/mod metadata, package/release recipes updated. Manifest distinguishes historical c5 PASS, measured c6 regression, synthetic c7 PASS and pending c7 real run.

## Preserved boundaries

Exact Spider-Man2.exe2.810.0.0 SHA256 and all existing caller/writer/patch/proof admission remain. The native lifecycle publisher, raw hook/source thunks, stack-store helpers, Path B installer/compact proof, hardware-watch/coordinator, Position inspector and quaternion transition math are byte-identical to c6. No new native ABI, hook, direct Actor write, Position store, page protection or keepalive game call. A48 orientation bytes / B36 compact bytes only; A Position16, B row scale/later metadata preserved. Position proof admitted0, read-only ADHESION_SAMPLE/SUMMARY, corrections/source Position writes0 remain.

Native and worker freshness are still100ms. Original real5s-from-source-entry and4096 writes/path limits stay. A bounded publication-read miss rejects the individual call; sustained unavailable evidence hard-closes. As before, an already-admitted short guarded store can overlap a close/kill. During suspension vanilla can change orientation; synthetic correctness does not prove absence of a visible flash in game.

## Validation

Completed exit0:

- Windows x64 build with WallSprint warnings-as-errors.
- Portable ASan/UBSan **12 PASS**, DLL smoke **4 PASS**, full hooks/offline/native suite **39 PASS**: **55 total**, with **4 explicit historical private-input SKIPs**.
- Exact accepted-build B writer444/caller128 proof and production installer regression ran privately: nine recovered pairs, mutation/caller/stack/decode negatives and owned hook restoration pass; no original game code executes or ships. This does not claim an actual A Position-body replay or Position permission.
- Final owned recovered sequence: native elapsed**4101ms**, gap**274ms** (real production clock, no fake Update), A**174**/B**174**, suspend1/resume1, max expired-lease duration**174ms**, retention PASS, clean verified Deinit, **source_active_until_deinit=1**, last Up dot1. The gap starts after full pose at~2.41s; resume at~2.684s stays t1/transition_restarted0. No stale EXIT before Deinit.
- Normal-cadence owned sequence:4110ms, A188/B188, held3036ms/188 updates, clean until-Deinit1. Init is backdated800ms in these entry-delay fixtures. Separate production publication stress spans>=4 actual seconds with4 concurrent readers and no reader-induced native collision.
- Partial recovery first output matches pre-gap basis/progress, then reaches1. Eleven stale-hard controls cover session/owner/Hero/Transform/Deinit/collision/kill/integrity/native thread/generation/permission epoch and never resume after later Updates. Fresh-worker/stale-native commit denial and signal recovery pass. Existing ABI/AVX/FPU/stack/metadata/Position/compact proof/watch restore tests remain PASS.
- Fixture timing failures during development exposed fixed-sleep assumptions at admission and100ms sample boundaries. Initialization now sends ordinary native Updates until actual full progress; boundary waits check actual GetTickCount64 elapsed time using Sleep1 yields. Production gates and assertions were not widened. Final full suite passed after these fixes.

A fresh standalone source ZIP extraction rebuilt the Windows DLL successfully without private captures in its source tree. Final archive validation checks CRC, exact current complete source/docs/README/licenses, nested script contents/hash, Position-disabled manifest and private full-function byte/hex exclusion. Only selected independent validation logs ship; no raw user or synthetic full-function trace is included.

## Outputs and human test

- **WallSprint_v0.1c7_transient_freshness_recovery.zip**
- **WallSprint_v0.1c7_source.zip**
- refreshed root **WallSprint.script** / **BUILD_MANIFEST.json**
- **analysis/V01C7_RELEASE.json** records implementation SHA, exact ZIP sizes/SHA256, validation counts and synthetic summary.

Follow GitHub `README_TEST.md`: same large A wall, one continuous left-to-right~4s WallRun. Do not induce a hitch. Report full visible surface-relative hold, brief upright flash/snap/restarted rotation after any natural hitch, crash/Position corruption. Return the complete PRIVATE log including startup admission, SUSPEND/RESUME if present, A/B counts, retention, read-only adhesion summary, native Deinit lifecycle summary and watch cleanup_pending0.

Require retention1, no permanent fresh_update_age exit in a continuing matching session, clean verified Deinit until1, Position writes0. No-gap human run can pass with stale counts0; the native test independently covers the gap. Position whole-path/W/exclusive-effects proof and actual adhesion remain blocked until real c7 hold passes and a later task authorizes them. No movement/animation/camera/gravity/jump/corner/parkour/swing/combat added. No raw user logs/video/full native functions are committed or packaged.

## Persistence

Implementation is committed at the exact SHA above. Result/artifacts/release hashes are committed afterward on this same branch and pushed, then authenticated Git retrieval verifies delivery commit and ZIP bytes. Final download uses GitHub raw ZIP; test instructions use GitHub blob README_TEST (not workspace preview). The private GitHub repository may require the user's signed-in account; anonymous HTTP status is not treated as authenticated download evidence.
