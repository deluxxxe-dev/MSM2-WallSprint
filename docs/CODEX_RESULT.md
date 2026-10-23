# Codex result — WallSprint v0.1c11

Branch: **codex/v01c11-movement-path-ownership**.
Implementation commit: **2129e69fe003e71bf705fb67f723b447cf3e3764**.
Exact c10 delivery base: **6246fd6d2e52cf81e18e6ff8ffd4d8ee4d863aa1**. Used the pre-created branch at **4d78695d4a426158b46fb6ec3faf04ccbd8728db**, containing05c12e4/e27ab26/4d78695 project/task/runtime handoff. PROJECT_STATE/CODEX_NEXT stay byte-identical to that handoff. No main rewrite/force push.

## Outcome

**v0.1c11 / package0.1.29 implements movement-path ownership diagnostics, with Position and movement read-only.** It preserves c10 orientation and adds bounded Hero Position writer attribution, SurfaceFrame delta/finite-difference velocity, conservative whole-function Position-copy inspection and explicit missing upstream-proof evidence. No input rewrite or movement/Position injection.

Historical c10 real cardinal/diagonal Surface Up/lifecycle is VERIFIED PASS. Its natural refresh branch did not fire; deterministic c10 race fixtures remain that branch's evidence. User body/root lean and apparent foot floating are deferred visual/contact work and are not changed here.

Cloud diagnostic implementation/tests: PASS. **Actual c11 game Position writer identities, Path A causal propagation and a safe writable upstream movement source remain PENDING the A/E runtime log.** Diagnostic PASS with an exact blocker is explicitly allowed by CODEX_NEXT; this result does not claim full game proof or authorize c12 writes.

## Implementation / files

- `source/src/msm2/orientation_trace.cpp` and header: separate Position mode in the existing hardware lease/VEH/Toolhelp engine. Same coordinator, no concurrent Up/Position leases. Full t1/retention and elapsed1.55–3.0 request, prior Up cleanup,600ms window, one snapshot<=128 threads/no rescan. Two8-byte slots cover Transform+[0x30,0x40), including the fourth component without naming its meaning. Eight Position windows are budgeted separately from original eight Up windows. Foreign occupied DR0/DR1/general-detect refused; DR2/DR3/unrelated state retained. The original Up range/timing/threshold/policy stays in its old branches.
- Position cap/timeout/cancel restore uses exact saved owned-slot state, including DR6; cleanup never suspends itself. Failed cleanup stays pending rather than claiming success. Immutable lease tombstones remain resident for delayed owned exceptions. Position VEH preserves LastError. No allocation/log/SDK call while a thread is suspended, no callback logger/new lock wait.
- Worker records: max256 raw events/window,16 PC+caller identities,4 printed events/identity, aggregate hit/dropped/overflow/coverage/cleanup summary. RIP is after the data-breakpoint instruction; before is previous serialized watch snapshot, after is post-instruction, not function return or atomic pre-store. Records include Position4/raw W bits, projected XYZ delta, caller/function/module identities and relative A/B commit timestamps. Complete bounded Capstone decoding/hash of a first writer snapshot<=1024 bytes gives derived store metadata only; unknown runtime bodies are **not authenticated originals**. Owned Path A hooked bodies are not decoded as originals. No raw function body/disassembly is logged by the Position diagnostic.
- `source/src/NativeHooks/immediate_reset_trace.cpp` / request header: existing helper serializes both watches and rechecks read-only current source permission/session/Hero/Transform, native owner/thread/generation/init/phase/accepted/collision, worker context, current source/code integrity and100ms freshness. Native Deinit/F8 cancel capture; stale/identity/capacity failure closes diagnostics without killing a valid source. No secondary native publication, Update, keepalive or new game ABI. Inactive/not-armed reasons remain visible.
- `native_source_override.cpp` / header: diagnostic-only identity reader samples its age clock after coherent source/native publications and checks final permission again. Worker issues one late watch request, samples at most40 finite-difference attempts/session at>=100ms/distinct updates, records normal/forward/right components, dt and speed estimate. Stale clears only the diagnostic delta baseline. W is a separate guarded read. Existing A/B hot policy, entryToken/context, c7 recovery/c8 cause/c9 secondary/c10 refresh remain intact.
- `reset_copy_source.cpp`: after an already successful B override, a target-matching diagnostic timestamp updates the Position window; no native ownership migration or extra source store. A source provenance reuses the existing raw writerEntry callback and its actual c10 admission flag/stack proof.
- `path_a_position_proof.cpp` / header: existing c6 complete candidate inspector is preserved. Added strict whole-function direct-row-only certificate (one MOVUPS16 load from RDX+0x30, same-XMM store to RCX+0x30, RET/NOP only); unsupported instructions/calls/branches/effects report an exact offset and deny the certificate. This is a bounded sufficient certificate for its supported copy shape, not a guessed general CFG proof. Runtime linked source/full-row post-instruction equality/W and genuinely observed downstream overwrite remain **admitted=0/write_permission=0**, pending actual caller/CFG/W/exclusive-effects proof.
- New `Math/movement_delta.hpp`, portable movement tests, real Position hardware fixture, actual production-c10-with-real-watch integration fixture, causal positive/negative tests; existing test recipes link the existing pinned proof/decoder where needed. No new vendor/dependency.
- New root/source `POSITION_WRITER_ATTRIBUTION.md`, `PATH_A_POSITION_CAUSALITY.md`, `MOVEMENT_SOURCE_OWNERSHIP.md`, `V01C11_GATE.md`, source c10 runtime mirror; README_TEST/root-source README/BUILD/WORKLOG/mod/version/package/release docs and outputs refreshed.

## Validation

Final full suite:

```text
Windows x64 warnings-as-errors build
Portable ASan/UBSan            13 PASS
DLL smoke                      4 PASS
Full hooks/native/offline     46 PASS / 4 historical private-input SKIP
Total                         63 PASS / 4 SKIP
```

Accepted original B writer444/caller128 full compact proof and authenticated installer/mutation replay ran privately without executing game code or distributing private bodies. Actual A584/caller168 game Position semantics are not claimed from hashes or owned fixtures.

Native Position fixture executes independent owned Vec4-copy and computed/overwriting Position writers; source/target full16 equality positive, later XYZ/W mismatch negative. It checks exact post-store decode/effective address, unique-writer aggregation/4-event cap, raw256 cap, foreign DR0 refusal and DR2 preservation, exact final debug state/LastError, deadline/cancel/F8/wrong frame/target/timing, unchanged orientation/page protection. Position cap teardown's saved-debug-state handling is explicitly exercised.

Strict complete-copy fixture passes its narrow certificate and rejects computed/partial/branch/multiple effects/wrong caller/stack; identity/W-mismatch/overwrite cannot grant motion. Cardinal/diagonal decomposition, world-translation invariance, time-scaling, invalid time/frame/nonfinite controls pass under sanitizers.

Production c10 source policy with real hardware Position watch active: **session1: 4115ms/A178/B178, held2134ms/132updates; session2: 4103ms/A179/B179, held2143ms/133updates**. Both normal verified Deinit, source_active_until_deinit1, retention1, last Up dot1, first cause none/hard0. Each watch observed12 writes from one **owned synthetic unknown writer**, dropped0, cleanup_restored1/cleanup_pending0, mod Position writes0. The fixtures do not impersonate a measured game writer. Native Init is backdated800ms for entry delay; separate four-reader publication stress spans>=4 actual seconds. Wrong diagnostic session/owner/Hero/Transform/native thread/generation/permission/stale/Deinit requests refuse; A/B still retain Surface Up.

Final read-only clock and derived-store telemetry refinements were followed by a fresh Windows build,4 DLL smoke PASS,1 hardware Position suite PASS and1 production A/E integration PASS. Original c7/c8/c9/c10 full regressions remain PASS. Fresh extracted standalone source **rebuilt the Windows DLL with private captures absent**. Final archive validation: **PASS** for CRC/current complete source/result/README/licenses/nested script/hash/manifest, preserved source policy/handoff and private full-body byte/hex exclusion; exact ZIP hashes/sizes are in `analysis/V01C11_RELEASE.json`.

## Safety / exact blocker

No direct Actor/Position store, Path A Position modification, active adhesion, input rewrite, movement/velocity/gravity system, PAGE_GUARD/protection mutation, camera/animation/traversal work, native keepalive or unknown ABI. A48/B36 byte boundaries, shared4096 B attempts, original native lifecycle publisher/observer/raw thunks/store helpers/math and original source authentication remain. Position proof/admission/corrections0. The only game-data payload writes remain c10's proven orientation source ranges.

Position watch coverage is a single bounded incomplete snapshot; new/unwatched threads, drops/caps and unknown function boundaries can leave missing writers. Previous/after Position snapshots are not atomic whole-engine physics. A first worker code hash/decode is identification, not original-body authentication or a write certificate. Finite differences do not identify native velocity or input intent. An already-admitted c10 orientation store may still overlap a later close; c10's one-refresh bound remains unchanged. Human c11 hitch/crash/foreign-DR cleanup and actual writer evidence still require the game run.

**No safe upstream movement vector is authenticated.** Exact missing evidence: actual c11 writer/caller producer chain/source lineage on A/E, complete Position caller construction/CFG/downstream effects, fourth-component meaning and exclusive intended translation. Existing owner+0x320/+0x338 standoff/reference points are not renamed velocity/displacement; native Entry/Update identity alone does not establish a movement vector. Source/Hero equality alone cannot upgrade admission. New bounded logs identify the exact slice for the next handoff.

## Outputs / human test

- **WallSprint_v0.1c11_movement_path_ownership.zip**
- **WallSprint_v0.1c11_source.zip**
- root **WallSprint.script** / **BUILD_MANIFEST.json**
- **analysis/V01C11_RELEASE.json**: implementation SHA, ZIP sizes/hashes, compact owned test evidence, real proof blockers.

Use GitHub README_TEST: one continuous~4s A/cardinal wall run, then one~4s E/diagonal run. No special input or forced hitch. Report c10 posture hold, crash/hitch/freeze and full PRIVATE startup-through-cleanup log. Require normal verified Deinit/until1/retention1/no hard kill, Position writes0 and restored Position watch; if arming fails include the exact skip/summary reason. Actual Position writer/caller and candidate records determine the next safe movement-ownership investigation. No movement/input rewrite or visual-contact adjustment follows automatically.

## Persistence

Implementation is committed at the SHA above. Result/ZIPs/release hashes are a subsequent delivery commit on the same dedicated branch, pushed and verified by authenticated Git retrieval. Direct download uses GitHub raw ZIP; tests/results open as GitHub blob pages. Anonymous private-repository browser access is not claimed. No raw user log/video/screenshots/private full game function body is committed or packaged.
