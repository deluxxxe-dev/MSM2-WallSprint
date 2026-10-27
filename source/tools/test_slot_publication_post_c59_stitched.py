"""Owned c62 full-prefix/phase/origin/all-successor/counterexample tests."""
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

import slot_publication_post_c59_stitched as tool
c61,c60,c59,c58,c57,c56,cfg=tool.c61,tool.c60,tool.c59,tool.c58,tool.c57,tool.c56,tool.cfg

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
    return tool.ProofPool(b,accepted=r,owner_result=r),raw


def scan62(f,extra=(),limits=c58.Limits()):
    p,_=certificates(f,extra)
    return tool.scan(p,limits)


p,raw=certificates(post_functions(next_body=bytes.fromhex('488b08c3')))
r=tool.scan(p)
assert r['c61_boundary_instances_admitted']==r['c61_exact_edges_admitted']==1
assert r['c61_certified_crossings_with_facts']==r['c61_address_facts_crossing_rows']==1
assert len(refs(r,'READ'))==1
ref=refs(r)[0];assert [x.get('proof_kind','c57') for x in ref['boundary_path']]==['c57','c59_later_boundary','c61_post_boundary']
assert ref['address_proof_chain']==((0x1000,'rip_lea',cfg.SLOT),)
assert ref['path_c61_record_identities'] and ref['root_c61_record_identities']
assert ref['boundary_path'][-1]['terminal_phase']=='post_c59_reachable_context'
print('PASS: c62 exact c61 ordinary fallthrough carries existing fact and complete separate c57/c59/c61 path proof without adding a Fact hop')

for kind,body,next_body,targets in [
    ('direct_unconditional_branch',jump(0x1108,0x1400),None,{0x1400:bytes.fromhex('488908c3')}),
    ('direct_conditional_taken',jump(0x1108,0x1400,True)+b'\xc3',None,{0x1400:bytes.fromhex('488908c3')}),
    ('conditional_fallthrough',jump(0x1108,0x1108,True),bytes.fromhex('488908c3'),{})]:
    rr=scan62(post_functions(body,next_body,targets))
    assert len(refs(rr,'WRITE'))==1 and refs(rr)[0]['boundary_path'][-1]['edge_kind']==kind
print('PASS: c62 exact conditional taken/fallthrough and nonadjacent direct branch require original c61 source authorization')

ff=post_functions(jump(0x1108,0x1400,True),bytes.fromhex('488b08c3'),{0x1400:bytes.fromhex('488908c3')})
rr=scan62(ff)
assert rr['c61_boundary_instances_admitted']==1 and rr['c61_exact_edges_admitted']==2 and len(refs(rr))==2
assert rr['c61_certified_crossings_with_facts']==2
print('PASS: c62 all-exact two-successor boundary preflights both before one scan-token-limited sibling certificate reuse')

for free in (False,True):
    target=0x1400 if free else 0x1401
    ff=post_functions(jump(0x1108,target,True),bytes.fromhex('488b08c3'),None if free else {0x1400:b'\x90\xc3'})
    extra=[(b'.free',0x1400,b'\xc3',0x60000020,1)] if free else []
    rr=scan62(ff,extra)
    assert rr['c61_exact_edges_considered']==1 and rr['c61_exact_edges_admitted']==0
    assert rr['c61_boundary_instances_refused']==1 and not refs(rr)
print('PASS: c62 one exact plus interior or metadata-free RX successor rejects the entire boundary/root with no provisional candidate')

ff=post_functions(b'\xe8'+struct.pack('<i',0x1200-0x110d),bytes.fromhex('488b0b488b08488b0c24c3'))
ff[0x1000]=lea(0x1000,reg=0)+lea(0x1007,reg=3)+lea(0x100e,reg=4)+jump(0x1015,0x1100)
ff[0x1200]=bytes.fromhex('31c0c3')
rr=scan62(ff)
assert len(refs(rr,'READ'))==1 and refs(rr)[0]['site_rva']==0x110d
assert rr['call_edges_with_preserved_facts_crossing_rows']==1
assert refs(rr)[0]['boundary_path'][-1]['edge_kind']=='call_fallthrough'
print('PASS: c62 CALL fallthrough keeps only c54/c55-authorized RBX and kills callee-written RAX plus RSP with no ABI/return-value origin')

# Local preflight phase: no completed c59 crossing in this candidate path.
ff=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+jump(0x110e,0x1500))
ff.update({0x1400:b'\xc3',0x1500:bytes.fromhex('488b08c3')})
pp,_=certificates(ff);rr=tool.scan(pp)
assert len(refs(rr,'READ'))==1
proof=refs(rr)[0]['boundary_path'][-1]
assert proof['terminal_phase']==tool.PHASES[0] and proof['post_c59_origin'] is None and proof['local_c59_record_identities']
assert not refs(rr)[0]['path_c59_record_identities'] and rr['c61_terminal_phase_admitted'][tool.PHASES[0]]==2
print('PASS: c62 source-local post-preflight phase preserves local c59 IDs without fabricating a completed c59 crossing/path segment')

# Match the two-owner-case SHAPE on owned buffers, not proprietary image rows.
ff=functions(jump(0x1107,0x1200));ff[0x1200]=jump(0x1200,0x1400);ff[0x1400]=bytes.fromhex('488b08c3')
pp,_=certificates(ff);rr=tool.scan(pp)
assert len(refs(rr,'READ'))==1 and rr['c61_terminal_phase_admitted'][tool.PHASES[2]]==1
proof=refs(rr)[0]['boundary_path'][-1]
assert proof['terminal_phase']==tool.PHASES[2] and proof['post_c59_origin']['arrival_path']['c59_record_identities']
assert not refs(rr)[0]['path_c59_record_identities']
assert proof['source_boundary_path_identity']!=proof['post_c59_origin']['arrival_path']['boundary_path_identity']
print('PASS: c62 dedup-stored-earlier-path phase can authorize only exact full prefix while preserving distinct stored path and authenticated c59 origin')

# Two roots/states expose two owned dedup-shape records, both independently bound.
ff[0x1080]=lea(0x1080,cfg.SLOT+8)+jump(0x1087,0x1100)
pp2,_=certificates(ff);rr2=tool.scan(pp2)
assert rr2['c61_terminal_phase_considered'][tool.PHASES[2]]==2 and rr2['c61_terminal_phase_admitted'][tool.PHASES[2]]==2
assert len(refs(rr2,'READ'))==1 and refs(rr2)[0]['original_root_runtime_row']==0
print('PASS: c62 two owned instances of accepted dedup shape remain root/state/origin specific and never globalize a row fact')

# Replay the exact original prefix and pause at the first terminal POST.
def active_view(pool,view_type=tool.View):
    e=pool.baseline.pool.baseline.evidence
    v=view_type(pool);t=tool.Transaction(v,0,c58.c55.Engine(e.session.domain,e.session.profile.summary),c58.Limits())
    # Call original observer boundary logic for the prefix, disabling only the
    # new edge admission callback in this owned seam (no historical mutation).
    def observe(index,ins,kind,target,arch,relations):return c61.View.edge(v,index,ins,kind,target,arch,relations)
    v.edge=observe
    refused(t.run,c60.POST)
    del v.edge
    capture=deepcopy(v.capture);key=(capture['row'],capture['state_identity'])
    ins=next(cfg.old.decoder().disasm(e.session.read(capture['pc'],capture['size']),capture['pc'],count=1))
    edge=c59.classify_edges(e.session,*e.session.rows[key[0]][:2],ins)[0]
    return v,t,key,ins,edge

# Snapshot has one original edge callback already counted. Restore precisely
# that pre-callback relation state so the next call reproduces original record.
def rewind_edge(view,tx,edge):
    tx.relations[edge['target_metadata_relation']]-=1
    if tx.relations[edge['target_metadata_relation']]==0:del tx.relations[edge['target_metadata_relation']]
    view.lookup_misses-=1;view.capture=None

for cert_case,mutation in [(p,'root'),(p,'state'),(p,'path'),(p,'source'),(p,'work'),(p,'origin'),(pp,'origin')]:
    view,tx,key,ins,edge=active_view(cert_case);rewind_edge(view,tx,edge)
    if mutation=='root':tx.root=1
    if mutation=='state':
        state,path=tx.contexts[key];state=list(state);state[0]=cfg.Fact(cfg.SLOT+8,state[0].chain);tx.contexts[key]=(tuple(state),path)
    if mutation=='path':
        state,path=tx.contexts[key];path=deepcopy(path);path[0]['edge_identity']='wrong';tx.contexts[key]=(state,path)
    if mutation=='source':ins=next(cfg.old.decoder().disasm(jump(ins.address,0x1400),ins.address,count=1))
    if mutation=='work':tx.work['iterations']+=1
    if mutation=='origin':tx.post_origins[key]['arrival_path']['boundary_path_identity']='wrong'
    refused(lambda:view.edge(key[0],ins,edge['edge_kind'],edge['edge_target_rva'],True,tx.relations))
    assert not view.c61_eligible
print('PASS: c62 wrong root/state/path/source/full-prefix work/phase/origin refuses before first authorization, including earlier-stored-path origin mismatch')

# Interpose only the owned observer seam; no historical module mutation.
class WrongPhaseCapture(c61.View):
    def edge(self,*args):
        try:return super().edge(*args)
        except c58.Refusal as error:
            if str(error)==c60.POST:self.capture['phase']='wrong_phase'
            raise
class WrongPhaseView(tool.View,WrongPhaseCapture):pass
view,tx,key,ins,edge=active_view(p,WrongPhaseView);rewind_edge(view,tx,edge)
refused(lambda:view.edge(key[0],ins,edge['edge_kind'],edge['edge_target_rva'],True,tx.relations))
assert not view.c61_eligible
print('PASS: c62 mismatching terminal phase in an otherwise exact source capture refuses without altering original admission semantics')

# A genuine exact-prefix event is positive under the same pause/replay seam.
view,tx,key,ins,edge=active_view(p);rewind_edge(view,tx,edge)
proof=view.edge(key[0],ins,edge['edge_kind'],edge['edge_target_rva'],True,tx.relations)
assert proof['c61_record_identity'] and view.c61_source_keys=={key}
state,path=tx.contexts[key];other=deepcopy(path);other[0]['edge_identity']='new_arrival'
refused(lambda:tx.instance(key[0],state,other),'c61_predecessor_path_conflict')
print('PASS: c62 late different-path same-state arrival invalidates an already used c61 certificate instead of borrowing it')

view,tx,key,ins,edge=active_view(pp);rewind_edge(view,tx,edge)
proof=view.edge(key[0],ins,edge['edge_kind'],edge['edge_target_rva'],True,tx.relations)
assert proof['terminal_phase']==tool.PHASES[2]
state,path=tx.contexts[key];other=deepcopy(path);other[0]['edge_identity']='another_origin_path'
refused(lambda:tx.instance(key[0],state,other),'c61_predecessor_path_conflict')
print('PASS: c62 accepted dedup-phase certificate cannot migrate to another stored/origin path after authorization')

view,tx,key,ins,edge=active_view(p);rewind_edge(view,tx,edge)
state,path=tx.contexts[key];before=tx.deduplicated
assert tx.instance(key[0],state,path)==key and tx.deduplicated==before+1
assert tool.scan(p)==tool.scan(p)
print('PASS: c62 identical exact states/paths deduplicate deterministically without conflating different certificate contexts')

# New target has another boundary. Even a c57 row-global proof cannot authorize
# the new post-c61 path; earlier provisional READ must be discarded.
ff=post_functions(next_body=bytes.fromhex('488b08')+jump(0x110c,0x1400),targets={0x1400:b'\xc3'})
rr=scan62(ff)
assert not refs(rr) and rr['post_c61_uncertified_boundary_refusals'] and rr['c61_boundary_instances_admitted']==0
print('PASS: c62 next unknown row-external boundary refuses post-c61 even with a historical c57 row proof, leaking no provisional candidate')

# The earlier c59 arm and the new c61 arm merge at same state; a stored first
# path may not hide new post-c61 reachability or borrow a processed boundary.
ff=post_functions(jump(0x1108,0x1200),None);ff[0x1200]=bytes.fromhex('488b08')+jump(0x1203,0x1400);ff[0x1400]=b'\xc3'
rr=scan62(ff)
assert not refs(rr) and rr['post_c61_uncertified_boundary_refusals']
print('PASS: c62 post-c61 reachability survives same-state dedup and late arrivals cannot borrow earlier processed descendants')

# A source-local second boundary after C61 admission also remains unknown.
ff=post_functions(bytes.fromhex('7505')+jump(0x110a,0x1400)+jump(0x110f,0x1500),None,{0x1400:b'\xc3',0x1500:b'\xc3'})
rr=scan62(ff)
assert not refs(rr) and rr['post_c61_uncertified_boundary_refusals']
print('PASS: c62 further same-instance source boundary after c61 preflight is not recursively certified')

for body in (bytes.fromhex('488b00488b08c3'),bytes.fromhex('89c0488b08c3'),bytes.fromhex('488b0c08c3')):
    rr=scan62(post_functions(next_body=body))
    assert len(refs(rr))==(1 if body==bytes.fromhex('488b00488b08c3') else 0)
ff=post_functions(next_body=lea(0x1109)+bytes.fromhex('488b08c3'))
rr=scan62(ff);assert not refs(rr)
print('PASS: c62 loaded/partial/indexed values and independent new RIP origins never become incoming-carried pointer candidates')

for arm,expected in [(bytes.fromhex('31c0'),0),(bytes.fromhex('9090'),1)]:
    body=bytes.fromhex('7504')+arm+bytes.fromhex('eb02')+b'\x90\x90'+bytes.fromhex('488b08c3')
    rr=scan62(post_functions(next_body=body))
    assert len(refs(rr,'READ'))==expected
print('PASS: c62 target-row exact must joins retain only full agreement on every local predecessor')

ff=functions(bytes.fromhex('7505')+jump(0x1109,0x1400)+jump(0x110e,0x1500))
ff.update({0x1400:b'\xc3',0x1500:bytes.fromhex('488b08c3')});del ff[0x1000]
rr=scan62(ff)
assert not refs(rr) and rr['c61_boundary_instances_admitted']==1 and rr['c61_certified_crossings_with_facts']==0
print('PASS: c62 factless structural continuation and independent EMPTY roots do not fabricate a candidate or c59 crossing')

# Historical c57-only cycle behavior remains unchanged; changing cycle fails.
ff={0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:bytes.fromhex('488b08')+jump(0x1103,0x1200),0x1200:jump(0x1200,0x1100)}
rr=scan62(ff);assert refs(rr) and rr['cross_row_cycles_seen'] and rr['continuation_instances_deduplicated']
ff[0x1200]=bytes.fromhex('488d4008')+jump(0x1204,0x1100)
rr=scan62(ff);assert not refs(rr) and rr['root_refusal_reasons']['state_changing_cross_row_cycle']
print('PASS: c62 original same-state c57 cycles deduplicate and state-changing cycles discard complete root candidates')

for name in asdict(c58.Limits()):
    if name in ('root_references','retained_references'):continue
    rr=tool.scan(p,replace(c58.Limits(),**{name:1}))
    assert not refs(rr) and rr['cross_row_cap_refusals'],name
pp_refs,_=certificates(post_functions(next_body=bytes.fromhex('488b08488908c3')))
rr=tool.scan(pp_refs,replace(c58.Limits(),root_references=1));assert not refs(rr) and rr['cross_row_cap_refusals']
rr=tool.scan(pp_refs,replace(c58.Limits(),retained_references=1))
assert rr['reference_candidate_count']==2 and len(refs(rr))==1 and rr['candidate_records_not_retained']==1
print('PASS: c62 original work/context/path/reference caps stay finite; failures leak no candidates and retained records never truncate counts')

ff=post_functions(b'\xf2'+jump(0x1109,0x1400),bytes.fromhex('488b08c3'),{0x1400:bytes.fromhex('488908c3')})
rr=scan62(ff);assert not refs(rr) and rr['root_refusal_reasons']['model_only_successor']
refused(lambda:certificates({0x1100:b'\x90'*8,0x1104:b'\xc3'}))
rr=scan62(post_functions(jump(0x1108,0xD000010),None))
assert not refs(rr) and rr['c61_boundary_instances_admitted']==0
print('PASS: c62 model-only/overlapping/outside paths refuse without bypassing historical image/oracle checks')

# Independent exact-target preflight remains required even when metadata alone
# calls it an exact beginning. Test unbacked/nonexec through owned PE mutation.
def changed_target(raw,field,value):
    data=bytearray(raw)
    for i in range(struct.unpack_from('<H',data,0x86)[0]):
        at=0x188+40*i
        if struct.unpack_from('<I',data,at+12)[0]==0x1400:
            struct.pack_into('<I',data,at+field,value);return bytes(data)
    raise AssertionError('missing target')
for field,value in [(36,0xc0000040),(16,0)]:
    ff=post_functions(jump(0x1108,0x1400),None,{0x1400:bytes.fromhex('488b08c3')})
    e,rrraw,_,_=evidence(ff,mutate=lambda raw:changed_target(raw,field,value))
    old=c58.scan(e);b59=c59.Baseline(e,accepted=old);r59=c59.classify(b59)
    pool60=c60.ProofPool(b59,accepted=r59);old60=c60.scan(pool60);b61=c61.Baseline(pool60,accepted=old60)
    r61=c61.classify(b61);cert=tool.ProofPool(b61,accepted=r61)
    rr=tool.scan(cert);assert not refs(rr) and rr['c61_exact_edges_considered']==1 and rr['c61_exact_edges_admitted']==0
print('PASS: c62 exact metadata target still refuses non-executable/writable or unbacked current code')

# Other invalid relation classes are rejected by earlier accepted c57 evidence;
# preserve that refusal, never invent a later c61 record to bypass it.
for target,extra in [(cfg.DEFERRED,[]),(0x1400,[(b'.rw',0x1400,b'\xc3',0xc0000040,1)]),
    (0x1401,[(b'.unbacked',0x1400,b'\x90',0x60000020,20)])]:
    refused(lambda:certificates(post_functions(jump(0x1108,target),None),extra))
print('PASS: c62 deferred/nonexec/unbacked relation source refusals remain blocked by original c57 evidence rather than normalized into new certificates')

for field in ('schema_version','contract','c56_manifest_id','accepted_c57_boundary_identity','accepted_c59_result_identity',
    'accepted_c60_authentication_identity','accepted_c60_result_identity','accepted_c61_classifier_identity',
    'accepted_c61_result_identity','accepted_c61_record_set_identity','limits'):
    old=p.header[field];p.header[field]='wrong';refused(lambda:tool.scan(p),'identity');p.header[field]=old
print('PASS: c62 stale/wrong c56-c62 schema/contract/cap/proof/result/record-set identities fail globally')

old=p.records
for field in ('terminal_phase','post_c59_origin','local_c59_record_identities','incoming_state_identity','boundary_path_identity','source_code_sha256'):
    record=json.loads(old[0]);record[field]='wrong';p.records=(c56.encoded(record),)+old[1:]
    refused(lambda:tool.scan(p),'identity')
p.records=old
root=next(iter(p.index));ordinal=p.index[root];p.index[root]=99
refused(lambda:tool.scan(p),'identity');p.index[root]=ordinal
print('PASS: c62 mutated c61 phase/origin/state/source/index records cannot enter or migrate within a proof pool')

for field in ('schema_version','contract','classifier_identity','authenticated_record_set_sha256','terminal_phase_counts'):
    bad=deepcopy(p.result);bad[field]='wrong'
    refused(lambda:tool.ProofPool(p.baseline,accepted=p.result,owner_result=bad),'c61 JSON')
refused(lambda:tool.ProofPool(p.baseline),'accepted c61')
refused(lambda:tool.analyze(raw),'SHA256')
print('PASS: c62 entire owner c61 aggregate, accepted identities/counts and pinned image are required with no CLI bypass')

with tempfile.TemporaryDirectory() as directory:
    root=Path(directory);cache=root/'cache';cache.mkdir();b=p.baseline;s=b.pool.baseline.evidence.session;e=b.pool.baseline.evidence
    c56.atomic_json(cache/'manifest.json',s.manifest)
    for i,(start,end) in enumerate(s.plan):c56.atomic_json(cache/f'shard-{start:06d}-{end:06d}.json',s.make_shard(i))
    for name,data in [('c57',e.result),('c58',b.pool.baseline.result),('c59',b.pool.result),('c60',b.result),('c61',p.result)]:c56.atomic_json(root/(name+'.json'),data)
    before={x.relative_to(root):x.read_bytes() for x in root.rglob('*.json')}
    prior=c57.collect_baseline(s,cache,acceptance=c57.Acceptance(len(s.rows),len(e.proofs)))
    rel=e.result['target_metadata_relation_counts'];accept=c58.AcceptedEdges(e.result['boundary_edges_total'],rel['exact_unique_runtime_begin'],
        rel['inside_unique_runtime_span_nonentry'],rel['executable_file_backed_outside_runtime_metadata'],e.result['model_only_boundary_edges_total'],sum(e.result['exact_adjacency_edge_kind_counts'].values()))
    ee=c58.Evidence(prior,accepted=accept,owner_result=c56.read_json(root/'c57.json'))
    bb=c59.Baseline(ee,accepted=b.pool.baseline.result,owner_result=c56.read_json(root/'c58.json'))
    cc=c60.ProofPool(bb,accepted=b.pool.result,owner_result=c56.read_json(root/'c59.json'))
    dd=c61.Baseline(cc,accepted=b.result,owner_result=c56.read_json(root/'c60.json'))
    again=tool.ProofPool(dd,accepted=p.result,owner_result=c56.read_json(root/'c61.json'))
    assert tool.scan(again)==tool.scan(p) and before=={x.relative_to(root):x.read_bytes() for x in root.rglob('*.json')}
    (root/'c61.json').write_text('{}');refused(lambda:tool.ProofPool(dd,accepted=p.result,owner_result=c56.read_json(root/'c61.json')),'c61 JSON')
print('PASS: c62 c56 checkpoint+c57-through-c61 inputs are read-only and corrupt supplied evidence refuses without fallback/repair')

with tempfile.TemporaryDirectory() as directory:
    root=Path(directory);exe=root/'owned.exe';exe.write_bytes(raw)
    for flag,path in [(None,exe)]+[('--c'+str(n)+'-json',root/('c'+str(n)+'.json')) for n in range(57,62)]:
        if flag:path.write_text('{}')
        args=[str(exe),'--output',str(path)]+([flag,str(path)] if flag else [])
        before=path.read_bytes();proc=subprocess.run([sys.executable,tool.__file__,*args],capture_output=True,text=True)
        assert proc.returncode==2 and 'immutable input' in proc.stderr and before==path.read_bytes()
print('PASS: c62 CLI output aliases cannot overwrite EXE or any accepted owner JSON')

assert all(r[k] is False for k in c56.FALSE_GATES) and r['live_qword_reads']==0
assert r['c52_production_watch_gate'].endswith('_FAIL') and r['publication_source_kind']=='unproven'
assert r['c61_boundary_instances_considered']==r['c61_boundary_instances_admitted']+r['c61_boundary_instances_refused']
assert r['c61_exact_edges_considered']==r['c61_exact_edges_admitted']+r['c61_exact_edges_refused']
assert sum(r['c61_terminal_phase_admitted'].values())==r['c61_boundary_instances_admitted']
print('PASS: c62 phase/instance/edge scopes stay separate and all completeness/publication/ownership/lifetime/production gates remain false')

proc=subprocess.run([sys.executable,tool.__file__,'--help'],capture_output=True,text=True)
assert proc.returncode==0 and '--c61-json' in proc.stdout
proc=subprocess.run([sys.executable,tool.__file__,'missing.exe','--output','out.json'],capture_output=True,text=True)
assert proc.returncode==2 and tool.BLOCKED in proc.stderr
print('PASS: c62 CLI documents read-only full chain and absent private image returns BLOCKED without fabricated metrics')
