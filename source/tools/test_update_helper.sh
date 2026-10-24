#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/windows logs
gpp="${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}/usr/bin/x86_64-w64-mingw32-g++-posix"
"$gpp" -std=c++17 -O2 -Wall -Wextra -Werror -static tools/fixtures/update_for_test_host.cpp -o build/windows/update_for_test_host.exe
"$gpp" -std=c++17 -O2 -Wall -Wextra -Werror -shared -static -DWS_HELPER_DEPENDENCY tools/fixtures/update_for_test_host.cpp -o build/windows/update_helper_dependency.dll
python tools/test_update_helper.py | tee logs/windows_update_helper_tests.log
