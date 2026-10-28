"""c65 OFFLINE diagnostic of c64's exact first terminal post-c63 boundary.

Observe one unchanged edge invocation, capture its actual guard/conversion cause,
and rethrow. Audit complete original c64 accounting/candidate bytes. No admission,
post-abort traversal, new Fact, pointer/value read, native probe or game use.
"""
import argparse
from collections import Counter
from copy import deepcopy
from dataclasses import asdict
import hashlib
from pathlib import Path
import sys

import slot_publication_post_c61_stitched as c64

c63,c62,c61,c60,c59,c58,c57,c56,cfg = c64.c63,c64.c62,c64.c61,c64.c60,c64.c59,c64.c58,c64.c57,c64.c56,c64.cfg
VERSION = 'v0.1c65'
SCHEMA = 1
CONTRACT = 'exact_c64_first_terminal_post_c63_boundary_full_prefix_provenance_diagnostic_v1'
READY = 'POST_C63_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY'
BLOCKED = 'POST_C63_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED'
EXPECTED_SHA = c64.EXPECTED_SHA
PHASES = ('same_instance_after_c63_preflight_before_row_fixpoint','post_c63_reachable_context',
    'post_c63_reachable_dedup_stored_earlier_path','prior_c63_authorization_converted_earlier_refusal','used_c63_source_on_later_scan')
ACCEPTED_C64 = dict(repository_result=c64.READY,tool_version=c64.VERSION,schema_version=c64.SCHEMA,
    accepted_c63_classifier_identity=c64.ACCEPTED_C63['classifier_identity'],
    accepted_c63_record_set_identity=c64.ACCEPTED_C63['authenticated_record_set_sha256'],
    accepted_c63_result_identity='559e4935c8c6af7c4974d7b16703f8177cb0636a538a602ddb56bafb8c397343',
    root_transactions_attempted=162472,root_transactions_completed=105083,roots_completed=105083,root_transactions_refused=57389,
    c63_boundary_instances_available=2923,c63_boundary_instances_considered=2923,c63_boundary_instances_admitted=1150,c63_boundary_instances_refused=1773,
    c63_architectural_edges_available=3167,c63_exact_edges_available=3049,c63_exact_edges_considered=3049,c63_exact_edges_admitted=1346,c63_exact_edges_refused=1703,
    rows_reached_by_continuation=1549,address_facts_crossing_runtime_rows=2881,c63_certified_crossings_with_facts=3,c63_address_facts_crossing_rows=4,
    post_c63_uncertified_boundary_refusals=998,new_cross_row_candidate_count=0,reference_candidate_count=0,
    reader_candidates=0,writer_candidates=0,rmw_candidates=0,address_only_candidates=0,c56_through_c63_evidence_read_only=True)


def sources():
    return dict(c64.sources(),slot_publication_post_c63_boundary_py=c56.sha(Path(__file__).read_bytes()))


def evidence(certificates):
    return certificates.baseline.certificates.baseline.pool.baseline.evidence


class Baseline:
    def __init__(self,certificates,*,owner_result=None,accepted=None,progress=None):
        self.certificates = certificates
        self.result = c64.scan(certificates,progress=progress)
        expected = ACCEPTED_C64 if accepted is None else accepted  # owned fixture seam, no CLI bypass
        if any(c56.encoded(self.result.get(k))!=c56.encoded(v) for k,v in expected.items()):
            raise c58.EvidenceError('accepted c64 identity/count/result mismatch')
        if owner_result is not None and c56.encoded(owner_result)!=c56.encoded(self.result):
            raise c58.EvidenceError('stale/wrong image/tool/schema/contract/record-set/result c64 JSON')
        if (self.result['repository_result']!=c64.READY or self.result['tool_version']!=c64.VERSION or self.result['schema_version']!=c64.SCHEMA
            or self.result['contract']!=c64.CONTRACT or self.result['authentication_header']!=certificates.header
            or self.result['limits']!=asdict(c58.Limits()) or any(self.result[k] for k in c56.FALSE_GATES)):
            raise c58.EvidenceError('wrong c64 proof/contract/limits/gates')
        self.header = dict(tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_sha256=self.result['image_sha256'],c56_manifest_id=certificates.header['c56_manifest_id'],
            **{k:v for k,v in certificates.header.items() if k.startswith('accepted_')},
            accepted_c64_authentication_identity=certificates.identity,accepted_c64_result_identity=c56.digest(self.result),
            c64_contract=c64.CONTRACT,c64_limits=self.result['limits'],
            post_c63_uncertified_boundary_refused_roots_expected=self.result['post_c63_uncertified_boundary_refusals'],tool_sources=sources())
        self.identity = c56.digest(self.header)

    def assert_current(self):
        self.certificates.assert_current()
        if (self.header.get('tool_sources')!=sources() or c56.digest(self.header)!=self.identity
            or c56.digest(self.result)!=self.header['accepted_c64_result_identity']
            or self.certificates.identity!=self.header['accepted_c64_authentication_identity']
            or self.certificates.header!=self.result['authentication_header']):
            raise c58.EvidenceError('stale/mutated c65 schema/tool/contract/c64 result identity')


def path_summary(path):
    return dict(c63.path_summary(path),
        c63_record_identities=[p['c63_record_identity'] for p in path if p.get('proof_kind')=='c63_post_boundary'],
        c63_full_prefix_identities=[p['c63_full_prefix_identity'] for p in path if p.get('proof_kind')=='c63_post_boundary'])


def edge_before(tx,view,index,ins):
    auth = view.c63_record
    return dict(root=tx.root,key=view.key,index=index,pc=ins.address,size=ins.size,instruction_sha256=c56.sha(bytes(ins.bytes)),
        post_c63=view.key in tx.post_c63,used_c63_source=view.key in view.c63_source_keys,scan_token=tx.scan_token,
        authorization_scan_token=view.c63_scan_token,authorization_record_identity=auth['record_identity'] if auth else None,
        authorization_source_pc=auth['source_instruction_rva'] if auth else None,
        c61_mismatches=dict(view.c61_mismatches),c63_mismatches=dict(view.c63_mismatches))


def terminal_provenance(tx,view,index,ins):
    before = view.post63_edge_before
    if before is None:raise c58.EvidenceError('terminal POST lacks original edge-entry snapshot')
    current = edge_before(tx,view,index,ins)
    if any(c56.encoded(before[k])!=c56.encoded(current[k]) for k in before if k not in ('c61_mismatches','c63_mismatches')):
        raise c58.EvidenceError('stale/wrong terminal edge-entry context/source/proof')
    delta = lambda field:{k:current[field].get(k,0)-before[field].get(k,0) for k in set(current[field])|set(before[field])
        if current[field].get(k,0)!=before[field].get(k,0)}
    cause = view.post63_underlying_refusal
    result = dict(edge_entry=deepcopy(before),c61_mismatch_delta=delta('c61_mismatches'),c63_mismatch_delta=delta('c63_mismatches'),
        underlying_refusal=cause,underlying_c63_capture=None,underlying_c61_capture=None)
    if before['post_c63']:
        reason = 'current_context_post_c63_reachable'
    elif before['used_c63_source'] and (before['pc']!=before['authorization_source_pc'] or before['scan_token']!=before['authorization_scan_token']):
        reason = 'used_c63_source_later_scan' if before['scan_token']!=before['authorization_scan_token'] else 'used_c63_source_later_instruction'
    elif before['authorization_record_identity'] is not None and cause in (c62.POST,c60.POST,c59.LATER):
        reason = {c62.POST:'prior_c63_authorization_converted_c62_post',c60.POST:'prior_c63_authorization_converted_c60_post',
            c59.LATER:'prior_c63_authorization_converted_pre_c59_later'}[cause]
        if cause==c62.POST:
            if (view.diagnostic_capture is None or c56.encoded(view.diagnostic_capture)!=c56.encoded(c63.capture_for(tx,view,index,ins))):
                raise c58.EvidenceError('converted c62 POST lacks exact original c63 source capture')
            result['underlying_c63_capture'] = deepcopy(view.diagnostic_capture)
        if cause==c60.POST and view.capture is not None:
            raw = view.capture
            # A cached earlier capture is not evidence for this current source.
            if (raw['root'],raw['row'],raw['state_identity'],raw['pc'])==(tx.root,index,view.key[1],ins.address):
                c61.record_for(view.certificates.baseline,tx,view)
                result['underlying_c61_capture'] = deepcopy(raw)
    else:raise c58.EvidenceError('unrecognized original c64 terminal edge provenance')
    if reason.startswith(('current_context','used_c63_source')) and (cause is not None or result['c61_mismatch_delta'] or result['c63_mismatch_delta']):
        raise c58.EvidenceError('direct c64 guard has inconsistent cause/counter delta')
    result['reason'] = reason
    return result


def capture_for(tx,view,index,ins):
    key = view.key
    if key is None or key[0]!=index or key not in tx.contexts:raise c58.EvidenceError('terminal c64 POST lacks current source context')
    state,path = tx.contexts[key]
    if key[1]!=c58.state_id(state):raise c58.EvidenceError('wrong terminal incoming state')
    auth = view.c63_record;ordinal = view.c64_certificates.index.get(tx.root)
    if auth is None or ordinal is None or c56.encoded(auth)!=view.c64_certificates.records[ordinal]:
        raise c58.EvidenceError('terminal POST lacks exact sealed c63 authorization at original root')
    provenance = terminal_provenance(tx,view,index,ins);reason = provenance['reason'];origin = tx.post63_origins.get(key)
    if reason=='current_context_post_c63_reachable':
        if origin is None:raise c58.EvidenceError('post-c63 reachable source lacks diagnostic origin')
        phase = PHASES[1] if any(p.get('proof_kind')=='c63_post_boundary' for p in path) else PHASES[2]
    elif reason=='used_c63_source_later_instruction':phase = PHASES[0]
    elif reason=='used_c63_source_later_scan':phase = PHASES[4]
    else:
        phase = PHASES[3]
        if key in tx.post_c63 or origin is not None:raise c58.EvidenceError('converted refusal falsely labeled post-c63 reachable')
    _,_,code,_ = view.row(index)
    return dict(root=tx.root,row=index,state_identity=key[1],path=path_summary(path),entry_seed_fact_count=sum(f is not None for f in state),
        pc=ins.address,size=ins.size,instruction_sha256=c56.sha(bytes(ins.bytes)),row_sha256=view.session.row_identity(index),code_sha256=c56.sha(code),
        phase=phase,terminal_provenance=provenance,post_c63_origin=deepcopy(origin),post_c61_origin=deepcopy(tx.post61_origins.get(key)),
        post_c59_origin=deepcopy(tx.post_origins.get(key)),
        c63_authorization={k:deepcopy(auth[k]) for k in ('record_identity','classifier_identity','root_replay_identity',
            'entry_instance_identity','current_entry_instance_row','incoming_state_identity','boundary_path_identity','boundary_transition_identities',
            'source_instruction_rva','source_instruction_sha256','terminal_phase','terminal_reason','terminal_provenance',
            'post_c59_origin','post_c61_origin','c61_authorization','c61_terminal_phase','local_c61_edge_identities')},
        local_c63_edge_identities=sorted(p['edge_identity'] for p in view.c63_proofs.values()),
        c63_source_contexts=sorted(view.c63_source_keys),c63_scan_token=view.c63_scan_token,current_scan_token=tx.scan_token)


class View(c64.View):
    def __init__(self,certificates):
        super().__init__(certificates)
        self.post63_capture,self.post63_edge_before,self.post63_underlying_refusal = None,None,None

    def edge(self,index,ins,kind,target,architectural,relations):
        before = edge_before(self.tx,self,index,ins) if self.tx is not None else None
        try:return super().edge(index,ins,kind,target,architectural,relations)
        except c58.Refusal as error:
            if str(error)==c64.POST:
                if self.tx is None:raise c58.EvidenceError('missing terminal transaction')
                self.post63_edge_before = before
                self.post63_underlying_refusal = str(error.__cause__) if error.__cause__ is not None else None
                capture = capture_for(self.tx,self,index,ins)
                if self.post63_capture is not None and c56.encoded(self.post63_capture)!=c56.encoded(capture):
                    raise c58.EvidenceError('multiple distinct first terminal c64 source events')
                self.post63_capture = capture
            raise  # SAME original refusal; never return a c65 proof.


class Transaction(c64.Transaction):
    """Shadow the original already-metered post-c63 closure, no original mutation."""
    def __init__(self,view,root,engine,limits):
        super().__init__(view,root,engine,limits)
        self.post63_origins = {};self.post63_arrival = self.post63_connect = None

    def instance(self,row,state,path):
        before = self.post63_arrival;self.post63_arrival = ((row,c58.state_id(state)),path)
        try:return super().instance(row,state,path)
        finally:self.post63_arrival = before

    def connect(self,source,target):
        before = self.post63_connect;self.post63_connect = (source,target)
        try:return super().connect(source,target)
        finally:self.post63_connect = before

    def mark_post_c63(self,key):
        before = set(self.post_c63)
        super().mark_post_c63(key)  # Invalidation-only POST keeps its missing edge source.
        added = self.post_c63-before
        if not added and key in self.post63_origins:return
        if self.post63_arrival is not None and self.post63_arrival[0]==key:
            origin = dict(kind='authenticated_c63_arrival',entry_context=key,arrival_path=path_summary(self.post63_arrival[1]),propagation_contexts=[key])
            if not origin['arrival_path']['c63_record_identities']:raise c58.EvidenceError('post arrival lacks c63 proof')
        elif self.post63_connect is not None and self.post63_connect[1]==key:
            source,_ = self.post63_connect
            if source not in self.post63_origins:raise c58.EvidenceError('post propagation lacks c63 source origin')
            origin = deepcopy(self.post63_origins[source]);origin['propagation_contexts'].append(key)
        else:raise c58.EvidenceError('unexpected original c64 post marking call path')
        if len(origin['propagation_contexts'])>self.limits.instances:raise c58.EvidenceError('bounded diagnostic post-c63 origin cap')
        for target in sorted(added):self.post63_origins[target] = deepcopy(origin)
        self.post63_origins.setdefault(key,origin)


def record_for(baseline,tx,view):
    capture = view.post63_capture
    if capture is None:raise c58.EvidenceError('terminal c64 POST without source edge callback; diagnostic source unavailable')
    s = view.session;index = capture['row'];begin,end,code,_ = view.row(index);pc,size = capture['pc'],capture['size']
    if not begin<=pc<end or not 1<=size<=15:raise c58.EvidenceError('invalid terminal source span')
    ins = next(cfg.old.decoder().disasm(code[pc-begin:pc-begin+size],pc,count=1),None)
    if ins is None or ins.size!=size or c56.encoded(capture)!=c56.encoded(capture_for(tx,view,index,ins)):
        raise c58.EvidenceError('stale/wrong terminal root/state/path/source/phase/provenance')
    allowed = lambda target:any(b<=target<min(e,b+n) and flags&0x60000000==0x60000000 and not flags&0x80000000 for b,e,_,n,flags in s.sections)
    try:cfg.successors(ins,begin,end,s.size,allowed)
    except ValueError as error:
        if str(error)!='unsupported_runtime_boundary':raise c58.EvidenceError('terminal source not original boundary oracle refusal') from error
    else:raise c58.EvidenceError('terminal source has no outgoing boundary')
    edges = c59.classify_edges(s,begin,end,ins)
    if not 1<=len(edges)<=2:raise c58.EvidenceError('missing exact terminal successors')
    snapshot = dict(original_c64_prefix=c64.sibling_prefix(tx,view),considered=sorted(tx.considered),post_c63=sorted(tx.post_c63),
        c63_source_keys=sorted(view.c63_source_keys),c63_record= c56.digest(view.c63_record),c63_key=view.c63_key,
        c63_scan_token=view.c63_scan_token,c63_prefix=view.c63_prefix,c63_cache_identity=view.c63_cache_identity,
        c63_proofs=c56.digest(sorted(view.c63_proofs.items())),c63_siblings_used=sorted(view.c63_siblings_used),
        c63_considered_before=sorted(view.c63_considered_before),c63_considered_allowed=sorted(view.c63_considered_allowed),
        c63_considered=sorted(view.c63_considered),c63_eligible=sorted(view.c63_eligible),c63_edges_considered=sorted(view.c63_edges_considered),
        c63_edges_eligible=sorted(view.c63_edges_eligible),c63_refusals=dict(view.c63_refusals),c63_mismatches=dict(view.c63_mismatches),
        crossings=tx.crossings,c61_crossings=tx.c61_crossings,c63_crossings=tx.c63_crossings,capture=capture)
    prefix = c56.digest(snapshot)
    record = dict(classifier_identity=baseline.identity,**{k:v for k,v in baseline.header.items() if k.startswith('accepted_')},
        original_root_runtime_row=tx.root,original_root_row_sha256=s.row_identity(tx.root),current_entry_instance_row=index,
        incoming_state_identity=capture['state_identity'],entry_seed_fact_count=capture['entry_seed_fact_count'],
        entry_instance_identity=c56.digest((baseline.certificates.identity,tx.root,index,capture['state_identity'],capture['path']['boundary_path_identity'])),
        **capture['path'],source_runtime_row_sha256=capture['row_sha256'],source_code_sha256=capture['code_sha256'],
        source_begin_rva=begin,source_end_rva=end,source_instruction_rva=pc,source_instruction_size=size,source_instruction_sha256=capture['instruction_sha256'],
        source_mnemonic=ins.mnemonic,first_refusal=c64.POST,terminal_phase=capture['phase'],terminal_reason=capture['terminal_provenance']['reason'],
        terminal_provenance=capture['terminal_provenance'],post_c63_origin=capture['post_c63_origin'],post_c61_origin=capture['post_c61_origin'],post_c59_origin=capture['post_c59_origin'],
        c63_authorization=capture['c63_authorization'],c63_terminal_phase=capture['c63_authorization']['terminal_phase'],c63_terminal_reason=capture['c63_authorization']['terminal_reason'],
        local_c63_edge_identities=capture['local_c63_edge_identities'],root_replay_identity=prefix,current_c64_continuation_identity=prefix,edges=edges,
        state_scope='authenticated_entry_seed_not_source_instruction_must_state',new_post_c63_boundaries_admitted=0,new_candidates_emitted=0)
    record['record_identity'] = c56.digest(record)
    return record


class Audit:
    """Match every dynamic original c64 field and complete retained candidate bytes."""
    def __init__(self):
        self.old = c59.ReplayAudit()
        self.considered,self.admitted,self.edges_considered,self.edges_admitted = set(),set(),set(),set()
        self.preflight,self.mismatches,self.extra,self.refs,self.by_id = Counter(),Counter(),Counter(),[],{}

    def add(self,tx,view,reason,certificates):
        self.old.add(tx,reason);self.considered.update(view.c63_considered);self.edges_considered.update(view.c63_edges_considered)
        self.preflight.update(view.c63_refusals);self.mismatches.update(view.c63_mismatches)
        record = certificates.lookup(tx.root)
        if record is not None:self.by_id[record['record_identity']] = record
        if reason is None:
            self.admitted.update(view.c63_eligible);self.edges_admitted.update(view.c63_edges_eligible)
            self.extra['crossings'] += len(tx.c63_crossings);self.extra['facts'] += sum(n for _,n in tx.c63_crossings)
            for original in tx.references:
                ref = deepcopy(original);path = ref['boundary_path'];prior = certificates.baseline.certificates
                ref.update(c62_authentication_identity=prior.identity,boundary_path_identity=c56.digest(path),
                    c62_entry_instance_identity=c56.digest((prior.identity,tx.root,ref['continuation_runtime_row'],ref['incoming_state_identity'],c56.digest(path))),
                    path_c59_record_identities=[p['c59_record_identity'] for p in path if p.get('proof_kind')=='c59_later_boundary'],
                    path_c61_record_identities=[p['c61_record_identity'] for p in path if p.get('proof_kind')=='c61_post_boundary'],root_c61_record_identities=sorted(view.c61_eligible))
                if view.c63_eligible:
                    ref.update(c64_authentication_identity=certificates.identity,
                        c64_entry_instance_identity=c56.digest((certificates.identity,tx.root,ref['continuation_runtime_row'],ref['incoming_state_identity'],c56.digest(path))),
                        path_c63_record_identities=[p['c63_record_identity'] for p in path if p.get('proof_kind')=='c63_post_boundary'],root_c63_record_identities=sorted(view.c63_eligible))
                if len(self.refs)<c58.Limits().retained_references:self.refs.append(ref)

    def finish(self,original):
        t,ops = self.old.totals,self.old.ops;count = sum(ops.values())
        grouped = lambda ids,field:dict(sorted(Counter(self.by_id[i][field] for i in ids).items()))
        projection = dict(c63_boundary_instances_considered=len(self.considered),c63_boundary_instances_admitted=len(self.admitted),c63_boundary_instances_refused=len(self.considered-self.admitted),
            c63_exact_edges_considered=len(self.edges_considered),c63_exact_edges_admitted=len(self.edges_admitted),c63_exact_edges_refused=len(self.edges_considered-self.edges_admitted),
            c63_terminal_phase_considered=grouped(self.considered,'terminal_phase'),c63_terminal_phase_admitted=grouped(self.admitted,'terminal_phase'),
            c63_terminal_phase_refused=grouped(self.considered-self.admitted,'terminal_phase'),c63_terminal_reason_considered=grouped(self.considered,'terminal_reason'),
            c63_terminal_reason_admitted=grouped(self.admitted,'terminal_reason'),c63_terminal_reason_refused=grouped(self.considered-self.admitted,'terminal_reason'),
            c63_all_successor_preflight_refusal_relation_counts=dict(self.preflight),c63_certificate_mismatch_counts=dict(self.mismatches),
            c63_certified_crossings_with_facts=self.extra['crossings'],c63_address_facts_crossing_rows=self.extra['facts'],
            root_transactions_attempted=t['root_transactions_attempted'],root_transactions_completed=t['roots_completed'],roots_completed=t['roots_completed'],
            root_transactions_refused=sum(self.old.reasons.values()),root_refusal_reasons=dict(self.old.reasons),post_c63_uncertified_boundary_refusals=self.old.reasons[c64.POST],
            rows_reached_by_continuation=len(self.old.rows),remaining_boundary_refusal_relation_counts={k:self.old.relations[k] for k in c57.RELATIONS},
            reference_candidate_count=count,new_cross_row_candidate_count=count,reader_candidates=ops['READ'],writer_candidates=ops['WRITE'],rmw_candidates=ops['RMW'],address_only_candidates=ops['ADDRESS_ONLY'],
            reference_candidates=self.refs,candidate_records_not_retained=count-len(self.refs))
        for k in ('continuation_instances_created','continuation_instances_deduplicated','continuation_instances_refused','cross_row_cap_refusals',
            'cross_row_cycles_seen','address_facts_crossing_runtime_rows','call_edges_with_preserved_facts_crossing_rows','no_carried_fact_edges'):projection[k] = t[k]
        if any(c56.encoded(original.get(k))!=c56.encoded(v) for k,v in projection.items()):raise c58.EvidenceError('observer changed original c64 accounting/candidate bytes')


def replay_records(baseline,progress=None):
    baseline.assert_current();certificates = baseline.certificates;e = evidence(certificates);audit = Audit();engine = shard = None;count = 0
    for number,root in enumerate(e.proofs,1):
        current = root // e.session.profile.rows_per_shard
        if current!=shard:engine = c58.c55.Engine(e.session.domain,e.session.profile.summary);shard = current
        view = View(certificates);tx = Transaction(view,root,engine,c58.Limits());reason = None
        try:tx.run()
        except c58.EvidenceError:raise
        except (c58.Refusal,ValueError) as error:reason = str(error)
        audit.add(tx,view,reason,certificates)
        if reason==c64.POST:count += 1;yield record_for(baseline,tx,view)
        elif view.post63_capture is not None:raise c58.EvidenceError('POST capture not terminal root outcome')
        if progress and (number%1024==0 or number==len(e.proofs)):progress('c65 unchanged c64 terminal observer replay',number,len(e.proofs))
    audit.finish(baseline.result)
    if count!=baseline.header['post_c63_uncertified_boundary_refused_roots_expected']:raise c58.EvidenceError('missing accepted c64 POST root domain')
    baseline.assert_current()


class Aggregate:
    def __init__(self,baseline,example_limit=8):
        if type(example_limit)!=int or not 0<=example_limit<=8:raise ValueError('invalid example bound')
        self.baseline,self.limit,self.expected = baseline,example_limit,iter(replay_records(baseline));self.poisoned = False
        self.roots,self.instances,self.states,self.rows = set(),set(),set(),set();self.count = self.edges = self.model_only = 0
        self.kinds,self.relations,self.exact,self.interior,self.free,self.adjacent,self.phases,self.reasons,self.c63_phases,self.c63_reasons,self.causes = (Counter() for _ in range(11))
        self.examples_kind = {k:[] for k in c57.KINDS};self.examples_relation = {k:[] for k in c57.RELATIONS};self.records_sha = hashlib.sha256()

    def add(self,record):
        if self.poisoned:raise c58.EvidenceError('POST aggregate already poisoned')
        try:
            expected = next(self.expected,None)
            if expected is None or type(record)!=dict or c56.encoded(record)!=c56.encoded(expected):raise c58.EvidenceError('missing/duplicate/out-of-order/stale/wrong root/state/path/source/prefix/provenance record')
            root = record['original_root_runtime_row']
            if root in self.roots:raise c58.EvidenceError('multiple records for one original terminal root')
            self.count += 1;self.roots.add(root);self.instances.add(record['record_identity'])
            self.states.add((root,record['current_entry_instance_row'],record['incoming_state_identity'],record['boundary_path_identity']));self.rows.add(record['current_entry_instance_row'])
            self.phases[record['terminal_phase']] += 1;self.reasons[record['terminal_reason']] += 1;self.c63_phases[record['c63_terminal_phase']] += 1;self.c63_reasons[record['c63_terminal_reason']] += 1
            cause = record['terminal_provenance']['underlying_refusal']
            if cause is not None:self.causes[cause] += 1
            self.records_sha.update(c56.encoded(record)+b'\n')
            for edge in record['edges']:
                kind,relation = edge['edge_kind'],edge['target_metadata_relation'];self.edges += 1;self.model_only += not edge['architectural_successor'];self.kinds[kind] += 1;self.relations[relation] += 1
                if relation=='exact_unique_runtime_begin':self.exact[kind] += 1
                if relation=='inside_unique_runtime_span_nonentry':self.interior[kind] += 1
                if relation=='executable_file_backed_outside_runtime_metadata':self.free[kind] += 1
                if edge['exact_adjacency'] is True:self.adjacent[kind] += 1
                example = {k:v for k,v in record.items() if k!='edges'};example.update(edge)
                if len(self.examples_kind[kind])<self.limit:self.examples_kind[kind].append(example)
                if len(self.examples_relation[relation])<self.limit:self.examples_relation[relation].append(example)
        except (ValueError,KeyError,TypeError,IndexError,RecursionError):self.poisoned = True;raise

    def finish(self):
        try:
            b = self.baseline
            if self.poisoned or next(self.expected,None) is not None:raise c58.EvidenceError('poisoned/missing POST root record')
            b.assert_current()
            if (len(self.roots)!=b.header['post_c63_uncertified_boundary_refused_roots_expected'] or self.count!=len(self.instances) or self.count!=len(self.states)
                or sum(self.kinds.values())!=self.edges or sum(self.relations.values())!=self.edges):raise c58.EvidenceError('POST diagnostic count mismatch')
            return dict(repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,evidence='OFFLINE_STATIC_DIAGNOSTIC_ONLY',image_authenticated=True,image_sha256=b.header['image_sha256'],
                classifier_header=b.header,classifier_identity=b.identity,**{k:v for k,v in b.header.items() if k.startswith('accepted_')},
                post_c63_uncertified_boundary_refused_roots_expected=b.header['post_c63_uncertified_boundary_refused_roots_expected'],post_c63_uncertified_boundary_refused_roots_classified=len(self.roots),
                post_c63_boundary_instances_total=self.count,unique_entry_instance_states=len(self.states),unique_source_rows=len(self.rows),post_c63_boundary_edges_total=self.edges,
                architectural_post_c63_boundary_edges_total=self.edges-self.model_only,model_only_post_c63_boundary_edges_total=self.model_only,
                edge_kind_counts={k:self.kinds[k] for k in c57.KINDS},target_metadata_relation_counts={k:self.relations[k] for k in c57.RELATIONS},
                exact_runtime_begin_edge_kind_counts={k:self.exact[k] for k in c57.KINDS},interior_target_edge_kind_counts={k:self.interior[k] for k in c57.KINDS},
                metadata_free_RX_edge_kind_counts={k:self.free[k] for k in c57.KINDS},exact_adjacency_counts={k:self.adjacent[k] for k in c57.KINDS},
                terminal_phase_counts=dict(sorted(self.phases.items())),terminal_reason_counts=dict(sorted(self.reasons.items())),underlying_refusal_counts=dict(sorted(self.causes.items())),
                c63_authorization_phase_counts=dict(sorted(self.c63_phases.items())),c63_authorization_reason_counts=dict(sorted(self.c63_reasons.items())),
                authenticated_record_set_sha256=self.records_sha.hexdigest(),examples_per_class=self.limit,examples_by_edge_kind=self.examples_kind,examples_by_target_relation=self.examples_relation,
                classification_complete=True,new_post_c63_boundaries_admitted=0,cross_runtime_fact_propagation_unchanged=True,c64_candidate_output_unchanged=True,c64_candidate_output_sha256=c56.digest(b.result['reference_candidates']),
                historical_c64_candidate_counts={k:b.result[k] for k in ('reference_candidate_count','new_cross_row_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates')},
                new_cross_row_candidate_count=0,publication_source_kind='unproven',live_qword_reads=0,state_scope='entry_seed_not_instruction_must_state_stored_path_separate_from_post_origin',
                diagnostic_scope='unchanged_c64_first_terminal_POST_edge_callback_per_root_no_post_abort_exploration',post_c63_boundary_edges_scope='row_external_terminal_successors_unchanged_c57_vocabulary',
                c56_through_c64_evidence_read_only=True,c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',**{k:False for k in c56.FALSE_GATES})
        except (ValueError,KeyError,TypeError,IndexError,RecursionError):self.poisoned = True;raise


def classify(baseline,progress=None):
    aggregate = Aggregate(baseline)
    for record in replay_records(baseline,progress):aggregate.add(record)
    return aggregate.finish()


def analyze(data,cache=None,c57_json=None,c58_json=None,c59_json=None,c60_json=None,c61_json=None,c62_json=None,c63_json=None,c64_json=None,progress=None):
    if len(data)>cfg.old.MAX_FILE or c56.sha(data)!=EXPECTED_SHA:raise c58.EvidenceError('exact executable SHA256 mismatch or input cap')
    s = c56.Session(data);old = c57.collect_baseline(s,cache,progress=progress)
    e = c58.Evidence(old,owner_result=c56.read_json(c57_json) if c57_json is not None else None,progress=progress)
    b = c59.Baseline(e,owner_result=c56.read_json(c58_json) if c58_json is not None else None,progress=progress)
    p = c60.ProofPool(b,owner_result=c56.read_json(c59_json) if c59_json is not None else None,progress=progress)
    original = c61.Baseline(p,owner_result=c56.read_json(c60_json) if c60_json is not None else None,progress=progress)
    cert = c62.ProofPool(original,owner_result=c56.read_json(c61_json) if c61_json is not None else None,progress=progress)
    prior = c63.Baseline(cert,owner_result=c56.read_json(c62_json) if c62_json is not None else None,progress=progress)
    cert64 = c64.ProofPool(prior,owner_result=c56.read_json(c63_json) if c63_json is not None else None,progress=progress)
    return classify(Baseline(cert64,owner_result=c56.read_json(c64_json) if c64_json is not None else None,progress=progress),progress)


def main():
    parser = argparse.ArgumentParser(description=__doc__);parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    for flag in ('c56-cache','c57-json','c58-json','c59-json','c60-json','c61-json','c62-json','c63-json','c64-json'):parser.add_argument('--'+flag,type=Path)
    a = parser.parse_args()
    try:
        output = a.output.resolve();inputs = [p for p in (a.exe,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,a.c62_json,a.c63_json,a.c64_json) if p is not None]
        if any(output==p.resolve() for p in inputs) or (a.c56_cache and output.is_relative_to(a.c56_cache.resolve())):raise c58.EvidenceError('output overlaps immutable input evidence')
        if a.exe.stat().st_size>cfg.old.MAX_FILE:raise c58.EvidenceError('input file cap')
        result = analyze(a.exe.read_bytes(),a.c56_cache,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,a.c62_json,a.c63_json,a.c64_json,
            lambda phase,n,total:print(f'{phase}: {n}/{total}',file=sys.stderr));c56.atomic_json(a.output,result)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(f'{BLOCKED}: {error}',file=sys.stderr);return 2
    print(f'{READY}: {a.output}');return 0


if __name__=='__main__':raise SystemExit(main())
