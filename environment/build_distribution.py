"""Build only the independently authored toolkit, never a user's backup/mods."""
from pathlib import Path
import hashlib
import json
import subprocess
import zipfile

from mod_environment import KIT, VERSION

root = Path(__file__).resolve().parent
repo = root.parent
files = list(KIT) + ['test_mod_environment.py', 'test_windows_contract.ps1', 'build_distribution.py', 'TEST_REPORT.md']
output = repo / f'MSM2-Mod-Tools-v{VERSION}.zip'
pending = output.with_suffix('.pending')
prefix = f'MSM2-Mod-Tools-v{VERSION}'
with zipfile.ZipFile(pending, 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as z:
    for name in sorted(files):
        p = root / name
        assert p.is_file()
        assert p.suffix not in {'.exe', '.dll', '.script', '.log'}
        info = zipfile.ZipInfo(prefix + '/' + name, (2026, 10, 24, 0, 0, 0))
        info.compress_type = zipfile.ZIP_DEFLATED
        z.writestr(info, p.read_bytes())
with zipfile.ZipFile(pending) as z:
    assert z.testzip() is None
    assert set(z.namelist()) == {prefix + '/' + name for name in files}
    for name in files:
        assert z.read(prefix + '/' + name) == (root / name).read_bytes()
pending.replace(output)
try:
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=repo, text=True, stderr=subprocess.DEVNULL).strip()
except (OSError, subprocess.CalledProcessError):
    commit = 'standalone-source-archive'
manifest = {
    'tool_version': VERSION,
    'source_commit': commit,
    'artifact': output.name,
    'bytes': output.stat().st_size,
    'sha256': hashlib.sha256(output.read_bytes()).hexdigest(),
    'payloads': {name: hashlib.sha256((root / name).read_bytes()).hexdigest() for name in files},
    'contains_user_backups_mods_game_bodies_or_external_binaries': False,
    'gameplay_runtime_changed': False,
    'Windows_actual_shortcuts_launch_and_restore': 'PENDING_user_owned_PC',
}
(repo / 'analysis').mkdir(exist_ok=True)
(repo / 'analysis/MOD_TOOLS_RELEASE.json').write_text(json.dumps(manifest, indent=2) + '\n')
print(json.dumps({key: manifest[key] for key in ['tool_version', 'artifact', 'bytes', 'sha256']}, indent=2))
