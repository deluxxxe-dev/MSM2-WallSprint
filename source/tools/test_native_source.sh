#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gcc=/workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-gcc-posix
gpp=/workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-g++-posix
"$gcc" -c -g src/msm2/native_source_test_caller.S -o build/windows/native_source_test_caller.o
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -Iinclude -Ivendor/minhook/include -static src/msm2/native_source_override_smoke.cpp build/windows/native_source_test_caller.o build/windows/orientation_source_thunk.o build/windows/reset_copy_thunk.o build/windows/hook_capture.o build/windows/path_b_layout.o build/windows/stack_source.o build/windows/guard.o build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o -lbcrypt -o build/windows/native_source_override_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/native_source_override_smoke.exe" | tee logs/windows_native_source_override_smoke.log
