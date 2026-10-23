#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/windows logs
toolchain_root=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}
gpp=$toolchain_root/usr/bin/x86_64-w64-mingw32-g++-posix
gcc=$toolchain_root/usr/bin/x86_64-w64-mingw32-gcc-posix
"$gcc" -c src/NativeHooks/writer_entry_owned_test.S -o build/windows/writer_entry_owned_test.o
"$gcc" -c src/msm2/hook_probe_test.S -o build/windows/hook_probe_test.o
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -Iinclude -Ivendor/minhook/include -Ibuild/deps/capstone-5.0.6/include -static src/NativeHooks/writer_entry_snapshot_smoke.cpp build/windows/rdx_source_contract.o build/windows/writer_entry_thunk.o build/windows/writer_entry_owned_test.o build/windows/hook_probe_test.o build/windows/guard.o build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o build/deps/capstone-5.0.6/capstone.lib -lbcrypt -o build/windows/writer_entry_snapshot_smoke.exe
for mode in --guards --abi --pair --cleanup --accounting; do
 WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/writer_entry_snapshot_smoke.exe" "$mode"
done | tee logs/windows_writer_entry_tests.log
