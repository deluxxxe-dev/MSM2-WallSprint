#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
toolchain_root=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}
gpp="$toolchain_root/usr/bin/x86_64-w64-mingw32-g++-posix"
"$toolchain_root/usr/bin/x86_64-w64-mingw32-gcc-posix" -c src/msm2/hook_probe_test.S -o build/windows/hook_probe_test.o
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -Iinclude -Ivendor/minhook/include -static src/msm2/hook_abi_smoke.cpp build/windows/hook_probe_test.o build/windows/hook_thunks.o build/windows/hook_capture.o build/windows/update_bridge.o build/windows/guard.o build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o -o build/windows/hook_abi_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/hook_abi_smoke.exe" | tee logs/windows_hook_abi_smoke.log
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -Iinclude -Ivendor/minhook/src/hde -Ivendor/minhook/include -static src/msm2/wallrun_runtime_smoke.cpp build/windows/hook_thunks.o build/windows/hook_capture.o build/windows/update_bridge.o build/windows/guard.o build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o build/windows/actor_writer.o -lbcrypt -luser32 -o build/windows/wallrun_runtime_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/wallrun_runtime_smoke.exe" | tee logs/windows_wallrun_runtime_smoke.log
python tools/make_test_timeline.py logs/windows_surface_runtime_synthetic.log --output logs/TEST_TIMELINE_SYNTHETIC.txt
python tools/review_surface_candidates.py logs/windows_surface_runtime_synthetic.log > logs/SURFACE_CANDIDATES_SYNTHETIC.json
python tools/review_surface_source.py logs/windows_surface_runtime_synthetic.log --output-dir logs/source_synthetic
python tools/test_analysis.py
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -Iinclude -Ivendor/minhook/src/hde -static src/msm2/this_access_smoke.cpp build/windows/minhook_hde64.o -o build/windows/this_access_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/this_access_smoke.exe" | tee logs/windows_this_access_smoke.log
python tools/test_four_face_analysis.py
/workspace/toolchains/analysis-python/bin/python tools/test_static_access_v2.py | tee logs/static_access_v2_tests.log
python tools/test_static_field_review.py | tee logs/static_field_review_tests.log
python tools/test_surface_pair_review.py | tee logs/surface_pair_review_tests.log
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -Iinclude -Ivendor/minhook/include -static src/msm2/actor_writer_smoke.cpp build/windows/hook_capture.o build/windows/hook_thunks.o build/windows/update_bridge.o build/windows/guard.o build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o -luser32 -o build/windows/actor_writer_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/actor_writer_smoke.exe" | tee logs/windows_actor_writer_smoke.log

"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -Iinclude -static src/msm2/orientation_trace_smoke.cpp build/windows/guard.o build/windows/path_a_position_proof.o build/windows/movement_lineage.o build/deps/capstone-5.0.6/capstone.lib -lbcrypt -o build/windows/orientation_trace_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/orientation_trace_smoke.exe" | tee logs/windows_orientation_trace_smoke.log
/workspace/toolchains/analysis-python/bin/python tools/test_orientation_writer_review.py

bash tools/test_orientation_source.sh

bash tools/test_native_source.sh
/workspace/toolchains/analysis-python/bin/python tools/test_native_source_review.py | tee logs/native_source_review_tests.log

bash tools/test_reset_trace.sh

bash tools/test_dual_source.sh

/workspace/toolchains/analysis-python/bin/python tools/test_path_b_layout.py 2>&1 | tee logs/path_b_layout_tests.log

bash tools/test_compact_proof.sh

bash tools/test_third_writer.sh

bash tools/test_position_proof.sh

bash tools/test_position_writer.sh

bash tools/test_movement_lineage.sh
