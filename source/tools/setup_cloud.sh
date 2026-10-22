#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
# Isolated under /workspace: signed APT metadata, no system configuration edits.
mkdir -p /workspace/toolchains/apt/{lists/partial,cache/archives/partial,parts,sourceparts} /workspace/toolchains/{debs,windows,wine}
cat > /workspace/toolchains/apt/apt.conf <<'CONFIG'
Dir::Etc::parts "/workspace/toolchains/apt/parts";
Dir::Etc::main "/dev/null";
Dir::Etc::sourcelist "/workspace/toolchains/apt/sources.list";
Dir::Etc::sourceparts "/workspace/toolchains/apt/sourceparts";
Dir::State::lists "/workspace/toolchains/apt/lists";
Dir::Cache "/workspace/toolchains/apt/cache";
Dir::Cache::archives "/workspace/toolchains/apt/cache/archives";
APT::Get::List-Cleanup "false";
CONFIG
cat > /workspace/toolchains/apt/sources.list <<'SOURCES'
deb [signed-by=/usr/share/keyrings/debian-archive-keyring.gpg] https://deb.debian.org/debian trixie main
SOURCES
python - <<'PY'
import hashlib,json,os,pathlib,subprocess
packages=json.loads(pathlib.Path('tools/toolchain_packages.json').read_text())
want_wine=os.environ.get('WALLSPRINT_WITH_WINE')=='1'
selected=[p for p in packages if p['group']=='compiler' or want_wine]
env=dict(os.environ,APT_CONFIG='/workspace/toolchains/apt/apt.conf')
cache=pathlib.Path('/workspace/toolchains/debs')
missing=[]
for p in selected:
 found=any(hashlib.sha256(f.read_bytes()).hexdigest()==p['sha256'] for f in cache.glob('*.deb'))
 if not found:missing.append(p)
if missing:
 subprocess.run(['/usr/bin/apt-get','update'],env=env,check=True)
 subprocess.run(['/usr/bin/apt-get','download',*[p['name']+'='+p['version'] for p in missing]],env=env,cwd=cache,check=True)
# Match the recorded hashes as well as APT's signed-index checks.
for p in selected:
 matches=[f for f in cache.glob('*.deb') if hashlib.sha256(f.read_bytes()).hexdigest()==p['sha256']]
 if len(matches)!=1:raise RuntimeError('verified package missing/ambiguous: '+p['name'])
 dest='/workspace/toolchains/wine' if p['group']=='wine' else '/workspace/toolchains/windows'
 subprocess.run(['dpkg-deb','-x',str(matches[0]),dest],check=True)
PY
/workspace/toolchains/windows/usr/bin/x86_64-w64-mingw32-g++-posix --version
LD_LIBRARY_PATH=/workspace/toolchains/windows/usr/lib/x86_64-linux-gnu /workspace/toolchains/windows/usr/lib/llvm-19/bin/clang --version
