#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gpp=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}/usr/bin/x86_64-w64-mingw32-g++-posix
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -Iinclude -Ibuild/deps/capstone-5.0.6/include -static src/NativeHooks/compact_proof_replay.cpp build/windows/compact_map_proof.o build/deps/capstone-5.0.6/capstone.lib -o build/windows/compact_proof_replay.exe
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -Iinclude -Ivendor/minhook/src/hde -static tools/fixtures/c1_compact_map_proof.cpp tools/fixtures/c1_compact_replay.cpp build/windows/minhook_hde64.o -o build/windows/c1_compact_replay.exe
if [[ -n "${WALLSPRINT_PATH_B_CAPTURE:-}" ]]; then
 python tools/replay_compact_proof.py --capture "$WALLSPRINT_PATH_B_CAPTURE"
else
 echo 'SKIP: private accepted-build capture replay; set WALLSPRINT_PATH_B_CAPTURE to its PATH_B_FUNCTION/PATH_B_CODE log (never redistribute). Independent proof tests execute in test_dual_source.sh.'
fi
