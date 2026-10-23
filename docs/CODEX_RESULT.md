# Codex result — WallSprint v0.1b9

Branch: **codex/v01b9-path-b-layout**. Implementation commit: **b89b05bb89f4c0cbd0ffa18e45bfef6e7f5baa1a**. Baseline preserved from b8 branch28910fb; latest main task/runtime instructions cherry-picked without discarding b7/b8 implementation. No main rewrite or force push.

## Changes and files

Package0.1.18, `WallSprint_v0.1b9_path_b_layout.zip` (+ source ZIP). Path B mutation removed unconditionally; unknown layout cannot reject/end Path A. Existing A stack-matrix interpolation and build/ABI/lifecycle/identity gates remain.

New `source/include/NativeHooks/path_b_layout.hpp`, `source/src/NativeHooks/path_b_layout.cpp`: bounded raw128 entry ring, independent signed stack math, pointer/span/relation/readability flags, N398/Hero-before/valid A reference. Current-thread guarded root reads only; pointer-looking data never followed. Window0.60..1.50,128 entries/session,8 sessions,256 queued; contention/budget reported.

`reset_copy_source.cpp`: original caller128/writer444 private capture before local hook; exact caller unwind bounds/E8 destination and existing original writer digest/owned patch validation. Full original game bytes are not in Git/ZIP.

`native_source_override.cpp`, `stack_source.*`: A rejection reasons with caller/target/stack/layout/lifecycle/elapsed facts; read-only B callback preserves original execution and LastError. `orientation_trace.*`, `immediate_reset_trace.hpp`: keep bounded hardware watch after reset, link each event to its own B entry with current-thread serial and <=5ms identity checks; stale common args never supply B provenance. New callbacks clear old association before any possible admission/capture failure.

`tools/review_path_b_layout.py`, `test_path_b_layout.py`: original span/SHA/exact-call/full-decode checks, conservative CFG argument/load/store provenance, post-store RIP/effective-address anchoring, raw scalar/vector/pointer observations and same-offset A/E correlation candidates. Unknown instructions/computations/spills/nested pointers stay unknown. This tool never authorizes writes. Build/link/tests/package/version/telemetry updated; required analyses mirrored in root/source; root/source README_TEST updated.

## Validation

Completed with exit0:

- Windows x64 GCC/Clang build with warnings as errors (`logs/v01b9_build.log`).
- Portable ASan/UBSan suite:11 PASS reports.
- Windows DLL lifecycle/formatter/bounded-reader/build-refusal smoke:4 PASS reports.
- Full Windows hooks suite:27 PASS reports and4 explicit optional private-fixture SKIPs; 11 new offline layout tests at that run. Final decoder revision:12 tests OK, including read-versus-write direction, direct copy, CFG join, clobbers/call, caller RSP math, bad hash/decode, wrong post-store PC/effective address/thread/stale QPC association. No synthetic game promotion.
- Final targeted native MinHook regression after callback reentrancy/LastError changes: PASS; B source unchanged, A continues, no B commit/retention PASS, guard-page refusal and existing A safety gates.
- Final hardware reset suite: continued watch captures second reset; own B serial attached to first event and explicitly invalidated for the next missing entry, without inheriting stale/common data; debug registers restored. Existing prearm/helper/sequential-store/timeout/Deinit tests pass;1 optional private replay SKIP.
- Release checks: version/INI/PE32+/exports/imports/no WriteProcessMemory, CRC/hash/source/licenses and private capture exclusions. Selected independent cloud summaries only; no full raw runtime logs/game functions/reference-mod payloads.

## Safety / unresolved actual evidence

Exact accepted executable2.810.0.0/SHA, both writer/caller authentication, native owner/generation/thread/lifecycle/freshness, current Hero and original ABI/return gates retained. No direct Actor/Position/adhesion/movement/camera/animation/physics writes. Path B read-only even if numerical vector candidates match. Existing A failure gates are not relaxed.

**Primary diagnostic PASS remains pending**: supplied b8 summary gives addresses/registers but no original instruction bytes; source field semantics and R8/R9/constants/dataflow cannot be proven from a hash. Static original caller/writer map and same real A/E field use are explicitly UNKNOWN, not fabricated. New original-byte capture + conservative decoder prepare that exact evidence. No real-game retention/ownership claim.

Next human test: README_TEST A ~1.5s, E ~2s, explicit sessions. Submit to ChatGPT normally. Needed keys: startup PATH_B_FUNCTION/PATH_B_CODE (caller128/writer444) and elapsed0.60..1.50 RESET_COPY_LAYOUT_ENTRY/PATH_B_ENTRY_LINK/ORIENTATION_WRITER_EVENT/ORIENTATION_WRITER_REGS; late COMMON_OVERRIDE_REJECT. Keep raw functions private; commit only derived map/conclusions. Do not request a broad full-log re-analysis.

## Handoff

Root WallSprint.script/BUILD_MANIFEST.json and final ZIPs refreshed after validation. Publication is confirmed only after final push and remote-ref verification in chat. docs/PROJECT_STATE.md and docs/CODEX_NEXT.md unchanged by implementation.
