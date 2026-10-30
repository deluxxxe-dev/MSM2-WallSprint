"""Private local MSM2 mod-environment snapshots; no game data or uploads.

Independent shared-environment utility, explicitly invoked by the owner.
It does not call either project's updater or change project source code.
"""
from __future__ import annotations

import argparse
import contextlib
import datetime as dt
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import stat
import subprocess
import sys
import tempfile
import uuid
import zipfile
import operation_lock

VERSION = '1.2.0'
FORMAT = 'msm2-mod-environment-1'
TREES = ('Overstrike', 'Profiles', 'Mods Library', 'scripts', 'Mod-Tools')
FILES = ('scripts_proxy.dll', 'winmm.dll', 'scripts.txt', 'commandline.txt')
ROOTS = TREES + FILES
KIT = ('Repair-Mod-Tools.ps1', 'repair_tools.py', 'operation_lock.py', 'mod_environment.py', 'windows.ps1', 'Install-Mod-Tools.cmd',
       'Play-Modded.cmd', 'Open-Overstrike.cmd', 'Start-Overstrike.cmd', 'Backup-Mods.cmd',
       'Restore-Mods.cmd', 'Verify-Backup.cmd', 'README.md', 'LICENSE.txt')
BUFFER = 1024 * 1024
MAX_MANIFEST = 64 * 1024 * 1024
MAX_FILES = 500_000
MAX_BYTES = 1024 ** 4
RESERVED = re.compile(r'^(?:CON|PRN|AUX|NUL|COM[1-9¹²³]|LPT[1-9¹²³])(?:\.|$)', re.I)


class EnvironmentError(RuntimeError):
    pass


def require(ok, message):
    if not ok:
        raise EnvironmentError(message)


def io_path(path):
    # Extended local/UNC paths keep deep _source/.git trees usable on Windows.
    text = os.path.abspath(path)
    if os.name == 'nt' and not text.startswith('\\\\?\\'):
        text = ('\\\\?\\UNC\\' + text[2:]) if text.startswith('\\\\') else '\\\\?\\' + text
    return Path(text)


def sha(path):
    h = hashlib.sha256()
    with io_path(path).open('rb') as stream:
        for data in iter(lambda: stream.read(BUFFER), b''):
            h.update(data)
    return h.hexdigest()


def linked(path):
    s = io_path(path).lstat()
    return stat.S_ISLNK(s.st_mode) or bool(getattr(s, 'st_file_attributes', 0) & 0x400)


def ancestors_safe(path):
    for p in (path, *path.parents):
        if p.exists() or p.is_symlink():
            require(not linked(p), f'Symlink/junction/reparse path refused: {p}')


def safe_relative(name):
    require(isinstance(name, str) and name and len(name) <= 30000, 'Invalid relative path')
    require('\\' not in name and ':' not in name, f'Unsafe path: {name}')
    parts = name.split('/')
    for part in parts:
        require(part not in ('', '.', '..') and not part.endswith((' ', '.')),
                f'Unsafe path component: {name}')
        require(not any(ord(c) < 32 for c in part) and not RESERVED.match(part),
                f'Windows-reserved path: {name}')
    require(parts[0] in ROOTS, f'Not a mod-environment root: {name}')
    require(parts[0] not in FILES or len(parts) == 1, f'File root used as directory: {name}')
    return name


def collision_check(names):
    spelling = {}
    for name in names:
        parts = safe_relative(name).split('/')
        for i in range(1, len(parts) + 1):
            prefix = '/'.join(parts[:i])
            key = prefix.casefold()
            require(key not in spelling or spelling[key] == prefix,
                    f'Case-alias paths cannot be restored on Windows: {name}')
            spelling[key] = prefix


def remove_owned_tree(path):
    # Only pending/staging trees created by this operation are ever deleted.
    def writable(function, name, error):
        os.chmod(name, stat.S_IREAD | stat.S_IWRITE | stat.S_IEXEC)
        function(name)
    shutil.rmtree(io_path(path), onerror=writable)


def game_root(path):
    p = Path(os.path.abspath(path))
    ancestors_safe(p)
    require(p.is_dir() and (p / 'Spider-Man2.exe').is_file(),
            f'Select the game folder containing Spider-Man2.exe: {p}')
    require(not linked(p / 'Spider-Man2.exe'), 'Game executable is a link')
    return p.resolve()


def scan(game):
    """Read all selected files, including hidden Git data and empty directories."""
    entries, dirs = [], []
    for name in ROOTS:
        root = game / name
        if not root.exists() and not root.is_symlink():
            continue
        require(not linked(root), f'Linked backup root refused: {root}')
        require(root.is_dir() if name in TREES else root.is_file(), f'Wrong root type: {root}')
        items = [root]
        if root.is_dir():
            for parent, subdirs, files in os.walk(io_path(root), followlinks=False):
                for child in subdirs + files:
                    p = Path(parent) / child
                    require(not linked(p), f'Linked backup entry refused: {p}')
                    items.append(p)
        for p in items:
            rel = safe_relative(Path(os.path.relpath(io_path(p), io_path(game))).as_posix())
            if p.is_dir():
                dirs.append(rel)
            else:
                s = p.stat()
                require(stat.S_ISREG(s.st_mode), f'Not a regular file: {p}')
                if p.name == 'Update-WS.lock' and operation_lock.owned_artifact(p,game):
                    continue
                require(p.name not in ('index.lock', 'Update-WS.lock'), f'An update appears active: {p}')
                entries.append({'path': rel, 'size': s.st_size, 'mtime_ns': s.st_mtime_ns,
                                'mode': stat.S_IMODE(s.st_mode)})
    collision_check(dirs + [e['path'] for e in entries])
    return sorted(entries, key=lambda e: e['path']), sorted(dirs)


def project_storage(game):
    for project in ('MSM2-SF', 'MSM2-WS'):
        source = game / 'Mods Library' / 'Mods-Developing' / project / '_source'
        if source.exists():
            gd = source / '.git'
            ancestors_safe(gd)
            require(gd.is_dir() and not linked(gd), f'{project}: independent .git folder required')
            require(not (gd / 'commondir').exists() and not (gd / 'objects/info/alternates').exists(),
                    f'{project}: external Git storage cannot be snapshotted')
            config = gd / 'config'
            if config.exists():
                require(not re.search(r'(?im)^\s*worktree\s*=', config.read_text(encoding='utf-8-sig')), f'{project}: external core.worktree refused')


def environment_ready(game):
    for name in ('Overstrike/Overstrike.exe', 'scripts_proxy.dll', 'winmm.dll'):
        require((game / name).is_file(), f'Missing environment file: {name}')
        ancestors_safe(game / name)
    require((game / 'Mods Library').is_dir(), 'Mods Library is missing')
    misplaced = game / 'Overstrike' / 'winmm.dll'
    require(not os.path.lexists(misplaced), 'Remove the misplaced Overstrike/winmm.dll manually')
    proxy = game / 'Overstrike' / 'scripts_proxy.dll'
    ancestors_safe(proxy)
    require(not os.path.lexists(proxy) or (not linked(proxy) and proxy.is_file()), 'Overstrike/scripts_proxy.dll must be a regular non-reparse installer source')
    project_storage(game)


def inside(path, parent):
    try:
        path.relative_to(parent)
        return True
    except ValueError:
        return False


def external_folder(path, game):
    p = Path(os.path.abspath(path))
    ancestors_safe(p)
    require(not inside(p, game) and not inside(game, p),
            'Backup folder must be outside the game folder, and not its ancestor')
    return p


class Windows:
    def invoke(self, action, game, **kwargs):
        require(os.name == 'nt', 'Windows is required for launch/process/shortcut actions')
        request = {'action': action, 'game': str(game), **kwargs}
        with tempfile.TemporaryDirectory(prefix='msm2-mod-tools-') as tmp:
            cfg = Path(tmp) / 'request.json'
            cfg.write_text(json.dumps(request, ensure_ascii=False), encoding='utf-8')
            p = subprocess.run(['powershell.exe', '-NoLogo', '-NoProfile', '-NonInteractive',
                                '-ExecutionPolicy', 'RemoteSigned', '-File', str(Path(__file__).with_name('windows.ps1')),
                                '-Request', str(cfg)], capture_output=True, **operation_lock.child_options())
            require(p.returncode == 0, (p.stderr or p.stdout).decode('utf-8', errors='replace').strip())

    def quiet(self, game):
        self.invoke('check', game)

    def shortcuts(self, game):
        self.invoke('shortcuts', game)

    def launch(self, game, manager=False):
        if manager:
            self.invoke('manager', game)
        else:
            self.invoke('play', game)


def lock(game,action="shared-tools"):
    @contextlib.contextmanager
    def held():
        try:
            with operation_lock.hold(game,action) as h: yield h
        except operation_lock.OperationError as ex: raise EnvironmentError(str(ex)) from ex
    return held()


def unique(parent, stem):
    for n in range(10000):
        p = parent / (stem + (f'-{n:02d}' if n else ''))
        if not p.exists():
            return p
    raise EnvironmentError('Too many backup name collisions')


def validate_manifest(z):
    infos = z.infolist()
    require(len(infos) <= MAX_FILES + 1, 'Archive contains too many entries')
    require(len({i.filename for i in infos}) == len(infos), 'Duplicate archive entries')
    require('manifest.json' in z.namelist(), 'Backup manifest is missing')
    info = z.getinfo('manifest.json')
    require(info.file_size <= MAX_MANIFEST, 'Manifest is too large')
    m = json.loads(z.read(info))
    require(isinstance(m, dict), 'Manifest must be an object')
    require(m.get('format') == FORMAT and isinstance(m.get('tool_version'), str), 'Unsupported backup format/version')
    require(isinstance(m.get('game_sha256'), str) and re.fullmatch('[0-9a-f]{64}', m['game_sha256']), 'Invalid game fingerprint')
    require(isinstance(m.get('original_game_root'), str) and m['original_game_root'], 'Original game root is missing')
    require(isinstance(m.get('files'), list) and isinstance(m.get('directories'), list), 'Invalid inventory')
    require(m.get('roots') == list(ROOTS), 'Unexpected restore roots')
    require(all(isinstance(e, dict) for e in m['files']), 'Invalid file record')
    names = [safe_relative(e['path']) for e in m['files']]
    dirs = [safe_relative(d) for d in m['directories']]
    require(not set(names) & set(TREES) and not set(dirs) & set(FILES), 'Invalid root entry type')
    require(len(dirs) <= MAX_FILES, 'Too many directories')
    dir_set = set(dirs)
    require(len(names) == len(set(names)) and len(dirs) == len(set(dirs)), 'Duplicate manifest path')
    require(not set(names) & set(dirs), 'File/directory collision')
    collision_check(names + dirs)
    require(set(z.namelist()) == {'manifest.json', *('payload/' + n for n in names)}, 'Archive inventory mismatch')
    total = 0
    for e in m['files']:
        require(type(e.get('size')) is int and e['size'] >= 0, 'Invalid file size')
        require(type(e.get('mtime_ns')) is int and e['mtime_ns'] >= 0, 'Invalid file time')
        require(type(e.get('mode')) is int and 0 <= e['mode'] <= 0o777, 'Invalid mode')
        require(isinstance(e.get('sha256'), str) and re.fullmatch('[0-9a-f]{64}', e['sha256']), 'Invalid file hash')
        zi = z.getinfo('payload/' + e['path'])
        mode = zi.external_attr >> 16
        require(not zi.is_dir() and not stat.S_ISLNK(mode) and not (zi.external_attr & 0x400), 'Linked archive entry')
        require(zi.file_size == e['size'] and not zi.flag_bits & 1, 'Invalid size/encrypted entry')
        total += e['size']
    require(total <= MAX_BYTES and len(names) <= MAX_FILES, 'Backup exceeds supported limits')
    # Parent paths must be directories, never files or missing inventory entries.
    for name in names + dirs:
        for p in PurePosixPath(name).parents:
            if str(p) != '.':
                require(str(p) in dir_set, 'Missing parent directory in manifest')
    return m


def verify(archive, destination=None):
    """Validate every byte before application; never use ZipFile.extractall."""
    archive = Path(archive)
    ancestors_safe(archive)
    with zipfile.ZipFile(archive) as z:
        m = validate_manifest(z)
        if destination:
            for name in m['directories']:
                io_path(destination / name).mkdir(parents=True, exist_ok=True)
        for e in m['files']:
            h, count = hashlib.sha256(), 0
            out = None
            try:
                if destination:
                    out = io_path(destination / e['path']).open('xb')
                with z.open('payload/' + e['path']) as src:
                    for data in iter(lambda: src.read(BUFFER), b''):
                        h.update(data)
                        count += len(data)
                        require(count <= e['size'], 'Expanded data exceeds declared size')
                        if out:
                            out.write(data)
                require(count == e['size'] and h.hexdigest() == e['sha256'], f'Backup data corrupted: {e["path"]}')
            finally:
                if out:
                    out.close()
            if destination:
                p = io_path(destination / e['path'])
                os.utime(p, ns=(e['mtime_ns'], e['mtime_ns']))
                p.chmod(e['mode'])
    return m


def backup(game, output, system=None):
    system = system or Windows()
    game = game_root(game)
    output = external_folder(output, game)
    with lock(game):
        system.quiet(game)
        environment_ready(game)
        require(load_settings(game).get('format') == 'msm2-mod-tools-1', 'Run Install-Mod-Tools first')
        before, dirs = scan(game)
        output.mkdir(parents=True, exist_ok=True)
        total = sum(e['size'] for e in before)
        require(shutil.disk_usage(output).free > total + 64 * BUFFER, 'Insufficient free space for backup')
        stamp = dt.datetime.now().strftime('%Y%m%d-%H%M%S')
        final = unique(output, 'MSM2-Mod-Environment-' + stamp)
        pending = output / ('.pending-' + uuid.uuid4().hex)
        pending.mkdir()
        try:
            m = {'format': FORMAT, 'tool_version': VERSION, 'created_local': dt.datetime.now().isoformat(),
                 'original_game_root': str(game), 'game_sha256': sha(game / 'Spider-Man2.exe'),
                 'roots': list(ROOTS), 'directories': dirs, 'files': []}
            archive = pending / 'environment.zip'
            with zipfile.ZipFile(archive, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=1,
                                 allowZip64=True) as z:
                for index, e in enumerate(before):
                    h = hashlib.sha256()
                    with io_path(game / e['path']).open('rb') as src, z.open('payload/' + e['path'], 'w', force_zip64=True) as dst:
                        for data in iter(lambda: src.read(BUFFER), b''):
                            h.update(data)
                            dst.write(data)
                    m['files'].append({**e, 'sha256': h.hexdigest()})
                    if index % 200 == 0:
                        print(f'Backup {index + 1}/{len(before)}: {e["path"]}', flush=True)
                z.writestr('manifest.json', json.dumps(m, ensure_ascii=False, indent=2))
            after, after_dirs = scan(game)
            require(before == after and dirs == after_dirs, 'Source changed during backup; close editors/updaters and retry')
            verify(archive)
            require(sha(game / 'Spider-Man2.exe') == m['game_sha256'], 'Game executable changed during backup')
            # Include a local, independent recovery engine beside the private archive.
            for name in KIT:
                src = Path(__file__).with_name(name)
                require(src.is_file(), f'Toolkit is incomplete: {name}')
                shutil.copy2(src, pending / name)
            system.quiet(game)
            require(scan(game) == (before, dirs), 'Source changed before backup publication; retry when idle')
            require(sha(game / 'Spider-Man2.exe') == m['game_sha256'], 'Game changed before backup publication')
            pending.rename(final)
            return final
        except BaseException:
            remove_owned_tree(pending)
            raise


def read_manifest(archive):
    ancestors_safe(Path(archive))
    with zipfile.ZipFile(archive) as z:
        return validate_manifest(z)


def plan_restore(archive, game):
    game = game_root(game)
    m = read_manifest(archive)
    # Same-path recovery preserves all opaque third-party paths, Git/local config.
    require(os.path.normcase(str(game)) == os.path.normcase(m['original_game_root']),
            'Restore to the original game path; relocation of opaque mod/profile paths is not supported')
    require(sha(game / 'Spider-Man2.exe') == m['game_sha256'], 'Game executable differs from the backup; restore the matching Steam build first')
    return m, game


def restore(archive, game, system=None, apply=False):
    system = system or Windows()
    m, game = plan_restore(archive, game)
    if not apply:
        require(verify(archive) == m, 'Archive changed during validation')
        return {'game': str(game), 'files': len(m['files']),
                'bytes': sum(e['size'] for e in m['files']), 'roots': list(ROOTS)}
    require(not inside(Path(archive).resolve(), game), 'Recovery archive must be outside the game folder')
    with lock(game,"restore"):
        system.quiet(game)
        # Validate existing trees before moving anything; reject links, preserve data.
        existing = scan(game)
        frozen_kit={name:Path(__file__).with_name(name).read_bytes() for name in KIT}
        needed = sum(e['size'] for e in m['files'])
        require(shutil.disk_usage(game).free > needed + 64 * BUFFER, 'Insufficient free space for staged restore')
        stage = game / ('.MSM2-restore-' + uuid.uuid4().hex)
        rollback = game.parent / ('.MSM2-rollback-' + uuid.uuid4().hex)
        ancestors_safe(rollback)
        stage.mkdir()
        try:
            rollback.mkdir()
        except BaseException:
            stage.rmdir()
            raise
        moved, installed = [], []
        try:
            require(verify(archive, stage) == m, 'Archive changed since validation')
            environment_ready(stage)
            system.quiet(game)
            require(scan(game) == existing, 'Existing environment changed during staging; retry when idle')
            require(sha(game / 'Spider-Man2.exe') == m['game_sha256'], 'Game executable changed during staging')
            # Keep the current locking protocol even when restoring an older toolkit.
            kit=stage/'Mod-Tools';kit.mkdir(exist_ok=True)
            for name,data in frozen_kit.items(): (kit/name).write_bytes(data)
            compat=stage/'Mods Library/Mods-Developing/MSM2-WS/_archive/Update-WS.lock'
            if compat.parent.is_dir():compat.write_text(json.dumps({**operation_lock._current.record,'state':'released'})+'\n')
            for name in ROOTS:
                old, new = game / name, stage / name
                if old.exists():
                    old.rename(rollback / name)
                    moved.append(name)
                if new.exists():
                    new.rename(old)
                    installed.append(name)
            # A restore on vanilla still needs Overstrike's asset/mod installation.
            settings = load_settings(game)
            settings['restore_requires_install'] = True
            save_settings(game, settings)
            environment_ready(game)
        except BaseException:
            try:
                for name in reversed(installed):
                    (game / name).rename(stage / name)
                for name in reversed(moved):
                    (rollback / name).rename(game / name)
            except BaseException as rollback_error:
                raise EnvironmentError(f'Restore failed; rollback incomplete. Preserve {stage} and {rollback}: {rollback_error}') from rollback_error
            remove_owned_tree(stage)
            rollback.rmdir()
            raise
        remove_owned_tree(stage)
        return rollback


def load_settings(game):
    p = game / 'Mod-Tools/mod-tools.json'
    return json.loads(p.read_text(encoding='utf-8')) if p.exists() else {}


def save_settings(game, value):
    p = game / 'Mod-Tools/mod-tools.json'
    p.parent.mkdir(parents=True, exist_ok=True)
    tmp = p.with_suffix('.pending')
    tmp.write_text(json.dumps(value, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    os.replace(tmp, p)


def profile(game, settings=None):
    matches = []
    for p in (game / 'Profiles').glob('*.json'):
        data = json.loads(p.read_text(encoding='utf-8-sig'))
        if data.get('game') == 'MSM2' and os.path.normcase(str(Path(data.get('path', '')).resolve())) == os.path.normcase(str(game)):
            matches.append((p.stem, data))
    require(matches, 'No MSM2 Overstrike profile targets this game root')
    chosen = (load_settings(game) if settings is None else settings).get('profile')
    if not chosen and (game / 'Profiles/Settings.json').is_file():
        preferred = json.loads((game / 'Profiles/Settings.json').read_text(encoding='utf-8-sig')).get('profile')
        if any(name == preferred for name, _ in matches):
            chosen = preferred
    if chosen:
        matches = [m for m in matches if m[0] == chosen]
    require(len(matches) == 1, 'Choose a profile with install --profile NAME. Matches: ' + ', '.join(m[0] for m in matches))
    return matches[0]


def refresh_kit(game, system=None, initial=False):
    """Stage current owned code before legacy checker use; preserve existing settings values."""
    require(operation_lock._current is not None and operation_lock._current.record['game'] == str(game), 'Toolkit refresh requires shared OS ownership')
    system = system or Windows()
    target = game / 'Mod-Tools'
    settings = load_settings(game)
    if not initial and settings.get('format') != 'msm2-mod-tools-1':
        require(not target.exists() or not any(target.iterdir()), 'Unrecognized Mod-Tools folder preserved')
        return _install(game, system=system)
    ancestors_safe(target)
    target.mkdir(exist_ok=True)
    changed = []
    for name in KIT:
        src, dst = Path(__file__).with_name(name), target / name
        ancestors_safe(dst)
        require(src.is_file() and not linked(src), 'Incomplete/linked toolkit source: ' + str(src))
        require(not os.path.lexists(dst) or (dst.is_file() and not linked(dst)), 'Unsafe toolkit destination: ' + str(dst))
        if src.resolve() != dst.resolve() and (not dst.exists() or sha(src) != sha(dst)):
            changed.append((src,dst))
    metadata_change = settings.get('tool_version') != VERSION
    if not changed and not metadata_change:
        return False
    if not initial: system.quiet(game)
    saved = []
    with tempfile.TemporaryDirectory(prefix='.MSM2-kit-stage-', dir=game) as temp:
        stage = Path(temp)
        for src,dst in changed:
            shutil.copy2(src, stage / dst.name)
            require(sha(stage/dst.name) == sha(src), 'Toolkit staging hash mismatch')
        previous = target / '_toolkit_previous' / uuid.uuid4().hex
        previous.mkdir(parents=True)
        try:
            for src,dst in changed:
                old = previous / dst.name if dst.exists() else None
                if old: shutil.copy2(dst, old)
                digest = sha(stage/dst.name)
                os.replace(stage/dst.name, dst)
                saved.append((dst,old,digest))
        except BaseException:
            for dst,old,digest in reversed(saved):
                require(dst.is_file() and sha(dst)==digest, 'Toolkit rollback conflict; previous code preserved in ' + str(previous))
                if old: shutil.copy2(old,dst)
                else: dst.unlink()
            raise
    if not initial:
        if metadata_change:
            settings['tool_version']=VERSION
            save_settings(game,settings)
        system.shortcuts(game)
    return True


def repair_locked(game, system=None):
    require(operation_lock._current is not None, 'Repair requires OS ownership')
    game = game_root(game)
    system = system or Windows()
    system.quiet(game)
    # Current checker, never the installed older failing checker.
    environment_ready(game)
    refresh_kit(game,system)
    ws_launch_ready(game)
    ws = game / 'Mods Library/Mods-Developing/MSM2-WS'
    helper = ws / '_source/source/tools/windows/Update-WS.cmd'
    ancestors_safe(helper);ancestors_safe(ws/'Update-WS.cmd')
    require(helper.is_file(), 'Current Update-WS.cmd source missing')
    pending = ws / ('Update-WS-' + uuid.uuid4().hex + '.cmd.pending')
    try:
        with pending.open('xb') as f: f.write(helper.read_bytes())
        os.replace(pending,ws/'Update-WS.cmd')
    finally:
        if pending.exists(): pending.unlink()
    system.shortcuts(game)
    print('TOOLING_REPAIR_COMPLETE runtime_unchanged=1 no_build=1 no_game_launch=1')


def install(game, backup_dir=None, selected=None, system=None):
    game = game_root(game)
    with lock(game):
        return _install(game, backup_dir, selected, system)


def _install(game, backup_dir=None, selected=None, system=None):
    game = game_root(game)
    environment_ready(game)
    system = system or Windows()
    system.quiet(game)
    scan(game)
    settings = load_settings(game)
    target = game / 'Mod-Tools'
    require(not target.exists() or not any(target.iterdir()) or settings.get('format') == 'msm2-mod-tools-1', 'Unrecognized Mod-Tools folder; existing files preserved')
    settings.update(format='msm2-mod-tools-1', tool_version=VERSION)
    if backup_dir:
        settings['backup_directory'] = str(external_folder(backup_dir, game))
    else:
        settings.setdefault('backup_directory', str(Path.home() / 'MSM2-Mod-Backups'))
    if selected:
        settings['profile'] = selected
    name, _ = profile(game, settings)
    settings['profile'] = name
    target = game / 'Mod-Tools'
    target.mkdir(exist_ok=True)
    refresh_kit(game,system,initial=True)
    save_settings(game, settings)
    system.shortcuts(game)
    return target


def play(game, system=None, confirm_install=False):
    game = game_root(game)
    with lock(game):
        refresh_kit(game,system)
        return _play(game, system, confirm_install)


def open_manager(game, system=None):
    game = game_root(game)
    with lock(game):
        refresh_kit(game,system)
        environment_ready(game)
        (system or Windows()).launch(game, manager=True)


def ws_launch_ready(game):
    """Check the accepted WS layout without building, changing Git or reading SF."""
    workspace = game / 'Mods Library/Mods-Developing/MSM2-WS'
    require((workspace / '_source/.git').is_dir(), 'WS independent _source clone is missing')
    ancestors_safe(workspace)
    require(not (workspace / '_source/.git/index.lock').exists(),
            'WS Git index lock is present; let the current Git operation finish first')
    def walk_error(error):
        raise EnvironmentError('Cannot inspect WS layout: ' + str(error)) from error
    scripts = []
    for parent, dirs, files in os.walk(workspace, followlinks=False, onerror=walk_error):
        folder = Path(parent)
        for name in dirs + files:
            item = folder / name
            require(not linked(item), f'Linked WS path refused: {item}')
        if folder == workspace / '_source' and '.git' in dirs:
            dirs.remove('.git')  # Git object storage is not a deployed MOD.
        for name in files:
            item = folder / name
            require(item.suffix.lower() not in ('.zip', '.rar', '.7z'),
                    f'Overstrike-visible WS archive found; use Update-WS to validate it: {item}')
            if item.suffix.lower() == '.script':
                scripts.append(item)
    require(len(scripts) == 1 and scripts[0].parent == workspace and
            re.fullmatch(r'MSM2-WS-v[0-9A-Za-z.]+\.script', scripts[0].name),
            'Expected exactly one versioned WS .script at workspace root; use Update-WS to validate the layout')
    return scripts[0]


def start_manager(game, system=None):
    """One-time toolkit/shortcut setup, then ordinary checked manager launch."""
    game = game_root(game)
    system = system or Windows()
    with lock(game):
        known = load_settings(game).get('format') == 'msm2-mod-tools-1'
        if known: refresh_kit(game,system)
        environment_ready(game)
        profile(game)
        script = ws_launch_ready(game)
        if not known: refresh_kit(game,system)
        print('PASS: shared Overstrike, game-root loaders and MSM2 profile')
        print('PASS: one active WS script:', script.name)
        system.launch(game, manager=True)
        print('Shared Overstrike opened. Use Install mods -> Run modded game as needed.')


def _play(game, system=None, confirm_install=False):
    game = game_root(game)
    environment_ready(game)
    _, data = profile(game)
    require(data.get('settings', {}).get('scripts', {}).get('enabled') is True,
            'Enable script support in the selected Overstrike profile first')
    require((game / 'scripts.txt').is_file() and (game / 'scripts').is_dir(),
            'Install mods in the shared Overstrike first')
    ancestors_safe(game / 'scripts')
    dlls = (game / 'scripts.txt').read_text(encoding='utf-8-sig').splitlines()
    require(any(line.strip() for line in dlls), 'scripts.txt is empty; run Install mods first')
    for line in dlls:
        dll = line.strip()
        if not dll:
            continue
        require('/' not in dll and '\\' not in dll and ':' not in dll and dll.lower().endswith('.dll'), 'Invalid installed script name')
        ancestors_safe(game / 'scripts' / dll)
        require((game / 'scripts' / dll).is_file(), 'Installed script DLL missing: ' + dll)
    settings = load_settings(game)
    if settings.get('restore_requires_install'):
        require(confirm_install, 'Restore finished: use Open-Overstrike, complete Install mods, then confirm with play --confirm-installed')
        settings['restore_requires_install'] = False
        save_settings(game, settings)
    (system or Windows()).launch(game)


def prompt_path(message, default=''):
    answer = input(message + (f' [{default}]' if default else '') + ': ').strip().strip('"')
    return answer or default


def main(argv=None):
    parser = argparse.ArgumentParser(description='Local shared MSM2 mod launch/backup/restore; game data excluded')
    parser.add_argument('action', choices=['install', 'play', 'manager', 'start', 'repair', 'backup', 'verify', 'restore'])
    parser.add_argument('--game-root')
    parser.add_argument('--backup-dir')
    parser.add_argument('--profile')
    parser.add_argument('--archive')
    parser.add_argument('--apply', action='store_true')
    parser.add_argument('--confirm-installed', action='store_true')
    a = parser.parse_args(argv)
    home = Path(__file__).resolve().parent
    installed_game = home.parent if home.name == 'Mod-Tools' else None
    archive = Path(a.archive) if a.archive else home / 'environment.zip'
    if a.action in ('restore', 'verify') and not archive.exists():
        archive = Path(prompt_path('Backup environment.zip file'))
    if a.action == 'verify':
        m = verify(archive)
        print(f'PASS: all {len(m["files"])} files verified; source game: {m["original_game_root"]}')
        return
    root = a.game_root or installed_game
    if a.action == 'start' and not root:
        root = next((p for p in home.parents if (p / 'Spider-Man2.exe').is_file()), None)
    if a.action == 'restore' and not root:
        m = read_manifest(archive)
        root = prompt_path('Restore game folder', m['original_game_root'])
    if not root:
        root = prompt_path('Game folder containing Spider-Man2.exe')
    game = game_root(root)
    if a.action == 'repair':
        with lock(game): repair_locked(game)
    elif a.action == 'install':
        destination = a.backup_dir or prompt_path('Backup folder outside the game', load_settings(game).get('backup_directory', str(Path.home() / 'MSM2-Mod-Backups')))
        print('Installed:', install(game, destination, a.profile))
    elif a.action == 'backup':
        destination = a.backup_dir or load_settings(game).get('backup_directory')
        if not destination:
            destination = prompt_path('Backup folder outside the game', str(Path.home() / 'MSM2-Mod-Backups'))
        print('VERIFIED BACKUP:', backup(game, destination))
    elif a.action == 'restore':
        plan = restore(archive, game)
        print(json.dumps(plan, ensure_ascii=False, indent=2))
        apply = a.apply or input('Restore these roots; keep the current roots in a rollback folder? Type RESTORE: ') == 'RESTORE'
        if apply:
            old = restore(archive, game, apply=True)
            print('FILES RESTORED. Previous environment preserved:', old)
            print('Next: complete Install mods in the shared Overstrike, then use Play-Modded and confirm INSTALLED once.')
            open_manager(game)
    elif a.action == 'manager':
        open_manager(game)
    elif a.action == 'start':
        start_manager(game)
    else:
        if load_settings(game).get('restore_requires_install') and not a.confirm_installed:
            print('After restoring onto Vanilla, first complete Install mods in shared Overstrike.')
            a.confirm_installed = input('Has Install mods completed successfully? Type INSTALLED: ') == 'INSTALLED'
        play(game, confirm_install=a.confirm_installed)


if __name__ == '__main__':
    try:
        main()
    except (EnvironmentError, OSError, ValueError, KeyError, TypeError, zipfile.BadZipFile, EOFError) as error:
        print('MOD ENVIRONMENT ERROR:', error, file=sys.stderr)
        sys.exit(1)
