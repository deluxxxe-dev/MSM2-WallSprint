import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch
import capstone as cs
import motion_gravity_contract as c
from test_verify_motion_gravity import image,T


def snippet(code):
 md=cs.Cs(cs.CS_ARCH_X86,cs.CS_MODE_64);md.detail=True
 ins={i.address:i for i in md.disasm(code,0x1000)}
 return c.analyze_graph(ins,0x1000,0x1000+len(code),{})

def fixture():
 b=image();struct.pack_into('<I',b,0x98+112+28,36)
 for name,start,end in (('UpdateMotion',T['UpdateMotion'],0x1600),('RequestSimpleMotion',T['RequestSimpleMotion'],0x1800),('GravityOrientationOwner',T['GravityOrientationOwner'],0x2100),('SetGravityVector',T['SetGravityVector'],T['SetGravityVector']+0x4f)):
  b[0x400+end-0x1000-1]=0xc3
 roles={k:(v,{'UpdateMotion':0x1600,'RequestSimpleMotion':0x1800,'GravityOrientationOwner':0x2100,'SetGravityVector':T[k]+0x4f}[k],None) for k,v in T.items()}
 roles={k:(s,e,hashlib.sha256(b[0x400+s-0x1000:0x400+e-0x1000]).hexdigest()) for k,(s,e,h) in roles.items()}
 calls=tuple((a,T[a]+off,z) for a,off,z in c.resolution.verify.RELATIONS)
 return b,roles,calls

class Tests(unittest.TestCase):
 def test_pointer_value_store_vs_data_copy(self):
  r=snippet(bytes.fromhex('48895110c3'))
  self.assertTrue(r['access_candidates'][0]['full64_entry_argument_value_store_candidate'])
  self.assertFalse(r['no_pointer_retention_proven'])
  r=snippet(bytes.fromhex('488b02488901c3'))
  store=next(a for a in r['access_candidates'] if a['access']=='write')
  self.assertFalse(store['full64_entry_argument_value_store_candidate'])
  self.assertTrue(store['memory_value_copy_candidate'])
 def test_widths_and_owner_chain(self):
  r=snippet(bytes.fromhex('488b8128010000488b80f8000000f30f104018c3'))
  self.assertEqual([a['width'] for a in r['access_candidates']],[8,8,4])
  self.assertIn('load64',r['access_candidates'][-1]['address'][0])
 def test_stack_spill_and_call_arguments(self):
  r=snippet(bytes.fromhex('4889542410488b4c2410e800000000c3'))
  call=r['call_candidates'][0]
  self.assertEqual(call['arguments']['rcx']['address'],['entry_rdx',0])
  self.assertEqual(call['callee_retention'],'UNKNOWN')
  self.assertTrue(r['unknown_barriers'])
 def test_unknown_call_clobbers_volatile(self):
  r=snippet(bytes.fromhex('e80000000048895110c3'))
  store=next(a for a in r['access_candidates'] if a['access']=='write')
  self.assertIsNone(store['address']);self.assertFalse(store['full64_entry_argument_value_store_candidate'])
 def test_extension_not_full_pointer(self):
  r=snippet(bytes.fromhex('4863c2488901c3'))
  self.assertFalse(r['access_candidates'][0]['full64_entry_argument_value_store_candidate'])
  self.assertIsNone(r['access_candidates'][0]['source']['address'])
 def test_partial_write_invalidates_pointer(self):
  r=snippet(bytes.fromhex('b201488911c3'))
  self.assertFalse(r['access_candidates'][0]['full64_entry_argument_value_store_candidate'])
 def test_branch_merge_unknown_not_certificate(self):
  r=snippet(bytes.fromhex('85c074054889d0eb034c89c0488901c3'))
  stores=[a for a in r['access_candidates'] if a['access']=='write']
  self.assertTrue(any(a['source']['address'] is None for a in stores))
  self.assertFalse(r['object_allocation_lifetime_proven'])
 def test_simd_lane_copies_remain_candidates(self):
  r=snippet(bytes.fromhex('f30f1002f30f1101c3'))
  self.assertEqual([a['width'] for a in r['access_candidates']],[4,4])
  self.assertTrue(any('SIMD' in a['reason'] for a in r['unknown_barriers']))
 def test_register_only_leaf_still_no_lifetime(self):
  r=snippet(bytes.fromhex('4889d0c3'))
  self.assertTrue(r['register_only_leaf_candidate']);self.assertFalse(r['object_allocation_lifetime_proven'])
  self.assertEqual(r['return_candidates'][0]['rax']['address'],['entry_rdx',0])
 def test_exact_function_hash_and_setter_partial(self):
  b,roles,calls=fixture();r=c.report(b,expected_sha=hashlib.sha256(b).hexdigest(),roles=roles,required_calls=calls)
  self.assertEqual(r['roles']['SetGravityVector']['boundary'],'PARTIAL_no_pdata_bounded_leaf')
  self.assertFalse(r['production_permission']);self.assertFalse(r['callable_API_proven'])
  with self.assertRaisesRegex(c.pe.Refusal,'function_hash'):
   c.report(b,expected_sha=hashlib.sha256(b).hexdigest(),roles={**roles,'UpdateMotion':(0x1000,0x1600,'0'*64)},required_calls=calls)
 def test_required_call_changed(self):
  b,roles,calls=fixture()
  with self.assertRaisesRegex(c.pe.Refusal,'CALL_changed'):
   c.report(b,expected_sha=hashlib.sha256(b).hexdigest(),roles=roles,required_calls=(('UpdateMotion',0x1000,'RequestSimpleMotion'),))
 def test_caps(self):
  for attr,value,reason in (('MAX_EVAL',0,'evaluation_cap'),('MAX_EVENTS',0,'event_cap'),('MAX_STACK',0,'stack_cap'),('MAX_REGUSES',0,'register_use_cap'),('MAX_RETURNS',0,'return_cap')):
   with patch.object(c,attr,value):
    with self.assertRaisesRegex(c.pe.Refusal,reason):snippet(bytes.fromhex('4889542410c3'))
  b,roles,calls=fixture()
  with patch.object(c,'MAX_OUTPUT',1):
   with self.assertRaisesRegex(c.pe.Refusal,'output_cap'):c.report(b,expected_sha=hashlib.sha256(b).hexdigest(),roles=roles,required_calls=calls)
 def test_top_absorbs_and_implicit_push_cap(self):
  self.assertEqual(c.bounded({'TOP_origin_cap','entry_rcx'}),frozenset({'TOP_origin_cap'}))
  with patch.object(c,'MAX_STACK',0):
   with self.assertRaisesRegex(c.pe.Refusal,'stack_cap'):snippet(bytes.fromhex('52c3'))
 def test_cli_exclusive_and_no_identity_override(self):
  with tempfile.TemporaryDirectory() as t:
   p=Path(t);exe=p/'game テ.exe';exe.write_bytes(b'not game');out=p/'out.json'
   cmd=[sys.executable,c.__file__,str(exe),'--output',str(out)]
   self.assertEqual(subprocess.run(cmd,capture_output=True).returncode,2)
   before=out.read_bytes();self.assertEqual(json.loads(before)['reason'],'unsupported_build_sha256')
   self.assertNotEqual(subprocess.run(cmd,capture_output=True).returncode,0);self.assertEqual(out.read_bytes(),before)
   self.assertNotEqual(subprocess.run(cmd+['--rva','0'],capture_output=True).returncode,0)

if __name__=='__main__':unittest.main(verbosity=2)
