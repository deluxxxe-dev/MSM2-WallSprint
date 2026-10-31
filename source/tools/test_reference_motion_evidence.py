import hashlib
import io
import json
import subprocess
import sys
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import zipfile
import reference_motion_evidence as r
from test_ground_support_code_refs import image


def zipped(entries):
    out=io.BytesIO()
    with zipfile.ZipFile(out,'w') as z:
        for name,data in entries.items():z.writestr(name,data)
    return out.getvalue()


class Tests(unittest.TestCase):
    def setUp(self):
        self.data=image(r.LABELS[0]);self.sha=hashlib.sha256(self.data).hexdigest()

    def test_reference_relationship_not_game(self):
        v=r.analyze(self.data,self.sha)
        self.assertEqual(len(v['references']),1)
        self.assertEqual(v['reference_sha256'],self.sha)
        self.assertIn('NOT_game_RVAs',v['domain'])
        self.assertFalse(v['native_binding_proven']);self.assertIsNone(v['selected_controller'])

    def test_both_labels_and_unrelated(self):
        for name,expected in ((r.LABELS[1],1),('HeroStateRun',0),('RequestSimpleMotion',0)):
            b=image(name);v=r.analyze(b,hashlib.sha256(b).hexdigest())
            self.assertEqual(len(v['references']),expected)

    def test_nested_package_no_extraction(self):
        with tempfile.TemporaryDirectory() as t:
            root=Path(t)/'space テ';root.mkdir()
            nested=zipped({'scripts/flight.script':zipped({'ScriptedFlight.dll':self.data})})
            (root/'Scripted Flight 2598.zip').write_bytes(nested)
            before=sorted(p.name for p in root.iterdir())
            v=r.acquire([root],expected_sha=self.sha)
            self.assertEqual(v['result'],'CODE_REFERENCE_CANDIDATES')
            self.assertEqual(sorted(p.name for p in root.iterdir()),before)

    def test_duplicate_exact_dlls(self):
        with tempfile.TemporaryDirectory() as t:
            root=Path(t);(root/'ScriptedFlight.dll').write_bytes(self.data)
            (root/'Scripted Flight.zip').write_bytes(zipped({'ScriptedFlight.dll':self.data}))
            self.assertTrue(r.acquire([root],expected_sha=self.sha)['duplicate_exact_copies_deduplicated'])

    def test_wrong_sha_and_no_input(self):
        with tempfile.TemporaryDirectory() as t:
            root=Path(t);(root/'ScriptedFlight.dll').write_bytes(self.data)
            self.assertEqual(r.acquire([root])['result'],'REFERENCE_UNAVAILABLE_OR_DIFFERENT_SHA')
            self.assertEqual(r.acquire([root/'missing'])['named_packages'],0)

    def test_unrelated_archive_not_opened(self):
        with tempfile.TemporaryDirectory() as t:
            root=Path(t);(root/'unrelated.zip').write_bytes(b'not a zip')
            self.assertEqual(r.acquire([root])['named_packages'],0)

    def test_bad_archive(self):
        with tempfile.TemporaryDirectory() as t:
            p=Path(t)/'Scripted Flight.zip';p.write_bytes(b'bad')
            with self.assertRaisesRegex(r.refs.meta.Refusal,'unreadable'):r.acquire([p])

    def test_caps(self):
        with tempfile.TemporaryDirectory() as t:
            p=Path(t)/'Scripted Flight.zip';p.write_bytes(zipped({'ScriptedFlight.dll':self.data}))
            for key,value,reason in (('MAX_CONTAINER',1,'container_cap'),('MAX_TOTAL',1,'byte_cap'),
                                     ('MAX_MEMBERS',0,'member_cap'),('MAX_ARCHIVES',0,'package_cap')):
                with patch.object(r,key,value):
                    with self.assertRaisesRegex(r.refs.meta.Refusal,reason):r.acquire([p])
            with patch.object(r,'MAX_DEPTH',-1):
                with self.assertRaisesRegex(r.refs.meta.Refusal,'nesting_cap'):r.acquire([p])

    def test_cli_exclusive_output(self):
        with tempfile.TemporaryDirectory() as t:
            root=Path(t);out=root/'result.json'
            cmd=[sys.executable,r.__file__,'--root',str(root),'--output',str(out)]
            self.assertEqual(subprocess.run(cmd,capture_output=True).returncode,2)
            old=out.read_bytes()
            self.assertEqual(json.loads(old)['result'],'REFERENCE_UNAVAILABLE_OR_DIFFERENT_SHA')
            self.assertNotEqual(subprocess.run(cmd,capture_output=True).returncode,0)
            self.assertEqual(out.read_bytes(),old)

    def test_symlink_root_refused(self):
        with tempfile.TemporaryDirectory() as t:
            root=Path(t);link=root/'link';link.symlink_to(root,target_is_directory=True)
            with self.assertRaisesRegex(r.refs.meta.Refusal,'root_symlink'):r.acquire([link])

    def test_traversal_member_never_extracted(self):
        with tempfile.TemporaryDirectory() as t:
            p=Path(t)/'Scripted Flight.zip';p.write_bytes(zipped({'../../ScriptedFlight.dll':self.data}))
            self.assertEqual(r.acquire([p],expected_sha=self.sha)['result'],'CODE_REFERENCE_CANDIDATES')
            self.assertEqual(len(list(Path(t).iterdir())),1)

if __name__=='__main__':unittest.main(verbosity=2)
