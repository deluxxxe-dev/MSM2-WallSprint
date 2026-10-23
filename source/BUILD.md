# v0.1c3 third writer process-thread trace

Run `bash tools/test_third_writer.sh` after the Windows build for the actual debug-register/second-thread capture and cleanup fixture. It uses owned functions/memory only. No new external dependency is added in c3.

The Windows build fetches Capstone 5.0.6 from its upstream source archive, verifies SHA256 `240ebc834c51aae41ca9215d3190cc372fd132b9c5c8aa2d5f19ca0c325e28f9`, and statically builds its full x86 decoder (no diet/reduced ISA). `tools/setup_runtime_decoder.sh` caches only the archive under `/workspace/toolchains/decoder-cache`; generated sources/library stay under `build/deps`. No external Capstone DLL is needed. BSD/LLVM notices are in `vendor/capstone/`.

For the private actual-byte regression after build and `tools/test_dual_source.sh`, run `WALLSPRINT_PATH_B_CAPTURE=/private/startup-excerpt.log bash tools/test_compact_proof.sh`. The input must contain exactly one authenticated original writer444/caller128 capture with the documented hashes. The harness compares the published c1 decoder with c2 and exercises the production installer in a nonexecuted owned image. It emits hashes, counts and derived offsets only, and removes temporary input binaries. Do not place the private input in Git or a release ZIP.

Current version0.1.21: exact-build Path A and compact Path B consume one lock-free coherent orientation snapshot. Path B writes only its first nine floats and preserves row scale and all later metadata. See README_TEST.md and analysis/V01C3_GATE.md. Build with tools/build_windows.sh; run tools/test.sh, tools/smoke_windows.sh, tools/test_hooks_windows.sh and tools/release.py. Original game captures stay private.

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

Outputs: `../WallSprint_v0.1b8_dual_source_ownership.zip`, `../WallSprint_v0.1b8_source.zip`; generated script/manifest in `output/WallSprint_v0.1b8/`. Archive names distinguish previous b7 packages. GitHub branch/implementation commit are in `docs/CODEX_RESULT.md`; the public source includes all build/test instructions and known files needed to reproduce the implementation, without private game captures.

Offline real-log review:

```bash
python tools/review_dual_source.py /path/to/WallSprint.log --a-session 1 --e-session 2 --output /path/to/DUAL_SOURCE_REVIEW.json
```

A/E numbers are examples; confirm actual sessions from the user. For unknown reset stores, use the existing Capstone writer decoder with private bounded complete function captures. Native data-breakpoint RIP is post-store; do not guess its write address, function hash or source ABI. No real b8 ownership is verified until the accepted game supplies the entry/retention evidence. No adhesion is implemented.
