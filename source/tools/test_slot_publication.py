"""Owned instruction buffers only; no game image/value input."""
import struct
from slot_publication import scan_code, publication_verdict, analyze_image, parse_pe, SLOT

def instruction(opcode,at,target=SLOT):
    return bytes.fromhex(opcode)+struct.pack('<i',target-(at+7))

a=scan_code(instruction('488b05',0x1000)+instruction('488905',0x1007),0x1000)
assert len(a['references'])==2 and not a['references'][0]['may_write'] and a['references'][1]['may_write']
print('PASS: c37 exact-slot RIP reader/writer candidates have exact source kind/width/access without reading qword contents')
a=scan_code(instruction('488905',0x1000)+instruction('48890d',0x1007),0x1000)
v=publication_verdict([a],True)
assert v['direct_writer_candidates']==2 and not v['base_proven'] and 'ambiguous' in v['reason']
print('PASS: c37 multiple writer candidates refuse publication source/allocation/lifetime proof')
a=scan_code(bytes.fromhex('488901'),0x1000);v=publication_verdict([a],True)
assert a['unresolved_memory_operands']==1 and not v['writer_enumeration_complete']
print('PASS: c37 indirect writes cannot be enumerated by exact direct references and keep completeness false even with a clean sweep')
a=scan_code(instruction('488905',0x1000)+b'\x90'*30,0x1000,max_bytes=7)
v=publication_verdict([a],False)
assert a['capped'] and not v['writer_enumeration_complete'] and not v['destination_contained']
print('PASS: c37 bounded/incomplete xref coverage refuses rather than claiming all writers or destination containment')
a=scan_code(instruction('488905',0x1000),0x1000);v=publication_verdict([a],True)
assert all(v[x] is False for x in ['base_proven','allocation_proven','range_proven','lifetime_proven','CFG_continuation'])
print('PASS: c37 one direct writer and complete candidate sweep still lack allocation/range/lifetime/indirect-writer proof')
try: analyze_image(b'owned_not_a_game_image')
except ValueError as e: assert 'SHA256' in str(e)
else: raise AssertionError('unverified image accepted')
print('PASS: c37 CLI image entry requires exact authenticated executable hash, no fixture/hash override or live-memory input')
a=scan_code(instruction('488905',0x1000,SLOT+8),0x1000)
b=scan_code(instruction('488905',0x1000,SLOT-4),0x1000)
assert not a['references'] and len(b['references'])==1
print('PASS: c37 exact eight-byte slot overlap excludes adjacent storage while recognizing overlapping bounded writer candidates')
# Authenticated-image parser excludes the full runtime span of the deferred producer.
owned=bytearray(1052)
struct.pack_into('<H',owned,0,0x5a4d);struct.pack_into('<I',owned,60,0x80);owned[0x80:0x84]=b'PE\0\0'
struct.pack_into('<HH',owned,0x84,0x8664,2);struct.pack_into('<H',owned,0x94,240)
struct.pack_into('<H',owned,0x98,0x20b);struct.pack_into('<Q',owned,0x98+24,0x140000000)
struct.pack_into('<I',owned,0x98+56,0xd000000);struct.pack_into('<II',owned,0x98+112+24,0x2000,12)
struct.pack_into('<IIII',owned,0x188+8,8192,0x351e000,16,1024);struct.pack_into('<I',owned,0x188+36,0x60000020)
struct.pack_into('<IIII',owned,0x188+40+8,12,0x2000,12,1040);struct.pack_into('<I',owned,0x188+40+36,0x40000040)
struct.pack_into('<III',owned,1040,0x351eaf0,0x351eb80,0x2100)
assert parse_pe(bytes(owned))[2]==[(0x351eaf0,0x351eb80)]
a=scan_code(instruction('488905',0x1000)*4,0x1000,max_refs=1)
assert len(a['references'])<=1
v=publication_verdict([a],False)
assert v['live_qword_reads']==0 and not v['pointer_chase'] and not v['writer_enumeration_complete']
print('PASS: c37 deferred-producer runtime exclusion and reference cap/metadata never exports bytes/immediates/live pointers or promotes base/ownership from addressing form')

# New owned local register-affine fixtures; no actual parent body is copied.
from slot_publication import external_anchor_record, ANCHOR_SITE, ANCHOR_ORIGIN, decoder
import slot_publication as slot_module

def lea(at,target,opcode='488d0d'):
    return bytes.fromhex(opcode)+struct.pack('<i',target-(at+7))

def affine_fixture(tail,origin=SLOT-0x100):
    return lea(0x1000,origin)+bytes.fromhex(tail)

a=scan_code(affine_fixture('488b8100010000'),0x1000)
assert len(a['references'])==1 and a['references'][0]['effective_rva']==SLOT and a['references'][0]['address_proof_class']=='register_affine_static' and not a['references'][0]['may_write']
print('PASS: c38 owned RIP-LEA parent origin plus RCX+100 resolves exact slot reader without dereferencing memory or hardcoding a local game result')
a=scan_code(affine_fixture('48898100010000'),0x1000)
assert len(a['references'])==1 and a['references'][0]['may_write']
v=publication_verdict([a],True)
assert v['register_affine_writer_candidates']==1 and v['direct_writer_candidates']==0 and not v['writer_enumeration_complete'] and not v['base_proven']
print('PASS: c38 exact register-affine slot writer is counted separately from direct candidates and never grants writer completeness or allocation')
a=scan_code(affine_fixture('4889ca488b8200010000'),0x1000)
assert a['references'][0]['address_proof_class']=='register_affine_static' and a['references'][0]['effective_rva']==SLOT
print('PASS: c38 full64 register copy preserves only the proven exact image origin')
a=scan_code(affine_fixture('488d9180000000488b4280',origin=SLOT),0x1000)
assert a['references'][-1]['effective_rva']==SLOT  # signed -128 from origin+128
assert not a['references'][0]['may_write'] and a['references'][0]['reference_kind']=='address_only'
b=scan_code(affine_fixture('488d9180000000488b8280000000'),0x1000)
assert b['references'][0]['effective_rva']==SLOT
print('PASS: c38 checked signed affine LEA and displacement compute full exact slot overlap without memory-value propagation')
a=scan_code(affine_fixture('488b8108010000'),0x1000)
assert not a['references'] and a['register_affine_resolved_operands']>=1
b=scan_code(lea(0x1000,0x10)+bytes.fromhex('488d8900000080488b01'),0x1000)
assert not b['references'] and b['unresolved_memory_operands']
print('PASS: c38 adjacent/outside-image/underflow affine addresses cannot become slot candidates or wrapped image addresses')
a=scan_code(affine_fixture('488b8900010000488b01'),0x1000)
assert len(a['references'])==1 and a['unresolved_reasons'].get('memory_loaded_base')==1
print('PASS: c38 slot memory load invalidates destination origin, so a later dereference stays unknown and no loaded qword is treated as a pointer')
for mutation in ['b900000000','66b90000','b101']:
    a=scan_code(affine_fixture(mutation+'488b8100010000'),0x1000)
    assert not a['references'] and a['unresolved_reasons'].get('partial_register_mutation')==1
print('PASS: c38 32/16/8bit writes invalidate the full address parent rather than preserving a stale64bit origin')
for mutation in ['4883c101','48c1e106','48b980252d4c01000000']:
    a=scan_code(affine_fixture(mutation+'488b8100010000'),0x1000)
    assert not a['references'] and a['unresolved_reasons'].get('unsupported_register_mutation')==1
print('PASS: c38 unsupported arithmetic/shift/absolute immediate materialization cannot infer image origins or relocation semantics')
for boundary in ['7500','e800000000','ffe0','ffd0','c3','0f05']:
    a=scan_code(affine_fixture(boundary+'488b8100010000'),0x1000)
    assert not a['references'] and a['unresolved_reasons'].get('control_flow_boundary_loss')==1
print('PASS: c38 conditional/direct/indirect/call/return/system boundary loses every origin without ABI-preservation or path assumptions')
a=scan_code(affine_fixture('488b84010001000064488b8100010000'),0x1000)
assert not a['references'] and a['unresolved_reasons'].get('indexed_address')==1 and a['unresolved_reasons'].get('segment_unsupported')==1
print('PASS: c38 indexed and segment addressing remain separate unresolved classes rather than guessed affine/value expressions')
a=scan_code(affine_fixture('9090488b8100010000'),0x1000,max_state_insns=2)
b=scan_code(affine_fixture('488b8100010000'),0x1000,reset_points=[0x1007])
c=scan_code(affine_fixture('4889ca488b8200010000'),0x1000,max_hops=1)
assert not a['references'] and a['unresolved_reasons'].get('provenance_window_exhausted')
assert not b['references'] and b['unresolved_reasons'].get('runtime_region_boundary_loss')
assert not c['references'] and c['unresolved_reasons'].get('provenance_hop_cap')
raw=affine_fixture('0f0b488b8100010000');d=scan_code(raw,0x1000)
assert not d['references']
v=publication_verdict([a,b,c],True)
assert not v['candidate_sweep_complete'] and not v['writer_enumeration_complete']
print('PASS: c38 state-window/hop/runtime-region/decode-control boundaries and byte coverage never become all-entry or all-writer proof')
code=bytes.fromhex('488b8100010000')
record=external_anchor_record(code,0x140000000,0xd000000)
assert record and record['site_rva']==ANCHOR_SITE and record['origin_rva']==ANCHOR_ORIGIN and record['effective_rva']==SLOT
assert record['address_proof_class']=='authenticated_external_anchor' and not record['local_scanner_coverage']
assert not scan_code(code,ANCHOR_SITE)['references']
assert external_anchor_record(bytes.fromhex('488b8108010000'),0x140000000,0xd000000) is None
old=slot_module.cs.__version__
try:
    slot_module.cs.__version__='5.0.5'
    try: decoder()
    except ValueError: pass
    else: raise AssertionError('unpinned decoder accepted')
finally: slot_module.cs.__version__=old
# Owned PE integration: anchor is independently decoded within the aggregate
# budgets, excluded producer bytes contain a synthetic writer that must not appear.
image=bytearray(1024+8192+8192+24+4096)
struct.pack_into('<H',image,0,0x5a4d);struct.pack_into('<I',image,60,0x80);image[0x80:0x84]=b'PE\0\0'
struct.pack_into('<HH',image,0x84,0x8664,4);struct.pack_into('<H',image,0x94,240);struct.pack_into('<H',image,0x98,0x20b)
struct.pack_into('<Q',image,0x98+24,0x140000000);struct.pack_into('<I',image,0x98+56,0xd000000)
struct.pack_into('<II',image,0x98+112+24,0x2000,24)
regions=[(0x2cf7000,8192,1024,0x60000020),(0x351e000,8192,9216,0x60000020),(0x2000,24,17408,0x40000040),(0xc2d2000,4096,17432,0xc0000040)]
for index,(begin,size,raw,flags) in enumerate(regions):
    struct.pack_into('<IIII',image,0x188+index*40+8,size,begin,size,raw);struct.pack_into('<I',image,0x188+index*40+36,flags)
image[1024:17408]=b'\x90'*16384
struct.pack_into('<IIIIII',image,17408,0x2cf7e00,0x2cf7e20,0x2100,0x351eaf0,0x351eb80,0x2100)
image[1024+ANCHOR_SITE-0x2cf7000:1024+ANCHOR_SITE-0x2cf7000+7]=code
hidden=lea(0x351eaf0,SLOT-0x100)+bytes.fromhex('48898100010000')
image[9216+0xaf0:9216+0xaf0+len(hidden)]=hidden
previous=(slot_module.EXPECTED_SHA,slot_module.MAX_SCAN,slot_module.MAX_INSNS)
try:
    import hashlib
    slot_module.EXPECTED_SHA=hashlib.sha256(image).hexdigest()  # owned test only, no CLI override
    slot_module.MAX_SCAN=16;slot_module.MAX_INSNS=10
    report=analyze_image(bytes(image))
    assert report['tool_version']=='v0.1c38' and report['schema_version']==2
    assert report['bytes_scanned']<=16 and report['instructions_decoded']<=10
    assert len(report['external_anchors'])==1 and not report['reference_candidates']
    assert report['anchor_bytes_within_total_cap']==7 and report['anchor_instructions_within_total_cap']==1
    slot_module.MAX_SCAN=32768;slot_module.MAX_INSNS=30000
    report=analyze_image(bytes(image))
    assert not report['reference_candidates'] and report['excluded_deferred_regions']==[(0x351eaf0,0x351eb80)]
    assert not report['candidate_sweep_complete'] and not report['writer_enumeration_complete']
    assert sum(report['unresolved_reasons'].values())==report['indirect_or_unresolved_operands']
finally:
    slot_module.EXPECTED_SHA,slot_module.MAX_SCAN,slot_module.MAX_INSNS=previous
print('PASS: c38 current matching c28 consumer plus accepted parent anchor is explicit external evidence, never injected as local coverage; mismatches and unpinned decoder refuse')
