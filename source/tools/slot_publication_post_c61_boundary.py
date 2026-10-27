"""c63 OFFLINE diagnostic of c62's first terminal post-c61 boundary.

Original c62 transactions/admission run unchanged. The observer captures only an
existing terminal edge refusal and rethrows it; full output must match c62.
Stored paths, post-arrival origins and preflight/crossing phases stay separate.
"""
import argparse
from collections import Counter
from copy import deepcopy
from dataclasses import asdict
import hashlib
from pathlib import Path
import sys

import slot_publication_post_c59_stitched as c62

c61,c60,c59,c58,c57,c56,cfg = c62.c61,c62.c60,c62.c59,c62.c58,c62.c57,c62.c56,c62.cfg
VERSION = 'v0.1c63'
SCHEMA = 1
CONTRACT = 'exact_c62_first_terminal_post_c61_boundary_full_prefix_diagnostic_v1'
READY = 'POST_C61_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY'
BLOCKED = 'POST_C61_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED'
EXPECTED_SHA = c62.EXPECTED_SHA
PHASES = ('same_instance_after_c61_preflight_before_row_fixpoint','post_c61_reachable_context',
    'post_c61_reachable_dedup_stored_earlier_path')
ACCEPTED_C62 = dict(repository_result=c62.READY,tool_version=c62.VERSION,schema_version=c62.SCHEMA,
    accepted_c61_classifier_identity='a828ec479e0f3279ddaf117d76ff827652155c357a8415606cbe2924e479d349',
    accepted_c61_record_set_identity='f5bc25085c62e7f9a32eab74ade7af97e076c9b03c99f796cc06d179ad5f724e',
    accepted_c61_result_identity='7982be4b9e602fd4c01d84d2e1c9e8c169ed330508bd31ae79206c72ad969341',
    root_transactions_attempted=162472,root_transactions_completed=103933,roots_completed=103933,root_transactions_refused=58539,
    c61_post_boundary_instances_available=6566,c61_post_architectural_edges_available=7206,c61_post_exact_edges_available=6946,
    c61_boundary_instances_considered=6566,c61_boundary_instances_admitted=3204,c61_boundary_instances_refused=3362,
    c61_exact_edges_considered=6946,c61_exact_edges_admitted=3765,c61_exact_edges_refused=3181,
    c61_terminal_phase_considered=dict(zip(c62.PHASES,(5759,805,2))),
    c61_terminal_phase_admitted=dict(zip(c62.PHASES,(2800,403,1))),c61_terminal_phase_refused=dict(zip(c62.PHASES,(2959,402,1))),
    c61_all_successor_preflight_refusal_relation_counts=dict(inside_unique_runtime_span_nonentry=245,
        executable_file_backed_outside_runtime_metadata=15),
    c61_certificate_mismatch_counts=dict(context_phase_origin_or_full_prefix_mismatch=15),
    continuation_instances_created=8798,continuation_instances_deduplicated=51,continuation_instances_refused=6590,
    rows_reached_by_continuation=1317,address_facts_crossing_runtime_rows=2635,c61_certified_crossings_with_facts=13,
    c61_address_facts_crossing_rows=16,call_edges_with_preserved_facts_crossing_rows=0,cross_row_cap_refusals=44,cross_row_cycles_seen=0,
    post_c61_uncertified_boundary_refusals=2923,
    root_refusal_reasons=dict(inside_unique_runtime_span_nonentry=31696,executable_file_backed_outside_runtime_metadata=18700,
        uncertified_post_c61_runtime_boundary=2923,indirect_call=4884,indirect_jump=253,summary_cap_refused=35,
        system_or_unsupported_transfer=31,instruction_cap=9,decode_gap=5,uncertified_later_runtime_boundary=3),
    new_cross_row_candidate_count=0,reference_candidate_count=0,reader_candidates=0,writer_candidates=0,rmw_candidates=0,address_only_candidates=0)


def sources():
    return dict(c62.sources(),slot_publication_post_c61_boundary_py=c56.sha(Path(__file__).read_bytes()))


class Baseline:
    def __init__(self,certificates,*,owner_result=None,accepted=None,progress=None):
        self.certificates = certificates
        self.result = c62.scan(certificates,progress=progress)
        expected = ACCEPTED_C62 if accepted is None else accepted  # owned seam, no CLI bypass
        if any(c56.encoded(self.result.get(k))!=c56.encoded(v) for k,v in expected.items()):
            raise c58.EvidenceError('accepted c62 identity/count/phase/result mismatch')
        if owner_result is not None and c56.encoded(owner_result)!=c56.encoded(self.result):
            raise c58.EvidenceError('stale/wrong image/tool/schema/contract/record-set/result c62 JSON')
        if (self.result['repository_result']!=c62.READY or self.result['tool_version']!=c62.VERSION
            or self.result['schema_version']!=c62.SCHEMA or self.result['contract']!=c62.CONTRACT
            or self.result['authentication_header']!=certificates.header or self.result['limits']!=asdict(c58.Limits())
            or any(self.result[k] for k in c56.FALSE_GATES)):
            raise c58.EvidenceError('wrong c62 contract/limits/complete proof chain/gates')
        self.header = dict(tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_sha256=self.result['image_sha256'],c56_manifest_id=certificates.header['c56_manifest_id'],
            **{k:v for k,v in certificates.header.items() if k.startswith('accepted_')},
            accepted_c62_authentication_identity=certificates.identity,accepted_c62_result_identity=c56.digest(self.result),
            c62_contract=c62.CONTRACT,c62_limits=self.result['limits'],
            post_c61_uncertified_boundary_refused_roots_expected=self.result['post_c61_uncertified_boundary_refusals'],
            tool_sources=sources())
        self.identity = c56.digest(self.header)

    def assert_current(self):
        self.certificates.assert_current()
        if (self.header.get('tool_sources')!=sources() or c56.digest(self.header)!=self.identity
            or c56.digest(self.result)!=self.header['accepted_c62_result_identity']
            or self.certificates.identity!=self.header['accepted_c62_authentication_identity']
            or self.certificates.header!=self.result['authentication_header']):
            raise c58.EvidenceError('stale/mutated c63 schema/tool/contract/c62 result identity')


def path_summary(path):
    return dict(c61.path_summary(path),
        c61_record_identities=[p['c61_record_identity'] for p in path if p.get('proof_kind')=='c61_post_boundary'],
        c61_full_prefix_identities=[p['c61_full_prefix_identity'] for p in path if p.get('proof_kind')=='c61_post_boundary'])


def capture_for(tx,view,index,ins):
    key = view.key
    if key is None or key[0]!=index or key not in tx.contexts:raise c58.EvidenceError('terminal c62 POST lacks current source context')
    state,path = tx.contexts[key]
    if key[1]!=c58.state_id(state):raise c58.EvidenceError('wrong terminal incoming state')
    authorized = view.c61_record
    if authorized is None or c56.encoded(authorized)!=c56.encoded(view.certificates.lookup(tx.root)):
        raise c58.EvidenceError('terminal POST lacks exact authenticated c61 authorization')
    origin = tx.post61_origins.get(key)
    if key in tx.post_c61:
        if origin is None:raise c58.EvidenceError('post-c61 reachability missing diagnostic origin')
        phase = PHASES[1] if any(p.get('proof_kind')=='c61_post_boundary' for p in path) else PHASES[2]
    elif key in view.c61_source_keys:
        phase = PHASES[0]
    else:raise c58.EvidenceError('terminal POST not justified by original c62 state')
    _,_,code,_ = view.row(index)
    return dict(root=tx.root,row=index,state_identity=key[1],path=path_summary(path),
        entry_seed_fact_count=sum(f is not None for f in state),pc=ins.address,size=ins.size,
        instruction_sha256=c56.sha(bytes(ins.bytes)),row_sha256=view.session.row_identity(index),code_sha256=c56.sha(code),
        phase=phase,post_c61_origin=deepcopy(origin),post_c59_origin=deepcopy(tx.post_origins.get(key)),
        c61_authorization={k:deepcopy(authorized[k]) for k in ('record_identity','classifier_identity','root_replay_identity',
            'entry_instance_identity','current_entry_instance_row','incoming_state_identity','boundary_path_identity',
            'boundary_transition_identities','source_instruction_rva','source_instruction_sha256','terminal_phase','post_c59_origin',
            'local_c59_record_identities','c59_record_identities')},
        local_c61_edge_identities=sorted(p['edge_identity'] for p in view.proofs_at_c61.values()),
        c61_source_contexts=sorted(view.c61_source_keys),c61_scan_token=view.c61_scan_token,current_scan_token=tx.scan_token)


class View(c62.View):
    def __init__(self,certificates):
        super().__init__(certificates)
        self.diagnostic_capture = None  # independent of c61's original .capture

    def edge(self,index,ins,kind,target,architectural,relations):
        try:return super().edge(index,ins,kind,target,architectural,relations)
        except c58.Refusal as error:
            if str(error)==c62.POST:
                if self.tx is None:raise c58.EvidenceError('missing terminal transaction')
                capture = capture_for(self.tx,self,index,ins)
                if self.diagnostic_capture is not None and c56.encoded(self.diagnostic_capture)!=c56.encoded(capture):
                    raise c58.EvidenceError('multiple distinct terminal c62 source events')
                self.diagnostic_capture = capture
            raise  # Original c62 refusal; never return a c63 proof.


class Transaction(c62.Transaction):
    """Observe post-c61 arrival/propagation without changing original metered work."""
    def __init__(self,view,root,engine,limits):
        super().__init__(view,root,engine,limits)
        self.post61_origins = {}
        self.diagnostic_arrival = self.diagnostic_connect = None

    def instance(self,row,state,path):
        before = self.diagnostic_arrival
        self.diagnostic_arrival = ((row,c58.state_id(state)),path)
        try:return super().instance(row,state,path)
        finally:self.diagnostic_arrival = before

    def connect(self,source,target):
        before = self.diagnostic_connect;self.diagnostic_connect = (source,target)
        try:return super().connect(source,target)
        finally:self.diagnostic_connect = before

    def mark_post_c61(self,key):
        before = set(self.post_c61)
        super().mark_post_c61(key)  # Invalidation-only POST has no invented edge PC.
        added = self.post_c61-before
        if not added and key in self.post61_origins:return
        if self.diagnostic_arrival is not None and self.diagnostic_arrival[0]==key:
            origin = dict(kind='authenticated_c61_arrival',entry_context=key,
                arrival_path=path_summary(self.diagnostic_arrival[1]),propagation_contexts=[key])
            if not origin['arrival_path']['c61_record_identities']:raise c58.EvidenceError('post arrival lacks c61 proof')
        elif self.diagnostic_connect is not None and self.diagnostic_connect[1]==key:
            source,_ = self.diagnostic_connect
            if source not in self.post61_origins:raise c58.EvidenceError('post propagation lacks c61 source origin')
            origin = deepcopy(self.post61_origins[source]);origin['propagation_contexts'].append(key)
        else:raise c58.EvidenceError('unexpected original c62 post marking call path')
        if len(origin['propagation_contexts'])>self.limits.instances:raise c58.EvidenceError('bounded diagnostic post origin cap')
        # Original closure computed/charged already. No new original closure/cap.
        for target in sorted(added):self.post61_origins[target] = deepcopy(origin)
        self.post61_origins.setdefault(key,origin)


def record_for(baseline,tx,view):
    capture = view.diagnostic_capture
    if capture is None:raise c58.EvidenceError('terminal c62 POST without source edge callback; diagnostic source unavailable')
    s = view.session;index = capture['row']
    begin,end,code,_ = view.row(index);pc,size = capture['pc'],capture['size']
    if not begin<=pc<end or not 1<=size<=15:raise c58.EvidenceError('invalid terminal source span')
    ins = next(cfg.old.decoder().disasm(code[pc-begin:pc-begin+size],pc,count=1),None)
    if (ins is None or ins.size!=size or c56.encoded(capture)!=c56.encoded(capture_for(tx,view,index,ins))):
        raise c58.EvidenceError('stale/wrong terminal root/state/path/source/phase/origin')
    allowed = lambda target:any(b<=target<min(e,b+n) and flags&0x60000000==0x60000000 and not flags&0x80000000
        for b,e,_,n,flags in s.sections)
    try:cfg.successors(ins,begin,end,s.size,allowed)
    except ValueError as error:
        if str(error)!='unsupported_runtime_boundary':raise c58.EvidenceError('terminal source not original boundary oracle refusal') from error
    else:raise c58.EvidenceError('terminal source has no outgoing boundary')
    edges = c59.classify_edges(s,begin,end,ins)
    if not 1<=len(edges)<=2:raise c58.EvidenceError('missing terminal source successor')
    snapshot = dict(contexts=[(k,c56.digest(p)) for k,(_,p) in sorted(tx.contexts.items())],
        forward=[(k,sorted(tx.forward[k])) for k in sorted(tx.contexts)],reverse=[(k,sorted(tx.reverse[k])) for k in sorted(tx.contexts)],
        post_c59=sorted(tx.post_c59),post_c61=sorted(tx.post_c61),path_tainted=sorted(tx.path_tainted),
        work=dict(tx.work),relations=dict(tx.relations),created=tx.created,deduplicated=tx.deduplicated,cycles=tx.cycles,
        considered=sorted(tx.considered),admitted=sorted(tx.admitted),references_sha256=c56.digest(tx.references),
        c59_certified_contexts=sorted(view.certified_contexts),used_boundary_contexts=sorted(view.used_boundary_contexts),
        c61_considered=sorted(view.c61_considered),c61_eligible=sorted(view.c61_eligible),
        c61_edges_considered=sorted(view.c61_edges_considered),c61_edges_eligible=sorted(view.c61_edges_eligible),
        c61_preflight_refusals=dict(view.c61_refusals),c61_mismatches=dict(view.c61_mismatches),
        c61_proofs_identity=c56.digest(sorted(view.proofs_at_c61.items())),capture=capture)
    record = dict(classifier_identity=baseline.identity,**{k:v for k,v in baseline.header.items() if k.startswith('accepted_')},
        original_root_runtime_row=tx.root,original_root_row_sha256=s.row_identity(tx.root),current_entry_instance_row=index,
        incoming_state_identity=capture['state_identity'],entry_seed_fact_count=capture['entry_seed_fact_count'],
        entry_instance_identity=c56.digest((baseline.certificates.identity,tx.root,index,capture['state_identity'],capture['path']['boundary_path_identity'])),
        **capture['path'],source_runtime_row_sha256=capture['row_sha256'],source_code_sha256=capture['code_sha256'],
        source_begin_rva=begin,source_end_rva=end,source_instruction_rva=pc,source_instruction_size=size,
        source_instruction_sha256=capture['instruction_sha256'],source_mnemonic=ins.mnemonic,first_refusal=c62.POST,
        terminal_phase=capture['phase'],post_c61_origin=capture['post_c61_origin'],post_c59_origin=capture['post_c59_origin'],
        c61_authorization=capture['c61_authorization'],c61_terminal_phase=capture['c61_authorization']['terminal_phase'],
        local_c61_edge_identities=capture['local_c61_edge_identities'],root_replay_identity=c56.digest(snapshot),edges=edges,
        state_scope='authenticated_entry_seed_not_source_instruction_must_state',
        new_post_c61_boundaries_admitted=0,new_candidates_emitted=0)
    record['record_identity'] = c56.digest(record)
    return record


class Audit:
    """Reconstruct every dynamic c62 output field, including exact candidate bytes."""
    def __init__(self):
        self.old = c59.ReplayAudit()
        self.considered,self.admitted,self.edges_considered,self.edges_admitted = set(),set(),set(),set()
        self.preflight,self.mismatches,self.extra,self.phase_by_id,self.refs = Counter(),Counter(),Counter(),{},[]

    def add(self,tx,view,reason,certificates):
        self.old.add(tx,reason)
        self.considered.update(view.c61_considered);self.edges_considered.update(view.c61_edges_considered)
        self.preflight.update(view.c61_refusals);self.mismatches.update(view.c61_mismatches)
        record = certificates.lookup(tx.root)
        if record is not None:self.phase_by_id[record['record_identity']] = record['terminal_phase']
        if reason is None:
            self.admitted.update(view.c61_eligible);self.edges_admitted.update(view.c61_edges_eligible)
            self.extra['crossings'] += len(tx.c61_crossings);self.extra['facts'] += sum(n for _,n in tx.c61_crossings)
            for original in tx.references:
                ref = deepcopy(original);path = ref['boundary_path']
                ref.update(c62_authentication_identity=certificates.identity,boundary_path_identity=c56.digest(path),
                    c62_entry_instance_identity=c56.digest((certificates.identity,tx.root,ref['continuation_runtime_row'],
                        ref['incoming_state_identity'],c56.digest(path))),
                    path_c59_record_identities=[p['c59_record_identity'] for p in path if p.get('proof_kind')=='c59_later_boundary'],
                    path_c61_record_identities=[p['c61_record_identity'] for p in path if p.get('proof_kind')=='c61_post_boundary'],
                    root_c61_record_identities=sorted(view.c61_eligible))
                if len(self.refs)<c58.Limits().retained_references:self.refs.append(ref)

    def finish(self,original):
        t,ops = self.old.totals,self.old.ops;count = sum(ops.values())
        phases = lambda ids:{p:sum(self.phase_by_id[i]==p for i in ids) for p in c62.PHASES}
        projection = dict(c61_boundary_instances_considered=len(self.considered),c61_boundary_instances_admitted=len(self.admitted),
            c61_boundary_instances_refused=len(self.considered-self.admitted),c61_exact_edges_considered=len(self.edges_considered),
            c61_exact_edges_admitted=len(self.edges_admitted),c61_exact_edges_refused=len(self.edges_considered-self.edges_admitted),
            c61_terminal_phase_considered=phases(self.considered),c61_terminal_phase_admitted=phases(self.admitted),
            c61_terminal_phase_refused=phases(self.considered-self.admitted),c61_all_successor_preflight_refusal_relation_counts=dict(self.preflight),
            c61_certificate_mismatch_counts=dict(self.mismatches),c61_certified_crossings_with_facts=self.extra['crossings'],
            c61_address_facts_crossing_rows=self.extra['facts'],root_transactions_attempted=t['root_transactions_attempted'],
            root_transactions_completed=t['roots_completed'],roots_completed=t['roots_completed'],root_transactions_refused=sum(self.old.reasons.values()),
            root_refusal_reasons=dict(self.old.reasons),post_c61_uncertified_boundary_refusals=self.old.reasons[c62.POST],
            rows_reached_by_continuation=len(self.old.rows),remaining_boundary_refusal_relation_counts={k:self.old.relations[k] for k in c57.RELATIONS},
            reference_candidate_count=count,new_cross_row_candidate_count=count,reader_candidates=ops['READ'],writer_candidates=ops['WRITE'],
            rmw_candidates=ops['RMW'],address_only_candidates=ops['ADDRESS_ONLY'],reference_candidates=self.refs,candidate_records_not_retained=count-len(self.refs))
        for k in ('continuation_instances_created','continuation_instances_deduplicated','continuation_instances_refused','cross_row_cap_refusals',
            'cross_row_cycles_seen','address_facts_crossing_runtime_rows','call_edges_with_preserved_facts_crossing_rows','no_carried_fact_edges'):
            projection[k] = t[k]
        if any(c56.encoded(original.get(k))!=c56.encoded(v) for k,v in projection.items()):
            raise c58.EvidenceError('observer changed original c62 accounting/candidate bytes')


def replay_records(baseline,progress=None):
    baseline.assert_current()
    certificates = baseline.certificates;e = certificates.baseline.pool.baseline.evidence;audit = Audit()
    engine = shard = None;count = 0
    for number,root in enumerate(e.proofs,1):
        current = root // e.session.profile.rows_per_shard
        if current!=shard:engine = c58.c55.Engine(e.session.domain,e.session.profile.summary);shard = current
        view = View(certificates);tx = Transaction(view,root,engine,c58.Limits());reason = None
        try:tx.run()
        except c58.EvidenceError:raise
        except (c58.Refusal,ValueError) as error:reason = str(error)
        audit.add(tx,view,reason,certificates)
        if reason==c62.POST:
            count += 1;yield record_for(baseline,tx,view)
        elif view.diagnostic_capture is not None:raise c58.EvidenceError('POST capture not terminal root outcome')
        if progress and (number%1024==0 or number==len(e.proofs)):progress('c63 unchanged c62 terminal observer replay',number,len(e.proofs))
    audit.finish(baseline.result)
    if count!=baseline.header['post_c61_uncertified_boundary_refused_roots_expected']:
        raise c58.EvidenceError('missing accepted c62 terminal POST root domain')
    baseline.assert_current()


class Aggregate:
    def __init__(self,baseline,example_limit=8):
        if type(example_limit)!=int or not 0<=example_limit<=8:raise ValueError('invalid example bound')
        self.baseline,self.limit,self.expected = baseline,example_limit,iter(replay_records(baseline))
        self.poisoned = False
        self.roots,self.instances,self.states,self.rows = set(),set(),set(),set()
        self.count = self.edges = self.model_only = 0
        self.kinds,self.relations,self.exact,self.interior,self.free,self.adjacent,self.phases,self.c61_phases = (Counter() for _ in range(8))
        self.examples_kind = {k:[] for k in c57.KINDS};self.examples_relation = {k:[] for k in c57.RELATIONS}
        self.records_sha = hashlib.sha256()

    def add(self,record):
        if self.poisoned:raise c58.EvidenceError('POST aggregate already poisoned')
        try:
            expected = next(self.expected,None)
            if expected is None or type(record)!=dict or c56.encoded(record)!=c56.encoded(expected):
                raise c58.EvidenceError('missing/duplicate/out-of-order/stale/wrong root/state/path/source/prefix/phase/origin record')
            root = record['original_root_runtime_row']
            if root in self.roots:raise c58.EvidenceError('multiple records for one original terminal root')
            self.count += 1;self.roots.add(root);self.instances.add(record['record_identity'])
            self.states.add((root,record['current_entry_instance_row'],record['incoming_state_identity'],record['boundary_path_identity']))
            self.rows.add(record['current_entry_instance_row']);self.phases[record['terminal_phase']] += 1;self.c61_phases[record['c61_terminal_phase']] += 1
            self.records_sha.update(c56.encoded(record)+b'\n')
            for edge in record['edges']:
                kind,relation = edge['edge_kind'],edge['target_metadata_relation']
                self.edges += 1;self.model_only += not edge['architectural_successor'];self.kinds[kind] += 1;self.relations[relation] += 1
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
            if (len(self.roots)!=b.header['post_c61_uncertified_boundary_refused_roots_expected'] or self.count!=len(self.instances)
                or self.count!=len(self.states) or sum(self.kinds.values())!=self.edges or sum(self.relations.values())!=self.edges):
                raise c58.EvidenceError('POST diagnostic count mismatch')
            return dict(repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
                evidence='OFFLINE_STATIC_DIAGNOSTIC_ONLY',image_authenticated=True,image_sha256=b.header['image_sha256'],
                classifier_header=b.header,classifier_identity=b.identity,**{k:v for k,v in b.header.items() if k.startswith('accepted_')},
                post_c61_uncertified_boundary_refused_roots_expected=b.header['post_c61_uncertified_boundary_refused_roots_expected'],
                post_c61_uncertified_boundary_refused_roots_classified=len(self.roots),post_c61_boundary_instances_total=self.count,
                unique_entry_instance_states=len(self.states),unique_source_rows=len(self.rows),post_c61_boundary_edges_total=self.edges,
                architectural_post_c61_boundary_edges_total=self.edges-self.model_only,model_only_post_c61_boundary_edges_total=self.model_only,
                edge_kind_counts={k:self.kinds[k] for k in c57.KINDS},target_metadata_relation_counts={k:self.relations[k] for k in c57.RELATIONS},
                exact_runtime_begin_edge_kind_counts={k:self.exact[k] for k in c57.KINDS},interior_target_edge_kind_counts={k:self.interior[k] for k in c57.KINDS},
                metadata_free_RX_edge_kind_counts={k:self.free[k] for k in c57.KINDS},exact_adjacency_counts={k:self.adjacent[k] for k in c57.KINDS},
                terminal_phase_counts=dict(sorted(self.phases.items())),c61_authorization_phase_counts=dict(sorted(self.c61_phases.items())),
                authenticated_record_set_sha256=self.records_sha.hexdigest(),examples_per_class=self.limit,
                examples_by_edge_kind=self.examples_kind,examples_by_target_relation=self.examples_relation,classification_complete=True,
                new_post_c61_boundaries_admitted=0,cross_runtime_fact_propagation_unchanged=True,c62_candidate_output_unchanged=True,
                c62_candidate_output_sha256=c56.digest(b.result['reference_candidates']),
                historical_c62_candidate_counts={k:b.result[k] for k in ('reference_candidate_count','new_cross_row_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates')},
                new_cross_row_candidate_count=0,publication_source_kind='unproven',live_qword_reads=0,
                state_scope='entry_seed_not_instruction_must_state_stored_first_path_separate_from_post_origin',
                diagnostic_scope='unchanged_c62_first_terminal_POST_edge_callback_per_root_no_post_abort_exploration',
                post_c61_boundary_edges_scope='row_external_successors_of_terminal_instruction_unchanged_c57_vocabulary',
                c56_through_c62_evidence_read_only=True,c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
                **{k:False for k in c56.FALSE_GATES})
        except (ValueError,KeyError,TypeError,IndexError,RecursionError):self.poisoned = True;raise


def classify(baseline,progress=None):
    aggregate = Aggregate(baseline)
    for record in replay_records(baseline,progress):aggregate.add(record)
    return aggregate.finish()


def analyze(data,cache=None,c57_json=None,c58_json=None,c59_json=None,c60_json=None,c61_json=None,c62_json=None,progress=None):
    if len(data)>cfg.old.MAX_FILE or c56.sha(data)!=EXPECTED_SHA:raise c58.EvidenceError('exact executable SHA256 mismatch or input cap')
    s = c56.Session(data);old = c57.collect_baseline(s,cache,progress=progress)
    e = c58.Evidence(old,owner_result=c56.read_json(c57_json) if c57_json is not None else None,progress=progress)
    b = c59.Baseline(e,owner_result=c56.read_json(c58_json) if c58_json is not None else None,progress=progress)
    p = c60.ProofPool(b,owner_result=c56.read_json(c59_json) if c59_json is not None else None,progress=progress)
    original = c61.Baseline(p,owner_result=c56.read_json(c60_json) if c60_json is not None else None,progress=progress)
    certificates = c62.ProofPool(original,owner_result=c56.read_json(c61_json) if c61_json is not None else None,progress=progress)
    return classify(Baseline(certificates,owner_result=c56.read_json(c62_json) if c62_json is not None else None,progress=progress),progress)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    for flag in ('c56-cache','c57-json','c58-json','c59-json','c60-json','c61-json','c62-json'):parser.add_argument('--'+flag,type=Path)
    a = parser.parse_args()
    try:
        output = a.output.resolve();inputs = [p for p in (a.exe,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,a.c62_json) if p is not None]
        if any(output==p.resolve() for p in inputs) or (a.c56_cache and output.is_relative_to(a.c56_cache.resolve())):
            raise c58.EvidenceError('output overlaps immutable input evidence')
        if a.exe.stat().st_size>cfg.old.MAX_FILE:raise c58.EvidenceError('input file cap')
        result = analyze(a.exe.read_bytes(),a.c56_cache,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,a.c62_json,
            lambda phase,n,total:print(f'{phase}: {n}/{total}',file=sys.stderr))
        c56.atomic_json(a.output,result)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(f'{BLOCKED}: {error}',file=sys.stderr);return 2
    print(f'{READY}: {a.output}');return 0


if __name__=='__main__':raise SystemExit(main())
