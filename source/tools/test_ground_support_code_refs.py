import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch
import ground_support_code_refs as g
from test_discover_ground_support import fixture


def image(name='HeroStateRunLocal', site=0x1000, target=0x2040):
    b = fixture((name,))
    op = 0x98
    struct.pack_into('<I', b, op+108, 16)
    struct.pack_into('<II', b, op+112+24, 0x2600, 12)
    struct.pack_into('<III', b, 0x1200, 0x1000, 0x1080, 0x2640)
    b[0x1240:0x1244] = b'\x01\0\0\0'
    b[0x400:0x480] = b'\x90'*0x80
    off = 0x400+site-0x1000
    b[off:off+7] = b'\x48\x8d\x0d'+struct.pack('<i',target-site-7)
    b[off+7] = 0xc3
    return b


def run(b):
    return g.investigate(b, hashlib.sha256(b).hexdigest())


class Tests(unittest.TestCase):
    def test_entry_reference(self):
        r = run(image()); a = r['references'][0]
        self.assertEqual(a['boundary_proof'],'runtime_entry_straight_line_boundary')
        self.assertEqual(a['name_rva'],0x2040)
        self.assertEqual(r['functions'][0]['begin_rva'],0x1000)
        self.assertEqual(r['code_bytes_exported'],8)
        self.assertFalse(r['first_consumer_authenticated'])
        self.assertIsNone(r['selected_controller'])

    def test_linear_boundary(self):
        self.assertEqual(run(image(site=0x1005))['straight_line_boundary_refs'],1)

    def test_interior_byte_coincidence(self):
        b = image(site=0x1002)
        b[0x400:0x402] = b'\x48\xb8' # MOV RAX, imm64; embedded LEA bytes not entry.
        self.assertEqual(run(b)['references'],[])

    def test_prior_branch_not_boundary_proof(self):
        b = image(site=0x1004); b[0x400:0x402] = b'\xeb\x02'
        r = run(b)
        self.assertEqual(r['straight_line_boundary_refs'],0)
        self.assertEqual(r['references'][0]['boundary_proof'],'candidate_prior_control_boundary')

    def test_data_edge(self):
        b = image(target=0x2500)
        struct.pack_into('<Q',b,0x1100,0x140002040)
        r = run(b)
        self.assertEqual(r['references'][0]['relation'],'va64_word_to_name')
        self.assertFalse(r['registry_authenticated'])

    def test_unrelated_mount_bot_excluded(self):
        for name in ('HeroStateSprintMount','HeroStateRunBot','HeroStateRunRemote','BotHeroStateRun','SprintAnimDriver'):
            self.assertEqual(run(image(name))['references'],[],name)

    def test_lexical_family_and_secondary(self):
        for name in ('HeroStateRunMoveLocal','HeroStateSprint','HeroStateMoveLocal','HeroStateLocomotion','HeroStateGround','AutoUpdateGroundState','MoveAccelSprint'):
            self.assertTrue(run(image(name))['references'],name)
        self.assertEqual(g.rank('MoveAccelSprint')[0],2)

    def test_ambiguity_and_ranking(self):
        b = image(); b[0xd40:0xd40+len(b'HeroStateMoveLocal')] = b'HeroStateMoveLocal'
        r = run(b)
        self.assertEqual(len(r['selected_targets']),2)
        self.assertTrue(r['selection_ambiguous']); self.assertIsNone(r['selected_controller'])
        with patch.object(g,'MAX_TARGETS',1):
            r = run(b); self.assertEqual(r['omitted_ranked_names'],1)

    def test_hash_and_malformed(self):
        with self.assertRaisesRegex(g.meta.Refusal,'unsupported_build'): g.investigate(image())
        for data in (b'',b'MZ',image()[:512]):
            with self.assertRaises(g.meta.Refusal):run(data)

    def test_runtime_metadata_errors(self):
        for off,value,reason in ((0x98+108,0,'directory'),(0x98+112+28,13,'runtime_table'),(0x1204,0x1000,'runtime_table'),(0x1208,0x2641,'runtime_table')):
            b = image(); struct.pack_into('<I',b,off,value)
            with self.assertRaisesRegex(g.meta.Refusal,reason):run(b)

    def test_no_runtime_coverage(self):
        b = image();struct.pack_into('<II',b,0x1200,0x1010,0x1080)
        r = run(b)
        self.assertEqual(r['references'][0]['boundary_proof'],'candidate_no_runtime_coverage')
        self.assertEqual(r['functions'],[])

    def test_truncated_reference_at_section_end(self):
        b = image(); b[0x400:0x480] = b'\x90'*128
        b[0xbfd:0xc00] = b'\x48\x8d\x0d'
        self.assertEqual(run(b)['references'],[])

    def test_wrong_reference_target(self):
        self.assertEqual(run(image(target=0x2345))['references'],[])

    def test_all_caps(self):
        for attr,value,reason in (('MAX_NAMES',0,'name_cap'),('MAX_DATA_REFS',0,'data_reference_cap'),
                                 ('MAX_CODE_SCAN',1,'code_scan_cap'),('MAX_PREFILTER',0,'prefilter_work_cap'),
                                 ('MAX_XREFS',0,'instruction_reference_cap'),('MAX_FUNCTIONS',0,'function_cap'),
                                 ('MAX_FUNCTION_HASH_BYTES',0,'function_hash_byte_cap'),
                                 ('MAX_EVIDENCE_BYTES',0,'exported_code_cap'),('MAX_OUTPUT',1,'output_cap')):
            b = image();struct.pack_into('<Q',b,0x1100,0x140002040)
            with patch.object(g,attr,value):
                with self.assertRaisesRegex(g.meta.Refusal,reason):run(b)

    def test_boundary_caps_downgrade(self):
        with patch.object(g,'MAX_BOUNDARY_INSNS',0):
            self.assertEqual(run(image())['straight_line_boundary_refs'],0)
        with patch.object(g,'MAX_PREFIX_BYTES',0):
            self.assertEqual(run(image())['straight_line_boundary_refs'],0)

    def test_decoder_version(self):
        with patch.object(g.cs,'__version__','wrong'):
            with self.assertRaisesRegex(g.meta.Refusal,'Capstone'):run(image())

    def test_first_local_memory_followup(self):
        b = image();b[0x407:0x40c] = b'\x48\x8b\x41\x10\xc3'
        r = run(b)
        self.assertEqual(r['references'][0]['first_local_followup']['kind'], 'subsequent_memory_operand_not_owner_field_proof')
        self.assertEqual(r['references'][0]['first_local_followup']['operands'][0]['displacement'],16)
        self.assertFalse(r['first_consumer_authenticated'])

    def test_first_local_direct_call(self):
        b = image();b[0x407:0x40c] = b'\xe8'+struct.pack('<i',0x1000-0x100c)
        r = run(b)
        c = r['references'][0]['first_local_followup']
        self.assertTrue(c['exact_runtime_entry']);self.assertEqual(c['target_rva'],0x1000)

    def test_tiny_function_not_dumped(self):
        b = image();struct.pack_into('<I',b,0x1204,0x1008)
        r = run(b);a = r['references'][0]
        self.assertTrue(a['full_body_byte_dump_avoided'])
        self.assertEqual(len(a['evidence']),1)
        self.assertEqual(a['evidence'][0]['mnemonic'],'lea')
        self.assertEqual(r['code_bytes_exported'],7)

    def test_exclusive_cli_output(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp)/'space テ';root.mkdir()
            exe=root/'game.exe';exe.write_bytes(image());out=root/'out.json'
            cmd=[sys.executable,g.__file__,str(exe),'--output',str(out)]
            self.assertEqual(subprocess.run(cmd,capture_output=True).returncode,2)
            before=out.read_bytes()
            self.assertEqual(json.loads(before)['reason'],'unsupported_build_sha256')
            self.assertNotEqual(subprocess.run(cmd,capture_output=True).returncode,0)
            self.assertEqual(out.read_bytes(),before);self.assertEqual(exe.read_bytes(),image())

if __name__=='__main__':unittest.main(verbosity=2)
