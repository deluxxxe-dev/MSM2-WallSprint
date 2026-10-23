# v0.1c15 writer branch-aware RDX diagnostics

Run `bash tools/test_position_proof.sh` and `bash tools/test_native_source.sh` (including `tools/test_source_lifecycle.sh`), `bash tools/test_reset_trace.sh` and `bash tools/test_third_writer.sh` after the Windows build for sustained native publication/MinHook A-B continuity, production async threshold, actual Toolhelp filtering, record-pressure, debug-register/second-thread capture and cleanup fixtures. It uses owned functions/memory only. No new external dependency is added in c4.

The Windows build fetches Capstone 5.0.6 from its upstream source archive, verifies SHA256 `240ebc834c51aae41ca9215d3190cc372fd132b9c5c8aa2d5f19ca0c325e28f9`, and statically builds its full x86 decoder (no diet/reduced ISA). `tools/setup_runtime_decoder.sh` caches only the archive under `/workspace/toolchains/decoder-cache`; generated sources/library stay under `build/deps`. No external Capstone DLL is needed. BSD/LLVM notices are in `vendor/capstone/`.

For the private actual-byte regression after build and `tools/test_dual_source.sh`, run `WALLSPRINT_PATH_B_CAPTURE=/private/startup-excerpt.log bash tools/test_compact_proof.sh`. The input must contain exactly one authenticated original writer444/caller128 capture with the documented hashes. The harness compares the published c1 decoder with c2 and exercises the production installer in a nonexecuted owned image. It emits hashes, counts and derived offsets only, and removes temporary input binaries. Do not place the private input in Git or a release ZIP.

Current version0.1.33: exact-build Path A and compact Path B consume one lock-free coherent orientation snapshot. Path B writes only its first nine floats and preserves row scale and all later metadata. See README_TEST.md and analysis/V01C11_GATE.md. Build with tools/build_windows.sh; run tools/test.sh, tools/smoke_windows.sh, tools/test_hooks_windows.sh and tools/release.py. Original game captures stay private.

# WallSprint v0.1b8 build — package0.1.17

Use the existing checkout; isolated cloud tasks do not need an extra worktree. From `/workspace/WallSprint/source`:

```bash
WALLSPRINT_WITH_WINE=1 bash tools/setup_cloud.sh
bash tools/setup_analysis.sh
[[ -e /workspace/toolchains/wine/usr/share/wine/wine ]] || ln -s . /workspace/toolchains/wine/usr/share/wine/wine
bash tools/test.sh
bash tools/build_windows.sh
bash tools/smoke_windows.sh
bash tools/test_hooks_windows.sh
python tools/release.py
```

Pinned GCC14 POSIX, Clang19 SEH, MinHook1.3.4, Wine10 and hash-pinned Capstone5.0.6 remain. `smoke_windows.sh` restores the relocated Wine server wrapper after extraction. Never re-extract packages concurrently with Wine tests. CMake intentionally directs users to this cross-build path.

The hooks suite includes b7 prearm, original source/ABI/20k-call/stack/Position tests, dual native source transitions/retention/late same-update calls, real second-thunk MinHook vector/return/FPU preservation, exact-hash denial and offline gate negatives. No real game instructions are executed in synthetic fixtures. Private original-game replay is optional SKIP if unavailable, never a fake PASS.

Current outputs: `../WallSprint_v0.1c15_writer_branch_contract.zip`, `../WallSprint_v0.1c15_source.zip`; generated script/manifest in `output/WallSprint_v0.1c15/`. GitHub branch/implementation commit are in `docs/CODEX_RESULT.md`; the public source includes all build/test instructions and known files needed to reproduce the implementation, without private game captures.

Offline real-log review:

```bash
python tools/review_dual_source.py /path/to/WallSprint.log --a-session 1 --e-session 2 --output /path/to/DUAL_SOURCE_REVIEW.json
```

A/E numbers are examples; confirm actual sessions from the user. For unknown reset stores, use the existing Capstone writer decoder with private bounded complete function captures. Native data-breakpoint RIP is post-store; do not guess its write address, function hash or source ABI. No real b8 ownership is verified until the accepted game supplies the entry/retention evidence. No adhesion is implemented.

C10 adds deterministic guarded-reader interposition only in the owned native fixture (no production test hook). A real owner Update/worker publication occurs after old secondary payload computation. Tests require latest diagonal/partial basis, latest stack scale/metadata, unchanged native owner, clean primary continuation, strict one-refresh bound and fail-closed identity/token/integrity/stale/stop races. Refresh records cap16/process, independent first record, full atomic counters.

C11: tools/test_position_writer.sh exercises the shared real hardware engine after Up cleanup (foreign DR/caps/restore/anchored stores); tools/test_native_source.sh additionally builds the same production c10 source fixture with the real Position watch linked, A/E orientation retained through normal Deinit. Position causal certificate/negative tests live in test_position_proof.sh; cardinal/diagonal movement delta is also portable ASan/UBSan. No game bodies are supplied by the public source.

C12: tools/test_movement_lineage.sh adds complete scalar/vector source maps, anchored direct caller/return verification, supported caller substitution/untyped arithmetic, invalid CFG/effect/W/overwrite cases and A/E correlation. test_position_writer.sh also reconstructs four owned scalar stores from real hardware exception contexts and checks new guard-page reads refuse without protection changes. Source contains no private 0x2cfc900/0x351eeb0 game body.

C13: tools/test_writer_entry.sh builds independent owned raw-thunk ABI, exact hash/boundary/lifecycle/guard gates, actual concurrent+nested hardware invocation pairing, candidate/chain and cleanup/foreign-ownership fixtures. It is included in test_hooks_windows.sh. The cross-build compiles WallSprint-owned writer_entry_snapshot.cpp / writer_entry_thunk.S; no new dependency or private game body. The original c12 source analyzer remains unchanged.

C14: tools/test_rdx_contract.sh adds owned scalar/packed AVX/shuffle/double-lane RDX XYZ plus independent W, exact stack allocation/constructor and direct/wrapper/tail/indirect relation fixtures with refusal cases. tools/test_writer_entry.sh now tests sourceW0/destinationW1 in real concurrent+nested hardware maps and isolated session/duplicate/mismatch/partial/zero-capture accounting. Both are included in the hooks suite. Build links rdx_source_contract.cpp; no added dependency. C12 analyzer, source/lifecycle and hook cleanup are preserved. Actual c14 game causality/producer/lifetime remains pending.

C15: tools/test_writer_branch.sh tests independently authored183-byte conditional fixtures at exact known store offsets: reconvergence/all-path provenance, one-leg mutations, bypass/unreachable sites, nested4 branches/all16 paths, caps/loops/indirect/outside/mid-instruction/unsupported/memory refusal and independent W. test_writer_entry.sh additionally checks real preactivation/active drains versus stop-only final summaries. Both are in test_hooks_windows.sh. The old c14 straight-line/caller/producer API and c12 analyzer remain unchanged; no new dependency or game body. All test scripts create their output directories.

C16: tools/test_writer_call.sh adds independently authored nonexecuted exact-call/no-write-leaf/clobber/reload/memory/control/target/return/stack-alias/W tests. Included in full hooks suite. No new dependency or runtime observation hook; tests remain isolated from private game bytes.
