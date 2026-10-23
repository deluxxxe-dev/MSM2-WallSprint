#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gpp=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}/usr/bin/x86_64-w64-mingw32-g++-posix
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -Iinclude -static src/NativeHooks/source_lifecycle_smoke.cpp -o build/windows/source_lifecycle_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/source_lifecycle_smoke.exe" | tee logs/windows_source_lifecycle_smoke.log
