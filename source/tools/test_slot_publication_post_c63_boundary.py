"""Owned c65 first-terminal c64 diagnostic/context/accounting tests; no private EXE."""
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

import slot_publication_post_c63_boundary as tool
from types import SimpleNamespace
c64,c63,c62,c61,c60,c59,c58,c57,c56,cfg=tool.c64,tool.c63,tool.c62,tool.c61,tool.c60,tool.c59,tool.c58,tool.c57,tool.c56,tool.cfg

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
    return c63.Baseline(p,accepted=original,owner_result=original),raw


def next_functions(body=b'\x90',next_body=b'\xc3',targets=None,roots=None):
    f=post_functions(next_body=body,roots=roots)
    if next_body is not None:f[0x1109+len(body)]=next_body
    if targets:f.update(targets)
    return f


def records63(b):return list(c63.replay_records(b))


def result63(b,records=None,limit=8):
    a=c63.Aggregate(b,limit)
    for record in records63(b) if records is None else records:a.add(record)
    return a.finish()


def certificates64(f,extra=()):
    b,raw=baseline63(f,extra);r=c63.classify(b)
    return c64.ProofPool(b,accepted=r,owner_result=r),raw


def scan64(f,extra=(),limits=c58.Limits()):
    p,_=certificates64(f,extra)
    return c64.scan(p,limits)


def local_functions():
    f=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+bytes.fromhex('7505')+jump(0x1110,0x1500)+jump(0x1115,0x1600))
    f.update({0x1400:b'\xc3',0x1500:b'\xc3',0x1600:bytes.fromhex('488b08c3')})
    return f


def conversion_functions(body=None,targets=None,roots=None):
    f=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+jump(0x110e,0x1500),roots=roots)
    f.update({0x1200:b'\xc3',0x1400:jump(0x1400,0x1600) if body is None else body,0x1500:b'\xc3',0x1600:bytes.fromhex('488b08c3')})
    if targets:f.update(targets)
    return f


def baseline65(f,extra=()):
    p,raw=certificates64(f,extra);original=c64.scan(p)
    return tool.Baseline(p,accepted=original,owner_result=original),raw


def after_functions(body=b'\x90',following=b'\xc3',targets=None,roots=None):
    f=next_functions(next_body=body,roots=roots)
    if following is not None:f[0x110a+len(body)]=following
    if targets:f.update(targets)
    return f


def records65(b):return list(tool.replay_records(b))


def result65(b,records=None,limit=8):
    a=tool.Aggregate(b,limit)
    for record in records65(b) if records is None else records:a.add(record)
    return a.finish()


def local65():
    f=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+bytes.fromhex('7505')+jump(0x1110,0x1500)+bytes.fromhex('7505')+jump(0x1117,0x1600)+jump(0x111c,0x1700))
    f.update({0x1400:b'\xc3',0x1500:b'\xc3',0x1600:b'\xc3',0x1700:b'\xc3'})
    return f


def converted65():
    f=local_functions();f[0x1400]=jump(0x1400,0x1700);f[0x1600]=b'\xc3';f[0x1700]=b'\xc3'
    return f

b,raw=baseline65(after_functions());v=records65(b);r=result65(b,v)
assert len(v)==b.result['post_c63_uncertified_boundary_refusals']==1
assert r['post_c63_boundary_instances_total']==r['post_c63_uncertified_boundary_refused_roots_classified']==1
assert v[0]['source_instruction_rva']==0x110a and v[0]['terminal_phase']==tool.PHASES[1]
assert v[0]['terminal_reason']=='current_context_post_c63_reachable'
assert v[0]['c63_record_identities'] and v[0]['post_c63_origin']['arrival_path']['c63_record_identities']
assert r['edge_kind_counts']['ordinary_fallthrough']==r['target_metadata_relation_counts']['exact_unique_runtime_begin']==1
assert r['accepted_c64_authentication_identity']==b.certificates.identity and r['accepted_c64_result_identity']==c56.digest(b.result)
assert v[0]['current_c64_continuation_identity']==v[0]['root_replay_identity']
print('PASS: c65 exact first terminal c64 POST binds original root/state/path/source/full-prefix and authenticated c63 arrival/c64 result identities')

for kind,body,following,targets in [
    ('direct_unconditional_branch',jump(0x110a,0x1400),None,{0x1400:b'\xc3'}),
    ('direct_conditional_taken',jump(0x110a,0x1400,True)+b'\xc3',None,{0x1400:b'\xc3'}),
    ('conditional_fallthrough',jump(0x110a,0x110a,True),b'\xc3',{}),
    ('call_fallthrough',b'\xe8'+struct.pack('<i',0x1200-0x110f),b'\xc3',{})]:
    bb,_=baseline65(after_functions(body,following,targets));rr=result65(bb)
    assert rr['edge_kind_counts'][kind]==1 and rr['new_post_c63_boundaries_admitted']==0
    if kind=='call_fallthrough':assert rr['examples_by_edge_kind'][kind][0]['edge_target_rva']==0x110f
    assert sum(rr['exact_adjacency_counts'].values())==(1 if kind in ('conditional_fallthrough','call_fallthrough') else 0)
print('PASS: c65 ordinary/direct/conditional taken/fallthrough/CALL return-site vocabulary and adjacency stay diagnostic with no target traversal')

bb,_=baseline65(after_functions(jump(0x110a,0x1400,True),b'\xc3',{0x1400:b'\xc3'}));rr=result65(bb)
assert rr['post_c63_boundary_instances_total']==1 and rr['post_c63_boundary_edges_total']==2
print('PASS: c65 root/entry-instance/source-row/state and edge scopes stay separate for multi-successor terminal instructions')

bb,_=baseline65(after_functions(jump(0x110a,0x1401),None,{0x1400:b'\x90\xc3'}))
assert result65(bb)['target_metadata_relation_counts']['inside_unique_runtime_span_nonentry']==1
bb,_=baseline65(after_functions(jump(0x110a,0x1400),None),[(b'.free',0x1400,b'\xc3',0x60000020,1)])
assert result65(bb)['target_metadata_relation_counts']['executable_file_backed_outside_runtime_metadata']==1
print('PASS: c65 interior and metadata-free RX remain exact relation diagnostics without nearest-entry recovery or admission')

bb,_=baseline65(after_functions(b'\xf2'+jump(0x110b,0x1400),b'\xc3',{0x1400:b'\xc3'}));rr=result65(bb)
assert rr['architectural_post_c63_boundary_edges_total']==rr['model_only_post_c63_boundary_edges_total']==1
assert rr['edge_kind_counts']['other_supported_direct_successor']==1
print('PASS: c65 decorated JMP model successor is distinct from its architectural target and grants no continuation')

s=tool.evidence(b.certificates).session
for target,domain,relation in [
    (0x1405,c58.c55.Domain(image_id='owned',rows=[(0x1400,0x1410,0),(0x1404,0x1420,0)],sections=[],image_size=s.size,excluded=[],read=lambda *_:None),'overlapping_multiple_runtime_spans'),
    (0x1401,c58.c55.Domain(image_id='owned',rows=[],sections=[(0x1400,0x1420,0,1,0x60000020)],image_size=s.size,excluded=[],read=lambda *_:None),'executable_not_fully_file_backed'),
    (0x1400,c58.c55.Domain(image_id='owned',rows=[],sections=[(0x1400,0x1401,0,1,0xc0000040)],image_size=s.size,excluded=[],read=lambda *_:None),'non_executable_or_writable_section'),
    (s.size+1,s.domain,'outside_image'),(cfg.DEFERRED,s.domain,'deferred_producer_excluded')]:
    ins=next(cfg.old.decoder().disasm(jump(0x110a,target),0x110a,count=1))
    assert c59.classify_edges(SimpleNamespace(domain=domain),0x110a,0x110f,ins)[0]['target_metadata_relation']==relation
refused(lambda:baseline65({0x1100:b'\x90'*8,0x1104:b'\xc3'}))
print('PASS: c65 all unchanged target-relation helpers cover overlap/unbacked/nonexec/outside/deferred while upstream image refusals remain intact')

local,_=baseline65(local65());lv=records65(local)
assert lv and all(x['terminal_phase']==tool.PHASES[0] and x['terminal_reason']=='used_c63_source_later_instruction' for x in lv)
assert all(x['local_c63_edge_identities'] and not x['c63_record_identities'] and x['post_c63_origin'] is None for x in lv)
assert all(x['c63_terminal_phase']==c63.PHASES[0] for x in lv)
print('PASS: c65 source-local preflight-before-fixpoint keeps prior c63 phase and local proofs without fabricating a completed crossing')

f=local_functions();f[0x1100]=f[0x1100][:-5]+jump(0x1115,0x1200);f[0x1200]=jump(0x1200,0x1700);f[0x1600]=b'\xc3';f[0x1700]=b'\xc3'
dedup,_=baseline65(f);dv=records65(dedup)
assert dv and dv[0]['terminal_phase']==tool.PHASES[2]
assert not dv[0]['c63_record_identities'] and dv[0]['post_c63_origin']['arrival_path']['c63_record_identities']
assert records65(dedup)==dv
print('PASS: c65 dedup-stored first path stays separate from actual c63 arrival/closure provenance and cannot borrow an authorization')

f=conversion_functions();f[0x1600]=b'\x90';f[0x1601]=b'\xc3'
old_converted,_=baseline65(f);ov=records65(old_converted)
assert ov and ov[0]['c63_terminal_phase']==c63.PHASES[3]
assert ov[0]['c63_authorization']['terminal_reason']=='prior_c61_authorization_converted_context_match_failure'
assert ov[0]['c63_authorization']['post_c61_origin'] is None
assert ov[0]['c63_authorization']['terminal_provenance']['first_failed_conjunct']=='scan_token_matches_prior'
print('PASS: c65 inherited converted c60-POST c63 authorization retains original failed match/scan/path evidence, not borrowed c61 reachability')

converted,_=baseline65(converted65());cv=records65(converted);cp=cv[0]['terminal_provenance']
assert cv and cv[0]['terminal_phase']==tool.PHASES[3] and cv[0]['post_c63_origin'] is None
assert cv[0]['terminal_reason']=='prior_c63_authorization_converted_c62_post'
assert cp['underlying_refusal']==c62.POST and cp['underlying_c63_capture']['pc']==cv[0]['source_instruction_rva']==0x1400
assert not cp['edge_entry']['post_c63'] and not cp['edge_entry']['used_c63_source']
assert cp['underlying_c63_capture']['terminal_provenance']['underlying_refusal']==c60.POST
print('PASS: c65 observes original c64 converted earlier-refusal cause outside both post/source sets without relabeling current reachability')

bb,_=baseline65(after_functions(roots={0x1080:lea(0x1080,cfg.SLOT+8)+jump(0x1087,0x1100)}));vv=records65(bb)
assert len(vv)==2 and len({x['source_instruction_rva'] for x in vv})==1
assert len({x['record_identity'] for x in vv})==len({x['incoming_state_identity'] for x in vv})==2
assert result65(bb)['unique_source_rows']==1 and result65(bb)['unique_entry_instance_states']==2
assert all(x['state_scope'].startswith('authenticated_entry_seed') for x in vv)
print('PASS: c65 same PC under separate original roots/states/paths/full prefixes stays separate; entry seed is not instruction must-state')

for field,value in [('original_root_runtime_row',99),('current_entry_instance_row',99),('incoming_state_identity','wrong'),('entry_instance_identity','wrong'),
    ('boundary_path_identity','wrong'),('boundary_transition_identities',[]),('source_instruction_rva',0),('source_instruction_size',0),('source_instruction_sha256','wrong'),
    ('source_runtime_row_sha256','wrong'),('source_code_sha256','wrong'),('root_replay_identity','wrong'),('current_c64_continuation_identity','wrong'),
    ('terminal_phase',tool.PHASES[0]),('terminal_reason','wrong'),('post_c63_origin',None),('accepted_c64_authentication_identity','wrong'),('accepted_c64_result_identity','wrong')]:
    bad=deepcopy(v[0]);bad[field]=value;a=tool.Aggregate(b);refused(lambda:a.add(bad));refused(a.finish,'poisoned')
print('PASS: c65 wrong root/state/path/source/current-prefix/phase/reason/origin/c64 identity permanently poisons aggregate')

for field,value in [('underlying_refusal',None),('edge_entry',{}),('underlying_c63_capture',None),('c61_mismatch_delta',{'wrong':1})]:
    bad=deepcopy(cv[0]);bad['terminal_provenance'][field]=value;a=tool.Aggregate(converted);refused(lambda:a.add(bad));refused(a.finish,'poisoned')
for field in ['record_identity','terminal_phase','terminal_reason','terminal_provenance','c61_authorization','root_replay_identity']:
    bad=deepcopy(ov[0]);bad['c63_authorization'][field]='wrong';a=tool.Aggregate(old_converted);refused(lambda:a.add(bad));refused(a.finish,'poisoned')
print('PASS: c65 converted earlier-refusal cause and inherited c63 phase/reason/full provenance cannot be borrowed, erased or relabeled')

a=tool.Aggregate(bb);a.add(vv[0]);refused(lambda:a.add(vv[0]));refused(a.finish,'poisoned')
a=tool.Aggregate(bb);refused(lambda:a.add(vv[1]));refused(a.finish,'poisoned')
a=tool.Aggregate(bb);a.add(vv[0]);refused(a.finish,'missing')
print('PASS: c65 missing/duplicate/out-of-order records fail complete-domain validation and cannot publish partial completeness')

rr=result65(bb,vv,0);rr8=result65(bb,vv,8)
assert rr['authenticated_record_set_sha256']==rr8['authenticated_record_set_sha256'] and rr['post_c63_boundary_edges_total']==rr8['post_c63_boundary_edges_total']==2
assert not any(rr['examples_by_edge_kind'].values())
for limit in [-1,9,True]:refused(lambda:tool.Aggregate(bb,limit),'bound')
print('PASS: c65 full ordered record-set hash and exact totals do not depend on bounded per-class examples')

# Positive c64 candidates from both new and old unextended roots remain exact.
f=after_functions();f.update({0x2000:lea(0x2000)+jump(0x2007,0x2100),0x2100:bytes.fromhex('488908c3')})
f.update({0x3000:lea(0x3000)+jump(0x3007,0x3100),0x3100:bytes.fromhex('7505')+jump(0x3102,0x3200)+b'\x90',0x3108:b'\x90',0x3109:b'\x90',0x310a:bytes.fromhex('488b08c3'),0x3200:b'\xc3'})
positive,_=baseline65(f);pr=deepcopy(positive.result);rr=result65(positive)
assert pr['reference_candidate_count']>=2 and any('c64_authentication_identity' in ref for ref in pr['reference_candidates'])
assert any('c64_authentication_identity' not in ref for ref in pr['reference_candidates'])
assert rr['c64_candidate_output_sha256']==c56.digest(pr['reference_candidates']) and positive.result==pr
assert rr['c64_candidate_output_unchanged'] and rr['cross_runtime_fact_propagation_unchanged']
assert rr['new_post_c63_boundaries_admitted']==rr['new_cross_row_candidate_count']==0 and 'reference_candidates' not in rr
print('PASS: c65 preserves entire positive c64 extended and legacy candidate bytes/hash and emits no new candidate, edge or Fact')


def observed_audit(b):
    e=tool.evidence(b.certificates);audit=tool.Audit();engine=shard=None
    for root in e.proofs:
        current=root//e.session.profile.rows_per_shard
        if current!=shard:engine=c58.c55.Engine(e.session.domain,e.session.profile.summary);shard=current
        view=tool.View(b.certificates);tx=tool.Transaction(view,root,engine,c58.Limits());reason=None
        try:tx.run()
        except c58.Refusal as error:reason=str(error)
        audit.add(tx,view,reason,b.certificates)
    return audit
au=observed_audit(positive);au.finish(positive.result)
for field in ['continuation_instances_created','root_transactions_completed','post_c63_uncertified_boundary_refusals',
    'c63_exact_edges_admitted','c63_certified_crossings_with_facts','call_edges_with_preserved_facts_crossing_rows','candidate_records_not_retained','no_carried_fact_edges']:
    bad=deepcopy(positive.result);bad[field]+=1;refused(lambda:au.finish(bad),'accounting')
for field in ['reference_candidates','c63_terminal_phase_admitted','c63_terminal_reason_admitted','root_refusal_reasons','c63_certificate_mismatch_counts']:
    bad=deepcopy(positive.result);bad[field]=[];refused(lambda:au.finish(bad),'accounting')
print('PASS: c65 audit rejects drift in every original c64 continuation/instance/edge/phase/reason/fact/CALL/cap/refusal/candidate accounting domain')

for f in [functions(),post_functions(next_body=b'\xc3'),next_functions(next_body=b'\xc3'),{0x1000:b'\xff\xd0\xc3'}]:
    bb,_=baseline65(f);rr=result65(bb)
    assert rr['post_c63_boundary_instances_total']==bb.result['post_c63_uncertified_boundary_refusals']==0 and not records65(bb)
print('PASS: c65 earlier refusals and successful roots remain unchanged and are never relabeled as terminal post-c63 records')

view=tool.View(b.certificates);tx=tool.Transaction(view,0,c58.c55.Engine(s.domain,s.profile.summary),c58.Limits())
key=(0,'owned');tx.contexts[key]=(tuple([None]*16),());tx.forward[key]=set();tx.reverse[key]=set();view.used_boundary_contexts.add(key)
refused(lambda:tx.mark_post_c63(key),c64.POST)
assert view.post63_capture is None
refused(lambda:tool.record_for(b,tx,view),'without source edge callback')
print('PASS: c65 invalidation-only POST without unique edge source remains BLOCKED and cannot invent an instruction or partial record')


def captured(b):
    e=tool.evidence(b.certificates);engine=shard=None
    for root in e.proofs:
        current=root//e.session.profile.rows_per_shard
        if current!=shard:engine=c58.c55.Engine(e.session.domain,e.session.profile.summary);shard=current
        view=tool.View(b.certificates);tx=tool.Transaction(view,root,engine,c58.Limits())
        try:tx.run()
        except c58.Refusal as error:
            if str(error)==c64.POST:return tx,view
    raise AssertionError('no terminal c64 source')
tx,view=captured(b);cap=deepcopy(view.post63_capture)
for field,value in [('root',99),('state_identity','wrong'),('phase',tool.PHASES[0]),('post_c63_origin',None),('row_sha256','wrong')]:
    view.post63_capture=deepcopy(cap);view.post63_capture[field]=value;refused(lambda:tool.record_for(b,tx,view),'terminal')
view.post63_capture=cap;assert tool.record_for(b,tx,view)==v[0]
tx.work['iterations']+=1;bad=tool.record_for(b,tx,view);assert bad['root_replay_identity']!=v[0]['root_replay_identity']
a=tool.Aggregate(b);refused(lambda:a.add(bad));refused(a.finish,'poisoned')
print('PASS: c65 live source/capture/provenance revalidation and exact current c64 work/prefix identity reject stale observer metadata')

for field,value in [('contract','wrong'),('tool_version','wrong'),('schema_version',9),('image_sha256','wrong'),('post_c63_uncertified_boundary_refusals',998),('accepted_c63_record_set_identity','wrong')]:
    bad=deepcopy(b.result);bad[field]=value;refused(lambda:tool.Baseline(b.certificates,accepted=b.result,owner_result=bad))
refused(lambda:tool.Baseline(b.certificates),'accepted c64')
refused(lambda:tool.analyze(raw),'SHA256')
print('PASS: c65 accepted 998-root domain, full c64 owner JSON/schema/tool/contract/image/proof identity are required without CLI bypass')

for field in ['accepted_c64_result_identity','tool_sources','c64_limits']:
    prior=deepcopy(b.header);b.header[field]='wrong';refused(b.assert_current,'c65');b.header=prior
prior=b.result['root_transactions_completed'];b.result['root_transactions_completed']+=1;refused(b.assert_current,'c65');b.result['root_transactions_completed']=prior
prior=b.certificates;b.certificates=converted.certificates;refused(b.assert_current,'c65');b.certificates=prior;b.assert_current()
print('PASS: c65 mutated/substituted c56-c65 source/proof/header/limits/baseline/result seals fail globally before or after replay')

with tempfile.TemporaryDirectory(prefix='ws-c65-chain-') as directory:
    root=Path(directory);cache=root/'cache';cache.mkdir();cert64=b.certificates;prior63=cert64.baseline;cert62=prior63.certificates;old=cert62.baseline;e=old.pool.baseline.evidence;s=e.session
    c56.atomic_json(cache/'manifest.json',s.manifest)
    for i,(start,end) in enumerate(s.plan):c56.atomic_json(cache/f'shard-{start:06d}-{end:06d}.json',s.make_shard(i))
    data=dict(c57=e.result,c58=old.pool.baseline.result,c59=old.pool.result,c60=old.result,c61=cert62.result,c62=prior63.result,c63=cert64.result,c64=b.result)
    for name,value in data.items():c56.atomic_json(root/(name+'.json'),value)
    before={pth.relative_to(root):pth.read_bytes() for pth in root.rglob('*.json')}
    prior=c57.collect_baseline(s,cache,acceptance=c57.Acceptance(len(s.rows),len(e.proofs)))
    rel=e.result['target_metadata_relation_counts'];accept=c58.AcceptedEdges(e.result['boundary_edges_total'],rel['exact_unique_runtime_begin'],rel['inside_unique_runtime_span_nonentry'],rel['executable_file_backed_outside_runtime_metadata'],e.result['model_only_boundary_edges_total'],sum(e.result['exact_adjacency_edge_kind_counts'].values()))
    ee=c58.Evidence(prior,accepted=accept,owner_result=c56.read_json(root/'c57.json'))
    bb=c59.Baseline(ee,accepted=data['c58'],owner_result=c56.read_json(root/'c58.json'))
    cc=c60.ProofPool(bb,accepted=data['c59'],owner_result=c56.read_json(root/'c59.json'))
    dd=c61.Baseline(cc,accepted=data['c60'],owner_result=c56.read_json(root/'c60.json'))
    pp=c62.ProofPool(dd,accepted=data['c61'],owner_result=c56.read_json(root/'c61.json'))
    prior63=c63.Baseline(pp,accepted=data['c62'],owner_result=c56.read_json(root/'c62.json'))
    prior64=c64.ProofPool(prior63,accepted=data['c63'],owner_result=c56.read_json(root/'c63.json'))
    again=tool.Baseline(prior64,accepted=data['c64'],owner_result=c56.read_json(root/'c64.json'))
    assert result65(again)==result65(b) and before=={pth.relative_to(root):pth.read_bytes() for pth in root.rglob('*.json')}
    (root/'c64.json').write_text('{}');refused(lambda:tool.Baseline(prior64,accepted=data['c64'],owner_result=c56.read_json(root/'c64.json')),'c64 JSON')
    (root/'c64.json').unlink();refused(lambda:c56.read_json(root/'c64.json'))
print('PASS: c65 entire c56 checkpoint plus c57-c64 JSON replay stays read-only; stale/corrupt/missing supplied evidence refuses without repair')

with tempfile.TemporaryDirectory(prefix='ws-c65-cli-') as directory:
    root=Path(directory);exe=root/'owned.exe';exe.write_bytes(raw);out=root/'prior.json';out.write_text('preserve');inputs=[]
    for n in range(57,65):
        file=root/f'c{n}.json';file.write_text('{}');inputs.append((n,file))
    before={pth:pth.read_bytes() for _,pth in inputs};before[exe]=exe.read_bytes()
    for file in [exe]+[pth for _,pth in inputs]:
        args=[sys.executable,tool.__file__,str(exe),'--output',str(file)]
        for n,pth in inputs:args+=['--c'+str(n)+'-json',str(pth)]
        proc=subprocess.run(args,capture_output=True,text=True);assert proc.returncode==2 and 'immutable input' in proc.stderr
    cache=root/'cache';cache.mkdir();part=cache/'existing.json';part.write_text('preserve')
    proc=subprocess.run([sys.executable,tool.__file__,str(exe),'--c56-cache',str(cache),'--output',str(part)],capture_output=True,text=True)
    assert proc.returncode==2 and part.read_text()=='preserve'
    proc=subprocess.run([sys.executable,tool.__file__,str(exe),'--output',str(out)],capture_output=True,text=True)
    assert proc.returncode==2 and out.read_text()=='preserve' and all(pth.read_bytes()==value for pth,value in before.items())
print('PASS: c65 CLI output aliases/wrong image preserve EXE, old JSON/checkpoints and previous output atomically')

rr=result65(b)
assert all(rr[k] is False for k in c56.FALSE_GATES) and rr['live_qword_reads']==0 and rr['publication_source_kind']=='unproven'
assert rr['c52_production_watch_gate']=='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'
assert rr['new_post_c63_boundaries_admitted']==rr['new_cross_row_candidate_count']==0
assert sum(rr['terminal_phase_counts'].values())==sum(rr['terminal_reason_counts'].values())==rr['post_c63_boundary_instances_total']
proc=subprocess.run([sys.executable,tool.__file__,'--help'],capture_output=True,text=True)
assert proc.returncode==0 and '--c64-json' in proc.stdout
proc=subprocess.run([sys.executable,tool.__file__,'missing-private.exe','--output','unused-c65.json'],capture_output=True,text=True)
assert proc.returncode==2 and tool.BLOCKED in proc.stderr and not Path('unused-c65.json').exists()
print('PASS: c65 no edge/Fact/candidate admission or safety gate promotion; absent private image BLOCKED is never native/game PASS')

# Explicit original c64 conversion routes. MRO interposes ONLY an owned lower
# boundary response after a real original prefix; never monkeypatch old modules.
def core(tx,view):
    return c56.encoded(dict(root=tx.root,contexts=[(k,c58.state_id(state),path) for k,(state,path) in sorted(tx.contexts.items())],
        forward=[(k,sorted(v)) for k,v in sorted(tx.forward.items())],reverse=[(k,sorted(v)) for k,v in sorted(tx.reverse.items())],
        work=tx.work,relations=tx.relations,created=tx.created,deduplicated=tx.deduplicated,cycles=tx.cycles,
        post59=sorted(tx.post_c59),post61=sorted(tx.post_c61),post63=sorted(tx.post_c63),tainted=sorted(tx.path_tainted),
        references=tx.references,crossings=tx.crossings,c61_crossings=tx.c61_crossings,c63_crossings=tx.c63_crossings,
        c61_mismatches=view.c61_mismatches,c63_mismatches=view.c63_mismatches,c63_record=view.c63_record,
        c63_proofs=sorted(view.c63_proofs.items()),c63_source_keys=sorted(view.c63_source_keys),c63_siblings=sorted(view.c63_siblings_used),
        considered=sorted(tx.considered),admitted=sorted(tx.admitted),c63_considered=sorted(view.c63_considered),c63_eligible=sorted(view.c63_eligible),
        capture=view.capture,diagnostic_capture=view.diagnostic_capture,post_origins=sorted(tx.post_origins.items()),post61_origins=sorted(tx.post61_origins.items())))


def invoke_lower(b,observed,cause):
    class LowerBoundary(c63.View):
        def edge(self,*args):
            if self.c63_record is not None and args[1].address==0x1400:raise c58.Refusal(cause)
            return super().edge(*args)
    parent=tool.View if observed else c64.View
    class Owned(parent,LowerBoundary):pass
    e=tool.evidence(b.certificates);root=next(iter(e.proofs));view=Owned(b.certificates)
    tx=(tool.Transaction if observed else c64.Transaction)(view,root,c58.c55.Engine(e.session.domain,e.session.profile.summary),c58.Limits())
    try:tx.run()
    except c58.Refusal as error:return tx,view,str(error)
    raise AssertionError('expected original owned lower-boundary refusal')

for cause,reason in [(c60.POST,'prior_c63_authorization_converted_c60_post'),(c59.LATER,'prior_c63_authorization_converted_pre_c59_later')]:
    ot,oview,old_reason=invoke_lower(converted,False,cause);nt,nview,new_reason=invoke_lower(converted,True,cause)
    assert old_reason==new_reason==c64.POST and core(ot,oview)==core(nt,nview)
    record=tool.record_for(converted,nt,nview)
    assert record['terminal_reason']==reason and record['terminal_phase']==tool.PHASES[3] and record['post_c63_origin'] is None
    assert record['terminal_provenance']['underlying_refusal']==cause
    assert record['terminal_provenance']['underlying_c63_capture'] is None
    assert record['terminal_provenance']['underlying_c61_capture'] is None  # earlier cached source is not this PC
    a=tool.Aggregate(converted);refused(lambda:a.add(record));refused(a.finish,'poisoned')
print('PASS: c65 observes original chained c60 POST/pre-c59 LATER conversions in owned protocol seams with identical c64 state, never natural-domain evidence')

ot,oview,old_reason=invoke_lower(converted,False,'owned_unrecognized_refusal')
nt,nview,new_reason=invoke_lower(converted,True,'owned_unrecognized_refusal')
assert old_reason==new_reason=='owned_unrecognized_refusal' and nview.post63_capture is None and core(ot,oview)==core(nt,nview)
print('PASS: c65 unknown earlier refusal remains original and is not normalized or admitted by the observer')

class PrefixPause(Exception):pass

def later_scan(b,observed):
    parent=tool.View if observed else c64.View
    class Paused(parent):
        def edge(self,*args):
            if self.c63_record is not None and args[1].address==0x111c:self.saved_args=args;raise PrefixPause()
            return super().edge(*args)
    e=tool.evidence(b.certificates);root=next(iter(e.proofs));view=Paused(b.certificates)
    tx=(tool.Transaction if observed else c64.Transaction)(view,root,c58.c55.Engine(e.session.domain,e.session.profile.summary),c58.Limits())
    try:tx.run()
    except PrefixPause:pass
    else:raise AssertionError('owned source did not pause before first terminal')
    tx.scan_token+=1;args=list(view.saved_args);pc=view.c63_record['source_instruction_rva'];begin,end,code,_=view.row(view.key[0])
    args[1]=next(cfg.old.decoder().disasm(code[pc-begin:],pc,count=1));edge=c59.classify_edges(view.session,begin,end,args[1])[0]
    args[2:5]=[edge['edge_kind'],edge['edge_target_rva'],edge['architectural_successor']]
    try:(tool.View.edge if observed else c64.View.edge)(view,*args)
    except c58.Refusal as error:return tx,view,str(error)
    raise AssertionError('later source scan unexpectedly admitted')
ot,oview,old_reason=later_scan(local,False);nt,nview,new_reason=later_scan(local,True)
assert old_reason==new_reason==c64.POST and core(ot,oview)==core(nt,nview)
record=tool.record_for(local,nt,nview)
assert record['terminal_phase']==tool.PHASES[4] and record['terminal_reason']=='used_c63_source_later_scan'
assert record['source_instruction_rva']==record['c63_authorization']['source_instruction_rva']
a=tool.Aggregate(local);refused(lambda:a.add(record));refused(a.finish,'poisoned')
print('PASS: c65 later-scan used-source guard is distinct from reachability and source-local phase; protocol seam cannot replace original first terminal')

tx,view=captured(converted);before=deepcopy(view.post63_edge_before);cause=view.post63_underlying_refusal
for field,value in [('root',99),('pc',0),('post_c63',True),('used_c63_source',True),('authorization_record_identity','wrong')]:
    view.post63_edge_before=deepcopy(before);view.post63_edge_before[field]=value;refused(lambda:tool.record_for(converted,tx,view))
view.post63_edge_before=before;view.post63_underlying_refusal='unrecognized'
refused(lambda:tool.record_for(converted,tx,view),'provenance');view.post63_underlying_refusal=cause
assert tool.record_for(converted,tx,view)==cv[0]
print('PASS: c65 exact original edge-entry guard snapshot and chained cause revalidation reject stale or fabricated terminal provenance')

# Complete domain must remain exact even with a self-consistent owned override.
prior=deepcopy(b.header);seal=b.identity
b.header['post_c63_uncertified_boundary_refused_roots_expected']=998;b.identity=c56.digest(b.header)
refused(lambda:records65(b),'root domain')
b.header=prior;b.identity=seal;b.assert_current()
assert result65(b)['post_c63_uncertified_boundary_refused_roots_classified']==1
print('PASS: c65 cannot turn one actual owned terminal into accepted 998-root completeness or bypass validator accounting tail')
