#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/windows logs
toolchain_root=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}
"$toolchain_root/usr/bin/x86_64-w64-mingw32-g++-posix" -std=c++17 -O2 -g -Wall -Wextra -Werror -Iinclude -static src/GameInterop/log_file_smoke.cpp -o build/windows/log_file_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/log_file_smoke.exe" | tee logs/windows_log_file_tests.log
