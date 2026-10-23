# Codex result — WallSprint v0.1c10

Branch: **codex/v01c10-secondary-publication-race**.
Implementation commit: **609fdf935e40947d7aad29fea4bed5f2cf3d0e2e**.
Base: published c9 **b865dabfa81626248ea8a8ae56a1dd1c992d7301**. Imported authoritative main A/E runtime/state/task34efb64/030adcd/dcab277/fc0c322/4aa6379/ca684f1 as1a6d57c/bb29b56/8a09f13/ffc9803/9cc7b0a/bf52cd7. PROJECT_STATE/CODEX_NEXT match ca684f1 exactly; no main rewrite/force push.

## Outcome

**v0.1c10 / package0.1.28 repairs the demonstrated secondary Path B publication race with one bounded refresh/revalidation.** Normal time/update/interpolation changes within the same active ownership no longer immediately leak the vanilla WorldUp payload. The compact replacement is rebuilt from the latest admitted basis and the callback's current stack block. Real identity/lifecycle/integrity/stale/stop changes never gain permission; expected native thread and Path A policy remain fixed.

Historical real c9 A full hold is VERIFIED PASS. Diagonal E Surface Up/lifecycle also passed, but112 secondary calls included2 publication-change rejects, each followed by a short WorldUp observation (~2.34/~3.53s). The human did not notice them visually. C10 synthetic tests pass; **actual c10 E zero-flash continuity/A regression remain PENDING**. No movement or active adhesion is authorized.

## Changes / files

- `source/src/NativeHooks/native_source_override.cpp`: explicit semantic identity vs refreshable snapshot comparison. Pin active/session/owner/Hero/Transform/generation/permission epoch/accepted epoch/init origin/native thread/fixed surface. publishedMs/nativeUpdate/elapsed/t/basis may advance without regression. Equal-ms basis/update changes still count.
- One shared validatePayload routine assembles both initial and refreshed B payload through existing entryToken, secondaryContext, currentGuards, fresh SDK identity, current-thread TEB stackCompact, compactReplacement. Native token phase3/owner/generation/init/thread/accepted/collision and100ms native/worker age remain mandatory. Ret/RCX/RDX-entryRSP0x28/current own stack/proof rechecked. Only36 orientation bytes; original scale and28 metadata preserved.
- Final secondary change may refresh once only after semantic identity matches. Full live revalidation and replacement recomputation, then a settled publication equality read. Another advance rejects `secondary_publication_refresh_retry_exhausted`, with no store/budget reservation. Common final guards/token/worker context remain before existing atomic4096 B slot and guarded store. No secondary publication/Update, owner migration, delay/target/transition reset or extra game call.
- Genuine still-open identity changes hard-close with c8 immutable attribution. Already-revoked Deinit/stale/kill permission stays revoked; normal Deinit does not consume abnormal first cause. Foreign target/caller/stack/layout still gets no payload. Time-only stale retains c7 suspension/later-real-primary-Update recovery. Primary B still validates its final native token; Path A body unchanged.
- Bounded `PATH_B_SECONDARY_PUBLICATION_REFRESH_EVENT` includes old/new ms/update, semantic Published identity and live admission outcome. Independent first record outside shared ring;16 records/process, full atomic seen/admitted/rejected_identity_change/stale/integrity/other counters in existing secondary summary. Actual token/SDK/worker failures can reject despite semantic Published match. No callback logger/allocation/lock/wait/thread scan/watch extension.
- `source/src/msm2/native_source_override_smoke.cpp`: test-only guarded-reader interposition pauses the owned MinHook callback after old replacement computation; main performs real Update/worker publication. Diagonal full/partial/current-stack/scales/metadata/same-ms/second-refresh,26 negative races, first-record pressure/cap. Failure diagnostic added for the old real-time recovery assertion. No production test hook or reset API.
- New `analysis/V01C9_E_RESULT.md`, `SECONDARY_PUBLICATION_RACE.md`, `V01C10_GATE.md`, source mirrors and c9 A/E runtime mirrors. Root/source README_TEST, source README/BUILD/WORKLOG/version/mod metadata, package/release recipes and generated script/manifest updated.

## Validation

All final commands exit0:

```text
bash tools/build_windows.sh
bash tools/test.sh                       12 PASS (ASan/UBSan)
bash tools/smoke_windows.sh               4 PASS
WALLSPRINT_PATH_B_CAPTURE=<private> bash tools/test_hooks_windows.sh
                                        43 PASS / 4 historical private SKIP
```

**59 total PASS /4 SKIP**. Accepted original B writer444/caller128 complete proof and authenticated production installer/mutation replay ran privately without executing game code or redistributing full captures. Actual A Position whole-path/W/exclusive-effects proof remains unproven/denied.

Canonical diagonal production-policy fixture: real secondary callback refresh old Update62→63 / published ms55209770→55209786, semantic1/admitted, exact B caller/current own stack, fixed expected native thread, no hard cause. Primary A/B continue through **4201ms / A194/B195 / held3127ms/196updates**, retention1/source_active_until_deinit1/verified Deinit1/first_hard_kill none/hard_kill_calls0. Secondary seen1/admitted1/overridden1/rejected0/unique1/inflight0; refresh seen1/admitted1/all rejection classes0. No post-race WorldUp observation in the owned sequence. Native Init is backdated800ms in the entry-delay fixture; separate4-reader publication stress spans>=4 actual seconds.

Partial transition uses a numerically newer basis and preserves.75/1.25/1.5 scales/metadata; another test changes the actual current incoming stack block during the pause and requires its latest scale/metadata. Same-ms newer update detected. Second advancement rejects at the one-refresh bound and a subsequent legitimate primary call still works.

26 intervening negative races cover published session/owner/Hero/Transform/generation/permission/accepted epoch/thread/init/active; real Deinit; native token thread/owner/generation/accepted/phase/collision; actual lifecycle patch and compact proof loss during revalidation; expired native/worker publication; F8; fresh SDK Hero/Transform; live worker session/owner. All block secondary stores and consume no B attempt. Genuine hard cases cannot revive, verified Deinit stays normal, stale-only recovers under original c7 rules. First refresh survives a full shared ring;20 refreshed successes produce16 records/full counts. All c7/c8/c9 cases,8-thread shared budget, LastError/raw ABI/AVX/FPU/Position/metadata/watch cleanup still pass.

One initial full-suite run hit the existing c7 exact-one-suspend/resume assertion while the fresh source build ran concurrently. The final complete suite after compilation passed without changing the100ms freshness policy or weakening assertions. The first run did not record enough state to prove its scheduling cause; fixture failure diagnostics now expose that state. The separate native suite had also passed before that concurrent run.

Fresh standalone source ZIP extraction **rebuilt the Windows DLL successfully with private captures absent**. Final archive validation: **PASS** for CRC, current complete sources/result/README/licenses/nested script hashes, safe manifest and private full-body byte/hex exclusion; exact ZIP sizes/hashes are recorded in `analysis/V01C10_RELEASE.json`.

## Safety / remaining limits

Path A function, entryToken/secondaryContext validators, native lifecycle publisher/raw thunks, stack-store helpers, B installer/compact proof, watch coordinator, Position inspector, quaternion math and worker/hook framework match published c9. Exact exe2.810.0.0/SHA/original hashes and source ownership gates retained. Position proof0/corrections0/source Position writes0, no direct Actor/adhesion/movement/gravity/animation/camera/physics writes, protection change, new game ABI/hook/vendor copy.

A short already-admitted guarded store can overlap a later close, as in c9. A second concurrent advance after the single refresh bound still rejects/pass-through; no unlimited retry hides arbitrary scheduling. Foreign calls remain vanilla. Atomic counters are separately sampled; inflight/caps/drops/unique table saturation delimit diagnostic interpretation. Real c10 zero-WorldUp behavior cannot be inferred from synthetic tests.

## Outputs / human test

- **WallSprint_v0.1c10_secondary_publication_race.zip**
- **WallSprint_v0.1c10_source.zip**
- root **WallSprint.script** / **BUILD_MANIFEST.json**
- **analysis/V01C10_RELEASE.json** with implementation SHA, exact output hashes/sizes and compact validation evidence.

Use GitHub README_TEST: same diagonal E wall, one continuous~4s WallRun first, then A wall once if E is clean. Confirm rotation~.7s/full diagonal Surface Up, hold through normal Deinit, no flash/snap/crash/Position corruption. Return startup-through-cleanup full log PRIVATE plus visual observation. Require no post-full-Surface-Up actor_up=(0,1,0) while source active, no benign-refresh secondary_source_publication_changed vanilla leak, retention1/untilDeinit1/verified1/firstCause none/hard0/Position0. Natural race should produce refresh-admitted evidence; no natural refresh is acceptable with clean hold because deterministic native fixture covers it. Do not advance movement/active adhesion until the human E regression passes and a new handoff authorizes the next task.

## Persistence

Implementation is committed at the SHA above; result/ZIPs/hashes are a following delivery commit on the same dedicated branch. Push and authenticated Git retrieval verify the delivered ZIP hashes and test/result documents. Direct links use GitHub raw ZIP and GitHub blob README_TEST, not a workspace preview. Anonymous private-repository browser download is not claimed. No raw user log/video/full captured game function is committed or packaged.
