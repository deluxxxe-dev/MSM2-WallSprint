"""Owned c64 exact c63 context/provenance continuation tests; no private EXE."""
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

import slot_publication_post_c61_stitched as tool
from types import SimpleNamespace
c63,c62,c61,c60,c59,c58,c57,c56,cfg=tool.c63,tool.c62,tool.c61,tool.c60,tool.c59,tool.c58,tool.c57,tool.c56,tool.cfg

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
    return tool.ProofPool(b,accepted=r,owner_result=r),raw


def scan64(f,extra=(),limits=c58.Limits()):
    p,_=certificates64(f,extra)
    return tool.scan(p,limits)


def local_functions():
    f=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+bytes.fromhex('7505')+jump(0x1110,0x1500)+jump(0x1115,0x1600))
    f.update({0x1400:b'\xc3',0x1500:b'\xc3',0x1600:bytes.fromhex('488b08c3')})
    return f


def conversion_functions(body=None,targets=None,roots=None):
    f=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+jump(0x110e,0x1500),roots=roots)
    f.update({0x1200:b'\xc3',0x1400:jump(0x1400,0x1600) if body is None else body,0x1500:b'\xc3',0x1600:bytes.fromhex('488b08c3')})
    if targets:f.update(targets)
    return f

p,raw=certificates64(next_functions(next_body=bytes.fromhex('488b08c3')));r=tool.scan(p)
assert r['c63_boundary_instances_admitted']==r['c63_exact_edges_admitted']==1
assert r['c63_certified_crossings_with_facts']==r['c63_address_facts_crossing_rows']==1
assert len(refs(r,'READ'))==1
ref=refs(r)[0];assert [x.get('proof_kind','c57') for x in ref['boundary_path']]==['c57','c59_later_boundary','c61_post_boundary','c63_post_boundary']
assert ref['address_proof_chain']==((0x1000,'rip_lea',cfg.SLOT),)
assert ref['path_c63_record_identities'] and ref['root_c63_record_identities']
assert r['historical_c62_candidate_output_sha256']==p.result['c62_candidate_output_sha256']
print('PASS: c64 exact ordinary boundary carries only original incoming Fact with separate complete c57/c59/c61/c63 proof metadata')

for kind,body,next_body,targets in [
    ('direct_unconditional_branch',jump(0x1109,0x1400),None,{0x1400:bytes.fromhex('488908c3')}),
    ('direct_conditional_taken',jump(0x1109,0x1400,True)+b'\xc3',None,{0x1400:bytes.fromhex('488908c3')}),
    ('conditional_fallthrough',jump(0x1109,0x1109,True),bytes.fromhex('488908c3'),{}),
    ('call_fallthrough',b'\xe8'+struct.pack('<i',0x1200-0x110e),bytes.fromhex('488908c3'),{})]:
    rr=scan64(next_functions(body,next_body,targets));assert rr['c63_exact_edges_admitted']==1 and refs(rr,'WRITE')
    assert refs(rr)[0]['boundary_path'][-1]['edge_kind']==kind
print('PASS: c64 exact conditional taken/fallthrough, nonadjacent direct branch and CALL return-site use existing architectural semantics')

for target,extra,targets in [(0x1401,[],{0x1400:b'\x90\xc3'}),(0x1400,[(b'.free',0x1400,b'\xc3',0x60000020,1)],{})]:
    rr=scan64(next_functions(jump(0x1109,target,True),b'\xc3',targets),extra)
    assert not refs(rr) and rr['c63_exact_edges_considered']==1 and rr['c63_exact_edges_admitted']==0
    assert rr['c63_boundary_instances_admitted']==0 and sum(rr['c63_all_successor_preflight_refusal_relation_counts'].values())==1
print('PASS: c64 one exact plus interior or metadata-free RX arm aborts complete boundary/root before any proof or candidate commits')

rr=scan64(next_functions(jump(0x1109,0x1400,True),bytes.fromhex('488908c3'),{0x1400:bytes.fromhex('488b08c3')}))
assert rr['c63_exact_edges_admitted']==2 and rr['c63_boundary_instances_admitted']==1 and len(refs(rr))==2
print('PASS: c64 all-exact multi-successor boundary preflights both then uses exact same-instruction sealed sibling cache')

for reg,expect in [(3,True),(0,False),(4,False)]:
    # Replace root LEA carrier and use matching target full-parent operand.
    f=next_functions(b'\xe8'+struct.pack('<i',0x1300-0x110e),bytes.fromhex({3:'488b0bc3',0:'488b08c3',4:'488b0c24c3'}[reg]),{0x1300:bytes.fromhex('4831c0c3')})
    f[0x1000]=lea(0x1000,reg=reg)+jump(0x1007,0x1100)
    rr=scan64(f);assert bool(refs(rr))==expect
    if expect:assert rr['call_edges_with_preserved_facts_crossing_rows']>=1
print('PASS: c64 CALL crossing preserves only c54/c55-authorized RBX, kills callee-written RAX and RSP; no ABI or return-value origin')

local,_=certificates64(local_functions());lr=tool.scan(local)
assert lr['c63_boundary_instances_admitted'] and refs(lr)
for ref in refs(lr):
    proof=ref['boundary_path'][-1]
    assert proof['terminal_phase']==c63.PHASES[0] and proof['terminal_reason']=='used_c61_source_later_instruction'
    assert proof['local_c61_edge_identities'] and proof['post_c61_origin'] is None
    assert not any(x.get('proof_kind')=='c61_post_boundary' for x in ref['boundary_path'])
print('PASS: c64 source-local c63 phase retains local prior preflight without inventing completed c61 crossing or post reachability')

f=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+jump(0x110e,0x1200))
f.update({0x1200:jump(0x1200,0x1600),0x1400:b'\xc3',0x1600:bytes.fromhex('488b08c3')})
dedup,_=certificates64(f);dr=tool.scan(dedup)
assert refs(dr) and dr['c63_boundary_instances_admitted']==1
record=json.loads(dedup.records[0]);assert record['terminal_phase']==c63.PHASES[2]
assert not record['c61_record_identities'] and record['post_c61_origin']['arrival_path']['c61_record_identities']
proof=refs(dr)[0]['boundary_path'][-1]
assert proof['terminal_phase']==c63.PHASES[2] and proof['post_c61_origin']==record['post_c61_origin']
print('PASS: c64 dedup-stored earlier path and authenticated post-c61 arrival stay separate while only exact recorded combination continues')

converted,_=certificates64(conversion_functions());cr=tool.scan(converted);ref=refs(cr)[0];proof=ref['boundary_path'][-1]
assert cr['c63_boundary_instances_admitted']==1 and proof['terminal_phase']==c63.PHASES[3]
assert proof['terminal_reason']=='prior_c61_authorization_converted_context_match_failure' and proof['post_c61_origin'] is None
assert proof['terminal_provenance']['first_failed_conjunct']=='scan_token_matches_prior'
assert not proof['terminal_provenance']['edge_entry']['post_c61'] and not proof['terminal_provenance']['edge_entry']['used_c61_source']
assert not any(x.get('proof_kind')=='c61_post_boundary' for x in ref['boundary_path'])
assert ref['path_c63_record_identities'] and proof['c61_authorization']['record_identity']
print('PASS: c64 converted-context shape gains permission solely from exact c63 record, never prior c61 certificate or invented post-c61 origin')

cm,_=certificates64(conversion_functions(jump(0x1400,0x1600,True),{0x1406:bytes.fromhex('488908c3')}));mr=tool.scan(cm)
assert mr['c63_exact_edges_admitted']==2 and mr['c63_boundary_instances_admitted']==1 and len(refs(mr))==2
for ref in refs(mr):
    prov=ref['boundary_path'][-1]['terminal_provenance']
    assert prov['mismatch_delta']=={'context_phase_origin_or_full_prefix_mismatch':1}
    assert prov['mismatch_counts_after']=={'context_phase_origin_or_full_prefix_mismatch':1}
print('PASS: c64 converted multi-successor cache never repeats mismatch conversion or masks changed absolute counters/provenance')

# Per-instance proof-pool lookup seams test the first-admission guard. Immutable
# global pool seals and the original observer remain separately validated.
class WrongLookup:
    def __init__(self,original,change):self.original=original;self.change=change
    def __getattr__(self,name):return getattr(self.original,name)
    def lookup(self,root):
        record=self.original.lookup(root)
        if record is not None:self.change(record)
        return record


def run_tx(cert,cls=tool.View):
    e=cert.baseline.certificates.baseline.pool.baseline.evidence
    root=next(iter(e.proofs));view=cls(cert);tx=tool.Transaction(view,root,c58.c55.Engine(e.session.domain,e.session.profile.summary),c58.Limits())
    reason=None
    try:tx.run()
    except (c58.Refusal,c58.EvidenceError) as error:reason=str(error)
    return tx,view,reason

for field,value in [('original_root_runtime_row',99),('current_entry_instance_row',99),('incoming_state_identity','wrong'),
    ('boundary_path_identity','wrong'),('boundary_transition_identities',[]),('source_instruction_rva',0),('source_instruction_sha256','wrong'),
    ('root_replay_identity','wrong'),('terminal_phase',c63.PHASES[1]),('terminal_reason','current_context_post_c61_reachable'),
    ('post_c59_origin',None),('c61_authorization',{}),('accepted_c62_result_identity','wrong')]:
    fake=WrongLookup(converted,lambda record,field=field,value=value:record.__setitem__(field,value))
    tx,view,reason=run_tx(fake)
    assert reason==c62.POST and not view.c63_eligible and not tx.references,(field,reason)
print('PASS: c64 exact current root/row/state/path/source/full-prefix/phase/reason/origin/authorization identity required before first admission')

for field,value in [('first_failed_conjunct','semantic_record_matches_prior'),('mismatch_delta',{}),('lookup_record_identity','wrong'),('underlying_refusal',tool.POST)]:
    fake=WrongLookup(converted,lambda record,field=field,value=value:record['terminal_provenance'].__setitem__(field,value))
    tx,view,reason=run_tx(fake);assert reason==c62.POST and not view.c63_eligible
print('PASS: c64 converted refusal conjunct, source scan/path provenance and underlying refusal cannot be relabeled or borrowed')

for change in [lambda tx,view:tx.work.update(iterations=1),lambda tx,view:view.c61_mismatches.update(context_phase_origin_or_full_prefix_mismatch=1),
    lambda tx,view:view.c63_record.__setitem__('terminal_reason','wrong'),lambda tx,view:tx.considered.add((999,0,'wrong',0))]:
    class ChangedSibling(tool.View):
        def edge(self,*args):
            proof=super().edge(*args)
            if len(self.c63_siblings_used)==1:change(self.tx,self)
            return proof
    tx,view,reason=run_tx(cm,ChangedSibling)
    assert reason is not None and not tx.references
print('PASS: c64 same-instruction sibling rejects changed work/counters/cache/reason or non-sibling considered keys with no candidate leak')

# First-use proof may not migrate through a later different-path same-state arrival.
tx,view,reason=run_tx(converted);assert reason is None
key=view.c63_key;state,path=tx.contexts[key]
refused(lambda:tx.instance(key[0],state,path+({'edge_identity':'different-path'},)),'c63_predecessor_path_conflict')
tx,view,reason=run_tx(dedup);assert reason is None
key=view.c63_key;state,path=tx.contexts[key]
refused(lambda:tx.instance(key[0],state,path+({'edge_identity':'other-arrival'},)),'c63_predecessor_path_conflict')
print('PASS: c64 later different-path same-state arrivals invalidate already used converted or dedup c63 certificates and descendants')

# A new post-arrival origin cannot silently alter an already used converted source.
tx,view,_=run_tx(converted)
key=view.c63_key;tx.diagnostic_arrival=(key,tx.contexts[key][1]+(next(iter(view.proofs_at_c61.values())),))
refused(lambda:tx.mark_post_c61(key),'c63_origin_conflict')
print('PASS: c64 newly post-c61 source origin after converted c63 use aborts instead of retroactively changing phase/reachability')

rr=scan64(next_functions(next_body=b'\x90',targets={0x110b:bytes.fromhex('488b08c3')}))
assert rr['root_refusal_reasons'][tool.POST]==rr['post_c63_uncertified_boundary_refusals']==1
assert rr['c63_boundary_instances_admitted']==0 and rr['c63_exact_edges_admitted']==0 and not refs(rr)
print('PASS: c64 next unknown boundary refuses post-c63 even with historical row-global proof and rolls back provisional edge admission')

f=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+bytes.fromhex('7505')+jump(0x1110,0x1500)+bytes.fromhex('7505')+jump(0x1117,0x1600)+jump(0x111c,0x1700))
f.update({0x1400:b'\xc3',0x1500:b'\xc3',0x1600:b'\xc3',0x1700:b'\xc3'})
rr=scan64(f);assert rr['post_c63_uncertified_boundary_refusals'] and not refs(rr)
print('PASS: c64 further boundary in same source row after c63 preflight is never recursively certified')

# Positive old completed root keeps original c62 candidate bytes, not just count0.
f=next_functions(next_body=bytes.fromhex('488b08c3'));f.update({0x2000:lea(0x2000)+jump(0x2007,0x2100),0x2100:bytes.fromhex('488908c3')})
old,_=certificates64(f);baseline_before=deepcopy(old.baseline.result);rr=tool.scan(old)
legacy=[ref for ref in refs(rr) if 'c64_authentication_identity' not in ref]
assert legacy==baseline_before['reference_candidates'] and legacy
assert old.baseline.result==baseline_before and rr['historical_c62_candidate_output_sha256']==c56.digest(legacy)
print('PASS: c64 preserves complete positive historical c62 candidate bytes for unextended roots and never mutates accepted baseline output')

rr=scan64(next_functions(next_body=bytes.fromhex('488b00488b08c3')))
assert len(refs(rr))==1 and refs(rr)[0]['site_rva']==0x110a  # The slot-source read itself is legitimate.
for body in [bytes.fromhex('b800000000488b08c3'),bytes.fromhex('488d05')+struct.pack('<i',cfg.SLOT-0x1111)+bytes.fromhex('488b08c3')]:
    rr=scan64(next_functions(next_body=body));assert not refs(rr)
print('PASS: c64 loaded/partial/independent local origins cannot fabricate incoming-carried Fact candidates or general pointers')

factless=local_functions();factless[0x1000]=bytes.fromhex('4831c0')+jump(0x1003,0x1100)
rr=scan64(factless);assert not refs(rr) and rr['c63_certified_crossings_with_facts']==0 and rr['c63_boundary_instances_admitted']
print('PASS: c64 factless structural preflight and independent EMPTY roots produce no useful crossing or fake candidate')

# A must-join conflict kills the carried Fact at a target operand.
f=next_functions(next_body=bytes.fromhex('75054831c0eb0090488b08c3'))
rr=scan64(f);assert not refs(rr) and rr['root_transactions_refused']==0 and rr['c63_boundary_instances_admitted']==1
print('PASS: c64 exact local must-join agreement remains necessary for incoming-carried operand facts')

# Retain existing deterministic finite per-root caps and rollback semantics.
for name in ['instances','path_transitions','iterations','bytes','instructions','states','edges','blocks','transitions']:
    rr=tool.scan(p,replace(c58.Limits(),**{name:1}));assert not refs(rr)
    assert rr['root_transactions_refused'] and rr['cross_row_cap_refusals']
for limits in [replace(c58.Limits(),instances=0),replace(c58.Limits(),instances=True)]:refused(lambda:tool.scan(p,limits),'positive integers')
print('PASS: c64 original instance/path/work/reference caps stay finite; cap failures commit no provisional candidates')

# Every output phase/reason is counted in its accepted-record domain.
for cert in [p,local,dedup,converted]:
    rr=tool.scan(cert)
    assert rr['c63_boundary_instances_considered']==rr['c63_boundary_instances_admitted']+rr['c63_boundary_instances_refused']
    assert rr['c63_exact_edges_considered']==rr['c63_exact_edges_admitted']+rr['c63_exact_edges_refused']
    assert sum(rr['c63_terminal_phase_admitted'].values())==sum(rr['c63_terminal_reason_admitted'].values())==rr['c63_boundary_instances_admitted']
print('PASS: c64 all four accepted terminal phases and three reasons keep edge/instance/structural/fact count scopes separate')

for field in ('schema_version','contract','c56_manifest_id','accepted_c61_record_set_identity','accepted_c62_result_identity','accepted_c63_classifier_identity','accepted_c63_record_set_identity','limits'):
    prior=p.header[field];p.header[field]='wrong';refused(lambda:tool.scan(p),'identity');p.header[field]=prior
prior=p.records
for field in ['terminal_phase','terminal_reason','terminal_provenance','post_c61_origin','source_code_sha256','incoming_state_identity']:
    record=json.loads(prior[0]);record[field]='wrong';p.records=(c56.encoded(record),)+prior[1:]
    refused(lambda:tool.scan(p),'identity')
p.records=prior
print('PASS: c64 mutated c56-c64 schema/contract/proof/result/record-set/phase/reason/provenance seals fail globally')

for field in ['schema_version','contract','classifier_identity','authenticated_record_set_sha256','terminal_reason_counts']:
    bad=deepcopy(p.result);bad[field]='wrong';refused(lambda:tool.ProofPool(p.baseline,accepted=p.result,owner_result=bad),'c63 JSON')
refused(lambda:tool.ProofPool(p.baseline),'accepted c63')
refused(lambda:tool.analyze(raw),'SHA256')
print('PASS: c64 entire owner c63 aggregate and accepted identity/count distribution required; no image/count/schema CLI bypass')

prior=p.baseline;p.baseline=converted.baseline;refused(p.assert_current,'identity');p.baseline=prior;p.assert_current()
root=next(iter(p.index));ordinal=p.index[root];p.index[root]=999;refused(p.assert_current,'identity');p.index[root]=ordinal
print('PASS: c64 substituted valid baseline or mutated root index cannot silently change authenticated c63 pool membership')

with tempfile.TemporaryDirectory(prefix='ws-c64-chain-') as directory:
    root=Path(directory);cache=root/'cache';cache.mkdir();b=p.baseline;cert=b.certificates;old=cert.baseline;e=old.pool.baseline.evidence;s=e.session
    c56.atomic_json(cache/'manifest.json',s.manifest)
    for i,(start,end) in enumerate(s.plan):c56.atomic_json(cache/f'shard-{start:06d}-{end:06d}.json',s.make_shard(i))
    data=dict(c57=e.result,c58=old.pool.baseline.result,c59=old.pool.result,c60=old.result,c61=cert.result,c62=b.result,c63=p.result)
    for name,value in data.items():c56.atomic_json(root/(name+'.json'),value)
    before={x.relative_to(root):x.read_bytes() for x in root.rglob('*.json')}
    prior=c57.collect_baseline(s,cache,acceptance=c57.Acceptance(len(s.rows),len(e.proofs)))
    rel=e.result['target_metadata_relation_counts'];accept=c58.AcceptedEdges(e.result['boundary_edges_total'],rel['exact_unique_runtime_begin'],rel['inside_unique_runtime_span_nonentry'],rel['executable_file_backed_outside_runtime_metadata'],e.result['model_only_boundary_edges_total'],sum(e.result['exact_adjacency_edge_kind_counts'].values()))
    ee=c58.Evidence(prior,accepted=accept,owner_result=c56.read_json(root/'c57.json'))
    bb=c59.Baseline(ee,accepted=data['c58'],owner_result=c56.read_json(root/'c58.json'))
    cc=c60.ProofPool(bb,accepted=data['c59'],owner_result=c56.read_json(root/'c59.json'))
    dd=c61.Baseline(cc,accepted=data['c60'],owner_result=c56.read_json(root/'c60.json'))
    pp=c62.ProofPool(dd,accepted=data['c61'],owner_result=c56.read_json(root/'c61.json'))
    prior63=c63.Baseline(pp,accepted=data['c62'],owner_result=c56.read_json(root/'c62.json'))
    again=tool.ProofPool(prior63,accepted=data['c63'],owner_result=c56.read_json(root/'c63.json'))
    assert tool.scan(again)==tool.scan(p) and before=={x.relative_to(root):x.read_bytes() for x in root.rglob('*.json')}
    (root/'c63.json').write_text('{}');refused(lambda:tool.ProofPool(prior63,accepted=data['c63'],owner_result=c56.read_json(root/'c63.json')),'c63 JSON')
print('PASS: c64 complete c56 checkpoint plus c57-c63 JSON replay remains read-only; corrupt supplied evidence refuses without repair')

with tempfile.TemporaryDirectory(prefix='ws-c64-cli-') as directory:
    root=Path(directory);exe=root/'owned.exe';exe.write_bytes(raw);out=root/'result.json';out.write_text('preserve')
    inputs=[]
    for n in range(57,64):
        file=root/f'c{n}.json';file.write_text('{}');inputs.append((n,file))
    before={pth:pth.read_bytes() for _,pth in inputs};before[exe]=exe.read_bytes()
    for file in [exe]+[pth for _,pth in inputs]:
        args=[sys.executable,tool.__file__,str(exe),'--output',str(file)]
        for n,pth in inputs:args+=['--c'+str(n)+'-json',str(pth)]
        proc=subprocess.run(args,capture_output=True,text=True);assert proc.returncode==2 and 'immutable input' in proc.stderr
    proc=subprocess.run([sys.executable,tool.__file__,str(exe),'--output',str(out)],capture_output=True,text=True)
    assert proc.returncode==2 and out.read_text()=='preserve' and all(pth.read_bytes()==data for pth,data in before.items())
print('PASS: c64 CLI cannot overwrite EXE, checkpoints or any accepted JSON and wrong image preserves prior output')

rr=tool.scan(p)
assert all(rr[k] is False for k in c56.FALSE_GATES) and rr['live_qword_reads']==0 and rr['publication_source_kind']=='unproven'
assert rr['c52_production_watch_gate']=='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'
proc=subprocess.run([sys.executable,tool.__file__,'--help'],capture_output=True,text=True)
assert proc.returncode==0 and '--c63-json' in proc.stdout
proc=subprocess.run([sys.executable,tool.__file__,'missing-private.exe','--output','unused-c64.json'],capture_output=True,text=True)
assert proc.returncode==2 and tool.BLOCKED in proc.stderr and not Path('unused-c64.json').exists()
print('PASS: c64 all completeness/publication/ownership/lifetime/production/watch/game gates remain false; missing private EXE is BLOCKED, never native/game PASS')

many,_=certificates64(next_functions(next_body=bytes.fromhex('488b08488908c3')))
rr=tool.scan(many,replace(c58.Limits(),root_references=1));assert not refs(rr) and rr['cross_row_cap_refusals']
rr=tool.scan(many,replace(c58.Limits(),retained_references=1))
assert rr['reference_candidate_count']==2 and len(refs(rr))==1 and rr['candidate_records_not_retained']==1
print('PASS: c64 per-root reference cap aborts transaction while global retained-record bound never truncates exact candidate counts')

rr=scan64(next_functions(b'\xf2'+jump(0x110a,0x1400),b'\xc3',{0x1400:bytes.fromhex('488b08c3')}))
assert not refs(rr) and rr['c63_boundary_instances_admitted']==0 and rr['root_refusal_reasons']['model_only_successor']
for target,extra in [(cfg.DEFERRED,[]),(0x1400,[(b'.rw',0x1400,b'\xc3',0xc0000040,1)]),
    (0x1401,[(b'.unbacked',0x1400,b'\x90',0x60000020,20)])]:
    refused(lambda:certificates64(next_functions(jump(0x1109,target),None),extra))
print('PASS: c64 model-only/deferred/nonexec/unbacked evidence refuses with existing image/oracle rules, never recovered by adjacency')

class TargetRefuses(tool.View):
    def row(self,index):
        if self.c63_considered and self.session.rows[index][0]==0x110a:
            raise c58.Refusal('owned_current_target_preflight_refused')
        return super().row(index)
tx,view,reason=run_tx(p,TargetRefuses)
assert reason=='owned_current_target_preflight_refused' and not view.c63_eligible and not tx.references
assert view.c63_refusals['target_code_preflight_refused']==1
print('PASS: c64 exact target metadata is insufficient without current immutable executable file-backed row/code preflight')

# Legitimate old same-state c57 cycles still deduplicate; c63 extension into a
# future unknown/cyclic boundary remains closed instead of fabricating more proofs.
rr=scan64({0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:jump(0x1100,0x1000)})
assert rr['cross_row_cycles_seen'] and not refs(rr)
rr=scan64(next_functions(next_body=jump(0x110a,0x1100)))
assert rr['post_c63_uncertified_boundary_refusals'] and not refs(rr)
print('PASS: c64 preserves old bounded same-state cycle dedup and refuses next-wave cycles without candidate or recursive certificate')

class DuplicateCallback(tool.View):
    def edge(self,*args):
        proof=super().edge(*args)
        if self.c63_record is not None:super().edge(*args)
        return proof
tx,view,reason=run_tx(p,DuplicateCallback)
assert reason=='duplicate_c63_sibling_callback' and not tx.references
with tempfile.TemporaryDirectory(prefix='ws-c64-cache-') as directory:
    root=Path(directory);exe=root/'owned.exe';exe.write_bytes(raw);cache=root/'cache';cache.mkdir();part=cache/'existing.json';part.write_text('preserve')
    proc=subprocess.run([sys.executable,tool.__file__,str(exe),'--c56-cache',str(cache),'--output',str(part)],capture_output=True,text=True)
    assert proc.returncode==2 and part.read_text()=='preserve'
print('PASS: c64 duplicate sibling callbacks and output-over-checkpoint aliases fail closed with original evidence preserved')
