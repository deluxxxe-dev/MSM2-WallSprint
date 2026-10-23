# Codex result — WallSprint v0.1c5

Branch: **codex/v01c5-sustained-lifecycle**.
Implementation commit: **1cfe3c76a5e09183757f891c5025def860ef4a88**.
Base: published c4 **6bf7646d2aebdfda9c285b9a3ca4108760d6baa1**. The pre-created branch was used; authoritative runtime/state/task commits4b953b8/3cc38de/ef55948 were imported. PROJECT_STATE/CODEX_NEXT retain exact main handoff content. No force push or main rewrite.

## Outcome and evidence boundary

v0.1c5 / package **0.1.23** repairs two demonstrated continuity defects and adds exact exit and verified-Deinit evidence. c4 real enumeration and threshold retention are PASS; c5 real full-WallRun hold remains PENDING. The precise sub-gate behind the user's c4 collapsed exit remains UNKNOWN; no unsupported claim of private-run causality is made.

Exact published c4 source reproduces a reader holding the lifecycle gate, dropping a native Update and permanently invalidating its collision epoch despite100 subsequent fresh same-owner Updates. c5 readers use bounded atomic publication and cannot prevent native event updates. Genuine event/event collision, invalid sequence and identity failures still fail closed until a clean session. Another demonstrated race sampled now before token acquisition, causing a newly published timestamp to appear in the future; now is sampled after acquisition. Neither100ms freshness limit is removed or widened.

## Changes and safety

- `source/src/NativeHooks/source_lifecycle.cpp`, `source/include/NativeHooks/source_lifecycle.hpp`: actual native writers retain a dedicated nonblocking gate; coherent readers use the existing AtomicPublication helper with at most8 attempts. Tokens retain native timestamps, explicit read status and accepted/collision epochs. No worker timestamp substitutes for native Update freshness.
- `source/src/NativeHooks/native_source_override.cpp`: once-per-exit NATIVE_LIFECYCLE_EXIT_DIAG names the exact failing sub-gate and records token/context/phase/owner/thread/generation/updates/time/age, individual freshness tests, Hero/Transform match and transition state. It is emitted on the worker outside the lossy per-call ring. Real failed sessions cannot re-enter on a later Update.
- A bounded native close notice and worker ORIENTATION_LIFECYCLE_SUMMARY retain the first exit reason, actual native WallRun duration, source-entry elapsed, A/B counts, threshold PASS/max held time/updates and last active-source dot/time/scope. until_deinit=1 requires matching fresh coherent ownership at native Deinit. An earlier exit remains0. A stop/new-session fallback has verified_deinit=0. Native close epoch wins over a worker publication racing Deinit; no stale pre-close token can adopt the new epoch.
- `source/src/NativeHooks/source_lifecycle_smoke.cpp`, extended `source/src/msm2/native_source_override_smoke.cpp`, `tools/test_source_lifecycle.sh` / `test_native_source.sh`: real multi-second publication stress, MinHook A/B hold and negative/summary/close-race tests. Version, README_TEST (root/source), README/BUILD/WORKLOG, analysis and release recipes updated.
- Required V01C4_RESULT, SOURCE_LIFECYCLE_CONTINUITY, V01C5_GATE and authoritative V01C4_RUNTIME_RESULT mirror are included in source and root analysis.

Exact executable2.810.0.0/SHA, caller/writer identity, current owned patches, compact proof, Path A Position preservation, Path B first0x24-only/row-scale/metadata boundaries, raw Win64 ABI and lock-free coherent A/B snapshot remain. The c4 watcher, immediate reset coordinator, compact proof, reset-copy installer, stack-source helper and hook thunks are unchanged. Native-thread/stack/owner/session/generation/Hero/Transform/freshness/kill gates remain. No third-writer patch, direct Actor/Position write, page protection/PAGE_GUARD, extra game call/hook, adhesion, movement, animation, camera or later milestone is added. Existing4096 writes/path and5-second-from-source-entry limits remain; the3–4-second human test fits within them.

## Validation

Completed exit0:

- Windows x64 build, WallSprint warnings as errors.
- Portable ASan/UBSan: **11 PASS**.
- DLL smoke: **4 PASS**.
- Full hook/offline/actual-byte/native-thread suite: **36 PASS**, **4 explicit historical private-fixture SKIPs**. Final lifecycle/clock/summary changes were followed by a fresh Windows build, DLL smoke and the complete affected native-source/publication suite, all PASS.
- Actual accepted-build writer444/caller128 proof/production installer replay ran and passed all9 pairs, authenticated installation/owned restoration and mutation negatives. Original game code is not executed.
- Exact c4 reader collision mechanism reproduced against its published source/header using owned tokens.
- Production publication sustained >=4 real seconds with4 concurrent OS readers: coherent monotonically advancing native updates, zero reader-induced event collisions, LastError preserved. Genuine native collision, owner mismatch, Deinit and disable fail closed.
- Real MinHook A/B fixture reached4.114s native elapsed with186 writes per path; held Surface Up for3006ms/186 updates, then clean Deinit summary until_deinit=1. Its existing setup backdates Init by800ms; the sustained commit loop itself spans roughly3 real seconds. A separate publication fixture runs>=4 actual seconds. Diagnostic committed status becomes false after1.50 while orientation continues.
- Real>100ms stale publication returns exact fresh_update_age; another Update cannot revive the failed session, and eventual native Deinit summary remains until_deinit=0. Native collision and Hero mismatch produce exact once-per-exit diagnostics. Future timestamps remain refused. Deinit injected during the worker Hero read cannot reopen A/B publication. Existing wrong-thread/caller/stack/auth/metadata/Position/A-E retention gates remain PASS.
- An initial time-order test assumed Sleep2 crossed GetTickCount64 granularity; it now awaits an actual bounded tick advance. Its time-order assertions were retained and pass.

A fresh standalone source ZIP extraction rebuilt the Windows DLL successfully with private captures absent. Final archive/privacy validation passes ZIP CRC, complete current source/docs/licenses and source-byte/proof/watch preservation, nested script contents/hash and private full-function byte exclusion. Only selected independent validation summaries are distributed; no raw game log/video or synthetic full-function trace is packaged.

## Outputs and next human test

- **WallSprint_v0.1c5_sustained_lifecycle.zip**
- **WallSprint_v0.1c5_source.zip**
- refreshed root **WallSprint.script** / **BUILD_MANIFEST.json**

Confirm exact startup proof/admission, then use the same large unobstructed A-direction wall for one continuous left-to-right WallRun around3–4 seconds. Require A and B commits after elapsed2.0, retained-threshold PASS, no early inactive/stale exit, and one verified native Deinit summary with source_active_until_deinit=1. Check last active observations remain Surface Up and its timestamp reaches the run's end; summary counts alone do not prove every unobserved frame. Report rotation, whole-run hold, crash/Position corruption and return the complete private log through normal ChatGPT handoff. E is unnecessary unless A passes and a specific angle regression needs checking.

If the issue remains, the diagnostic names the actual failed sub-gate; no raw-log prerequisite blocked this implementation. c4 capped thread-watch diagnostics remain available, with cleanup_pending=0 required; no captured third writer is not proof of absence. Raw game logs/video/full function capture must remain out of Git/ZIP.

Only a subsequent handoff after real lifecycle-wide hold may authorize adhesion or later milestones.

## Persistence

Implementation is in the exact SHA above. This result, release artifacts and machine-readable hash record are committed afterward on the same branch, then pushed and checked by authenticated Git retrieval. User links point at that delivery commit: GitHub raw ZIP for direct download, GitHub blob README_TEST for the test page.
