#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gpp=/workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-g++-posix
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -Iinclude -Ivendor/minhook/include -static src/NativeHooks/reset_copy_abi_smoke.cpp build/windows/compact_map_proof.o build/windows/reset_copy_thunk.o build/windows/hook_probe_test.o build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o build/deps/capstone-5.0.6/capstone.lib -lbcrypt -o build/windows/reset_copy_abi_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/reset_copy_abi_smoke.exe" | tee logs/windows_reset_copy_abi_smoke.log
python tools/test_dual_source_review.py
