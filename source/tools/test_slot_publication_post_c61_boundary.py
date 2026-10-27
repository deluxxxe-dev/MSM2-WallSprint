"""Owned c63 first terminal c62 observer/identity/output-preservation tests."""
from contextlib import contextmanager
from collections import Counter
from copy import deepcopy
from dataclasses import replace,asdict
from pathlib import Path
import json
import struct
import subprocess
import sys
import tempfile

import slot_publication_post_c61_boundary as tool
from types import SimpleNamespace
c62,c61,c60,c59,c58,c57,c56,cfg=tool.c62,tool.c61,tool.c60,tool.c59,tool.c58,tool.c57,tool.c56,tool.cfg

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
    return c61.Baseline(p,accepted=original,owner_result=original),raw


def post_functions(body=b'\x90',next_body=b'\xc3',targets=None,roots=None):
    f=functions(following=body,roots=roots)
    if next_body is not None:f[0x1108+len(body)]=next_body
    if targets:f.update(targets)
    return f


def values(b):return list(c61.replay_records(b))


def aggregate(b,records=None,limit=8):
    a=c61.Aggregate(b,limit)
    for record in values(b) if records is None else records:a.add(record)
    return a.finish()


def certificates(f,extra=()):
    b,raw=baseline(f,extra)
    r=c61.classify(b)
    return c62.ProofPool(b,accepted=r,owner_result=r),raw


def scan62(f,extra=(),limits=c58.Limits()):
    p,_=certificates(f,extra)
    return c62.scan(p,limits)



def baseline63(f,extra=()):
    p,raw=certificates(f,extra)
    original=c62.scan(p)
    return tool.Baseline(p,accepted=original,owner_result=original),raw


def next_functions(body=b'\x90',next_body=b'\xc3',targets=None,roots=None):
    f=post_functions(next_body=body,roots=roots)
    if next_body is not None:f[0x1109+len(body)]=next_body
    if targets:f.update(targets)
    return f


def records63(b):return list(tool.replay_records(b))


def result63(b,records=None,limit=8):
    a=tool.Aggregate(b,limit)
    for record in records63(b) if records is None else records:a.add(record)
    return a.finish()

b,raw=baseline63(next_functions());v=records63(b);r=result63(b,v)
assert len(v)==b.result['post_c61_uncertified_boundary_refusals']==1
assert r['post_c61_boundary_instances_total']==r['post_c61_uncertified_boundary_refused_roots_classified']==1
assert r['edge_kind_counts']['ordinary_fallthrough']==r['target_metadata_relation_counts']['exact_unique_runtime_begin']==1
assert v[0]['source_instruction_rva']==0x1109 and v[0]['terminal_phase']==tool.PHASES[1]
assert v[0]['c61_terminal_phase']==c62.PHASES[1]
assert v[0]['c61_record_identities'] and v[0]['post_c61_origin']['arrival_path']['c61_record_identities']
assert r['accepted_c62_authentication_identity']==b.certificates.identity
assert r['accepted_c62_result_identity']==c56.digest(b.result)
print('PASS: c63 observes only exact original c62 first terminal POST with root/state/path/source/c61 provenance and c62 identities')

for kind,body,next_body,targets in [
    ('direct_unconditional_branch',jump(0x1109,0x1400),None,{0x1400:b'\xc3'}),
    ('direct_conditional_taken',jump(0x1109,0x1400,True)+b'\xc3',None,{0x1400:b'\xc3'}),
    ('conditional_fallthrough',jump(0x1109,0x1109,True),b'\xc3',{}),
    ('call_fallthrough',b'\xe8'+struct.pack('<i',0x1200-0x110e),b'\xc3',{})]:
    bb,_=baseline63(next_functions(body,next_body,targets));rr=result63(bb)
    assert rr['edge_kind_counts'][kind]==1 and rr['new_post_c61_boundaries_admitted']==0
    if kind=='call_fallthrough':assert rr['examples_by_edge_kind'][kind][0]['edge_target_rva']==0x110e
    assert sum(rr['exact_adjacency_counts'].values())==(1 if kind in ('conditional_fallthrough','call_fallthrough') else 0)
print('PASS: c63 direct nonadjacent branch, conditional taken/fallthrough and CALL return-site use unchanged successor vocabulary only')

bb,_=baseline63(next_functions(jump(0x1109,0x1400,True),b'\xc3',{0x1400:b'\xc3'}));rr=result63(bb)
assert rr['post_c61_boundary_instances_total']==1 and rr['post_c61_boundary_edges_total']==2
assert rr['edge_kind_counts']['direct_conditional_taken']==rr['edge_kind_counts']['conditional_fallthrough']==1
print('PASS: c63 classifies all required successors while root, instance and edge totals remain distinct')

bb,_=baseline63(next_functions(jump(0x1109,0x1401),None,{0x1400:b'\x90\xc3'}))
assert result63(bb)['target_metadata_relation_counts']['inside_unique_runtime_span_nonentry']==1
bb,_=baseline63(next_functions(jump(0x1109,0x1400),None),[(b'.free',0x1400,b'\xc3',0x60000020,1)])
assert result63(bb)['target_metadata_relation_counts']['executable_file_backed_outside_runtime_metadata']==1
print('PASS: c63 interior and metadata-free RX remain diagnostic with no nearest-entry recovery or target traversal')

bb,_=baseline63(next_functions(b'\xf2'+jump(0x110a,0x1400),b'\xc3',{0x1400:b'\xc3'}));rr=result63(bb)
assert rr['architectural_post_c61_boundary_edges_total']==rr['model_only_post_c61_boundary_edges_total']==1
assert rr['edge_kind_counts']['other_supported_direct_successor']==1
print('PASS: c63 decorated JMP synthetic successor remains model-only, never architectural or admitted')

s=b.certificates.baseline.pool.baseline.evidence.session
for target,domain,relation in [
    (0x1405,c58.c55.Domain(image_id='owned',rows=[(0x1400,0x1410,0),(0x1404,0x1420,0)],sections=[],image_size=s.size,excluded=[],read=lambda *_:None),'overlapping_multiple_runtime_spans'),
    (0x1401,c58.c55.Domain(image_id='owned',rows=[],sections=[(0x1400,0x1420,0,1,0x60000020)],image_size=s.size,excluded=[],read=lambda *_:None),'executable_not_fully_file_backed'),
    (0x1400,c58.c55.Domain(image_id='owned',rows=[],sections=[(0x1400,0x1401,0,1,0xc0000040)],image_size=s.size,excluded=[],read=lambda *_:None),'non_executable_or_writable_section'),
    (s.size+1,s.domain,'outside_image'),(cfg.DEFERRED,s.domain,'deferred_producer_excluded')]:
    ins=next(cfg.old.decoder().disasm(jump(0x1109,target),0x1109,count=1))
    edge=c59.classify_edges(SimpleNamespace(domain=domain),0x1109,0x110e,ins)[0]
    assert edge['target_metadata_relation']==relation
refused(lambda:baseline63({0x1100:b'\x90'*8,0x1104:b'\xc3'}))
print('PASS: c63 unchanged overlap/unbacked/nonexec/outside/deferred relations tested separately without bypassing upstream refusals')

# Original c62 can return POST inside its source graph before row fixpoint.
ff=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+bytes.fromhex('7505')+jump(0x1110,0x1500)+jump(0x1115,0x1600))
ff.update({0x1400:b'\xc3',0x1500:b'\xc3',0x1600:b'\xc3'})
bb,_=baseline63(ff);vv=records63(bb)
assert vv and all(x['terminal_phase']==tool.PHASES[0] and x['c61_terminal_phase']==c62.PHASES[0] for x in vv)
assert all(x['local_c61_edge_identities'] and not x['c61_record_identities'] and x['post_c61_origin'] is None for x in vv)
assert all(x['c61_authorization']['local_c59_record_identities'] and not x['c59_record_identities'] for x in vv)
print('PASS: c63 source-local c61 preflight-before-fixpoint retains earlier phase and local proofs without fabricating c59/c61 crossing')

ff=functions(jump(0x1107,0x1200));ff.update({0x1200:jump(0x1200,0x1400),0x1400:b'\x90',0x1401:b'\xc3'})
bb,_=baseline63(ff);vv=records63(bb)
assert vv and vv[0]['c61_terminal_phase']==c62.PHASES[2]
auth=vv[0]['c61_authorization']
assert not auth['c59_record_identities'] and auth['post_c59_origin']['arrival_path']['c59_record_identities']
print('PASS: c63 preserves c61 dedup-stored-earlier-path authorization and its separate authenticated c59 arrival origin')

ff=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+jump(0x110e,0x1200))
ff.update({0x1200:jump(0x1200,0x1600),0x1400:b'\xc3',0x1600:b'\xc3'})
bb,_=baseline63(ff);vv=records63(bb)
assert vv and vv[0]['terminal_phase']==tool.PHASES[2]
assert not vv[0]['c61_record_identities'] and vv[0]['post_c61_origin']['arrival_path']['c61_record_identities']
assert records63(bb)==vv
print('PASS: c63 post-c61 dedup keeps actual arrival origin separate from stored first path; deterministic record never lends a certificate')

bb,_=baseline63(next_functions(roots={0x1080:lea(0x1080,cfg.SLOT+8)+jump(0x1087,0x1100)}));vv=records63(bb)
assert len(vv)==2 and len({x['source_instruction_rva'] for x in vv})==1
assert len({x['incoming_state_identity'] for x in vv})==len({x['record_identity'] for x in vv})==2
assert result63(bb)['unique_source_rows']==1 and result63(bb)['unique_entry_instance_states']==2
assert all(x['state_scope'].startswith('authenticated_entry_seed') for x in vv)
print('PASS: c63 same source under separate root/state/path/prefix remains separate; entry seed is not instruction must-state')

for field,value in [('original_root_runtime_row',99),('current_entry_instance_row',99),('incoming_state_identity','wrong'),
    ('entry_instance_identity','wrong'),('boundary_path_identity','wrong'),('boundary_transition_identities',['wrong']),
    ('source_instruction_rva',0),('source_instruction_size',0),('source_instruction_sha256','wrong'),('source_runtime_row_sha256','wrong'),
    ('source_code_sha256','wrong'),('root_replay_identity','wrong'),('terminal_phase',tool.PHASES[0]),('post_c61_origin',None),
    ('accepted_c62_authentication_identity','wrong'),('accepted_c62_result_identity','wrong'),('accepted_c61_record_set_identity','wrong')]:
    bad=deepcopy(v[0]);bad[field]=value;a=tool.Aggregate(b)
    refused(lambda:a.add(bad));refused(a.finish,'poisoned')
print('PASS: c63 wrong root/row/state/path/source/prefix/phase/origin/chain identity poisons aggregate permanently')

for key in ['terminal_phase','root_replay_identity','record_identity','post_c59_origin','local_c59_record_identities']:
    bad=deepcopy(v[0]);bad['c61_authorization'][key]='wrong';a=tool.Aggregate(b)
    refused(lambda:a.add(bad));refused(a.finish,'poisoned')
print('PASS: c63 c61 authorization phase/full-prefix/origin/local record identities are independently bound, never row-global metadata')

bb,_=baseline63(next_functions(roots={0x1080:lea(0x1080,cfg.SLOT+8)+jump(0x1087,0x1100)}));vv=records63(bb)
a=tool.Aggregate(bb);a.add(vv[0]);refused(lambda:a.add(vv[0]));refused(a.finish,'poisoned')
a=tool.Aggregate(bb);refused(lambda:a.add(vv[1]));refused(a.finish,'poisoned')
a=tool.Aggregate(bb);a.add(vv[0]);refused(a.finish,'missing')
print('PASS: c63 duplicate, reordered and missing records cannot publish partial completeness')

rr=result63(bb,vv,0);rr8=result63(bb,vv,8)
assert rr['authenticated_record_set_sha256']==rr8['authenticated_record_set_sha256']
assert rr['post_c61_boundary_edges_total']==rr8['post_c61_boundary_edges_total']==2
assert not any(rr['examples_by_edge_kind'].values())
for limit in [-1,9,True]:refused(lambda:tool.Aggregate(bb,limit),'bound')
print('PASS: c63 deterministic full ordered record-set hash and totals do not depend on bounded examples')

# Compare real nonempty owned candidate output, not just equal empty arrays.
ff=next_functions();ff.update({0x2000:lea(0x2000)+jump(0x2007,0x2100),0x2100:bytes.fromhex('488908c3')})
bb,_=baseline63(ff);prior=deepcopy(bb.result);rr=result63(bb)
assert prior['reference_candidate_count']>0 and rr['c62_candidate_output_sha256']==c56.digest(prior['reference_candidates'])
assert bb.result==prior and rr['historical_c62_candidate_counts']['writer_candidates']>0
assert rr['c62_candidate_output_unchanged'] and rr['cross_runtime_fact_propagation_unchanged']
assert rr['new_post_c61_boundaries_admitted']==rr['new_cross_row_candidate_count']==0
assert 'reference_candidates' not in rr
print('PASS: c63 preserves entire positive owned c62 candidate bytes/hash and every accounting domain; emits no new candidates')

# Exact audit compares every dynamic field, including separate phase/edge scopes.
def observed_audit(b):
    e=b.certificates.baseline.pool.baseline.evidence;a=tool.Audit();engine=shard=None
    for root in e.proofs:
        current=root//e.session.profile.rows_per_shard
        if current!=shard:engine=c58.c55.Engine(e.session.domain,e.session.profile.summary);shard=current
        view=tool.View(b.certificates);tx=tool.Transaction(view,root,engine,c58.Limits());reason=None
        try:tx.run()
        except c58.Refusal as error:reason=str(error)
        a.add(tx,view,reason,b.certificates)
    return a
au=observed_audit(bb);au.finish(bb.result)
for field in ['continuation_instances_created','root_transactions_completed','post_c61_uncertified_boundary_refusals',
    'c61_exact_edges_admitted','c61_certified_crossings_with_facts','call_edges_with_preserved_facts_crossing_rows',
    'candidate_records_not_retained','no_carried_fact_edges']:
    bad=deepcopy(bb.result);bad[field]+=1;refused(lambda:au.finish(bad),'accounting')
for field in ['reference_candidates','c61_terminal_phase_admitted','root_refusal_reasons','c61_certificate_mismatch_counts']:
    bad=deepcopy(bb.result);bad[field]=[];refused(lambda:au.finish(bad),'accounting')
print('PASS: c63 audit rejects drift in original c62 root/instance/edge/phase/fact/CALL/refusal/cap/candidate counters')

# Earlier c60/c62 reasons must not become new POST diagnostics.
for ff in [functions(),post_functions(next_body=b'\xc3'),{0x1000:b'\xff\xd0\xc3'}]:
    bb,_=baseline63(ff);rr=result63(bb)
    assert rr['post_c61_boundary_instances_total']==bb.result['post_c61_uncertified_boundary_refusals']==0
    assert not records63(bb)
print('PASS: c63 preserves pre-existing earlier refusals and successful roots without inventing terminal c62 records')

# No unique source exists when a late graph invalidation aborts outside edge().
view=tool.View(b.certificates);tx=tool.Transaction(view,0,c58.c55.Engine(s.domain,s.profile.summary),c58.Limits())
key=(0,'owned');tx.contexts[key]=(tuple([None]*16),());tx.forward[key]=set();tx.reverse[key]=set()
view.used_boundary_contexts.add(key)
refused(lambda:tx.mark_post_c61(key),c62.POST)
assert view.diagnostic_capture is None
refused(lambda:tool.record_for(b,tx,view),'without source edge callback')
print('PASS: c63 invalidation-only terminal POST has no fabricated PC; absent source blocks classification instead of incomplete success')

# Source capture itself is revalidated against exact live observer context.
def captured(b):
    e=b.certificates.baseline.pool.baseline.evidence;engine=None;shard=None
    for root in e.proofs:
        current=root//e.session.profile.rows_per_shard
        if current!=shard:engine=c58.c55.Engine(e.session.domain,e.session.profile.summary);shard=current
        view=tool.View(b.certificates);tx=tool.Transaction(view,root,engine,c58.Limits())
        try:tx.run()
        except c58.Refusal as error:
            if str(error)==c62.POST:return tx,view
    raise AssertionError('fixture has no terminal c62 source')
tx,view=captured(b);capture=deepcopy(view.diagnostic_capture)
for field,value in [('root',99),('state_identity','wrong'),('phase',tool.PHASES[0]),('post_c61_origin',None),('row_sha256','wrong')]:
    view.diagnostic_capture=deepcopy(capture);view.diagnostic_capture[field]=value
    refused(lambda:tool.record_for(b,tx,view),'terminal')
view.diagnostic_capture=capture
assert tool.record_for(b,tx,view)==v[0]
print('PASS: c63 live source/state/phase/origin capture revalidation rejects forged observer metadata')

for field,value in [('contract','wrong'),('tool_version','wrong'),('schema_version',9),('image_sha256','wrong'),
    ('post_c61_uncertified_boundary_refusals',99),('accepted_c61_record_set_identity','wrong')]:
    bad=deepcopy(b.result);bad[field]=value
    refused(lambda:tool.Baseline(b.certificates,accepted=b.result,owner_result=bad))
refused(lambda:tool.Baseline(b.certificates),'accepted c62')
refused(lambda:tool.analyze(raw),'SHA256')
print('PASS: c63 accepted image/schema/tool/contract/count/full c62 JSON identity required; no owner CLI bypass')

for field in ['accepted_c62_result_identity','tool_sources','c62_limits']:
    prior=deepcopy(b.header);b.header[field]='wrong';refused(b.assert_current,'c63');b.header=prior
prior=b.result['root_transactions_completed'];b.result['root_transactions_completed']+=1
refused(b.assert_current,'c63');b.result['root_transactions_completed']=prior
prior=b.certificates.header['index_sha256'];b.certificates.header['index_sha256']='wrong'
refused(b.assert_current);b.certificates.header['index_sha256']=prior;b.assert_current()
prior=b.certificates
alternate,_=certificates(next_functions(body=b'\x90\x90'))
b.certificates=alternate;refused(b.assert_current,'c63');b.certificates=prior;b.assert_current()
print('PASS: c63 mutated or substituted c56-c63 source/proof/header/limits/result seals fail globally before or after replay')

# Reconstruct the complete owned checkpoint/JSON chain through c62, read-only.
with tempfile.TemporaryDirectory(prefix='ws-c63-chain-') as directory:
    root=Path(directory);cache=root/'cache';cache.mkdir();cert=b.certificates;old=cert.baseline
    e=old.pool.baseline.evidence;session=e.session
    c56.atomic_json(cache/'manifest.json',session.manifest)
    for i,(start,end) in enumerate(session.plan):c56.atomic_json(cache/f'shard-{start:06d}-{end:06d}.json',session.make_shard(i))
    data=dict(c57=e.result,c58=old.pool.baseline.result,c59=old.pool.result,c60=old.result,c61=cert.result,c62=b.result)
    for name,value in data.items():c56.atomic_json(root/(name+'.json'),value)
    before={p.relative_to(root):p.read_bytes() for p in root.rglob('*.json')}
    prior=c57.collect_baseline(session,cache,acceptance=c57.Acceptance(len(session.rows),len(e.proofs)))
    rel=e.result['target_metadata_relation_counts'];accept=c58.AcceptedEdges(e.result['boundary_edges_total'],rel['exact_unique_runtime_begin'],
        rel['inside_unique_runtime_span_nonentry'],rel['executable_file_backed_outside_runtime_metadata'],e.result['model_only_boundary_edges_total'],sum(e.result['exact_adjacency_edge_kind_counts'].values()))
    ee=c58.Evidence(prior,accepted=accept,owner_result=c56.read_json(root/'c57.json'))
    bb=c59.Baseline(ee,accepted=data['c58'],owner_result=c56.read_json(root/'c58.json'))
    cc=c60.ProofPool(bb,accepted=data['c59'],owner_result=c56.read_json(root/'c59.json'))
    dd=c61.Baseline(cc,accepted=data['c60'],owner_result=c56.read_json(root/'c60.json'))
    cert2=c62.ProofPool(dd,accepted=data['c61'],owner_result=c56.read_json(root/'c61.json'))
    again=tool.Baseline(cert2,accepted=data['c62'],owner_result=c56.read_json(root/'c62.json'))
    assert result63(again)==result63(b) and before=={p.relative_to(root):p.read_bytes() for p in root.rglob('*.json')}
    (root/'c62.json').write_text('{}');refused(lambda:tool.Baseline(cert2,accepted=data['c62'],owner_result=c56.read_json(root/'c62.json')),'c62 JSON')
    (root/'c62.json').unlink();refused(lambda:c56.read_json(root/'c62.json'))
print('PASS: c63 complete c56 checkpoint plus c57-c62 JSON replay is read-only; corrupt or missing supplied evidence refuses without repair')

tx,view=captured(b);tx.work['iterations']+=1
bad=tool.record_for(b,tx,view);assert bad['root_replay_identity']!=v[0]['root_replay_identity']
a=tool.Aggregate(b);refused(lambda:a.add(bad));refused(a.finish,'poisoned')
print('PASS: c63 exact original graph/work prefix is bound independently of a matching source/state/phase capture')

with tempfile.TemporaryDirectory(prefix='ws-c63-owned-') as tmp:
    folder=Path(tmp);exe=folder/'owned.exe';exe.write_bytes(raw);out=folder/'old.json';out.write_text('preserve')
    inputs=[]
    for n in range(57,63):
        file=folder/f'c{n}.json';file.write_text('{"owned":true}');inputs.append((n,file))
    before={p:p.read_bytes() for _,p in inputs};before[exe]=exe.read_bytes()
    script=Path(tool.__file__)
    for file in [exe]+[p for _,p in inputs]:
        args=[sys.executable,str(script),str(exe),'--output',str(file)]
        for n,pth in inputs:args+=['--c'+str(n)+'-json',str(pth)]
        result=subprocess.run(args,capture_output=True,text=True)
        assert result.returncode==2 and tool.BLOCKED in result.stderr and 'overlaps' in result.stderr
    cache=folder/'c56.shards';cache.mkdir();part=cache/'existing.json';part.write_text('preserve')
    result=subprocess.run([sys.executable,str(script),str(exe),'--c56-cache',str(cache),'--output',str(part)],capture_output=True,text=True)
    assert result.returncode==2 and part.read_text()=='preserve'
    result=subprocess.run([sys.executable,str(script),str(exe),'--output',str(out)],capture_output=True,text=True)
    assert result.returncode==2 and tool.BLOCKED in result.stderr and out.read_text()=='preserve'
    assert all(p.read_bytes()==data for p,data in before.items())
print('PASS: c63 CLI refuses input aliases and wrong image without overwriting prior output, EXE, checkpoints or c57-c62 JSONs')

r=result63(b)
assert r['classification_complete'] and r['c56_through_c62_evidence_read_only']
assert all(r[k] is False for k in c56.FALSE_GATES)
assert r['live_qword_reads']==0 and r['publication_source_kind']=='unproven'
assert r['c52_production_watch_gate']=='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'
assert r['new_post_c61_boundaries_admitted']==r['new_cross_row_candidate_count']==0
assert sum(r['terminal_phase_counts'].values())==sum(r['c61_authorization_phase_counts'].values())==r['post_c61_boundary_instances_total']
print('PASS: c63 complete diagnostic counts remain distinct from all false completeness/publication/ownership/lifetime/CFG/production/watch/game gates')

help_result=subprocess.run([sys.executable,str(Path(tool.__file__)),'--help'],capture_output=True,text=True)
assert help_result.returncode==0 and all('--c'+str(n)+'-json' in help_result.stdout for n in range(57,63))
missing=subprocess.run([sys.executable,str(Path(tool.__file__)),'missing-private.exe','--output','unused-c63.json'],capture_output=True,text=True)
assert missing.returncode==2 and tool.BLOCKED in missing.stderr and not Path('unused-c63.json').exists()
print('PASS: c63 CLI exposes read-only complete evidence inputs and absent private image is BLOCKED, never native/game PASS')

# Regression for the legitimate c62 conversion omitted by the original observer.
def conversion_functions(roots=None):
    f=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+jump(0x110e,0x1500),roots=roots)
    f.update({0x1200:b'\xc3',0x1400:jump(0x1400,0x1600),0x1500:b'\xc3',0x1600:b'\xc3'})
    return f

conversion,_=baseline63(conversion_functions());cv=records63(conversion);cr=result63(conversion)
assert len(cv)==conversion.result['post_c61_uncertified_boundary_refusals']==1
record=cv[0];prov=record['terminal_provenance']
assert record['terminal_reason']=='prior_c61_authorization_converted_context_match_failure'
assert record['terminal_phase']==tool.PHASES[3] and record['post_c61_origin'] is None
assert not prov['edge_entry']['post_c61'] and not prov['edge_entry']['used_c61_source']
assert prov['underlying_refusal']==c60.POST and prov['underlying_post_capture']['pc']==record['source_instruction_rva']==0x1400
assert prov['mismatch_delta']=={'context_phase_origin_or_full_prefix_mismatch':1}
assert prov['match_conjuncts']==dict(record_identity_matches_prior=True,scan_token_matches_prior=False,semantic_record_matches_prior=False)
assert prov['first_failed_conjunct']=='scan_token_matches_prior'
assert cr['terminal_reason_counts']=={record['terminal_reason']:1}
assert cr['terminal_conversion_first_failed_conjunct_counts']=={'scan_token_matches_prior':1}
assert cr['c62_candidate_output_unchanged'] and cr['new_post_c61_boundaries_admitted']==0
print('PASS: repaired c63 naturally replays converted c60 POST outside both c62 post/source sets with exact reason and no false post-c61 reachability')

conversion2,_=baseline63(conversion_functions(roots={0x1080:lea(0x1080,cfg.SLOT+8)+jump(0x1087,0x1100)}));cvs=records63(conversion2)
assert len(cvs)==2 and len({r['source_instruction_rva'] for r in cvs})==1
assert len({r['original_root_runtime_row'] for r in cvs})==len({r['incoming_state_identity'] for r in cvs})==len({r['record_identity'] for r in cvs})==2
assert len({r['root_replay_identity'] for r in cvs})==2 and records63(conversion2)==cvs
assert all(r['terminal_phase']==tool.PHASES[3] and r['post_c61_origin'] is None for r in cvs)
print('PASS: repaired c63 converted terminal stays root/state/path/source/full-prefix specific across two real owned replay roots')

# Boundary-only seams exercise an otherwise invariant-impossible missing lookup
# and a later-scan guard. No historical class/module/source is mutated. The full
# natural replay above, not these seams, proves accepted transaction preservation.
class PrefixPause(Exception):pass

class LookupBoundary:
    def __init__(self,original):self.original=original;self.mode=None
    def __getattr__(self,name):return getattr(self.original,name)
    def lookup(self,root):return None if self.mode=='unrecorded' else self.original.lookup(root)


def paused_edge(b,observed,pc):
    parent=tool.View if observed else c62.View
    class Paused(parent):
        def edge(self,*args):
            if self.c61_record is not None and args[1].address==pc:
                self.saved_args=args;raise PrefixPause()
            return super().edge(*args)
    proxy=LookupBoundary(b.certificates);view=Paused(proxy)
    e=b.certificates.baseline.pool.baseline.evidence;root=next(iter(e.proofs))
    tx=(tool.Transaction if observed else c62.Transaction)(view,root,c58.c55.Engine(e.session.domain,e.session.profile.summary),c58.Limits())
    try:tx.run()
    except PrefixPause:pass
    else:raise AssertionError('owned prefix did not pause before terminal callback')
    return tx,view,proxy


def core_state(tx,view):
    return c56.encoded(dict(root=tx.root,contexts=[(k,c58.state_id(state),path) for k,(state,path) in sorted(tx.contexts.items())],
        forward=[(k,sorted(v)) for k,v in sorted(tx.forward.items())],reverse=[(k,sorted(v)) for k,v in sorted(tx.reverse.items())],
        work=tx.work,relations=tx.relations,created=tx.created,deduplicated=tx.deduplicated,cycles=tx.cycles,
        post_c59=sorted(tx.post_c59),post_c61=sorted(tx.post_c61),path_tainted=sorted(tx.path_tainted),
        references=tx.references,crossings=tx.crossings,later_crossings=tx.later_crossings,c61_crossings=tx.c61_crossings,
        capture=view.capture,c61_record=view.c61_record,c61_source_keys=sorted(view.c61_source_keys),
        considered=sorted(view.c61_considered),eligible=sorted(view.c61_eligible),edges_considered=sorted(view.c61_edges_considered),
        edges_eligible=sorted(view.c61_edges_eligible),refusals=view.c61_refusals,mismatches=view.c61_mismatches,
        lookup_misses=view.lookup_misses,original_considered=sorted(tx.considered),original_admitted=sorted(tx.admitted),
        used_boundary_contexts=sorted(view.used_boundary_contexts),certified_contexts=sorted(view.certified_contexts)))


def invoke_boundary(b,observed,pc,mode=None,later_scan=False,prior_authorized=True):
    tx,view,proxy=paused_edge(b,observed,pc);args=list(view.saved_args);proxy.mode=mode
    if later_scan:
        tx.scan_token+=1
        auth_pc=view.c61_record['source_instruction_rva'];begin,end,code,_=view.row(view.key[0])
        args[1]=next(cfg.old.decoder().disasm(code[auth_pc-begin:],auth_pc,count=1))
        edge=c59.classify_edges(view.session,begin,end,args[1])[0]
        args[2:5]=[edge['edge_kind'],edge['edge_target_rva'],edge['architectural_successor']]
    if not prior_authorized:view.c61_record=None
    fn=tool.View.edge if observed else c62.View.edge
    try:fn(view,*args)
    except c58.Refusal as error:reason=str(error)
    else:raise AssertionError('original terminal refusal unexpectedly admitted')
    return tx,view,reason

orig_tx,orig_view,reason=invoke_boundary(conversion,False,0x1400,mode='unrecorded')
obs_tx,obs_view,obs_reason=invoke_boundary(conversion,True,0x1400,mode='unrecorded')
assert reason==obs_reason==c62.POST and core_state(orig_tx,orig_view)==core_state(obs_tx,obs_view)
missing_record=tool.record_for(conversion,obs_tx,obs_view);mp=missing_record['terminal_provenance']
assert missing_record['terminal_reason']=='prior_c61_authorization_converted_unrecorded_root'
assert missing_record['terminal_phase']==tool.PHASES[3] and missing_record['post_c61_origin'] is None
assert not mp['edge_entry']['post_c61'] and not mp['edge_entry']['used_c61_source']
assert mp['first_failed_conjunct']=='lookup_record_present' and mp['lookup_record_identity'] is None
assert mp['mismatch_delta']=={'unrecorded_root':1} and mp['underlying_post_capture']['pc']==0x1400
assert obs_view.certificates.records==conversion.certificates.records and obs_view.certificates.index==conversion.certificates.index
# A protocol-boundary seam is not silently added to the natural accepted stream.
a=tool.Aggregate(conversion);refused(lambda:a.add(missing_record));refused(a.finish,'poisoned')
print('PASS: repaired c63 missing-lookup conversion boundary seam preserves exact original c62 exception/counters/state and sealed prior proof; never admitted as natural replay')

orig_tx,orig_view,reason=invoke_boundary(conversion,False,0x1400)
obs_tx,obs_view,obs_reason=invoke_boundary(conversion,True,0x1400)
assert reason==obs_reason==c62.POST and core_state(orig_tx,orig_view)==core_state(obs_tx,obs_view)
assert tool.record_for(conversion,obs_tx,obs_view)==cv[0]
print('PASS: repaired c63 natural match-failure conversion calls original edge once and preserves all c62 state/counters/candidate bytes')

local=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+bytes.fromhex('7505')+jump(0x1110,0x1500)+jump(0x1115,0x1600))
local.update({0x1400:b'\xc3',0x1500:b'\xc3',0x1600:b'\xc3'});local_base,_=baseline63(local)
lr=records63(local_base)[0]
assert lr['terminal_reason']=='used_c61_source_later_instruction' and lr['terminal_phase']==tool.PHASES[0]
assert lr['terminal_provenance']['underlying_refusal'] is None and not lr['terminal_provenance']['mismatch_delta']
orig_tx,orig_view,reason=invoke_boundary(local_base,False,0x1115,later_scan=True)
obs_tx,obs_view,obs_reason=invoke_boundary(local_base,True,0x1115,later_scan=True)
assert reason==obs_reason==c62.POST and core_state(orig_tx,orig_view)==core_state(obs_tx,obs_view)
sr=tool.record_for(local_base,obs_tx,obs_view)
assert sr['terminal_reason']=='used_c61_source_later_scan' and sr['terminal_phase']==tool.PHASES[4]
assert sr['source_instruction_rva']==sr['c61_authorization']['source_instruction_rva']
assert sr['terminal_provenance']['underlying_refusal'] is None and not sr['terminal_provenance']['mismatch_delta']
print('PASS: repaired c63 distinguishes later instruction and later scan on prior c61 source without relabeling either as post reachability or c60 conversion')

assert v[0]['terminal_reason']=='current_context_post_c61_reachable'
assert v[0]['terminal_provenance']['edge_entry']['post_c61'] and v[0]['terminal_provenance']['underlying_refusal'] is None
orig_tx,orig_view,reason=invoke_boundary(conversion,False,0x1400,prior_authorized=False)
obs_tx,obs_view,obs_reason=invoke_boundary(conversion,True,0x1400,prior_authorized=False)
assert reason==obs_reason==c60.POST and obs_view.diagnostic_capture is None
assert core_state(orig_tx,orig_view)==core_state(obs_tx,obs_view)
print('PASS: repaired c63 preserves guard precedence, existing reachable terminals and pre-c61 refusal when no prior authorization exists')

for field,value in [('terminal_reason','current_context_post_c61_reachable'),('terminal_phase',tool.PHASES[1]),('post_c61_origin',{'invented':True})]:
    bad=deepcopy(cv[0]);bad[field]=value;a=tool.Aggregate(conversion);refused(lambda:a.add(bad));refused(a.finish,'poisoned')
for field,value in [('mismatch_delta',{}),('underlying_refusal',c62.POST),('first_failed_conjunct','semantic_record_matches_prior'),
    ('lookup_record_identity','wrong'),('current_c61_record_identity','wrong'),('match_conjuncts',{'invented':True})]:
    bad=deepcopy(cv[0]);bad['terminal_provenance'][field]=value;a=tool.Aggregate(conversion)
    refused(lambda:a.add(bad));refused(a.finish,'poisoned')
print('PASS: repaired c63 wrong terminal reason/conversion subtype/predicate/source provenance permanently poisons full ordered aggregate')

obs_tx,obs_view,_=invoke_boundary(conversion,True,0x1400)
original=deepcopy(obs_view.terminal_edge_before)
for field,value in [('root',99),('pc',0),('post_c61',True),('used_c61_source',True),('authorization_record_identity','wrong'),('mismatch_counts',{})]:
    obs_view.terminal_edge_before=deepcopy(original);obs_view.terminal_edge_before[field]=value
    if field=='mismatch_counts':obs_view.terminal_edge_before[field]={'context_phase_origin_or_full_prefix_mismatch':-1}
    refused(lambda:tool.record_for(conversion,obs_tx,obs_view))
obs_view.terminal_edge_before=original;assert tool.record_for(conversion,obs_tx,obs_view)==cv[0]
assert sum(cr['terminal_reason_counts'].values())==cr['post_c61_boundary_instances_total']
assert sum(cr['terminal_phase_counts'].values())==cr['post_c61_boundary_instances_total']
assert cr['c52_production_watch_gate']=='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'
assert all(cr[k] is False for k in c56.FALSE_GATES)
print('PASS: repaired c63 edge-entry snapshot and mismatch delta revalidation bind exact source/root/prior proof; diagnostic repair opens no gate')
