"""Focused c70 c69-authorized continuation fixtures; no private EXE."""
from copy import deepcopy
import struct

import slot_publication_post_c69_stitched_fix as fix
import test_slot_publication_post_c65_stitched as prior68

tool = fix.base
prior67 = prior68.prior67
prior = prior68.prior
c69,c68,c58,c56 = tool.c69,tool.c68,tool.c58,tool.c56


def refused(fn,text=None):
    try:
        fn()
    except ValueError as error:
        if text is not None:
            assert text in str(error),(text,str(error))
    else:
        raise AssertionError('refusal expected')


def fixture70(source=b'\x90',after=b'\xc3',targets=None,roots=None):
    f = prior67.fixture(body=b'\x90',following=source,targets=targets,roots=roots)
    if after is not None:
        f[0x110c+len(source)] = after
    return f


def pool70(f,extra=()):
    p,raw = prior68.certificates68(f,extra)
    r68 = c68.scan(p)
    b69 = c69.Baseline(p,accepted=r68,owner_result=r68)
    r69,records = c69.classify(b69)
    cp = c69.validate_checkpoint(c69.make_checkpoint(b69,r69,records))
    return tool.ProofPool(b69,r69,cp,production=False),raw,r69,cp


assert tool.EXPECTED_RECORDS==191 and tool.EXPECTED_ALL_EXACT==177
assert tool.EXPECTED_CONTINUABLE_EXACT_EDGES==191 and tool.EXPECTED_INTERIOR_INSTANCES==13
assert tool.ProofPool is fix.ProofPool and tool.View is fix.View and tool.Transaction is fix.Transaction
print('PASS: c70 production contract pins accepted c69 continuation-domain counts')

p,_,_,_=pool70(fixture70())
r=tool.scan(p)
assert r['repository_result']==tool.READY
assert r['full_historical_replay_used'] is True and r['checkpoint_accelerated_resume_used'] is False
assert r['c69_boundary_instances_available']==r['c69_boundary_instances_considered']==1
assert r['c69_boundary_instances_preflight_eligible']==r['c69_boundary_instances_admitted']==1
assert r['c69_continuable_exact_edges_available']==r['c69_continuable_exact_edges_admitted']==1
assert r['c69_transaction_terminal_refusals']==0
assert r['historical_c69_candidate_counts']==p.result['historical_c68_candidate_counts']
assert c56.digest(p.result)==p.header['accepted_c69_result_identity']
assert all(r[k] is False for k in c56.FALSE_GATES)
print('PASS: c70 exact single-successor c69 terminal continues under unchanged full-root replay')
print('PASS: c70 reports inherited c69 candidate totals without mutating accepted c69 result identity')

cond=prior.jump(0x110c,0x1400,True)
p,_,_,_=pool70(fixture70(cond,b'\xc3',{0x1400:b'\xc3'}))
r=tool.scan(p)
assert r['c69_boundary_instances_preflight_eligible']==r['c69_boundary_instances_admitted']==1
assert r['c69_continuable_exact_edges_available']==r['c69_continuable_exact_edges_admitted']==2
print('PASS: c70 exact two-successor c69 terminal preflights both arms before admission')

# Owner-discovered lifecycle regression: after an accepted c69 continuation, a
# second distinct row-external event belongs to c70. It must become c70 POST and
# must not be sent back through c69's one-first-terminal observer.
f=fixture70(b'\x90',b'\x90');f[0x110e]=b'\xc3'
p,_,_,_=pool70(f)
r=tool.scan(p)
assert r['root_refusal_reasons'][tool.POST]==r['post_c69_uncertified_boundary_refusals']==1
assert r['c69_boundary_instances_considered']==r['c69_boundary_instances_preflight_eligible']==1
assert r['c69_boundary_instances_admitted']==0
assert r['c69_continuable_exact_edges_considered']==r['c69_continuable_exact_edges_preflight_eligible']==1
assert r['c69_continuable_exact_edges_admitted']==0
assert r['new_cross_row_candidate_count']==0
print('PASS: c70 later distinct edge bypasses retired c69 first-terminal observer and stops as post-c69 unknown')

jump=prior.jump(0x110c,0x1401)
p,_,_,_=pool70(fixture70(jump,None,{0x1400:b'\x90\xc3'}))
r=tool.scan(p)
assert r['c69_boundary_instances_considered']==1 and r['c69_boundary_instances_preflight_eligible']==0
assert r['c69_boundary_instances_admitted']==0 and r['c69_continuable_exact_edges_available']==0
assert r['c69_preflight_refusal_counts']['inside_unique_runtime_span_nonentry']==1
assert r['new_cross_row_candidate_count']==0
print('PASS: c70 interior c69 terminal remains wholly fail-closed')

p,_,r69,cp=pool70(fixture70())
bad=deepcopy(cp);bad['records'][0]['terminal_reason']='wrong'
refused(lambda:tool.ProofPool(p.baseline,r69,bad,production=False),'record')
print('PASS: c70 rejects mutated c69 checkpoint record evidence')

# Reproduce the owner-discovered transaction-origin class. The forced c68 POST
# is active while both the fixture baseline and the c69/c70 replays are built, so
# the observer comparison remains exact instead of comparing different c68 runs.
f=fixture70();p68,raw=prior68.certificates68(f)
original=c68.Transaction.mark_post_c67

def forced_transaction_terminal(self,key):
    raise c58.Refusal(c68.POST)

c68.Transaction.mark_post_c67=forced_transaction_terminal
try:
    r68=c68.scan(p68)
    b69=c69.Baseline(p68,accepted=r68,owner_result=r68)
    r69,records=c69.classify(b69)
    cp=c69.validate_checkpoint(c69.make_checkpoint(b69,r69,records))
    pool=tool.ProofPool(b69,r69,cp,production=False)
    rr=tool.scan(pool)
finally:
    c68.Transaction.mark_post_c67=original
assert rr['c69_boundary_instances_considered']==1 and rr['c69_boundary_instances_admitted']==0
assert rr['c69_transaction_terminal_refusals']==1
assert rr['c69_continuable_exact_edges_available']==0
assert rr['new_cross_row_candidate_count']==0
print('PASS: c70 transaction-origin c69 terminal validates then refuses without synthetic successor')

print('PASS: c70 focused full-replay c69 continuation groups complete')
