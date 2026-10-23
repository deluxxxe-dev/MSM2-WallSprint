#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gpp=/workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-g++-posix
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -Iinclude -Ivendor/minhook/include -static src/NativeHooks/immediate_reset_smoke.cpp build/windows/orientation_source_thunk.o build/windows/guard.o build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o -lbcrypt -o build/windows/immediate_reset_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/immediate_reset_smoke.exe" | tee logs/windows_reset_trace_smoke.log
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/immediate_reset_smoke.exe" --helper | tee logs/windows_reset_coordinator_smoke.log
/workspace/toolchains/analysis-python/bin/python tools/test_reset_writer_review.py | tee logs/reset_writer_review_tests.log

WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/immediate_reset_smoke.exe" --prearm | tee logs/windows_prearmed_reset_smoke.log

WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/immediate_reset_smoke.exe" --sequence | tee logs/windows_dual_reset_sequence_smoke.log
