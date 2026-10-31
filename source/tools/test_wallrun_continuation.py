import hashlib
import json
import struct
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch
import capture_wallrun_continuation as c

def fixture(code=b'\x90'*127+b'\xc3',end=None,entry=True,unwind=b'\x01\0\0\0'):
    d=bytearray(4096);d[:2]=b'MZ';struct.pack_into('<I',d,60,0x80)
    d[0x80:0x84]=b'PE\0\0';struct.pack_into('<HH',d,0x84,0x8664,2);struct.pack_into('<H',d,0x94,240)
    o=0x98;struct.pack_into('<H',d,o,0x20b);struct.pack_into('<Q',d,o+24,0x140000000);struct.pack_into('<I',d,o+56,0x2000000);struct.pack_into('<I',d,o+108,16)
    # Executable mapping and readonly metadata mapping. No actual game bytes.
    s=o+240;struct.pack_into('<IIII',d,s+8,0x200,c.TARGET-0xF9,0x200,0x400);struct.pack_into('<I',d,s+36,0x60000020)
    struct.pack_into('<IIII',d,s+48,0x200,0x1800000,0x200,0x800);struct.pack_into('<I',d,s+76,0x40000040)
    if entry:
        struct.pack_into('<II',d,o+112+24,0x1800000,12)
        struct.pack_into('<III',d,0x800,c.TARGET-5,end or c.TARGET+128,0x1800040)
    d[0x840:0x840+len(unwind)]=unwind
    d[0x4F9:0x4F9+len(code)]=code
    return d
class Tests(unittest.TestCase):
    def run_fixture(self,d,**kw):
        with patch.object(c,'EXPECTED_SHA',hashlib.sha256(d).hexdigest()):return c.extract(d,**kw)
    def test_auth_and_invalid_range(self):
        with self.assertRaisesRegex(c.Refusal,'unsupported_build_sha256'):c.extract(fixture())
        for kw in [{'start':c.TARGET+1},{'limit':129},{'limit':0}]:
            with self.assertRaises(c.Refusal):self.run_fixture(fixture(),**kw)
    def test_control_stop_and_cap(self):
        for code,n in [(b'\x90\xe8\0\0\0\0'+b'\x90'*122,6),(b'\x90'*128,128),(b'\xc3'+b'\x90'*127,1)]:
            r=self.run_fixture(fixture(code));self.assertEqual(r['bytes'],n);self.assertEqual(len(r['byte_evidence_hex']),2*n);self.assertFalse(r['write_certificate']);self.assertLess(len(json.dumps(r)),40000)
    def test_runtime_and_no_entry_fact(self):
        r=self.run_fixture(fixture(end=c.TARGET+8));self.assertEqual(r['bytes'],8);self.assertEqual(r['runtime']['metadata_bytes'],4)
        r=self.run_fixture(fixture(entry=False));self.assertEqual(r['runtime_coverage'],'no_covering_entry');self.assertIsNone(r['runtime'])
    def test_section_and_raw_refusals(self):
        for change,reason in [('flags','not_executable'),('raw','not_file_backed'),('trunc','invalid_PE')]:
            d=fixture();s=0x98+240
            if change=='flags':struct.pack_into('<I',d,s+36,0x40000040)
            if change=='raw':struct.pack_into('<I',d,s+16,0x100)
            if change=='trunc':d=d[:0x900]
            with self.assertRaisesRegex(c.Refusal,reason):self.run_fixture(d)
    def test_unwind_refusal_and_chain_no_follow(self):
        for u in [b'\x02\0\0\0',b'\x39\0\0\0',b'\x01\0\xff\0']:
            with self.assertRaises(c.Refusal):self.run_fixture(fixture(unwind=u))
        u=b'\x21\0\0\0'+struct.pack('<III',c.TARGET-20,c.TARGET-10,0x1800080)
        r=self.run_fixture(fixture(unwind=u));self.assertFalse(r['runtime']['chained_entry']['followed'])
    def test_metadata_conflict_and_decoder_boundary(self):
        d=fixture();struct.pack_into('<II',d,0x98+112+24,0x1800000,24);struct.pack_into('<III',d,0x80c,c.TARGET,c.TARGET+4,0x1800040)
        with self.assertRaisesRegex(c.Refusal,'runtime'):self.run_fixture(d)
        d=fixture(b'\x90'*127+b'\x0f')
        with self.assertRaisesRegex(c.Refusal,'decode_gap'):self.run_fixture(d)
    def test_section_edge_future_metadata_and_size(self):
        d=fixture();struct.pack_into('<I',d,0x98+240+8,0x101)
        r=self.run_fixture(d);self.assertEqual(r['bytes'],8)
        d=fixture(entry=False);struct.pack_into('<II',d,0x98+112+24,0x1800000,12);struct.pack_into('<III',d,0x800,c.TARGET+5,c.TARGET+10,0x1800040)
        r=self.run_fixture(d);self.assertEqual(r['bytes'],5);self.assertIsNone(r['runtime'])
        d=fixture();struct.pack_into('<I',d,0x98+108,0)
        with self.assertRaisesRegex(c.Refusal,'not_declared'):self.run_fixture(d)
    def test_cli_refusal_no_overwrite(self):
        import sys
        with tempfile.TemporaryDirectory() as t:
            exe=Path(t)/'fake.exe';exe.write_bytes(fixture());out=Path(t)/'out.json'
            args=['capture',str(exe),'--output',str(out)]
            with patch.object(sys,'argv',args):self.assertEqual(c.main(),2)
            self.assertEqual(json.loads(out.read_text())['result'],'REFUSED');self.assertNotIn('byte_evidence_hex',out.read_text())
            with patch.object(sys,'argv',args):self.assertEqual(c.main(),2)
            self.assertEqual(exe.read_bytes(),fixture())
if __name__=='__main__':unittest.main()
