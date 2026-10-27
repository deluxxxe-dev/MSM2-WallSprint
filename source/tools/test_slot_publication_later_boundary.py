"""Owned c59 refusal-observer/authentication/aggregation tests; no private EXE."""
from contextlib import contextmanager
from copy import deepcopy
from dataclasses import replace
from pathlib import Path
from types import SimpleNamespace
import struct
import subprocess
import sys
import tempfile

import slot_publication_later_boundary as tool
c58,c57,c56,cfg=tool.c58,tool.c57,tool.c56,tool.cfg

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
        a=c58.AcceptedEdges(result['boundary_edges_total'],relations['exact_unique_runtime_begin'],
            relations['inside_unique_runtime_span_nonentry'],relations['executable_file_backed_outside_runtime_metadata'],
            result['model_only_boundary_edges_total'],sum(result['exact_adjacency_edge_kind_counts'].values()))
        return c58.Evidence(baseline,accepted=a,owner_result=result),raw,baseline,a


def baseline(functions,extra=()):
    e,raw,_,_=evidence(functions,extra)
    original=c58.scan(e)
    return tool.Baseline(e,accepted=original,owner_result=original),raw


def functions(tail=b'\x90',following=None,roots=None):
    # Heap first boundary at1102 is c57-certified. Reachable1107 is not.
    result={0x1000:lea(0x1000)+jump(0x1007,0x1100),
        0x1100:bytes.fromhex('7505')+jump(0x1102,0x1200)+tail,0x1200:b'\xc3'}
    if following is not None:result[0x1107+len(tail)]=following
    if roots:result.update(roots)
    return result


def records(b):return list(tool.replay_records(b))


def aggregate(b,values=None,limit=8):
    a=tool.Aggregate(b,limit)
    for r in records(b) if values is None else values:a.add(r)
    return a.finish()


def refused(fn,text=None):
    try:fn()
    except ValueError as e:
        if text:assert text in str(e),(text,str(e))
    else:raise AssertionError('invalid later-boundary proof accepted')


b,raw=baseline(functions(following=b'\xc3'))
values=records(b);r=aggregate(b,values)
assert len(values)==2 and r['later_boundary_refused_roots_classified']==2 and r['later_boundary_instances_total']==2
assert r['edge_kind_counts']['ordinary_fallthrough']==2 and r['target_metadata_relation_counts']['exact_unique_runtime_begin']==2
assert sum(r['exact_adjacency_counts'].values())==2 and r['new_later_boundaries_admitted']==0
assert r['later_boundary_instances_with_empty_entry_seed']==1 and r['later_boundary_instances_in_original_root']==1
assert {v['incoming_state_fact_count'] for v in values}=={0,1}
print('PASS: c59 later ordinary exact-adjacent fallthrough classified without admission; original EMPTY and carried contexts distinguished')

for kind,tail,extra in [
    ('direct_unconditional_branch',jump(0x1107,0x1300),{0x1300:b'\xc3'}),
    ('direct_conditional_taken',jump(0x1107,0x1300,True)+b'\xc3',{0x1300:b'\xc3'}),
    ('conditional_fallthrough',jump(0x1107,0x1100,True),{0x110d:b'\xc3'})]:
    f=functions(tail);f.update(extra);bb,_=baseline(f);rr=aggregate(bb)
    assert rr['edge_kind_counts'][kind]==2 and rr['target_metadata_relation_counts']['exact_unique_runtime_begin']==2
    assert sum(rr['exact_adjacency_counts'].values())==(2 if kind=='conditional_fallthrough' else 0)
print('PASS: c59 later conditional taken/fallthrough and nonadjacent unconditional edges use exact c57 semantics')

f=functions(jump(0x1107,0x1300,True));f.update({0x110d:b'\xc3',0x1300:b'\xc3'})
bb,_=baseline(f);rr=aggregate(bb)
assert rr['later_boundary_refused_roots_classified']==2 and rr['later_boundary_edges_total']==4
assert rr['edge_kind_counts']['direct_conditional_taken']==rr['edge_kind_counts']['conditional_fallthrough']==2
print('PASS: c59 root/instance counts stay separate from two outgoing edge counts at one conditional refusal')

f=functions(b'\xe8'+struct.pack('<i',0x1200-0x110c),following=b'\xc3')
bb,_=baseline(f);rr=aggregate(bb)
assert rr['edge_kind_counts']['call_fallthrough']==2
assert all(e['edge_target_rva']==0x110c for e in rr['examples_by_edge_kind']['call_fallthrough'])
assert bb.result['call_edges_with_preserved_facts_crossing_rows']==0 and rr['new_later_boundaries_admitted']==0
print('PASS: c59 later CALL reports return-site fallthrough, never callee edge, summary, preserved post-CALL fact or return value')

f=functions(jump(0x1107,0x1301));f[0x1300]=b'\x90\xc3'
bb,_=baseline(f);rr=aggregate(bb)
assert rr['target_metadata_relation_counts']['inside_unique_runtime_span_nonentry']==2
print('PASS: c59 exact runtime interior target is diagnostic only without nearest-entry repair')

bb,_=baseline(functions(jump(0x1107,0x1300)),[(b'.free',0x1300,b'\xc3',0x60000020,1)])
assert aggregate(bb)['target_metadata_relation_counts']['executable_file_backed_outside_runtime_metadata']==2
print('PASS: c59 later metadata-free RX target is classified without inventing a span or reading target code')

for target,extra,relation in [
    (0x1301,[(b'.unbacked',0x1300,b'\x90',0x60000020,20)],'executable_not_fully_file_backed'),
    (0x1300,[(b'.rw',0x1300,b'\xc3',0xe0000020,1)],'non_executable_or_writable_section'),
    (0x1300,[(b'.data2',0x1300,b'\xc3',0x40000040,1)],'non_executable_or_writable_section'),
    (cfg.DEFERRED,[],'deferred_producer_excluded')]:
    bb,_=baseline(functions(jump(0x1107,target)),extra)
    assert aggregate(bb)['target_metadata_relation_counts'][relation]==2
print('PASS: c59 unbacked/writable/nonexec/deferred later targets remain refused while metadata classes are counted')

# These target classes are refused by upstream PE/oracle admission before a
# c58 later-boundary event. Exercise the shared diagnostic relation explicitly;
# do not fabricate a replay record or claim an unreachable owned source exists.
s=b.evidence.session
ins=next(cfg.old.decoder().disasm(jump(0x1107,s.size+1),0x1107,count=1))
assert tool.classify_edges(s,0x1100,0x110c,ins)[0]['target_metadata_relation']=='outside_image'
domain=c58.c55.Domain(image_id='owned',rows=[(0x1300,0x1310,0),(0x1304,0x1320,0)],
    sections=[],image_size=s.size,excluded=[],read=lambda *_: (_ for _ in ()).throw(AssertionError('no target read')))
ins=next(cfg.old.decoder().disasm(jump(0x1107,0x1305),0x1107,count=1))
assert tool.classify_edges(SimpleNamespace(domain=domain),0x1100,0x110c,ins)[0]['target_metadata_relation']=='overlapping_multiple_runtime_spans'
refused(lambda:baseline({0x1100:b'\x90'*8,0x1104:b'\xc3'}))
print('PASS: c59 outside/overlap relation helper tested separately; original PE/oracle refusals are not bypassed to invent records')

# Both outgoing edges occur after the accepted first boundary. Original c58
# aborts on the architectural target before inspecting the model-only arm.
f=functions(b'\xf2'+jump(0x1108,0x1300));f.update({0x110d:b'\xc3',0x1300:b'\xc3'})
bb,_=baseline(f);rr=aggregate(bb)
assert rr['model_only_later_boundary_edges_total']==2 and rr['architectural_later_boundary_edges_total']==2
assert rr['edge_kind_counts']['other_supported_direct_successor']==2
assert rr['new_later_boundaries_admitted']==0
print('PASS: c59 decorated JMP model-only successor is explicit and counted separately from architectural edge')

f=functions(following=b'\xc3',roots={0x1080:lea(0x1080,cfg.SLOT+8)+jump(0x1087,0x1100)})
bb,_=baseline(f);v=records(bb)
carried=[x for x in v if x['incoming_state_fact_count']]
assert len(carried)==2 and len({x['incoming_state_identity'] for x in carried})==2
assert len({x['record_identity'] for x in v})==len(v) and len({x['current_entry_instance_row'] for x in v})==1
assert aggregate(bb)['unique_entry_instance_states']==3
print('PASS: c59 different incoming states at identical row/instruction remain separate authenticated instances')

# Same root/state reaches A by two paths; original c58 keeps first path and dedups.
f=functions(following=b'\xc3');f[0x1000]=lea(0x1000)+jump(0x1007,0x1050,True)
f[0x100d]=jump(0x100d,0x1100);f[0x1050]=jump(0x1050,0x1100)
bb,_=baseline(f)
assert bb.result['continuation_instances_deduplicated']>=1
v=records(bb);assert len(v)==2 and v==records(bb)
view=tool.ObserverView(bb.evidence)
c=tool.Capture(1,2,'state','path',(),1,0x1000,1,'ins','row','body')
view.remember(c);view.remember(c)
assert view.capture==c and view.identical_capture_duplicates==1
refused(lambda:view.remember(replace(c,state_identity='different')),'different events')
print('PASS: c59 identical authenticated c58 contexts/events deduplicate deterministically; different events cannot collapse')

for field,new in [('incoming_state_identity','0'*64),('boundary_path_identity','0'*64),
    ('boundary_transition_identities',['wrong']),('original_root_runtime_row',999),
    ('entry_instance_identity','0'*64),('current_entry_instance_row',999),
    ('accepted_c58_proof_identity','0'*64),('accepted_c58_result_identity','0'*64)]:
    wrong=deepcopy(values[0]);wrong[field]=new;a=tool.Aggregate(b)
    refused(lambda:a.add(wrong),'record');assert a.poisoned
    refused(a.finish,'poisoned')
print('PASS: c59 wrong root/state/path/instance/c58 proof or result poisons aggregation')

for field in ['source_code_sha256','source_instruction_sha256','source_runtime_row_sha256','root_replay_identity','classifier_identity']:
    wrong=deepcopy(values[0]);wrong[field]='0'*64
    refused(lambda:tool.Aggregate(b).add(wrong),'record')
wrong=deepcopy(values[0]);wrong['edges'][0]['edge_target_rva']+=1
refused(lambda:tool.Aggregate(b).add(wrong),'record')
print('PASS: c59 stale source instruction/body/row/replay/tool identity and mismatched target bytes refuse')

for variant in (values[1:],values[:-1],values[::-1],values+values[:1]):
    a=tool.Aggregate(b)
    refused(lambda:([a.add(v) for v in variant],a.finish()))
    assert a.poisoned
print('PASS: c59 missing/duplicate/out-of-order/root-instance record sets poison aggregate instead of partial completeness')

for limit in (0,1,8):
    rr=aggregate(b,limit=limit)
    assert rr['later_boundary_edges_total']==r['later_boundary_edges_total']
    assert rr['authenticated_record_set_sha256']==r['authenticated_record_set_sha256']
    assert all(len(examples)<=limit for examples in rr['examples_by_edge_kind'].values())
    assert all(len(examples)<=limit for examples in rr['examples_by_target_relation'].values())
refused(lambda:tool.Aggregate(b,9),'bound')
print('PASS: c59 bounded examples0/1/8 never truncate exact totals or authenticated record-set identity')

for key in ('schema_version','contract','tool_version','image_sha256','limits','authentication_header'):
    wrong=deepcopy(b.result);wrong[key]='wrong'
    refused(lambda:tool.Baseline(b.evidence,owner_result=wrong,accepted=b.result),'c58 JSON')
refused(lambda:tool.Baseline(b.evidence),'accepted c58')
refused(lambda:tool.analyze(raw),'SHA256')
print('PASS: c59 exact original c58 JSON and accepted private distribution/image guards have no CLI bypass')

for field in ('schema_version','contract','c56_manifest_id','accepted_c57_boundary_identity','accepted_c58_proof_identity'):
    old=b.header[field];b.header[field]='wrong'
    refused(lambda:aggregate(b),'identity');b.header[field]=old
old=b.result['limits'];b.result['limits']={}
refused(lambda:aggregate(b),'identity');b.result['limits']=old
print('PASS: c59 stale c56/c57/c58/c59 schema/contract/tool/proof/caps identity refuses before publishing')

# An independent positive c58 candidate is retained unchanged; failed roots
# may have read/store-looking instructions but never emit provisional candidates.
f=functions(following=b'\xc3',roots={0x1400:lea(0x1400)+jump(0x1407,0x1500),0x1500:bytes.fromhex('488b08c3')})
bb,_=baseline(f);before=deepcopy(bb.result);rr=aggregate(bb)
assert before==bb.result and before==c58.scan(bb.evidence)
assert before['reference_candidate_count']==1 and rr['new_cross_row_candidate_count']==0
assert rr['historical_c58_candidate_counts']['reference_candidate_count']==1
assert rr['c58_candidate_output_sha256']==c56.digest(before['reference_candidates'])
assert rr['c58_candidate_output_unchanged'] and rr['cross_runtime_fact_propagation_unchanged']
print('PASS: c59 retains positive historical c58 candidate bytes/counts without emitting any later-boundary candidate')

# At most one first terminal event per original c58 root, even if the root has
# pending other continuation instances. Diagnostic code never resumes them.
f=functions(following=b'\xc3');f[0x1000]=lea(0x1000)+jump(0x1007,0x1050,True)
f[0x100d]=jump(0x100d,0x1100);f[0x1050]=bytes.fromhex('488d4008')+jump(0x1054,0x1100)
bb,_=baseline(f);v=records(bb)
assert len([x for x in v if x['original_root_runtime_row']==0])==1
assert all(x['instruction_state_scope'].endswith('before_local_must_fixpoint') for x in v)
print('PASS: c59 observes first c58 abort only, keeps seed state distinct from unavailable instruction fixpoint, never explores after failure')

with tempfile.TemporaryDirectory() as directory:
    p=Path(directory);exe=p/'owned.exe';exe.write_bytes(raw)
    for input_flag,input_path in [(None,exe),('--c57-json',p/'c57.json'),('--c58-json',p/'c58.json')]:
        if input_flag:input_path.write_text('{}')
        args=[str(exe),'--output',str(input_path)]
        if input_flag:args.extend([input_flag,str(input_path)])
        before=input_path.read_bytes()
        proc=subprocess.run([sys.executable,tool.__file__,*args],capture_output=True,text=True)
        assert proc.returncode==2 and 'immutable input' in proc.stderr and input_path.read_bytes()==before
    cache=p/'cache';cache.mkdir()
    proc=subprocess.run([sys.executable,tool.__file__,str(exe),'--c56-cache',str(cache),'--output',str(cache/'new.json')],capture_output=True,text=True)
    assert proc.returncode==2 and not (cache/'new.json').exists()
print('PASS: c59 CLI cannot overwrite EXE/c57/c58 inputs or any c56 checkpoint path')

with tempfile.TemporaryDirectory() as directory:
    p=Path(directory);cache=p/'cache';cache.mkdir();s=b.evidence.session
    c56.atomic_json(cache/'manifest.json',s.manifest)
    for i,(start,end) in enumerate(s.plan):c56.atomic_json(cache/f'shard-{start:06d}-{end:06d}.json',s.make_shard(i))
    c56.atomic_json(p/'c57.json',b.evidence.result);c56.atomic_json(p/'c58.json',b.result)
    before={x.relative_to(p):x.read_bytes() for x in p.rglob('*.json')}
    c56base=c57.collect_baseline(s,cache,acceptance=c57.Acceptance(len(s.rows),len(b.evidence.proofs)))
    relations=b.evidence.result['target_metadata_relation_counts'];a=c58.AcceptedEdges(
        b.evidence.result['boundary_edges_total'],relations['exact_unique_runtime_begin'],relations['inside_unique_runtime_span_nonentry'],
        relations['executable_file_backed_outside_runtime_metadata'],b.evidence.result['model_only_boundary_edges_total'],
        sum(b.evidence.result['exact_adjacency_edge_kind_counts'].values()))
    e=c58.Evidence(c56base,accepted=a,owner_result=c56.read_json(p/'c57.json'))
    again=tool.Baseline(e,accepted=b.result,owner_result=c56.read_json(p/'c58.json'))
    assert aggregate(again)==aggregate(b)
    assert before=={x.relative_to(p):x.read_bytes() for x in p.rglob('*.json')}
    (cache/'manifest.json').write_text('{}')
    refused(lambda:c57.collect_baseline(s,cache,acceptance=c56base.acceptance),'manifest')
print('PASS: c59 original c56 checkpoints+c57+c58 JSON are read-only, deterministic, and corrupt evidence never falls back')

rr=aggregate(b)
assert all(rr[k] is False for k in c56.FALSE_GATES) and rr['live_qword_reads']==0
assert rr['new_later_boundaries_admitted']==rr['new_cross_row_candidate_count']==0
assert rr['publication_source_kind']=='unproven' and rr['c52_production_watch_gate'].endswith('_FAIL')
print('PASS: c59 all publication/ownership/lifetime/CFG/EH/production/watch/game gates remain false and C52 FAIL binding')

proc=subprocess.run([sys.executable,tool.__file__,'--help'],capture_output=True,text=True)
assert proc.returncode==0 and all(flag in proc.stdout for flag in ['--c56-cache','--c57-json','--c58-json'])
proc=subprocess.run([sys.executable,tool.__file__,'missing.exe','--output','out.json'],capture_output=True,text=True)
assert proc.returncode==2 and tool.BLOCKED in proc.stderr
print('PASS: c59 CLI documents all read-only evidence inputs and missing private image is BLOCKED without fabricated results')
