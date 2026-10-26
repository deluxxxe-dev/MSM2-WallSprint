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
