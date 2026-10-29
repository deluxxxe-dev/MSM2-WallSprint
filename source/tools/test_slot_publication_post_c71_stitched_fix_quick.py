"""Quick regressions for the c72 owner-observed lifecycle/provenance/order blockers."""
from copy import deepcopy
from types import SimpleNamespace

import slot_publication_post_c71_stitched_fix as tool

base = tool.base
OriginalView = tool._OriginalView
OriginalTransaction = tool._OriginalTransaction


def make_view(*,retired,record=None,key=(2,'later'),scan_token=20):
    view = object.__new__(tool.View)
    view.c71_first_terminal_observer_retired = retired
    view.c71_record = record
    view.c71_key = (1,'first') if record is not None else None
    view.c71_scan_token = 10 if record is not None else None
    view.tx = SimpleNamespace(scan_token=scan_token,post_c71=set())
    view.key = key
    return view


original_post69 = OriginalView._post69_boundary
original_post71 = tool.View._post71_boundary
calls = []


def fake_post69(self,index,ins,kind,target,architectural,relations):
    calls.append(('old',index,ins.address,kind,target,architectural))
    if self.c71_record is None:
        self.c71_record = {'source_instruction_rva':ins.address}
        self.c71_key = self.key
        self.c71_scan_token = self.tx.scan_token
    return 'old-result'


def fake_post71(self,index,ins,kind,target,architectural,relations):
    calls.append(('next',index,ins.address,kind,target,architectural))
    return 'next-result'

try:
    OriginalView._post69_boundary = fake_post69
    tool.View._post71_boundary = fake_post71

    # The exact accepted source is still allowed to deliver remaining sibling
    # callbacks after the first callback has retired the first-terminal observer.
    record = {'source_instruction_rva':0x1111}
    same = make_view(retired=True,record=record,key=(1,'first'),scan_token=10)
    result = same._post69_boundary(1,SimpleNamespace(address=0x1111),'conditional_fallthrough',0x1200,True,{})
    assert result=='old-result'
    assert calls==[('old',1,0x1111,'conditional_fallthrough',0x1200,True)]

    # The first owner blocker: a later distinct c70 POST in the same root is not
    # a second c71 first terminal. It must bypass recapture and become the next
    # post-c71 unknown without claiming tx.post_c71 reachability.
    calls.clear()
    later = make_view(retired=True,record=record,key=(2,'later'),scan_token=20)
    result = later._post69_boundary(2,SimpleNamespace(address=0x2222),'ordinary_fallthrough',0x2300,True,{})
    assert result=='next-result'
    assert calls==[('next',2,0x2222,'ordinary_fallthrough',0x2300,True)]
    assert later.key not in later.tx.post_c71

    # A successful first c71 authorization retires the observer immediately for
    # subsequent distinct c70 POSTs in that root.
    calls.clear()
    first = make_view(retired=False,record=None,key=(3,'first-live'),scan_token=30)
    result = first._post69_boundary(3,SimpleNamespace(address=0x3333),'ordinary_fallthrough',0x3400,True,{})
    assert result=='old-result'
    assert first.c71_first_terminal_observer_retired is True
    assert calls==[('old',3,0x3333,'ordinary_fallthrough',0x3400,True)]
finally:
    OriginalView._post69_boundary = original_post69
    tool.View._post71_boundary = original_post71

print('PASS: c72 retired c71 first-terminal observer sends later distinct c70 POST to next unknown without relabeling reachability')
print('PASS: c72 retired observer still permits exact sibling callbacks from the authenticated c71 source')

# The second owner blocker: a valid c71 proof can represent a later-stage arrival
# whose current path does not itself contain c69_post_boundary proof. Do not call
# historical mark_post_c69 in that case; doing so would invent c69 reachability
# and raises "post-c69 arrival lacks c69 proof". Historical c69 proof paths must
# still use the original behavior unchanged.
original_mark69 = OriginalTransaction.mark_post_c69
mark_calls = []


def fake_mark69(self,key,path=None):
    mark_calls.append((key,tuple(p.get('proof_kind') for p in path or ())))
    return 'marked-c69'

try:
    OriginalTransaction.mark_post_c69 = fake_mark69
    tx = object.__new__(tool.Transaction)

    c71_only = ({'proof_kind':'c71_post_boundary'},)
    assert tx.mark_post_c69((4,'c71-only'),c71_only) is None
    assert mark_calls==[]

    c69_and_c71 = ({'proof_kind':'c69_post_boundary'},{'proof_kind':'c71_post_boundary'})
    assert tx.mark_post_c69((5,'historical-c69'),c69_and_c71)=='marked-c69'
    assert mark_calls==[((5,'historical-c69'),('c69_post_boundary','c71_post_boundary'))]

    mark_calls.clear()
    c69_only = ({'proof_kind':'c69_post_boundary'},)
    assert tx.mark_post_c69((6,'c69-only'),c69_only)=='marked-c69'
    assert mark_calls==[((6,'c69-only'),('c69_post_boundary',))]
finally:
    OriginalTransaction.mark_post_c69 = original_mark69

print('PASS: c72 c71-only arrival does not invent post-c69 reachability')
print('PASS: c72 paths with actual c69 proof preserve historical mark_post_c69 behavior')

# The third owner blocker occurred only after all 162472 roots completed. Each
# live record had already been authenticated exactly against the accepted record
# for its root and the full 102-record/95-record/99-edge domains had matched. Live
# discovery order is incidental, so iterate already-authenticated records in the
# accepted checkpoint root order for order-sensitive c71 result reproduction.
accepted = [
    {'original_root_runtime_row':20,'record_identity':'accepted-20','payload':'b'},
    {'original_root_runtime_row':10,'record_identity':'accepted-10','payload':'a'},
    {'original_root_runtime_row':30,'record_identity':'accepted-30','payload':'c'},
]
live = tool._CheckpointOrderedLiveRecords(accepted)
live.append(deepcopy(accepted[1]))
live.append(deepcopy(accepted[0]))
live.append(deepcopy(accepted[2]))
assert live.arrival_roots==[10,20,30]
assert live.checkpoint_roots==(20,10,30)
assert live.discovery_order_matches_checkpoint() is False
assert live.discovery_order_mismatch_positions()==2
assert list(live)==accepted
assert live==accepted

try:
    live.append(deepcopy(accepted[1]))
    raise AssertionError('duplicate root was not refused')
except base.c58.EvidenceError as error:
    assert str(error)=='duplicate live c71 terminal root in order adapter'

print('PASS: c72 exact per-root live records are normalized to accepted checkpoint order without weakening duplicate-root authentication')

# Owner attempt 4 still reached the same final stream guard after checkpoint-order
# normalization. Per-root authentication already uses c56.encoded(), whose JSON
# canonicalization treats tuple/list containers identically. The final stream
# guard must use that same identity relation instead of raw Python object equality.
accepted_repr = [{
    'original_root_runtime_row':40,
    'record_identity':'accepted-40',
    'terminal_provenance':{'key':[40,1234], 'path':[[1,2],[3,4]]},
}]
live_repr_record = {
    'original_root_runtime_row':40,
    'record_identity':'accepted-40',
    'terminal_provenance':{'key':(40,1234), 'path':[(1,2),(3,4)]},
}
assert live_repr_record != accepted_repr[0]
assert base.c56.encoded(live_repr_record)==base.c56.encoded(accepted_repr[0])
canonical = tool._CheckpointOrderedLiveRecords(accepted_repr)
canonical.append(live_repr_record)
assert list(canonical) != accepted_repr
assert canonical == accepted_repr

print('PASS: c72 final stream comparison uses canonical encoded record identity rather than raw Python list/tuple representation')
