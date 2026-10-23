# Codex result — WallSprint v0.1c9

Branch: **codex/v01c9-secondary-path-b-thread-admission**.
Implementation commit: **0c276126ccac8e3bd2a6dd1d850da63e80bc35ce**.
Base: published c8 **75af734d4d32d5e3a77087cbee6b2247b9617ced**. The pre-created branch was used; main c8-result/c9-state/task a8acdeb/629d78b/96fa426 were imported as23e3570/000713e/575a3db. `docs/PROJECT_STATE.md` / `docs/CODEX_NEXT.md` remain exactly the authoritative main handoff. No main rewrite or force push.

## Outcome

**v0.1c9 / package0.1.27 implements narrowly verified secondary-thread Path B source admission.** A different B calling thread starts per-call validation instead of a fatal path_b_wrong_thread close. Exact caller, current thread's own stack, compact proof/layout, active source/worker/native identity/freshness and current code integrity still must pass. Native owner thread/token and Path A policy never migrate. Foreign calls receive no injected payload; genuine source corruption/identity/collision/rollback/F8 conditions remain hard-close.

The real c8 failure is authoritative: Surface Up/retention passed, then thread35948 made a same-Hero/exact0x2CFC84B/RDX-entryRSP0x28 call at1.750s while expected/native10680 and all other guards were healthy. C8 killed source solely on thread inequality, but verified WallRun continued4.405647s to Deinit. This proves the B invariant was false; it does not relax native lifecycle thread identity or identify c7's earlier anonymous caller retroactively. **Actual c9 visible full hold/flash/crash/Position safety remain PENDING.**

## Changes and files

- `source/src/NativeHooks/native_source_override.cpp`: primary/secondary/foreign classification inside the existing B hook. Both B classes share production entryToken/currentGuards, fresh SDK sample, stackCompact/compactReplacement/writeStackCompact. Secondary calls additionally require a coherent current verified worker context for the same active session/owner. Native token owner/generation/init/native thread/accepted and collision epochs stay pinned; freshness100ms remains checked at entry and pre-store. No secondary lifecycle publication or Update.
- Secondary caller must be exact0x2CFC84B, RCX current active Transform, source=raw entryRSP+0x28,16-aligned/readable/writable/full64 span inside that calling thread's TEB stack. Only36 orientation bytes change; scale/later28 metadata remain. Shared published full or partial axes are used directly; no delay/entry/target/progress reset. The primary hardware-watch commit request remains primary-only so the new call cannot adopt its owner; bounded layout entries use the actual calling thread.
- Secondary wrong target/caller/relation/span/layout/context-availability rejects normally pass through without killing active ownership. Actual proof/writer/caller/patch/SDK Hero/Transform/verified session/owner/generation/epoch/native-token-thread/sequence/collision loss stays fatal. Stale time-only token rejects signal the same c7 suspension/recovery path. Retained identity is attribution only, never permission from an empty/inactive publication.
- B's4096 store-attempt budget is reserved atomically across primary/secondary calls before the guarded store, preventing concurrent overrun. Failed guarded attempts consume a slot; foreign/stale/provenance rejections do not. Committed writes remain separately counted. B's per-call override diagnostic flag resets at entry, so rejection cannot inherit a previous call's flag.
- B-only event/summary telemetry: process-wide atomic seen/admitted-to-attempt/overridden/rejected counters, first/last secondary thread IDs,16-slot unique-ID table with explicit saturation/overflow/inflight. First override and reject records are worker-drained independently of the lossy shared ring; later events cap16/action. Logs include exact call/stack/token/identity/basis/action/reason. No callback logger/worker lock, allocation, process thread scan/suspension or broad hardware-watch extension. This hook evidence continues beyond the old1.50s watch while source is active, including the real1.75s event window.
- `source/src/msm2/native_source_override_smoke.cpp`: real secondary-thread MinHook caller/stack after retention through4.4s native Deinit; partial shared basis, foreign and hard negatives, secondary stale recovery, scale/metadata, telemetry cap and8-thread shared-budget race. Thread-ready/GO handshakes let normal main-thread Update/publication precede intentional faults; the secondary never synthesizes Update. Owned target is captured before SDK identity mutations to test the actual identity guard, rather than accidentally turning that case into unrelated target traffic.
- `analysis/V01C8_RESULT.md`, `SECONDARY_PATH_B_THREAD.md`, `V01C9_GATE.md`, source mirrors/authoritative c8 runtime mirror; root/source README_TEST, source README/BUILD/WORKLOG/version/mod metadata and package/release manifests updated.

## Preserved safety and limits

Path A's expected-thread/caller/stack/identity function body is text-identical to c8. Native lifecycle publisher/raw hook/source thunks, stack-store helpers, B installer/compact proof, reset/watch coordinator, detailed wallrun observation/worker framework, Position inspector and quaternion transition math are byte-identical to c8. Startup exact executable2.810.0.0/SHA/original function/caller/build/full-plan admission and all runtime code identities remain. No new hook/native ABI/vendor copy, PAGE_GUARD/protection change, native keepalive call, direct Actor/Position store or later movement/animation/camera/gravity/adhesion work.

C8 immutable first cause/detailed observation and source guards remain. Successful secondary calls do not consume the latch. Native Update and worker publication freshness stay100ms; c7 later-real-Update recovery/frozen partial transition, original real5s-from-source-entry/4096-path caps remain. Position proof0, read-only plane samples/summary, corrections/source Position writes0, A Position16 and B metadata preserved.

As before, a short already-admitted guarded store can overlap close/kill. Counts are separately sampled atomics; inflight marks incomplete snapshots. Unique IDs beyond16 are explicitly a lower bound; event caps/drops never grant or suppress source writes. Foreign B traffic remains vanilla and can run its original writer; c9 does not inject unverified source payload to hide its effects. Human visual behavior still requires the run.

## Validation

Completed exit0:

- Windows x64 warnings-as-errors build.
- Portable ASan/UBSan **12 PASS**, DLL smoke **4 PASS**, full hooks/offline/native **42 PASS**: **58 total**, **4 historical private-input SKIPs**.
- Exact accepted B writer444/caller128 complete nine-pair proof and authenticated production installer/mutation/caller/stack/decode regression ran privately without executing captured game code. Actual A Position whole-path/W/exclusive-effects proof is not claimed or admitted.
- C8-equivalent owned native sequence: same session reaches full pose/retention, actual other-thread B caller at**1.752s**, exact caller/RDX-RSP40/current stack/compact/token/identity pass, native owner/Update counter/session/entry/target unchanged, full Surface Up returned, first cause empty. Primary A/B continue afterward to**4409ms** verified Deinit: **A207/B208**, retention1, held**3335ms/207 updates**, **source_active_until_deinit=1**, first_hard_kill none. Secondary summary seen1/admitted1/overridden1/rejected0/unique1/inflight0. Native Init is backdated800ms for the entry-delay fixture; separate publication stress spans>=4 actual seconds with4 readers.
- Partial interpolation first secondary output matches published progress with no restart. Wrong target/caller/relation/other-stack span/invalid rows get no payload and primary continues. Bad compact proof and four actual lifecycle patch mutations hard-kill with c8 precise first cause; SDK Hero/Transform/current worker session/owner/native generation/accepted epoch/genuine collision negatives cannot revive on a later primary call. Original Path A wrong-thread remains fatal.
- A real270ms stale secondary call cannot store, c7 worker suspends, still rejects while inactive, and a later main-thread accepted Update resumes existing ownership and permits another valid secondary call. Existing missed-worker signal/partial quaternion/18 episodes/16-pair/11 stale-hard controls remain PASS.
- Secondary row scales.75/1.25/1.5 and all metadata preserved.20 override+20 reject calls produce32 bounded records/full counts. Eight simultaneous own-stack B candidates compete for one remaining shared attempt: exactly1 override/7 rejections/8 unique IDs/no hard cause/no budget overrun. LastError, raw thunk ABI/AVX/FPU/stack/return preservation, ordinary metadata/Position/guard/trace cleanup suites remain PASS.
- Tests own all executed functions/buffers; no raw private user log/video/full game body is used as synthetic game PASS or packaged. Fixture resets explicitly model separate process counters/latches; no production reset API is added.

Standalone source ZIP rebuild: **PASS**, using a fresh extracted source tree with private captures absent. Final archive validation: **PASS** — CRC, complete current sources, mirrored result/README, nested script hashes, licenses, safe manifest, unchanged ABI/store/proof/watch/math and private full-body byte exclusion. Exact output hashes/sizes are recorded in `analysis/V01C9_RELEASE.json`.

## Outputs and human test

- **WallSprint_v0.1c9_secondary_path_b_thread.zip**
- **WallSprint_v0.1c9_source.zip**
- refreshed root **WallSprint.script** / **BUILD_MANIFEST.json**
- **analysis/V01C9_RELEASE.json** records implementation SHA, exact ZIP hashes/sizes, validation counts and compact synthetic secondary/recovery evidence.

Use GitHub README_TEST: same large A wall, continuous left-to-right~4s WallRun, no forced hitch/Crawl/E. Report rotation after~.7s, full Surface Up/hold to native Deinit, flash/snap especially1.5–2.0s, crash/Position corruption. Return the complete PRIVATE log including startup admission/status, secondary event/summary, primary continuation, retention/lifecycle/read-only adhesion summary and cleanup_pending0.

Primary PASS requires retention1, no path_b_wrong_thread close, secondary override if an eligible event occurs with native owner unchanged, continued primary A/B, clean verified Deinit/until1 and Position0. A no-secondary human run must still pass normal full hold; the native test independently covers the exact c8 sequence. Natural stale gaps use c7 SUSPEND/RESUME; no-gap counts0 are acceptable. Do not advance Position/adhesion/later roadmap until real full hold and later authorization.

## Persistence

Implementation is at the exact SHA above. Result/artifacts/release hashes are committed afterward on the same branch, pushed and verified by authenticated Git retrieval. Final download uses GitHub raw ZIP and instructions GitHub blob README_TEST. Anonymous private-repository HTTP status is not authenticated download evidence. No raw user logs/video/full captured game functions are committed or packaged.
