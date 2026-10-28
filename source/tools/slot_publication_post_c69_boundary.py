"""c71 OFFLINE diagnostic of c70's exact first terminal post-c69 boundary.

C71 replays accepted c70 unchanged, observes only roots whose terminal outcome is
``uncertified_post_c69_runtime_boundary``, records exact edge-backed or
transaction-level provenance, classifies architectural successors when an
instruction exists, and never traverses the c71 target.
"""
import argparse
from collections import Counter
from copy import deepcopy
from dataclasses import asdict
import hashlib
import json
from pathlib import Path
import sys

import slot_publication_post_c69_stitched_fix as c70fix

c70 = c70fix.base
c69,c68,c67,c66,c65,c64,c63,c62,c61,c60,c59,c58,c57,c56,cfg = (
    c70.c69,c70.c68,c70.c67,c70.c66,c70.c65,c70.c64,c70.c63,c70.c62,
    c70.c61,c70.c60,c70.c59,c70.c58,c70.c57,c70.c56,c70.cfg
)
VERSION = 'v0.1c71'
SCHEMA = 1
CONTRACT = 'exact_c70_first_terminal_post_c69_boundary_full_prefix_provenance_diagnostic_v1'
READY = 'POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY'
BLOCKED = 'POST_C69_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED'
EXPECTED_SHA = c70.EXPECTED_SHA
EXPECTED_C70_FILE_SHA = '2780ca89a98ad22d12e2252f53cd202830bc8228edd67d0b0c7fb8eccf7f15ac'
EXPECTED_C70_RESULT_IDENTITY = '9b213db28b05f9d1fc8baefa37f981aa521cd7ee45110a7cfe6b122567f0b9e1'
EXPECTED_TERMINALS = 102
CHECKPOINT_VERSION = 1
CHECKPOINT_CONTRACT = 'authenticated_c70_prefix_first_post_c69_records_v1'

_OriginalView = c70.View
_OriginalTransaction = c70.Transaction


def sources():
    return dict(c70.sources(),slot_publication_post_c69_boundary_py=c56.sha(Path(__file__).read_bytes()))


def read_c70(path):
    raw = Path(path).read_bytes()
    if len(raw)>c56.MAX_JSON or c56.sha(raw)!=EXPECTED_C70_FILE_SHA:
        raise c58.EvidenceError('stale/wrong physical c70 owner JSON')
    try:
        result = json.loads(raw)
    except (ValueError,TypeError) as error:
        raise c58.EvidenceError('invalid c70 owner JSON') from error
    if c56.digest(result)!=EXPECTED_C70_RESULT_IDENTITY:
        raise c58.EvidenceError('wrong semantic c70 owner result identity')
    return result


def path_summary(path):
    result = dict(c69.path_summary(path))
    result.update(
        c69_record_identities=[p['c69_record_identity'] for p in path if p.get('proof_kind')=='c69_post_boundary'],
        c69_edge_identities=[p['c69_edge_identity'] for p in path if p.get('proof_kind')=='c69_post_boundary'],
        c69_full_prefix_identities=[p['c69_full_prefix_identity'] for p in path if p.get('proof_kind')=='c69_post_boundary'])
    return result


def _c69_record_summary(record):
    if record is None:
        return None
    fields = ('record_identity','classifier_identity','root_replay_identity','entry_instance_identity',
        'current_entry_instance_row','incoming_state_identity','boundary_path_identity',
        'source_instruction_rva','source_instruction_sha256','terminal_phase','terminal_reason',
        'terminal_event_kind','terminal_event_edge_backed','c67_terminal_phase','c67_terminal_reason')
    return {k:deepcopy(record.get(k)) for k in fields}


class Baseline:
    def __init__(self,certificates,owner_result,*,production=True):
        certificates.assert_current()
        if type(owner_result)!=dict:
            raise c58.EvidenceError('missing c70 owner result')
        if (owner_result.get('repository_result')!=c70.READY or owner_result.get('tool_version')!=c70.VERSION
            or owner_result.get('schema_version')!=c70.SCHEMA or owner_result.get('contract')!=c70.CONTRACT
            or owner_result.get('authentication_header')!=certificates.header
            or owner_result.get('limits')!=asdict(c58.Limits())
            or owner_result.get('full_historical_replay_used') is not True
            or owner_result.get('checkpoint_accelerated_resume_used') is not False
            or owner_result.get('c56_through_c69_evidence_read_only') is not True
            or any(owner_result.get(k) for k in c56.FALSE_GATES)):
            raise c58.EvidenceError('wrong/incomplete c70 owner proof contract')
        expected = owner_result.get('post_c69_uncertified_boundary_refusals')
        if type(expected)!=int or expected<0:
            raise c58.EvidenceError('invalid c70 post-c69 boundary count')
        if production and (c56.digest(owner_result)!=EXPECTED_C70_RESULT_IDENTITY or expected!=EXPECTED_TERMINALS):
            raise c58.EvidenceError('wrong accepted production c70 identity/domain')
        self.certificates,self.result,self.production = certificates,deepcopy(owner_result),production
        self.header = dict(tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_sha256=owner_result['image_sha256'],c56_manifest_id=certificates.header['c56_manifest_id'],
            **{k:v for k,v in certificates.header.items() if k.startswith('accepted_')},
            accepted_c70_authentication_identity=certificates.identity,
            accepted_c70_result_identity=c56.digest(owner_result),
            accepted_c70_physical_sha256=EXPECTED_C70_FILE_SHA if production else None,
            c70_contract=c70.CONTRACT,c70_limits=owner_result['limits'],
            post_c69_uncertified_boundary_refused_roots_expected=expected,
            full_historical_replay_used=True,checkpoint_accelerated_resume_used=False,
            tool_sources=sources())
        self.identity = c56.digest(self.header)

    def assert_current(self):
        self.certificates.assert_current()
        if (self.header.get('tool_sources')!=sources() or c56.digest(self.header)!=self.identity
            or c56.digest(self.result)!=self.header['accepted_c70_result_identity']
            or self.certificates.identity!=self.header['accepted_c70_authentication_identity']
            or self.result.get('authentication_header')!=self.certificates.header):
            raise c58.EvidenceError('stale/mutated c71 schema/tool/contract/c70 result identity')


def _context(tx,view,*,key=None,row=None,state=None,path=None,source=None,target=None):
    if row is not None and state is not None and path is not None:
        sid = c58.state_id(state)
        return (row,sid),state,path
    for candidate in (key,target,source,getattr(view,'key',None)):
        if candidate is not None and candidate in tx.contexts:
            current_state,current_path = tx.contexts[candidate]
            if candidate[1]!=c58.state_id(current_state):
                raise c58.EvidenceError('wrong c70 transaction terminal incoming state')
            return candidate,current_state,current_path
    raise c58.EvidenceError('c70 transaction terminal lacks exact current context')


def _terminal_common(tx,view,context_key,state,path,event_kind,edge_backed,provenance):
    row = context_key[0];begin,end,code,_ = view.row(row);summary = path_summary(path)
    origin = deepcopy(getattr(tx,'post69_origins',{}).get(context_key))
    path_ids = summary['c69_record_identities']
    origin_ids = [] if origin is None else list(origin.get('c69_record_identities',()))
    active = _c69_record_summary(getattr(view,'c69_record',None))
    if not path_ids and not origin_ids and active is None:
        raise c58.EvidenceError('post-c69 terminal lacks c69 authorization provenance')
    snapshot = dict(original_c70_prefix=c68.sibling_prefix(tx,view),considered=sorted(tx.considered),
        post_c69=sorted(getattr(tx,'post_c69',set())),
        post69_origins=[(k,c56.digest(v)) for k,v in sorted(getattr(tx,'post69_origins',{}).items())],
        c69_source_keys=sorted(getattr(view,'c69_source_keys',set())),
        c69_record=c56.digest(view.c69_record) if getattr(view,'c69_record',None) is not None else None,
        c69_key=getattr(view,'c69_key',None),c69_scan_token=getattr(view,'c69_scan_token',None),
        c69_prefix=getattr(view,'c69_prefix',None),c69_cache_identity=getattr(view,'c69_cache_identity',None),
        c69_proofs=c56.digest(sorted(getattr(view,'c69_proofs',{}).items())),
        c69_siblings_used=sorted(getattr(view,'c69_siblings_used',set())),
        c69_considered=sorted(getattr(view,'c69_considered',set())),
        c69_preflight_eligible=sorted(getattr(view,'c69_preflight_eligible',set())),
        c69_edges_considered=sorted(getattr(view,'c69_edges_considered',set())),
        c69_edges_preflight_eligible=sorted(getattr(view,'c69_edges_preflight_eligible',set())),
        c69_refusals=dict(getattr(view,'c69_refusals',{})),c69_mismatches=dict(getattr(view,'c69_mismatches',{})),
        c69_crossings=deepcopy(getattr(tx,'c69_crossings',[])),event_kind=event_kind,
        event_context=deepcopy(provenance))
    prefix = c56.digest(snapshot)
    baseline = view.c70_certificates.c71_baseline
    record = dict(classifier_identity=baseline.identity,
        **{k:v for k,v in baseline.header.items() if k.startswith('accepted_')},
        original_root_runtime_row=tx.root,original_root_row_sha256=view.session.row_identity(tx.root),
        current_entry_instance_row=row,incoming_state_identity=context_key[1],
        entry_seed_fact_count=sum(f is not None for f in state),checkpoint_incoming_state=c69.serialize_state(state),
        entry_instance_identity=c56.digest((baseline.certificates.identity,tx.root,row,context_key[1],summary['boundary_path_identity'])),
        **summary,terminal_context_runtime_row_sha256=view.session.row_identity(row),
        terminal_context_code_sha256=c56.sha(code),terminal_context_begin_rva=begin,terminal_context_end_rva=end,
        first_refusal=c70.POST,terminal_event_kind=event_kind,terminal_event_edge_backed=edge_backed,
        terminal_event_context=deepcopy(provenance),terminal_phase=provenance['phase'],
        terminal_reason=provenance['reason'],terminal_provenance=deepcopy(provenance),
        post_c69_origin=origin,active_c69_authorization=active,
        current_path_c69_record_identities=path_ids,current_origin_c69_record_identities=origin_ids,
        root_replay_identity=prefix,current_c70_continuation_identity=prefix,
        new_post_c69_boundaries_admitted=0,new_candidates_emitted=0)
    return record


def _emit(view,record):
    record['record_identity'] = c56.digest(record)
    existing = view.c71_capture
    if existing is None:
        view.c71_capture = record
        view.c70_certificates.c71_records.append(deepcopy(record))
        return
    # Outer transaction wrappers may observe the same refusal. The innermost
    # origin wins; never relabel an already captured terminal.
    if existing.get('terminal_event_edge_backed') is True:
        return
    if c56.encoded(existing)==c56.encoded(record):
        return


def _capture_edge(tx,view,index,ins,kind,target,architectural):
    key = view.key
    if key is None or key[0]!=index or key not in tx.contexts:
        raise c58.EvidenceError('c70 edge terminal lacks current source context')
    state,path = tx.contexts[key]
    if key[1]!=c58.state_id(state):
        raise c58.EvidenceError('wrong c70 edge terminal incoming state')
    path_info = path_summary(path);origin = getattr(tx,'post69_origins',{}).get(key)
    if key in getattr(tx,'post_c69',set()):
        phase = 'post_c69_reachable_context' if path_info['c69_record_identities'] else 'post_c69_reachable_dedup_stored_earlier_path'
        reason = 'current_context_post_c69_reachable'
    elif key in getattr(view,'c69_source_keys',set()):
        phase = 'used_c69_source_on_later_scan'
        reason = ('used_c69_source_later_scan' if tx.scan_token!=getattr(view,'c69_scan_token',None)
            else 'used_c69_source_later_instruction')
    else:
        raise c58.EvidenceError('unrecognized c70 edge terminal provenance')
    provenance = dict(phase=phase,reason=reason,underlying_refusal=None,
        key=list(key),pc=ins.address,size=ins.size,scan_token=tx.scan_token,
        c69_authorization_scan_token=getattr(view,'c69_scan_token',None),
        post_c69=key in getattr(tx,'post_c69',set()),used_c69_source=key in getattr(view,'c69_source_keys',set()),
        post_c69_origin_digest=c56.digest(origin) if origin is not None else None)
    record = _terminal_common(tx,view,key,state,path,'edge_callback',True,provenance)
    begin,end,code,_ = view.row(index)
    edges = c59.classify_edges(view.session,begin,end,ins)
    if not 1<=len(edges)<=2:
        raise c58.EvidenceError('missing exact c70 terminal successors')
    if not any(edge.get('edge_kind')==kind and edge.get('edge_target_rva')==target
        and edge.get('architectural_successor')==architectural for edge in edges):
        raise c58.EvidenceError('c70 terminal callback not present in classified successor set')
    record.update(source_runtime_row_sha256=view.session.row_identity(index),source_code_sha256=c56.sha(code),
        source_begin_rva=begin,source_end_rva=end,source_instruction_rva=ins.address,source_instruction_size=ins.size,
        source_instruction_sha256=c56.sha(bytes(ins.bytes)),source_mnemonic=ins.mnemonic,edges=edges,
        state_scope='authenticated_entry_seed_not_source_instruction_must_state')
    _emit(view,record)


def _capture_transaction(tx,view,event_kind,error,**context):
    if str(error)!=c70.POST or view.c71_capture is not None:
        return
    key,state,path = _context(tx,view,**context)
    cause = str(error.__cause__) if error.__cause__ is not None else None
    reason = ('transaction_'+event_kind+'_converted_'+cause) if cause else ('transaction_'+event_kind+'_guard_refusal')
    provenance = dict(phase='transaction_'+event_kind,reason=reason,underlying_refusal=cause,event_kind=event_kind,
        key=list(key),source=list(context['source']) if context.get('source') is not None else None,
        target=list(context['target']) if context.get('target') is not None else None,
        current_view_key=list(view.key) if view.key is not None else None,scan_token=tx.scan_token)
    record = _terminal_common(tx,view,key,state,path,event_kind,False,provenance)
    record.update(source_runtime_row_sha256=None,source_code_sha256=None,source_begin_rva=None,source_end_rva=None,
        source_instruction_rva=None,source_instruction_size=None,source_instruction_sha256=None,source_mnemonic=None,
        edges=[],state_scope='authenticated_entry_seed_transaction_terminal_no_instruction_must_state')
    _emit(view,record)


class View(_OriginalView):
    def __init__(self,certificates):
        super().__init__(certificates)
        self.c71_capture = None

    def _post69_boundary(self,index,ins,kind,target,architectural,relations):
        if self.tx is None:
            raise c58.EvidenceError('missing c70 terminal transaction')
        _capture_edge(self.tx,self,index,ins,kind,target,architectural)
        return super()._post69_boundary(index,ins,kind,target,architectural,relations)


class Transaction(_OriginalTransaction):
    def mark_post_c69(self,key,path=None):
        try:return super().mark_post_c69(key,path)
        except c58.Refusal as error:
            _capture_transaction(self,self.e,'mark_post_c69',error,key=key);raise

    def instance(self,row,state,path):
        try:return super().instance(row,state,path)
        except c58.Refusal as error:
            _capture_transaction(self,self.e,'instance',error,row=row,state=state,path=path);raise

    def connect(self,source,target):
        try:return super().connect(source,target)
        except c58.Refusal as error:
            _capture_transaction(self,self.e,'connect',error,source=source,target=target);raise

    def scan_row(self,key):
        try:return super().scan_row(key)
        except c58.Refusal as error:
            _capture_transaction(self,self.e,'scan_row',error,key=key);raise


def replay_records(baseline,progress=None):
    baseline.assert_current();pool = baseline.certificates
    pool.c71_baseline = baseline;pool.c71_records = []
    old_view,old_transaction = c70.View,c70.Transaction
    try:
        c70.View,c70.Transaction = View,Transaction
        replayed = c70.scan(pool,progress=progress)
    finally:
        c70.View,c70.Transaction = old_view,old_transaction
    if c56.encoded(replayed)!=c56.encoded(baseline.result):
        raise c58.EvidenceError('c71 observer changed original c70 accounting/candidate bytes')
    records = pool.c71_records
    expected = baseline.header['post_c69_uncertified_boundary_refused_roots_expected']
    if len(records)!=expected or len({r['original_root_runtime_row'] for r in records})!=expected:
        raise c58.EvidenceError('missing/duplicate c70 post-c69 terminal root domain')
    baseline.assert_current()
    return records


class Aggregate:
    def __init__(self,baseline,records,example_limit=8):
        if type(example_limit)!=int or not 0<=example_limit<=8:
            raise ValueError('invalid c71 example bound')
        self.baseline,self.records,self.limit = baseline,records,example_limit

    def finish(self):
        b = self.baseline;b.assert_current();records = self.records
        roots=set();states=set();rows=set();ids=set();events=Counter();phases=Counter();reasons=Counter();causes=Counter()
        kinds=Counter();relations=Counter();edges=0;model_only=0;edge_backed=0;transaction=0;complete=Counter()
        stream=hashlib.sha256();examples_kind={k:[] for k in c57.KINDS};examples_relation={k:[] for k in c57.RELATIONS}
        for record in records:
            if type(record)!=dict or c56.digest({k:v for k,v in record.items() if k!='record_identity'})!=record.get('record_identity'):
                raise c58.EvidenceError('bad c71 record identity')
            root=record['original_root_runtime_row'];rid=record['record_identity']
            if root in roots or rid in ids:
                raise c58.EvidenceError('duplicate c71 terminal record')
            roots.add(root);ids.add(rid);rows.add(record['current_entry_instance_row'])
            states.add((root,record['current_entry_instance_row'],record['incoming_state_identity'],record['boundary_path_identity']))
            events[record['terminal_event_kind']]+=1;phases[record['terminal_phase']]+=1;reasons[record['terminal_reason']]+=1
            cause=record['terminal_provenance'].get('underlying_refusal')
            if cause is not None:causes[cause]+=1
            stream.update(c56.encoded(record)+b'\n')
            if record['terminal_event_edge_backed']:
                edge_backed+=1;rels=[]
                for edge in record['edges']:
                    kind=edge['edge_kind'];relation=edge['target_metadata_relation'];rels.append(relation)
                    edges+=1;model_only+=not edge['architectural_successor'];kinds[kind]+=1;relations[relation]+=1
                    example={k:v for k,v in record.items() if k not in ('edges','checkpoint_incoming_state')};example.update(edge)
                    if len(examples_kind[kind])<self.limit:examples_kind[kind].append(example)
                    if len(examples_relation[relation])<self.limit:examples_relation[relation].append(example)
                if rels and all(r=='exact_unique_runtime_begin' for r in rels):complete['all_successors_exact_runtime_begin']+=1
                elif any(r=='inside_unique_runtime_span_nonentry' for r in rels):complete['contains_interior_successor']+=1
                else:complete['other_relation_set']+=1
            else:
                transaction+=1
                if record['edges'] or record['source_instruction_rva'] is not None:
                    raise c58.EvidenceError('transaction c71 record invented instruction/successor')
        expected=b.header['post_c69_uncertified_boundary_refused_roots_expected']
        if len(roots)!=expected or len(states)!=expected:
            raise c58.EvidenceError('c71 diagnostic count mismatch')
        historical={k:b.result[k] for k in ('reference_candidate_count','new_cross_row_candidate_count','reader_candidates',
            'writer_candidates','rmw_candidates','address_only_candidates')}
        return dict(repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            evidence='OFFLINE_STATIC_DIAGNOSTIC_ONLY',image_authenticated=True,image_sha256=b.header['image_sha256'],
            classifier_header=b.header,classifier_identity=b.identity,
            **{k:v for k,v in b.header.items() if k.startswith('accepted_')},
            full_historical_replay_used=True,checkpoint_accelerated_resume_used=False,
            post_c69_uncertified_boundary_refused_roots_expected=expected,
            post_c69_uncertified_boundary_refused_roots_classified=len(roots),post_c69_boundary_instances_total=len(records),
            unique_entry_instance_states=len(states),unique_terminal_context_rows=len(rows),
            edge_backed_terminal_instances=edge_backed,transaction_terminal_instances=transaction,
            terminal_event_kind_counts=dict(sorted(events.items())),terminal_phase_counts=dict(sorted(phases.items())),
            terminal_reason_counts=dict(sorted(reasons.items())),transaction_terminal_underlying_refusal_counts=dict(sorted(causes.items())),
            post_c69_boundary_edges_total=edges,architectural_post_c69_boundary_edges_total=edges-model_only,
            model_only_post_c69_boundary_edges_total=model_only,edge_kind_counts={k:kinds[k] for k in c57.KINDS},
            target_metadata_relation_counts={k:relations[k] for k in c57.RELATIONS},
            complete_successor_relation_group_counts=dict(sorted(complete.items())),
            authenticated_record_set_sha256=stream.hexdigest(),examples_per_class=self.limit,
            examples_by_edge_kind=examples_kind,examples_by_target_relation=examples_relation,
            classification_complete=True,terminal_origin_classification_complete=True,
            new_post_c69_boundaries_admitted=0,c70_candidate_output_unchanged=True,
            c70_candidate_output_sha256=c56.digest(b.result['reference_candidates']),historical_c70_candidate_counts=historical,
            new_cross_row_candidate_count=0,publication_source_kind='unproven',live_qword_reads=0,
            diagnostic_scope='unchanged_c70_first_terminal_post_c69_origin_per_root_no_target_traversal',
            post_c69_boundary_edges_scope='edge_backed_terminal_successors_only_transaction_terminals_have_no_invented_instruction_or_successor',
            c56_through_c70_evidence_read_only=True,c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
            **{k:False for k in c56.FALSE_GATES})


def classify(baseline,progress=None):
    records = replay_records(baseline,progress)
    return Aggregate(baseline,records).finish(),records


def make_checkpoint(baseline,result,records):
    stream=hashlib.sha256()
    for record in records:stream.update(c56.encoded(record)+b'\n')
    header=dict(checkpoint_version=CHECKPOINT_VERSION,checkpoint_contract=CHECKPOINT_CONTRACT,
        image_sha256=baseline.header['image_sha256'],accepted_c70_physical_sha256=baseline.header['accepted_c70_physical_sha256'],
        accepted_c70_result_identity=baseline.header['accepted_c70_result_identity'],
        accepted_c70_authentication_identity=baseline.header['accepted_c70_authentication_identity'],
        c71_classifier_identity=baseline.identity,c71_result_identity=c56.digest(result),record_count=len(records),
        record_stream_sha256=stream.hexdigest(),limits=asdict(c58.Limits()),tool_sources=sources(),
        mutable_transaction_resume_snapshot=False)
    payload=dict(header=header,result=result,records=records);payload['checkpoint_identity']=c56.digest(payload);return payload


def validate_checkpoint(value):
    if type(value)!=dict or set(('header','result','records','checkpoint_identity'))-set(value):
        raise c58.EvidenceError('invalid c71 checkpoint shape')
    identity=value['checkpoint_identity'];body={k:v for k,v in value.items() if k!='checkpoint_identity'}
    if c56.digest(body)!=identity:raise c58.EvidenceError('corrupt c71 checkpoint identity')
    header,result,records=value['header'],value['result'],value['records']
    if (header.get('checkpoint_version')!=CHECKPOINT_VERSION or header.get('checkpoint_contract')!=CHECKPOINT_CONTRACT
        or header.get('image_sha256')!=EXPECTED_SHA or header.get('tool_sources')!=sources()
        or header.get('limits')!=asdict(c58.Limits()) or header.get('mutable_transaction_resume_snapshot') is not False
        or result.get('repository_result')!=READY or result.get('tool_version')!=VERSION or result.get('schema_version')!=SCHEMA
        or result.get('contract')!=CONTRACT or c56.digest(result)!=header.get('c71_result_identity')
        or len(records)!=header.get('record_count') or result.get('post_c69_boundary_instances_total')!=len(records)):
        raise c58.EvidenceError('stale/wrong c71 checkpoint header/result')
    stream=hashlib.sha256()
    for record in records:
        if type(record)!=dict or c56.digest({k:v for k,v in record.items() if k!='record_identity'})!=record.get('record_identity'):
            raise c58.EvidenceError('stale/wrong c71 checkpoint record')
        stream.update(c56.encoded(record)+b'\n')
    if stream.hexdigest()!=header.get('record_stream_sha256') or stream.hexdigest()!=result.get('authenticated_record_set_sha256'):
        raise c58.EvidenceError('wrong c71 checkpoint record set identity')
    return value


def analyze(data,cache=None,c57_json=None,c58_json=None,c59_json=None,c60_json=None,c61_json=None,c62_json=None,
        c63_json=None,c64_json=None,c65_json=None,c66_json=None,c67_json=None,c68_json=None,c69_json=None,
        c69_checkpoint=None,c70_json=None,progress=None):
    if len(data)>cfg.old.MAX_FILE or c56.sha(data)!=EXPECTED_SHA:
        raise c58.EvidenceError('exact executable SHA256 mismatch or input cap')
    s=c56.Session(data);old=c57.collect_baseline(s,cache,progress=progress)
    e=c58.Evidence(old,owner_result=c56.read_json(c57_json) if c57_json is not None else None,progress=progress)
    b=c59.Baseline(e,owner_result=c56.read_json(c58_json) if c58_json is not None else None,progress=progress)
    p=c60.ProofPool(b,owner_result=c56.read_json(c59_json) if c59_json is not None else None,progress=progress)
    original=c61.Baseline(p,owner_result=c56.read_json(c60_json) if c60_json is not None else None,progress=progress)
    cert=c62.ProofPool(original,owner_result=c56.read_json(c61_json) if c61_json is not None else None,progress=progress)
    prior=c63.Baseline(cert,owner_result=c56.read_json(c62_json) if c62_json is not None else None,progress=progress)
    cert64=c64.ProofPool(prior,owner_result=c56.read_json(c63_json) if c63_json is not None else None,progress=progress)
    base65=c65.Baseline(cert64,owner_result=c56.read_json(c64_json) if c64_json is not None else None,progress=progress)
    cert66=c66.ProofPool(base65,owner_result=c56.read_json(c65_json) if c65_json is not None else None,progress=progress)
    owner66=c67.read_c66(c66_json) if c66_json is not None else None;base67=c67.Baseline(cert66,owner_result=owner66,progress=progress)
    owner67=c68.read_c67(c67_json) if c67_json is not None else None;cert68=c68.ProofPool(base67,owner_result=owner67,progress=progress)
    owner68=c69.read_c68(c68_json) if c68_json is not None else None;baseline69=c69.Baseline(cert68,owner_result=owner68,progress=progress)
    if c69_json is None or c69_checkpoint is None or c70_json is None:
        raise c58.EvidenceError('c71 requires explicit c69 result/checkpoint and c70 owner JSON')
    owner69=c70.read_c69(c69_json);checkpoint=c70.read_c69_checkpoint(c69_checkpoint)
    pool70=c70.ProofPool(baseline69,owner69,checkpoint,production=True)
    baseline71=Baseline(pool70,read_c70(c70_json),production=True)
    return classify(baseline71,progress)


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--checkpoint-output',type=Path)
    for flag in ('c56-cache','c57-json','c58-json','c59-json','c60-json','c61-json','c62-json','c63-json','c64-json','c65-json','c66-json','c67-json','c68-json','c69-json','c69-checkpoint','c70-json'):
        parser.add_argument('--'+flag,type=Path)
    a=parser.parse_args()
    try:
        output=a.output.resolve();inputs=[p for p in (a.exe,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,a.c62_json,
            a.c63_json,a.c64_json,a.c65_json,a.c66_json,a.c67_json,a.c68_json,a.c69_json,a.c69_checkpoint,a.c70_json) if p is not None]
        if any(output==p.resolve() for p in inputs) or (a.c56_cache and output.is_relative_to(a.c56_cache.resolve())):
            raise c58.EvidenceError('output overlaps immutable input evidence')
        result,records=analyze(a.exe.read_bytes(),a.c56_cache,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,
            a.c62_json,a.c63_json,a.c64_json,a.c65_json,a.c66_json,a.c67_json,a.c68_json,a.c69_json,a.c69_checkpoint,a.c70_json,
            lambda phase,n,total:print(f'{phase}: {n}/{total}',file=sys.stderr))
        c56.atomic_json(a.output,result)
        if a.checkpoint_output is not None:
            # Reconstruct only the compact baseline identity from the result header;
            # checkpoint creation requires the same records already classified.
            class _B:pass
            b=_B();b.header=result['classifier_header'];b.identity=result['classifier_identity']
            checkpoint=make_checkpoint(b,result,records);validate_checkpoint(checkpoint);c56.atomic_json(a.checkpoint_output,checkpoint)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(f'{BLOCKED}: {error}',file=sys.stderr);return 2
    print(f'{READY}: {a.output}')
    if a.checkpoint_output is not None:print(f'C71_AUTHENTICATED_PREFIX_CHECKPOINT_READY: {a.checkpoint_output}')
    return 0


if __name__=='__main__':
    raise SystemExit(main())
