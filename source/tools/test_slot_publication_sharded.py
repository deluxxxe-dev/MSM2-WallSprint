"""c56 owned PE/checkpoint tests; no private image or runtime evidence."""
from contextlib import contextmanager,redirect_stdout,redirect_stderr
from copy import deepcopy
from dataclasses import replace
import hashlib
import io
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

import slot_publication_sharded as tool

cfg,c55=tool.cfg,tool.c55


def body(pc,leaf=0x1800):
    raw=bytearray(bytes.fromhex('488d0d')+struct.pack('<i',cfg.SLOT-0x100-pc-7))
    raw.extend(b'\xe8'+struct.pack('<i',leaf-pc-len(raw)-5))
    raw.extend(bytes.fromhex('488b810001000048898100010000c3'))
    return bytes(raw)


def owned_pe(count=5,anchor=False,leaf='b801000000c3',deferred_row=True):
    text=bytearray(b'\xcc'*0x900);rows=[]
    for i in range(count):
        pc=0x1000+i*0x40;code=body(pc);text[pc-0x1000:pc-0x1000+len(code)]=code
        rows.append((pc,pc+len(code),0x4100))
    leaf=bytes.fromhex(leaf);text[0x800:0x800+len(leaf)]=leaf
    if deferred_row:rows.append((cfg.DEFERRED,cfg.DEFERRED+1,0x4100))
    table=b''.join(struct.pack('<III',*r) for r in rows)
    specs=[(b'.text',0x1000,bytes(text),0x60000020),(b'.defer',cfg.DEFERRED,b'\xc3',0x60000020),
        (b'.pdata',0x4000,table,0x40000040),(b'.data',cfg.SLOT&~0xfff,b'\0'*4096,0xc0000040)]
    if anchor:specs.append((b'.anchor',cfg.old.ANCHOR_SITE,bytes.fromhex('488b8100010000'),0x60000020))
    data=bytearray(1024);struct.pack_into('<H',data,0,0x5a4d);struct.pack_into('<I',data,60,0x80);data[0x80:0x84]=b'PE\0\0'
    struct.pack_into('<HH',data,0x84,0x8664,len(specs));struct.pack_into('<H',data,0x94,240);struct.pack_into('<H',data,0x98,0x20b)
    struct.pack_into('<Q',data,0x98+24,0x140000000);struct.pack_into('<I',data,0x98+56,0xD000000)
    struct.pack_into('<II',data,0x98+112+24,0x4000 if table else 0,len(table))
    for i,(name,pc,raw,flags) in enumerate(specs):
        at=0x188+40*i;data[at:at+len(name)]=name
        struct.pack_into('<IIII',data,at+8,len(raw),pc,len(raw),len(data));struct.pack_into('<I',data,at+36,flags);data.extend(raw)
    return bytes(data)


@contextmanager
def authenticated(raw):
    prior=tool.EXPECTED_SHA,c55.EXPECTED_SHA,cfg.EXPECTED_SHA
    try:
        tool.EXPECTED_SHA=c55.EXPECTED_SHA=cfg.EXPECTED_SHA=hashlib.sha256(raw).hexdigest()  # owned unit seam, never CLI
        yield
    finally:tool.EXPECTED_SHA,c55.EXPECTED_SHA,cfg.EXPECTED_SHA=prior


def session(raw=None,profile=tool.Profile(rows_per_shard=2)):
    raw=owned_pe() if raw is None else raw
    with authenticated(raw):return tool.Session(raw,profile)


def shards(s):return [s.make_shard(i) for i in range(len(s.plan))]
def aggregate(s,parts):
    a=tool.Aggregate(s)
    for p in parts:a.add(p)
    return a.finish()
def refused(fn,substring=None):
    try:fn()
    except ValueError as e:
        if substring:assert substring in str(e),(substring,str(e))
    else:raise AssertionError('invalid evidence accepted')
def reseal(p):p['payload_sha256']=tool.digest(p['payload']);return p


assert tool.intervals(7,3)==[(0,3),(3,6),(6,7)] and tool.intervals(0,3)==[]
s=session();parts=shards(s);assert s.plan==[(0,2),(2,4),(4,6)]
assert tool.encoded(parts)==tool.encoded(shards(s))
print('PASS: c56 deterministic authenticated row order, half-open partition and shard bytes')

a=aggregate(s,parts)
assert a['repository_result']==tool.READY and a['runtime_rows_covered']==6 and a['runtime_functions_not_considered']==0
assert a['main_coverage']['regions_modeled']==5 and a['main_coverage']['regions_excluded']==1
assert a['reference_candidate_count']==10 and a['reader_candidates']==a['writer_candidates']==5
assert not a['candidate_sweep_complete'] and not a['writer_enumeration_complete']
print('PASS: c56 exact full metadata coverage includes deferred row and stays distinct from all-writer proof')

for selected in [parts[:-1],parts[1:],parts+[parts[-1]],parts[:1]+[parts[0]]+parts[1:],list(reversed(parts))]:
    refused(lambda:aggregate(s,selected))
for field,value in [('interval',[0,3]),('interval',[-1,2]),('interval',[0,7]),('interval',[False,2]),('shard_id',1),('shard_id',False)]:
    wrong=deepcopy(parts);wrong[0]['payload'][field]=value;reseal(wrong[0]);refused(lambda:aggregate(s,wrong))
print('PASS: c56 missing/duplicate/overlap/out-of-range/out-of-order/boolean intervals fail closed')

for field,value in [('image_sha256','0'*64),('schema_version',99),('tool_version','v0.1c55'),
                    ('contracts',{'c55':'wrong'}),('runtime_metadata_sha256','1'*64),('tool_sources',{})]:
    wrong=deepcopy(parts);header=deepcopy(s.header);header[field]=value
    wrong[0]['payload']['manifest_id']=tool.digest(header);reseal(wrong[0]);refused(lambda:aggregate(s,wrong))
print('PASS: c56 image/schema/tool/contracts/metadata/code-source identities cannot be mixed')

wrong=deepcopy(parts);wrong[0]['payload']['rows'][0]['index']=1;reseal(wrong[0]);refused(lambda:aggregate(s,wrong))
wrong=deepcopy(parts);wrong[0]['payload']['rows'][0]['metadata_row_sha256']='0'*64;reseal(wrong[0]);refused(lambda:aggregate(s,wrong))
wrong=deepcopy(parts);wrong[0]['payload']['rows'][0]['body_code_sha256']='0'*64;reseal(wrong[0]);refused(lambda:aggregate(s,wrong))
wrong=deepcopy(parts);wrong[0]['payload']['rows'].pop();reseal(wrong[0]);refused(lambda:aggregate(s,wrong))
print('PASS: c56 missing/duplicate/stale row or code identities refuse before coverage can be certified')

for path,value in [('status','UNKNOWN'),('operation_counts',dict(READ=-1,WRITE=0,RMW=0,ADDRESS_ONLY=0)),
                   ('work',{}),('reason','invented failure')]:
    wrong=deepcopy(parts);wrong[0]['payload']['rows'][0][path]=value;reseal(wrong[0]);refused(lambda:aggregate(s,wrong))
wrong=deepcopy(parts);wrong[0]['payload']['summary_counts']['direct_entry_summary_admitted']+=1;reseal(wrong[0]);refused(lambda:aggregate(s,wrong))
wrong=deepcopy(parts);wrong[0]['payload']['rows'][0]['work']['instructions_decoded']=100000;reseal(wrong[0]);refused(lambda:aggregate(s,wrong))
print('PASS: c56 malformed/negative/nonconserving/out-of-cap row and summary counts refuse')

wrong=deepcopy(parts);wrong[0]['payload']['rows'][0]['status']='REFUSED';wrong[0]['payload']['rows'][0]['reason']='instruction_cap'
reseal(wrong[0]);refused(lambda:aggregate(s,wrong),'escapes refused region')
wrong=deepcopy(parts);wrong[0]['payload_sha256']='0'*64;refused(lambda:aggregate(s,wrong))
ag=tool.Aggregate(s);refused(lambda:ag.add(wrong[0]));refused(lambda:ag.add(parts[0]),'poisoned')
refused(ag.finish)
print('PASS: c56 refused regions cannot leak candidates and aggregate poisoning cannot be recovered by catching an error')

for main in [replace(cfg.Caps(),region_instructions=1),replace(cfg.Caps(),references=1),replace(cfg.Caps(),region_bytes=1)]:
    capped=session(profile=replace(tool.Profile(rows_per_shard=2),main=main));r=aggregate(capped,shards(capped))
    assert r['runtime_rows_covered']==6 and r['runtime_functions_not_considered']==0
    assert r['main_coverage']['regions_refused']==5 and not r['reference_candidates'] and r['reference_candidate_count']==0
print('PASS: c56 local instruction/reference/byte caps mark rows considered-and-refused instead of skipping later rows')

for limits in [dict(work=1),dict(instructions=1)]:
    capped=session(profile=replace(tool.Profile(rows_per_shard=2),summary=replace(c55.Caps(),**limits)))
    r=aggregate(capped,shards(capped));assert r['main_coverage']['regions_refused']==5 and not r['reference_candidates']
    assert r['runtime_rows_covered']==6 and r['row_refusal_reasons']['summary_cap_refused']==5
    if 'work' in limits:assert r['shards_summary_budget_exhausted']>0
    else:assert r['direct_entry_cache_hits']>0
print('PASS: c56 global and cached local summary caps remain closed; whole affected rows refuse without omissions')

for key in tool.WORK:
    assert a['main_coverage'][key]==sum(r['work'][key] for p in parts for r in p['payload']['rows'])
for key in tool.SUMMARY:assert a[key]==sum(p['payload']['summary_counts'][key] for p in parts)
assert a['call_edges_with_any_preserved_origin']==a['preserved_origin_register_facts_across_calls']==5
assert a['total_main_and_summary_instructions_decoded']==a['main_coverage']['instructions_decoded']+a['c54_runtime_instructions_decoded']+a['direct_entry_instructions_decoded']+a['call_edge_verification_instructions_decoded']
assert a['aggregate_validation_candidate_decode_attempts']==10
assert 'global_unique_callees' in a['count_scope']
print('PASS: c56 work/summary/access counters are exact additive sums with separate validation and nonunique scopes')

for ref in a['reference_candidates']:
    index=ref['runtime_row_index'];assert ref['source_shard']==index//2 and ref['runtime_row_sha256']==s.row_identity(index)
    assert ref['runtime_function']['begin_rva']==s.rows[index][0] and ref['cfg']['fixpoint_complete']
for field,value in [('source_shard',1),('runtime_row_index',5),('instruction_sha256','0'*64),('operation','RMW')]:
    wrong=deepcopy(parts);wrong[0]['payload']['candidate_records'][0][field]=value;reseal(wrong[0]);refused(lambda:aggregate(s,wrong))
wrong=deepcopy(parts);wrong[0]['payload']['candidate_records'][0]['raw_proprietary_body']='not allowed';reseal(wrong[0]);refused(lambda:aggregate(s,wrong))
print('PASS: c56 candidate row/shard/CFG/actual instruction/access provenance is retained and malformed records refuse')

limited=session(profile=replace(tool.Profile(rows_per_shard=2),records_per_shard=1,records_total=2))
r=aggregate(limited,shards(limited));assert r['reference_candidate_count']==10 and len(r['reference_candidates'])==2 and r['candidate_records_not_retained']==8
assert r['runtime_functions_not_considered']==0
print('PASS: c56 record storage bounds truncate only records, preserve exact counts and never stop later rows')

raw=owned_pe(1,deferred_row=False)
with authenticated(raw):
    one=tool.Session(raw,tool.Profile(rows_per_shard=1));r=aggregate(one,shards(one));old=c55.analyze_image(raw)
    assert r['reference_candidate_count']==len(old['reference_candidates'])==2
    assert r['direct_entry_summary_admitted']==old['direct_entry_summary_admitted']==1
    for current,prior in zip(r['reference_candidates'],old['reference_candidates']):
        assert (current['site_rva'],current['operation'],current['address_proof_chain'])==(prior['site_rva'],prior['operation'],prior['address_proof_chain'])
print('PASS: c56 single-row shard preserves actual c55 direct-entry candidates and never-written address proof')

for leaf in ['b901000000c3','ffe0','ebfe','51b90100000059c3']:
    t=session(owned_pe(2,leaf=leaf));r=aggregate(t,shards(t));assert not r['reference_candidates'] and r['runtime_rows_covered']==3
print('PASS: c56 retains c55 RCX clobber/indirect/cycle/write-restore refusals inside shards')

reads=[];base=s.read
def watched(pc,n):reads.append((pc,n));return base(pc,n)
s.read=s.domain.read=watched
r=aggregate(s,shards(s))
assert all(not (pc<cfg.DEFERRED+1 and cfg.DEFERRED<pc+n) for pc,n in reads)
assert all(not (pc<cfg.SLOT+8 and cfg.SLOT<pc+n) for pc,n in reads)
assert all(not r[k] for k in tool.FALSE_GATES) and r['live_qword_reads']==0
print('PASS: c56 deferred producer and slot bytes stay unread; pointer chase/runtime/watch/gameplay gates remain false')

t=session(owned_pe(5,anchor=True));r=aggregate(t,shards(t))
assert len(r['external_anchors'])==1 and not r['anchor_counted_as_local_candidate'] and r['reference_candidate_count']==10
assert r['anchor_decode_attempts_once']==r['anchor_instructions_decoded_once']==1 and r['anchor_file_code_bytes_loaded_once']==7
assert not any(ref['site_rva']==cfg.old.ANCHOR_SITE for ref in r['reference_candidates'])
print('PASS: c56 external c28 anchor is validated once, never duplicated into shards/local coverage or caller state')

with tempfile.TemporaryDirectory(prefix='c56 resume ') as tmp:
    cache=Path(tmp)/'cache';r1=tool.run(s,cache)
    snapshots={p.name:p.read_bytes() for p in cache.iterdir()}
    old=s.make_shard
    def no_reanalysis(index):raise AssertionError('completed checkpoint reanalysed')
    s.make_shard=no_reanalysis
    r2=tool.run(s,cache);s.make_shard=old
    assert tool.encoded(r1)==tool.encoded(r2) and snapshots=={p.name:p.read_bytes() for p in cache.iterdir()}
print('PASS: c56 complete resume validates without rerunning shards and emits byte-identical aggregate')

with tempfile.TemporaryDirectory(prefix='c56 partial ') as tmp:
    cache=Path(tmp)/'cache';cache.mkdir();tool.atomic_json(cache/'manifest.json',s.manifest)
    start,end=s.plan[0];tool.atomic_json(cache/f'shard-{start:06d}-{end:06d}.json',s.make_shard(0))
    (cache/'.c56-interrupted.tmp').write_text('incomplete temporary write')
    called=[];old=s.make_shard
    def capture(index):called.append(index);return old(index)
    s.make_shard=capture;r=tool.run(s,cache);s.make_shard=old
    assert called==[1,2] and tool.encoded(r)==tool.encoded(a)
print('PASS: c56 partial/crash resume reuses only validated committed shards and completes missing intervals')

with tempfile.TemporaryDirectory() as tmp:
    cache=Path(tmp);tool.run(s,cache);manifest=tool.read_json(cache/'manifest.json')
    manifest['header']['schema_version']=99;tool.atomic_json(cache/'manifest.json',manifest)
    refused(lambda:tool.run(s,cache),'manifest')
with tempfile.TemporaryDirectory() as tmp:
    cache=Path(tmp);tool.run(s,cache);(cache/'shard-999999-999999.json').write_text('{}')
    refused(lambda:tool.run(s,cache),'unexpected')
with tempfile.TemporaryDirectory() as tmp:
    cache=Path(tmp);(cache/'shard-000000-000002.json').write_text('{}')
    refused(lambda:tool.run(s,cache),'without authenticated manifest')
print('PASS: c56 stale manifest/unexpected shards/orphan evidence refuses instead of silently recomputing or mixing runs')

with tempfile.TemporaryDirectory() as tmp:
    p=Path(tmp)/'bad.json'
    for text in ['{"a":1,"a":2}','{"a":NaN}','incomplete']:
        p.write_text(text);refused(lambda:tool.read_json(p))
    p.write_bytes(b' '* (tool.MAX_JSON+1));refused(lambda:tool.read_json(p),'capped')
    p.unlink();p.symlink_to(Path(tmp)/'missing');refused(lambda:tool.read_json(p))
print('PASS: c56 duplicate/nonfinite/truncated/oversize/symlink checkpoint JSON refuses')

original=tool.source_identity
try:
    tool.source_identity=lambda:{'changed':'0'*64}
    refused(lambda:s.make_shard(0),'changed during scan')
finally:tool.source_identity=original
for profile in [replace(tool.Profile(),rows_per_shard=True),replace(tool.Profile(),rows_per_shard=129),
                replace(tool.Profile(),main=replace(cfg.Caps(),region_instructions=2048))]:
    refused(profile.check)
print('PASS: c56 changed scanner sources and widened/malformed profile caps cannot resume or certify evidence')

empty=session(owned_pe(0,deferred_row=False));r=aggregate(empty,[])
assert r['runtime_rows_covered']==r['shard_count']==r['runtime_functions_not_considered']==0 and not r['writer_enumeration_complete']
assert all(r[k]==0 for k in (*tool.SUMMARY,*tool.CALLS))
print('PASS: c56 empty metadata edge case is deterministic coverage only, never writer completeness')

raw=owned_pe(3,anchor=True)
with authenticated(raw),tempfile.TemporaryDirectory(prefix='c56 CLI ') as tmp:
    exe,out=Path(tmp)/'owned.exe',Path(tmp)/'result.json';exe.write_bytes(raw);args=sys.argv
    try:
        sys.argv=[tool.__file__,str(exe),'--output',str(out)];capture,progress=io.StringIO(),io.StringIO()
        with redirect_stdout(capture),redirect_stderr(progress):tool.main()
        assert capture.getvalue().encode()==out.read_bytes() and len(capture.getvalue().splitlines())==1
        assert 'verified shards' in progress.getvalue() and json.loads(out.read_text())['runtime_functions_not_considered']==0
    finally:sys.argv=args
print('PASS: c56 one CLI command performs all shards/aggregation with compact JSON and separate progress')

with authenticated(raw),tempfile.TemporaryDirectory(prefix='c56 bad cache ') as tmp:
    exe,out=Path(tmp)/'owned.exe',Path(tmp)/'result.json';exe.write_bytes(raw);out.write_text('keep')
    cache=Path(tmp)/'cache';current=tool.Session(raw);tool.run(current,cache)
    path=next(cache.glob('shard-*.json'));bad=tool.read_json(path);bad['payload_sha256']='0'*64;tool.atomic_json(path,bad)
    preserved=path.read_bytes();args=sys.argv
    try:
        sys.argv=[tool.__file__,str(exe),'--output',str(out),'--checkpoint-dir',str(cache)];capture=io.StringIO()
        with redirect_stdout(capture):
            try:tool.main()
            except SystemExit as error:assert error.code==2
            else:raise AssertionError('corrupt cache was silently replaced')
        failure=json.loads(capture.getvalue());assert failure['repository_result']==tool.BLOCKED and failure['image_authenticated'] and not failure['aggregate_published']
        assert out.read_text()=='keep' and path.read_bytes()==preserved
    finally:sys.argv=args
print('PASS: c56 authenticated CLI corrupt-cache failure preserves both old aggregate and rejected evidence')

with tempfile.TemporaryDirectory() as tmp:
    exe,out=Path(tmp)/'owned.exe',Path(tmp)/'result.json';exe.write_bytes(raw);out.write_text('keep')
    p=subprocess.run([sys.executable,tool.__file__,str(exe),'--output',str(out)],capture_output=True,text=True)
    assert p.returncode==2 and out.read_text()=='keep' and json.loads(p.stdout)['repository_result']==tool.BLOCKED
    assert not json.loads(p.stdout)['image_authenticated'] and not out.with_suffix('.shards').exists()
print('PASS: c56 CLI has no exact-image bypass; failed authentication preserves old aggregate and creates no cache')
