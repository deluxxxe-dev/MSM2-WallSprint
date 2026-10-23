#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root=$(pwd)
wine_root=/workspace/toolchains/wine
if [[ ! -x "$wine_root/usr/lib/wine/wine64" ]]; then
 WALLSPRINT_WITH_WINE=1 bash tools/setup_cloud.sh
fi
# Relocate Debian's wrapper and NLS path without changing host /usr.
cat > "$wine_root/usr/lib/wine/wineserver" <<'WRAPPER'
#!/bin/sh
exec /workspace/toolchains/wine/usr/lib/wine/wineserver64 -p0 "$@"
WRAPPER
chmod +x "$wine_root/usr/lib/wine/wineserver"
[[ -e "$wine_root/usr/share/wine/wine" ]] || ln -s . "$wine_root/usr/share/wine/wine/wine"
mkdir -p /workspace/toolchains/wine-prefix/drive_c/windows/system32 /workspace/toolchains/fontcache
cp -asn "$wine_root/usr/lib/x86_64-linux-gnu/wine/x86_64-windows/." /workspace/toolchains/wine-prefix/drive_c/windows/system32/
cp "$wine_root/usr/x86_64-w64-mingw32/lib/zlib1.dll" /workspace/toolchains/wine-prefix/drive_c/windows/system32/zlib1.dll
/workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-g++-posix -std=c++17 -O2 -Wall -Wextra -Werror -Wno-cast-function-type -static src/msm2/lifecycle_smoke.cpp build/windows/guard.o -o build/windows/lifecycle_smoke.exe
/workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-g++-posix -std=c++17 -O2 -Wall -Wextra -Werror -Iinclude -static src/msm2/diagnostic_log_smoke.cpp build/windows/orientation_log.o -o build/windows/diagnostic_log_smoke.exe
cat > build/windows/WallSprint.ini <<'CONFIG'
[Diagnostics]
ORIENTATION_DIAGNOSTIC=1
ORIENTATION_INTERVAL_MS=100
REFERENCE_AOB_AUDIT=1
CONFIG
cd build/windows
python - <<'PRECHECK'
from pathlib import Path
import json,hashlib
old=Path('logs/WallSprint.log')
Path('logger_before.json').write_text(json.dumps({'files':[p.name for p in Path('log_wallsprint').glob('*.log')], 'shared_hash':hashlib.sha256(old.read_bytes()).hexdigest() if old.exists() else None}))
PRECHECK
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH="$wine_root/usr/lib/x86_64-linux-gnu/wine" WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' "$wine_root/usr/lib/wine/wine64" "$root/build/windows/lifecycle_smoke.exe" | tee "$root/logs/windows_lifecycle_smoke.log"
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH="$wine_root/usr/lib/x86_64-linux-gnu/wine" WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' "$wine_root/usr/lib/wine/wine64" "$root/build/windows/diagnostic_log_smoke.exe" | tee "$root/logs/windows_diagnostic_formatter.log"
python - <<'PY'
from pathlib import Path
import json,hashlib,re
before=json.loads(Path('logger_before.json').read_text())
files=[p for p in Path('log_wallsprint').glob('WallSprint_*.log') if p.name not in before['files']]
assert files and all(re.fullmatch(r'WallSprint_\d{8}-\d{6}(?:_\d{2,4})?\.log',p.name) for p in files)
log=max(files,key=lambda p:p.stat().st_mtime_ns).read_text()
assert 'filename_clock=host_local line_clock=UTC shared_logs_fallback=0' in log
old=Path('logs/WallSprint.log')
assert (hashlib.sha256(old.read_bytes()).hexdigest() if old.exists() else None)==before['shared_hash']
assert 'HeroSystem matches=0 GetActor matches=0' in log
assert 'target_build_gate=REJECTED' in log
assert 'ORIENTATION_INTERVAL' in log
assert 'marker_keys=1,2,3,4,5,6 modifier=NONE' in log
assert 'snapshot=unavailable' in log
assert 'no game API calls/hooks' in log
assert 'wallrun_hooks=disabled reason=build_gate_rejected' in log
import re
version_header=Path('../../include/version.hpp').read_text()
label=re.search(r'versionLabel\[\]="([^"]+)"',version_header)[1]
package_version=re.search(r'packageVersion\[\]="([^"]+)"',version_header)[1]
assert f'WallSprint {label} package_version={package_version}' in log
assert 'RESET_TRACE_SELF_CHECK prerequisite_gates_verified=0 common_writer_hook=0 reset_trace_enabled=1 reset_trace_admitted=0 primary_thread_prearm_supported=0' in log
assert 'ORIENTATION_ADMISSION exe_build_verified=0 path_a_writer_verified=0 path_b_writer_verified=0 path_b_caller_verified=0 path_b_map_verified=0 dual_source_admitted=0' in log
assert 'V01B7_DIAGNOSTIC_NOT_ADMITTED' in log
assert 'reason=TelemetryStopped' in log
assert 'orientation_trace_config enabled=0 admitted=0' in log
assert 'orientation_trace_arm ' not in log and 'ORIENTATION_WRITER_EVENT' not in log
assert 'orientation_source_config requested=1 admitted_build=0' in log
assert 'orientation_source_hook installed=1' not in log and 'ORIENTATION_WRITER_ENTRY' not in log
assert 'exe_file_sha256=unavailable' not in log
print('PASS: synthetic-host log confirms zero-match/build-gate refusal, executable hashing, 100ms diagnostic intervals, clean stop')
PY

cd "$root"
/workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-g++-posix -std=c++17 -O2 -Wall -Wextra -Werror -Iinclude -static src/msm2/object_read_smoke.cpp -o build/windows/object_read_smoke.exe
WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH="$wine_root/usr/lib/x86_64-linux-gnu/wine" WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' "$wine_root/usr/lib/wine/wine64" "$root/build/windows/object_read_smoke.exe" | tee logs/windows_object_read_smoke.log

bash tools/test_log_file.sh
