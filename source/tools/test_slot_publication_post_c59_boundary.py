"""Owned c61 terminal POST observer/identity/aggregation tests, no private EXE."""
from contextlib import contextmanager
from collections import Counter
from copy import deepcopy
from pathlib import Path
from types import SimpleNamespace
import json
import struct
import subprocess
import sys
import tempfile

import slot_publication_post_c59_boundary as tool
c60,c59,c58,c57,c56,cfg=tool.c60,tool.c59,tool.c58,tool.c57,tool.c56,tool.cfg

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


def evidence(functions,extra=(),mutate=None):
    raw,count=owned_pe(functions,extra)
    if mutate is not None:raw=mutate(raw)
    with accepted_image(raw):
        session=c56.Session(raw)
        parts=[session.make_shard(i) for i in range(len(session.plan))]
        boundary=sum(r['reason']=='unsupported_runtime_boundary' for p in parts for r in p['payload']['rows'])
        total=c56.Aggregate(session)
        for part in parts:total.add(part)
        counts=total.finish()
        accepted=c57.Acceptance(count,boundary,*[counts[k] for k in ('reference_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates')])
        baseline=c57.collect_baseline(session,acceptance=accepted)
        classifier=c57.Classifier(baseline);agg=c57.Aggregate(classifier)
        for w in baseline.witnesses:agg.add(classifier.record(w))
        result=agg.finish();relations=result['target_metadata_relation_counts']
        a=c58.AcceptedEdges(result['boundary_edges_total'],relations['exact_unique_runtime_begin'],
            relations['inside_unique_runtime_span_nonentry'],relations['executable_file_backed_outside_runtime_metadata'],
            result['model_only_boundary_edges_total'],sum(result['exact_adjacency_edge_kind_counts'].values()))
        return c58.Evidence(baseline,accepted=a,owner_result=result),raw,baseline,a


def pool(functions,extra=(),mutate=None):
    e,raw,_,_=evidence(functions,extra,mutate)
    original=c58.scan(e)
    b=c59.Baseline(e,accepted=original,owner_result=original)
    r=c59.classify(b)
    return c60.ProofPool(b,accepted=r,owner_result=r),raw


def functions(tail=b'\x90',following=None,roots=None):
    f={0x1000:lea(0x1000)+jump(0x1007,0x1100),
        0x1100:bytes.fromhex('7505')+jump(0x1102,0x1200)+tail,0x1200:b'\xc3'}
    if following is not None:f[0x1107+len(tail)]=following
    if roots:f.update(roots)
    return f


def scan(f,extra=(),limits=c58.Limits()):
    p,_=pool(f,extra)
    return c60.scan(p,limits)


def refused(fn,text=None):
    try:fn()
    except ValueError as error:
        if text:assert text in str(error),(text,str(error))
    else:raise AssertionError('unmatched later-edge evidence accepted')


def refs(r,operation=None):
    return [x for x in r['reference_candidates'] if operation is None or x['operation']==operation]


def baseline(f,extra=()):
    p,raw=pool(f,extra)
    original=c60.scan(p)
    return tool.Baseline(p,accepted=original,owner_result=original),raw


def post_functions(body=b'\x90',next_body=b'\xc3',targets=None,roots=None):
    f=functions(following=body,roots=roots)
    if next_body is not None:f[0x1108+len(body)]=next_body
    if targets:f.update(targets)
    return f


def values(b):return list(tool.replay_records(b))


def aggregate(b,records=None,limit=8):
    a=tool.Aggregate(b,limit)
    for record in values(b) if records is None else records:a.add(record)
    return a.finish()


b,raw=baseline(post_functions())
v=values(b);r=aggregate(b,v)
assert len(v)==b.result['post_c59_uncertified_boundary_refusals']==1
assert r['post_c59_boundary_instances_total']==r['post_c59_uncertified_boundary_refused_roots_classified']==1
assert r['edge_kind_counts']['ordinary_fallthrough']==1 and r['target_metadata_relation_counts']['exact_unique_runtime_begin']==1
assert sum(r['exact_adjacency_counts'].values())==1
assert v[0]['source_instruction_rva']==0x1108 and v[0]['current_entry_instance_row']==2
assert v[0]['c59_record_identities'] and v[0]['c59_classifier_identities']
assert v[0]['post_c59_origin']['arrival_path']['c59_record_identities']
print('PASS: c61 exact first terminal POST ordinary adjacent edge is bound to root/entry seed/full path/source and c59 origin')

# Original c57 proof for that row exists; c60 refuses its new post-c59 path.
row=v[0]['current_entry_instance_row']
assert b.pool.baseline.evidence.proofs[row].pc==v[0]['source_instruction_rva']
assert r['new_post_c59_boundaries_admitted']==r['new_cross_row_candidate_count']==0
assert all(x['first_refusal']==c60.POST for x in v)
print('PASS: c61 observes a c57-known source only as the actual c60 POST event, never reuses its row-global certificate')

for kind,body,next_body,targets in [
    ('direct_unconditional_branch',jump(0x1108,0x1400),None,{0x1400:b'\xc3'}),
    ('direct_conditional_taken',jump(0x1108,0x1400,True)+b'\xc3',None,{0x1400:b'\xc3'}),
    ('conditional_fallthrough',jump(0x1108,0x1108,True),b'\xc3',{}),
    ('call_fallthrough',b'\xe8'+struct.pack('<i',0x1200-0x110d),b'\xc3',{})]:
    bb,_=baseline(post_functions(body,next_body,targets));rr=aggregate(bb)
    assert rr['edge_kind_counts'][kind]==1 and rr['new_post_c59_boundaries_admitted']==0
    if kind=='call_fallthrough':assert rr['examples_by_edge_kind'][kind][0]['edge_target_rva']==0x110d
    assert sum(rr['exact_adjacency_counts'].values())==(1 if kind in ('conditional_fallthrough','call_fallthrough') else 0)
print('PASS: c61 direct/conditional taken/fallthrough and CALL return-site successors classify without target traversal or call semantics')

bb,_=baseline(post_functions(jump(0x1108,0x1400,True),b'\xc3',{0x1400:b'\xc3'}));rr=aggregate(bb)
assert rr['post_c59_boundary_instances_total']==1 and rr['post_c59_boundary_edges_total']==2
assert rr['edge_kind_counts']['direct_conditional_taken']==rr['edge_kind_counts']['conditional_fallthrough']==1
print('PASS: c61 one first terminal instance may classify two edges without equating edge and root counts')

bb,_=baseline(post_functions(jump(0x1108,0x1401),None,{0x1400:b'\x90\xc3'}))
assert aggregate(bb)['target_metadata_relation_counts']['inside_unique_runtime_span_nonentry']==1
bb,_=baseline(post_functions(jump(0x1108,0x1400),None),[(b'.free',0x1400,b'\xc3',0x60000020,1)])
assert aggregate(bb)['target_metadata_relation_counts']['executable_file_backed_outside_runtime_metadata']==1
print('PASS: c61 interior and metadata-free RX targets remain metadata-only diagnostics without nearest-entry inference')

bb,_=baseline(post_functions(b'\xf2'+jump(0x1109,0x1400),b'\xc3',{0x1400:b'\xc3'}));rr=aggregate(bb)
assert rr['architectural_post_c59_boundary_edges_total']==rr['model_only_post_c59_boundary_edges_total']==1
assert rr['edge_kind_counts']['other_supported_direct_successor']==1
print('PASS: c61 decorated JMP synthetic successor is explicitly model-only, never architectural or admitted')

# Some classes cannot reach accepted POST because upstream image/oracle/c57
# guards reject them. Test the unchanged vocabulary separately rather than
# weakening a historical admission or fabricating a POST context.
s=b.pool.baseline.evidence.session
for target,domain,relation in [
    (0x1405,c58.c55.Domain(image_id='owned',rows=[(0x1400,0x1410,0),(0x1404,0x1420,0)],sections=[],image_size=s.size,excluded=[],read=lambda *_:None),'overlapping_multiple_runtime_spans'),
    (0x1401,c58.c55.Domain(image_id='owned',rows=[],sections=[(0x1400,0x1420,0,1,0x60000020)],image_size=s.size,excluded=[],read=lambda *_:None),'executable_not_fully_file_backed'),
    (0x1400,c58.c55.Domain(image_id='owned',rows=[],sections=[(0x1400,0x1401,0,1,0xc0000040)],image_size=s.size,excluded=[],read=lambda *_:None),'non_executable_or_writable_section'),
    (s.size+1,s.domain,'outside_image'),(cfg.DEFERRED,s.domain,'deferred_producer_excluded')]:
    ins=next(cfg.old.decoder().disasm(jump(0x1108,target),0x1108,count=1))
    edge=c59.classify_edges(SimpleNamespace(domain=domain),0x1108,0x110d,ins)[0]
    assert edge['target_metadata_relation']==relation
refused(lambda:baseline({0x1100:b'\x90'*8,0x1104:b'\xc3'}))
print('PASS: c61 all overlap/unbacked/nonexec/outside/deferred relation helpers tested without bypassing earlier PE/oracle/c57 refusals')

# No positive step is inferred from an authenticated incoming seed.
bb,_=baseline(post_functions(bytes.fromhex('488b08')+b'\x90',b'\xc3',roots={0x1080:lea(0x1080,cfg.SLOT+8)+jump(0x1087,0x1100)}))
records=values(bb)
assert len(records)==2 and len({x['source_instruction_rva'] for x in records})==1
assert len({x['incoming_state_identity'] for x in records})==2 and len({x['record_identity'] for x in records})==2
assert aggregate(bb)['unique_source_rows']==1 and aggregate(bb)['unique_entry_instance_states']==2
assert all(x['entry_seed_fact_count']==1 and x['state_scope'].startswith('authenticated_entry_seed') for x in records)
print('PASS: c61 same row/instruction under distinct incoming states/roots remains separate; entry seed is not source must-state')

# Different paths to the same original context may dedup while preserving post
# reachability; diagnostic stored first path must not be forged to include c59.
ff=functions(jump(0x1107,0x1200));ff[0x1200]=jump(0x1200,0x1400);ff[0x1400]=b'\xc3'
bb,_=baseline(ff);records=values(bb)
assert len(records)==1 and records[0]['terminal_phase']=='post_c59_reachable_dedup_stored_earlier_path'
assert not records[0]['c59_record_identities']
assert records[0]['post_c59_origin']['arrival_path']['c59_record_identities']
assert records==values(bb)
print('PASS: c61 dedup-stored earlier path stays distinct from authenticated c59 arrival origin with deterministic identity')

# A second source-local boundary can fail after c59 edge preflight but before
# the row fixpoint or any transition into its first later target.
ff=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+jump(0x110e,0x1500))
ff.update({0x1400:b'\xc3',0x1500:b'\xc3'})
bb,_=baseline(ff);records=values(bb)
assert records and all(x['terminal_phase']=='same_instance_after_c59_preflight_before_row_fixpoint' for x in records)
assert all(x['local_c59_record_identities'] and x['post_c59_origin'] is None for x in records)
print('PASS: c61 same-row post-preflight boundary is observed without claiming an unexecuted c59 crossing or instruction fixpoint')

# A pre-c59 mismatch raises original LATER and cannot become a POST capture.
e=b.pool.baseline.evidence
observer=c59.ObserverView(e);old=c59.ObservedTransaction(observer,0,c58.c55.Engine(e.session.domain,e.session.profile.summary),c58.Limits())
refused(old.run,c59.LATER)
view=tool.View(b.pool);tx=tool.Transaction(view,99,c58.c55.Engine(e.session.domain,e.session.profile.summary),c58.Limits())
tx.contexts=deepcopy(old.contexts);tx.forward=deepcopy(old.forward);tx.reverse=deepcopy(old.reverse)
key=(observer.capture.row,observer.capture.state_identity);view.tx,view.key=tx,key
ins=next(cfg.old.decoder().disasm(e.session.read(observer.capture.pc,observer.capture.size),observer.capture.pc,count=1))
edge=c59.classify_edges(e.session,*e.session.rows[key[0]][:2],ins)[0]
refused(lambda:view.edge(key[0],ins,edge['edge_kind'],edge['edge_target_rva'],True,Counter()),c59.LATER)
assert view.capture is None
print('PASS: c61 pre-c59 uncertified_later_runtime_boundary is preserved and never relabeled as POST')

for field,value in [('original_root_runtime_row',99),('current_entry_instance_row',99),('incoming_state_identity','wrong'),
    ('boundary_path_identity','wrong'),('boundary_transition_identities',['wrong']),('source_instruction_rva',0),
    ('accepted_c60_authentication_identity','wrong'),('accepted_c60_result_identity','wrong'),('c59_record_identities',['wrong'])]:
    bad=deepcopy(v[0]);bad[field]=value;a=tool.Aggregate(b)
    refused(lambda:a.add(bad),'record');assert a.poisoned
    refused(a.finish,'poisoned')
print('PASS: c61 wrong root/current row/state/path/source/c59/c60 binding poisons the aggregate')

for field in ('source_runtime_row_sha256','source_code_sha256','source_instruction_sha256','root_replay_identity','classifier_identity','post_c59_origin'):
    bad=deepcopy(v[0]);bad[field]='wrong'
    refused(lambda:tool.Aggregate(b).add(bad),'record')
bad=deepcopy(v[0]);bad['edges'][0]['edge_target_rva']+=1
refused(lambda:tool.Aggregate(b).add(bad),'record')
print('PASS: c61 stale source/body/instruction/tool/origin and mismatched target classification cannot enter counts')

bb,_=baseline(post_functions(roots={0x1080:lea(0x1080,cfg.SLOT+8)+jump(0x1087,0x1100)}))
records=values(bb)
for variant in (records[1:],records[:-1],records[::-1],records+records[:1]):
    a=tool.Aggregate(bb)
    refused(lambda:([a.add(x) for x in variant],a.finish()));assert a.poisoned
print('PASS: c61 duplicate/missing/out-of-order/wrong-domain records poison completeness instead of partial output')

for limit in (0,1,8):
    rr=aggregate(bb,limit=limit)
    assert rr['post_c59_boundary_edges_total']==2
    assert rr['authenticated_record_set_sha256']==aggregate(bb)['authenticated_record_set_sha256']
    assert all(len(x)<=limit for x in rr['examples_by_edge_kind'].values())
    assert all(len(x)<=limit for x in rr['examples_by_target_relation'].values())
refused(lambda:tool.Aggregate(bb,9),'bound')
print('PASS: c61 bounded examples never truncate exact totals or deterministic ordered record-set SHA')

for field in ('schema_version','contract','image_sha256','c56_manifest_id','accepted_c57_boundary_identity','accepted_c59_result_identity','accepted_c60_result_identity','c60_limits'):
    before=b.header[field];b.header[field]='wrong';refused(lambda:aggregate(b),'identity');b.header[field]=before
print('PASS: c61 wrong c56-c61 schema/tool/image/contract/proof/limits identities refuse before publication')

for field in ('schema_version','contract','authentication_header','root_transactions_completed','post_c59_uncertified_boundary_refusals','reference_candidates'):
    bad=deepcopy(b.result);bad[field]='wrong'
    refused(lambda:tool.Baseline(b.pool,accepted=b.result,owner_result=bad),'c60 JSON')
refused(lambda:tool.Baseline(b.pool),'accepted c60')
refused(lambda:tool.analyze(raw),'SHA256')
print('PASS: c61 full owner c60 JSON and fixed accepted private identity/count guards cannot be overridden by CLI')

# Keep an independent positive c60 candidate to prove that c61 preserves bytes,
# rather than merely comparing two empty arrays from refused roots.
ff=post_functions(roots={0x1600:lea(0x1600)+jump(0x1607,0x1700),0x1700:bytes.fromhex('488b08c3')})
bb,_=baseline(ff);before=deepcopy(bb.result);rr=aggregate(bb)
assert before['reference_candidate_count']==1 and bb.result==before==c60.scan(bb.pool)
assert rr['c60_candidate_output_sha256']==c56.digest(before['reference_candidates'])
assert rr['new_cross_row_candidate_count']==0 and rr['historical_c60_candidate_counts']['reference_candidate_count']==1
print('PASS: c61 reproduces positive c60 candidate bytes/hash and all original accounting while emitting no new candidate')

# Diagnostic source cannot be invented for a terminal invalidation-only POST.
view=tool.View(b.pool);tx=tool.Transaction(view,0,c58.c55.Engine(e.session.domain,e.session.profile.summary),c58.Limits())
refused(lambda:tool.record_for(b,tx,view),'without source edge callback')
print('PASS: c61 missing edge-callback source for a different POST shape fails closed instead of choosing an arbitrary instruction')

with tempfile.TemporaryDirectory() as directory:
    root=Path(directory);cache=root/'cache';cache.mkdir();s=b.pool.baseline.evidence.session
    c56.atomic_json(cache/'manifest.json',s.manifest)
    for i,(start,end) in enumerate(s.plan):c56.atomic_json(cache/f'shard-{start:06d}-{end:06d}.json',s.make_shard(i))
    prior=b.pool.baseline.evidence
    data=[('c57',prior.result),('c58',b.pool.baseline.result),('c59',b.pool.result),('c60',b.result)]
    for name,value in data:c56.atomic_json(root/(name+'.json'),value)
    before={x.relative_to(root):x.read_bytes() for x in root.rglob('*.json')}
    old=c57.collect_baseline(s,cache,acceptance=c57.Acceptance(len(s.rows),len(prior.proofs)))
    relations=prior.result['target_metadata_relation_counts']
    accepted=c58.AcceptedEdges(prior.result['boundary_edges_total'],relations['exact_unique_runtime_begin'],
        relations['inside_unique_runtime_span_nonentry'],relations['executable_file_backed_outside_runtime_metadata'],
        prior.result['model_only_boundary_edges_total'],sum(prior.result['exact_adjacency_edge_kind_counts'].values()))
    ee=c58.Evidence(old,accepted=accepted,owner_result=c56.read_json(root/'c57.json'))
    base59=c59.Baseline(ee,accepted=b.pool.baseline.result,owner_result=c56.read_json(root/'c58.json'))
    pp=c60.ProofPool(base59,accepted=b.pool.result,owner_result=c56.read_json(root/'c59.json'))
    base61=tool.Baseline(pp,accepted=b.result,owner_result=c56.read_json(root/'c60.json'))
    assert aggregate(base61)==aggregate(b) and before=={x.relative_to(root):x.read_bytes() for x in root.rglob('*.json')}
    (root/'c60.json').write_text('{}')
    refused(lambda:tool.Baseline(pp,accepted=b.result,owner_result=c56.read_json(root/'c60.json')),'c60 JSON')
print('PASS: c61 c56 checkpoints and c57/c58/c59/c60 JSON remain read-only; stale evidence refuses without repair/fallback')

with tempfile.TemporaryDirectory() as directory:
    root=Path(directory);exe=root/'owned.exe';exe.write_bytes(raw)
    for flag,path in [(None,exe)]+[('--c'+str(n)+'-json',root/('c'+str(n)+'.json')) for n in range(57,61)]:
        if flag:path.write_text('{}')
        args=[str(exe),'--output',str(path)]+([flag,str(path)] if flag else [])
        before=path.read_bytes();proc=subprocess.run([sys.executable,tool.__file__,*args],capture_output=True,text=True)
        assert proc.returncode==2 and 'immutable input' in proc.stderr and before==path.read_bytes()
    cache=root/'cache';cache.mkdir()
    proc=subprocess.run([sys.executable,tool.__file__,str(exe),'--c56-cache',str(cache),'--output',str(cache/'new.json')],capture_output=True,text=True)
    assert proc.returncode==2 and not (cache/'new.json').exists()
print('PASS: c61 CLI cannot overwrite EXE/any owner JSON or write inside historical c56 checkpoints')

rr=aggregate(b)
assert all(rr[k] is False for k in c56.FALSE_GATES) and rr['live_qword_reads']==0
assert rr['publication_source_kind']=='unproven' and rr['c52_production_watch_gate'].endswith('_FAIL')
assert rr['cross_runtime_fact_propagation_unchanged'] and rr['c60_candidate_output_unchanged']
assert rr['new_post_c59_boundaries_admitted']==rr['new_cross_row_candidate_count']==0
print('PASS: c61 all completeness/publication/ownership/lifetime/production/watch/game gates remain false with C52 FAIL binding')

proc=subprocess.run([sys.executable,tool.__file__,'--help'],capture_output=True,text=True)
assert proc.returncode==0 and '--c60-json' in proc.stdout
proc=subprocess.run([sys.executable,tool.__file__,'missing.exe','--output','out.json'],capture_output=True,text=True)
assert proc.returncode==2 and tool.BLOCKED in proc.stderr
print('PASS: c61 CLI documents the read-only full chain and missing private image yields BLOCKED without fabricated counts')
