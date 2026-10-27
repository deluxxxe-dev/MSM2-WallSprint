#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build logs
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/NativeHooks/vch_post_return_witness_tests.cpp -o build/vch_post_return_witness_tests
./build/vch_post_return_witness_tests | tee logs/vch_post_return_witness_model.log
./build/vch_post_return_witness_tests --json-fixture > logs/vch_post_return_witness_synthetic.json
gpp=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}/usr/bin/x86_64-w64-mingw32-g++-posix
"$gpp" -c src/NativeHooks/continuation_witness_harness.S -o build/windows/c51_reviewed_machine.o
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -D_WIN32_WINNT=0x0a00 -Iinclude -Ibuild/deps/capstone-5.0.6/include -static src/NativeHooks/vch_post_return_witness_smoke.cpp build/windows/c51_reviewed_machine.o build/windows/guard.o build/windows/path_a_position_proof.o build/windows/movement_lineage.o build/windows/rdx_source_contract.o build/windows/writer_entry_snapshot.o build/windows/writer_entry_thunk.o build/deps/capstone-5.0.6/capstone.lib build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o -lbcrypt -o build/windows/vch_post_return_witness_smoke.exe
: > logs/windows_vch_post_return_witness.log
for extra in --stub-contract-test ordinary --unrelated-worker; do
 args=(--position --mixed-search-vch);if [[ "$extra" != ordinary ]];then args+=("$extra");fi
 WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/vch_post_return_witness_smoke.exe" "${args[@]}" | tee -a logs/windows_vch_post_return_witness.log
done
python3 tools/check_vch_post_return_witness.py logs/windows_vch_post_return_witness.log logs/vch_post_return_witness_synthetic.json
