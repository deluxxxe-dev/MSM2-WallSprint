#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/windows logs
toolchain_root=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}
"$toolchain_root/usr/bin/x86_64-w64-mingw32-g++-posix" -std=c++17 -O2 -g -Wall -Wextra -Werror -Iinclude -static src/NativeHooks/callee_branch_alias_smoke.cpp build/windows/rdx_source_contract.o build/deps/capstone-5.0.6/capstone.lib -o build/windows/callee_branch_alias_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/callee_branch_alias_smoke.exe" | tee logs/windows_callee_branch_alias_tests.log
