import hashlib
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch
import audit_presence_hook_sites as a
from test_verify_motion_gravity import image,T


def fixture():
 b=image()
 # Authored entry-like instruction fixtures, not captured game functions.
 b[0xb00:0xb07]=bytes.fromhex('4c8bdc49895b08')
 b[0x1400:0x1405]=bytes.fromhex('48895c2408')
 # Declare matched fixture prologues, with one RBX SAVE_NONVOL slot + extension.
 for j in (1,2):
  at=0x2ac0+j*4
  u=0x4700+(j-1)*16
  struct.pack_into('<I',b,0x2a00+j*12+8,u)
  raw=0x2400+u-0x4000
  b[raw:raw+8]=bytes([1,23,2,0,7 if j==1 else 5,0x34,1,0])
 sites={name:(T[key],end,hashlib.sha256(b[0x400+T[key]-0x1000:0x400+end-0x1000]).hexdigest()) for name,key,end in [('RequestSimpleMotion','RequestSimpleMotion',0x1800),('GravityOrientationOwner','GravityOrientationOwner',0x2100)]}
 return b,sites

def run(b,s):return a.audit(b,expected_sha=hashlib.sha256(b).hexdigest(),sites=s)

class Tests(unittest.TestCase):
 def test_audit_never_admits(self):
  b,s=fixture();r=run(b,s)
  self.assertEqual(r['sites']['RequestSimpleMotion']['prefix']['stolen_bytes'],7)
  self.assertEqual(r['sites']['GravityOrientationOwner']['prefix']['stolen_bytes'],5)
  self.assertFalse(r['observer_implemented']);self.assertFalse(r['production_permission'])
  self.assertTrue(all(not x['hook_admitted'] for x in r['sites'].values()))
 def test_unwind_decode(self):
  b,s=fixture();u=run(b,s)['sites']['RequestSimpleMotion']['unwind']
  self.assertEqual(u['codes'][0]['operation'],4)
  self.assertEqual(u['codes'][0]['value'],8)
 def test_wrong_hash(self):
  b,s=fixture()
  with self.assertRaisesRegex(a.pe.Refusal,'unsupported_build'):a.audit(b)
  bad=dict(s);v=bad['RequestSimpleMotion'];bad['RequestSimpleMotion']=(v[0],v[1],'0'*64)
  with self.assertRaisesRegex(a.pe.Refusal,'hash_changed'):run(b,bad)
 def test_unwind_invalid_versions_and_slots(self):
  for index,value,why in ((0,2,'unsupported'),(2,65,'unsupported'),(4,24,'code_order'),(5,6,'operation'),(2,1,'extra_slots')):
   b,s=fixture();b[0x2b00+index]=value
   with self.assertRaisesRegex(a.pe.Refusal,why):run(b,s)
 def test_changed_runtime_entry(self):
  b,s=fixture();struct.pack_into('<I',b,0x2a0c,0x1710)
  with self.assertRaisesRegex(a.pe.Refusal,'runtime_entry'):run(b,s)
 def test_unsupported_prefix_not_permission(self):
  b,s=fixture();b[0xb00:0xb07]=b'\x90'*7
  name='RequestSimpleMotion';start,end,_=s[name];s[name]=(start,end,hashlib.sha256(b[0xb00:0xc00]).hexdigest())
  r=run(b,s)
  self.assertFalse(r['sites'][name]['prefix']['simple_no_stack_adjust_candidate'])
  self.assertFalse(r['sites'][name]['hook_admitted'])
 def test_prefix_outside_prologue(self):
  b,s=fixture();b[0x2b01]=1;b[0x2b04]=1
  with self.assertRaisesRegex(a.pe.Refusal,'outside_declared'):run(b,s)
 def test_no_full_body_or_raw_prefix(self):
  b,s=fixture();text=str(run(b,s))
  self.assertNotIn('hex',text);self.assertIn('NOT_EXECUTED',text)
 def test_cli_exclusive(self):
  with tempfile.TemporaryDirectory() as t:
   p=Path(t);exe=p/'game テ.exe';exe.write_bytes(b'wrong');out=p/'audit.json'
   cmd=[sys.executable,a.__file__,str(exe),'--output',str(out)]
   self.assertEqual(subprocess.run(cmd,capture_output=True).returncode,2);old=out.read_bytes()
   self.assertNotEqual(subprocess.run(cmd,capture_output=True).returncode,0);self.assertEqual(out.read_bytes(),old)

if __name__=='__main__':unittest.main(verbosity=2)
