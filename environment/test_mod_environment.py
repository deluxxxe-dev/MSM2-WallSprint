"""Owned temporary files/Git repositories only. Never touches a real game."""
from pathlib import Path
import copy
import importlib.util
import io
import json
import os
import shutil
import stat
import subprocess
import tempfile
import unittest
from unittest.mock import patch
import zipfile

import mod_environment as m


class System:
    def __init__(self):
        self.events = []
        self.busy = False

    def quiet(self, game):
        self.events.append(('quiet', game))
        if self.busy:
            raise m.EnvironmentError('owned running-process refusal')

    def shortcuts(self, game):
        self.events.append(('shortcuts', game))

    def launch(self, game, manager=False):
        self.events.append(('launch', game, manager))


def git(root, *args):
    return subprocess.check_output(['git', '-C', str(root), *args], stderr=subprocess.PIPE).decode().strip()


def tree(root):
    return {str(p.relative_to(root)): p.read_bytes() for p in root.rglob('*') if p.is_file()}


class EnvironmentTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(prefix='MSM2 owned 日本語 ')
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.game = self.root / "Game's Steam テ"
        self.game.mkdir()
        (self.game / 'Spider-Man2.exe').write_bytes(b'owned game fixture; never executed')
        (self.game / 'asset_archive').mkdir()
        (self.game / 'asset_archive/toc').write_bytes(b'owned vanilla game data; must never be snapshotted or changed')
        for name in ('Overstrike/Overstrike.exe', 'scripts_proxy.dll', 'winmm.dll', 'scripts/SF.dll', 'scripts/WS.dll'):
            p = self.game / name
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_bytes(('owned ' + name).encode())
        (self.game / 'scripts.txt').write_text('SF.dll\nWS.dll\n')
        (self.game / 'Profiles').mkdir()
        (self.game / 'Profiles/MSM2 (Steam).json').write_text(json.dumps({
            'game': 'MSM2', 'path': str(self.game), 'mods': [['普通.zip', True]],
            'settings': {'scripts': {'enabled': True}}}))
        (self.game / 'Profiles/Settings.json').write_text(json.dumps({'profile': 'MSM2 (Steam)'}))
        ordinary = self.game / 'Mods Library/普通 MOD'
        ordinary.mkdir(parents=True)
        (ordinary / 'keep.zip').write_bytes(b'owned ordinary archive bytes; never opened by toolkit')
        for project in ('MSM2-SF', 'MSM2-WS'):
            root = self.game / 'Mods Library/Mods-Developing' / project
            source = root / '_source'
            source.mkdir(parents=True)
            git(source, 'init', '-b', 'owned-fixture')
            git(source, 'config', 'user.name', 'Owned Fixture')
            git(source, 'config', 'user.email', 'owned@example.invalid')
            git(source, 'config', 'core.autocrlf', 'false')
            (source / 'code.txt').write_text('owned initial source')
            git(source, 'add', '.')
            git(source, 'commit', '-m', 'owned initial')
            (source / 'code.txt').write_text('owned second version')
            git(source, 'commit', '-am', 'owned second')
            (source / 'code.txt').write_text('uncommitted work must survive')
            (source / '未追跡.txt').write_text('untracked work', encoding='utf-8')
            git(source, 'status', '--porcelain')
            (root / 'logs').mkdir()
            (root / 'logs/current.log').write_text('owned synthetic log; not real user data')
            (root / 'empty-folder').mkdir()
            (root / (project + '-v1.0.0.script')).write_bytes(b'owned package bytes')
        self.system = System()
        self.output = self.root / '別ドライブ Backups'
        m.install(self.game, self.output, system=self.system)
        self.before_game = (self.game / 'Spider-Man2.exe').read_bytes()
        self.before_toc = (self.game / 'asset_archive/toc').read_bytes()

    def snapshot(self):
        with patch('sys.stdout', new=io.StringIO()):
            return m.backup(self.game, self.output, self.system)

    def project(self, name):
        return self.game / 'Mods Library/Mods-Developing' / name

    def assert_game_untouched(self):
        self.assertEqual((self.game / 'Spider-Man2.exe').read_bytes(), self.before_game)
        self.assertEqual((self.game / 'asset_archive/toc').read_bytes(), self.before_toc)

    def rewrite(self, original, change_manifest=None, change_payload=None, extra=None):
        out = self.root / 'modified.zip'
        with zipfile.ZipFile(original) as z:
            members = {n: z.read(n) for n in z.namelist()}
        if change_manifest:
            manifest = json.loads(members['manifest.json'])
            change_manifest(manifest)
            members['manifest.json'] = json.dumps(manifest).encode()
        if change_payload:
            change_payload(members)
        with zipfile.ZipFile(out, 'w') as z:
            for n, data in members.items():
                z.writestr(n, data)
            if extra:
                z.writestr(*extra)
        return out

    def test_installer_is_shared_and_does_not_mutate_project_sources(self):
        self.assertEqual(json.loads((self.game / 'Mod-Tools/mod-tools.json').read_text())['profile'], 'MSM2 (Steam)')
        self.assertEqual(self.system.events[-1], ('shortcuts', self.game))
        for name in ('MSM2-SF', 'MSM2-WS'):
            self.assertEqual((self.project(name) / '_source/code.txt').read_text(), 'uncommitted work must survive')
        self.assert_game_untouched()

    def test_full_roundtrip_git_history_dirty_work_logs_ordinary_mods_empty_dirs(self):
        original = tree(self.game)
        histories = {n: git(self.project(n) / '_source', 'log', '--format=%H') for n in ('MSM2-SF', 'MSM2-WS')}
        archive = self.snapshot() / 'environment.zip'
        metadata = m.verify(archive)
        self.assertNotIn('Spider-Man2.exe', [e['path'] for e in metadata['files']])
        self.assertFalse(any(e['path'].startswith('asset_archive') for e in metadata['files']))
        (self.game / 'Mods Library/普通 MOD/keep.zip').write_bytes(b'newer data retained in rollback')
        rollback = m.restore(archive, self.game, self.system, apply=True)
        self.assertEqual((rollback / 'Mods Library/普通 MOD/keep.zip').read_bytes(), b'newer data retained in rollback')
        for rel, content in original.items():
            if rel == 'Mod-Tools/mod-tools.json':
                continue
            self.assertEqual((self.game / rel).read_bytes(), content, rel)
        for name in ('MSM2-SF', 'MSM2-WS'):
            source = self.project(name) / '_source'
            self.assertEqual(git(source, 'log', '--format=%H'), histories[name])
            self.assertIn('code.txt', git(source, 'status', '--porcelain'))
            self.assertTrue((self.project(name) / 'empty-folder').is_dir())
            git(source, 'fsck', '--no-progress')
        self.assertTrue(m.load_settings(self.game)['restore_requires_install'])
        self.assert_game_untouched()

    def test_restore_onto_vanilla_same_path_with_independent_recovery_engine(self):
        folder = self.snapshot()
        archive = folder / 'environment.zip'
        old = self.root / 'previous game'
        self.game.rename(old)
        self.game.mkdir()
        (self.game / 'Spider-Man2.exe').write_bytes(self.before_game)
        (self.game / 'asset_archive').mkdir()
        (self.game / 'asset_archive/toc').write_bytes(self.before_toc)
        # Import the engine saved beside the archive: no dependency on old game/repo.
        spec = importlib.util.spec_from_file_location('owned_recovery', folder / 'mod_environment.py')
        recovery = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(recovery)
        recovery.restore(archive, self.game, self.system, apply=True)
        self.assertEqual((self.game / 'Mods Library/普通 MOD/keep.zip').read_bytes(), b'owned ordinary archive bytes; never opened by toolkit')
        self.assertTrue((self.game / 'Mod-Tools/Play-Modded.cmd').is_file())
        self.assert_game_untouched()

    def test_verify_only_and_restore_plan_do_not_change_existing_roots(self):
        archive = self.snapshot() / 'environment.zip'
        before = tree(self.game)
        plan = m.restore(archive, self.game)
        self.assertGreater(plan['files'], 1)
        self.assertEqual(before, tree(self.game))

    def test_payload_hash_mismatch_even_with_valid_zip_crc_refuses(self):
        archive = self.snapshot() / 'environment.zip'
        corrupt = self.rewrite(archive, change_payload=lambda members: members.__setitem__('payload/scripts_proxy.dll', b'corrupt data of a different size'))
        before = tree(self.game)
        with self.assertRaises(m.EnvironmentError):
            m.restore(corrupt, self.game, self.system, apply=True)
        self.assertEqual(before, tree(self.game))
        # Same-size valid-CRC corruption reaches the SHA check too.
        with zipfile.ZipFile(archive) as z:
            original = z.read('payload/scripts_proxy.dll')
        corrupt = self.rewrite(archive, change_payload=lambda members: members.__setitem__('payload/scripts_proxy.dll', bytes([original[0] ^ 1]) + original[1:]))
        with self.assertRaisesRegex(m.EnvironmentError, 'corrupted'):
            m.verify(corrupt)

    def test_path_traversal_windows_devices_and_aliases_refuse(self):
        archive = self.snapshot() / 'environment.zip'
        for bad in ('../outside', '/absolute', 'Mods Library/../outside', 'Mods Library/C:ads', 'Mods Library/CON.txt', 'Mods Library/end.', 'Mods Library/end ', 'Mods Library\\other'):
            with self.subTest(bad=bad):
                invalid = self.rewrite(archive, change_manifest=lambda data: data['directories'].append(bad))
                with self.assertRaises(m.EnvironmentError):
                    m.verify(invalid)
        invalid = self.rewrite(archive, change_manifest=lambda data: data['directories'].append('Mods Library/普通 mod'))
        with self.assertRaisesRegex(m.EnvironmentError, 'Case-alias'):
            m.verify(invalid)

    def test_unlisted_and_duplicate_archive_entries_refuse(self):
        archive = self.snapshot() / 'environment.zip'
        for name in ('payload/Spider-Man2.exe', 'manifest.json'):
            with self.subTest(name=name), self.assertWarns(Warning) if name == 'manifest.json' else context_null():
                invalid = self.rewrite(archive, extra=(name, b'owned invalid extra'))
                with self.assertRaises(m.EnvironmentError):
                    m.verify(invalid)

    def test_symlinks_in_source_target_and_archive_refuse(self):
        outside = self.root / 'outside'
        outside.write_text('must remain')
        link = self.project('MSM2-SF') / 'linked'
        link.symlink_to(outside)
        with self.assertRaises(m.EnvironmentError):
            self.snapshot()
        link.unlink()
        archive = self.snapshot() / 'environment.zip'
        link.symlink_to(outside)
        with self.assertRaises(m.EnvironmentError):
            m.restore(archive, self.game, self.system, apply=True)
        self.assertEqual(outside.read_text(), 'must remain')
        invalid = self.root / 'symlink.zip'
        with zipfile.ZipFile(archive) as src, zipfile.ZipFile(invalid, 'w') as dst:
            for item in src.infolist():
                if item.filename == 'payload/scripts_proxy.dll':
                    item.external_attr = (stat.S_IFLNK | 0o777) << 16
                dst.writestr(item, src.read(item))
        with self.assertRaises(m.EnvironmentError):
            m.verify(invalid)

    def test_linked_git_storage_and_in_progress_lock_refuse(self):
        for rel in ('.git/commondir', '.git/objects/info/alternates', '.git/index.lock'):
            p = self.project('MSM2-WS') / '_source' / rel
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text('external or in progress')
            with self.assertRaises(m.EnvironmentError):
                self.snapshot()
            p.unlink()

    def test_backup_folder_inside_game_or_ancestor_refuses(self):
        for folder in (self.game / 'backups', self.game / 'Mods Library/backups', self.game.parent):
            with self.assertRaises(m.EnvironmentError):
                m.backup(self.game, folder, self.system)

    def test_wrong_exe_or_changed_restore_location_refuses_before_changes(self):
        archive = self.snapshot() / 'environment.zip'
        (self.game / 'Spider-Man2.exe').write_bytes(b'another game build')
        before = tree(self.game)
        with self.assertRaisesRegex(m.EnvironmentError, 'executable differs'):
            m.restore(archive, self.game, self.system, apply=True)
        self.assertEqual(before, tree(self.game))
        other = self.root / 'another game'
        other.mkdir()
        (other / 'Spider-Man2.exe').write_bytes(self.before_game)
        with self.assertRaisesRegex(m.EnvironmentError, 'original game path'):
            m.restore(archive, other, self.system, apply=True)

    def test_running_process_and_operation_lock_preserve_environment(self):
        archive = self.snapshot() / 'environment.zip'
        before = tree(self.game)
        self.system.busy = True
        with self.assertRaises(m.EnvironmentError):
            m.backup(self.game, self.output, self.system)
        with self.assertRaises(m.EnvironmentError):
            m.restore(archive, self.game, self.system, apply=True)
        self.assertEqual(before, tree(self.game))
        self.system.busy = False
        lock = self.game / '.MSM2-Mod-Tools.lock'
        lock.write_text('owned existing lock')
        with self.assertRaises(m.EnvironmentError):
            self.snapshot()
        self.assertEqual(lock.read_text(), 'owned existing lock')
        with self.assertRaises(m.EnvironmentError):
            m.play(self.game, self.system)
        with self.assertRaises(m.EnvironmentError):
            m.open_manager(self.game, self.system)

    def test_file_changed_during_backup_rejects_and_removes_only_pending(self):
        original_scan = m.scan
        calls = 0
        def changing(game):
            nonlocal calls
            calls += 1
            if calls == 2:
                (self.project('MSM2-SF') / '_source/code.txt').write_text('concurrent edit must survive')
            return original_scan(game)
        with patch.object(m, 'scan', changing), self.assertRaisesRegex(m.EnvironmentError, 'changed during backup'):
            self.snapshot()
        self.assertEqual((self.project('MSM2-SF') / '_source/code.txt').read_text(), 'concurrent edit must survive')
        self.assertFalse(list(self.output.iterdir()))

    def test_mid_replacement_failure_rolls_back_every_existing_root(self):
        archive = self.snapshot() / 'environment.zip'
        (self.game / 'Mods Library/普通 MOD/keep.zip').write_bytes(b'new current state')
        before = tree(self.game)
        rename = Path.rename
        def fail_one(path, target):
            if path.parent.name.startswith('.MSM2-restore-') and path.name == 'Mods Library':
                raise OSError('owned mid-deployment failure')
            return rename(path, target)
        with patch.object(Path, 'rename', fail_one), self.assertRaises(OSError):
            m.restore(archive, self.game, self.system, apply=True)
        self.assertEqual(before, tree(self.game))
        self.assertFalse(list(self.root.glob('.MSM2-rollback-*')))
        self.assertFalse(list(self.game.glob('.MSM2-restore-*')))
        self.assert_game_untouched()

    def test_successful_restore_preserves_extra_current_data_in_rollback(self):
        archive = self.snapshot() / 'environment.zip'
        extra = self.project('MSM2-SF') / '_source/new uncommitted.txt'
        extra.write_text('newer work not in snapshot')
        rollback = m.restore(archive, self.game, self.system, apply=True)
        self.assertEqual((rollback / extra.relative_to(self.game)).read_text(), 'newer work not in snapshot')
        self.assertFalse(extra.exists())

    def test_rollback_failure_keeps_both_recovery_trees_and_reports_paths(self):
        archive = self.snapshot() / 'environment.zip'
        rename = Path.rename
        def fail_twice(path, target):
            if path.parent.name.startswith('.MSM2-restore-') and path.name == 'Mods Library':
                raise OSError('owned deployment failure')
            if path.parent.name.startswith('.MSM2-rollback-') and path.name == 'Profiles':
                raise OSError('owned rollback failure')
            return rename(path, target)
        with patch.object(Path, 'rename', fail_twice), self.assertRaisesRegex(m.EnvironmentError, 'rollback incomplete'):
            m.restore(archive, self.game, self.system, apply=True)
        stashes = list(self.root.glob('.MSM2-rollback-*'))
        self.assertEqual(len(stashes), 1)
        self.assertTrue((stashes[0] / 'Profiles/MSM2 (Steam).json').is_file())
        self.assertTrue(list(self.game.glob('.MSM2-restore-*')))
        self.assert_game_untouched()

    def test_restore_refuses_current_files_changed_during_staging(self):
        archive = self.snapshot() / 'environment.zip'
        verify = m.verify
        def changing(path, destination=None):
            result = verify(path, destination)
            if destination:
                (self.project('MSM2-SF') / '_source/code.txt').write_text('new concurrent work')
            return result
        with patch.object(m, 'verify', changing), self.assertRaisesRegex(m.EnvironmentError, 'changed during staging'):
            m.restore(archive, self.game, self.system, apply=True)
        self.assertEqual((self.project('MSM2-SF') / '_source/code.txt').read_text(), 'new concurrent work')
        self.assert_game_untouched()

    def test_file_timestamp_readonly_mode_and_bootstrap_tools_preserved(self):
        original = self.project('MSM2-SF') / '_source/readonly.txt'
        original.write_text('saved file')
        os.utime(original, ns=(1600000000123456789, 1600000000123456789))
        original.chmod(0o444)
        folder = self.snapshot()
        m.restore(folder / 'environment.zip', self.game, self.system, apply=True)
        self.assertEqual(original.stat().st_mtime_ns, 1600000000123456789)
        self.assertEqual(stat.S_IMODE(original.stat().st_mode), 0o444)
        for file in m.KIT:
            self.assertTrue((folder / file).is_file(), file)

    def test_snapshot_names_never_overwrite_previous_backup(self):
        first = self.snapshot()
        second = self.snapshot()
        self.assertNotEqual(first, second)
        self.assertTrue((first / 'environment.zip').is_file())
        self.assertTrue((second / 'environment.zip').is_file())

    def test_play_and_restore_install_confirmation_gate(self):
        m.play(self.game, self.system)
        self.assertEqual(self.system.events[-1], ('launch', self.game, False))
        settings = m.load_settings(self.game)
        settings['restore_requires_install'] = True
        m.save_settings(self.game, settings)
        with self.assertRaises(m.EnvironmentError):
            m.play(self.game, self.system)
        m.play(self.game, self.system, confirm_install=True)
        self.assertFalse(m.load_settings(self.game)['restore_requires_install'])
        (self.game / 'scripts/WS.dll').unlink()
        with self.assertRaises(m.EnvironmentError):
            m.play(self.game, self.system)

    def test_ambiguous_profile_or_unrelated_tools_folder_preserved(self):
        second = self.game / 'Profiles/another.json'
        second.write_bytes((self.game / 'Profiles/MSM2 (Steam).json').read_bytes())
        self.assertEqual(m.profile(self.game)[0], 'MSM2 (Steam)')
        settings = self.game / 'Mod-Tools/mod-tools.json'
        settings.write_text('{}')
        with self.assertRaisesRegex(m.EnvironmentError, 'Unrecognized'):
            m.install(self.game, self.output, system=self.system)
        self.assertEqual(settings.read_text(), '{}')


from contextlib import nullcontext as context_null
if __name__ == '__main__':
    unittest.main(verbosity=2)
