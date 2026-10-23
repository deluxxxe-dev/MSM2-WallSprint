#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gpp=/workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-g++-posix
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -Iinclude -Ivendor/minhook/include -static src/msm2/orientation_source_smoke.cpp build/windows/orientation_source_thunk.o build/windows/hook_probe_test.o build/windows/hook_capture.o build/windows/native_source_override.o build/windows/path_b_layout.o build/windows/reset_copy_source.o build/windows/compact_map_proof.o build/windows/reset_copy_thunk.o build/windows/immediate_reset_trace.o build/windows/orientation_trace.o build/windows/source_lifecycle.o build/windows/stack_source.o build/windows/guard.o build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o build/deps/capstone-5.0.6/capstone.lib -lbcrypt -o build/windows/orientation_source_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/orientation_source_smoke.exe" | tee logs/windows_orientation_source_smoke.log
/workspace/toolchains/analysis-python/bin/python tools/test_orientation_source_review.py | tee logs/orientation_source_review_tests.log
