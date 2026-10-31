"""Real Git/binary extraction and Wine-owned execution; no game fixtures/data."""
import hashlib
import importlib.util
import json
from pathlib import Path
import subprocess
import tempfile
import unittest
import run_presence_owned as runner

ROOT = Path(__file__).resolve().parents[2]
BINARY = ROOT / 'source/build/presence-owned-fixed/presence-owned.exe'


class RunnerTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='WS-owned-')
        self.addCleanup(self.temp.cleanup)
        self.source = Path(self.temp.name) / 'WS テ space' / '_source'
        self.source.mkdir(parents=True)
        (self.source.parent / 'logs').mkdir()
        self.git('init', '-q', '-b', runner.BRANCH)
        self.git('config', 'user.email', 'fixture@example.invalid')
        self.git('config', 'user.name', 'Owned fixture')
        self.git('remote', 'add', 'origin', runner.ORIGIN)
        binary = BINARY.read_bytes()
        artifact = self.source / runner.ARTIFACT
        artifact.parent.mkdir(parents=True)
        artifact.write_bytes(binary)
        self.manifest = self.source / runner.MANIFEST
        self.manifest.write_text(json.dumps({'size':len(binary), 'sha256':hashlib.sha256(binary).hexdigest(), 'production_admitted':False}))
        self.commit()

    def git(self, *args):
        return subprocess.check_output(['git', '-C', str(self.source), *args], stderr=subprocess.PIPE)

    def commit(self):
        self.git('add', '.')
        self.git('commit', '-qm', 'owned fixture')
        self.head = self.git('rev-parse', 'HEAD').decode().strip()

    def test_real_binary_roundtrip_unicode_and_cleanup(self):
        staged = []
        def launch(args, **kwargs):
            staged.append(Path(args[0]))
            self.assertFalse(args[0].startswith(str(self.source.parent)))
            # Wine can consume Z:/ absolute Linux paths; Unicode exercises _wfopen.
            return subprocess.run(['/workspace/toolchains/wine/usr/lib/wine/wine64', args[0], 'Z:' + args[1]], **kwargs)
        output = runner.run(self.source, self.head, launch)
        result = json.loads(output.read_text())
        self.assertEqual(result['native_result'], 'UNAVAILABLE')
        self.assertEqual(result['fail'], 0)
        self.assertEqual(result['diagnostic_revision'], 3)
        self.assertEqual(len(result['stackwalk_diagnostics']), 110)
        self.assertTrue(all(not p.exists() for p in staged))
        self.assertEqual(self.git('status', '--porcelain'), b'')

    def test_synthetic_native_55_failures_remain_fail_and_report_survives(self):
        # Classification regression only, NOT a native measurement.
        report = json.loads((BINARY.parent / 'result.json').read_text())
        report['environment'] = 'native Windows'
        report['native_windows_unavailable'] = 0
        for assertion in report['assertions']:
            if assertion['name'].endswith(' StackWalk64 caller'):
                assertion['pass'] = False
        report['pass'], report['fail'] = sum(x['pass'] for x in report['assertions']), 55
        def failed_launch(args, **kwargs):
            Path(args[1]).write_text(json.dumps(report))
            return subprocess.CompletedProcess(args, 1, stdout=b'', stderr=b'')
        with self.assertRaisesRegex(RuntimeError, 'assertion/runtime failure'):
            runner.run(self.source, self.head, failed_launch)
        outputs = list((self.source.parent / 'logs').glob('*.json'))
        self.assertEqual(len(outputs), 1)
        retained = json.loads(outputs[0].read_text())
        self.assertEqual(retained['native_result'], 'FAIL')
        self.assertEqual((retained['pass'], retained['fail']), (150, 55))
        self.assertFalse(retained['production_admitted'])

    def test_hash_mismatch_refuses_execution(self):
        data = json.loads(self.manifest.read_text()); data['sha256'] = '0'*64
        self.manifest.write_text(json.dumps(data)); self.commit()
        with self.assertRaisesRegex(ValueError, 'hash mismatch'):
            runner.run(self.source, self.head, lambda *a, **k: self.fail('must not execute'))

    def test_wrong_branch_refuses_execution(self):
        self.git('switch', '-qc', 'wrong')
        with self.assertRaisesRegex(ValueError, 'Wrong WS branch'):
            runner.run(self.source, self.head, lambda *a, **k: self.fail('must not execute'))

    def test_wrong_origin_refuses_execution(self):
        self.git('remote', 'set-url', 'origin', 'https://example.invalid/wrong')
        with self.assertRaisesRegex(ValueError, 'Wrong WS origin'):
            runner.run(self.source, self.head, lambda *a, **k: self.fail('must not execute'))

    def test_missing_logs_refuses_execution(self):
        (self.source.parent / 'logs').rmdir()
        with self.assertRaisesRegex(ValueError, 'logs directory required'):
            runner.run(self.source, self.head, lambda *a, **k: self.fail('must not execute'))


if __name__ == '__main__':
    unittest.main()
