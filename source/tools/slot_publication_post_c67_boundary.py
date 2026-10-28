"""c69 OFFLINE diagnostic of c68's exact first terminal post-c67 boundary.

Observe one unchanged c68 terminal edge invocation, capture exact current
c67/c68 provenance, classify successors, and rethrow the same refusal. No target
traversal, admission, new Fact, candidate, pointer/value read, native probe or game.

The optional checkpoint is deterministic JSON for future local analysis reuse.
It never weakens provenance checks and is not a writer/completeness certificate.
"""
import argparse
from collections import Counter
from copy import deepcopy
from dataclasses import asdict
import hashlib
import json
from pathlib import Path
import sys

import slot_publication_post_c65_stitched as c68

c67,c66,c65,c64,c63,c62,c61,c60,c59,c58,c57,c56,cfg = (
    c68.c67,c68.c66,c68.c65,c68.c64,c68.c63,c68.c62,c68.c61,c68.c60,c68.c59,c68.c58,c68.c57,c68.c56,c68.cfg
)
VERSION = 'v0.1c69'
SCHEMA = 1
CONTRACT = 'exact_c68_first_terminal_post_c67_boundary_full_prefix_provenance_diagnostic_v1'
READY = 'POST_C67_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY'
BLOCKED = 'POST_C67_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED'
EXPECTED_SHA = c68.EXPECTED_SHA
EXPECTED_C68_FILE_SHA = '47fee13729d1b2d30b1c3cffc9f4bc19837fc18f580d0c0daac909ea7056a246'
EXPECTED_C68_RESULT_IDENTITY = 'dbfcdefeccbc7bcaba048829336d945707f0fc4fc76b2c17780f33413b48255a'
CHECKPOINT_VERSION = 1
CHECKPOINT_CONTRACT = 'authenticated_c68_prefix_first_post_c67_records_v1'
PHASES = (
    'same_instance_after_c67_preflight_before_row_fixpoint',
    'post_c67_reachable_context',
    'post_c67_reachable_dedup_stored_earlier_path',
    'prior_c67_authorization_converted_earlier_refusal',
    'used_c67_source_on_later_scan',
)
LOWER_CAUSES = (
    c66.POST,c64.POST,c62.POST,c60.POST,c59.LATER,
    'c59_predecessor_path_conflict','c61_predecessor_path_conflict',
    'c63_predecessor_path_conflict','c65_predecessor_path_conflict',
)
ACCEPTED_C68 = {'accepted_c67_classifier_identity': 'a130b082512449cd008a912b94e2e3a2bcabf15140906c4780983bac07b99f20',
 'accepted_c67_record_set_identity': 'f0b2ce8c91868e572a83d59fc425c4e40d3fe4fe9faa7957e26a252b65e85ed6',
 'accepted_c67_result_identity': 'ac043225b8b4464e8fe7e52a2c44a9a81c72bf0c2fad6e30df173fdab83b113d',
 'address_facts_crossing_runtime_rows': 3161,
 'address_only_candidates': 0,
 'c56_through_c67_evidence_read_only': True,
 'c67_address_facts_crossing_rows': 3,
 'c67_all_successor_preflight_refusal_relation_counts': {'executable_file_backed_outside_runtime_metadata': 1,
                                                         'inside_unique_runtime_span_nonentry': 74},
 'c67_architectural_edges_available': 507,
 'c67_boundary_instances_admitted': 180,
 'c67_boundary_instances_available': 458,
 'c67_boundary_instances_considered': 458,
 'c67_boundary_instances_refused': 278,
 'c67_certificate_mismatch_counts': {},
 'c67_certified_crossings_with_facts': 3,
 'c67_exact_edges_admitted': 221,
 'c67_exact_edges_available': 432,
 'c67_exact_edges_considered': 432,
 'c67_exact_edges_refused': 211,
 'c67_terminal_phase_admitted': {'post_c65_reachable_context': 9,
                                 'post_c65_reachable_dedup_stored_earlier_path': 1,
                                 'prior_c65_authorization_converted_earlier_refusal': 5,
                                 'same_instance_after_c65_preflight_before_row_fixpoint': 165},
 'c67_terminal_phase_considered': {'post_c65_reachable_context': 34,
                                   'post_c65_reachable_dedup_stored_earlier_path': 3,
                                   'prior_c65_authorization_converted_earlier_refusal': 73,
                                   'same_instance_after_c65_preflight_before_row_fixpoint': 348},
 'c67_terminal_phase_refused': {'post_c65_reachable_context': 25,
                                'post_c65_reachable_dedup_stored_earlier_path': 2,
                                'prior_c65_authorization_converted_earlier_refusal': 68,
                                'same_instance_after_c65_preflight_before_row_fixpoint': 183},
 'c67_terminal_reason_admitted': {'current_context_post_c65_reachable': 10,
                                  'prior_c65_authorization_converted_c64_post': 5,
                                  'used_c65_source_later_instruction': 165},
 'c67_terminal_reason_considered': {'current_context_post_c65_reachable': 37,
                                    'prior_c65_authorization_converted_c64_post': 73,
                                    'used_c65_source_later_instruction': 348},
 'c67_terminal_reason_refused': {'current_context_post_c65_reachable': 27,
                                 'prior_c65_authorization_converted_c64_post': 68,
                                 'used_c65_source_later_instruction': 183},
 'contract': 'exact_c67_full_prefix_terminal_reason_provenance_all_successor_continuation_v1',
 'historical_c66_candidate_output_sha256': '4f53cda18c2baa0c0354bb5f9a3ecbe5ed12ab4d8e11ba873c2f11161202b945',
 'new_cross_row_candidate_count': 0,
 'post_c67_uncertified_boundary_refusals': 191,
 'reader_candidates': 0,
 'reference_candidate_count': 0,
 'repository_result': 'POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_READY',
 'rmw_candidates': 0,
 'root_transactions_attempted': 162472,
 'root_transactions_completed': 105721,
 'root_transactions_refused': 56751,
 'roots_completed': 105721,
 'rows_reached_by_continuation': 1750,
 'schema_version': 1,
 'tool_version': 'v0.1c68',
 'writer_candidates': 0}


def sources():
    return dict(c68.sources(),slot_publication_post_c67_boundary_py=c56.sha(Path(__file__).read_bytes()))


def evidence(certificates):
    return c67.evidence(certificates.baseline.certificates)


def read_c68(path):
    raw = Path(path).read_bytes()
    if len(raw)>c56.MAX_JSON or c56.sha(raw)!=EXPECTED_C68_FILE_SHA:
        raise c58.EvidenceError('stale/wrong physical c68 owner JSON')
    try:
        result = json.loads(raw)
    except (ValueError,TypeError) as error:
        raise c58.EvidenceError('invalid c68 owner JSON') from error
    if c56.digest(result)!=EXPECTED_C68_RESULT_IDENTITY:
        raise c58.EvidenceError('wrong semantic c68 owner result identity')
    return result


class Baseline:
    def __init__(self,certificates,*,owner_result=None,accepted=None,progress=None):
        self.certificates = certificates
        self.result = deepcopy(owner_result) if owner_result is not None else c68.scan(certificates,progress=progress)
        expected = ACCEPTED_C68 if accepted is None else accepted
        if any(c56.encoded(self.result.get(k))!=c56.encoded(v) for k,v in expected.items()):
            raise c58.EvidenceError('accepted c68 identity/count/result mismatch')
        if owner_result is not None and c56.encoded(owner_result)!=c56.encoded(self.result):
            raise c58.EvidenceError('stale/wrong image/tool/schema/contract/result c68 JSON')
        if (self.result['repository_result']!=c68.READY or self.result['tool_version']!=c68.VERSION
            or self.result['schema_version']!=c68.SCHEMA or self.result['contract']!=c68.CONTRACT
            or self.result['authentication_header']!=certificates.header
            or self.result['limits']!=asdict(c58.Limits()) or any(self.result[k] for k in c56.FALSE_GATES)
            or not self.result['c56_through_c67_evidence_read_only']):
            raise c58.EvidenceError('wrong c68 proof/contract/limits/gates')
        self.header = dict(tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_sha256=self.result['image_sha256'],c56_manifest_id=certificates.header['c56_manifest_id'],
            **{k:v for k,v in certificates.header.items() if k.startswith('accepted_')},
            accepted_c68_authentication_identity=certificates.identity,accepted_c68_result_identity=c56.digest(self.result),
            accepted_c68_physical_sha256=EXPECTED_C68_FILE_SHA,c68_contract=c68.CONTRACT,c68_limits=self.result['limits'],
            post_c67_uncertified_boundary_refused_roots_expected=self.result['post_c67_uncertified_boundary_refusals'],
            tool_sources=sources())
        self.identity = c56.digest(self.header)

    def assert_current(self):
        self.certificates.assert_current()
        if (self.header.get('tool_sources')!=sources() or c56.digest(self.header)!=self.identity
            or c56.digest(self.result)!=self.header['accepted_c68_result_identity']
            or self.certificates.identity!=self.header['accepted_c68_authentication_identity']
            or self.certificates.header!=self.result['authentication_header']):
            raise c58.EvidenceError('stale/mutated c69 schema/tool/contract/c68 result identity')


def path_summary(path):
    return dict(c67.path_summary(path),
        c67_record_identities=[p['c67_record_identity'] for p in path if p.get('proof_kind')=='c67_post_boundary'],
        c67_full_prefix_identities=[p['c67_full_prefix_identity'] for p in path if p.get('proof_kind')=='c67_post_boundary'])


def edge_before(tx,view,index,ins):
    auth = view.c67_record
    return dict(root=tx.root,key=view.key,index=index,pc=ins.address,size=ins.size,
        instruction_sha256=c56.sha(bytes(ins.bytes)),post_c67=view.key in tx.post_c67,
        used_c67_source=view.key in view.c67_source_keys,scan_token=tx.scan_token,
        authorization_scan_token=view.c67_scan_token,
        authorization_record_identity=auth['record_identity'] if auth else None,
        authorization_source_pc=auth['source_instruction_rva'] if auth else None,
        c67_mismatches=dict(view.c67_mismatches))


def _cause_reason(cause):
    if cause not in LOWER_CAUSES:
        raise c58.EvidenceError('unrecognized converted c68 lower refusal')
    labels = {
        c66.POST:'c66_post',c64.POST:'c64_post',c62.POST:'c62_post',c60.POST:'c60_post',c59.LATER:'pre_c59_later',
        'c59_predecessor_path_conflict':'c59_predecessor_path_conflict',
        'c61_predecessor_path_conflict':'c61_predecessor_path_conflict',
        'c63_predecessor_path_conflict':'c63_predecessor_path_conflict',
        'c65_predecessor_path_conflict':'c65_predecessor_path_conflict',
    }
    return 'prior_c67_authorization_converted_'+labels[cause]


def terminal_provenance(tx,view,index,ins):
    before = view.post67_edge_before
    if before is None:
        raise c58.EvidenceError('terminal c68 POST lacks original edge-entry snapshot')
    current = edge_before(tx,view,index,ins)
    if any(c56.encoded(before[k])!=c56.encoded(current[k]) for k in before if k!='c67_mismatches'):
        raise c58.EvidenceError('stale/wrong c68 terminal edge-entry context/source/proof')
    delta = {k:current['c67_mismatches'].get(k,0)-before['c67_mismatches'].get(k,0)
        for k in set(current['c67_mismatches'])|set(before['c67_mismatches'])
        if current['c67_mismatches'].get(k,0)!=before['c67_mismatches'].get(k,0)}
    cause = view.post67_underlying_refusal
    result = dict(edge_entry=deepcopy(before),c67_mismatch_delta=delta,underlying_refusal=cause)
    if before['post_c67']:
        reason = 'current_context_post_c67_reachable'
    elif before['used_c67_source'] and (before['pc']!=before['authorization_source_pc']
            or before['scan_token']!=before['authorization_scan_token']):
        reason = 'used_c67_source_later_scan' if before['scan_token']!=before['authorization_scan_token'] else 'used_c67_source_later_instruction'
    elif before['authorization_record_identity'] is not None and cause is not None:
        reason = _cause_reason(cause)
    else:
        raise c58.EvidenceError('unrecognized original c68 terminal edge provenance')
    if reason.startswith(('current_context','used_c67_source')) and (cause is not None or delta):
        raise c58.EvidenceError('direct c68 guard has inconsistent cause/counter delta')
    result['reason'] = reason
    return result


def serialize_state(state):
    return [None if f is None else dict(rva=f.rva,chain=list(f.chain)) for f in state]


def capture_for(tx,view,index,ins):
    key = view.key
    if key is None or key[0]!=index or key not in tx.contexts:
        raise c58.EvidenceError('terminal c68 POST lacks current source context')
    state,path = tx.contexts[key]
    if key[1]!=c58.state_id(state):
        raise c58.EvidenceError('wrong c68 terminal incoming state')
    auth = view.c67_record
    ordinal = view.c68_certificates.index.get(tx.root)
    if auth is None or ordinal is None or c56.encoded(auth)!=view.c68_certificates.records[ordinal]:
        raise c58.EvidenceError('terminal c68 POST lacks exact sealed c67 authorization at original root')
    provenance = terminal_provenance(tx,view,index,ins)
    reason = provenance['reason'];origin = tx.post67_origins.get(key)
    if reason=='current_context_post_c67_reachable':
        if origin is None:
            raise c58.EvidenceError('post-c67 reachable source lacks diagnostic origin')
        phase = PHASES[1] if any(p.get('proof_kind')=='c67_post_boundary' for p in path) else PHASES[2]
    elif reason=='used_c67_source_later_instruction':
        phase = PHASES[0]
    elif reason=='used_c67_source_later_scan':
        phase = PHASES[4]
    else:
        phase = PHASES[3]
        if key in tx.post_c67 or origin is not None:
            raise c58.EvidenceError('converted refusal falsely labeled post-c67 reachable')
    _,_,code,_ = view.row(index)
    auth_fields = ('record_identity','classifier_identity','root_replay_identity','entry_instance_identity',
        'current_entry_instance_row','incoming_state_identity','boundary_path_identity','boundary_transition_identities',
        'source_instruction_rva','source_instruction_sha256','terminal_phase','terminal_reason','terminal_provenance',
        'post_c59_origin','post_c61_origin','post_c63_origin','post_c65_origin','c65_authorization',
        'c65_terminal_phase','c65_terminal_reason','local_c65_edge_identities')
    return dict(root=tx.root,row=index,state_identity=key[1],state=serialize_state(state),path=path_summary(path),
        entry_seed_fact_count=sum(f is not None for f in state),pc=ins.address,size=ins.size,
        instruction_sha256=c56.sha(bytes(ins.bytes)),row_sha256=view.session.row_identity(index),code_sha256=c56.sha(code),
        phase=phase,terminal_provenance=provenance,post_c67_origin=deepcopy(origin),
        post_c65_origin=deepcopy(getattr(tx,'post65_origins',{}).get(key)),
        post_c63_origin=deepcopy(getattr(tx,'post63_origins',{}).get(key)),
        post_c61_origin=deepcopy(getattr(tx,'post61_origins',{}).get(key)),
        post_c59_origin=deepcopy(getattr(tx,'post_origins',{}).get(key)),
        c67_authorization={k:deepcopy(auth[k]) for k in auth_fields},
        local_c67_edge_identities=sorted(p['edge_identity'] for p in view.c67_proofs.values()),
        c67_source_contexts=sorted(view.c67_source_keys),c67_scan_token=view.c67_scan_token,current_scan_token=tx.scan_token)


class View(c68.View):
    def __init__(self,certificates):
        super().__init__(certificates)
        self.post67_capture,self.post67_edge_before,self.post67_underlying_refusal = None,None,None

    def edge(self,index,ins,kind,target,architectural,relations):
        before = edge_before(self.tx,self,index,ins) if self.tx is not None else None
        try:
            return super().edge(index,ins,kind,target,architectural,relations)
        except c58.Refusal as error:
            if str(error)==c68.POST:
                if self.tx is None:
                    raise c58.EvidenceError('missing c68 terminal transaction')
                self.post67_edge_before = before
                self.post67_underlying_refusal = str(error.__cause__) if error.__cause__ is not None else None
                capture = capture_for(self.tx,self,index,ins)
                if self.post67_capture is not None and c56.encoded(self.post67_capture)!=c56.encoded(capture):
                    raise c58.EvidenceError('multiple distinct first terminal c68 source events')
                self.post67_capture = capture
            raise


class Transaction(c68.Transaction):
    """Shadow c68's already-metered post-c67 closure without changing it."""
    def __init__(self,view,root,engine,limits):
        super().__init__(view,root,engine,limits)
        self.post67_origins = {};self.post67_arrival = self.post67_connect = None

    def instance(self,row,state,path):
        before = self.post67_arrival;self.post67_arrival = ((row,c58.state_id(state)),path)
        try:
            return super().instance(row,state,path)
        finally:
            self.post67_arrival = before

    def connect(self,source,target):
        before = self.post67_connect;self.post67_connect = (source,target)
        try:
            return super().connect(source,target)
        finally:
            self.post67_connect = before

    def mark_post_c67(self,key):
        before = set(self.post_c67)
        super().mark_post_c67(key)
        added = self.post_c67-before
        if not added and key in self.post67_origins:
            return
        if self.post67_arrival is not None and self.post67_arrival[0]==key:
            origin = dict(kind='authenticated_c67_arrival',entry_context=key,
                arrival_path=path_summary(self.post67_arrival[1]),propagation_contexts=[key])
            if not origin['arrival_path']['c67_record_identities']:
                raise c58.EvidenceError('post-c67 arrival lacks c67 proof')
        elif self.post67_connect is not None and self.post67_connect[1]==key:
            source,_ = self.post67_connect
            if source not in self.post67_origins:
                raise c58.EvidenceError('post-c67 propagation lacks source origin')
            origin = deepcopy(self.post67_origins[source]);origin['propagation_contexts'].append(key)
        else:
            raise c58.EvidenceError('unexpected original c68 post-c67 marking call path')
        if len(origin['propagation_contexts'])>self.limits.instances:
            raise c58.EvidenceError('bounded diagnostic post-c67 origin cap')
        for target in sorted(added):
            self.post67_origins[target] = deepcopy(origin)
        self.post67_origins.setdefault(key,origin)


def record_for(baseline,tx,view):
    capture = view.post67_capture
    if capture is None:
        raise c58.EvidenceError('terminal c68 POST without source edge callback; diagnostic source unavailable')
    s = view.session;index = capture['row'];begin,end,code,_ = view.row(index);pc,size = capture['pc'],capture['size']
    if not begin<=pc<end or not 1<=size<=15:
        raise c58.EvidenceError('invalid c68 terminal source span')
    ins = next(cfg.old.decoder().disasm(code[pc-begin:pc-begin+size],pc,count=1),None)
    if ins is None or ins.size!=size or c56.encoded(capture)!=c56.encoded(capture_for(tx,view,index,ins)):
        raise c58.EvidenceError('stale/wrong c68 terminal root/state/path/source/phase/provenance')
    allowed = lambda target:any(b<=target<min(e,b+n) and flags&0x60000000==0x60000000 and not flags&0x80000000
        for b,e,_,n,flags in s.sections)
    try:
        cfg.successors(ins,begin,end,s.size,allowed)
    except ValueError as error:
        if str(error)!='unsupported_runtime_boundary':
            raise c58.EvidenceError('c68 terminal source not original boundary oracle refusal') from error
    else:
        raise c58.EvidenceError('c68 terminal source has no outgoing boundary')
    edges = c59.classify_edges(s,begin,end,ins)
    if not 1<=len(edges)<=2:
        raise c58.EvidenceError('missing exact c68 terminal successors')
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
        c65_crossings=tx.c65_crossings,c67_crossings=tx.c67_crossings,capture={k:v for k,v in capture.items() if k!='state'})
    prefix = c56.digest(snapshot)
    record = dict(classifier_identity=baseline.identity,**{k:v for k,v in baseline.header.items() if k.startswith('accepted_')},
        original_root_runtime_row=tx.root,original_root_row_sha256=s.row_identity(tx.root),current_entry_instance_row=index,
        incoming_state_identity=capture['state_identity'],entry_seed_fact_count=capture['entry_seed_fact_count'],
        checkpoint_incoming_state=capture['state'],
        entry_instance_identity=c56.digest((baseline.certificates.identity,tx.root,index,capture['state_identity'],capture['path']['boundary_path_identity'])),
        **capture['path'],source_runtime_row_sha256=capture['row_sha256'],source_code_sha256=capture['code_sha256'],
        source_begin_rva=begin,source_end_rva=end,source_instruction_rva=pc,source_instruction_size=size,
        source_instruction_sha256=capture['instruction_sha256'],source_mnemonic=ins.mnemonic,first_refusal=c68.POST,
        terminal_phase=capture['phase'],terminal_reason=capture['terminal_provenance']['reason'],
        terminal_provenance=capture['terminal_provenance'],post_c67_origin=capture['post_c67_origin'],
        post_c65_origin=capture['post_c65_origin'],post_c63_origin=capture['post_c63_origin'],
        post_c61_origin=capture['post_c61_origin'],post_c59_origin=capture['post_c59_origin'],
        c67_authorization=capture['c67_authorization'],c67_terminal_phase=capture['c67_authorization']['terminal_phase'],
        c67_terminal_reason=capture['c67_authorization']['terminal_reason'],
        local_c67_edge_identities=capture['local_c67_edge_identities'],root_replay_identity=prefix,
        current_c68_continuation_identity=prefix,edges=edges,state_scope='authenticated_entry_seed_not_source_instruction_must_state',
        new_post_c67_boundaries_admitted=0,new_candidates_emitted=0)
    record['record_identity'] = c56.digest(record)
    return record


class Audit:
    """Match c68 dynamic accounting and retained candidate bytes during observation."""
    def __init__(self):
        self.old = c59.ReplayAudit()
        self.considered,self.admitted,self.edges_considered,self.edges_admitted = set(),set(),set(),set()
        self.preflight,self.mismatches,self.extra,self.refs,self.by_id = Counter(),Counter(),Counter(),[],{}

    def add(self,tx,view,reason,certificates):
        self.old.add(tx,reason);self.considered.update(view.c67_considered);self.edges_considered.update(view.c67_edges_considered)
        self.preflight.update(view.c67_refusals);self.mismatches.update(view.c67_mismatches)
        record = certificates.lookup(tx.root)
        if record is not None:
            self.by_id[record['record_identity']] = record
        if reason is None:
            self.admitted.update(view.c67_eligible);self.edges_admitted.update(view.c67_edges_eligible)
            self.extra['crossings'] += len(tx.c67_crossings);self.extra['facts'] += sum(n for _,n in tx.c67_crossings)
            for original in tx.references:
                ref = deepcopy(original);path = ref['boundary_path']
                if view.c67_eligible:
                    ref.update(c68_authentication_identity=certificates.identity,
                        c68_entry_instance_identity=c56.digest((certificates.identity,tx.root,ref['continuation_runtime_row'],
                            ref['incoming_state_identity'],c56.digest(path))),
                        path_c67_record_identities=[p['c67_record_identity'] for p in path if p.get('proof_kind')=='c67_post_boundary'],
                        root_c67_record_identities=sorted(view.c67_eligible))
                if len(self.refs)<c58.Limits().retained_references:
                    self.refs.append(ref)

    def finish(self,original):
        t,ops = self.old.totals,self.old.ops;count = sum(ops.values())
        grouped = lambda ids,field:dict(sorted(Counter(self.by_id[i][field] for i in ids).items()))
        projection = dict(c67_boundary_instances_considered=len(self.considered),c67_boundary_instances_admitted=len(self.admitted),
            c67_boundary_instances_refused=len(self.considered-self.admitted),c67_exact_edges_considered=len(self.edges_considered),
            c67_exact_edges_admitted=len(self.edges_admitted),c67_exact_edges_refused=len(self.edges_considered-self.edges_admitted),
            c67_terminal_phase_considered=grouped(self.considered,'terminal_phase'),
            c67_terminal_phase_admitted=grouped(self.admitted,'terminal_phase'),
            c67_terminal_phase_refused=grouped(self.considered-self.admitted,'terminal_phase'),
            c67_terminal_reason_considered=grouped(self.considered,'terminal_reason'),
            c67_terminal_reason_admitted=grouped(self.admitted,'terminal_reason'),
            c67_terminal_reason_refused=grouped(self.considered-self.admitted,'terminal_reason'),
            c67_all_successor_preflight_refusal_relation_counts=dict(sorted(self.preflight.items())),
            c67_certificate_mismatch_counts=dict(sorted(self.mismatches.items())),
            c67_certified_crossings_with_facts=self.extra['crossings'],c67_address_facts_crossing_rows=self.extra['facts'],
            root_transactions_attempted=t['root_transactions_attempted'],root_transactions_completed=t['roots_completed'],
            roots_completed=t['roots_completed'],root_transactions_refused=sum(self.old.reasons.values()),
            root_refusal_reasons=dict(sorted(self.old.reasons.items())),post_c67_uncertified_boundary_refusals=self.old.reasons[c68.POST],
            rows_reached_by_continuation=len(self.old.rows),
            remaining_boundary_refusal_relation_counts={k:self.old.relations[k] for k in c57.RELATIONS},
            reference_candidate_count=count,new_cross_row_candidate_count=count,reader_candidates=ops['READ'],
            writer_candidates=ops['WRITE'],rmw_candidates=ops['RMW'],address_only_candidates=ops['ADDRESS_ONLY'],
            reference_candidates=self.refs,candidate_records_not_retained=count-len(self.refs))
        for k in ('continuation_instances_created','continuation_instances_deduplicated','continuation_instances_refused',
            'cross_row_cap_refusals','cross_row_cycles_seen','address_facts_crossing_runtime_rows',
            'call_edges_with_preserved_facts_crossing_rows','no_carried_fact_edges'):
            projection[k] = t[k]
        if any(c56.encoded(original.get(k))!=c56.encoded(v) for k,v in projection.items()):
            raise c58.EvidenceError('observer changed original c68 accounting/candidate bytes')


def replay_records(baseline,progress=None):
    baseline.assert_current();certificates = baseline.certificates;e = evidence(certificates);audit = Audit();engine = shard = None;count = 0
    for number,root in enumerate(e.proofs,1):
        current = root // e.session.profile.rows_per_shard
        if current!=shard:
            engine = c58.c55.Engine(e.session.domain,e.session.profile.summary);shard = current
        view = View(certificates);tx = Transaction(view,root,engine,c58.Limits());reason = None
        try:
            tx.run()
        except c58.EvidenceError:
            raise
        except (c58.Refusal,ValueError) as error:
            reason = str(error)
        audit.add(tx,view,reason,certificates)
        if reason==c68.POST:
            count += 1;yield record_for(baseline,tx,view)
        elif view.post67_capture is not None:
            raise c58.EvidenceError('c68 POST capture not terminal root outcome')
        if progress and (number%1024==0 or number==len(e.proofs)):
            progress('c69 unchanged c68 terminal observer replay',number,len(e.proofs))
    audit.finish(baseline.result)
    if count!=baseline.header['post_c67_uncertified_boundary_refused_roots_expected']:
        raise c58.EvidenceError('missing accepted c68 POST root domain')
    baseline.assert_current()


class Aggregate:
    def __init__(self,baseline,expected_records,example_limit=8):
        if type(example_limit)!=int or not 0<=example_limit<=8:
            raise ValueError('invalid example bound')
        self.baseline,self.limit,self.expected = baseline,example_limit,iter(expected_records);self.poisoned = False
        self.roots,self.instances,self.states,self.rows = set(),set(),set(),set();self.count = self.edges = self.model_only = 0
        self.kinds,self.relations,self.exact,self.interior,self.free,self.adjacent,self.phases,self.reasons,self.c67_phases,self.c67_reasons,self.causes = (Counter() for _ in range(11))
        self.examples_kind = {k:[] for k in c57.KINDS};self.examples_relation = {k:[] for k in c57.RELATIONS}
        self.records_sha = hashlib.sha256()

    def add(self,record):
        if self.poisoned:
            raise c58.EvidenceError('c69 aggregate already poisoned')
        try:
            expected = next(self.expected,None)
            if expected is None or type(record)!=dict or c56.encoded(record)!=c56.encoded(expected):
                raise c58.EvidenceError('missing/duplicate/out-of-order/stale/wrong c69 root/state/path/source/prefix/provenance record')
            root = record['original_root_runtime_row']
            if root in self.roots:
                raise c58.EvidenceError('multiple c69 records for one original terminal root')
            self.count += 1;self.roots.add(root);self.instances.add(record['record_identity'])
            self.states.add((root,record['current_entry_instance_row'],record['incoming_state_identity'],record['boundary_path_identity']))
            self.rows.add(record['current_entry_instance_row']);self.phases[record['terminal_phase']] += 1;self.reasons[record['terminal_reason']] += 1
            self.c67_phases[record['c67_terminal_phase']] += 1;self.c67_reasons[record['c67_terminal_reason']] += 1
            cause = record['terminal_provenance']['underlying_refusal']
            if cause is not None:
                self.causes[cause] += 1
            self.records_sha.update(c56.encoded(record)+b'\n')
            for edge in record['edges']:
                kind,relation = edge['edge_kind'],edge['target_metadata_relation'];self.edges += 1;self.model_only += not edge['architectural_successor']
                self.kinds[kind] += 1;self.relations[relation] += 1
                if relation=='exact_unique_runtime_begin':
                    self.exact[kind] += 1
                if relation=='inside_unique_runtime_span_nonentry':
                    self.interior[kind] += 1
                if relation=='executable_file_backed_outside_runtime_metadata':
                    self.free[kind] += 1
                if edge['exact_adjacency'] is True:
                    self.adjacent[kind] += 1
                example = {k:v for k,v in record.items() if k not in ('edges','checkpoint_incoming_state')};example.update(edge)
                if len(self.examples_kind[kind])<self.limit:
                    self.examples_kind[kind].append(example)
                if len(self.examples_relation[relation])<self.limit:
                    self.examples_relation[relation].append(example)
        except (ValueError,KeyError,TypeError,IndexError,RecursionError):
            self.poisoned = True;raise

    def finish(self):
        try:
            b = self.baseline
            if self.poisoned or next(self.expected,None) is not None:
                raise c58.EvidenceError('poisoned/missing c69 root record')
            b.assert_current()
            if (len(self.roots)!=b.header['post_c67_uncertified_boundary_refused_roots_expected'] or self.count!=len(self.instances)
                or self.count!=len(self.states) or sum(self.kinds.values())!=self.edges or sum(self.relations.values())!=self.edges):
                raise c58.EvidenceError('c69 diagnostic count mismatch')
            return dict(repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
                evidence='OFFLINE_STATIC_DIAGNOSTIC_ONLY',image_authenticated=True,image_sha256=b.header['image_sha256'],
                classifier_header=b.header,classifier_identity=b.identity,
                **{k:v for k,v in b.header.items() if k.startswith('accepted_')},
                post_c67_uncertified_boundary_refused_roots_expected=b.header['post_c67_uncertified_boundary_refused_roots_expected'],
                post_c67_uncertified_boundary_refused_roots_classified=len(self.roots),post_c67_boundary_instances_total=self.count,
                unique_entry_instance_states=len(self.states),unique_source_rows=len(self.rows),post_c67_boundary_edges_total=self.edges,
                architectural_post_c67_boundary_edges_total=self.edges-self.model_only,model_only_post_c67_boundary_edges_total=self.model_only,
                edge_kind_counts={k:self.kinds[k] for k in c57.KINDS},target_metadata_relation_counts={k:self.relations[k] for k in c57.RELATIONS},
                exact_runtime_begin_edge_kind_counts={k:self.exact[k] for k in c57.KINDS},
                interior_target_edge_kind_counts={k:self.interior[k] for k in c57.KINDS},
                metadata_free_RX_edge_kind_counts={k:self.free[k] for k in c57.KINDS},
                exact_adjacency_counts={k:self.adjacent[k] for k in c57.KINDS},
                terminal_phase_counts=dict(sorted(self.phases.items())),terminal_reason_counts=dict(sorted(self.reasons.items())),
                underlying_refusal_counts=dict(sorted(self.causes.items())),
                c67_authorization_phase_counts=dict(sorted(self.c67_phases.items())),
                c67_authorization_reason_counts=dict(sorted(self.c67_reasons.items())),
                authenticated_record_set_sha256=self.records_sha.hexdigest(),examples_per_class=self.limit,
                examples_by_edge_kind=self.examples_kind,examples_by_target_relation=self.examples_relation,
                classification_complete=True,new_post_c67_boundaries_admitted=0,cross_runtime_fact_propagation_unchanged=True,
                c68_candidate_output_unchanged=True,c68_candidate_output_sha256=c56.digest(b.result['reference_candidates']),
                historical_c68_candidate_counts={k:b.result[k] for k in ('reference_candidate_count','new_cross_row_candidate_count',
                    'reader_candidates','writer_candidates','rmw_candidates','address_only_candidates')},
                new_cross_row_candidate_count=0,publication_source_kind='unproven',live_qword_reads=0,
                state_scope='entry_seed_not_instruction_must_state_stored_path_separate_from_post_origin',
                diagnostic_scope='unchanged_c68_first_terminal_POST_edge_callback_per_root_no_post_abort_exploration',
                post_c67_boundary_edges_scope='row_external_terminal_successors_unchanged_c57_vocabulary',
                c56_through_c68_evidence_read_only=True,c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
                **{k:False for k in c56.FALSE_GATES})
        except (ValueError,KeyError,TypeError,IndexError,RecursionError):
            self.poisoned = True;raise


def classify(baseline,progress=None):
    records = list(replay_records(baseline,progress))
    aggregate = Aggregate(baseline,records)
    for record in records:
        aggregate.add(record)
    return aggregate.finish(),records


def make_checkpoint(baseline,result,records):
    record_stream_sha = hashlib.sha256()
    for record in records:
        record_stream_sha.update(c56.encoded(record)+b'\n')
    header = dict(checkpoint_version=CHECKPOINT_VERSION,checkpoint_contract=CHECKPOINT_CONTRACT,
        image_sha256=baseline.header['image_sha256'],accepted_c68_physical_sha256=EXPECTED_C68_FILE_SHA,
        accepted_c68_result_identity=EXPECTED_C68_RESULT_IDENTITY,accepted_c68_authentication_identity=baseline.header['accepted_c68_authentication_identity'],
        c69_classifier_identity=baseline.identity,c69_result_identity=c56.digest(result),
        record_count=len(records),record_stream_sha256=record_stream_sha.hexdigest(),limits=asdict(c58.Limits()),tool_sources=sources())
    payload = dict(header=header,result=result,records=records)
    payload['checkpoint_identity'] = c56.digest(payload)
    return payload


def validate_checkpoint(value):
    if type(value)!=dict or set(('header','result','records','checkpoint_identity'))-set(value):
        raise c58.EvidenceError('invalid c69 checkpoint shape')
    identity = value['checkpoint_identity'];body = {k:v for k,v in value.items() if k!='checkpoint_identity'}
    if c56.digest(body)!=identity:
        raise c58.EvidenceError('corrupt c69 checkpoint identity')
    header,result,records = value['header'],value['result'],value['records']
    if (header.get('checkpoint_version')!=CHECKPOINT_VERSION or header.get('checkpoint_contract')!=CHECKPOINT_CONTRACT
        or header.get('image_sha256')!=EXPECTED_SHA or header.get('accepted_c68_physical_sha256')!=EXPECTED_C68_FILE_SHA
        or header.get('accepted_c68_result_identity')!=EXPECTED_C68_RESULT_IDENTITY or header.get('tool_sources')!=sources()
        or header.get('limits')!=asdict(c58.Limits()) or result.get('repository_result')!=READY
        or result.get('tool_version')!=VERSION or result.get('schema_version')!=SCHEMA or result.get('contract')!=CONTRACT
        or c56.digest(result)!=header.get('c69_result_identity') or len(records)!=header.get('record_count')
        or result.get('post_c67_boundary_instances_total')!=len(records)):
        raise c58.EvidenceError('stale/wrong c69 checkpoint header/result')
    stream = hashlib.sha256()
    for record in records:
        if type(record)!=dict or c56.digest({k:v for k,v in record.items() if k!='record_identity'})!=record.get('record_identity'):
            raise c58.EvidenceError('stale/wrong c69 checkpoint record')
        stream.update(c56.encoded(record)+b'\n')
    if stream.hexdigest()!=header.get('record_stream_sha256') or stream.hexdigest()!=result.get('authenticated_record_set_sha256'):
        raise c58.EvidenceError('wrong c69 checkpoint record set identity')
    return value


def read_checkpoint(path):
    raw = Path(path).read_bytes()
    if len(raw)>c56.MAX_JSON:
        raise c58.EvidenceError('c69 checkpoint file cap')
    try:
        value = json.loads(raw)
    except (ValueError,TypeError) as error:
        raise c58.EvidenceError('invalid c69 checkpoint JSON') from error
    return validate_checkpoint(value)


def analyze(data,cache=None,c57_json=None,c58_json=None,c59_json=None,c60_json=None,c61_json=None,c62_json=None,
        c63_json=None,c64_json=None,c65_json=None,c66_json=None,c67_json=None,c68_json=None,progress=None):
    if len(data)>cfg.old.MAX_FILE or c56.sha(data)!=EXPECTED_SHA:
        raise c58.EvidenceError('exact executable SHA256 mismatch or input cap')
    s = c56.Session(data);old = c57.collect_baseline(s,cache,progress=progress)
    e = c58.Evidence(old,owner_result=c56.read_json(c57_json) if c57_json is not None else None,progress=progress)
    b = c59.Baseline(e,owner_result=c56.read_json(c58_json) if c58_json is not None else None,progress=progress)
    p = c60.ProofPool(b,owner_result=c56.read_json(c59_json) if c59_json is not None else None,progress=progress)
    original = c61.Baseline(p,owner_result=c56.read_json(c60_json) if c60_json is not None else None,progress=progress)
    cert = c62.ProofPool(original,owner_result=c56.read_json(c61_json) if c61_json is not None else None,progress=progress)
    prior = c63.Baseline(cert,owner_result=c56.read_json(c62_json) if c62_json is not None else None,progress=progress)
    cert64 = c64.ProofPool(prior,owner_result=c56.read_json(c63_json) if c63_json is not None else None,progress=progress)
    base65 = c65.Baseline(cert64,owner_result=c56.read_json(c64_json) if c64_json is not None else None,progress=progress)
    cert66 = c66.ProofPool(base65,owner_result=c56.read_json(c65_json) if c65_json is not None else None,progress=progress)
    owner66 = c67.read_c66(c66_json) if c66_json is not None else None
    base67 = c67.Baseline(cert66,owner_result=owner66,progress=progress)
    owner67 = c68.read_c67(c67_json) if c67_json is not None else None
    cert68 = c68.ProofPool(base67,owner_result=owner67,progress=progress)
    owner68 = read_c68(c68_json) if c68_json is not None else None
    baseline = Baseline(cert68,owner_result=owner68,progress=progress)
    result,records = classify(baseline,progress)
    return result,make_checkpoint(baseline,result,records)


def main():
    parser = argparse.ArgumentParser(description=__doc__);parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--checkpoint-output',type=Path)
    for flag in ('c56-cache','c57-json','c58-json','c59-json','c60-json','c61-json','c62-json','c63-json','c64-json','c65-json','c66-json','c67-json','c68-json'):
        parser.add_argument('--'+flag,type=Path)
    a = parser.parse_args()
    try:
        output = a.output.resolve();checkpoint = a.checkpoint_output.resolve() if a.checkpoint_output else None
        inputs = [p for p in (a.exe,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,a.c62_json,a.c63_json,a.c64_json,
            a.c65_json,a.c66_json,a.c67_json,a.c68_json) if p is not None]
        if any(output==p.resolve() for p in inputs) or (a.c56_cache and output.is_relative_to(a.c56_cache.resolve())):
            raise c58.EvidenceError('output overlaps immutable input evidence')
        if checkpoint is not None and (checkpoint==output or any(checkpoint==p.resolve() for p in inputs)
            or (a.c56_cache and checkpoint.is_relative_to(a.c56_cache.resolve()))):
            raise c58.EvidenceError('checkpoint overlaps immutable input/output evidence')
        if a.exe.stat().st_size>cfg.old.MAX_FILE:
            raise c58.EvidenceError('input file cap')
        result,checkpoint_value = analyze(a.exe.read_bytes(),a.c56_cache,a.c57_json,a.c58_json,a.c59_json,a.c60_json,
            a.c61_json,a.c62_json,a.c63_json,a.c64_json,a.c65_json,a.c66_json,a.c67_json,a.c68_json,
            lambda phase,n,total:print(f'{phase}: {n}/{total}',file=sys.stderr))
        c56.atomic_json(a.output,result)
        if a.checkpoint_output is not None:
            validate_checkpoint(checkpoint_value);c56.atomic_json(a.checkpoint_output,checkpoint_value)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(f'{BLOCKED}: {error}',file=sys.stderr);return 2
    print(f'{READY}: {a.output}')
    if a.checkpoint_output is not None:
        print(f'C69_AUTHENTICATED_PREFIX_CHECKPOINT_READY: {a.checkpoint_output}')
    return 0


if __name__=='__main__':
    raise SystemExit(main())
