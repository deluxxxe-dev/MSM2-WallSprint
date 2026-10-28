"""Fast c69 fix2 smoke: exercise transaction-origin capture/record without replaying c56-c68 fixtures."""
from types import SimpleNamespace

import slot_publication_post_c67_boundary_fix2 as fix

base = fix.base
c68,c58,c56 = base.c68,base.c58,base.c56

state = (None,)*16
sid = c58.state_id(state)
row = 7
root = 3
key = (row,sid)
code = b'\x90\xc3'

auth = {name: None for name in fix.AUTH_FIELDS}
auth.update(
    record_identity='auth-record',
    classifier_identity='auth-classifier',
    root_replay_identity='auth-root',
    entry_instance_identity='auth-entry',
    current_entry_instance_row=row,
    incoming_state_identity=sid,
    boundary_path_identity='auth-path',
    boundary_transition_identities=[],
    source_instruction_rva=0x1000,
    source_instruction_sha256='auth-ins',
    terminal_phase='auth-phase',
    terminal_reason='auth-reason',
    terminal_provenance={},
    local_c65_edge_identities=[])

class Session:
    def row_identity(self,index):
        return 'row-id-%d' % index

class View:
    def __init__(self):
        self.post67_capture = None
        self.post67_transaction_capture = None
        self.c67_record = auth
        self.c68_certificates = SimpleNamespace(index={root:0}, records=[c56.encoded(auth)], identity='cert-id')
        self.c67_proofs = {}
        self.c67_source_keys = {key}
        self.c67_scan_token = 'scan-auth'
        self.key = key
        self.session = Session()
        self.c67_key = key
        self.c67_prefix = 'c67-prefix'
        self.c67_cache_identity = 'cache-id'
        self.c67_siblings_used = set()
        self.c67_considered_before = set()
        self.c67_considered_allowed = set()
        self.c67_considered = set()
        self.c67_eligible = set()
        self.c67_edges_considered = set()
        self.c67_edges_eligible = set()
        self.c67_refusals = {}
        self.c67_mismatches = {}
    def row(self,index):
        assert index == row
        return (0x1000,0x1000+len(code),code,None)

view = View()
tx = SimpleNamespace(
    root=root,
    contexts={key:(state,[])},
    scan_token='scan-current',
    post67_origins={},post65_origins={},post63_origins={},post61_origins={},post_origins={},
    considered=set(),post_c67={key},crossings=[],c61_crossings=[],c63_crossings=[],c65_crossings=[],c67_crossings=[])

error = c58.Refusal(c68.POST)
fix.capture_transaction_terminal(tx,view,'mark_post_c67',error,key=key)
capture = view.post67_transaction_capture
assert capture is not None
assert capture['event_kind'] == 'mark_post_c67'
assert capture['row'] == row and capture['state_identity'] == sid
assert capture['terminal_provenance']['event_kind'] == 'mark_post_c67'

baseline = SimpleNamespace(identity='classifier-id',header={
    'accepted_c68_authentication_identity':'accepted-auth',
    'accepted_c68_result_identity':'accepted-result'},
    certificates=SimpleNamespace(identity='cert-id'))

original_prefix = c68.sibling_prefix
c68.sibling_prefix = lambda tx,view:'synthetic-prefix'
try:
    record = fix.record_for(baseline,tx,view)
finally:
    c68.sibling_prefix = original_prefix

assert record['terminal_event_kind'] == 'mark_post_c67'
assert record['terminal_event_edge_backed'] is False
assert record['source_instruction_rva'] is None
assert record['source_instruction_sha256'] is None
assert record['edges'] == []
assert record['terminal_context_runtime_row_sha256'] == 'row-id-%d' % row
assert record['c67_authorization']['record_identity'] == 'auth-record'
assert record['new_post_c67_boundaries_admitted'] == 0
assert record['new_candidates_emitted'] == 0
assert c56.digest({k:v for k,v in record.items() if k!='record_identity'}) == record['record_identity']

print('PASS: c69 quick transaction-origin capture/record smoke')
