#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gpp=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}/usr/bin/x86_64-w64-mingw32-g++-posix
"$gpp" -c src/NativeHooks/continuation_witness_harness.S -o build/windows/continuation_witness_harness.o
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -D_WIN32_WINNT=0x0a00 -Iinclude -Ibuild/deps/capstone-5.0.6/include -static src/NativeHooks/continuation_witness_smoke.cpp build/windows/continuation_witness_harness.o build/windows/guard.o build/windows/path_a_position_proof.o build/windows/movement_lineage.o build/windows/rdx_source_contract.o build/windows/writer_entry_snapshot.o build/windows/writer_entry_thunk.o build/deps/capstone-5.0.6/capstone.lib build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o -lbcrypt -o build/windows/continuation_witness_smoke.exe
: > logs/windows_continuation_witness_tests.log
for scenario in close --timeout --kill --nested --multiple --cap; do
 for mode in reset position; do
  args=();if [[ "$scenario" != close ]];then args+=("$scenario");fi;if [[ "$mode" == position ]];then args+=(--position);fi
  WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/continuation_witness_smoke.exe" "${args[@]}" | tee -a logs/windows_continuation_witness_tests.log
 done
done
python3 tools/check_continuation_diagnostics.py logs/windows_continuation_witness_tests.log
