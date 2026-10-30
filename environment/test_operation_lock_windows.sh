#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
out=$(mktemp -d /tmp/msm2-operation-win32.XXXXXX)
trap 'rm -rf -- "$out"' EXIT
/workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-g++-posix -std=c++17 -Wall -Wextra -Werror -O2 -static -municode environment/operation_lock_windows.cpp -o "$out/operation_lock.exe"
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$out/operation_lock.exe"
