"""c69 terminal-origin correction after the first owner OFFLINE run.

Accepted c68 uses the same POST refusal for two observable classes:
(1) an instruction edge callback and (2) transaction-level entry/propagation
fail-closed guards.  The original c69 observer assumed class (1) only.

This layer does not change c68 continuation semantics.  It observes every c68
POST root, keeps the existing edge classifier for edge-backed terminals, and
records exact transaction context/provenance without inventing an instruction or
successor for transaction-level terminals.
"""
from collections import Counter
from copy import deepcopy
from pathlib import Path

import slot_publication_post_c67_boundary_fix as fix1

base = fix1.base
c68,c67,c58,c56 = base.c68,base.c67,base.c58,base.c56
_OriginalView = base.View
_OriginalTransaction = base.Transaction
_OriginalAggregate = base.Aggregate
_original_record_for = base.record_for
_original_sources = base.sources

AUTH_FIELDS = (
    'record_identity','classifier_identity','root_replay_identity','entry_instance_identity',
    'current_entry_instance_row','incoming_state_identity','boundary_path_identity','boundary_transition_identities',
    'source_instruction_rva','source_instruction_sha256','terminal_phase','terminal_reason','terminal_provenance',
    'post_c59_origin','post_c61_origin','post_c63_origin','post_c65_origin','c65_authorization',
    'c65_terminal_phase','c65_terminal_reason','local_c65_edge_identities')


def sources():
    return dict(_original_sources(),
        slot_publication_post_c67_boundary_fix2_py=c56.sha(Path(__file__).read_bytes()))


def _context(tx,view,*,key=None,row=None,state=None,path=None,source=None,target=None):
    if row is not None and state is not None and path is not None:
        sid = c58.state_id(state)
        return (row,sid),state,path
    for candidate in (key,target,source,getattr(view,'key',None)):
        if candidate is not None and candidate in tx.contexts:
            current_state,current_path = tx.contexts[candidate]
            if candidate[1]!=c58.state_id(current_state):
                raise c58.EvidenceError('wrong c68 transaction terminal incoming state')
            return candidate,current_state,current_path
    raise c58.EvidenceError('c68 transaction terminal lacks exact current context')


def capture_transaction_terminal(tx,view,event_kind,error,*,key=None,row=None,state=None,path=None,source=None,target=None):
    if str(error)!=c68.POST or view.post67_capture is not None:
        return
    context_key,state,path = _context(tx,view,key=key,row=row,state=state,path=path,source=source,target=target)
    row = context_key[0]
    auth = view.c67_record
    ordinal = view.c68_certificates.index.get(tx.root)
    if auth is None or ordinal is None or c56.encoded(auth)!=view.c68_certificates.records[ordinal]:
        raise c58.EvidenceError('transaction c68 POST lacks exact sealed c67 authorization at original root')
    begin,end,code,_ = view.row(row)
    cause = str(error.__cause__) if error.__cause__ is not None else None
    reason = ('transaction_'+event_kind+'_converted_'+cause) if cause else ('transaction_'+event_kind+'_guard_refusal')
    path_info = base.path_summary(path)
    event_context = dict(event_kind=event_kind,key=list(context_key),
        source=list(source) if source is not None else None,target=list(target) if target is not None else None,
        current_view_key=list(view.key) if view.key is not None else None,
        scan_token=tx.scan_token,underlying_refusal=cause)
    capture = dict(root=tx.root,row=row,state_identity=context_key[1],state=base.serialize_state(state),path=path_info,
        entry_seed_fact_count=sum(f is not None for f in state),row_sha256=view.session.row_identity(row),code_sha256=c56.sha(code),
        begin=begin,end=end,event_kind=event_kind,event_context=event_context,
        phase='transaction_'+event_kind,terminal_provenance=dict(reason=reason,underlying_refusal=cause,
            event_kind=event_kind,event_context=deepcopy(event_context)),
        post_c67_origin=deepcopy(getattr(tx,'post67_origins',{}).get(context_key)),
        post_c65_origin=deepcopy(getattr(tx,'post65_origins',{}).get(context_key)),
        post_c63_origin=deepcopy(getattr(tx,'post63_origins',{}).get(context_key)),
        post_c61_origin=deepcopy(getattr(tx,'post61_origins',{}).get(context_key)),
        post_c59_origin=deepcopy(getattr(tx,'post_origins',{}).get(context_key)),
        c67_authorization={k:deepcopy(auth[k]) for k in AUTH_FIELDS},
        local_c67_edge_identities=sorted(p['edge_identity'] for p in view.c67_proofs.values()),
        c67_source_contexts=sorted(view.c67_source_keys),c67_scan_token=view.c67_scan_token,current_scan_token=tx.scan_token)
    existing = view.post67_transaction_capture
    if existing is not None and c56.encoded(existing)!=c56.encoded(capture):
        # The innermost c68 POST origin wins.  An outer wrapper may observe the
        # same exception again, but it must not relabel the terminal origin.
        return
    if existing is None:
        view.post67_transaction_capture = capture


class View(_OriginalView):
    def __init__(self,certificates):
        super().__init__(certificates)
        self.post67_transaction_capture = None


class Transaction(_OriginalTransaction):
    def _observe(self,event_kind,error,**context):
        capture_transaction_terminal(self,self.e,event_kind,error,**context)

    def mark_post(self,key):
        try:return super().mark_post(key)
        except c58.Refusal as error:
            self._observe('mark_post',error,key=key);raise

    def mark_post_c61(self,key):
        try:return super().mark_post_c61(key)
        except c58.Refusal as error:
            self._observe('mark_post_c61',error,key=key);raise

    def mark_post_c63(self,key):
        try:return super().mark_post_c63(key)
        except c58.Refusal as error:
            self._observe('mark_post_c63',error,key=key);raise

    def mark_post_c65(self,key):
        try:return super().mark_post_c65(key)
        except c58.Refusal as error:
            self._observe('mark_post_c65',error,key=key);raise

    def mark_post_c67(self,key):
        try:return super().mark_post_c67(key)
        except c58.Refusal as error:
            self._observe('mark_post_c67',error,key=key);raise

    def instance(self,row,state,path):
        try:return super().instance(row,state,path)
        except c58.Refusal as error:
            self._observe('instance',error,row=row,state=state,path=path);raise

    def connect(self,source,target):
        try:return super().connect(source,target)
        except c58.Refusal as error:
            self._observe('connect',error,source=source,target=target);raise


def record_for(baseline,tx,view):
    if view.post67_capture is not None:
        return _original_record_for(baseline,tx,view)
    capture = view.post67_transaction_capture
    if capture is None:
        raise c58.EvidenceError('terminal c68 POST lacks edge or transaction origin capture')
    index = capture['row'];begin,end,code,_ = view.row(index)
    if (begin,end)!=(capture['begin'],capture['end']) or view.session.row_identity(index)!=capture['row_sha256'] or c56.sha(code)!=capture['code_sha256']:
        raise c58.EvidenceError('stale/wrong c68 transaction terminal row/context')
    auth = capture['c67_authorization']
    snapshot = dict(original_c68_prefix=c68.sibling_prefix(tx,view),considered=sorted(tx.considered),
        post_c67=sorted(tx.post_c67),post67_origins=[(k,c56.digest(v)) for k,v in sorted(tx.post67_origins.items())],
        c67_source_keys=sorted(view.c67_source_keys),c67_record=c56.digest(view.c67_record),c67_key=view.c67_key,
        c67_scan_token=view.c67_scan_token,c67_prefix=view.c67_prefix,c67_cache_identity=view.c67_cache_identity,
        c67_proofs=c56.digest(sorted(view.c67_proofs.items())),c67_siblings_used=sorted(view.c67_siblings_used),
        c67_considered_before=sorted(view.c67_considered_before),c67_considered_allowed=sorted(view.c67_considered_allowed),
        c67_considered=sorted(view.c67_considered),c67_eligible=sorted(view.c67_eligible),
        c67_edges_considered=sorted(view.c67_edges_considered),c67_edges_eligible=sorted(view.c67_edges_eligible),
        c67_refusals=dict(view.c67_refusals),c67_mismatches=dict(view.c67_mismatches),
        crossings=tx.crossings,c61_crossings=tx.c61_crossings,c63_crossings=tx.c63_crossings,
        c65_crossings=tx.c65_crossings,c67_crossings=tx.c67_crossings,
        transaction_capture=c56.digest({k:v for k,v in capture.items() if k!='state'}))
    prefix = c56.digest(snapshot)
    record = dict(classifier_identity=baseline.identity,**{k:v for k,v in baseline.header.items() if k.startswith('accepted_')},
        original_root_runtime_row=tx.root,original_root_row_sha256=view.session.row_identity(tx.root),
        current_entry_instance_row=index,incoming_state_identity=capture['state_identity'],
        entry_seed_fact_count=capture['entry_seed_fact_count'],checkpoint_incoming_state=capture['state'],
        entry_instance_identity=c56.digest((baseline.certificates.identity,tx.root,index,capture['state_identity'],capture['path']['boundary_path_identity'])),
        **capture['path'],source_runtime_row_sha256=None,source_code_sha256=None,source_begin_rva=None,source_end_rva=None,
        source_instruction_rva=None,source_instruction_size=None,source_instruction_sha256=None,source_mnemonic=None,
        terminal_context_runtime_row_sha256=capture['row_sha256'],terminal_context_code_sha256=capture['code_sha256'],
        terminal_context_begin_rva=begin,terminal_context_end_rva=end,first_refusal=c68.POST,
        terminal_event_kind=capture['event_kind'],terminal_event_edge_backed=False,
        terminal_event_context=capture['event_context'],terminal_phase=capture['phase'],
        terminal_reason=capture['terminal_provenance']['reason'],terminal_provenance=capture['terminal_provenance'],
        post_c67_origin=capture['post_c67_origin'],post_c65_origin=capture['post_c65_origin'],
        post_c63_origin=capture['post_c63_origin'],post_c61_origin=capture['post_c61_origin'],post_c59_origin=capture['post_c59_origin'],
        c67_authorization=auth,c67_terminal_phase=auth['terminal_phase'],c67_terminal_reason=auth['terminal_reason'],
        local_c67_edge_identities=capture['local_c67_edge_identities'],root_replay_identity=prefix,
        current_c68_continuation_identity=prefix,edges=[],state_scope='authenticated_entry_seed_transaction_terminal_no_instruction_must_state',
        new_post_c67_boundaries_admitted=0,new_candidates_emitted=0)
    record['record_identity'] = c56.digest(record)
    return record


class Aggregate(_OriginalAggregate):
    def __init__(self,*args,**kwargs):
        super().__init__(*args,**kwargs)
        self.terminal_events = Counter();self.transaction_causes = Counter();self.edge_backed = 0;self.transaction_backed = 0

    def add(self,record):
        super().add(record)
        kind = record.get('terminal_event_kind','edge_callback')
        edge_backed = record.get('terminal_event_edge_backed',True)
        self.terminal_events[kind] += 1
        if edge_backed:self.edge_backed += 1
        else:
            self.transaction_backed += 1
            cause = record.get('terminal_provenance',{}).get('underlying_refusal')
            self.transaction_causes[cause if cause is not None else 'direct_guard'] += 1

    def finish(self):
        result = super().finish()
        if self.edge_backed+self.transaction_backed!=result['post_c67_boundary_instances_total']:
            raise c58.EvidenceError('c69 terminal origin accounting mismatch')
        result.update(terminal_event_kind_counts=dict(sorted(self.terminal_events.items())),
            edge_backed_terminal_instances=self.edge_backed,transaction_terminal_instances=self.transaction_backed,
            transaction_terminal_underlying_refusal_counts=dict(sorted(self.transaction_causes.items())),
            terminal_origin_classification_complete=True,
            diagnostic_scope='unchanged_c68_first_terminal_POST_origin_per_root_edge_callback_or_transaction_guard_no_post_abort_exploration',
            post_c67_boundary_edges_scope='edge_backed_terminal_successors_only_transaction_terminals_have_no_invented_instruction_or_successor')
        return result


# Patch the already-corrected c69 module.  Its replay/classify/main functions use
# module globals at call time, so accepted c56-c68 logic remains untouched.
base.sources = sources
fix1.sources = sources
base.View = View
base.Transaction = Transaction
base.record_for = record_for
base.Aggregate = Aggregate


def main():
    return fix1.main()


if __name__=='__main__':
    raise SystemExit(main())
