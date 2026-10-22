# Codex result — WallSprint v0.1b7

## Implemented

Package **0.1.16**, diagnostic-only prearmed reset capture. The existing exact-build/caller-local 48-byte orientation source override still begins at >=0.70 s and preserves Position. No reset patch, caller suppression, direct Actor/Position, adhesion/movement/camera/animation/physics behavior was added.

- Native verified WallRun Update requests prearm at >=0.60 s (matching common-writer entry is a fallback); it does not depend on worker-observed tilted Actor Up.
- Helper secures only the known primary native thread, with no preceding process-wide thread enumeration. Both DR slots are read back before reporting installed. Deadline is native Init+1.10 s, or earlier candidate/Deinit/identity/guard loss/F8/budget completion.
- Startup `RESET_TRACE_SELF_CHECK` explicitly reports build/hook/enabled/admitted/primary support; failed admission logs `V01B7_DIAGNOSTIC_NOT_ADMITTED`.
- `PREARM_STATUS first_override=1` records whether both slots were installed before the first source write began, using a separate pre-write QPC. Event timing snapshots the last committed override, not the later drain time.
- `COMMON_WRITER_TRACE` retains bounded read-only records for all current-Hero callers, including invalid-source records without interpreting invalid RDX. No caller restriction was removed from the actual source write.
- Hardware events retain raw post-store RIP/registers, unwound callers, before/after Up, normal and source association. Surface-to-WorldUp candidates and evidence-backed known-common-writer correlation are explicit. Unknown writers remain candidates with no invented source ABI.
- Offline analysis includes common WorldUp-after-surface entry candidates, first-override prearm gates, explicit A/E session mapping, existing exact full-function hash normalization and anchored effective-address decoding.

## Files changed

- `source/include/NativeHooks/{immediate_reset_trace,native_source_override,source_lifecycle}.hpp`, `source/include/msm2/orientation_trace.hpp`.
- `source/src/NativeHooks/{immediate_reset_trace,native_source_override,source_lifecycle}.cpp`, `source/src/msm2/orientation_trace.cpp`.
- Independent tests: `source/src/NativeHooks/immediate_reset_smoke.cpp`, `source/src/msm2/native_source_override_smoke.cpp`, `source/tools/{test_reset_trace.sh,test_reset_writer_review.py,smoke_windows.sh}`.
- Offline reviewers: `source/tools/{review_reset_writers.py,review_orientation_writers.py}`.
- Version/build/package: `source/include/version.hpp`, `source/mod/info.json`, `source/CMakeLists.txt`, `source/BUILD.md`, `source/README.md`, `source/tools/{package.py,release.py}`, root `WallSprint.script` and `BUILD_MANIFEST.json` refreshed from the generated package.
- Root/source `README_TEST.md` and mirrored root/source `analysis/{V01B6_RESULT,PREARMED_RESET_TRACE,COMMON_WRITER_RESET_CALLS,RESET_WRITER_VERIFICATION,V01B7_GATE}.md`; this result file.
- `docs/PROJECT_STATE.md`, `docs/CODEX_NEXT.md`, existing vendored source and original stack-write/assembly code are unchanged.

## Cloud validation

All final commands completed with exit **0**:

- `bash tools/build_windows.sh`: pinned GCC/Clang Windows x64 DLL, warnings as errors.
- `bash tools/test.sh`: 11 PASS reports with ASan/UBSan, no skipped checks.
- `bash tools/smoke_windows.sh`: DLL lifecycle/load/unload, diagnostic formatter, bounded object reader, wrong-build refusal and explicit diagnostic-not-admitted logs.
- `timeout 120s bash tools/test_hooks_windows.sh`: complete ABI/20k-call, lifecycle, original source/stack/Position/negative gates, independent native source, legacy reset, offline decode and new prearm regression suite.
- New native Update test requests at .61 s with zero source writes. New actual Wine DR/MinHook fixture arms at .60, confirms prearmed status at ~.71, captures surface then WorldUp stores, records all-caller/invalid-source entries, expires at 1.10, preserves occupied DR state, refuses late requests, cancels on Deinit and restores owned DR state. Unknown synthetic writer is never correlated as the known game common writer.
- Offline negative tests reject missing/false first-override prearm, drops, bad targets/source/correlation, absent or identical A/E mapping. Synthetic data never produces game verification.

Initial development runs found premature cancellation on a transient partially written basis; admission still checks initial basis sanity, while ongoing capture checks identity/lifecycle rather than orthogonality between native stores. Test event matching was corrected to distinguish `armed=1` from `prearmed=1`. A legacy 30 ms helper fixture also missed a timing-sensitive window once; its isolated rerun and final complete regression passed. No assertions were disabled or windows extended to conceal failures.

Optional private original-game fixture replays remain explicit SKIPs; all independent synthetic tests ran. Release validation checks DLL/version/INI/PE32+/exports/system imports/no WriteProcessMemory, ZIP CRC/hash/licenses/source and private-capture exclusions. Only selected independent cloud validation summaries are packaged; raw user logs and full game functions are excluded.

## Safety/admission and remaining uncertainty

The independent positive admission fixture reports all self-check fields 1. The non-game Windows smoke host correctly reports not admitted. **No accepted Spider-Man2.exe instance was run here. Actual game self-check, first-override prearm timing, reset writer/caller and A/E reproduction remain unverified. RESET_WRITER_VERIFIED=0.** Helper scheduling can still miss prearm; a first-override status of 0 is a failed diagnostic session. Primary-thread-only hardware coverage is intentional. Old Up is a previous observation and RIP is post-store; actual effective store address/function hash require the private anchored offline decode. OS scheduling can delay physical DR cleanup after capture is invalidated.

## Output / next human test

`WallSprint_v0.1b7_prearmed_reset_capture.zip` (install package) and `WallSprint_v0.1b7_source.zip` (public source). DLL/script/manifest also under `source/output/WallSprint_v0.1b7/`. Generated archives are local outputs; nothing was pushed or published.

Exit the game completely, disable old WallSprint, extract the install ZIP, add the inner `WallSprint.script` unopened to Overstrike and Install mods. Use the existing Script support / `-scripts`. Record ground ~2 s, Control A one left-to-right WallRun ~1.5 s, ground ~2 s, known diagonal Face E one left-to-right WallRun ~2 s, then exit. No Crawl or marker keys. Submit the unmodified `logs/WallSprint.log`, video and explicit A/E session mapping. Check startup self-check fields all 1, first-override prearm=1 and at least one hardware event. Visible rotation may still reset. F8 latches diagnostics/source OFF; restart fully before retesting. Do not advance to adhesion or patch a reset based on synthetic results.
