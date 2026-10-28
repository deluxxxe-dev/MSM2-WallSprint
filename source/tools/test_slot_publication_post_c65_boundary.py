"""Focused c67 post-c65 boundary-classifier tests using owned synthetic fixtures."""
from contextlib import redirect_stderr, redirect_stdout
from copy import deepcopy
import io
from pathlib import Path
import struct
import tempfile

import slot_publication_post_c65_boundary as tool

with redirect_stdout(io.StringIO()), redirect_stderr(io.StringIO()):
    import test_slot_publication_post_c63_stitched as prior66
prior = prior66.prior
c65,c66,c58,c56,cfg = tool.c65,tool.c66,tool.c58,tool.c56,tool.cfg


def refused(fn,text=None):
    try:fn()
    except ValueError as error:
        if text:assert text in str(error),(text,str(error))
    else:raise AssertionError('refusal expected')


def fixture(body=b'\x90',following=b'\xc3',targets=None,roots=None):
    """One admitted c65 boundary at 0x110a, then a c67-observed boundary at 0x110b."""
    f=prior.after_functions(body=b'\x90',following=body,targets=targets,roots=roots)
    if following is not None:f[0x110b+len(body)]=following
    return f


def baseline67(f,extra=()):
    p,raw=prior66.certificates66(f,extra);r=c66.scan(p)
    return tool.Baseline(p,accepted=r,owner_result=r),raw


def records67(b):return list(tool.replay_records(b))


def result67(b,records=None,limit=8):
    a=tool.Aggregate(b,limit)
    for record in records67(b) if records is None else records:a.add(record)
    return a.finish()


b,_=baseline67(fixture());records=records67(b);assert len(records)==1
record=records[0];assert record['source_instruction_rva']==0x110b
assert record['terminal_reason']=='current_context_post_c65_reachable' and record['terminal_phase']==tool.PHASES[1]
assert record['c65_record_identities'] and record['post_c65_origin']['arrival_path']['c65_record_identities']
result=result67(b);assert result['repository_result']==tool.READY and result['classification_complete'] is True
assert result['post_c65_uncertified_boundary_refused_roots_classified']==1
assert result['edge_kind_counts']['ordinary_fallthrough']==result['target_metadata_relation_counts']['exact_unique_runtime_begin']==1
assert result['new_post_c65_boundaries_admitted']==result['new_cross_row_candidate_count']==0
print('PASS: c67 exact first post-c65 terminal binds current source/path/provenance and stays diagnostic-only')

for kind,body,following,targets in [
    ('direct_unconditional_branch',prior.jump(0x110b,0x1400),None,{0x1400:b'\xc3'}),
    ('direct_conditional_taken',prior.jump(0x110b,0x1400,True),b'\xc3',{0x1400:b'\xc3'}),
    ('call_fallthrough',b'\xe8'+struct.pack('<i',0x1200-0x1110),b'\xc3',{})]:
    bb,_=baseline67(fixture(body,following,targets));rr=result67(bb)
    assert rr['edge_kind_counts'][kind]==1 and rr['new_post_c65_boundaries_admitted']==0
    if kind=='direct_conditional_taken':assert rr['edge_kind_counts']['conditional_fallthrough']==1
    if kind=='call_fallthrough':assert rr['examples_by_edge_kind'][kind][0]['edge_target_rva']==0x1110
print('PASS: c67 direct/conditional/CALL-fallthrough edge vocabulary stays classification-only')

bb,_=baseline67(fixture(prior.jump(0x110b,0x1400,True),b'\xc3',{0x1400:b'\xc3'}));rr=result67(bb)
assert rr['post_c65_boundary_instances_total']==1 and rr['post_c65_boundary_edges_total']==2
assert rr['architectural_post_c65_boundary_edges_total']==2 and rr['model_only_post_c65_boundary_edges_total']==0
assert rr['new_post_c65_boundaries_admitted']==0
print('PASS: c67 multi-successor terminal classifies both architectural arms without target traversal')

bb,_=baseline67(fixture(prior.jump(0x110b,0x1401),None,{0x1400:b'\x90\xc3'}))
assert result67(bb)['target_metadata_relation_counts']['inside_unique_runtime_span_nonentry']==1
bb,_=baseline67(fixture(prior.jump(0x110b,0x1400),None),[(b'.free',0x1400,b'\xc3',0x60000020,1)])
assert result67(bb)['target_metadata_relation_counts']['executable_file_backed_outside_runtime_metadata']==1
print('PASS: c67 preserves interior and metadata-free RX relations without promoting either to an entry')

for field,value in [
    ('original_root_runtime_row',99),('current_entry_instance_row',99),('incoming_state_identity','wrong'),
    ('boundary_path_identity','wrong'),('source_instruction_rva',0),('source_instruction_sha256','wrong'),
    ('root_replay_identity','wrong'),('current_c66_continuation_identity','wrong'),('terminal_phase',tool.PHASES[0]),
    ('terminal_reason','wrong'),('post_c65_origin',None),('accepted_c66_authentication_identity','wrong'),
    ('accepted_c66_result_identity','wrong')]:
    bad=deepcopy(record);bad[field]=value;a=tool.Aggregate(b);refused(lambda:a.add(bad));refused(a.finish,'poisoned')
for mutate in ['terminal_provenance','c65_authorization']:
    bad=deepcopy(record)
    if mutate=='terminal_provenance':bad[mutate]['underlying_refusal']='wrong'
    else:bad[mutate]['terminal_reason']='wrong'
    a=tool.Aggregate(b);refused(lambda:a.add(bad));refused(a.finish,'poisoned')
print('PASS: c67 exact root/state/path/source/c66-prefix/phase/reason/origin/c65 provenance cannot be borrowed or relabeled')

# Reuse the owned c65 dedup fixture, then make its admitted c65 destination cross one
# additional row boundary. The old stored-path phase stays nested in c65 authorization;
# the current post-c65 arrival is independently represented by a c65 proof path.
f=prior.local_functions();f[0x1100]=f[0x1100][:-5]+prior.jump(0x1115,0x1200);f[0x1200]=prior.jump(0x1200,0x1700)
f[0x1600]=b'\xc3';f[0x1700]=b'\x90';f[0x1701]=b'\xc3'
dedup,_=baseline67(f);dv=records67(dedup)
d=next(x for x in dv if x['c65_authorization']['terminal_phase']==c65.PHASES[2])
assert d['terminal_phase']==tool.PHASES[1] and d['post_c65_origin']['arrival_path']['c65_record_identities']
assert d['c65_authorization']['terminal_phase']=='post_c63_reachable_dedup_stored_earlier_path'
print('PASS: c67 keeps stored earlier-path c65 provenance separate from the actual post-c65 arrival')

# Likewise preserve a converted c65 authorization as nested provenance rather than
# relabeling it as natural c65 reachability. The c67 terminal itself is a later,
# separately authenticated post-c65 arrival.
f=prior.converted65();f[0x1700]=b'\x90';f[0x1701]=b'\xc3'
converted,_=baseline67(f);cv=records67(converted)
c=next(x for x in cv if x['c65_authorization']['terminal_phase']==c65.PHASES[3])
assert c['c65_authorization']['terminal_reason'] in ('prior_c63_authorization_converted_c62_post','prior_c63_authorization_converted_pre_c59_later')
assert c['terminal_phase']==tool.PHASES[1] and c['post_c65_origin'] is not None
assert c['terminal_provenance']['underlying_refusal'] is None
print('PASS: c67 preserves converted c65 authorization provenance without turning it into current reachability')

bad=deepcopy(record);bad['terminal_reason']='wrong'
refused(lambda:tool.Aggregate(b).add(bad),'wrong c67')
a=tool.Aggregate(b);a.add(record);refused(lambda:a.add(record),'duplicate');refused(a.finish,'poisoned')
refused(lambda:tool.Aggregate(b).finish(),'missing')
roots={0x1080:prior.lea(0x1080,cfg.SLOT+8)+prior.jump(0x1087,0x1100)}
bb,_=baseline67(fixture(roots=roots));two=records67(bb);assert len(two)==2
a=tool.Aggregate(bb);refused(lambda:a.add(two[1]),'out-of-order');refused(a.finish,'poisoned')
print('PASS: c67 record stream rejects stale, duplicate, missing and out-of-order evidence')

p,_=prior66.certificates66(fixture());good=c66.scan(p);bad=deepcopy(good);bad['post_c65_uncertified_boundary_refusals']=2
refused(lambda:tool.Baseline(p,accepted=good,owner_result=bad),'stale/wrong')
with tempfile.TemporaryDirectory(prefix='ws-c67-') as directory:
    wrong=Path(directory)/'c66.json';wrong.write_text('{}')
    refused(lambda:tool.read_c66(wrong),'physical')
print('PASS: c67 rejects stale semantic c66 accounting and wrong physical c66 owner evidence')

# An invalidation-only post-c65 refusal has no unique source edge. It must remain
# unusable instead of fabricating a diagnostic boundary instruction.
s=tool.evidence(b.certificates).session;view=tool.View(b.certificates)
tx=tool.Transaction(view,0,c58.c55.Engine(s.domain,s.profile.summary),c58.Limits());key=(0,'owned')
tx.contexts[key]=(tuple([None]*16),());tx.forward[key]=set();tx.reverse[key]=set();view.c65_source_keys.add(key)
refused(lambda:tx.mark_post_c65(key),c66.POST);assert view.post65_capture is None
refused(lambda:tool.record_for(b,tx,view),'without source edge callback')
print('PASS: c67 invalidation-only terminal without an exact source edge stays blocked')

# Positive historical candidates, including both extended and legacy roots, must stay
# byte-identical while c67 emits no new candidate or Fact propagation.
f=fixture();f.update({0x2000:prior.lea(0x2000)+prior.jump(0x2007,0x2100),0x2100:bytes.fromhex('488908c3')})
f.update({0x3000:prior.lea(0x3000)+prior.jump(0x3007,0x3100),0x3100:bytes.fromhex('7505')+prior.jump(0x3102,0x3200)+b'\x90',
    0x3108:b'\x90',0x3109:b'\x90',0x310a:bytes.fromhex('488b08c3'),0x3200:b'\xc3'})
positive,_=baseline67(f);original=deepcopy(positive.result);rr=result67(positive)
assert original['reference_candidate_count']>=2 and positive.result==original
assert rr['c66_candidate_output_sha256']==c56.digest(original['reference_candidates']) and rr['c66_candidate_output_unchanged']
assert rr['historical_c66_candidate_counts']['reference_candidate_count']==original['reference_candidate_count']
assert rr['new_post_c65_boundaries_admitted']==rr['new_cross_row_candidate_count']==0 and rr['cross_runtime_fact_propagation_unchanged']
print('PASS: c67 preserves complete c66 candidate bytes/counts and adds no edge, Fact or candidate admission')

result=result67(b)
assert result['c56_through_c66_evidence_read_only'] is True and result['live_qword_reads']==0
assert result['c52_production_watch_gate']=='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'
assert all(result[k] is False for k in c56.FALSE_GATES)
assert sum(result['terminal_phase_counts'].values())==sum(result['terminal_reason_counts'].values())==result['post_c65_boundary_instances_total']
print('PASS: c67 keeps all completeness/publication/ownership/production/watch/game gates closed')
print('PASS: c67 focused boundary-classifier groups complete')
