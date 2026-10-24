#!/usr/bin/env bash
# Diagnose owned reset fixtures in a case-sensitive Linux tree; never deploy.
set -euo pipefail
prep_source=${1:-$(cd "$(dirname "$0")/../.." && pwd)}
[[ -f "$prep_source/tools/build_windows.sh" && -f "$prep_source/tools/test_reset_trace.sh" ]] || {
 echo 'ERROR: expected the preparation checkout source directory' >&2; exit 1;
}
reset_stage=$(mktemp -d /tmp/msm2-ws-reset-diagnostic.XXXXXX)
printf 'Diagnostic folder: %s\n' "$reset_stage"
# Retain this owned directory and its logs after either success or failure.
tar -C "$prep_source" --exclude=./build --exclude=./logs --exclude=./output --exclude=__pycache__ -cf - . | tar -C "$reset_stage" -xf -
cd "$reset_stage"
mkdir -p logs
[[ ! -e tools/compat/windows.h ]] || {
 echo 'ERROR: build folder must distinguish Windows.h from windows.h' >&2; exit 1;
}
export WALLSPRINT_TOOLCHAIN_ROOT=/workspace/toolchains/windows
export WALLSPRINT_DECODER_CACHE=/workspace/toolchains/decoder-cache
unset WALLSPRINT_PATH_B_CAPTURE
bash tools/build_windows.sh
bash tools/test_reset_trace.sh
