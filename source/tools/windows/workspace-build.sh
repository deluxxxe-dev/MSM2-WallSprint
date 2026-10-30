#!/usr/bin/env bash
# Full existing cross-build/regressions, staged OUTSIDE the scanned Git clone.
set -euo pipefail
repo=$1
result=$2
# The steady-state updater must not inherit a legacy-drive toolchain/capture.
export WALLSPRINT_TOOLCHAIN_ROOT=/workspace/toolchains/windows
export WALLSPRINT_DECODER_CACHE=/workspace/toolchains/decoder-cache
unset WALLSPRINT_PATH_B_CAPTURE
command -v python >/dev/null || { echo 'WS ERROR: Python is required; no validation is skipped' >&2; exit 1; }
python - "$repo" <<'PY'
from pathlib import Path
import sys
source=Path(sys.argv[1]).resolve()
if any(p.name.casefold()=='mods library' for p in [source,*source.parents]):
 if any(p.is_file() and p.suffix.lower() in {'.script','.stage','.zip','.rar','.7z'} for p in source.rglob('*')):
  raise SystemExit('WS ERROR: source tree contains scanner-visible scripts/archives; update the preparation checkout and rerun its latest Migrate-WS.cmd before building')
PY
export MSM2_WS_STATUS_DIR="$result"
status_helper="$repo/source/tools/windows/build-status.py"
build_stage=preflight
stage=''
progress(){ if [[ -n "${MSM2_WS_OPERATION_TOKEN:-}" ]]; then python3 "$status_helper" "$result" "$build_stage" "$1" "${2:-}"; fi; }
finish(){
 local rc=$?
 trap - EXIT INT TERM
 progress finished "$rc" || { if [[ $rc == 0 ]]; then rc=74; fi; }
 if [[ -n "$stage" ]]; then rm -rf -- "$stage"; fi
 exit "$rc"
}
trap finish EXIT
trap 'exit 130' INT
trap 'exit 143' TERM
progress running
for tool in bash python g++ make objdump tar curl sha256sum; do
 command -v "$tool" >/dev/null || { echo "WS ERROR: missing WSL prerequisite $tool; see source/BUILD.md" >&2; exit 1; }
done
for tool in /workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-g++-posix /workspace/toolchains/windows/usr/lib/llvm-19/bin/clang /workspace/toolchains/analysis-python/bin/python /workspace/toolchains/wine/usr/lib/wine/wine64; do
 [[ -x "$tool" ]] || { echo "WS ERROR: missing configured WSL toolchain $tool; no validation is skipped" >&2; exit 1; }
done
stage=$(mktemp -d /tmp/msm2-ws-validation.XXXXXX)

mkdir -p "$stage/repository"
# Copy sources/docs only. No legacy checkout/gitdir/reference/cache dependency.
tar -C "$repo" --exclude=source/build --exclude=source/logs --exclude=source/output --exclude=__pycache__ -cf - AGENTS.md docs analysis source environment README.md README_TEST.md Update-WS.cmd Migrate-WS.cmd .gitattributes | tar -C "$stage/repository" -xf -
cd "$stage/repository"
build_stage=build; progress running
bash source/tools/build_windows.sh
build_stage=portable; progress running
bash source/tools/test.sh
build_stage=smoke; progress running
bash source/tools/smoke_windows.sh
build_stage=hooks; progress running
bash source/tools/test_hooks_windows.sh
build_stage=package; progress running
python source/tools/release.py
build_stage=stage_payload; progress running
python - "$result" <<'PY'
from pathlib import Path
import json,shutil,sys,zipfile
root=Path.cwd();result=Path(sys.argv[1]);result.mkdir(parents=True,exist_ok=True)
m=json.loads((root/'BUILD_MANIFEST.json').read_text());label=m['version']
name='MSM2-WS-'+label+'.script.disabled'
payload=root/name
assert payload.is_file() and not list((root/'source').rglob('*.script'))
with zipfile.ZipFile(payload) as z:assert z.testzip() is None
shutil.copy2(payload,result/name);shutil.copy2(root/'BUILD_MANIFEST.json',result/'BUILD_MANIFEST.json')
for path in root.glob('MSM2-WS-*.zip'):
 # Overstrike also discovers scripts inside archives without extracting them.
 shutil.copy2(path,result/(path.name+'.disabled'))
assert not any(p.suffix.lower() in {'.zip','.rar','.7z','.script'} for p in result.rglob('*') if p.is_file())
(result/'Build-WS-result.json').write_text(json.dumps({'artifact':name,'version':label},indent=2)+'\n')
print('PASS: required full WSL build/test/package completed; disabled artifact staged for transactional WS-only deployment')
PY
