"""Owned c66 c65-authenticated continuation tests; no private EXE."""
from copy import deepcopy
import struct

import slot_publication_post_c63_stitched as tool
import test_slot_publication_post_c63_boundary as prior

c65,c64,c63,c62,c61,c60,c59,c58,c57,c56,cfg = tool.c65,tool.c64,tool.c63,tool.c62,tool.c61,tool.c60,tool.c59,tool.c58,tool.c57,tool.c56,tool.cfg


def refused(fn,text=None):
    try:fn()
    except ValueError as error:
        if text:assert text in str(error),(text,str(error))
    else:raise AssertionError('refusal expected')


def certificates66(f,extra=()):
    b,raw = prior.baseline65(f,extra);r = prior.result65(b)
    return tool.ProofPool(b,accepted=r,owner_result=r),raw


def scan66(f,extra=(),limits=c58.Limits()):
    p,_ = certificates66(f,extra)
    return tool.scan(p,limits)


p,_=certificates66(prior.after_functions());r=tool.scan(p)
assert r['repository_result']==tool.READY and r['c65_boundary_instances_considered']==r['c65_boundary_instances_admitted']==1
assert r['c65_exact_edges_considered']==r['c65_exact_edges_admitted']==1 and r['post_c65_uncertified_boundary_refusals']==0
assert r['new_cross_row_candidate_count']==r['writer_candidates']==0
print('PASS: c66 exact ordinary c65 boundary continues only the authenticated exact current record')

for kind,body,following,targets in [
    ('direct_unconditional_branch',prior.jump(0x110a,0x1400),None,{0x1400:b'\xc3'}),
    ('direct_conditional_taken',prior.jump(0x110a,0x1400,True),b'\xc3',{0x1400:b'\xc3'}),
    ('call_fallthrough',b'\xe8'+struct.pack('<i',0x1200-0x110f),b'\xc3',{})]:
    rr=scan66(prior.after_functions(body,following,targets))
    assert rr['c65_boundary_instances_admitted']==1 and rr['c65_exact_edges_admitted']>=1
print('PASS: c66 direct/conditional/CALL-fallthrough exact successors use the same bounded all-successor contract')

rr=scan66(prior.after_functions(prior.jump(0x110a,0x1400,True),b'\xc3',{0x1400:b'\xc3'}))
assert rr['c65_exact_edges_considered']==rr['c65_exact_edges_admitted']==2 and rr['c65_boundary_instances_admitted']==1
print('PASS: c66 multi-successor boundary admits only after both architectural arms preflight exactly')

rr=scan66(prior.after_functions(prior.jump(0x110a,0x1401,True),b'\xc3',{0x1400:b'\x90\xc3'}))
assert rr['c65_boundary_instances_considered']==1 and rr['c65_boundary_instances_admitted']==0
assert rr['c65_all_successor_preflight_refusal_relation_counts']['inside_unique_runtime_span_nonentry']==1
assert rr['new_cross_row_candidate_count']==0
print('PASS: c66 one interior arm aborts the whole root and commits no provisional candidate')

rr=scan66(prior.after_functions(prior.jump(0x110a,0x1400),None),[(b'.free',0x1400,b'\xc3',0x60000020,1)])
assert rr['c65_boundary_instances_admitted']==0
assert rr['c65_all_successor_preflight_refusal_relation_counts']['executable_file_backed_outside_runtime_metadata']==1
print('PASS: c66 metadata-free RX remains refused and is never promoted to an entry')

pp,_=certificates66(prior.local65());rr=tool.scan(pp)
assert rr['c65_terminal_phase_considered'].get(c65.PHASES[0],0)>0
assert rr['c65_terminal_reason_considered'].get('used_c63_source_later_instruction',0)>0
print('PASS: c66 preserves c65 source-local phase/reason instead of making it row-global')

f=prior.local_functions();f[0x1100]=f[0x1100][:-5]+prior.jump(0x1115,0x1200);f[0x1200]=prior.jump(0x1200,0x1700);f[0x1600]=b'\xc3';f[0x1700]=b'\xc3'
pp,_=certificates66(f);rr=tool.scan(pp)
assert rr['c65_terminal_phase_considered'].get(c65.PHASES[2],0)>0
print('PASS: c66 keeps dedup-stored path provenance separate and cannot borrow a different arrival proof')

pp,_=certificates66(prior.converted65());records=list(c65.replay_records(pp.baseline));assert records
assert records[0]['terminal_phase']==c65.PHASES[3] and records[0]['post_c63_origin'] is None
assert records[0]['terminal_reason'] in ('prior_c63_authorization_converted_c62_post','prior_c63_authorization_converted_pre_c59_later')
rr=tool.scan(pp);assert rr['c65_terminal_phase_considered'].get(c65.PHASES[3],0)>0
print('PASS: c66 converted-refusal shape uses only its exact c65 provenance and does not borrow earlier authorization')

b,_=prior.baseline65(prior.after_functions());good=prior.result65(b);bad=deepcopy(good);bad['terminal_reason_counts']={'wrong':1}
refused(lambda:tool.ProofPool(b,accepted=good,owner_result=bad),'stale/wrong')
print('PASS: c66 stale/mismatched c65 phase/reason owner evidence fails closed')

f=prior.after_functions(body=b'\x90',following=b'\x90');f[0x110c]=b'\xc3'
rr=scan66(f)
assert rr['c65_boundary_instances_admitted']==1 and rr['post_c65_uncertified_boundary_refusals']==1
assert rr['root_refusal_reasons'][tool.POST]==1 and rr['new_cross_row_candidate_count']==0
print('PASS: c66 next unknown boundary stops as uncertified_post_c65_runtime_boundary with no recursive certificate')

f=prior.after_functions(roots={0x1080:prior.lea(0x1080,cfg.SLOT+8)+prior.jump(0x1087,0x1100)})
pp,_=certificates66(f);records=list(c65.replay_records(pp.baseline));assert len(records)==2
assert len({x['record_identity'] for x in records})==len({x['original_root_runtime_row'] for x in records})==2
rr=tool.scan(pp);assert rr['c65_boundary_instances_considered']==2
print('PASS: c66 different-root/path same-source contexts remain separate exact instances')

rr=scan66(prior.after_functions())
assert all(rr[k] is False for k in c56.FALSE_GATES)
assert rr['c52_production_watch_gate']=='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'
assert rr['live_qword_reads']==0 and rr['c56_through_c65_evidence_read_only'] is True
print('PASS: c66 keeps all completeness/publication/ownership/production/watch/game gates closed')

print('PASS: c66 focused continuation groups complete')
