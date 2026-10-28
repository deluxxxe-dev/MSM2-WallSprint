"""Focused c67 post-c65 boundary-classifier tests using owned synthetic fixtures."""
from contextlib import redirect_stderr, redirect_stdout
from copy import deepcopy
import io

import slot_publication_post_c65_boundary as tool

with redirect_stdout(io.StringIO()), redirect_stderr(io.StringIO()):
    import test_slot_publication_post_c63_stitched as prior66
prior = prior66.prior
c66,c58,c56,cfg = tool.c66,tool.c58,tool.c56,tool.cfg


def refused(fn,text=None):
    try:fn()
    except ValueError as error:
        if text:assert text in str(error),(text,str(error))
    else:raise AssertionError('refusal expected')


def fixture(roots=None):
    f=prior.after_functions(body=b'\x90',following=b'\x90',roots=roots)
    f[0x110c]=b'\xc3'
    return f


def baseline67(f):
    p,raw=prior66.certificates66(f);r=c66.scan(p)
    return tool.Baseline(p,accepted=r,owner_result=r),raw


b,_=baseline67(fixture());records=list(tool.replay_records(b));assert len(records)==1
record=records[0];assert record['terminal_reason']=='current_context_post_c65_reachable'
assert record['terminal_phase']==tool.PHASES[1] and record['c65_record_identities']
result=tool.classify(b);assert result['repository_result']==tool.READY and result['classification_complete'] is True
assert result['post_c65_uncertified_boundary_refused_roots_classified']==1
assert result['new_post_c65_boundaries_admitted']==result['new_cross_row_candidate_count']==0
print('PASS: c67 exact first post-c65 terminal is classified without admission')

bad=deepcopy(records[0]);bad['terminal_reason']='wrong'
refused(lambda:tool.Aggregate(b).add(bad),'wrong c67')
a=tool.Aggregate(b);a.add(records[0]);refused(lambda:a.add(records[0]),'duplicate')
refused(lambda:tool.Aggregate(b).finish(),'missing')
print('PASS: c67 record stream rejects stale, duplicate and missing evidence')

p,_=prior66.certificates66(fixture());good=c66.scan(p);bad=deepcopy(good);bad['post_c65_uncertified_boundary_refusals']=2
refused(lambda:tool.Baseline(p,accepted=good,owner_result=bad),'stale/wrong')
print('PASS: c67 rejects stale c66 owner accounting')

roots={0x1080:prior.lea(0x1080,cfg.SLOT+8)+prior.jump(0x1087,0x1100)}
bb,_=baseline67(fixture(roots));rr=list(tool.replay_records(bb));assert len(rr)==2
assert len({x['record_identity'] for x in rr})==len({x['original_root_runtime_row'] for x in rr})==2
print('PASS: c67 keeps different original roots as separate instances')

result=tool.classify(b)
assert result['c56_through_c66_evidence_read_only'] is True and result['live_qword_reads']==0
assert result['c52_production_watch_gate']=='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL'
assert all(result[k] is False for k in c56.FALSE_GATES)
print('PASS: c67 keeps all safety gates closed')
print('PASS: c67 focused boundary-classifier groups complete')
