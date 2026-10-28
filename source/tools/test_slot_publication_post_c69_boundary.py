"""Focused c71 post-c69 terminal diagnostic fixtures; no private EXE."""
from copy import deepcopy

print('C71_FOCUSED_PREREQUISITE_C70_START',flush=True)
import test_slot_publication_post_c69_stitched as prior70
print('C71_FOCUSED_PREREQUISITE_C70_PASS',flush=True)

import slot_publication_post_c69_boundary as tool

c70,c58,c56 = tool.c70,tool.c58,tool.c56


def refused(fn,text=None):
    try:fn()
    except ValueError as error:
        if text is not None:assert text in str(error),(text,str(error))
    else:raise AssertionError('refusal expected')


assert tool.EXPECTED_TERMINALS==102
assert tool.EXPECTED_C70_FILE_SHA=='2780ca89a98ad22d12e2252f53cd202830bc8228edd67d0b0c7fb8eccf7f15ac'
assert tool.EXPECTED_C70_RESULT_IDENTITY=='9b213db28b05f9d1fc8baefa37f981aa521cd7ee45110a7cfe6b122567f0b9e1'
print('PASS: c71 production contract pins accepted c70 identity and exact 102-terminal domain')

# Reuse the owner-discovered c70 lifecycle fixture: one accepted c69 continuation
# reaches one later row-external event, which c71 must diagnose without traversal.
f=prior70.fixture70(b'\x90',b'\x90');f[0x110e]=b'\xc3'
p,_,_,_=prior70.pool70(f);r70=c70.scan(p)
assert r70['post_c69_uncertified_boundary_refusals']==1
b=tool.Baseline(p,r70,production=False);r,records=tool.classify(b)
assert r['repository_result']==tool.READY and r['post_c69_boundary_instances_total']==1
assert r['post_c69_uncertified_boundary_refused_roots_expected']==r['post_c69_uncertified_boundary_refused_roots_classified']==1
assert r['edge_backed_terminal_instances']==1 and r['transaction_terminal_instances']==0
assert r['post_c69_boundary_edges_total']>=1 and records[0]['source_instruction_rva'] is not None
assert records[0]['new_post_c69_boundaries_admitted']==records[0]['new_candidates_emitted']==0
assert r['new_post_c69_boundaries_admitted']==r['new_cross_row_candidate_count']==0
assert r['historical_c70_candidate_counts']['reference_candidate_count']==r70['reference_candidate_count']==0
assert all(r[k] is False for k in c56.FALSE_GATES) and r['live_qword_reads']==0
print('PASS: c71 exact edge-backed post-c69 terminal is classified without target traversal')

cp=tool.validate_checkpoint(tool.make_checkpoint(b,r,records));cp2=tool.validate_checkpoint(tool.make_checkpoint(b,r,records))
assert cp['checkpoint_identity']==cp2['checkpoint_identity']
bad=deepcopy(cp);bad['records'][0]['terminal_reason']='wrong'
refused(lambda:tool.validate_checkpoint(bad),'identity')
print('PASS: c71 diagnostic checkpoint is deterministic and record mutation fails closed')

# Force a transaction-level c70 POST at post-c69 marking. Apply the same condition
# while building the synthetic c70 owner result and while c71 replays it.
f=prior70.fixture70();p,_,_,_=prior70.pool70(f)
original=c70.Transaction.mark_post_c69

def forced_transaction_terminal(self,key,path=None):
    raise c58.Refusal(c70.POST)

c70.Transaction.mark_post_c69=forced_transaction_terminal
try:
    r70=c70.scan(p)
    assert r70['post_c69_uncertified_boundary_refusals']==1
    b=tool.Baseline(p,r70,production=False);rr,recs=tool.classify(b)
finally:
    c70.Transaction.mark_post_c69=original
assert rr['edge_backed_terminal_instances']==0 and rr['transaction_terminal_instances']==1
assert recs[0]['terminal_event_kind']=='mark_post_c69' and recs[0]['terminal_event_edge_backed'] is False
assert recs[0]['source_instruction_rva'] is None and recs[0]['edges']==[]
assert rr['new_post_c69_boundaries_admitted']==rr['new_cross_row_candidate_count']==0
print('PASS: c71 transaction-origin post-c69 terminal is recorded without synthetic instruction or successor')

bad=deepcopy(r70);bad['authentication_header']=dict(bad['authentication_header']);bad['authentication_header']['image_sha256']='wrong'
refused(lambda:tool.Baseline(p,bad,production=False),'contract')
print('PASS: c71 rejects stale c70 authentication evidence')

print('PASS: c71 focused diagnostic groups complete')
