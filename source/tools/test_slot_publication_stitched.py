"""Owned c58 entry-instance/certificate/cap tests, never real-image evidence."""
from contextlib import contextmanager
from copy import deepcopy
from dataclasses import asdict, replace
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

import slot_publication_stitched as tool

c57,c56,cfg=tool.c57,tool.c56,tool.cfg


def jump(pc,target,conditional=False):
    prefix=bytes.fromhex('0f85') if conditional else b'\xe9'
    return prefix+struct.pack('<i',target-pc-len(prefix)-4)


def lea(pc,target=cfg.SLOT,reg=0):
    return bytes((0x48,0x8d,0x05+8*reg))+struct.pack('<i',target-pc-7)


def owned_pe(functions,extra=()):
    functions=dict(functions);functions[cfg.DEFERRED]=b'\xc3'
    rows=sorted((pc,pc+len(raw),0x6100) for pc,raw in functions.items())
    table=b''.join(struct.pack('<III',*r) for r in rows)
    specs=[(f'.f{i}'.encode(),pc,raw,0x60000020,len(raw)) for i,(pc,raw) in enumerate(sorted(functions.items()))]
    specs.extend(extra)
    specs.extend([(b'.pdata',0x6000,table,0x40000040,len(table)),
        (b'.data',cfg.SLOT&~0xfff,b'\0'*4096,0xc0000040,4096)])
    data=bytearray(4096);struct.pack_into('<H',data,0,0x5a4d);struct.pack_into('<I',data,60,0x80);data[0x80:0x84]=b'PE\0\0'
    struct.pack_into('<HH',data,0x84,0x8664,len(specs));struct.pack_into('<H',data,0x94,240);struct.pack_into('<H',data,0x98,0x20b)
    struct.pack_into('<Q',data,0x98+24,0x140000000);struct.pack_into('<I',data,0x98+56,0xD000000)
    struct.pack_into('<II',data,0x98+112+24,0x6000,len(table))
    for i,(name,pc,raw,flags,virtual) in enumerate(specs):
        at=0x188+40*i;data[at:at+len(name)]=name
        struct.pack_into('<IIII',data,at+8,virtual,pc,len(raw),len(data));struct.pack_into('<I',data,at+36,flags);data.extend(raw)
    return bytes(data),len(rows)


@contextmanager
def accepted_image(raw):
    prior=c56.EXPECTED_SHA
    c56.EXPECTED_SHA=c56.sha(raw)  # internal owned seam; CLI has no bypass
    try:yield
    finally:c56.EXPECTED_SHA=prior


def evidence(functions,extra=()):
    raw,count=owned_pe(functions,extra)
    with accepted_image(raw):
        session=c56.Session(raw)
        parts=[session.make_shard(i) for i in range(len(session.plan))]
        boundary=sum(r['reason']=='unsupported_runtime_boundary' for p in parts for r in p['payload']['rows'])
        baseline=c57.collect_baseline(session,acceptance=c57.Acceptance(count,boundary))
        classifier=c57.Classifier(baseline);agg=c57.Aggregate(classifier)
        for w in baseline.witnesses:agg.add(classifier.record(w))
        result=agg.finish();relations=result['target_metadata_relation_counts']
        a=tool.AcceptedEdges(result['boundary_edges_total'],relations['exact_unique_runtime_begin'],
            relations['inside_unique_runtime_span_nonentry'],relations['executable_file_backed_outside_runtime_metadata'],
            result['model_only_boundary_edges_total'],sum(result['exact_adjacency_edge_kind_counts'].values()))
        return tool.Evidence(baseline,accepted=a,owner_result=result),raw,baseline,a


def scan(functions,extra=(),limits=tool.Limits()):
    e,_,_,_=evidence(functions,extra)
    return tool.scan(e,limits)


def refused(fn,text=None):
    try:fn()
    except ValueError as error:
        if text:assert text in str(error),(text,str(error))
    else:raise AssertionError('unsafe evidence admitted')


def refs(result,operation=None):
    return [r for r in result['reference_candidates'] if operation is None or r['operation']==operation]


r=scan({0x1000:lea(0x1000),0x1007:bytes.fromhex('488b08c3')})
assert len(refs(r,'READ'))==1 and r['address_facts_crossing_runtime_rows']==1
ref=refs(r)[0]
assert ref['boundary_path'][0]['edge_kind']=='ordinary_fallthrough' and ref['address_proof_chain']==((0x1000,'rip_lea',cfg.SLOT),)
assert ref['effective_rva']==cfg.SLOT and ref['incoming_state_identity'] and ref['entry_instance_identity']
print('PASS: c58 ordinary adjacent boundary carries exact fact with separate unchanged chain and boundary proof')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1000,True),0x100d:bytes.fromhex('488b08c3')})
assert refs(r,'READ') and refs(r)[0]['boundary_path'][0]['edge_kind']=='conditional_fallthrough'
print('PASS: c58 conditional adjacent fallthrough carries only converged must state')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1100,True)+b'\xc3',0x1100:bytes.fromhex('488908c3')})
assert len(refs(r,'WRITE'))==1 and refs(r)[0]['boundary_path'][0]['edge_kind']=='direct_conditional_taken'
print('PASS: c58 nonadjacent direct conditional taken edge yields encoding-verified writer candidate')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1200),0x1200:bytes.fromhex('480108c3')})
assert len(refs(r,'RMW'))==1 and refs(r)[0]['boundary_path'][0]['target_begin_rva']==0x1200
print('PASS: c58 nonadjacent unconditional branch preserves existing exact-slot RMW classification')

# Seed RAX and RSP; callee writes RAX, never writes RBX; RSP always killed.
root=lea(0x1000,reg=0)+lea(0x1007,reg=3)+lea(0x100e,reg=4)
root+=b'\xe8'+struct.pack('<i',0x1200-0x101a)
r=scan({0x1000:root,0x101a:bytes.fromhex('488b0b488b08488b0c24c3'),0x1200:bytes.fromhex('31c0c3')})
assert len(refs(r,'READ'))==1 and refs(r)[0]['site_rva']==0x101a
assert r['call_edges_with_preserved_facts_crossing_rows']==1
print('PASS: c58 CALL fallthrough uses authenticated c55 never-written banks only and kills RAX/RSP')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1101),0x1100:bytes.fromhex('90488b08c3')})
assert not refs(r) and r['root_refusal_reasons']['inside_unique_runtime_span_nonentry']==1
print('PASS: c58 interior runtime target never stitches')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1300)},[(b'.free',0x1300,b'\x48\x8b\x08\xc3',0x60000020,4)])
assert not refs(r) and r['root_refusal_reasons']['executable_file_backed_outside_runtime_metadata']==1
print('PASS: c58 metadata-free RX target refuses instead of inventing a function entry')

for target,extra in [(0x1301,[(b'.unbacked',0x1300,b'\x90',0x60000020,20)]),
                     (0x1300,[(b'.rw',0x1300,b'\xc3',0xe0000020,1)]),(cfg.DEFERRED,[])]:
    refused(lambda:evidence({0x1000:lea(0x1000)+jump(0x1007,target)},extra),'distribution mismatch')
print('PASS: c58 unbacked/writable/deferred domains fail evidence admission without reading target values')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0xD000010)})
assert not refs(r)  # old oracle unresolved target is not even in the c57 first-boundary set
refused(lambda:evidence({0x1000:b'\x90'*8,0x1004:b'\xc3'}))
print('PASS: c58 outside-image and overlapping metadata cannot produce certified continuation')

r=scan({0x1000:lea(0x1000)+b'\xf2'+jump(0x1008,0x1200),0x100d:b'\xc3',0x1200:bytes.fromhex('488b08c3')})
assert not refs(r) and r['root_refusal_reasons']['model_only_successor']==1
print('PASS: c58 decorated JMP synthetic successor rejects the root and cannot leak architectural-arm candidates')

functions={0x1000:lea(0x1000)+jump(0x1007,0x1200),0x1200:bytes.fromhex('488b08c3')}
e,_,_,_=evidence(functions)
r=tool.scan(e)
independent=cfg.scan_region(e.session.read(0x1200,4),0x1200)
assert not independent['references'] and refs(r,'READ') and not r['historical_c56_candidates']['reference_candidate_count']
print('PASS: c58 target independent EMPTY root remains separate and historical c56 candidate output unchanged')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1200),0x1100:lea(0x1100,cfg.SLOT+8)+jump(0x1107,0x1200),0x1200:bytes.fromhex('488b08c3')})
assert len(refs(r,'READ'))==1 and refs(r)[0]['original_root_runtime_row']==0
print('PASS: c58 different authenticated roots never merge their incoming target facts')

# Same root forks to two certified rows which reach a common target with different states.
r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1100,True),
    0x100d:bytes.fromhex('488d4008')+jump(0x1011,0x1200),
    0x1100:jump(0x1100,0x1200),0x1200:bytes.fromhex('488b08c3')})
assert len([a for a in refs(r,'READ') if a['original_root_runtime_row']==0])==1
assert r['continuation_instances_created']>=4
print('PASS: c58 same-root different incoming states remain distinct instances without fabricated join facts')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1100,True),0x100d:jump(0x100d,0x1200),
    0x1100:jump(0x1100,0x1200),0x1200:bytes.fromhex('488b08c3')})
assert len(refs(r,'READ'))==1 and r['continuation_instances_deduplicated']>=1
assert r==scan({0x1000:lea(0x1000)+jump(0x1007,0x1100,True),0x100d:jump(0x100d,0x1200),0x1100:jump(0x1100,0x1200),0x1200:bytes.fromhex('488b08c3')})
print('PASS: c58 identical same-root incoming states deduplicate deterministically')

# In-row diamond: one arm kills RAX. Both arms agree only in the unchanged version.
for kill,expected in [(bytes.fromhex('31c0'),0),(bytes.fromhex('9090'),1)]:
    body=bytes.fromhex('7504')+kill+bytes.fromhex('eb02')+b'\x90\x90'+bytes.fromhex('488b08c3')
    r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1200),0x1200:body})
    assert len(refs(r,'READ'))==expected
print('PASS: c58 within-instance must join retains exact agreement and discards one-arm clobber')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:bytes.fromhex('488b08')+jump(0x1103,0x1200),0x1200:jump(0x1200,0x1100)})
assert refs(r,'READ') and r['cross_row_cycles_seen'] and r['continuation_instances_deduplicated']
print('PASS: c58 exact-state cross-row cycle converges without changing Fact or duplicating candidate')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:bytes.fromhex('488b08')+jump(0x1103,0x1200),0x1200:bytes.fromhex('488d4008')+jump(0x1204,0x1100)})
assert not refs(r) and r['root_refusal_reasons']['state_changing_cross_row_cycle'] and r['continuation_instances_refused']
print('PASS: c58 state-changing cycle discards earlier provisional candidates from the entire root')

f={0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:bytes.fromhex('488b08')+jump(0x1103,0x1200),0x1200:b'\xc3'}
for name in asdict(tool.Limits()):
    if name=='retained_references':continue
    r=scan(f,limits=replace(tool.Limits(),**{name:1}))
    if name=='root_references':continue  # dedicated multiple-candidate case below
    assert not refs(r) and r['cross_row_cap_refusals'],(name,r)
r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:bytes.fromhex('488b08488908c3')},limits=replace(tool.Limits(),root_references=1))
assert not refs(r) and r['cross_row_cap_refusals']
print('PASS: c58 instance/transition/byte/instruction/edge/state/iteration/block/path/reference caps fail closed')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:bytes.fromhex('488b08488908c3')},limits=replace(tool.Limits(),retained_references=1))
assert r['reference_candidate_count']==2 and len(refs(r))==1 and r['candidate_records_not_retained']==1
print('PASS: c58 bounded retained output truncates records explicitly without falsifying exact candidate counts')

# c57 only certifies first heap-ordered boundary; a later boundary is not guessed.
r=scan({0x1000:lea(0x1000)+bytes.fromhex('7505')+jump(0x1009,0x1100)+jump(0x100e,0x1200),0x1100:bytes.fromhex('488b08c3'),0x1200:b'\xc3'})
assert not refs(r) and r['root_refusal_reasons']['uncertified_later_runtime_boundary']
print('PASS: c58 later unclassified source boundary refuses rather than extrapolating c57 first-refusal certificate')

for body,expected in ((bytes.fromhex('488b00488b08c3'),1),(bytes.fromhex('488b4808c3'),0),(bytes.fromhex('89c0488b08c3'),0),(bytes.fromhex('488b0c08c3'),0)):
    r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:body})
    assert len(refs(r))==expected
    if expected:assert refs(r)[0]['site_rva']==0x1100
print('PASS: c58 loaded base/adjacent non-slot/partial mutation/indexed addressing never gain pointer facts')

r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:bytes.fromhex('488d08488b09c3')})
assert refs(r,'ADDRESS_ONLY') and refs(r,'READ') and len(refs(r,'READ')[0]['address_proof_chain'])==2
print('PASS: c58 existing affine LEA propagation preserves original derivation hop semantics after crossing')

for kind in ('indirect_call','indirect_jump'):
    body=bytes.fromhex('ffd0') if kind=='indirect_call' else bytes.fromhex('ffe0')
    r=scan({0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:bytes.fromhex('488b08')+body})
    assert not refs(r) and r['root_refusal_reasons'][kind]
print('PASS: c58 unknown indirect CALL/JMP aborts root including earlier local candidate')

r=scan({0x1000:lea(0x1000)+b'\xf2'+jump(0x1008,0x1200)+b'\xc3',0x1200:bytes.fromhex('488b08c3')})
assert not refs(r) and r['root_refusal_reasons']['model_only_successor']
print('PASS: c58 model-only fallthrough inside source row is refused too, never used as a normal path')

r=scan({0x1000:lea(0x1000)+bytes.fromhex('31c0')+jump(0x1009,0x1200),0x1200:bytes.fromhex('488b08c3')})
assert not refs(r) and r['continuation_instances_created']==0 and r['no_carried_fact_edges']==1
print('PASS: c58 no-fact boundary cannot seed a new independent target scan')

e,_,_,_=evidence({0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:bytes.fromhex('488b08e8')+struct.pack('<i',0x1200-0x1108)+b'\xc3',0x1200:b'\xc3'})
class Exhausted:
    def for_site(self,*args):return dict(status='REFUSED',refusal_reason='cached_summary_work_cap',preserved_banks=[])
tx=tool.Transaction(e,0,Exhausted(),tool.Limits())
refused(tx.run,'cap')
# Candidate emission follows the complete local fixedpoint, so even a local
# read preceding this CALL cannot escape its failed continuation context.
assert not tx.references
print('PASS: c58 cached summary-cap refusal aborts without publishing earlier local candidates')

for field in ('schema_version','contract','image_sha256','c56_manifest_id','accepted_c57_boundary_identity'):
    e,_,_,_=evidence(functions);e.header[field]='wrong'
    refused(lambda:tool.scan(e),'identity')
print('PASS: c58 stale/wrong schema/contract/image/c56/c57 identity refuses globally')

for field,value in [('row_sha256','0'*64),('code_sha256','0'*64),('pc',0x1008),('instruction_sha256','0'*64),('edges',())]:
    e,_,_,_=evidence(functions);index=next(iter(e.proofs));e.proofs[index]=replace(e.proofs[index],**{field:value})
    refused(lambda:tool.scan(e),'identity')
e,_,_,_=evidence(functions);e.header['tool_sources']['slot_publication_cfg.py']='wrong'
refused(lambda:tool.scan(e),'identity')
print('PASS: c58 mutated row/code/instruction/edge/tool proof cannot become a local permissive fallback')

e,raw,b,a=evidence(functions)
wrong=deepcopy(e.result);wrong['classifier_header']['schema_version']=99
refused(lambda:tool.Evidence(b,accepted=a,owner_result=wrong),'c57 JSON')
refused(lambda:tool.Evidence(b),'distribution mismatch')
refused(lambda:tool.analyze(raw),'SHA256')
print('PASS: c58 owner JSON exact replay and fixed accepted distribution/image guard have no CLI override')

with tempfile.TemporaryDirectory() as directory:
    cache=Path(directory)/'cache';cache.mkdir();c56.atomic_json(cache/'manifest.json',e.session.manifest)
    for i,(start,end) in enumerate(e.session.plan):c56.atomic_json(cache/f'shard-{start:06d}-{end:06d}.json',e.session.make_shard(i))
    before={p.name:p.read_bytes() for p in cache.iterdir()}
    with accepted_image(raw):baseline=c57.collect_baseline(e.session,cache,acceptance=b.acceptance)
    again=tool.Evidence(baseline,accepted=a,owner_result=e.result)
    assert tool.scan(again)==tool.scan(e)
    assert before=={p.name:p.read_bytes() for p in cache.iterdir()}
    shard=next(cache.glob('shard-*.json'));shard.write_text('{}')
    refused(lambda:c57.collect_baseline(e.session,cache,acceptance=b.acceptance))
print('PASS: c58 complete valid c56 checkpoints are read-only and corrupt evidence never recomputes silently')

r=tool.scan(e)
assert all(r[k] is False for k in c56.FALSE_GATES) and r['live_qword_reads']==0
assert not r['anchor_counted_as_local_candidate'] and r['c52_production_watch_gate'].endswith('_FAIL')
assert r['publication_source_kind']=='unproven' and r['exact_runtime_begin_edges_admitted']+r['exact_runtime_begin_edges_refused']==r['exact_runtime_begin_edges_considered']
print('PASS: c58 all completeness/ownership/production gates remain false with explicit instance/count scope')

for args in (['--help'],['missing.exe','--output','out.json']):
    p=subprocess.run([sys.executable,str(Path(tool.__file__)),*args],capture_output=True,text=True)
    assert p.returncode==(0 if args==['--help'] else 2)
    assert ('--c57-json' in p.stdout) if args==['--help'] else (tool.BLOCKED in p.stderr)
print('PASS: c58 CLI documents read-only inputs and missing private image returns BLOCKED without fabricated JSON')
