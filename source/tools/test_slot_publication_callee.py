"""Owned synthetic code/PE only; no private image, ABI or runtime evidence."""
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
from types import SimpleNamespace

import slot_publication_callee as callee
import slot_publication_cfg as cfg


class Code:
    def __init__(self,rva=0x1000):self.rva=rva;self.data=bytearray();self.labels={};self.patches=[]
    def emit(self,s):self.data.extend(bytes.fromhex(s))
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


def engine(functions,caps=callee.SummaryCaps(),excluded=()):
    return callee.SummaryEngine(callee.OwnedDomain(functions,excluded),caps)


def caller(e,target=0x2000,tail='488b810001000048898100010000c3'):
    b=Code();b.lea();b.call(target);b.emit(tail)
    return cfg.scan_region(b.raw(),b.rva,call_preservation=e.for_call)


e=engine({0x2000:bytes.fromhex('b801000000c3')});a=caller(e)
assert [r['operation'] for r in a['references']]==['READ','WRITE']
assert a['stats']['preserved_origin_register_facts_across_calls']==1
assert 1 in e.get(0x2000).preserved and 0 not in e.get(0x2000).preserved
assert a['references'][0]['cfg']['call_preservation_graph_sha256']
print('PASS: c54 authenticated owned leaf never writes RCX; exact reader/writer survive CALL with summary identity')

b=Code(0x2000);b.emit('85c0');b.jump('end',True);b.emit('b900000000');b.label('end');b.emit('c3')
e=engine({0x2000:b.raw()});a=caller(e)
assert not a['references'] and 1 not in e.get(0x2000).preserved
assert 3 in e.get(0x2000).preserved  # known-written RCX does not guess all other registers written
print('PASS: c54 one branch writing RCX kills only unproven registers; no branch outcome or ABI assumption')

b=Code();b.lea();b.emit('85c0');b.jump('bypass',True);b.call(0x2000);b.jump('join')
b.label('bypass');b.emit('90');b.label('join');b.emit('488b8100010000c3')
for raw,preserved in [(b'\xc3',True),(bytes.fromhex('b901000000c3'),False)]:
    e=engine({0x2000:raw});a=cfg.scan_region(b.raw(),b.rva,call_preservation=e.for_call)
    assert bool(a['references']) is preserved
    if not preserved:assert a['stats']['conflicting_facts_discarded']
print('PASS: c54 caller branch bypass/summary paths still use a must join; a clobbered branch cannot be hidden')

b=Code(0x2000);b.emit('85c0');b.jump('other',True);b.emit('c3');b.label('other');b.emit('90c3')
s=engine({0x2000:b.raw()}).get(0x2000)
assert s.status=='ADMITTED' and 1 in s.preserved and s.returns==2
print('PASS: c54 all reachable normal RET exits must preserve the register')

b=Code(0x2000);b.emit('85c0');b.jump('other',True);b.emit('c3');b.label('other');b.emit('0f31c3')
s=engine({0x2000:b.raw()}).get(0x2000)
assert s.status=='REFUSED' and not s.preserved and s.reason=='unsupported_instruction_effect'
print('PASS: c54 unsupported effect on one path refuses the entire summary, not only the observed RET path')

b=Code(0x2000);b.call(0x3000);b.emit('c3')
for body,preserve in [('b801000000c3',True),('b900000000c3',False)]:
    e=engine({0x2000:b.raw(),0x3000:bytes.fromhex(body)})
    a=caller(e);s=e.get(0x2000)
    assert bool(a['references']) is preserve and (1 in s.preserved) is preserve
    assert s.height==2 and len(s.children)==1 and e.metrics['nested_summaries_used']==1
print('PASS: c54 nested summaries preserve only never-written registers; nested RCX writes kill caller RCX')

for recursive in [True,False]:
    b=Code(0x2000);b.call(0x2000 if recursive else 0x3000);b.emit('c3')
    c=Code(0x3000);c.call(0x2000);c.emit('c3')
    e=engine({0x2000:b.raw(),0x3000:c.raw()});s=e.get(0x2000)
    assert s.status=='REFUSED' and not s.preserved and e.metrics['summary_cycles_refused']
e=engine({0x2000:bytes.fromhex('ebfe')});assert e.get(0x2000).reason=='callee_control_cycle'
print('PASS: c54 direct/self recursion and reachable CFG cycles never become PRESERVED SCCs')

for body in ['ffe0','ffd0','0f05','0f0b']:
    e=engine({0x2000:bytes.fromhex(body)});s=e.get(0x2000)
    assert s.status=='REFUSED' and not s.preserved
print('PASS: c54 indirect jump/call and system transfer refuse without target recovery')

for body in ['0f','488d0d00000000e9f5ffffff','e900100000']:
    s=engine({0x2000:bytes.fromhex(body)}).get(0x2000)
    assert s.status=='REFUSED' and not s.preserved
e=engine({0x2000:bytes.fromhex('90c3')})
assert e.get(0x2001).status=='REFUSED' and e.source.loads==0
print('PASS: c54 decode gaps/middle targets/out-of-span edges/non-entry callees refuse before preserving any origin')

for name in ['region_bytes','instructions','states','edges','blocks']:
    b=Code(0x2000);b.emit('85c0');b.jump('other',True);b.emit('90c3');b.label('other');b.emit('90c3')
    e=engine({0x2000:b.raw()},replace(callee.SummaryCaps(),**{name:1}))
    assert e.get(0x2000).status=='REFUSED',name
print('PASS: c54 independent per-summary byte/instruction/state/edge/block caps refuse')

b=Code(0x2000);b.call(0x3000);b.emit('c3')
for limits in [dict(depth=1),dict(functions=1),dict(bytes=6),dict(work=6)]:
    e=engine({0x2000:b.raw(),0x3000:b'\xc3'},replace(callee.SummaryCaps(),**limits))
    assert e.get(0x2000).status=='REFUSED'
    assert e.metrics['summary_work']<=e.caps.work and e.metrics['summary_code_bytes_loaded']<=e.caps.bytes
print('PASS: c54 depth/global-function/global-byte/total-work exhaustion never yields partial preservation')

e=engine({0x2000:b'\xc3',0x3000:b'\xc3'},replace(callee.SummaryCaps(),functions=1,refusal_examples=2))
assert e.get(0x2000).status=='ADMITTED'
assert e.get(0x3000).status=='REFUSED' and e.exhausted
for i in range(8):assert e.get(0x2000+i).status=='REFUSED'
assert e.source.loads==1 and len(e.report()['refusal_examples'])<=2
e=engine({0x2000:b'\x90\xc3'},replace(callee.SummaryCaps(),work=1))
assert e.get(0x2000).status=='REFUSED' and e.source.loads==0
e=engine({0x2000:b'\xc3'},replace(callee.SummaryCaps(),work=8))
assert e.get(0x2000).status=='ADMITTED'
for _ in range(10):e.get(0x2000)
assert e.exhausted and e.metrics['summary_work']<=8 and e.source.loads==1
print('PASS: c54 exhausted global budget stays closed, including cached requests; refusal records and pre-load work stay bounded')

c=Code(0x3000);c.call(0x4000);c.emit('c3')
b=Code(0x2000);b.call(0x3000);b.emit('c3')
e=engine({0x2000:b.raw(),0x3000:c.raw(),0x4000:b'\xc3'},replace(callee.SummaryCaps(),depth=2))
assert e.get(0x3000).status=='ADMITTED'  # cached subtree height2 at depth1
assert e.get(0x2000).status=='REFUSED' and 'cached_transitive_depth_cap' in str(e.refusals)
print('PASS: c54 cached transitive height cannot bypass depth admission from a deeper callsite')

b=Code();b.lea();b.lea(op='488d25');b.call(0x2000);b.emit('488b8100010000488b842400010000c3')
e=engine({0x2000:b'\xc3'});a=cfg.scan_region(b.raw(),b.rva,call_preservation=e.for_call)
assert len(a['references'])==1 and a['stats']['preserved_origin_register_facts_across_calls']==1
assert a['unresolved_reasons'].get('call_clobber_all_address_origins')
assert 4 not in e.get(0x2000).preserved
print('PASS: c54 RSP image-address fact always dies across CALL, independently of other preserved facts')

e=engine({0x2000:bytes.fromhex('51b90100000059c3')})
assert not caller(e)['references'] and 1 not in e.get(0x2000).preserved
print('PASS: c54 write then restore via PUSH/POP is still not never-written preservation')

for target,count in [(cfg.SLOT+8,0),(cfg.SLOT-8,0),(cfg.SLOT-4,1)]:
    b=Code();b.lea(target);b.call(0x2000);b.emit('488901c3')
    a=cfg.scan_region(b.raw(),b.rva,call_preservation=engine({0x2000:b'\xc3'}).for_call)
    assert len([r for r in a['references'] if r['may_write']])==count
print('PASS: c54 call-preserved addresses keep exact slot overlap and exclude adjacent storage')

b=Code();b.lea(cfg.SLOT-0x500,op='488d3d');b.lea();b.emit('488b8900010000');b.call(0x2000);b.emit('488b01c3')
e=engine({0x2000:b'\xc3'});a=cfg.scan_region(b.raw(),b.rva,call_preservation=e.for_call)
assert len(a['references'])==1 and a['references'][0]['operation']=='READ'
assert a['unresolved_reasons'].get('memory_loaded_base')
print('PASS: c54 even a preserved unknown loaded register remains unknown; no pointer value is propagated')

c=Code(0x2000);c.lea(cfg.SLOT,op='488d05');c.emit('c3')
e=engine({0x2000:c.raw()});a=caller(e,tail='488b10c3')
assert not a['references'] and 0 not in e.get(0x2000).preserved
print('PASS: c54 callee-local LEA/return-register addresses never create a caller origin')

e=engine({cfg.DEFERRED:b'\xc3'},excluded=[(cfg.DEFERRED,cfg.DEFERRED+1)])
assert not caller(e,cfg.DEFERRED)['references'] and e.source.loads==0
assert 'deferred_producer_excluded' in e.refusals
print('PASS: c54 deferred callee domain is rejected before loading or decoding its bytes')

b=Code();b.call(0x2000);b.emit('488b8100010000c3')
e=engine({0x2000:b'\xc3'});a=cfg.scan_region(b.raw(),b.rva,call_preservation=e.for_call)
assert not a['references'] and e.metrics['summary_requests']==0
assert cfg.old.external_anchor_record(bytes.fromhex('488b8100010000'),0x140000000,0xD000000)['local_scanner_coverage'] is False
print('PASS: c54 no incoming exact fact means no new origin/summary work; external c28 anchor remains separate')

original={0x2000:b'\xc3'};e=engine(original);a=e.get(0x2000);original[0x2000]=bytes.fromhex('b900000000c3')
assert e.get(0x2000)==a and e.metrics['summary_cache_hits']==1
other=engine(original).get(0x2000)
assert a.identity!=other.identity and 1 in a.preserved and 1 not in other.preserved
assert len(next(iter(e.cache)))==5  # actual key includes image/span/hash/contract
print('PASS: c54 immutable image/span/code-hash cache identity cannot reuse a changed callee proof')

def unknown_effect():raise cfg.cs.CsError(cfg.cs.CS_ERR_DETAIL)
fake=SimpleNamespace(mnemonic='nop',operands=[],group=lambda g:False,regs_access=unknown_effect)
try:callee.written_banks(fake)
except ValueError as error:assert str(error)=='unknown_register_effects'
else:raise AssertionError('unknown effect admitted')
e=engine({0x2000:bytes.fromhex('488b0401c3')})
assert e.get(0x2000).reason=='unsupported_callee_memory_addressing'
xadd=next(cfg.old.decoder().disasm(bytes.fromhex('480fc1c8'),0x2000,count=1))
assert {0,1} <= callee.written_banks(xadd)
print('PASS: c54 missing register effects and unsupported indexed callee addressing refuse')

e=engine({0x2000:bytes.fromhex('7500c3')});s=e.get(0x2000)
assert s.status=='ADMITTED' and s.returns==1
print('PASS: c54 duplicate target/fallthrough edges are normalized before all-path DAG checking')

snapshots=json.loads((Path(__file__).parent/'fixtures/c53_cfg_compatibility.json').read_text())
assert len(snapshots['cases'])==14
for row in snapshots['cases']:
    a=cfg.scan_region(bytes.fromhex(row['code']),row['rva'],**row['kwargs'])
    assert hashlib.sha256(json.dumps(a,sort_keys=True,separators=(',',':')).encode()).hexdigest()==row['canonical_result_sha256']
print('PASS: c54 optional integration leaves14 pre-c54 c53 default-result snapshots byte-equivalent')


def owned_pe():
    b=Code();b.lea();b.call(0x1100);b.emit('488b810001000048898100010000c3')
    leaf=bytes.fromhex('b801000000c3');text=b.raw()+b'\x90'*(0x100-len(b.raw()))+leaf
    rows=struct.pack('<IIIIIIIII',0x1000,0x1000+len(b.raw()),0x2100,0x1100,0x1100+len(leaf),0x2100,cfg.DEFERRED,cfg.DEFERRED+1,0x2100)
    specs=[(b'.text',0x1000,text,0x60000020),(b'.defer',cfg.DEFERRED,b'\xc3',0x60000020),
           (b'.pdata',0x2000,rows,0x40000040),(b'.data',cfg.SLOT&~0xfff,b'\0'*4096,0xc0000040)]
    data=bytearray(1024);struct.pack_into('<H',data,0,0x5a4d);struct.pack_into('<I',data,60,0x80);data[0x80:0x84]=b'PE\0\0'
    struct.pack_into('<HH',data,0x84,0x8664,4);struct.pack_into('<H',data,0x94,240);struct.pack_into('<H',data,0x98,0x20b)
    struct.pack_into('<Q',data,0x98+24,0x140000000);struct.pack_into('<I',data,0x98+56,0xD000000);struct.pack_into('<II',data,0x98+112+24,0x2000,len(rows))
    for i,(name,pc,raw,flags) in enumerate(specs):
        at=0x188+40*i;data[at:at+len(name)]=name;struct.pack_into('<IIII',data,at+8,len(raw),pc,len(raw),len(data));struct.pack_into('<I',data,at+36,flags);data.extend(raw)
    return bytes(data)


raw=owned_pe();prior=(callee.EXPECTED_SHA,cfg.EXPECTED_SHA)
try:
    callee.EXPECTED_SHA=cfg.EXPECTED_SHA=hashlib.sha256(raw).hexdigest()  # owned unit seam; no CLI bypass
    a=callee.analyze_image(raw);b=callee.analyze_image(raw)
    legacy=cfg.analyze_image(raw)
    assert legacy['tool_version']=='v0.1c53' and legacy['reader_candidates']==legacy['writer_candidates']==0
    assert 'callee_summary_analysis' not in legacy
    assert a==b and a['tool_version']=='v0.1c54' and a['schema_version']==2
    assert a['reader_candidates']==a['writer_candidates']==1
    assert a['callee_summaries_attempted']==a['callee_summaries_admitted']==1 and a['callee_summaries_refused']==0
    assert a['call_edges_with_any_preserved_origin']==a['preserved_origin_register_facts_across_calls']==1
    assert a['c53_call_clobber_all_address_origins_baseline']==324714
    assert not a['writer_enumeration_complete'] and not a['hardware_watch_enabled'] and a['live_qword_reads']==0 and not a['pointer_chase']
    assert a['c52_production_watch_gate']=='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'
    with tempfile.TemporaryDirectory(prefix='c54 owned ') as tmp:
        exe,out=Path(tmp)/'owned image.exe',Path(tmp)/'result.json';exe.write_bytes(raw);args=sys.argv
        try:
            sys.argv=[str(Path(callee.__file__)),str(exe),'--output',str(out)]
            capture=io.StringIO()
            with contextlib.redirect_stdout(capture):callee.main()
            assert capture.getvalue()==out.read_text() and len(capture.getvalue().splitlines())==1
        finally:sys.argv=args
finally:callee.EXPECTED_SHA,cfg.EXPECTED_SHA=prior
print('PASS: c54 owned PE/CLI integration is deterministic, metadata-authenticated, compact JSON and keeps all publication/runtime gates closed')

with tempfile.TemporaryDirectory() as tmp:
    exe,out=Path(tmp)/'owned.exe',Path(tmp)/'result.json';exe.write_bytes(raw);out.write_text('keep')
    r=subprocess.run([sys.executable,str(Path(callee.__file__)),str(exe),'--output',str(out)],capture_output=True,text=True)
    assert r.returncode!=0 and 'SHA256' in r.stderr and out.read_text()=='keep'
    r=subprocess.run([sys.executable,str(Path(callee.__file__)),'--help'],capture_output=True,text=True)
    assert r.returncode==0 and '--hash' not in r.stdout
print('PASS: c54 CLI exact-image authentication has no override and refuses without replacing an existing report')
