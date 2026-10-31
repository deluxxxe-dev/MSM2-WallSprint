import hashlib
from pathlib import Path
import subprocess
import struct
import sys
import tempfile
import unittest
from unittest.mock import patch
import resolve_motion_gravity as r
from test_verify_motion_gravity import image,T,M

def token(j):return b'\xb8'+(0x11223300+j).to_bytes(4,'little')+b'\x90'*3

def desc(role,kind,b):
 return dict(role=role,kind=kind,length=len(b),wildcards=[],seed_offset=0,
             seed_hex=b[:8].hex(),masked_sha256=hashlib.sha256(b).hexdigest())

def fixture():
 b=image();patterns=[]
 for j,(role,start) in enumerate(T.items()):
  at=start-0x1000+0x400;b[at:at+8]=token(j)
  patterns.append(desc(role,'entry',token(j)))
 markers=[(name,off,n,hashlib.sha256(bytes([0xb8]) if off==0 else b'\x90').hexdigest()) for name,off,n,_ in M]
 return b,patterns,markers

def run(b,p,m,canonical=None):
 with patch.object(r.verify,'MARKERS',m):
  return r.resolve(b,expected_sha=hashlib.sha256(b).hexdigest(),patterns=p,
                   canonical=canonical or {k:v-0x20 for k,v in T.items()})

class Tests(unittest.TestCase):
 def test_relocation(self):
  b,p,m=fixture();v=run(b,p,m)
  self.assertEqual(v['result'],'RELOCATED_REFERENCE_RELATIONS_VERIFIED_READONLY')
  self.assertEqual(v['consensus_shift'],32);self.assertFalse(v['production_permission'])
  self.assertIn('not_zero_shift_capture',v['verification']['selection'])
 def test_independent_noncommon_shifts(self):
  b,p,m=fixture();v=run(b,p,m,{k:v-32-i*16 for i,(k,v) in enumerate(T.items())})
  self.assertEqual(v['result'],'RELOCATED_REFERENCE_RELATIONS_VERIFIED_READONLY');self.assertIsNone(v['consensus_shift'])
 def test_missing_fallback_requires_three_distinct_roles(self):
  b,p,m=fixture();p=p[:-1]
  v=run(b,p,m);self.assertEqual(v['roles']['SetGravityVector']['status'],'fallback_candidate')
  v=run(b,p[:-1],m);self.assertEqual(v['result'],'UNRESOLVED_STOP_REFERENCE_RELOCATION_ROUTE')
 def test_misleading_common_shift_fails_semantics(self):
  b,p,m=fixture();p=p[:-1];b[T['SetGravityVector']-0x1000+0x400+0xc]=0xcc
  self.assertEqual(run(b,p,m)['result'],'SEMANTIC_OR_CALL_BOUNDARY_MISMATCH_STOP')
 def test_ambiguous_no_fallback(self):
  b,p,m=fixture();b[0x500:0x508]=token(0)
  v=run(b,p,m);self.assertTrue(v['ambiguous']);self.assertEqual(v['roles']['UpdateMotion']['status'],'ambiguous')
 def test_no_nearest_entry_snap(self):
  b,p,m=fixture();site=T['UpdateMotion'];off=site-0x1000+0x400
  b[off:off+8]=b'\x90'*8;b[off+0x20:off+0x28]=token(0)
  v=run(b,p,m);self.assertEqual(v['roles']['UpdateMotion']['status'],'boundary_unproven')
 def test_interior_cfg_maps_exact_container(self):
  b,p,m=fixture();start=T['UpdateMotion'];at=start-0x1000+0x400+0x20;b[at:at+8]=token(4)
  p[0]=desc('UpdateMotion','interior',token(4))
  v=run(b,p,m);self.assertEqual(v['roles']['UpdateMotion']['candidate_rva'],start)
  self.assertEqual(v['roles']['UpdateMotion']['source'],'interior_exact_range_CFG')
 def test_interior_immediate_coincidence_not_boundary(self):
  b,p,m=fixture();at=0x420;b[at:at+10]=b'\x48\xb8'+token(4)
  p[0]=desc('UpdateMotion','interior',token(4))
  v=run(b,p,m,{k:v-32-i*16 for i,(k,v) in enumerate(T.items())})
  self.assertEqual(v['roles']['UpdateMotion']['status'],'missing')
 def test_call_and_marker_boundary_mismatch(self):
  b,p,m=fixture();b[0x8e6:0x8eb]=b'\x90'*5
  self.assertEqual(run(b,p,m)['result'],'SEMANTIC_OR_CALL_BOUNDARY_MISMATCH_STOP')
  b,p,m=fixture();b[0xb00+0x72:0xb00+0x77]=b'\xb8\x90\x90\x90\x90'
  self.assertEqual(run(b,p,m)['result'],'SEMANTIC_OR_CALL_BOUNDARY_MISMATCH_STOP')
 def test_wildcard_mask(self):
  b,p,m=fixture();p[0]['length']=12;p[0]['wildcards']=[8,9,10,11]
  p[0]['masked_sha256']=hashlib.sha256(token(0)+b'\0'*4).hexdigest()
  self.assertTrue(run(b,p,m)['verification']['relations_match'])
 def test_range_overflow_and_consensus_tie(self):
  self.assertIsNone(r.consensus(dict(a=1,b=1,c=1,d=2,e=2,f=2)))
  with self.assertRaisesRegex(r.pe.Refusal,'overflow'):r.consensus(dict(a=2**64,b=2**64,c=2**64))
  b,p,m=fixture();p=p[:-1];canon={k:v-32 for k,v in T.items()};canon['SetGravityVector']=2**63
  self.assertEqual(run(b,p,m,canon)['roles']['SetGravityVector']['status'],'fallback_range_rejected')
 def test_partial_leaf_never_votes_and_no_crossing_entry(self):
  b,p,m=fixture();struct.pack_into('<I',b,0x98+112+28,36)
  at=T['SetGravityVector']-0x1000+0x400+0x4e;b[at]=0xc3
  m=[(k,o,n,hashlib.sha256(b'\xc3').hexdigest() if k=='SetGravityVector' and o==0x4e else h) for k,o,n,h in m]
  v=run(b,p,m);self.assertNotIn('SetGravityVector',v['independent_votes'])
  self.assertEqual(v['method_boundary_status'],'PARTIAL_reference_bounded_leaf_candidate')
  b,p,m=fixture();struct.pack_into('<I',b,0x2a00+36,0x2210)
  v=run(b,p,m);self.assertEqual(v['roles']['SetGravityVector']['boundary']['reason'],'leaf_extent_crosses_runtime_entry')
 def test_hash_pe_and_profile(self):
  b,p,m=fixture()
  with self.assertRaisesRegex(r.pe.Refusal,'unsupported_build'):r.resolve(b)
  with self.assertRaises(r.pe.Refusal):run(b[:100],p,m)
  with patch.object(r,'PROFILE_SHA','bad'):
   with self.assertRaisesRegex(r.pe.Refusal,'pattern_profile'):r.resolve(b,expected_sha=hashlib.sha256(b).hexdigest())
 def test_caps(self):
  for name,value,reason in (('MAX_SCAN',0,'scan_cap'),('MAX_SEEDS',0,'seed_work_cap'),('MAX_MATCHES',0,'match_cap'),('MAX_FUNC_BYTES',1,'function_range'),('MAX_FUNCTIONS',0,'function_range'),('MAX_DECODE',0,'decoder_cap'),('MAX_OUTPUT',1,'output_cap')):
   b,p,m=fixture()
   with patch.object(r,name,value):
    with self.assertRaisesRegex(r.pe.Refusal,reason):run(b,p,m)
 def test_exclusive_cli(self):
  b,_,_=fixture()
  with tempfile.TemporaryDirectory() as d:
   f=Path(d)/'game テ.exe';f.write_bytes(b);out=Path(d)/'out.json'
   cmd=[sys.executable,r.__file__,str(f),'--output',str(out)]
   self.assertEqual(subprocess.run(cmd,capture_output=True).returncode,2);old=out.read_bytes()
   self.assertNotEqual(subprocess.run(cmd,capture_output=True).returncode,0);self.assertEqual(out.read_bytes(),old)
   self.assertNotEqual(subprocess.run(cmd+['--shift','32'],capture_output=True).returncode,0)

if __name__=='__main__':unittest.main(verbosity=2)
