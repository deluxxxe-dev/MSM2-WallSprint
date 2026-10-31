from pathlib import Path
import tempfile,unittest,zipfile,json
import select_wallrun_comparison as c
class Quiet:
    def stop(self,g):pass
class Tests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup);self.game=Path(self.tmp.name)/'game テ space';self.root=self.game/'Mods Library/Mods-Developing/MSM2-WS'
        for d in ['logs','_source','_archive']:(self.root/d).mkdir(parents=True)
        (self.game/'scripts').mkdir();self.ini=self.game/'scripts/WallSprint.ini';self.dll=self.game/'scripts/WallSprint.dll';self.original=b'[WallSprint]\r\nWALLSPRINT_NATIVE_SOURCE_OVERRIDE=1\r\nOTHER=keep\r\n';self.ini.write_bytes(self.original);self.dll.write_bytes(b'owned synthetic DLL');self.pkg=self.root/'MSM2-WS-v0.1c38r7.script'
        with zipfile.ZipFile(self.pkg,'w') as z:
            z.writestr('info.json',json.dumps({'name':'WallSprint','version':'0.1.63'}));z.writestr('WallSprint.dll',self.dll.read_bytes());z.writestr('WallSprint.ini',self.original)
        self.foreign=self.game/'scripts/Other.ini';self.foreign.write_bytes(b'preserve')
    def test_modes_and_restore(self):
        off=c.select(self.game,'OFF',Quiet());self.assertIn(b'OVERRIDE=0',off);self.assertIn(b'OTHER=keep\r\n',off)
        c.select(self.game,'ON',Quiet());self.assertEqual(self.ini.read_bytes(),self.original)
        c.select(self.game,'OFF',Quiet());c.select(self.game,'RESTORE',Quiet());self.assertEqual(self.ini.read_bytes(),self.original);self.assertEqual(self.foreign.read_bytes(),b'preserve')
    def test_modified_config_or_dll_refuses(self):
        self.ini.write_bytes(self.original+b'extra=private')
        with self.assertRaises(ValueError):c.select(self.game,'OFF',Quiet())
        self.ini.write_bytes(self.original);self.dll.write_bytes(b'foreign')
        with self.assertRaises(ValueError):c.select(self.game,'OFF',Quiet())
        self.assertEqual(self.ini.read_bytes(),self.original)
    def test_running_refuses(self):
        class Running:
            def stop(self,g):raise RuntimeError('running')
        with self.assertRaises(RuntimeError):c.select(self.game,'OFF',Running())
        self.assertEqual(self.ini.read_bytes(),self.original)
    def test_backup_and_extra_script_refuse(self):
        (self.root/'_archive/WallSprint-comparison-original.ini').write_bytes(b'other backup')
        with self.assertRaises(ValueError):c.select(self.game,'OFF',Quiet())
        self.assertEqual(self.ini.read_bytes(),self.original)
        (self.root/'extra.script').write_bytes(b'extra')
        with self.assertRaises(RuntimeError):c.select(self.game,'OFF',Quiet())
if __name__=='__main__':unittest.main()
