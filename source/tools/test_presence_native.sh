#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}
gcc="$root/usr/bin/x86_64-w64-mingw32-gcc-posix"
gpp="$root/usr/bin/x86_64-w64-mingw32-g++-posix"
mode=${1:-fixed}
[[ "$mode" == fixed || "$mode" == legacy-counterexample ]] || exit 2
build="$PWD/build/presence-owned-$mode"
legacy=0
[[ "$mode" != legacy-counterexample ]] || legacy=1
mkdir -p "$build"
for name in buffer hook trampoline hde/hde64; do
 "$gcc" -O2 -c "vendor/minhook/src/$name.c" -o "$build/${name##*/}.o"
done
"$gcc" -Wa,--defsym,PRESENCE_LEGACY_UNWIND=$legacy -c tests/presence/fixture.S -o "$build/fixture.o"
"$gcc" -c src/msm2/hook_probe_test.S -o "$build/invoke.o"
"$gpp" -std=c++17 -O2 -g -municode -Wall -Wextra -Werror -static -Ivendor/minhook/include -Ivendor/minhook/src/hde tests/presence/fixture.cpp "$build/fixture.o" "$build/invoke.o" "$build/buffer.o" "$build/hook.o" "$build/trampoline.o" "$build/hde64.o" -ldbghelp -lversion -o "$build/presence-owned.exe"
set +e
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$build/presence-owned.exe" "Z:$build/result.json"
rc=$?
set -e
python3 - "$build/result.json" "$mode" "$rc" <<'PYTEST'
import json,sys
r=json.load(open(sys.argv[1]));mode=sys.argv[2];rc=int(sys.argv[3])
if mode=='legacy-counterexample':
    failed=[x['name'] for x in r['assertions'] if not x['pass']]
    expected={f'observer restore {site} step{step} {api}' for site in range(2) for step in range(1,9) for api in ['StackWalk64 caller','VirtualUnwind stack/nonvolatiles']}
    assert rc==1 and set(failed)==expected and len(failed)==32, (rc,failed)
    print('PASS: retained legacy unwind counterexample: exactly32 restore assertions fail')
else:
    assert rc==0 and r['fail']==0,r
PYTEST
