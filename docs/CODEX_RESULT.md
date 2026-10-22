# Codex result — WallSprint v0.1b8

## GitHub handoff

Branch: **codex/v01b8-dual-source**.

Implementation commit: **HANDOFF_IMPLEMENTATION_SHA** (filled after committing the implementation; the following handoff commit records this exact ancestor). Preserved b7 implementation baseline: **f26fe5a**. Base task instructions/runtime summary: **dd87768**. No main history rewrite or force push. GitHub publication is confirmed only after the final push/ref check, reported in chat.

## Changes

Package **0.1.17**, orientation-only dual-source ownership. The original common writer remains; no reset NOP/skip, direct Actor/Position, adhesion/movement/animation/camera/physics work was added.

- Reused and committed the surviving tested b7 prearm/common diagnostics before b8 changes.
- Added exact-original SHA256/unwind-boundary authentication for0x2CFC3C0..0x2CFC57C (444 bytes, ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196), exact direct caller0x2CFC84B and live full owned-body/caller integrity. Missing/changed/unsupported evidence fails closed.
- Added a second ABI-neutral raw-entry tailjump thunk. Its own RCX/RDX/R8/R9/return/RSP/thread/TEB bounds and source layout are recorded, never borrowed from stale common-writer entry data.
- Both source paths share one session entry time/matrix/quaternion/forward/target/current basis. Only the caller-local source orientation48 bytes are changed; each path preserves its own Position16 bytes and original native execution/return behavior.
- The second source must be a sane affine64-byte block inside the current thread's writable aligned caller-local stack band, above entry return+shadow space. The post-prologue b7 RSP+0x60 observation is not treated as an entry offset. Rejected sources log raw evidence/reason without modification.
- Common source coverage now uses fresh native elapsed instead of potentially lagging worker elapsed. Observing a reset no longer ends exact valid source ownership and lets later WorldUp calls through. Guard/identity/normal/stack/patch/kill/budget failures still stop safely. Multiple valid same-update calls are overridden; repeated WorldUp inputs are explicit late diagnostics.
- Retention requires both source paths to have committed, shared transition completion and dot(ActorUp,N)>=.98 for300 ms OR20 distinct native updates. Same-update bad observations also break continuity; duplicate calls cannot inflate counts. A >100 ms observation gap resets the interval. Worker samples cannot grant PASS.
- Retained primary prearm in a reduced verification role through~1.50 s in dual mode. Unchanged Up events skip expensive record/unwind/log work. A reset after both commits logs NEW_RESET_AFTER_DUAL_OVERRIDE; sequential dword resets can use an explicitly logged <=5 ms previous surface anchor without claiming atomic instruction-before values.
- Added read-only dual-source log review: exact header/hash/admission, own-entry layout/stack/caller/target/Position proof, retention and explicit A/E mapping. Later sampled loss/new-reset events prevent strong verification. Synthetic data never establishes game ownership or grants adhesion permission.

## Files

New production files: `source/include/NativeHooks/reset_copy_source.hpp`, `source/src/NativeHooks/reset_copy_source.cpp`, `source/src/NativeHooks/reset_copy_thunk.S`.

Shared/verification updates: `source/include/{GameInterop/stack_source,NativeHooks/native_source_override,NativeHooks/immediate_reset_trace}.hpp`; `source/src/{GameInterop/stack_source,NativeHooks/native_source_override,msm2/orientation_trace,msm2/telemetry}.cpp`.

Tests/tools: native source smoke/independent assembly caller, immediate reset smoke, new reset-copy ABI/auth smoke, new `tools/{review_dual_source.py,test_dual_source_review.py,test_dual_source.sh}`, build/hooks/native/orientation scripts, package/release scripts. Version/header/mod metadata/CMake/build README and root script/manifest refreshed.

Required analyses mirrored in root/source: `V01B7_RESULT.md`, `RESET_COPY_WRITER_ANALYSIS.md`, `DUAL_SOURCE_OWNERSHIP.md`, `V01B8_GATE.md`; supplied sanitized `V01B7_RUNTIME_RESULT.md` also included in source. Root/source `README_TEST.md` updated. `docs/PROJECT_STATE.md`, `docs/CODEX_NEXT.md`, vendor code and existing common raw thunk are unchanged by implementation.

## Validation

Final build and suites completed with exit **0**:

- `bash tools/build_windows.sh`: pinned GCC/Clang Windows x64 DLL, warnings as errors.
- `bash tools/test.sh`: ASan/UBSan portable checks,11 PASS reports.
- `bash tools/smoke_windows.sh`:4 PASS reports, DLL lifecycle/formatter/bounded reader and explicit wrong-build refusal. Copy/dual admission correctly remain0 on the non-game host.
- `timeout 150s bash tools/test_hooks_windows.sh`: complete existing ABI/20k-call, observer/lifecycle/source/stack/Position/negative/legacy reset/prearm regressions plus new dual-source tests;24 PASS reports. Four unavailable private original-game replays are explicit SKIPs, not passes.
- Independent real MinHook sources on A/E normals share transition and preserve parent input/Position/returns; cover same-update late calls and both >=20-update and real >=300 ms retention alternatives. Injected same-update reset prevents a false retention PASS. Heap, bad caller/stack band/layout, copy patch ownership loss, Deinit and existing wrong-thread/F8/stale/native gates refuse modification.
- Real second-thunk probe deliberately clobbers YMM/FPU state: original GPR/flags/stack/nonvolatiles/YMM0..15/returns/x87/MXCSR are preserved. SHA256 known vector and correct-unwind/wrong-original-body refusal pass. No captured game instructions are executed by these tests.
- Hardware fixture exercises dual-commit reset classification, unchanged-store suppression, primary prearm, absolute deadlines, occupied DR preservation, rejected late request and cleanup. Legacy private-game replay remains optional SKIP.
- Offline negative tests reject bad original hash, admission, caller, layout, stack, target, Position, insufficient retention, duplicate A/E mapping, synthetic headers, new resets and later retention loss.
- Package validation checks version/INI/DLL, PE32+/exports/system-only imports/no WriteProcessMemory, source/licenses, CRC/hashes and private-capture exclusions. Only selected independent cloud summaries are packaged; raw user logs/full game functions/reference MOD payloads are excluded.

Development fixes included keeping capture alive across transient partially written target bases and ensuring bad same-update observations reset retention. Initial compiler indentation warnings were fixed; no warning/assertion/checksum verification was disabled.

## Remaining game gate

The b7 reset identity is accepted as supplied project ground truth. **No accepted Spider-Man2.exe was run here for b8. Actual second-entry source layout/provenance, successful hook/caller admission and retained A/E orientation are pending real logs. ORIENTATION_OWNERSHIP_VERIFIED=0.** The exact caller supports a verified direct E8 call; other encoding/layout/provenance is refused rather than guessed. Hardware coverage is bounded/primary-only; post-store RIP and observed sequence anchors need private anchored decode. OS scheduling can delay physical DR cleanup. A retention marker is an observed interval, not proof that later resets are absent. No adhesion is implemented or automatically enabled.

## Outputs / human test

**WallSprint_v0.1b8_dual_source_ownership.zip** and **WallSprint_v0.1b8_source.zip**. Generated source/script/manifest are under `source/output/WallSprint_v0.1b8/`; root `WallSprint.script`/`BUILD_MANIFEST.json` match the new package. Final archives and this handoff are committed after the implementation commit so a fresh GitHub checkout preserves the deliverable; installation and build remain reproducible from repository source.

Fully exit game, disable old WallSprint, extract install ZIP, add inner script unopened to Overstrike / Install mods, retain existing Script support and `-scripts`. Ground~2 s, Control A one left-to-right WallRun~1.5 s, ground~2 s, same diagonal Face E one left-to-right WallRun~2 s, exit. No Crawl. Submit unmodified runtime log with explicit A/E sessions; video if convenient. Check both hook/admission paths, source Position preservation and ORIENTATION_RETENTION_PASS. Report any new reset/unknown writer, crash or Position corruption. F8 latches writes/diagnostics OFF; fully restart before retesting. Camera stays world-up and animation may look wrong; do not proceed to adhesion within this task.
