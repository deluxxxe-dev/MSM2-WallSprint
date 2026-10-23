# Codex result — WallSprint v0.1c6

Branch: **codex/v01c6-planar-adhesion**.
Implementation commit: **6ed21d411b8b6868185143d275c2e40486b57781**.
Base: published c5 **4917a3af9afb683db7f7b130626f5b25e20aebca**. The pre-created branch was used; authoritative handoff commits0c7305f/ffef8bb/2d2f2be were imported. PROJECT_STATE/CODEX_NEXT retain the exact main handoff. No main rewrite or force push.

## Outcome: permitted read-only c6 diagnostic milestone

v0.1c6 / package **0.1.24** preserves c5 orientation and implements the task's explicitly permitted Position-read-only outcome. **No Position correction is enabled or implemented.** The safe Position/W/downstream-exclusive-effects proof is incomplete. Full adhesion PASS is not claimed.

C5 real lifecycle-wide orientation is now authoritative PASS:3843ms WallRun, A219/B218, retained2906ms/205 updates, clean native Deinit. This permits investigating planar adhesion; it does not prove Position source ownership. The source/caller Position instruction bodies are not supplied in the checked-in runtime evidence for independent cloud replay. Instead of inferring permission from a layout/hash, c6 inspects the authenticated original functions at game startup and ships precise bounded candidates/rejection reasons plus valid-plane telemetry.

## Implementation and files

- `source/include/NativeHooks/path_a_position_proof.hpp`, `source/src/NativeHooks/path_a_position_proof.cpp`: existing pinned static Capstone fully decodes584-byte original writer and168-byte original caller, never hooked bytes. Tracks exact alias/stack/move and clobber facts, exact E8 destination/return/RDX relation where supported, Position read/store candidates, component mask and branch/call targets. Access/control records cap24/16 with suppressed counts. Arithmetic/unknown effects cannot become a direct-copy proof; linear candidates are labeled not path-complete. W semantics and exclusive downstream effects remain unproven even for an apparent complete copy, so admitted stays0 in this revision.
- `source/include/msm2/orientation_source.hpp` / `source/src/msm2/orientation_source.cpp`: one internal original-writer snapshot accessor, available only while the existing whole-writer ownership/authentication holds. The original saved body was authenticated before MinHook. No raw function bytes are logged or exported as a loader API.
- `source/src/NativeHooks/native_source_override.cpp`: startup PATH_A_POSITION_PROOF / ACCESS / CONTROL, first2 authenticated-A source Position/W/provenance records/session, worker read-only planar samples and once-at-source-end ADHESION_SUMMARY. Plane sampling checks the existing active publication/session/generation/owner/Hero/Transform/epoch, validates P338/N398 and revalidates native identity/freshness/open epoch around the reads. Known Deinit/kill/identity loss blocks owner reads; racing closes invalidate observations. Invalid planes do not stop otherwise valid c5 orientation.
- `source/include/Math/planar_standoff.hpp`: finite plane/normal validation and shadow request only: distance dot(Hero-P338,normalized N398), target.5, deadzone.01, gain.25, cap.02, reject abs(error)>.35, no request before t1. Candidate displacement is wall-normal; vanilla tangential motion is untouched.
- New owned proof/math fixtures, extended actual MinHook source fixture and build/test links. Version/mod metadata and INI comment, root/source README_TEST, README/BUILD/WORKLOG and package/release manifests updated. Required V01C5_RESULT / PATH_A_POSITION_PROOF / PLANAR_ADHESION / V01C6_GATE and authoritative runtime mirror are included under root/source.

Sampling caps64 attempts/session, >=100ms between records and distinct native updates. Actual correction/Position-write/applied-step/tangent counters remain0. The requested correction is only a candidate. Summary error_after explicitly means the same unmodified observation, not a native post-write measurement or improvement. Normalized N398 must have raw length.95..1.05, abs(y)<=.20 and match the owned surface dot>=.995. Observations are separate guarded snapshots, not atomic physics values.

## Preserved safety

The c5 native lifecycle implementation, source-store helper, Path B installer/compact proof, hardware watcher/coordinator and raw hook/source thunks are unchanged. C5 orientation rows/interpolation/lock-free publication/retention/lifecycle-summary semantics are retained; new evidence is layered alongside them. Path A still writes48 orientation bytes and verifies Position16 unchanged; Path B still writes only36 bytes with scale/later metadata preserved. Exact executable/SHA, caller/writer/current patch, native thread, owner/session/generation/epoch/Hero/Transform/stack/freshness/kill gates remain.

No Position store helper, direct Actor write, page protection/PAGE_GUARD, new game call to set Position, typed unknown native ABI, extra hook or vendor copy is added. No movement/input/animation/camera/gravity/jump/corner/swing/combat work. INI cannot enable a Position writer that does not exist. Existing5-second-from-source-entry and4096-write/path limits remain.

## Validation

Completed exit0:

- Windows x64 build with WallSprint warnings as errors.
- Portable ASan/UBSan: **12 PASS**.
- DLL smoke: **4 PASS**.
- Full hooks/offline/actual-byte/native-thread suite: **38 PASS**, **4 explicit historical private-input SKIPs**.
- Actual accepted-build B writer444/caller128 regression executed: nine-pair full proof, authenticated production installer/owned restoration and mutation negatives remain PASS; no original game code executes. This is not a claim of an actual A Position proof replay.
- Owned584/168 Position fixtures cover complete decode, exact call/stack, direct/computed/conditional map, mutated call/stack, incomplete VEX/span and both evidence caps. Every case remains denied for Position, including a perfect-looking copy.
- A/E standoff math covers gain/deadzone/range/step/t1, tangent translation/projection and invalid/nonfinite/nonunit/steep controls. A translation check initially straddled the exact0.35 float discontinuity; test points now cover both sides away from that ambiguity. Production limits were not widened.
- Actual owned MinHook source session reached4.118s native elapsed, A186/B186, retained2995ms/186 updates and clean until-Deinit orientation summary. It produced28 valid plane samples and one clean read-only adhesion summary with zero corrections/Position writes. Existing fixture Init is backdated800ms; sustained commit loop spans about3 real seconds. Separate native publication fixture runs>=4 actual seconds with4 concurrent readers.
- Unreadable P338 and out-of-range distances refuse candidate correction while orientation stays active; valid requested step<=.02. Position16 remains byte-identical; no additional plane reads after Deinit/kill. Existing ABI/vector/stack/metadata/retention/negative/debug-restore tests remain PASS.
- One standalone fixture initially missed the new static inspector object; its link dependencies were corrected before the successful full run.

A fresh standalone source ZIP extraction rebuilt the Windows DLL successfully with private captures absent. Final archive/privacy validation passes ZIP CRC, complete current source/docs/licenses, nested script contents/hash, Position-disabled manifest and private full-function byte exclusion. Only selected independent validation summaries are distributed; no raw user log/video or synthetic full-function trace is packaged.

## Outputs and next human test

- **WallSprint_v0.1c6_planar_adhesion.zip**
- **WallSprint_v0.1c6_source.zip**
- refreshed root **WallSprint.script** / **BUILD_MANIFEST.json**

Use the same large A-direction planar wall for one continuous left-to-right3–4-second WallRun. Preserve startup build/B proof/dual admission; Position admitted=0 is expected. Return the complete PRIVATE log including startup Position proof/candidates, first A provenance, ADHESION_SAMPLE/SUMMARY and ORIENTATION_LIFECYCLE_SUMMARY. Require valid plausible plane/distance telemetry, Position writes0, orientation retained through verified native Deinit and cleanup_pending=0. Report visible hold plus new snap/jitter/penetration/outward pop/crash/Position corruption. E is not needed. Natural distance≈.5/deadzone with no request is valid; do not force movement.

Real c6 diagnostic telemetry and c5-orientation regression remain PENDING. Full adhesion remains blocked by Position whole-path/caller construction/W/downstream-exclusive-effect proof. The new offsets/control/entry records identify the precise missing slice for the next handoff. No raw log or full function was required to proceed with the explicitly allowed diagnostic outcome, and none is committed or packaged.

## Persistence

Implementation is in the exact SHA above. This result, refreshed ZIP/script/manifest and machine-readable release hashes are committed afterward on the same branch, then pushed and verified via authenticated Git retrieval. Final user links use that delivery commit: GitHub raw ZIP for direct download and GitHub blob README_TEST for the test page.
