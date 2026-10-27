"""Owned c57 edge/metadata/replay/aggregate/CLI tests; no private executable."""
from contextlib import contextmanager,redirect_stdout,redirect_stderr
from copy import deepcopy
from dataclasses import replace
import io
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
from types import SimpleNamespace

import slot_publication_boundary as tool

c56,cfg=tool.c56,tool.cfg


def jump(pc,target,conditional=False):
    prefix=bytes.fromhex('0f85') if conditional else b'\xe9'
    return prefix+struct.pack('<i',target-pc-len(prefix)-4)


def owned_pe(functions,extra=(),image_size=0xD000000):
    functions=dict(functions);functions[cfg.DEFERRED]=b'\xc3'
    rows=sorted((pc,pc+len(raw),0x6100) for pc,raw in functions.items())
    table=b''.join(struct.pack('<III',*r) for r in rows)
    specs=[(f'.f{i}'.encode(),pc,raw,0x60000020,len(raw)) for i,(pc,raw) in enumerate(sorted(functions.items()))]
    specs.extend(extra)
    specs.extend([(b'.pdata',0x6000,table,0x40000040,len(table)),
        (b'.data',cfg.SLOT&~0xfff,b'\0'*4096,0xc0000040,4096)])
    data=bytearray(2048);struct.pack_into('<H',data,0,0x5a4d);struct.pack_into('<I',data,60,0x80);data[0x80:0x84]=b'PE\0\0'
    struct.pack_into('<HH',data,0x84,0x8664,len(specs));struct.pack_into('<H',data,0x94,240);struct.pack_into('<H',data,0x98,0x20b)
    struct.pack_into('<Q',data,0x98+24,0x140000000);struct.pack_into('<I',data,0x98+56,image_size)
    struct.pack_into('<II',data,0x98+112+24,0x6000,len(table))
    for i,(name,pc,raw,flags,virtual) in enumerate(specs):
        at=0x188+40*i;data[at:at+len(name)]=name
        struct.pack_into('<IIII',data,at+8,virtual,pc,len(raw),len(data));struct.pack_into('<I',data,at+36,flags);data.extend(raw)
    return bytes(data),len(rows)


@contextmanager
def authenticated(raw,metadata_rows,boundary_rows):
    prior=tool.EXPECTED_SHA,c56.EXPECTED_SHA,tool.ACCEPTED_C56
    try:
        tool.EXPECTED_SHA=c56.EXPECTED_SHA=c56.sha(raw)  # owned Python seam only, no CLI override
        tool.ACCEPTED_C56=tool.Acceptance(metadata_rows,boundary_rows)
        yield
    finally:tool.EXPECTED_SHA,c56.EXPECTED_SHA,tool.ACCEPTED_C56=prior


def fixture(functions,extra=(),expected=1,profile=None):
    raw,count=owned_pe(functions,extra)
    with authenticated(raw,count,expected):
        session=c56.Session(raw,profile or c56.Profile())
        baseline=tool.collect_baseline(session)
    return raw,baseline,tool.Classifier(baseline)


def result(classifier,records=None):
    aggregate=tool.Aggregate(classifier)
    for record in (records if records is not None else [classifier.record(w) for w in classifier.baseline.witnesses]):aggregate.add(record)
    return aggregate.finish()


def refused(fn,text=None):
    try:fn()
    except ValueError as error:
        if text:assert text in str(error),(text,str(error))
    else:raise AssertionError('invalid boundary proof accepted')


raw,baseline,c=fixture({0x1000:b'\x90',0x1001:b'\xc3'})
record=c.record(baseline.witnesses[0]);r=result(c)
assert record['edges'][0]['edge_kind']=='ordinary_fallthrough' and record['edges'][0]['edge_target_rva']==0x1001
assert r['boundary_refused_rows_classified']==1 and r['edge_kind_counts']['ordinary_fallthrough']==1
assert record['edges'][0]['exact_adjacency'] is True and record['edges'][0]['source_target_spans_overlap'] is False
assert not r['cross_runtime_fact_propagation'] and not r['candidate_sweep_complete']
print('PASS: c57 ordinary end fallthrough and exact adjacency are measured without admitting a continuation')

_,b,c=fixture({0x1000:jump(0x1000,0x1100),0x1100:b'\xc3'})
edge=c.record(b.witnesses[0])['edges'][0]
assert edge['edge_kind']=='direct_unconditional_branch' and edge['target_metadata_relation']=='exact_unique_runtime_begin'
assert edge['target_runtime_row_index']==1 and (edge['target_begin_rva'],edge['target_end_rva'])==(0x1100,0x1101) and edge['exact_adjacency'] is False
print('PASS: c57 unconditional immediate branch classifies exact target row identity independently of distance')

_,b,c=fixture({0x1000:jump(0x1000,0x1100,True)+b'\xc3',0x1100:b'\xc3'})
edges=c.record(b.witnesses[0])['edges'];assert len(edges)==1 and edges[0]['edge_kind']=='direct_conditional_taken'
assert result(c)['exact_runtime_begin_edge_kind_counts']['direct_conditional_taken']==1
print('PASS: c57 conditional taken crossing is separate from its in-row fallthrough')

_,b,c=fixture({0x1000:jump(0x1000,0x1000,True),0x1006:b'\xc3'})
edges=c.record(b.witnesses[0])['edges'];assert len(edges)==1 and edges[0]['edge_kind']=='conditional_fallthrough'
assert edges[0]['edge_target_rva']==0x1006
print('PASS: c57 conditional fallthrough crossing is exact decoder semantics, not a proximity guess')

call=b'\xe8'+struct.pack('<i',0x1100-0x1005)
_,b,c=fixture({0x1000:call,0x1005:b'\xc3',0x1100:b'\xc3'})
edges=c.record(b.witnesses[0])['edges'];assert len(edges)==1 and edges[0]['edge_kind']=='call_fallthrough'
assert edges[0]['edge_target_rva']==0x1005 and edges[0]['edge_target_rva']!=0x1100
print('PASS: c57 CALL boundary successor is the return-site fallthrough, never a fabricated callee CFG edge')

_,b,c=fixture({0x1000:jump(0x1000,0x1006,True),0x1006:b'\xc3'})
r=result(c);edges=c.record(b.witnesses[0])['edges']
assert len(edges)==2 and edges[0]['edge_target_rva']==edges[1]['edge_target_rva']==0x1006
assert r['boundary_edges_total']==2 and r['edge_kind_counts']['direct_conditional_taken']==r['edge_kind_counts']['conditional_fallthrough']==1
print('PASS: c57 equal taken/fallthrough target addresses still represent two distinct semantic edges')

bnd=b'\xf2\xe9'+struct.pack('<i',0x1000-0x1006)
_,b,c=fixture({0x1000:bnd,0x1006:b'\xc3'})
edge=c.record(b.witnesses[0])['edges'][0];r=result(c)
assert edge['edge_kind']=='other_supported_direct_successor' and edge['edge_target_rva']==0x1006
assert edge['architectural_successor'] is False and edge['diagnostic_note']=='existing_c53_decorated_JMP_model_only_fallthrough'
assert r['architectural_boundary_edges_total']==0 and r['model_only_boundary_edges_total']==r['boundary_edges_total']==1
print('PASS: c57 decorated JMP identity distinguishes the old model-only fallthrough without changing c53 refusal semantics')

_,b,c=fixture({0x1000:jump(0x1000,0x1101),0x1100:b'\x90\xc3'})
r=result(c);edge=c.record(b.witnesses[0])['edges'][0]
assert edge['target_metadata_relation']=='inside_unique_runtime_span_nonentry' and edge['target_runtime_row_index'] is None
assert r['interior_target_edge_kind_counts']['direct_unconditional_branch']==1 and not r['cross_runtime_fact_propagation']
print('PASS: c57 runtime-span interior target remains an unadmitted diagnostic class')

domain=c56.c55.Domain(image_id='OWNED',rows=[(0x1000,0x1090,1),(0x1080,0x1100,1)],
    sections=[(0x1000,0x2000,0,0x1000,0x60000020)],image_size=0x3000,excluded=[],read=lambda pc,n:None)
fake=SimpleNamespace(domain=domain,rows=((0x1000,0x1090,1),(0x1080,0x1100,1)))
edge=tool.target_relation(fake,0x2000,0x2100,0x1088)
assert edge['target_metadata_relation']=='overlapping_multiple_runtime_spans' and edge['target_runtime_row_index'] is None
edge=tool.target_relation(fake,0x1080,0x1100,0x1000)
assert edge['target_metadata_relation']=='exact_unique_runtime_begin' and edge['source_target_spans_overlap'] is True
print('PASS: c57 overlapping metadata and unique-at-target but overlapping spans are explicit diagnostics, never proof')

_,b,c=fixture({0x1000:jump(0x1000,0x1100)},[(b'.free',0x1100,b'\xc3',0x60000020,1)])
r=result(c);assert r['metadata_free_RX_edge_kind_counts']['direct_unconditional_branch']==1
assert r['boundary_edges_total']==1 and not r['writer_enumeration_complete']
print('PASS: c57 metadata-free RX target is classified but not followed or admitted')

for flags,virtual,target,expected in [(0x60000020,2,0x1101,'executable_not_fully_file_backed'),
        (0x40000040,1,0x1100,'non_executable_or_writable_section'),(0xe0000020,1,0x1100,'non_executable_or_writable_section')]:
    _,b,c=fixture({0x1000:jump(0x1000,target)},[(b'.bad',0x1100,b'\xc3',flags,virtual)])
    assert c.record(b.witnesses[0])['edges'][0]['target_metadata_relation']==expected
    assert result(c)['ambiguous_or_invalid_target_counts'][expected]==1
print('PASS: c57 unbacked/nonexec/writable targets remain invalid diagnostic classes')

_,b,c=fixture({0xCFFFFFF:b'\x90'})
edge=c.record(b.witnesses[0])['edges'][0];assert edge['edge_kind']=='ordinary_fallthrough' and edge['edge_target_rva']==0xD000000 and edge['target_metadata_relation']=='outside_image'
print('PASS: c57 exact last-byte fallthrough can classify outside-image boundary without guessing an indirect target')

_,b,c=fixture({0x1000:jump(0x1000,cfg.DEFERRED)})
reads=[];old=b.session.read
def spy(pc,n):reads.append((pc,n));return old(pc,n)
b.session.read=spy;r=result(c)
assert r['target_metadata_relation_counts']['deferred_producer_excluded']==1
assert all(not(pc<cfg.DEFERRED+1 and cfg.DEFERRED<pc+n) for pc,n in reads)
assert all(not(pc<cfg.SLOT+8 and cfg.SLOT<pc+n) for pc,n in reads)
print('PASS: c57 producer target is metadata-only; no target code or slot bytes are read')

code=jump(0x1000,0x100B,True)+jump(0x1006,0x1100)+jump(0x100B,0x1200)
_,b,c=fixture({0x1000:code,0x1100:b'\xc3',0x1200:b'\xc3'})
record=c.record(b.witnesses[0]);assert record['source_instruction_rva']==0x1006 and record['edges'][0]['edge_target_rva']==0x1100
assert record['recovery_work']['states_discovered']==2 and record['recovery_work']['edges_traversed']==2
print('PASS: c57 replays original heap order and stops at the exact first boundary refusal, not a later source exit')

raw,b,c=fixture({0x1000:b'\x90',0x1100:jump(0x1100,0x1200),0x1200:b'\xc3'},expected=2)
records=[c.record(w) for w in b.witnesses]
for supplied in [records[:-1],records[1:],list(reversed(records)),[records[0],records[0]],records+[records[1]]]:
    refused(lambda:result(c,supplied))
refused(lambda:tool.Classifier(replace(b,witnesses=(b.witnesses[0],b.witnesses[0]))))
print('PASS: c57 missing/duplicate/out-of-order source classifications and source-set identities poison/refuse aggregation')

for key,value in [('source_runtime_row_index',1),('source_runtime_row_sha256','0'*64),('source_code_sha256','0'*64),
                 ('source_instruction_rva',0x1001),('source_instruction_sha256','0'*64)]:
    wrong=deepcopy(records);wrong[0][key]=value;refused(lambda:result(c,wrong))
for key,value in [('tool_version','wrong'),('schema_version',99),('contract','wrong'),('image_sha256','0'*64)]:
    header=deepcopy(c.header);header[key]=value;wrong=deepcopy(records);wrong[0]['classifier_identity']=c56.digest(header)
    refused(lambda:result(c,wrong),'identity')
print('PASS: c57 wrong image/tool/schema/contract/row/source instruction identities cannot be accepted')

for key,value in [('edge_kind','direct_conditional_taken'),('target_metadata_relation','exact_unique_runtime_begin'),
                 ('edge_target_rva',0x1200),('target_runtime_row_index',99),('exact_adjacency',True)]:
    wrong=deepcopy(records);wrong[0]['edges'][0][key]=value;refused(lambda:result(c,wrong))
wrong=deepcopy(records);wrong[0]['edges'].append(deepcopy(wrong[0]['edges'][0]));refused(lambda:result(c,wrong))
ag=tool.Aggregate(c);refused(lambda:ag.add(wrong[0]));refused(lambda:ag.add(records[0]),'poisoned');refused(ag.finish)
print('PASS: c57 malformed edge kinds/relations/counts/adjacency cannot recover poisoned partial aggregate state')

limited=tool.Classifier(b,0);r0=result(limited);r8=result(c)
for key in ['boundary_edges_total','boundary_refused_rows_classified','edge_kind_counts','target_metadata_relation_counts',
            'exact_runtime_begin_edge_kind_counts','interior_target_edge_kind_counts','metadata_free_RX_edge_kind_counts']:
    assert r0[key]==r8[key]
assert not any(r0['examples_by_edge_kind'].values()) and all(len(v)<=8 for v in r8['examples_by_target_relation'].values())
assert sum(r8['edge_kind_counts'].values())==sum(r8['target_metadata_relation_counts'].values())==r8['boundary_edges_total']
print('PASS: c57 bounded examples never truncate exact row/edge/class counters')

for key in ['reference_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates','reference_candidates','external_anchors']:
    assert r8[key]==b.result[key]
assert all(r8[k] is False for k in c56.FALSE_GATES) and r8['live_qword_reads']==0 and not r8['cross_runtime_fact_propagation']
print('PASS: c57 preserves c56 candidate outputs and every completeness/publication/production/gameplay gate')

refused(lambda:tool.collect_baseline(b.session,acceptance=replace(b.acceptance,boundary_rows=3)),'count mismatch')
refused(lambda:tool.collect_baseline(b.session,acceptance=replace(b.acceptance,metadata_rows=99)),'metadata count')
bad_w=replace(b.witnesses[0],work_sha256='0'*64);refused(lambda:tool.recover(b.session,bad_w),'work mismatch')
bad_w=replace(b.witnesses[0],body_sha256='0'*64);refused(lambda:tool.recover(b.session,bad_w),'stale')
print('PASS: c57 accepted c56 set cardinality and exact first-refusal work/bytes must agree')

old_read=b.session.read
try:
    b.session.read=lambda pc,n:b'\xcc'*n
    refused(lambda:tool.recover(b.session,b.witnesses[0]),'stale')
finally:b.session.read=old_read
original_identity=tool.source_identity
try:
    tool.source_identity=lambda:{'wrong source':'0'*64}
    refused(lambda:result(c),'source changed')
finally:tool.source_identity=original_identity
print('PASS: c57 changed source bytes and classifier implementation identity refuse final publication')

_,clean,c=fixture({0x1000:b'\xc3'},expected=0)
assert result(c)['classification_complete'] and result(c)['boundary_edges_total']==0
_,other,c=fixture({0x1000:bytes.fromhex('ffe0')},expected=0)
assert not other.witnesses and other.result['row_refusal_reasons']['indirect_jump']==1
print('PASS: c57 does not relabel RET, indirect or other refusal classes into the boundary set')

raw,b,c=fixture({0x1000:b'\x90',0x1001:b'\xc3'})
with tempfile.TemporaryDirectory(prefix='c57 read only ') as tmp:
    cache=Path(tmp)/'c56';prior=c56.run(b.session,cache)
    before={p.name:p.read_bytes() for p in cache.iterdir()}
    def no_compute(index):raise AssertionError('read-only classifier recomputed c56 shard')
    old=b.session.make_shard;b.session.make_shard=no_compute
    loaded=tool.collect_baseline(b.session,cache,b.acceptance);b.session.make_shard=old
    assert loaded.witnesses==b.witnesses and c56.encoded(result(tool.Classifier(loaded)))==c56.encoded(result(c))
    assert before=={p.name:p.read_bytes() for p in cache.iterdir()} and prior==loaded.result
    path=next(cache.glob('shard-*.json'));path.unlink()
    refused(lambda:tool.collect_baseline(b.session,cache,b.acceptance))
print('PASS: c57 reuses complete c56 checkpoints read-only, produces identical classification and refuses missing evidence')

with tempfile.TemporaryDirectory() as tmp:
    cache=Path(tmp);c56.run(b.session,cache);path=cache/'manifest.json';value=c56.read_json(path)
    value['header']['schema_version']=99;c56.atomic_json(path,value)
    refused(lambda:tool.collect_baseline(b.session,cache,b.acceptance),'manifest')
print('PASS: c57 stale c56 image/schema/tool/contract manifest refuses without silently repairing cache')

with authenticated(raw,len(b.session.rows),1),tempfile.TemporaryDirectory(prefix='c57 CLI ') as tmp:
    exe,out=Path(tmp)/'owned.exe',Path(tmp)/'result.json';exe.write_bytes(raw);args=sys.argv
    try:
        sys.argv=[tool.__file__,str(exe),'--output',str(out)];capture,progress=io.StringIO(),io.StringIO()
        with redirect_stdout(capture),redirect_stderr(progress):tool.main()
        assert capture.getvalue().encode()==out.read_bytes() and len(capture.getvalue().splitlines())==1
        assert b'RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY' in out.read_bytes() and 'boundary classification' in progress.getvalue()
    finally:sys.argv=args
print('PASS: c57 one offline CLI command validates baseline and publishes compact diagnostic JSON with separate progress')

with authenticated(raw,len(b.session.rows),2),tempfile.TemporaryDirectory(prefix='c57 mismatch ') as tmp:
    exe,out=Path(tmp)/'owned.exe',Path(tmp)/'result.json';exe.write_bytes(raw);out.write_text('keep');args=sys.argv
    try:
        sys.argv=[tool.__file__,str(exe),'--output',str(out)];capture=io.StringIO()
        with redirect_stdout(capture):
            try:tool.main()
            except SystemExit as error:assert error.code==2
            else:raise AssertionError('wrong accepted cardinality silently certified')
        assert tool.BLOCKED in capture.getvalue() and 'count mismatch' in capture.getvalue() and out.read_text()=='keep'
    finally:sys.argv=args
print('PASS: c57 authenticated CLI count mismatch refuses and preserves the prior diagnostic JSON')

with tempfile.TemporaryDirectory() as tmp:
    exe,out=Path(tmp)/'owned.exe',Path(tmp)/'result.json';exe.write_bytes(raw);out.write_text('keep')
    r=subprocess.run([sys.executable,tool.__file__,str(exe),'--output',str(out)],capture_output=True,text=True)
    assert r.returncode==2 and out.read_text()=='keep' and tool.BLOCKED in r.stdout
    r=subprocess.run([sys.executable,tool.__file__,'--help'],capture_output=True,text=True)
    assert r.returncode==0 and '--expected' not in r.stdout and '--hash' not in r.stdout
print('PASS: c57 CLI has no image/count override, rejects unauthenticated input and preserves the previous output')
