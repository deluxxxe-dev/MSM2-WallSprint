"""Run c69 focused coverage under terminal-origin correction."""
import runpy

import slot_publication_post_c67_boundary_fix2 as fix

base = fix.base
assert base.View is fix.View
assert base.Transaction is fix.Transaction
assert base.record_for is fix.record_for
assert base.Aggregate is fix.Aggregate
assert base.sources is fix.sources

ns = runpy.run_path('source/tools/test_slot_publication_post_c67_boundary.py',run_name='__main__')

# Force the c68 POST to originate from transaction-level post-c67 marking rather
# than View.edge.  The accepted synthetic c68 root still ends as the same POST;
# c69 must bind the transaction origin and must not invent outgoing edges.
b,_ = ns['baseline69'](ns['fixture69']())
c68,c58 = base.c68,base.c58
original = c68.Transaction.mark_post_c67

def forced_transaction_terminal(self,key):
    raise c58.Refusal(c68.POST)

c68.Transaction.mark_post_c67 = forced_transaction_terminal
try:
    records = list(base.replay_records(b))
finally:
    c68.Transaction.mark_post_c67 = original

assert len(records)==1
record=records[0]
assert record['terminal_event_kind']=='mark_post_c67'
assert record['terminal_event_edge_backed'] is False
assert record['source_instruction_rva'] is None and record['edges']==[]
assert record['terminal_provenance']['event_kind']=='mark_post_c67'
assert record['c67_authorization']['record_identity']

a=base.Aggregate(b,records)
for item in records:a.add(item)
result=a.finish()
assert result['transaction_terminal_instances']==1
assert result['edge_backed_terminal_instances']==0
assert result['terminal_event_kind_counts']=={'mark_post_c67':1}
assert result['post_c67_boundary_edges_total']==0
assert result['terminal_origin_classification_complete'] is True
assert result['new_post_c67_boundaries_admitted']==result['new_cross_row_candidate_count']==0
assert all(result[k] is False for k in base.c56.FALSE_GATES)
print('PASS: c69 transaction-origin POST is classified without inventing an instruction or successor')
print('PASS: c69 terminal-origin correction preserves the complete focused contract')
