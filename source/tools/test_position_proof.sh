#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gpp=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}/usr/bin/x86_64-w64-mingw32-g++-posix
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -Iinclude -static src/NativeHooks/path_a_position_proof_smoke.cpp build/windows/path_a_position_proof.o build/windows/movement_lineage.o build/deps/capstone-5.0.6/capstone.lib -o build/windows/path_a_position_proof_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/path_a_position_proof_smoke.exe" | tee logs/windows_position_proof_smoke.log
