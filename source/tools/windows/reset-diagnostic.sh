#!/usr/bin/env bash
# Diagnose owned reset fixtures in a case-sensitive Linux tree; never deploy.
set -euo pipefail
prep_source=${1:-$(cd "$(dirname "$0")/../.." && pwd)}
[[ -f "$prep_source/tools/build_windows.sh" && -f "$prep_source/tools/test_reset_trace.sh" ]] || {
 echo 'ERROR: expected the preparation checkout source directory' >&2; exit 1;
}
reset_stage=$(mktemp -d /tmp/msm2-ws-reset-diagnostic.XXXXXX)
printf 'Diagnostic folder: %s\n' "$reset_stage"
trap 'reset_status=$?; printf "Reset diagnostic exit: %s; retained folder: %s\n" "$reset_status" "$reset_stage"' EXIT
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
bash tools/build_windows.sh 2>&1 | tee logs/reset-diagnostic-build.log
bash tools/test_reset_trace.sh 2>&1 | tee logs/reset-diagnostic-console.log
# The reported admission failure is intermittent: exercise the same unchanged
# short-window fixture again without rebuilding or repeating the migration.
for reset_attempt in 1 2 3 4 5; do
 printf 'Reset arm repetition %s/5\n' "$reset_attempt" | tee -a logs/reset-diagnostic-console.log
 WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/immediate_reset_smoke.exe" 2>&1 | tee -a logs/reset-diagnostic-console.log
done
