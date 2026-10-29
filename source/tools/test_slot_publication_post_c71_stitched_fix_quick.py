"""Quick regression for the c72 owner-observed duplicate-live-root lifecycle."""
from types import SimpleNamespace

import slot_publication_post_c71_stitched_fix as tool

base = tool.base
OriginalView = tool._OriginalView


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

    # The owner blocker: a later distinct c70 POST in the same root is not a
    # second c71 first terminal.  It must bypass recapture and become the next
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
