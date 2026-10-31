"""Synthetic PE metadata tests; never claim real game/controller evidence."""
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch
import discover_ground_support as d


def fixture(names=('HeroStateSprint',)):
    b = bytearray(0x1400)
    b[:2] = b'MZ'; struct.pack_into('<I', b, 60, 0x80)
    b[0x80:0x84] = b'PE\0\0'
    struct.pack_into('<HH', b, 0x84, 0x8664, 2)
    struct.pack_into('<H', b, 0x94, 240)
    op = 0x98
    struct.pack_into('<H', b, op, 0x20b)
    struct.pack_into('<Q', b, op+24, 0x140000000)
    struct.pack_into('<II', b, op+56, 0x4000, 0x400)
    for i, (rva, raw, flags) in enumerate(((0x1000,0x400,0x60000020), (0x2000,0xc00,0x40000040))):
        at = op+240+40*i
        struct.pack_into('<IIII', b, at+8, 0x800, rva, 0x800, raw)
        struct.pack_into('<I', b, at+36, flags)
    for i, name in enumerate(names):
        raw = 0xc40+i*0x100
        b[raw:raw+len(name)] = name.encode()
    return b


def run(b):
    return d.discover(b, hashlib.sha256(b).hexdigest())


class DiscoveryTests(unittest.TestCase):
    def test_single_candidate_not_consumer(self):
        r = run(fixture()); self.assertEqual(r['result'], 'CANDIDATE_ONLY')
        self.assertIsNone(r['selected_consumer']); self.assertFalse(r['ownership_proven'])
        self.assertFalse(r['code_evidence_exported']); self.assertNotIn('hex', json.dumps(r))

    def test_rtti_and_reference_candidates(self):
        b = fixture(('.?AVHeroStateSprint@@',))
        struct.pack_into('<QQ', b, 0xc30, 0x140002700, 0)
        struct.pack_into('<I', b, 0xf00, 0x2030)
        struct.pack_into('<Q', b, 0xf08, 0x140002040)
        r = run(b)
        self.assertEqual(r['anchors'][0]['type_descriptor_candidate_rva'], 0x2030)
        self.assertEqual(len(r['metadata_reference_candidates']), 2)
        self.assertFalse(r['unique_consumer_authenticated'])

    def test_utf16(self):
        b = fixture(())
        name = 'HeroGroundLocomotion'.encode('utf-16le')
        b[0xc40:0xc40+len(name)] = name
        self.assertEqual(run(b)['anchors'][0]['encoding'], 'utf16le')

    def test_ambiguity_never_selects(self):
        r = run(fixture(('Sprint', 'SprintOther')))
        self.assertEqual(r['result'], 'AMBIGUOUS_CANDIDATES_ONLY')
        self.assertIsNone(r['selected_consumer'])

    def test_no_match(self):
        self.assertEqual(run(fixture(('Unrelated',)))['result'], 'NO_ANCHOR_MATCH')

    def test_exec_strings_ignored(self):
        b = fixture(())
        b[0x450:0x456] = b'Sprint'
        self.assertEqual(run(b)['anchors'], [])

    def test_writable_metadata_ignored(self):
        b = fixture(); struct.pack_into('<I', b, 0x98+240+40+36, 0xc0000040)
        self.assertEqual(run(b)['anchors'], [])

    def test_unsupported_hash(self):
        with self.assertRaisesRegex(d.Refusal, 'unsupported_build'):
            d.discover(fixture())

    def test_malformed_and_truncated(self):
        for b in (b'', b'MZ', fixture()[:0x200]):
            with self.assertRaises(d.Refusal): run(b)

    def test_raw_range(self):
        b = fixture(); struct.pack_into('<I', b, 0x98+240+40+20, len(b)-1)
        with self.assertRaisesRegex(d.Refusal, 'raw_range'): run(b)

    def test_virtual_range(self):
        b = fixture(); struct.pack_into('<I', b, 0x98+240+40+12, 0x4000)
        with self.assertRaisesRegex(d.Refusal, 'virtual_range'): run(b)

    def test_overlaps(self):
        for field, value, why in ((12,0x1000,'virtual'), (20,0x400,'raw')):
            b = fixture(); struct.pack_into('<I', b, 0x98+240+40+field, value)
            with self.assertRaisesRegex(d.Refusal, 'overlapping_'+why): run(b)

    def test_no_interior_or_unterminated_text(self):
        b = fixture(())
        b[0xc40:0xc40+210] = b'X'*100 + b'Sprint' + b'X'*104
        self.assertEqual(run(b)['anchors'], [])

    def test_name_cap(self):
        with patch.object(d, 'MAX_NAMES', 1):
            with self.assertRaisesRegex(d.Refusal, 'name_candidate_cap'):
                run(fixture(('Sprint', 'SprintOther')))

    def test_reference_cap(self):
        b = fixture(); struct.pack_into('<I', b, 0xf00, 0x2040)
        with patch.object(d, 'MAX_REFS', 0):
            with self.assertRaisesRegex(d.Refusal, 'metadata_reference_cap'): run(b)

    def test_scan_file_output_caps(self):
        for cap, value, why in (('MAX_FILE', 1, 'file_cap'),
                                ('MAX_METADATA', 1, 'metadata_scan_cap'),
                                ('MAX_OUTPUT', 1, 'output_cap')):
            with patch.object(d, cap, value):
                with self.assertRaisesRegex(d.Refusal, why): run(fixture())

    def test_rtti_header_not_invented(self):
        r = run(fixture(('.?AVHeroSprint@@',)))
        self.assertIsNone(r['anchors'][0]['type_descriptor_candidate_rva'])

    def test_cli_refusal_and_no_overwrite_unicode(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)/'space テ'; root.mkdir()
            exe = root/'game.exe'; exe.write_bytes(fixture())
            out = root/'result.json'
            cmd = [sys.executable, str(Path(d.__file__)), str(exe), '--output', str(out)]
            p = subprocess.run(cmd, capture_output=True)
            self.assertEqual(p.returncode, 2)
            self.assertEqual(json.loads(out.read_text())['reason'], 'unsupported_build_sha256')
            old = out.read_bytes(); original = exe.read_bytes()
            self.assertNotEqual(subprocess.run(cmd, capture_output=True).returncode, 0)
            self.assertEqual(out.read_bytes(), old); self.assertEqual(exe.read_bytes(), original)
            self.assertNotEqual(subprocess.run(cmd+['--expected-sha','0'*64],capture_output=True).returncode, 0)

if __name__ == '__main__':
    unittest.main(verbosity=2)
