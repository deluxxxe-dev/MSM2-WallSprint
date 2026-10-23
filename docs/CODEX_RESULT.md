# Codex result — WallSprint v0.1c8

Branch: **codex/v01c8-kill-cause-attribution**.
Implementation commit: **98c1d45108c89262750616c7258414cf762cac55**.
Main attribution/guard change:6e2b057; final hard-stop-before-record ordering and regression verification:98c1d45.
Base: published c7 **466f0ab237faccd8c769d17fedd66a42138a7829**. Main c7-runtime/c8-state/task commits cab6f6d/0365b42/03a4c64 were imported as814ef44/1262fa6/92160e4 on the pre-created branch. `docs/PROJECT_STATE.md` / `docs/CODEX_NEXT.md` retain the main handoff exactly. No main rewrite or force push.

## Outcome

**v0.1c8 / package0.1.26 implements exact first hard-kill attribution and a proven diagnostic-only guard separation.** The first stop/fault caller is latched once per process, with reason/origin/thread/context/native identity and detailed source/observation authentication. The four WallRun hooks report separate enabled/read/match/difference status. Runtime field-plan-only loss stops diagnostics while independently authenticated A/B source ownership continues. Real code/patch/build/identity/thread/stack/epoch/rollback failures remain fail-closed.

**The exact first real c7 kill caller is still UNKNOWN; c8 real-game rotation/hold is PENDING.** c7 really failed after only two A commits at.703/.719s, t0/.064, then disabled_kill_or_patch_gate while verified WallRun continued. Nearby observation_guards_lost is temporal evidence, not proof of initiating cause. Static review and owned production-policy tests demonstrate the limited field-plan coupling; they do not prove this was the real run's failing condition. Historical c5 remains the last real full-hold PASS. No Position/adhesion advancement.

## Implementation and files

- `source/include/NativeHooks/ownership_guards.hpp`: side-effect-free detailed patch and observation status. Source integrity requires verified availability and all four enabled/readable/unchanged patches. Diagnostic viability additionally requires fieldPlanEnabled.
- `source/include/msm2/wallrun_hooks.hpp` / `source/src/msm2/wallrun_hooks.cpp`: public internal detailed status/read-only API, existing diagnostic boolean retained, bounded change-only OBSERVATION_PLAN_AUTH_STATUS/FAILURE records (16/process). All14 required gates plus each hook's first differing byte offset and failure mask are available. The periodic ownership checker reports before resetting its observer. Offset32 is a sentinel interpreted with read/match flags, not an equality assertion after failed read.
- `source/include/NativeHooks/native_source_override.hpp` / implementation: kill requires explicit reason/origin; nonwaiting CAS claims one immutable first-cause record. Hard kill closes write permission before diagnostic reads; a racing worker cannot republish permission or finalize an incomplete cause. Only its winner writes, release/acquire publishes it and worker drain logs exactly once outside the lossy event ring. Configure housekeeping does not consume the latch, and production exposes no reset. Later requests count without overwriting. Source entry/writer/caller/compact/lifecycle patch sub-gates and first differences are sampled with source and worker context, actual native token, expected-vs-current identity/thread/generation/epochs/init and saved raw entry registers. A separate diagnostic identity publication retains expected session/Hero/Transform during soft suspension; it never grants stores and fallback is labeled.
- Explicit A/B wrong-thread/caller/stack/layout/Hero/guard/rollback reasons, entry-token incoherence versus identity/epoch, worker observation-plan versus input-auth loss, F8 versus script stop. Hard session ends also latch their specific gate. Normal verified Deinit emits NATIVE_SOURCE_CLOSE and lifecycle summary without occupying the abnormal cause slot. Source exits use the actual latched reason instead of disabled_kill_or_patch_gate. Summary adds first_hard_kill/close-request count. A worker does not finalize an incomplete racing cause record or block a callback waiting for logging.
- `source/src/msm2/telemetry.cpp`: reads/reports detailed observation status and uses the production source worker policy. Field-plan-only failure stops reset/trace without killing input/native source. Actual source integrity still kills source before input cleanup; independent input authentication is checked again after input tick. Input tick uses source integrity instead of the diagnostic field-plan boolean. Startup full-plan and original exact-code admission are unchanged.
- Native-source fixture and actual wallrun-hooks runtime fixture extend tests for activation~.7s, four independent real owned-patch mutations, exact first cause/offset, hot/worker/input/thread/caller/stack/token paths, racing requests, retained identity and no hard-close revival. Fixture resets model separate processes; no production latch reset exists.
- `analysis/V01C7_RESULT.md`, `HARD_KILL_ATTRIBUTION.md`, `V01C8_GATE.md`, source mirrors and authoritative c7 runtime mirror; root/source README_TEST, source README/BUILD/WORKLOG, version/mod metadata, package/release recipes and manifest updated.

## Proof and preserved safety

Dependency review: surface_source.hpp uses fieldPlanEnabled for diagnostic root/child observations. It does not supply the A/B basis or authorize source stores. Startup still requires full-plan/original exact authentication. In a production-worker/owned-MinHook fixture, the old diagnosticViable predicate becomes false only because fieldPlanEnabled changes just after the first two A/B commits. The diagnostic stop interfaces run, independently verified input/source stay alive, real A/B calls reach t1 and retention. This is the narrowly proven false coupling.

Conversely, every actual WallRun patch mutation makes sourceIntegrity false, logs its exact hook/offset, immediately blocks both A/B, and never revives after restoration/later Update. Actual caller/writer/compact proof, source current guards, stack/thread/Hero/Transform/session/owner/generation/accepted/collision epochs, executable/hash,100ms native/worker freshness, F8 and rollback failure remain required. Guard snapshots are separate reads after the first atomic claim, not an atomic all-engine snapshot; concurrent cleanup can affect sampled status, while the winner's original reason remains immutable.

Native publisher/raw source and hook thunks, stack-store helpers, B installer/compact proof, reset/hardware watch and Position inspector/quaternion math are byte-identical to c7. Position proof remains0; A48/B36 orientation only, A Position16 and B scale/metadata unchanged; read-only plane samples/summaries with corrections/source Position writes0. No native keepalive call, direct Actor/Position store, added hook/ABI/vendor copy, PAGE_GUARD/protection change, movement/animation/camera/gravity or later milestone.

c7 stale handling remains: time-only native age>100ms blocks A/B and suspends; a strictly later real same-identity accepted Update resumes the retained full target or frozen partial progress. Original source-entry real5s and4096/path limits, episode16-pair cap and retention maxima remain. As before, an already-admitted short guarded store can overlap close/kill. Human visual behavior is not established by fixtures.

## Validation

Completed exit0:

- Windows x64 build with WallSprint warnings-as-errors.
- Portable ASan/UBSan **12 PASS**, DLL smoke **4 PASS**, full hook/offline/native suite **41 PASS**: **57 total**, **4 explicit historical private-input SKIPs**.
- Exact accepted-build B writer444/caller128 proof and authenticated production installer regression ran privately, including nine-pair map and mutation/caller/stack/decode negatives. No captured game code executes or ships. Actual A Position whole-path/W/exclusive-effects proof is not claimed.
- Actual detailed wallrun_hooks API tests separately identify Init/Entry/Update/Deinit mutations at offsets9/10/11/12; disabled/read-failed/status/field-only controls and bounded transition logging pass.
- Production source worker tests at~.7s after two A/B commits (0<t<1): field-only loss shuts diagnostic interfaces down, leaves input/source admitted and reaches t1/retention. Actual four patch mutations at offsets7/8/9/10 identify the exact first worker reason/hook/offset, stop both paths immediately and never resume. Hot source integrity, input loss, B wrong thread, wrong owned callers/stacks, real native collision/incoherent token and native thread identity changes have explicit cause checks. Racing explicit kills retain one coherent record; subsequent shutdown does not overwrite it.
- Final expected-identity/token diagnostics and immediate pre-record hard-stop ordering were followed by another Windows build, **4 DLL smoke** and **all6 affected native-source/publication PASS**. A test assertion explicitly treats verified Deinit as a normal session close with an empty abnormal-cause slot; other stale-hard controls retain expected Hero/Transform while write permission is empty.
- Final no-gap owned session4116ms native elapsed, A189/B189, held3041ms/189 updates, clean verified Deinit until1 and first_hard_kill none. Recovered owned session4107ms, A174/B174, one suspend/resume/max expired lease176ms, retention1, clean until1/first_hard_kill none. Existing real>=270ms gap, missed-worker signal, partial quaternion continuity,18 episodes/16 logged pairs and11 stale-hard controls pass. Init is backdated800ms for entry delay; separate4-reader native publication stress spans>=4 actual seconds without reader-induced collision.
- Existing ABI/AVX/FPU/stack/metadata/Position/compact/watch cleanup tests remain PASS. Source and trace-status fixtures own all functions/buffers. Diagnostic stop interfaces are stubbed only in the source policy fixture; actual status and unchanged diagnostic engine are separately covered by native suites.

A fresh final standalone source ZIP extraction rebuilt the Windows DLL successfully, with private captures absent from the extracted source tree. Final archive checks cover CRC, exact current complete source/docs/README/licenses, nested three-file script and hashes, Position-disabled/strict-integrity manifest and private full-function byte/hex exclusion. Only selected independent cloud validation logs are packaged, including the final affected native-source run; no raw user or synthetic full-function trace ships.

## Outputs and human test

- **WallSprint_v0.1c8_kill_cause_attribution.zip**
- **WallSprint_v0.1c8_source.zip**
- refreshed root **WallSprint.script** / **BUILD_MANIFEST.json**
- **analysis/V01C8_RELEASE.json**: implementation SHA, exact ZIP sizes/SHA256, validation counts, independent patch results and compact synthetic recovery summary.

Use GitHub README_TEST: same large A wall, one continuous~4s left-to-right WallRun; no forced hitch, Crawl/E not required. Report whether rotation starts after~.7s, reaches Surface Up, remains until verified Deinit, any upright flash/snap/crash/Position corruption. Return the complete PRIVATE log including startup all-good status, first-cause/failure records if any, A/B writes, retention/lifecycle/read-only adhesion summary and cleanup_pending0.

Primary PASS: rotation beyond the first commits, retention1, no anonymous kill reason, verified Deinit final close/until1, Position0. If a hard kill remains, claim only diagnostic success with exact first cause and actual failed observation sub-gate/patch (or an all-good observation status for an independent hot fault). Shutdown/F8 are explicit stop requests and do not overwrite earlier runtime causes. No-gap run may have stale counts0; independent native tests cover recovery. Real c7 first cause and c8 game hold remain unverified. Position whole-path/W/exclusive-effects proof/adhesion and all later roadmap work stay blocked until later authorization after real hold.

## Persistence

Implementation is at the exact SHA above. This result, refreshed artifacts and release hashes are committed afterward on the same branch, pushed and verified by authenticated Git retrieval. Final download uses GitHub raw ZIP, instructions use GitHub blob README_TEST; anonymous private-repository HTTP status is not authenticated download evidence. No raw user log/video/full native function is committed or packaged.
