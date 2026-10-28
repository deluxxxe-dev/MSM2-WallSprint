"""Owned c69 post-c67 boundary/checkpoint tests; no private EXE."""
from contextlib import redirect_stderr, redirect_stdout
from copy import deepcopy
import io
from pathlib import Path
import struct
import tempfile

import slot_publication_post_c67_boundary as tool

with redirect_stdout(io.StringIO()), redirect_stderr(io.StringIO()):
    import test_slot_publication_post_c65_stitched as prior68

prior67 = prior68.prior67
prior = prior68.prior
c68,c67,c65,c58,c56,cfg = tool.c68,tool.c67,tool.c65,tool.c58,tool.c56,tool.cfg


def refused(fn,text=None):
    try:
        fn()
    except ValueError as error:
        if text:
            assert text in str(error),(text,str(error))
    else:
        raise AssertionError('refusal expected')


def fixture69(source=b'\x90',after=b'\xc3',targets=None,roots=None):
    """Admit c67 at 0x110b, then expose c68 POST at source 0x110c."""
    f = prior67.fixture(body=b'\x90',following=source,targets=targets,roots=roots)
    if after is not None:
        f[0x110c+len(source)] = after
    return f


def baseline69(f,extra=()):
    p,raw = prior68.certificates68(f,extra);r = c68.scan(p)
    return tool.Baseline(p,accepted=r,owner_result=r),raw


def records69(b):
    return list(tool.replay_records(b))


def result69(b,records=None,limit=8):
    records = records69(b) if records is None else records
    a = tool.Aggregate(b,records,limit)
    for record in records:
        a.add(record)
    return a.finish()


assert tool.ACCEPTED_C68['post_c67_uncertified_boundary_refusals']==191
print('PASS: c69 owner contract pins the exact 191 post-c67 terminal domain')

b,_=baseline69(fixture69());records=records69(b);assert len(records)==1
record=records[0];assert record['source_instruction_rva']==0x110c
assert record['terminal_reason']=='current_context_post_c67_reachable' and record['terminal_phase']==tool.PHASES[1]
assert record['c67_record_identities'] and record['post_c67_origin']['arrival_path']['c67_record_identities']
state=tuple(None if x is None else cfg.Fact(x['rva'],tuple(tuple(y) for y in x['chain'])) for x in record['checkpoint_incoming_state'])
assert c58.state_id(state)==record['incoming_state_identity']
rr=result69(b,records)
assert rr['repository_result']==tool.READY and rr['classification_complete'] is True
assert rr['post_c67_uncertified_boundary_refused_roots_classified']==1
assert rr['edge_kind_counts']['ordinary_fallthrough']==rr['target_metadata_relation_counts']['exact_unique_runtime_begin']==1
assert rr['new_post_c67_boundaries_admitted']==rr['new_cross_row_candidate_count']==0
print('PASS: c69 exact first post-c67 terminal binds source/state/path/provenance and stays diagnostic-only')

cases=[]
cases.append(('direct_unconditional_branch',prior.jump(0x110c,0x1400),None,{0x1400:b'\xc3'}))
cond=prior.jump(0x110c,0x1400,True);cases.append(('direct_conditional_taken',cond,b'\xc3',{0x1400:b'\xc3'}))
call=b'\xe8'+struct.pack('<i',0x1200-(0x110c+5));cases.append(('call_fallthrough',call,b'\xc3',{}))
for kind,source,after,targets in cases:
    bb,_=baseline69(fixture69(source,after,targets));res=result69(bb)
    assert res['edge_kind_counts'][kind]==1 and res['new_post_c67_boundaries_admitted']==0
    if kind=='direct_conditional_taken':
        assert res['edge_kind_counts']['conditional_fallthrough']==1
    if kind=='call_fallthrough':
        assert res['examples_by_edge_kind'][kind][0]['edge_target_rva']==0x1111
print('PASS: c69 direct/conditional/CALL-fallthrough vocabulary is diagnostic-only')

bb,_=baseline69(fixture69(cond,b'\xc3',{0x1400:b'\xc3'}));res=result69(bb)
assert res['post_c67_boundary_instances_total']==1 and res['post_c67_boundary_edges_total']==2
assert res['architectural_post_c67_boundary_edges_total']==2 and res['model_only_post_c67_boundary_edges_total']==0
print('PASS: c69 multi-successor terminal classifies both arms without traversal')

bb,_=baseline69(fixture69(prior.jump(0x110c,0x1401),None,{0x1400:b'\x90\xc3'}))
assert result69(bb)['target_metadata_relation_counts']['inside_unique_runtime_span_nonentry']==1
bb,_=baseline69(fixture69(prior.jump(0x110c,0x1400),None),[(b'.free',0x1400,b'\xc3',0x60000020,1)])
assert result69(bb)['target_metadata_relation_counts']['executable_file_backed_outside_runtime_metadata']==1
print('PASS: c69 interior and metadata-free RX relations remain diagnostics without promotion')

for field,value in [
    ('original_root_runtime_row',99),('current_entry_instance_row',99),('incoming_state_identity','wrong'),
    ('boundary_path_identity','wrong'),('source_instruction_rva',0),('source_instruction_sha256','wrong'),
    ('root_replay_identity','wrong'),('current_c68_continuation_identity','wrong'),('terminal_phase',tool.PHASES[0]),
    ('terminal_reason','wrong'),('post_c67_origin',None),('accepted_c68_authentication_identity','wrong'),
    ('accepted_c68_result_identity','wrong')]:
    bad=deepcopy(record);bad[field]=value;a=tool.Aggregate(b,[record])
    refused(lambda:a.add(bad));refused(a.finish,'poisoned')
for mutate in ('terminal_provenance','c67_authorization','checkpoint_incoming_state'):
    bad=deepcopy(record)
    if mutate=='terminal_provenance':
        bad[mutate]['underlying_refusal']='wrong'
    elif mutate=='c67_authorization':
        bad[mutate]['terminal_reason']='wrong'
    else:
        bad[mutate]=[None]*16
    a=tool.Aggregate(b,[record]);refused(lambda:a.add(bad));refused(a.finish,'poisoned')
print('PASS: c69 root/state/path/source/c68-prefix/phase/reason/origin/c67 provenance cannot be borrowed')

f=prior.local_functions();f[0x1100]=f[0x1100][:-5]+prior.jump(0x1115,0x1200);f[0x1200]=prior.jump(0x1200,0x1700)
f[0x1600]=b'\xc3';f[0x1700]=b'\x90';f[0x1701]=b'\x90';f[0x1702]=b'\xc3'
stored,_=baseline69(f);sv=records69(stored)
s=next(x for x in sv if x['c67_authorization']['c65_authorization']['terminal_phase']==c65.PHASES[2])
assert s['terminal_phase']==tool.PHASES[1] and s['post_c67_origin'] is not None
assert s['c67_authorization']['c65_authorization']['terminal_phase']=='post_c63_reachable_dedup_stored_earlier_path'
print('PASS: c69 keeps stored-path nested provenance separate from current post-c67 arrival')

f=prior.converted65();f[0x1700]=b'\x90';f[0x1701]=b'\x90';f[0x1702]=b'\xc3'
converted,_=baseline69(f);cv=records69(converted)
c=next(x for x in cv if x['c67_authorization']['c65_authorization']['terminal_phase']==c65.PHASES[3])
assert c['terminal_phase']==tool.PHASES[1] and c['post_c67_origin'] is not None
assert c['c67_authorization']['c65_authorization']['terminal_reason'] in (
    'prior_c63_authorization_converted_c62_post','prior_c63_authorization_converted_pre_c59_later')
print('PASS: c69 preserves converted historical authorization without relabeling it')

bad=deepcopy(record);bad['terminal_reason']='wrong'
a=tool.Aggregate(b,[record]);refused(lambda:a.add(bad),'wrong c69');refused(a.finish,'poisoned')
a=tool.Aggregate(b,[record]);a.add(record);refused(lambda:a.add(record));refused(a.finish,'poisoned')
refused(lambda:tool.Aggregate(b,[record]).finish(),'missing')
roots={0x1080:prior.lea(0x1080,cfg.SLOT+8)+prior.jump(0x1087,0x1100)}
bb,_=baseline69(fixture69(roots=roots));two=records69(bb);assert len(two)==2
a=tool.Aggregate(bb,two);refused(lambda:a.add(two[1]),'out-of-order');refused(a.finish,'poisoned')
print('PASS: c69 record stream rejects stale, duplicate, missing and out-of-order evidence')

p,_=prior68.certificates68(fixture69());good=c68.scan(p);bad=deepcopy(good);bad['post_c67_uncertified_boundary_refusals']=2
refused(lambda:tool.Baseline(p,accepted=good,owner_result=bad),'accepted c68')
with tempfile.TemporaryDirectory(prefix='ws-c69-') as directory:
    wrong=Path(directory)/'c68.json';wrong.write_text('{}')
    refused(lambda:tool.read_c68(wrong),'physical')
print('PASS: c69 rejects stale semantic c68 accounting and wrong physical c68 owner evidence')

result,records=tool.classify(b);cp1=tool.make_checkpoint(b,result,records);cp2=tool.make_checkpoint(b,result,records)
assert c56.encoded(cp1)==c56.encoded(cp2)
assert tool.validate_checkpoint(deepcopy(cp1))['result']==result
for which in ('image','artifact','tool','schema','limits','record','identity'):
    bad=deepcopy(cp1)
    if which=='image':bad['header']['image_sha256']='wrong'
    elif which=='artifact':bad['header']['accepted_c68_physical_sha256']='wrong'
    elif which=='tool':bad['header']['tool_sources']={'wrong':'wrong'}
    elif which=='schema':bad['result']['schema_version']=99
    elif which=='limits':bad['header']['limits']['instances']+=1
    elif which=='record':bad['records'][0]['terminal_reason']='wrong'
    else:bad['checkpoint_identity']='wrong'
    refused(lambda bad=bad:tool.validate_checkpoint(bad))
with tempfile.TemporaryDirectory(prefix='ws-c69-checkpoint-') as directory:
    path=Path(directory)/'checkpoint.json';c56.atomic_json(path,cp1)
    assert tool.read_checkpoint(path)['checkpoint_identity']==cp1['checkpoint_identity']
print('PASS: c69 authenticated-prefix checkpoint is deterministic and every identity mutation fails closed')

rr=result69(b,records69(b))
assert rr['c68_candidate_output_unchanged'] and rr['new_cross_row_candidate_count']==0
assert rr['c56_through_c68_evidence_read_only'] is True and rr['live_qword_reads']==0
assert rr['c52_production_watch_gate']=='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'
assert all(rr[k] is False for k in c56.FALSE_GATES)
assert sum(rr['terminal_phase_counts'].values())==sum(rr['terminal_reason_counts'].values())==rr['post_c67_boundary_instances_total']
print('PASS: c69 adds no admission/Fact/candidate and keeps all safety gates closed')
print('PASS: c69 focused boundary/checkpoint groups complete')
