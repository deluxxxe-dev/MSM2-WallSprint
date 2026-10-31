import hashlib
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch
import verify_motion_gravity as v
from test_discover_ground_support import fixture

T={'UpdateMotion':0x1000,'RequestSimpleMotion':0x1700,'GravityOrientationOwner':0x2000,'SetGravityVector':0x2200}
M=[(name,off,1,hashlib.sha256(b'\x90').hexdigest()) for name,off,_,_ in v.MARKERS]
def image():
    b=fixture(())+bytearray(0x1800)
    op=0x98
    struct.pack_into('<I',b,op+56,0x6000)
    struct.pack_into('<IIII',b,op+240+8,0x2000,0x1000,0x2000,0x400)
    struct.pack_into('<IIII',b,op+280+8,0x800,0x4000,0x800,0x2400)
    b[0x400:0x2400]=b'\x90'*0x2000
    struct.pack_into('<I',b,op+108,16)
    struct.pack_into('<II',b,op+112+24,0x4600,48)
    for j,(a,e) in enumerate(((0x1000,0x1600),(0x1700,0x1800),(0x2000,0x2100),(0x2200,0x2300))):
        struct.pack_into('<III',b,0x2a00+j*12,a,e,0x46c0+j*4)
        b[0x2ac0+j*4:0x2ac4+j*4]=b'\x01\0\0\0'
    for owner,off,target in v.RELATIONS:
        site=T[owner]+off;at=site-0x1000+0x400
        b[at:at+5]=b'\xe8'+struct.pack('<i',T[target]-site-5)
    return b

def run(b):
    with patch.object(v,'MARKERS',M):return v.verify(b,expected_sha=hashlib.sha256(b).hexdigest(),targets=T)

class Tests(unittest.TestCase):
    def test_positive(self):
        r=run(image());self.assertEqual(r['result'],'REFERENCE_RELATIONS_MATCH')
        self.assertFalse(r['callable_ABI_proven']);self.assertFalse(r['production_permission']);self.assertFalse(r['code_exported'])
    def test_marker_mismatch(self):
        b=image();b[0xb00]=0xcc
        self.assertEqual(run(b)['result'],'REFERENCE_RELATIONS_MISMATCH')
    def test_bad_call_opcode_and_target(self):
        for value in (b'\x90'*5,b'\xe8\0\0\0\0'):
            b=image();b[0x8e6:0x8eb]=value
            self.assertFalse(run(b)['relations_match'])
    def test_boundary_partial(self):
        b=image();struct.pack_into('<II',b,0x2a00+36,0x2210,0x2300)
        self.assertEqual(run(b)['result'],'RELATIONS_MATCH_BOUNDARY_PARTIAL')
    def test_call_outside_owner(self):
        b=image();struct.pack_into('<I',b,0x2a04,0x1100)
        self.assertFalse(run(b)['relations_match'])
    def test_hash_malformed_ranges(self):
        with self.assertRaisesRegex(v.pe.Refusal,'unsupported_build'):v.verify(image())
        for b in (b'',image()[:0x200]):
            with self.assertRaises(v.pe.Refusal):run(b)
        b=image();struct.pack_into('<I',b,0x98+240+20,len(b))
        with self.assertRaises(v.pe.Refusal):run(b)
    def test_runtime_caps_and_order(self):
        for at,value in ((0x98+112+28,9*1024*1024),(0x2a0c,0x1000)):
            b=image();struct.pack_into('<I',b,at,value)
            with self.assertRaises(v.pe.Refusal):run(b)
    def test_writeable_code_and_decoder(self):
        b=image();struct.pack_into('<I',b,0x98+240+36,0xe0000020)
        with self.assertRaises(v.pe.Refusal):run(b)
        with patch.object(v.cs,'__version__','wrong'):
            with self.assertRaises(v.pe.Refusal):run(image())
    def test_exclusive_cli_no_override(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp)/'space テ';root.mkdir();exe=root/'game.exe';exe.write_bytes(image());out=root/'result.json'
            cmd=[sys.executable,v.__file__,str(exe),'--output',str(out)]
            self.assertEqual(subprocess.run(cmd,capture_output=True).returncode,2)
            old=out.read_bytes();self.assertNotEqual(subprocess.run(cmd,capture_output=True).returncode,0)
            self.assertEqual(out.read_bytes(),old);self.assertEqual(exe.read_bytes(),image())
            self.assertNotEqual(subprocess.run(cmd+['--shift','1'],capture_output=True).returncode,0)

if __name__=='__main__':unittest.main(verbosity=2)
