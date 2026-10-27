"""Owned byte/PE fixtures only. No game bytes, addresses-as-values or image input."""
from dataclasses import replace
import hashlib
import contextlib
import io
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
from types import SimpleNamespace

import slot_publication_cfg as cfg
import slot_publication as legacy


class Code:
    def __init__(self, rva=0x1000):
        self.rva, self.data, self.labels, self.patches = rva, bytearray(), {}, []

    def emit(self, hex_bytes):
        self.data.extend(bytes.fromhex(hex_bytes))

    def lea(self, target=cfg.SLOT-0x100, opcode='488d0d'):
        at = self.rva + len(self.data)
        self.data.extend(bytes.fromhex(opcode) + struct.pack('<i', target-(at+7)))

    def label(self, name):
        self.labels[name] = self.rva + len(self.data)

    def jump(self, name, conditional=False):
        self.data.extend(bytes.fromhex('0f85' if conditional else 'e9'))
        self.patches.append((len(self.data), name))
        self.data.extend(b'\0'*4)

    def raw(self):
        data = self.data.copy()
        for at, name in self.patches:
            struct.pack_into('<i', data, at, self.labels[name]-(self.rva+at+4))
        return bytes(data)


def run(code, **kwargs):
    return cfg.scan_region(code.raw(), code.rva, **kwargs)


def diamond(mutation='90', same_copy=False, mutate_left=False):
    b = Code(); b.lea(); b.emit('85c0'); b.jump('right', True)
    b.emit('4889ca' if same_copy else mutation if mutate_left else '90'); b.jump('join')
    b.label('right'); b.emit('4889ca' if same_copy else '90' if mutate_left else mutation)
    b.label('join'); b.emit('488b8200010000' if same_copy else '488b8100010000')
    b.emit('48898100010000c3')
    return b


a = run(diamond())
assert a['status'] == 'MODELED_NORMAL_CFG'
assert [r['operation'] for r in a['references']] == ['READ', 'WRITE']
assert a['stats']['previously_control_lost_edges_safely_modeled'] >= 3
assert all(r['cfg']['fixpoint_complete'] and r['address_proof_chain'][0][1] == 'rip_lea' for r in a['references'])
print('PASS: c53 both conditional legs preserve identical origin; post-join reader/writer emitted only after fixpoint')

for mutation in ['b900000000', '4883c108']:
    for left in [False, True]:
        a = run(diamond(mutation, mutate_left=left))
        assert not a['references'] and a['stats']['conflicting_facts_discarded']
print('PASS: c53 missing/conflicting branch facts remove the merged origin, including late predecessor invalidation')

b = diamond(same_copy=True); a = run(b)
assert all(r['operation'] != 'READ' for r in a['references'])  # different copy-site chains do not merge
assert any(r['operation'] == 'WRITE' for r in a['references'])  # original RCX still agrees
print('PASS: c53 equal numeric origins with different derivation chains do not become one must-proven register')

b = Code(); b.lea(); b.jump('read'); b.emit('909090'); b.label('read'); b.emit('488b8100010000c3')
a = run(b); assert len(a['references']) == 1 and a['stats']['direct_branch_edges'] == 1
assert a['stats']['states_discovered'] < a['stats']['instructions_decoded']
print('PASS: c53 unconditional direct jump has target only; unreachable canonical bytes do not become CFG states')

for mutation, expected in [('90', True), ('4883c108', False), ('4889c9', False)]:
    b = Code(); b.lea(); b.label('loop'); b.emit(mutation+'85c0'); b.jump('loop', True)
    b.emit('488b8100010000c3'); a = run(b)
    assert bool(a['references']) is expected
    assert a['stats']['loop_back_edges'] == 1 and a['stats']['state_revisits']
print('PASS: c53 stable loops converge; loop mutation or changing provenance chain loses exactness without unbounded unfolding')

b = Code(); b.lea(opcode='488d3d'); b.emit('e800100000488b8700010000c3')
a = run(b); assert not a['references'] and a['stats']['direct_calls_all_facts_invalidated'] == 1
assert a['unresolved_reasons']['call_clobber_all_address_origins']
a = run(b, call_target_allowed=lambda pc: False)
assert not a['references'] and 'unsupported_call_target_section' in a['stopped_paths']
print('PASS: c53 direct CALL fallthrough kills ALL address facts including nonvolatile registers; no ABI summary')

for ending, reason in [('ffe0', 'indirect_jump'), ('ffd0', 'indirect_call'), ('0f05', 'system_or_unsupported_transfer')]:
    b = Code(); b.lea(); b.emit('48898100010000'+ending)
    a = run(b); assert not a['references'] and reason in a['stopped_paths']
print('PASS: c53 indirect/system transfer rejects the region and discards earlier provisional writer evidence')

for tail, reason in [('e9f5ffffff', 'branch_into_middle_bytes'), ('0f', 'decode_gap'), ('e900100000', 'unsupported_runtime_boundary')]:
    b = Code(); b.lea(); b.emit(tail); a = run(b)
    assert not a['references'] and reason in a['stopped_paths'], a
print('PASS: c53 middle-byte target, undecodable bytes and external runtime edge refuse with no byte resync')

for name in ['bytes', 'instructions', 'edges', 'states', 'iterations', 'blocks', 'references']:
    a = run(diamond(), caps=replace(cfg.Caps(), **{name: 1}))
    assert not a['references'] and any('cap' in r for r in a['stopped_paths']), (name, a)
    assert a['writer_enumeration_complete'] is False
print('PASS: c53 independent byte/instruction/edge/state/iteration/block/reference caps discard incomplete-region candidates')

b = Code(); b.lea(); b.emit('4889ca488b8200010000c3')
a = run(b, caps=replace(cfg.Caps(), hops=1)); assert not a['references']
assert a['unresolved_reasons']['provenance_hop_cap']
print('PASS: c53 per-register provenance hop cap refuses an otherwise exact copy chain')

for target, count in [(cfg.SLOT+8, 0), (cfg.SLOT-8, 0), (cfg.SLOT-4, 1)]:
    b = Code(); b.lea(target); b.emit('488901c3'); a = run(b)
    refs = [r for r in a['references'] if r['operation'] == 'WRITE']
    assert len(refs) == count
    if count: assert refs[0]['overlap']['bytes'] == 4
print('PASS: c53 exact overlap includes only bounded overlapping bytes and excludes adjacent storage')

b = Code(); b.lea(); b.emit('488b8900010000488b01c3'); a = run(b)
assert len(a['references']) == 1 and a['references'][0]['operation'] == 'READ'
assert a['unresolved_reasons']['memory_loaded_base'] == 1
print('PASS: c53 reading the addressed slot never supplies pointer provenance for a subsequent dereference')

for mutation in ['b900000000', '66b90000', 'b100', '4883c101', '48c1e106', '48b980252d4c01000000']:
    b = Code(); b.lea(); b.emit(mutation+'488b8100010000c3'); a = run(b)
    assert not a['references']
print('PASS: c53 partial writes, arithmetic, shifts and immediate pointer materialization destroy origins')

b = Code(); b.lea(); b.emit('488b84010001000064488b8100010000c3'); a = run(b)
assert not a['references'] and a['unresolved_reasons']['indexed_address'] == 1 and a['unresolved_reasons']['segment_unsupported'] == 1
print('PASS: c53 indexed and segment memory addressing remain explicitly unresolved')

b = Code(); b.lea(cfg.SLOT); b.emit('480101c5fc1101c3'); a = run(b)
assert [r['operation'] for r in a['references']] == ['ADDRESS_ONLY', 'RMW', 'WRITE']
assert a['references'][0]['access_width'] == 0 and a['references'][2]['access_proof'] == 'encoding_verified_move_direction'
store = next(legacy.decoder().disasm(bytes.fromhex('c5fc1101'), 0x1000))
fake = SimpleNamespace(mnemonic=store.mnemonic, id=store.id, bytes=store.bytes,
                       operands=[SimpleNamespace(access=1), SimpleNamespace()])
assert cfg.access_kind(fake, 0)[0] == 'WRITE'
print('PASS: c53 address-only/RMW/AVX store classes are separate; encoding proves WRITE even with erroneous decoder READ')

b = Code(); b.lea(cfg.SLOT); b.emit('0f1801c3'); a = run(b)
assert all(r['operation'] == 'ADDRESS_ONLY' for r in a['references'])
assert a['unresolved_reasons']['unsupported_access_semantics_possible_write']
print('PASS: c53 unsupported access semantics are withheld as possible writes rather than downgraded to READ')

raw = bytes.fromhex('488b8100010000c3')
assert not cfg.scan_region(raw, legacy.ANCHOR_SITE)['references']
assert legacy.external_anchor_record(raw[:7], 0x140000000, 0xD000000)['local_scanner_coverage'] is False
print('PASS: c53 external c28 parent anchor is never injected as a local CFG entry fact')

b = Code(cfg.DEFERRED-8); b.lea(); b.emit('48898100010000c3')
a = run(b, excluded=[(cfg.DEFERRED-16, cfg.DEFERRED+64)])
assert not a['references'] and a['stats'].get('instructions_decoded', 0) == 0
assert 'deferred_region_excluded' in a['stopped_paths']
print('PASS: c53 overlapping deferred span is excluded BEFORE decoding any prefix or creating state')


def owned_pe(with_anchor=False):
    code = diamond().raw()
    # Four sections with real names; no raw game body/value is included.
    specs = [(b'.text', 0x1000, code, 0x60000020),
             (b'.defer', cfg.DEFERRED-16, bytes.fromhex('48890500000000c3')+b'\x90'*24, 0x60000020),
             (b'.pdata', 0x2000, struct.pack('<IIIIII', 0x1000, 0x1000+len(code), 0x2100, cfg.DEFERRED-16, cfg.DEFERRED+16, 0x2100), 0x40000040),
             (b'.data', cfg.SLOT & ~0xfff, b'\0'*4096, 0xc0000040)]
    if with_anchor:
        specs.append((b'.anchor', legacy.ANCHOR_SITE, bytes.fromhex('488b8100010000c3'), 0x60000020))
    data = bytearray(1024)
    struct.pack_into('<H', data, 0, 0x5a4d); struct.pack_into('<I', data, 60, 0x80); data[0x80:0x84] = b'PE\0\0'
    struct.pack_into('<HH', data, 0x84, 0x8664, len(specs)); struct.pack_into('<H', data, 0x94, 240)
    struct.pack_into('<H', data, 0x98, 0x20b); struct.pack_into('<Q', data, 0x98+24, 0x140000000)
    struct.pack_into('<I', data, 0x98+56, 0xD000000); struct.pack_into('<II', data, 0x98+112+24, 0x2000, 24)
    for i, (name, at, raw, flags) in enumerate(specs):
        header = 0x188+40*i; data[header:header+len(name)] = name
        struct.pack_into('<IIII', data, header+8, len(raw), at, len(raw), len(data))
        struct.pack_into('<I', data, header+36, flags); data.extend(raw)
    return bytes(data)


raw = owned_pe(); previous = cfg.EXPECTED_SHA
try:
    cfg.EXPECTED_SHA = hashlib.sha256(raw).hexdigest()  # Pure unit seam only, NO CLI switch.
    a = cfg.analyze_image(raw)
    assert a['reader_candidates'] == a['writer_candidates'] == 1
    assert a['runtime_functions_in_metadata'] == a['coverage']['regions_considered'] == 2
    assert a['coverage']['regions_modeled'] == 1 and a['stopped_paths']['deferred_region_excluded'] == 1
    assert a['excluded_deferred_regions'] == [(cfg.DEFERRED-16, cfg.DEFERRED+16)]
    assert all(r['section']['name'] == '.text' and r['runtime_function']['unwind_rva'] == 0x2100 for r in a['reference_candidates'])
    assert a['live_qword_reads'] == 0 and not a['pointer_chase'] and not a['writer_enumeration_complete']
    assert all(not a[k] for k in ['base_proven','allocation_proven','lifetime_proven','destination_contained','hardware_watch_enabled','game_used'])
    assert a == cfg.analyze_image(raw)
    b = cfg.analyze_image(raw, replace(cfg.Caps(), regions=1))
    assert 'global_region_cap' in b['stopped_paths'] and not b['candidate_sweep_complete']
finally:
    cfg.EXPECTED_SHA = previous
print('PASS: c53 owned PE integration authenticates runtime/section metadata, excludes producer, is deterministic and never certifies publication')

try:
    anchored = owned_pe(True); cfg.EXPECTED_SHA = hashlib.sha256(anchored).hexdigest()
    a = cfg.analyze_image(anchored)
    assert len(a['external_anchors']) == 1 and len(a['reference_candidates']) == 2
    anchor = a['external_anchors'][0]
    assert anchor['mnemonic'] == 'mov' and anchor['operation'] == 'READ'
    assert len(anchor['instruction_sha256']) == 64 and anchor['section']['name'] == '.anchor'
    assert anchor['effective_section']['name'] == '.data' and anchor['runtime_function'] is None
    assert anchor['cfg']['entry_rva'] is None and anchor['overlap']['bytes'] == 8
    assert a['coverage']['regions_excluded'] == 1 and not a['anchor_counted_as_local_candidate']
    b = cfg.analyze_image(anchored, replace(cfg.Caps(), bytes=7, instructions=1))
    assert b['coverage']['file_code_bytes_loaded'] == 7 and b['coverage']['instructions_decoded'] == 1
    assert not b['reference_candidates'] and len(b['external_anchors']) == 1
    c = cfg.analyze_image(anchored, replace(cfg.Caps(), references=1))
    assert not c['reference_candidates'] and len(c['external_anchors']) == 1 and 'reference_cap' in c['stopped_paths']
    fallback = bytearray(raw); struct.pack_into('<I', fallback, 0x98+116+24, 12)
    cfg.EXPECTED_SHA = hashlib.sha256(fallback).hexdigest()
    a = cfg.analyze_image(bytes(fallback))
    assert a['excluded_deferred_regions'] == [(cfg.DEFERRED-16, cfg.DEFERRED+16)]
finally:
    cfg.EXPECTED_SHA = previous
print('PASS: c53 image anchor stays external and consumes aggregate caps; missing producer row retains c38 whole-section exclusion')

try:
    changed = bytearray(raw)
    slot_raw = struct.unpack_from('<I', changed, 0x188+3*40+20)[0] + (cfg.SLOT & 0xfff)
    struct.pack_into('<Q', changed, slot_raw, 0xaabbccddeeff1122)
    cfg.EXPECTED_SHA = hashlib.sha256(raw).hexdigest(); a = cfg.analyze_image(raw)
    cfg.EXPECTED_SHA = hashlib.sha256(changed).hexdigest(); b = cfg.analyze_image(bytes(changed))
    a.pop('image_sha256'); b.pop('image_sha256'); assert a == b
    with tempfile.TemporaryDirectory(prefix='c53 owned ') as tmp:
        exe, out = Path(tmp)/'owned image.exe', Path(tmp)/'compact result.json'
        exe.write_bytes(changed)
        args = sys.argv
        try:
            sys.argv = [str(Path(cfg.__file__)), str(exe), '--output', str(out)]
            capture = io.StringIO()
            with contextlib.redirect_stdout(capture): cfg.main()
            assert capture.getvalue() == out.read_text() and len(capture.getvalue().splitlines()) == 1
            assert json.loads(capture.getvalue())['writer_enumeration_complete'] is False
        finally:
            sys.argv = args
finally:
    cfg.EXPECTED_SHA = previous
print('PASS: c53 changing owned slot qword changes only authenticated file hash, not scan facts; owned CLI emits one compact JSON line with spaced paths')

try: cfg.analyze_image(raw)
except ValueError as error: assert 'SHA256' in str(error)
else: raise AssertionError('unauthenticated executable accepted')
version = legacy.cs.__version__
try:
    legacy.cs.__version__ = '5.0.5'
    try: run(diamond())
    except ValueError as error: assert 'pinned' in str(error)
    else: raise AssertionError('unpinned decoder accepted')
finally: legacy.cs.__version__ = version
with tempfile.TemporaryDirectory() as tmp:
    exe, out = Path(tmp)/'owned.exe', Path(tmp)/'result.json'
    exe.write_bytes(raw)
    result = subprocess.run([sys.executable, str(Path(cfg.__file__)), str(exe), '--output', str(out)], capture_output=True, text=True)
    assert result.returncode != 0 and not out.exists() and 'SHA256' in result.stderr
    result = subprocess.run([sys.executable, str(Path(cfg.__file__)), '--help'], capture_output=True, text=True)
    assert result.returncode == 0 and '--output' in result.stdout and '--hash' not in result.stdout
print('PASS: c53 exact-image hash and pinned decoder guard every CLI path; no hash override, process-memory or fixture CLI')
