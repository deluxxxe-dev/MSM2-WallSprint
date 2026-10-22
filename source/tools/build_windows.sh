#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
toolchain_root=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}
clang_bin="$toolchain_root/usr/lib/llvm-19/bin/clang"
gpp="$toolchain_root/usr/bin/x86_64-w64-mingw32-g++-posix"
export LD_LIBRARY_PATH="$toolchain_root/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
mkdir -p build/windows
common=(-std=c++17 -O2 -g -ffunction-sections -fdata-sections -DWINVER=0x0601 -D_WIN32_WINNT=0x0601 -DLOGLEVEL=0 -DNOMINMAX -Iinclude -Ivendor/minhook/src/hde -Itools/compat -isystem vendor/SM2ScriptTemplate -Wall -Wextra -Werror)
gcc="$toolchain_root/usr/bin/x86_64-w64-mingw32-gcc-posix"
for source in buffer hook trampoline; do
 "$gcc" -O2 -g -DNOMINMAX -Itools/compat -Ivendor/minhook/include -c "vendor/minhook/src/$source.c" -o "build/windows/minhook_$source.o"
done
"$gcc" -O2 -g -c vendor/minhook/src/hde/hde64.c -o build/windows/minhook_hde64.o
"$gcc" -c -g src/msm2/hook_thunks.S -o build/windows/hook_thunks.o
"$gpp" "${common[@]}" -Ivendor/minhook/include -c src/msm2/hook_capture.cpp -o build/windows/hook_capture.o
"$gpp" "${common[@]}" -Ivendor/minhook/include -c src/msm2/wallrun_hooks.cpp -o build/windows/wallrun_hooks.o
"$gpp" "${common[@]}" -c src/msm2/update_bridge.cpp -o build/windows/update_bridge.o
"$gcc" -c -g src/msm2/orientation_source_thunk.S -o build/windows/orientation_source_thunk.o
"$gpp" "${common[@]}" -Ivendor/minhook/include -c src/msm2/orientation_source.cpp -o build/windows/orientation_source.o
"$gpp" "${common[@]}" -c src/msm2/orientation_trace.cpp -o build/windows/orientation_trace.o
"$gpp" "${common[@]}" -c src/NativeHooks/source_lifecycle.cpp -o build/windows/source_lifecycle.o
"$gpp" "${common[@]}" -c src/NativeHooks/immediate_reset_trace.cpp -o build/windows/immediate_reset_trace.o
"$gpp" "${common[@]}" -c src/NativeHooks/native_source_override.cpp -o build/windows/native_source_override.o
"$gpp" "${common[@]}" -c src/GameInterop/stack_source.cpp -o build/windows/stack_source.o
"$gpp" "${common[@]}" -c src/msm2/actor_writer.cpp -o build/windows/actor_writer.o
"$gpp" "${common[@]}" -c src/msm2/sdk_bridge.cpp -o build/windows/sdk_bridge.o
"$gpp" "${common[@]}" -c src/msm2/public_transform.cpp -o build/windows/public_transform.o
"$gpp" "${common[@]}" -c src/msm2/orientation_log.cpp -o build/windows/orientation_log.o
"$gpp" "${common[@]}" -c src/msm2/telemetry.cpp -o build/windows/telemetry.o
"$clang_bin" --target=x86_64-w64-windows-gnu --sysroot="$toolchain_root/usr/x86_64-w64-mingw32" -O2 -g -fms-extensions -Wall -Wextra -Werror -c src/msm2/guard.c -o build/windows/guard.o
"$gpp" -shared -static -static-libgcc -static-libstdc++ -o build/windows/WallSprint.dll build/windows/sdk_bridge.o build/windows/telemetry.o build/windows/guard.o build/windows/public_transform.o build/windows/orientation_log.o build/windows/hook_capture.o build/windows/hook_thunks.o build/windows/wallrun_hooks.o build/windows/actor_writer.o build/windows/orientation_trace.o build/windows/orientation_source.o build/windows/orientation_source_thunk.o build/windows/source_lifecycle.o build/windows/native_source_override.o build/windows/immediate_reset_trace.o build/windows/stack_source.o build/windows/update_bridge.o build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o -lbcrypt -lversion -luser32 -Wl,--exclude-all-symbols,--gc-sections,--dynamicbase,--nxcompat
