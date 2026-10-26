#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build logs
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/NativeHooks/vch_search_bridge_tests.cpp -o build/vch_search_bridge_tests
./build/vch_search_bridge_tests | tee logs/vch_search_bridge_model.log
gpp=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}/usr/bin/x86_64-w64-mingw32-g++-posix
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -D_WIN32_WINNT=0x0a00 -Iinclude -static src/NativeHooks/vch_search_bridge_smoke.cpp build/windows/guard.o build/windows/path_a_position_proof.o build/windows/movement_lineage.o build/windows/rdx_source_contract.o build/windows/writer_entry_snapshot.o build/windows/writer_entry_thunk.o build/deps/capstone-5.0.6/capstone.lib build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o -lbcrypt -o build/windows/vch_search_bridge_smoke.exe
: > logs/windows_vch_search_bridge.log
for scenario in close --timeout --kill; do
 for mode in reset position; do
  args=(--mixed-search-vch);if [[ "$scenario" != close ]];then args+=("$scenario");fi;if [[ "$mode" == position ]];then args+=(--position);fi
  WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/vch_search_bridge_smoke.exe" "${args[@]}" | tee -a logs/windows_vch_search_bridge.log
 done
done
python3 tools/check_vch_search_bridge.py logs/windows_vch_search_bridge.log
# Keep bridge-disabled mixed resurrection and foreign-only controls in this
# focused command too. Execute the original c48 harness without modifying it.
bash tools/test_production_witness_review.sh
