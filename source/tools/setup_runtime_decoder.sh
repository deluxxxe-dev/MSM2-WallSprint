#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
toolchain_root=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}
cache_dir=${WALLSPRINT_DECODER_CACHE:-/workspace/toolchains/decoder-cache}
archive="$cache_dir/capstone-5.0.6.tar.gz"
expected=240ebc834c51aae41ca9215d3190cc372fd132b9c5c8aa2d5f19ca0c325e28f9
mkdir -p "$cache_dir" build/deps
if [[ ! -f "$archive" ]]; then
 curl -L --fail --silent --show-error https://codeload.github.com/capstone-engine/capstone/tar.gz/refs/tags/5.0.6 -o "$archive"
fi
echo "$expected  $archive" | sha256sum --check --status
if [[ ! -f build/deps/capstone-5.0.6/Makefile ]]; then
 tar -xzf "$archive" -C build/deps
fi
make -C build/deps/capstone-5.0.6 -j2 CAPSTONE_ARCHS=x86 CAPSTONE_SHARED=no CAPSTONE_STATIC=yes CAPSTONE_BUILD_CORE_ONLY=yes CAPSTONE_X86_ATT_DISABLE=yes CROSS="$toolchain_root/usr/bin/x86_64-w64-mingw32-" CC="$toolchain_root/usr/bin/x86_64-w64-mingw32-gcc-posix" capstone.lib
