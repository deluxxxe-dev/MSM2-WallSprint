"""Quick c71 regression for owner-observed retired-c69-observer provenance."""
from types import SimpleNamespace

import slot_publication_post_c69_boundary_fix as fix

base = fix.base
key = (7,'sid')
state = object()
path = ()
tx = SimpleNamespace(
    contexts={key:(state,path)},post_c69=set(),post69_origins={},scan_token=11)

class Session:
    def row_identity(self,index):
        return 'row-sha'

view = SimpleNamespace(
    key=key,c69_source_keys=set(),
    c69_record={'source_instruction_rva':100,'record_identity':'accepted-c69-record'},
    c69_key=(1,'old-sid'),c69_scan_token=3,
    session=Session(),row=lambda index:(1000,1010,b'\x90'*10,None))
ins = SimpleNamespace(address=1005,size=1,bytes=b'\x90',mnemonic='nop')

original_state_id = base.c58.state_id
original_terminal_common = base._terminal_common
original_classify_edges = base.c59.classify_edges
original_emit = base._emit
captured = []
try:
    base.c58.state_id = lambda value:'sid'
    base._terminal_common = lambda tx,view,key,state,path,event_kind,edge_backed,provenance: {
        'terminal_event_kind':event_kind,
        'terminal_event_edge_backed':edge_backed,
        'terminal_phase':provenance['phase'],
        'terminal_reason':provenance['reason'],
        'terminal_provenance':provenance,
    }
    base.c59.classify_edges = lambda session,begin,end,ins:[{
        'edge_kind':'ordinary_fallthrough',
        'edge_target_rva':2000,
        'architectural_successor':True,
        'target_metadata_relation':'exact_unique_runtime_begin',
    }]
    base._emit = lambda view,record:captured.append(record.copy())
    fix._capture_edge(tx,view,7,ins,'ordinary_fallthrough',2000,True)
finally:
    base.c58.state_id = original_state_id
    base._terminal_common = original_terminal_common
    base.c59.classify_edges = original_classify_edges
    base._emit = original_emit

assert len(captured)==1
record=captured[0]
assert record['terminal_phase']=='retired_c69_first_terminal_observer_distinct_edge'
assert record['terminal_reason']=='c70_retired_c69_first_terminal_observer_later_distinct_edge'
assert record['terminal_provenance']['post_c69'] is False
assert record['terminal_provenance']['used_c69_source'] is False
assert record['terminal_provenance']['retired_c69_first_terminal_observer'] is True
assert record['terminal_provenance']['active_c69_record_identity']=='accepted-c69-record'
assert record['source_instruction_rva']==1005
assert len(record['edges'])==1
assert record['new_post_c69_boundaries_admitted'] if 'new_post_c69_boundaries_admitted' in record else True
print('PASS: c71 owner-observed retired c69 observer distinct edge is recorded without relabeling current reachability')
