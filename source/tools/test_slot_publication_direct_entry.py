"""Owned synthetic code/PE, never private image or runtime acceptance."""
import contextlib
from dataclasses import replace
import hashlib
import io
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

import slot_publication_direct_entry as c55

cfg,c54=c55.cfg,c55.c54


class Code:
    def __init__(self,rva=0x1000):self.rva=rva;self.data=bytearray();self.labels={};self.patches=[]
    def emit(self,text):self.data.extend(bytes.fromhex(text))
    def lea(self,target=cfg.SLOT-0x100,op='488d0d'):
        pc=self.rva+len(self.data);self.data.extend(bytes.fromhex(op)+struct.pack('<i',target-pc-7))
    def call(self,target):
        pc=self.rva+len(self.data);self.data.extend(b'\xe8'+struct.pack('<i',target-pc-5))
    def label(self,name):self.labels[name]=self.rva+len(self.data)
    def jump(self,name,conditional=False):
        self.emit('0f85' if conditional else 'e9');self.patches.append((len(self.data),name));self.emit('00000000')
    def raw(self):
        result=self.data.copy()
        for at,name in self.patches:struct.pack_into('<i',result,at,self.labels[name]-self.rva-at-4)
        return bytes(result)


def caller(target=0x2000,tail='488b810001000048898100010000c3'):
    b=Code();b.lea();b.call(target);b.emit(tail);return b


def domain(functions,rows=(),excluded=(),sections=None):
    text=bytearray(b'\xcc'*0x5000)
    for pc,body in functions.items():text[pc-0x1000:pc-0x1000+len(body)]=body
    frozen=bytes(text);reads=[]
    def read(pc,n):
        reads.append((pc,n))
        if not 0x1000<=pc<=pc+n<=0x6000:raise ValueError('owned not backed')
        return frozen[pc-0x1000:pc-0x1000+n]
    sections=sections if sections is not None else [(0x1000,0x6000,0,len(frozen),0x60000020)]
    result=c55.Domain(image_id=c55.digest([frozen.hex(),rows,excluded,sections]),rows=rows,
        sections=sections,image_size=0xD000000,excluded=excluded,read=read)
    result.reads=reads
    return result


def setup(functions,rows=(),caps=c55.Caps(),excluded=(),builder=None):
    b=builder or caller();all_functions=dict(functions);all_functions[b.rva]=b.raw()
    full_rows=[(b.rva,b.rva+len(b.raw()),0x800),*rows]
    return b,c55.Engine(domain(all_functions,full_rows,excluded),caps)


def run(e,b):return cfg.scan_region(b.raw(),b.rva,call_site_preservation=e.for_site)
def first(e):return next(iter(e.cache.values()))
def request(e,b):
    ins=next(i for i in cfg.old.decoder().disasm(b.raw(),b.rva) if i.group(cfg.cs.CS_GRP_CALL))
    return e.get(ins.operands[0].imm,edge=e.edge(ins,b.rva,'authenticated_runtime_caller_normal_CFG',b.rva+len(b.raw())))


d=domain({},[(0x2000,0x2010,1),(0x3000,0x3020,1),(0x3010,0x3030,1)],[(0x4000,0x4010)],
    [(0x1000,0x6000,0,0x3800,0x60000020),(0x7000,0x8000,0,0x1000,0xe0000020)])
for pc,expected in [(0x2000,'exact_runtime_entry'),(0x2001,'inside_unique_runtime_span_nonentry'),
    (0x3011,'overlapping_multiple_runtime_spans'),(0x1100,'executable_file_backed_outside_runtime_metadata'),
    (0x5000,'executable_not_fully_file_backed'),(0x7000,'non_executable_or_writable_section'),
    (-1,'outside_image'),(0xD000000,'outside_image'),(0x4000,'deferred_producer_excluded')]:
    assert d.relation(pc)==expected,(pc,d.relation(pc))
print('PASS: c55 all eight deterministic metadata relations, including overlap/interior/deferred, precede fallback')

b,e=setup({0x2000:bytes.fromhex('b801000000c3')},[(0x2000,0x2006,0x800)])
a=run(e,b);r=e.report()
assert len(a['references'])==2 and r['c54_runtime_entry_summary_admitted']==1 and r['direct_entry_summary_attempted']==0
assert r['c54_runtime_entry_analysis']['summaries'][0]['contract']==c54.CONTRACT
print('PASS: c55 exact metadata entry uses the c54 canonical span/never-written contract, never metadata-free fallback')

b,e=setup({0x2000:bytes.fromhex('b801000000c3')});a=run(e,b);s=first(e)
assert [r['operation'] for r in a['references']]==['READ','WRITE'] and 1 in s.preserved and 0 not in s.preserved
assert s.end is None and s.identity and s.code_hash and s.cfg_hash and len(next(iter(e.cache)))==6
assert a['stats']['preserved_origin_register_facts_across_calls']==1
print('PASS: c55 direct-CALL-anchored metadata-free leaf preserves RCX with exact slot reader/writer and reachable hashes')

c=Code(0x2000);c.emit('85c0');c.jump('end',True);c.emit('b901000000');c.label('end');c.emit('c3')
b,e=setup({0x2000:c.raw()});assert not run(e,b)['references'] and 1 not in first(e).preserved
print('PASS: c55 a write on either reachable branch kills RCX, irrespective of actual branch outcome')

b=Code();b.lea();b.emit('85c0');b.jump('bypass',True);b.call(0x2000);b.jump('join')
b.label('bypass');b.emit('90');b.label('join');b.emit('488b8100010000c3')
for raw,preserve in [(b'\xc3',True),(bytes.fromhex('b901000000c3'),False)]:
    b,e=setup({0x2000:raw},builder=b);a=run(e,b)
    assert bool(a['references']) is preserve
    if not preserve:assert a['stats']['conflicting_facts_discarded']
print('PASS: c55 caller merges remain must joins; a clobbered CALL branch cannot be hidden by a bypass')

c=Code(0x2000);c.emit('85c0');c.jump('other',True);c.emit('c3');c.emit('0f0b');c.label('other');c.emit('90c3')
b,e=setup({0x2000:c.raw()});assert len(run(e,b)['references'])==2 and first(e).returns==2
assert first(e).instructions==5  # unreachable UD2 is not part of an inferred function span
print('PASS: c55 multiple finite RET paths preserve; unreachable gaps are not swept into a fabricated function span')

b,e=setup({0x2000:bytes.fromhex('51b90100000059c3')});assert not run(e,b)['references'] and 1 not in first(e).preserved
print('PASS: c55 write then restore is not never-written preservation')

b,e=setup({0x2000:b'\x90\xc3'},[(0x1fff,0x2002,0x800)])
assert not run(e,b)['references'] and not e.cache
assert e.report()['target_admission_refusal_reasons']['target_metadata_relation:inside_unique_runtime_span_nonentry']
print('PASS: c55 runtime interior direct target stays refused; nearest metadata is never a subentry proof')

for flags,raw_size,pc in [(0xe0000020,0x5000,0x2000),(0x40000040,0x5000,0x2000),(0x60000020,0x100,0x2000)]:
    b=caller(pc);d=domain({0x1000:b.raw(),0x2000:b'\xc3'},[(0x1000,0x1000+len(b.raw()),1)],
        sections=[(0x1000,0x6000,0,raw_size,flags)])
    # Domain refusal must not read target bytes; writable caller is also refused.
    e=c55.Engine(d);assert not run(e,b)['references'] and all(not pc<=at<pc+1 for at,_ in d.reads)
b,e=setup({},builder=caller(0xD000001));assert not run(e,b)['references']
print('PASS: c55 writable/nonexec/unbacked/outside-image domains refuse without decoding the target')

for destination,rows in [(0x7000,()),(0x3000,((0x3000,0x3001,1),))]:
    body=b'\xe9'+struct.pack('<i',destination-0x2005)
    b,e=setup({0x2000:body,0x3000:b'\xc3'},rows);assert not run(e,b)['references']
    assert first(e).status=='REFUSED'
print('PASS: c55 normal branches into unmapped or runtime-owned domains refuse instead of becoming tail/secondary entries')

for body in ['488d0d00000000e9f5ffffff','75014889c8c3']:
    b,e=setup({0x2000:bytes.fromhex(body)});assert not run(e,b)['references']
    assert 'conflicting_instruction_intervals' in first(e).reason
print('PASS: c55 branch-into-middle and conflicting decodes refuse regardless of discovery order')

b=Code();b.lea();b.call(0x2000);b.call(0x2001);b.emit('488b8100010000c3')
b,e=setup({0x2000:bytes.fromhex('4889c0c3')},builder=b);assert not run(e,b)['references']
assert 'cross_summary_conflicting_decode' in str(e.refusals)
print('PASS: c55 different metadata-free CALL roots cannot establish conflicting overlapping global instruction partitions')

for body in ['ffd0','ffe0','0f05','0f0b','0f31c3','488b0401c3','c20800']:
    b,e=setup({0x2000:bytes.fromhex(body)});assert not run(e,b)['references'] and first(e).status=='REFUSED'
b,e=setup({0x5fff:b'\x0f'},builder=caller(0x5fff));assert not run(e,b)['references']
print('PASS: c55 indirect/system/unknown effect/indexed addressing/adjusted RET and boundary decode gaps refuse')

parent=Code(0x2000);parent.call(0x3000);parent.emit('c3')
for rows in [(),((0x3000,0x3001,1),),((0x2000,0x2006,1),)]:
    b,e=setup({0x2000:parent.raw(),0x3000:b'\xc3'},rows);assert len(run(e,b)['references'])==2
    if not rows:assert first(e).height in {1,2} and e.metrics['direct_entry_nested_summaries_used']==1
    else:assert e.report()['c54_runtime_entry_summary_admitted']==1
print('PASS: c55 nested direct-entry and exact-runtime children, including runtime parent/direct-entry child, use independent proofs')

b,e=setup({0x2000:parent.raw(),0x3000:bytes.fromhex('b901000000c3')});assert not run(e,b)['references']
b,e=setup({0x2000:parent.raw(),0x3000:bytes.fromhex('ffe0')});assert not run(e,b)['references']
assert 'nested_summary_refused' in str(e.refusals)
print('PASS: c55 child RCX writes kill RCX; refused child refuses the parent without partial preservation')

for body in ['ebfe','e8fbffffffc3']:
    b,e=setup({0x2000:bytes.fromhex(body)});assert not run(e,b)['references'] and e.metrics['direct_entry_cycles_refused']
back=Code(0x3000);back.call(0x2000);back.emit('c3')
for rows in [(),((0x3000,0x3006,1),)]:
    b,e=setup({0x2000:parent.raw(),0x3000:back.raw()},rows);assert not run(e,b)['references']
print('PASS: c55 normal CFG/self/mutual/cross-contract recursive cycles fail closed')

c=Code(0x2000);c.emit('85c0');c.jump('other',True);c.emit('90c3');c.label('other');c.emit('90c3')
for name in ['region_bytes','instructions','states','edges','blocks']:
    b,e=setup({0x2000:c.raw()},caps=replace(c55.Caps(),**{name:1}))
    assert not run(e,b)['references'] and first(e).status=='REFUSED',name
print('PASS: c55 independent decoded-byte/instruction/state/edge/block per-summary caps refuse')

for limits in [dict(depth=1),dict(functions=1),dict(bytes=8),dict(work=10)]:
    b,e=setup({0x2000:parent.raw(),0x3000:b'\xc3'},caps=replace(c55.Caps(),**limits));assert not run(e,b)['references']
    assert e.metrics['direct_entry_work']<=e.caps.work and e.metrics['direct_entry_code_bytes_loaded']<=e.caps.bytes
print('PASS: c55 depth/global entry/byte/work caps refuse and account actual lookahead/edge/recheck reads')

# Populate one runtime and one direct proof; the second direct entry exceeds
# its independent cap, which must close later runtime cache requests as well.
b=Code();b.lea();b.call(0x3000);b.call(0x2000);b.call(0x4000);b.emit('488b8100010000c3')
b,e=setup({0x2000:b'\xc3',0x3000:b'\xc3',0x4000:b'\xc3'},[(0x3000,0x3001,1)],caps=replace(c55.Caps(),functions=1),builder=b)
assert not run(e,b)['references'] and e.closed
assert e.runtime.metrics['callee_summaries_admitted']==1 and e.metrics['direct_entry_summary_admitted']==1
assert request(e,b).status=='REFUSED'
print('PASS: c55 global exhaustion is sticky across both contracts and cached requests')

c=Code(0x3000);c.call(0x4000);c.emit('c3')
b=caller(0x3000);b,e=setup({0x2000:parent.raw(),0x3000:c.raw(),0x4000:b'\xc3'},caps=replace(c55.Caps(),depth=2),builder=b)
assert request(e,b).status=='ADMITTED'
other=caller(0x2000);e.source=domain({0x1000:other.raw(),0x2000:parent.raw(),0x3000:c.raw(),0x4000:b'\xc3'},[(0x1000,0x1000+len(other.raw()),1)])
# New domain identity is refused, not used as a way around cached depth.
assert request(e,other).status=='REFUSED'
b=Code();b.lea();b.call(0x3000);b.call(0x2000);b.emit('488b8100010000c3')
b,e=setup({0x2000:parent.raw(),0x3000:c.raw(),0x4000:b'\xc3'},caps=replace(c55.Caps(),depth=2),builder=b)
assert not run(e,b)['references'] and 'cached_direct_entry_transitive_depth_cap' in str(e.refusals)
print('PASS: c55 cached transitive depth and changed image identity cannot bypass current callsite limits')

b=Code();b.lea();b.lea(op='488d25');b.call(0x2000);b.emit('488b8100010000488b842400010000c3')
b,e=setup({0x2000:b'\xc3'},builder=b);a=run(e,b)
assert len(a['references'])==1 and a['stats']['preserved_origin_register_facts_across_calls']==1 and 4 not in first(e).preserved
print('PASS: c55 RSP address facts always die across CALL independently of RCX preservation')

c=Code(0x2000);c.lea(cfg.SLOT,op='488d05');c.emit('c3')
b,e=setup({0x2000:c.raw()},builder=caller(tail='488b10c3'));assert not run(e,b)['references']
b=Code();b.lea(cfg.SLOT-0x400,op='488d3d');b.lea();b.emit('488b8900010000');b.call(0x2000);b.emit('488b01c3')
b,e=setup({0x2000:b'\xc3'},builder=b);a=run(e,b)
assert len(a['references'])==1 and a['unresolved_reasons'].get('memory_loaded_base')
print('PASS: c55 return/local LEA/loaded base never creates a caller origin or pointer value')

for address,count in [(cfg.SLOT+8,0),(cfg.SLOT-8,0),(cfg.SLOT-4,1)]:
    b=Code();b.lea(address);b.call(0x2000);b.emit('488901c3')
    b,e=setup({0x2000:b'\xc3'},builder=b);assert len(run(e,b)['references'])==count
print('PASS: c55 preserved addresses retain byte-exact overlap and exclude adjacent storage')

b=Code();b.call(0x2000);b.emit('488b8100010000c3')
b,e=setup({0x2000:b'\xc3'},builder=b);assert not run(e,b)['references'] and e.metrics['summary_requests']==0
assert cfg.old.external_anchor_record(bytes.fromhex('488b8100010000'),0x140000000,0xD000000)['local_scanner_coverage'] is False
print('PASS: c55 absent incoming facts do not seed summaries; external c28 anchor remains separate')

b,e=setup({0x2000:b'\xc3'},excluded=((0x2000,0x2001),));assert not run(e,b)['references']
assert all(not at<=0x2000<at+n for at,n in e.source.reads)
b,e=setup({0x2000:b'\xc3'},excluded=((0x2001,0x2010),));assert len(run(e,b)['references'])==2
assert all(not (at<0x2010 and 0x2001<at+n) for at,n in e.source.reads)
print('PASS: c55 producer/deferred spans and even unreachable lookahead bytes stay unread')

b,e=setup({0x2000:b'\xc3'});assert e.get(0x2000).status=='REFUSED'
ins=next(i for i in cfg.old.decoder().disasm(b.raw(),b.rva) if i.group(cfg.cs.CS_GRP_CALL));edge=e.edge(ins,b.rva,c54.CONTRACT)
assert e.get(0x3000,edge=edge).status=='REFUSED'
edge['instruction_sha256']='0'*64;assert e.get(0x2000,edge=edge).status=='REFUSED'
assert not e.cache
print('PASS: c55 missing, wrong-target or stale CALL edge cannot authenticate metadata-free entry')

b,e=setup({0x2000:b'\xc3'});s=request(e,b);assert request(e,b)==s and e.metrics['direct_entry_cache_hits']==1
b,other=setup({0x2000:bytes.fromhex('90c3')});t=request(other,b);assert s.identity!=t.identity
record=s.record(e.image_id);assert record['end_rva'] is None and record['contract']==c55.CONTRACT and not record['function_span_inferred']
assert 'code' not in record and 'bytes' not in record
r=e.report();assert sum(r['target_metadata_relation_request_counts'].values())==2 and sum(r['target_metadata_relation_unique_counts'].values())==1
assert r['unique_callsites_reported']==1 and r['target_frequency_examples'][0]['requests']==2
print('PASS: c55 immutable reachable-code/CFG/child cache identity, request versus unique versus callsite totals, no raw bodies')

for limits in [dict(diagnostic_targets=1),dict(diagnostic_callsites=1)]:
    b=Code();b.lea();b.call(0x2000);b.call(0x3000);b.emit('488b8100010000c3')
    b,e=setup({0x2000:b'\xc3',0x3000:b'\xc3'},caps=replace(c55.Caps(),**limits),builder=b)
    assert not run(e,b)['references'] and e.closed and not e.report()['diagnostic_counts_complete']
print('PASS: c55 bounded diagnostic target/callsite storage exhausts closed rather than dropping positive-proof evidence')

b,e=setup({0x2000:b'\xc3'});old=e.source.read;count=0
def stale(pc,n):
    global count
    if pc==0x2000:
        count+=1
        if count>1:return b'\x90'*n
    return old(pc,n)
e.source.read=stale
assert not run(e,b)['references'] and first(e).reason=='direct_entry_stale_byte_mismatch'
print('PASS: c55 reached instruction final byte mismatch refuses, despite an otherwise successful RET graph')

for row in json.loads((Path(__file__).parent/'fixtures/c54_summary_compatibility.json').read_text())['cases']:
    e54=c54.SummaryEngine(c54.OwnedDomain({int(k):bytes.fromhex(v) for k,v in row['functions'].items()}));e54.get(row['target'])
    assert c55.digest(e54.report())==row['canonical_result_sha256'],row['name']
print('PASS: c55 child-routing seam leaves all nine pre-c55 default c54 output snapshots identical')


def owned_pe():
    b=caller(0x1100);leaf=bytes.fromhex('b801000000c3');text=b.raw()+b'\x90'*(0x100-len(b.raw()))+leaf
    rows=struct.pack('<IIIIII',0x1000,0x1000+len(b.raw()),0x2100,cfg.DEFERRED,cfg.DEFERRED+1,0x2100)
    specs=[(b'.text',0x1000,text,0x60000020),(b'.defer',cfg.DEFERRED,b'\xc3',0x60000020),
           (b'.pdata',0x2000,rows,0x40000040),(b'.data',cfg.SLOT&~0xfff,b'\0'*4096,0xc0000040)]
    data=bytearray(1024);struct.pack_into('<H',data,0,0x5a4d);struct.pack_into('<I',data,60,0x80);data[0x80:0x84]=b'PE\0\0'
    struct.pack_into('<HH',data,0x84,0x8664,4);struct.pack_into('<H',data,0x94,240);struct.pack_into('<H',data,0x98,0x20b)
    struct.pack_into('<Q',data,0x98+24,0x140000000);struct.pack_into('<I',data,0x98+56,0xD000000);struct.pack_into('<II',data,0x98+112+24,0x2000,len(rows))
    for i,(name,pc,raw,flags) in enumerate(specs):
        at=0x188+40*i;data[at:at+len(name)]=name;struct.pack_into('<IIII',data,at+8,len(raw),pc,len(raw),len(data));struct.pack_into('<I',data,at+36,flags);data.extend(raw)
    return bytes(data)


raw=owned_pe();prior=(c55.EXPECTED_SHA,c54.EXPECTED_SHA,cfg.EXPECTED_SHA)
try:
    c55.EXPECTED_SHA=c54.EXPECTED_SHA=cfg.EXPECTED_SHA=hashlib.sha256(raw).hexdigest()  # owned unit seam only
    a=c55.analyze_image(raw);assert a==c55.analyze_image(raw)
    assert c54.analyze_image(raw)['reader_candidates']==cfg.analyze_image(raw)['reader_candidates']==0
    assert a['repository_result']=='DIRECT_CALL_ENTRY_SLOT_SCANNER_READY' and a['schema_version']==3
    assert a['reader_candidates']==a['writer_candidates']==a['direct_entry_summary_admitted']==1
    assert a['c54_runtime_entry_summary_attempted']==0 and a['call_edges_with_any_preserved_origin']==1
    assert not a['writer_enumeration_complete'] and not a['production_integrated'] and a['live_qword_reads']==0 and not a['pointer_chase']
    assert a['c52_production_watch_gate']=='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'
    with tempfile.TemporaryDirectory(prefix='c55 owned ') as tmp:
        exe,out=Path(tmp)/'owned image.exe',Path(tmp)/'result.json';exe.write_bytes(raw);args=sys.argv
        try:
            sys.argv=[str(Path(c55.__file__)),str(exe),'--output',str(out)];capture=io.StringIO()
            with contextlib.redirect_stdout(capture):c55.main()
            assert capture.getvalue()==out.read_text() and len(capture.getvalue().splitlines())==1
        finally:sys.argv=args
finally:c55.EXPECTED_SHA,c54.EXPECTED_SHA,cfg.EXPECTED_SHA=prior
print('PASS: c55 owned PE and compact CLI separate real authenticated input from fixtures and preserve every completeness gate')

with tempfile.TemporaryDirectory() as tmp:
    exe,out=Path(tmp)/'owned.exe',Path(tmp)/'result.json';exe.write_bytes(raw);out.write_text('keep')
    r=subprocess.run([sys.executable,str(Path(c55.__file__)),str(exe),'--output',str(out)],capture_output=True,text=True)
    assert r.returncode!=0 and 'SHA256' in r.stderr and out.read_text()=='keep'
print('PASS: c55 CLI has no image-guard bypass and refuses wrong input without replacing the existing report')
