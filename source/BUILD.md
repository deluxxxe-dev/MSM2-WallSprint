# WallSprint v0.1b7 build — package 0.1.16

Linux cloud workflow: cross-build the Windows x64 DLL and run independent Wine/native hardware/ABI tests. Spider-Man2.exe is unavailable here; actual A/E reset-path verification remains pending.

From `/workspace/WallSprint/source`:

```bash
WALLSPRINT_WITH_WINE=1 bash tools/setup_cloud.sh
bash tools/setup_analysis.sh
# Relocated Wine data directory: create only when missing.
[[ -e /workspace/toolchains/wine/usr/share/wine/wine ]] || ln -s . /workspace/toolchains/wine/usr/share/wine/wine
bash tools/test.sh
bash tools/build_windows.sh
bash tools/smoke_windows.sh
bash tools/test_hooks_windows.sh
python tools/release.py
```

The repository-pinned GCC14 POSIX, Clang19 SEH leaf, MinHook1.3.4, Wine10 and hash-pinned Capstone5.0.6 are retained. `smoke_windows.sh` restores the local Wine server wrapper after package extraction. Do not re-extract toolchain packages while Wine tests run. CMake intentionally directs users to the cross-build script.

Outputs: `../WallSprint_v0.1b7_prearmed_reset_capture.zip` and `../WallSprint_v0.1b7_source.zip`; generated script/manifest under `output/WallSprint_v0.1b7/`. Do not confuse earlier b6 archives with these outputs.

`test_hooks_windows.sh` includes original ABI/20k-call, lifecycle/negative source/position/stack tests, legacy reset tests and new prearm tests. The b7 fixture verifies native Update prearm before source delay, primary-first actual DR setup/readback, source/reset events, all-caller and invalid-source records, absolute deadline, occupied DR refusal, late request rejection, Deinit and cleanup. Optional private original-game replay skips when captures are absent. A Wine timing failure must be investigated/reported; it is never a passing zero-test run.

Offline private-log review:

```bash
/workspace/toolchains/analysis-python/bin/python tools/review_reset_writers.py /path/to/WallSprint.log --a-session 1 --e-session 2 --output /path/to/RESET_REVIEW.json
```

Confirm A/E mapping from the real recording/log; numbers above are examples. No raw private logs or captured game functions belong in release ZIPs. See `analysis/PREARMED_RESET_TRACE.md`, `analysis/RESET_WRITER_VERIFICATION.md`, and repository `docs/CODEX_RESULT.md` for gates and current validation. No reset writer/caller patch or next-milestone changes are authorized.
