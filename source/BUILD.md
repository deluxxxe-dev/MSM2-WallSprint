# v0.1b9 diagnostic update

Current version0.1.18: Path B is read-only, Path A interpolation is a reference. See README_TEST.md and analysis/V01B9_GATE.md. Historical version claims below do not grant Path B layout/ownership proof. Build with tools/build_windows.sh; run tools/test.sh, tools/smoke_windows.sh, tools/test_hooks_windows.sh and tools/release.py. Original game captures stay private.

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
