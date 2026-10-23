"""Independent owned instruction fixtures; never game-semantic evidence."""
import struct, unittest
from unittest.mock import patch
from review_path_b_layout import WRITER,RETURN
from review_path_b_layout import flow,review,captures,candidates
class Tests(unittest.TestCase):
 def test_direct(self):
  f=flow(bytes.fromhex('8b4204894110c3'),0x1000);s=f['stores'][0]
  self.assertEqual((s['source_kind'],s['source_rdx_offset'],s['destination_rcx_offset']),('RDX_direct',4,16))
 def test_clobber(self):
  f=flow(bytes.fromhex('8b420431c0894110c3'),0x1000)
  self.assertIsNone(f['stores'][0]['source_rdx_offset'])
 def test_branch_join(self):
  f=flow(bytes.fromhex('8b420485c074028b02894110c3'),0x1000)
  self.assertIsNone(f['stores'][0]['source_rdx_offset'])
 def test_caller(self):
  f=flow(bytes.fromhex('4883ec38488d542420e8000000004883c438c3'),0x1000)
  self.assertEqual(f['calls'][0]['arguments']['rdx'],('ptr','rsp',-0x18))
 def test_read_is_not_store(self):
  self.assertEqual(flow(bytes.fromhex('83791000c3'),0x1000)['stores'],[])
 def test_partial(self):
  with self.assertRaises(ValueError):flow(b'\x0f',0x1000)
 def test_hash_reject(self):
  with self.assertRaises(ValueError):captures(['PATH_B_FUNCTION role=writer begin_rva=0x2cfc3c0 size=444 sha256=bad authenticated=1 original_before_hook=1'])
 def test_simd_and_call(self):
  f=flow(bytes.fromhex('0f1042100f114110c3'),0x1000);self.assertEqual(f['stores'][0]['source_rdx_offset'],16)
  f=flow(bytes.fromhex('8b4204e800000000894110c3'),0x1000);self.assertEqual(f['stores'],[])
 def test_candidates(self):
  e={'raw_source_0x80':struct.pack('<32f',*([0,1,0,0]*8)).hex(),'entry_id':'1','session':'1','N398':'(0,0,1)','hero_up_before':'(1,0,0)'}
  c=candidates(e);self.assertEqual(len(c['finite_scalars']),32)
  self.assertEqual(c['vectors'][31]['cosines']['WorldUp'],1)
 def anchored(self,change=''):
  raw=bytes.fromhex('8b4204894110c3');capture={'writer':{'raw':raw,'flow':flow(raw,WRITER)}}
  e='RESET_COPY_LAYOUT_ENTRY session=1 entry_id=7 thread_id=10 qpc=100 caller_rva='+hex(RETURN)+' RCX=0x10000 source_readable=1 raw_source_0x80='+bytes(128).hex()
  link='PATH_B_ENTRY_LINK session=1 qpc_ms=1 entry_id=7 entry_qpc=100 thread_id=10 entry_link_checks=1'
  ev='ORIENTATION_WRITER_EVENT writer_id=1 session=1 qpc_ms=1 thread_id=10 transform=0x10000 module_offset_after='+hex(WRITER+6)+' read_valid=1 old_observed_up=(0,0,1) new_observed_up=(0,1,0)'
  g='ORIENTATION_WRITER_REGS writer_id=1 qpc_ms=1 post_gpr_RAX_RCX_RDX_RBX_RSP_RBP_RSI_RDI_R8_R9_R10_R11_R12_R13_R14_R15=0,10000,20000,0,0,0,0,0,0,0,0,0,0,0,0,0'
  if change=='address':g=g.replace('0,10000,','0,11000,')
  if change=='thread':ev=ev.replace('thread_id=10','thread_id=11')
  if change=='stale':link=link.replace('entry_qpc=100','entry_qpc=99')
  if change=='pc':ev=ev.replace(hex(WRITER+6),hex(WRITER+5))
  with patch('review_path_b_layout.captures',return_value=capture):return review('\n'.join([e,link,ev,g]),'A')
 def test_anchor(self):
  r=self.anchored();self.assertEqual(len(r['stores']),1);self.assertEqual(r['stores'][0]['source_rdx_offset'],4);self.assertFalse(r['PATH_B_WRITE_PERMISSION'])
 def test_anchor_negatives(self):
  for why in ('address','thread','stale','pc'):
   with self.subTest(why=why):self.assertEqual(self.anchored(why)['stores'],[])
 def test_missing_evidence(self):
  r=review('', 'A');self.assertFalse(r['PATH_B_WRITE_PERMISSION']);self.assertFalse(r['PATH_B_FIELDS_VERIFIED']);self.assertEqual(r['stores'],[])
if __name__=='__main__':unittest.main()
