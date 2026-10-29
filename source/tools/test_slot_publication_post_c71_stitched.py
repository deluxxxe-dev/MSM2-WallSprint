"""Focused c72 continuation fixtures; no private EXE."""
from copy import deepcopy

print('C72_FOCUSED_PREREQUISITE_C71_START',flush=True)
import test_slot_publication_post_c69_boundary as prior71
print('C72_FOCUSED_PREREQUISITE_C71_PASS',flush=True)

import slot_publication_post_c71_stitched as tool

c71,c70,c58,c56 = tool.c71,tool.c70,tool.c58,tool.c56
prior70 = prior71.prior70


def refused(fn,text=None):
    try:
        fn()
    except ValueError as error:
        if text is not None:
            assert text in str(error),(text,str(error))
    else:
        raise AssertionError('refusal expected')


def pool72(f):
    p,_,_,_=prior70.pool70(f)
    r70=c70.scan(p)
    b71=c71.Baseline(p,r70,production=False)
    r71,records=c71.classify(b71)
    cp71=c71.validate_checkpoint(c71.make_checkpoint(b71,r71,records))
    return tool.ProofPool(b71,cp71,production=False),r71,records,cp71


assert tool.EXPECTED_RECORDS==102
assert tool.EXPECTED_ALL_EXACT==95
assert tool.EXPECTED_CONTINUABLE_EXACT_EDGES==99
assert tool.EXPECTED_C71_CHECKPOINT_FILE_SHA=='25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926'
assert tool.EXPECTED_C71_CHECKPOINT_IDENTITY=='4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e'
assert tool.EXPECTED_C71_RESULT_IDENTITY=='18d61be59032514a4c74c9b015f422c379e2c05050e2d5819c1025048a2276d7'
assert tool.EXPECTED_C71_RECORD_SET=='6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8'
print('PASS: c72 production contract pins accepted c71 identities and 95/99 continuation domain')

# One c69 continuation reaches one c71 exact terminal and then RETs after c72 admission.
f=prior70.fixture70(b'\x90',b'\x90');f[0x110e]=b'\xc3'
p,r71,records,_=pool72(f)
assert r71['post_c69_boundary_instances_total']==1
assert r71['complete_successor_relation_group_counts']['all_successors_exact_runtime_begin']==1
r=tool.scan(p)
assert r['repository_result']==tool.READY
assert r['c71_boundary_instances_available']==r['c71_boundary_instances_considered']==1
assert r['c71_all_successor_exact_instances_available']==r['c71_boundary_instances_preflight_eligible']==1
assert r['c71_continuable_exact_edges_available']==r['c71_continuable_exact_edges_preflight_eligible']==1
assert r['c71_boundary_instances_admitted']==1 and r['c71_continuable_exact_edges_admitted']==1
assert r['new_cross_row_candidate_count']==0 and r['live_qword_reads']==0
assert all(r[k] is False for k in c56.FALSE_GATES)
print('PASS: c72 exact c71 terminal continues only after exact live record/all-successor authentication')

# Add one more row-external edge after the c71-authorized destination. It must be
# the next post-c71 unknown rather than being borrowed as another c71 authorization.
f=prior70.fixture70(b'\x90',b'\x90');f[0x110e]=b'\x90';f[0x110f]=b'\xc3'
p,_,_,_=pool72(f)
rr=tool.scan(p)
assert rr['post_c71_uncertified_boundary_refusals']==1
assert rr['c71_boundary_instances_considered']==rr['c71_boundary_instances_preflight_eligible']==1
assert rr['c71_boundary_instances_admitted']==0
assert rr['c71_continuable_exact_edges_admitted']==0
assert rr['new_cross_row_candidate_count']==0
print('PASS: c72 later distinct edge stops as uncertified_post_c71_runtime_boundary and rolls back provisional admission')

# A c71 boundary that targets an interior address remains wholly fail-closed.
jump=prior70.prior.jump
f=prior70.fixture70(jump(0x110c,0x1400),None,{
    0x1400:jump(0x1400,0x1501),
    0x1500:b'\x90\xc3'})
p,r71,_,_=pool72(f)
assert r71['target_metadata_relation_counts']['inside_unique_runtime_span_nonentry']==1
rr=tool.scan(p)
assert rr['c71_boundary_instances_considered']==1
assert rr['c71_boundary_instances_preflight_eligible']==0
assert rr['c71_boundary_instances_admitted']==0
assert rr['c71_preflight_refusal_counts']['inside_unique_runtime_span_nonentry']==1
assert rr['new_cross_row_candidate_count']==0
print('PASS: c72 interior c71 terminal remains diagnostic/refused and is never promoted to an entry')

f=prior70.fixture70(b'\x90',b'\x90');f[0x110e]=b'\xc3'
p,_,_,cp=pool72(f)
bad=deepcopy(cp);bad['records'][0]['terminal_reason']='wrong'
refused(lambda:tool.ProofPool(p.baseline,bad,production=False),'identity')
print('PASS: c72 rejects mutated c71 checkpoint record evidence')

print('PASS: c72 focused full-replay c71 continuation groups complete')
