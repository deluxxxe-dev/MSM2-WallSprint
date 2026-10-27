"""Owned c60 all-successor/exact-instance/provenance/refusal tests, no private EXE."""
from contextlib import contextmanager
from collections import Counter
from copy import deepcopy
from dataclasses import asdict,replace
from pathlib import Path
import json
import struct
import subprocess
import sys
import tempfile

import slot_publication_later_stitched as tool
c59,c58,c57,c56,cfg=tool.c59,tool.c58,tool.c57,tool.c56,tool.cfg

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
    return tool.ProofPool(b,accepted=r,owner_result=r),raw


def functions(tail=b'\x90',following=None,roots=None):
    f={0x1000:lea(0x1000)+jump(0x1007,0x1100),
        0x1100:bytes.fromhex('7505')+jump(0x1102,0x1200)+tail,0x1200:b'\xc3'}
    if following is not None:f[0x1107+len(tail)]=following
    if roots:f.update(roots)
    return f


def scan(f,extra=(),limits=c58.Limits()):
    p,_=pool(f,extra)
    return tool.scan(p,limits)


def refused(fn,text=None):
    try:fn()
    except ValueError as error:
        if text:assert text in str(error),(text,str(error))
    else:raise AssertionError('unmatched later-edge evidence accepted')


def refs(r,operation=None):
    return [x for x in r['reference_candidates'] if operation is None or x['operation']==operation]


f=functions(following=bytes.fromhex('488b08c3'))
p,raw=pool(f);r=tool.scan(p)
assert r['c59_boundary_instances_considered']==r['c59_boundary_instances_admitted']==2
assert r['c59_exact_edges_admitted']==2 and r['c59_certified_crossings_with_facts']==1
assert len(refs(r,'READ'))==1 and refs(r)[0]['effective_rva']==cfg.SLOT
ref=refs(r)[0]
assert len(ref['boundary_path'])==2 and ref['boundary_path'][1]['proof_kind']=='c59_later_boundary'
assert ref['c59_later_record_identities']==[ref['boundary_path'][1]['c59_record_identity']]
assert ref['address_proof_chain']==((0x1000,'rip_lea',cfg.SLOT),)
assert p.baseline.result['new_cross_row_candidate_count']==0
print('PASS: c60 authenticated later ordinary exact fallthrough carries a proven fact with separate first/c59 boundary and unchanged Fact-chain provenance')

assert r['c59_boundary_instances_admitted']>r['c59_certified_crossings_with_facts']
assert refs(r)[0]['original_root_runtime_row']==0 and r['candidate_sweep_complete'] is False
assert r['historical_c58_candidate_counts']['reference_candidate_count']==0
print('PASS: c60 EMPTY entry seed does not imply empty instruction state; factless admitted boundary is separate from useful crossing and independent roots')

for kind,tail,targets in [
    ('direct_unconditional_branch',jump(0x1107,0x1300),{0x1300:bytes.fromhex('488908c3')}),
    ('direct_conditional_taken',jump(0x1107,0x1300,True)+b'\xc3',{0x1300:bytes.fromhex('488908c3')}),
    ('conditional_fallthrough',jump(0x1107,0x1100,True),{0x110d:bytes.fromhex('488908c3')})]:
    ff=functions(tail);ff.update(targets);rr=scan(ff)
    assert len(refs(rr,'WRITE'))==1 and refs(rr)[0]['boundary_path'][-1]['edge_kind']==kind
    assert rr['c59_boundary_instances_admitted']==2
print('PASS: c60 later conditional taken/fallthrough and nonadjacent direct branch carry facts only under exact current-instance c59 proof')

# All external arms must pass, even if the first is an exact target.
ff=functions(jump(0x1107,0x1300,True),following=bytes.fromhex('488b08c3'));ff[0x1300]=bytes.fromhex('488908c3')
rr=scan(ff)
assert rr['c59_exact_edges_admitted']==4 and rr['c59_boundary_instances_admitted']==2
assert len(refs(rr))==2 and rr['c59_certified_crossings_with_facts']==2
print('PASS: c60 all-exact multi-successor instruction admits two edges per completed boundary instance')

for interior in (True,False):
    target=0x1301 if interior else 0x1300
    ff=functions(jump(0x1107,target,True),following=bytes.fromhex('488b08c3'))
    extra=[]
    if interior:ff[0x1300]=b'\x90\xc3'
    else:extra=[(b'.free',0x1300,b'\xc3',0x60000020,1)]
    rr=scan(ff,extra)
    assert not refs(rr) and rr['c59_exact_edges_considered']==2
    assert rr['c59_exact_edges_admitted']==rr['c59_boundary_instances_admitted']==0
    assert rr['c59_boundary_instances_refused']==2
print('PASS: c60 one exact plus interior/free-RX successor rejects the whole boundary/root without partial candidates')

# CALL has RAX/RBX/RSP seed facts. Only never-written RBX survives callee.
ff=functions(b'\xe8'+struct.pack('<i',0x1200-0x110c),following=bytes.fromhex('488b0b488b08488b0c24c3'))
ff[0x1000]=lea(0x1000,reg=0)+lea(0x1007,reg=3)+lea(0x100e,reg=4)+jump(0x1015,0x1100)
ff[0x1200]=bytes.fromhex('31c0c3')
rr=scan(ff)
assert len(refs(rr,'READ'))==1 and refs(rr)[0]['site_rva']==0x110c
assert rr['call_edges_with_preserved_facts_crossing_rows']==1 and rr['c59_certified_crossings_with_facts']==1
assert refs(rr)[0]['boundary_path'][-1]['edge_kind']=='call_fallthrough'
print('PASS: c60 CALL later fallthrough retains only c54/c55-proven banks and kills RAX/RSP without ABI or return-value facts')

for target,extra,relation in [
    (0x1300,[(b'.free',0x1300,b'\xc3',0x60000020,1)],'executable_file_backed_outside_runtime_metadata'),
    (0x1301,[(b'.unbacked',0x1300,b'\x90',0x60000020,20)],'executable_not_fully_file_backed'),
    (0x1300,[(b'.rw',0x1300,b'\xc3',0xe0000020,1)],'non_executable_or_writable_section'),
    (0x1300,[(b'.data2',0x1300,b'\xc3',0x40000040,1)],'non_executable_or_writable_section'),
    (cfg.DEFERRED,[],'deferred_producer_excluded')]:
    rr=scan(functions(jump(0x1107,target)),extra)
    assert not refs(rr) and rr['root_refusal_reasons'][relation]==2 and rr['c59_boundary_instances_admitted']==0
print('PASS: c60 metadata-free/unbacked/writable/nonexec/deferred later targets refuse with no value read or producer traversal')

ff=functions(b'\xf2'+jump(0x1108,0x1300),following=bytes.fromhex('488b08c3'));ff[0x1300]=bytes.fromhex('488908c3')
rr=scan(ff)
assert not refs(rr) and rr['root_refusal_reasons']['model_only_successor']==2
assert rr['c59_exact_edges_admitted']==0
refused(lambda:pool({0x1100:b'\x90'*8,0x1104:b'\xc3'}))
ff=functions(jump(0x1107,0xD000010));rr=scan(ff)
assert not refs(rr) and rr['c59_boundary_instances_admitted']==0
print('PASS: c60 model-only arm aborts all successors; overlapping/outside sources remain upstream PE/oracle refusals')

# A target reached through c59 has another boundary, even with a c57 row proof.
ff=functions(following=bytes.fromhex('488b08')+jump(0x110b,0x1300));ff[0x1300]=b'\xc3'
rr=scan(ff)
assert not refs(rr) and rr['post_c59_uncertified_boundary_refusals']
assert rr['c59_boundary_instances_admitted']==1  # independent EMPTY source completes without crossing
print('PASS: c60 post-c59 unknown boundary refuses, including a c57 row proof that cannot authorize the new c59 predecessor path')

# Reconstruct original current context for direct lookup rejection tests.
def active_view(pool):
    e=pool.baseline.evidence
    observer=c59.ObserverView(e)
    old=c59.ObservedTransaction(observer,0,c58.c55.Engine(e.session.domain,e.session.profile.summary),c58.Limits())
    refused(old.run,c59.LATER)
    view=tool.View(pool);tx=tool.Transaction(view,0,c58.c55.Engine(e.session.domain,e.session.profile.summary),c58.Limits())
    tx.contexts=deepcopy(old.contexts);tx.forward=deepcopy(old.forward);tx.reverse=deepcopy(old.reverse)
    key=(observer.capture.row,observer.capture.state_identity)
    view.tx,view.key=tx,key
    ins=next(cfg.old.decoder().disasm(e.session.read(observer.capture.pc,observer.capture.size),observer.capture.pc,count=1))
    edge=c59.classify_edges(e.session,*e.session.rows[key[0]][:2],ins)[0]
    return view,tx,key,ins,edge

for mutation in ('root','state','path','source'):
    view,tx,key,ins,edge=active_view(p)
    if mutation=='root':tx.root=99
    if mutation=='state':
        state,path=tx.contexts[key];state=list(state);state[0]=cfg.Fact(cfg.SLOT+8,state[0].chain)
        tx.contexts[key]=(tuple(state),path)
    if mutation=='path':
        state,path=tx.contexts[key];path=deepcopy(path);path[0]['edge_identity']='wrong'
        tx.contexts[key]=(state,path)
    if mutation=='source':ins=next(cfg.old.decoder().disasm(jump(ins.address,0x1300),ins.address,count=1))
    refused(lambda:view.edge(key[0],ins,edge['edge_kind'],edge['edge_target_rva'],True,Counter()),c59.LATER)
print('PASS: c60 exact root/state/path/instruction lookup prevents row-global or source-site-global certificate reuse')

# At the same source, only the original authenticated path can use its record.
view,tx,key,ins,edge=active_view(p)
proof=view.edge(key[0],ins,edge['edge_kind'],edge['edge_target_rva'],True,Counter())
assert proof['c59_record_identity'] and proof['source_boundary_path_identity']==c56.digest(tx.contexts[key][1])
state,path=tx.contexts[key];other=deepcopy(path);other[0]['edge_identity']='another_path'
refused(lambda:tx.instance(key[0],state,other),'path_conflict')
print('PASS: c60 a later same-state different-path arrival invalidates a previously used c59 certificate and entire root')

# Same-state dedup at an ancestor taints descendants, not just the join itself.
view,tx,key,ins,edge=active_view(p)
ancestor=next(k for k in tx.contexts if k!=key)
state,path=tx.contexts[ancestor];other=path+({'edge_identity':'alternate'},)
tx.instance(ancestor[0],state,other)
assert key in tx.path_tainted
refused(lambda:view.edge(key[0],ins,edge['edge_kind'],edge['edge_target_rva'],True,Counter()),'path_conflict')
print('PASS: c60 alternate-path taint propagates to descendants so canonical first-path storage cannot lend a certificate indirectly')

# Identical state AND identical path still use original deterministic dedup.
view,tx,key,_,_=active_view(p);state,path=tx.contexts[key];before=tx.deduplicated
assert tx.instance(key[0],state,path)==key and tx.deduplicated==before+1 and key not in tx.path_tainted
assert tool.scan(p)==tool.scan(p)
print('PASS: c60 identical authenticated state/path deduplicates deterministically without fabricating a merged fact')

ff=functions(following=bytes.fromhex('488b08c3'),roots={0x1080:lea(0x1080,cfg.SLOT+8)+jump(0x1087,0x1100)})
rr=scan(ff)
assert len(refs(rr,'READ'))==1 and refs(rr)[0]['original_root_runtime_row']==0
assert rr['c59_boundary_instances_admitted']==3
print('PASS: c60 two different incoming states remain distinct, and independent target EMPTY-root acquires no predecessor fact')

# Stable exact-state c57-only cycle still dedups; changing cycle refuses.
ff={0x1000:lea(0x1000)+jump(0x1007,0x1100),0x1100:bytes.fromhex('488b08')+jump(0x1103,0x1200),0x1200:jump(0x1200,0x1100)}
rr=scan(ff)
assert refs(rr) and rr['cross_row_cycles_seen'] and rr['continuation_instances_deduplicated']
ff[0x1200]=bytes.fromhex('488d4008')+jump(0x1204,0x1100)
rr=scan(ff)
assert not refs(rr) and rr['root_refusal_reasons']['state_changing_cross_row_cycle']
print('PASS: c60 original exact-state c57 cycles deduplicate and state-changing cycles discard all provisional candidates')

for name in asdict(c58.Limits()):
    if name in ('retained_references','root_references'):continue
    rr=tool.scan(p,replace(c58.Limits(),**{name:1}))
    assert not refs(rr) and rr['cross_row_cap_refusals'],name
ff=functions(following=bytes.fromhex('488b08488908c3'));pp,_=pool(ff)
rr=tool.scan(pp,replace(c58.Limits(),root_references=1))
assert not refs(rr) and rr['cross_row_cap_refusals']
rr=tool.scan(pp,replace(c58.Limits(),retained_references=1))
assert rr['reference_candidate_count']==2 and len(refs(rr))==1 and rr['candidate_records_not_retained']==1
print('PASS: c60 all original work/instance/path/reference caps remain finite; cap failure leaks no candidate and retained records never truncate totals')

for body in [bytes.fromhex('488b00488b08c3'),bytes.fromhex('89c0488b08c3'),bytes.fromhex('488b0c08c3')]:
    rr=scan(functions(following=body))
    assert all(x['site_rva']==0x1108 for x in refs(rr))
    assert len(refs(rr))==(1 if body==bytes.fromhex('488b00488b08c3') else 0)
rr=scan(functions(following=lea(0x1108)+bytes.fromhex('488b08c3')))
assert not refs(rr)  # independent new RIP origin does not depend on incoming chain
print('PASS: c60 loaded/partial/indexed values and new independent RIP origins never become carried-pointer candidates')

# The local branch join before a later source must agree exactly.
# Existing c53/c58 transfer/must code is unchanged; a clobber before entry to
# the later target kills the incoming fact even when the c59 seed was nonempty.
ff=functions(bytes.fromhex('31c0')+b'\x90',following=bytes.fromhex('488b08c3'))
rr=scan(ff)
assert not refs(rr) and rr['c59_certified_crossings_with_facts']==0
print('PASS: c60 c59 entry-seed fact is not treated as a source-instruction fact after local clobber/fixpoint')

# Current immutable target code preflight is independent from metadata point
# classification: an exact row begin alone cannot certify RX backing.
def change_target(raw,field,value):
    data=bytearray(raw);count=struct.unpack_from('<H',data,0x86)[0]
    for i in range(count):
        at=0x188+40*i
        if struct.unpack_from('<I',data,at+12)[0]==0x1300:
            struct.pack_into('<I',data,at+field,value)
            return bytes(data)
    raise AssertionError('target section missing')
ff=functions(jump(0x1107,0x1300));ff[0x1300]=bytes.fromhex('488b08c3')
for field,value in [(36,0xc0000040),(16,0)]:
    pp,_=pool(ff,mutate=lambda raw:change_target(raw,field,value))
    rr=tool.scan(pp)
    assert not refs(rr) and rr['c59_exact_edges_considered']==2 and rr['c59_exact_edges_admitted']==0
print('PASS: c60 exact metadata beginning still refuses non-RX or unbacked target code under current independent preflight')

for arm,expected in [(bytes.fromhex('31c0'),0),(bytes.fromhex('9090'),1)]:
    body=bytes.fromhex('7504')+arm+bytes.fromhex('eb02')+b'\x90\x90'+bytes.fromhex('488b08c3')
    rr=scan(functions(following=body))
    assert len(refs(rr,'READ'))==expected
print('PASS: c60 target-row must join preserves only exact all-arm agreement after the c59 boundary')

# The first c57 arm and c59 arm reach the same row/state by different paths.
# Keeping the first path in a dedup context must not hide the c59 arrival.
ff=functions(jump(0x1107,0x1200));ff[0x1200]=bytes.fromhex('488b08')+jump(0x1203,0x1300);ff[0x1300]=b'\xc3'
rr=scan(ff)
assert not refs(rr) and rr['post_c59_uncertified_boundary_refusals']
print('PASS: c60 same-state dedup retains post-c59 reachability and cannot borrow the earlier stored c57 path certificate')

for key in ('schema_version','contract','accepted_c57_boundary_identity','accepted_c58_result_identity',
    'accepted_c58_proof_identity','accepted_c59_classifier_identity','accepted_c59_result_identity','accepted_c59_record_set_identity','limits'):
    old=p.header[key];p.header[key]='wrong'
    refused(lambda:tool.scan(p),'identity');p.header[key]=old
print('PASS: c60 stale/wrong c56-c60 proof/schema/contract/caps/result identities fail globally')

old=p.records
for field in ('original_root_runtime_row','incoming_state_identity','boundary_path_identity','source_code_sha256','source_instruction_sha256','classifier_identity'):
    record=json.loads(old[0]);record[field]='wrong';p.records=(c56.encoded(record),)+old[1:]
    refused(lambda:tool.scan(p),'identity')
p.records=old
index_key=next(iter(p.index));old_index=p.index[index_key];p.index[index_key]=999
refused(lambda:tool.scan(p),'identity');p.index[index_key]=old_index
print('PASS: c60 mutated c59 record stream/index/source bytes cannot enter the proof pool or become a weaker fallback')

for field in ('schema_version','contract','classifier_identity','authenticated_record_set_sha256','later_boundary_edges_total'):
    wrong=deepcopy(p.result);wrong[field]='wrong'
    refused(lambda:tool.ProofPool(p.baseline,accepted=p.result,owner_result=wrong),'c59 JSON')
refused(lambda:tool.ProofPool(p.baseline),'accepted c59')
refused(lambda:tool.analyze(raw),'SHA256')
print('PASS: c60 full compact owner c59 JSON and accepted fixed private-image distribution/identities are required; no CLI override')

with tempfile.TemporaryDirectory() as directory:
    root=Path(directory);cache=root/'cache';cache.mkdir();s=p.baseline.evidence.session
    c56.atomic_json(cache/'manifest.json',s.manifest)
    for i,(start,end) in enumerate(s.plan):c56.atomic_json(cache/f'shard-{start:06d}-{end:06d}.json',s.make_shard(i))
    for name,data in [('c57',p.baseline.evidence.result),('c58',p.baseline.result),('c59',p.result)]:c56.atomic_json(root/(name+'.json'),data)
    before={x.relative_to(root):x.read_bytes() for x in root.rglob('*.json')}
    prior=c57.collect_baseline(s,cache,acceptance=c57.Acceptance(len(s.rows),len(p.baseline.evidence.proofs)))
    previous=p.baseline.evidence
    relations=previous.result['target_metadata_relation_counts']
    accepted=c58.AcceptedEdges(previous.result['boundary_edges_total'],relations['exact_unique_runtime_begin'],
        relations['inside_unique_runtime_span_nonentry'],relations['executable_file_backed_outside_runtime_metadata'],
        previous.result['model_only_boundary_edges_total'],sum(previous.result['exact_adjacency_edge_kind_counts'].values()))
    e=c58.Evidence(prior,accepted=accepted,owner_result=c56.read_json(root/'c57.json'))
    b=c59.Baseline(e,accepted=p.baseline.result,owner_result=c56.read_json(root/'c58.json'))
    again=tool.ProofPool(b,accepted=p.result,owner_result=c56.read_json(root/'c59.json'))
    assert tool.scan(again)==tool.scan(p) and before=={x.relative_to(root):x.read_bytes() for x in root.rglob('*.json')}
    (root/'c59.json').write_text('{}')
    refused(lambda:tool.ProofPool(b,accepted=p.result,owner_result=c56.read_json(root/'c59.json')),'c59 JSON')
print('PASS: c60 c56 checkpoint+c57+c58+c59 inputs remain byte-identical; stale supplied evidence refuses without repair/fallback')

with tempfile.TemporaryDirectory() as directory:
    root=Path(directory);exe=root/'owned.exe';exe.write_bytes(raw)
    for flag,path in [(None,exe),('--c57-json',root/'c57.json'),('--c58-json',root/'c58.json'),('--c59-json',root/'c59.json')]:
        if flag:path.write_text('{}')
        args=[str(exe),'--output',str(path)]+([flag,str(path)] if flag else [])
        before=path.read_bytes();proc=subprocess.run([sys.executable,tool.__file__,*args],capture_output=True,text=True)
        assert proc.returncode==2 and 'immutable input' in proc.stderr and before==path.read_bytes()
print('PASS: c60 CLI refuses output aliases of EXE and every supplied owner JSON')

assert all(r[k] is False for k in c56.FALSE_GATES) and r['live_qword_reads']==0
assert r['c52_production_watch_gate'].endswith('_FAIL') and r['publication_source_kind']=='unproven'
assert r['c59_boundary_instances_considered']==r['c59_boundary_instances_admitted']+r['c59_boundary_instances_refused']
assert r['c59_exact_edges_considered']==r['c59_exact_edges_admitted']+r['c59_exact_edges_refused']
print('PASS: c60 count scopes partition exact edges separately from boundary instances; all completeness/production/ownership/lifetime gates remain false')

proc=subprocess.run([sys.executable,tool.__file__,'--help'],capture_output=True,text=True)
assert proc.returncode==0 and '--c59-json' in proc.stdout
proc=subprocess.run([sys.executable,tool.__file__,'missing.exe','--output','out.json'],capture_output=True,text=True)
assert proc.returncode==2 and tool.BLOCKED in proc.stderr
print('PASS: c60 CLI documents the read-only chain and missing private image returns BLOCKED without fabricated metrics')
