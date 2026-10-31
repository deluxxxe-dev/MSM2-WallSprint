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
class CompleteTests(unittest.TestCase):
    def fixture77(self):
        # Synthetic conditional branch at41 bytes, no proprietary bytes.
        code=b'\x90'*39+b'\x74\x00'+b'\x90'*36
        u=b'\x21\0\0\0'+struct.pack('<III',*c.CHAIN)
        d=fixture(code=code,end=c.FRAGMENT_END,unwind=u)
        struct.pack_into("<I",d,0x98+56,0xA000000)
        struct.pack_into('<I',d,0x800,c.TARGET)
        return d
    def capture(self,d):
        with patch.object(c,'EXPECTED_SHA',hashlib.sha256(d).hexdigest()),patch.object(c,'FIRST_SHA',hashlib.sha256(d[0x4f9:0x4f9+41]).hexdigest()):return c.extract(d,complete=True)
    def test_whole77_across_conditional_no_follow(self):
        r=self.capture(self.fixture77());self.assertEqual(r['bytes'],77);self.assertEqual(r['range_end_rva'],c.FRAGMENT_END);self.assertEqual(len(r['edges']),1);self.assertFalse(r['edges'][0]['followed']);self.assertTrue(r['previous_window_sha_verified'])
    def test_metadata_identity_refusals(self):
        for offset in [0x800,0x804,0x844,0x848,0x84c]:
            d=self.fixture77();struct.pack_into('<I',d,offset,struct.unpack_from('<I',d,offset)[0]+1)
            with self.assertRaises(c.Refusal):self.capture(d)
        d=self.fixture77();d[0x840]=1
        with self.assertRaises(c.Refusal):self.capture(d)
    def test_first_capture_and_unsupported_ret(self):
        d=self.fixture77()
        with patch.object(c,'EXPECTED_SHA',hashlib.sha256(d).hexdigest()),self.assertRaisesRegex(c.Refusal,'41byte'):c.extract(d,complete=True)
        for code,reason in [(b'\xc3','RET'),(b'\xf4','unsupported')]:
            d=self.fixture77();d[0x4f9+41]=code[0]
            with self.assertRaisesRegex(c.Refusal,reason):self.capture(d)
    def test_tail_decode_and_raw_truncation(self):
        d=self.fixture77();d[0x4f9+76]=0x0f
        with self.assertRaisesRegex(c.Refusal,'decode_gap'):self.capture(d)
        d=self.fixture77();struct.pack_into('<I',d,0x98+240+16,0x110)
        with self.assertRaisesRegex(c.Refusal,'file_backed'):self.capture(d)
class SuccessorTests(unittest.TestCase):
    def fixture_successor(self,code=b'\x0f\xb6\xc0\x85\xd2\x75\x01'+b'\x90'*121):
        d=fixture();o=0x98;s=o+240
        struct.pack_into('<I',d,o+56,0xA000000)
        struct.pack_into('<IIII',d,s+8,0x600,0x1424500,0x600,0x400)
        struct.pack_into('<I',d,s+60,0xb00)
        struct.pack_into('<II',d,o+112+24,0x1800000,24)
        struct.pack_into('<III',d,0xb00,*c.CHAIN)
        struct.pack_into('<III',d,0xb0c,c.SUCCESSOR,c.SUCCESSOR+128,0x1800040)
        d[0xb40:0xb50]=b'\x21\0\0\0'+struct.pack('<III',*c.CHAIN)
        d[0x430:0x5d6]=b'\x90'*422
        d[0x5d6:0x5d6+len(code)]=code
        return d
    def capture_successor(self,d):
        with patch.object(c,'EXPECTED_SHA',hashlib.sha256(d).hexdigest()),patch.object(c,'PREFIX_SHA',hashlib.sha256(b'\x90'*422).hexdigest()):return c.extract(d,successor=True)
    def test_consumers_and_control_no_follow(self):
        r=self.capture_successor(self.fixture_successor());self.assertEqual(r['bytes'],7);self.assertEqual(r['target_rva'],c.SUCCESSOR);self.assertEqual(r['mode'],'shared_successor');self.assertEqual(len(r['register_overlap']),2);self.assertFalse(r['edges'][0]['followed']);self.assertFalse(r['write_certificate'])
    def test_missing_prefix_and_covering_metadata(self):
        d=self.fixture_successor();struct.pack_into('<I',d,0xb00,c.CHAIN[0]+1)
        with self.assertRaisesRegex(c.Refusal,'prefix_runtime'):self.capture_successor(d)
        d=self.fixture_successor();struct.pack_into('<I',d,0x98+112+28,12)
        with self.assertRaisesRegex(c.Refusal,'covering_runtime'):self.capture_successor(d)
    def test_prefix_hash_chain_unwind_and_raw(self):
        d=self.fixture_successor();d[0x430]=0xcc
        with self.assertRaisesRegex(c.Refusal,'prefix_sha'):self.capture_successor(d)
        d=self.fixture_successor();struct.pack_into('<I',d,0xb4c,c.CHAIN[2]+4)
        with self.assertRaisesRegex(c.Refusal,'chain_identity'):self.capture_successor(d)
        d=self.fixture_successor();d[0xb40]=2
        with self.assertRaisesRegex(c.Refusal,'unwind'):self.capture_successor(d)
        d=self.fixture_successor();struct.pack_into('<I',d,0x98+240+16,0x1da)
        with self.assertRaisesRegex(c.Refusal,'file_backed'):self.capture_successor(d)
    def test_metadata_bound_cap_and_mutually_exclusive(self):
        d=self.fixture_successor(b'\x90'*128);r=self.capture_successor(d);self.assertEqual(r['bytes'],128)
        struct.pack_into('<I',d,0xb10,c.SUCCESSOR+5);r=self.capture_successor(d);self.assertEqual(r['bytes'],5)
        with self.assertRaisesRegex(c.Refusal,'mutually_exclusive'):c.extract(d,complete=True,successor=True)
    def test_cli_successor_output(self):
        import sys
        d=self.fixture_successor()
        with tempfile.TemporaryDirectory() as t:
            exe=Path(t)/'fixture.exe';exe.write_bytes(d);out=Path(t)/'successor.json'
            with patch.object(sys,'argv',['capture',str(exe),'--shared-successor','--output',str(out)]),patch.object(c,'EXPECTED_SHA',hashlib.sha256(d).hexdigest()),patch.object(c,'PREFIX_SHA',hashlib.sha256(b'\x90'*422).hexdigest()):self.assertEqual(c.main(),0)
            r=json.loads(out.read_text());self.assertEqual(r['mode'],'shared_successor');self.assertEqual(exe.read_bytes(),d)
if __name__=='__main__':unittest.main()
