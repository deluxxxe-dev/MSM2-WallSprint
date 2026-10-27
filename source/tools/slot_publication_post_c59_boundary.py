"""c61 OFFLINE diagnostic of the exact accepted c60 first terminal POST set.

Original c60 edge/transaction methods run unchanged. The observer records a
terminal refusal and rethrows it; all counters/candidate bytes must equal original
c60. Stored first path and post-c59 arrival provenance are distinct diagnostics.
"""
import argparse
from collections import Counter
from copy import deepcopy
import hashlib
from pathlib import Path
import sys

import slot_publication_later_stitched as c60

c59,c58,c57,c56,cfg = c60.c59,c60.c58,c60.c57,c60.c56,c60.cfg
VERSION = 'v0.1c61'
SCHEMA = 1
CONTRACT = 'exact_c60_first_terminal_post_c59_boundary_instance_diagnostic_v1'
READY = 'POST_C59_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY'
BLOCKED = 'POST_C59_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED'
EXPECTED_SHA = c60.EXPECTED_SHA
ACCEPTED_C60 = dict(repository_result=c60.READY,tool_version=c60.VERSION,schema_version=c60.SCHEMA,
    accepted_c57_boundary_identity='b990e0d13ed735418fda2669a8e54fec4f7e0b5647d846a4fbaf99eb80639158',
    accepted_c58_proof_identity='9fbdd2552a4e42fe592ebc41ab2f5b9feb65d67349aee43742364140fda10d8f',
    accepted_c58_result_identity='718013be12205027c1e0ae777e70bd2a49602ad002bc6f26242f35fece148827',
    accepted_c59_classifier_identity='d4c9d6ad7e9403444ba97d5d181be56fd687b6c58cf401bed88940a2af7bb2e6',
    accepted_c59_record_set_identity='58695b4cf4df22dff856a9436c8bde13cd013b167deaaffb3f6f78872619fcea',
    accepted_c59_result_identity='6325fc846d9e7436df1255268f0c03c931fba155e00466b06572aa8127157a5b',
    c59_boundary_instances_considered=33719,c59_boundary_instances_admitted=18833,c59_boundary_instances_refused=14886,
    c59_exact_edges_considered=28201,c59_exact_edges_admitted=20570,c59_exact_edges_refused=7631,
    c59_all_successor_preflight_refusal_relation_counts=dict(inside_unique_runtime_span_nonentry=1566,
        executable_file_backed_outside_runtime_metadata=6154),
    continuation_instances_created=8425,continuation_instances_deduplicated=24,continuation_instances_refused=6757,
    rows_reached_by_continuation=794,address_facts_crossing_runtime_rows=2062,
    c59_certified_crossings_with_facts=187,c59_address_facts_crossing_rows=194,
    call_edges_with_preserved_facts_crossing_rows=0,cross_row_cap_refusals=41,cross_row_cycles_seen=0,
    root_transactions_attempted=162472,root_transactions_completed=100729,roots_completed=100729,
    root_transactions_refused=61743,post_c59_uncertified_boundary_refusals=6566,
    pre_c59_unmatched_instance_refusals=3,c59_certificate_lookup_misses=6569,c59_predecessor_path_conflict_refusals=0,
    root_refusal_reasons=dict(inside_unique_runtime_span_nonentry=31449,
        executable_file_backed_outside_runtime_metadata=18684,uncertified_post_c59_runtime_boundary=6566,
        indirect_call=4720,indirect_jump=244,summary_cap_refused=33,system_or_unsupported_transfer=31,
        instruction_cap=8,decode_gap=5,uncertified_later_runtime_boundary=3),
    remaining_boundary_refusal_relation_counts={k:dict(exact_unique_runtime_begin=36140,
        inside_unique_runtime_span_nonentry=31694,executable_file_backed_outside_runtime_metadata=18699).get(k,0)
        for k in c57.RELATIONS},reference_candidate_count=0,new_cross_row_candidate_count=0,
    reader_candidates=0,writer_candidates=0,rmw_candidates=0,address_only_candidates=0)


def sources():
    return dict(c60.sources(),slot_publication_post_c59_boundary_py=c56.sha(Path(__file__).read_bytes()))


class Baseline:
    def __init__(self,pool,*,owner_result=None,accepted=None,progress=None):
        self.pool = pool
        self.result = c60.scan(pool,progress=progress)
        expected = ACCEPTED_C60 if accepted is None else accepted  # owned seam, not CLI override
        if any(c56.encoded(self.result.get(k))!=c56.encoded(v) for k,v in expected.items()):
            raise c58.EvidenceError('accepted c60 result/identity/count mismatch')
        if owner_result is not None and c56.encoded(owner_result)!=c56.encoded(self.result):
            raise c58.EvidenceError('stale/wrong image/tool/schema/contract/caps/result c60 JSON')
        if (self.result['contract']!=c60.CONTRACT or self.result['authentication_header']!=pool.header
            or self.result['limits']!=pool.header['limits'] or any(self.result[k] for k in c56.FALSE_GATES)):
            raise c58.EvidenceError('wrong c60 contract/proof/limits/gates')
        if accepted is None and pool.header['c56_manifest_id']!='a93a176c58b53e0e5b16c74114ca2ab3956eeacc1d6649c7b3cac4dacc84046f':
            raise c58.EvidenceError('accepted c56 manifest mismatch')
        self.header = dict(tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_sha256=self.result['image_sha256'],c56_manifest_id=pool.header['c56_manifest_id'],
            accepted_c57_boundary_identity=pool.header['accepted_c57_boundary_identity'],
            accepted_c58_proof_identity=pool.header['accepted_c58_proof_identity'],
            accepted_c58_result_identity=pool.header['accepted_c58_result_identity'],
            accepted_c59_classifier_identity=pool.header['accepted_c59_classifier_identity'],
            accepted_c59_record_set_identity=pool.header['accepted_c59_record_set_identity'],
            accepted_c59_result_identity=pool.header['accepted_c59_result_identity'],
            accepted_c60_authentication_identity=pool.identity,accepted_c60_result_identity=c56.digest(self.result),
            c60_contract=c60.CONTRACT,c60_limits=self.result['limits'],
            post_c59_uncertified_boundary_refused_roots_expected=self.result['post_c59_uncertified_boundary_refusals'],
            tool_sources=sources())
        self.identity = c56.digest(self.header)

    def assert_current(self):
        self.pool.assert_current()
        if (self.header.get('tool_sources')!=sources() or c56.digest(self.header)!=self.identity
            or c56.digest(self.result)!=self.header['accepted_c60_result_identity']):
            raise c58.EvidenceError('stale/mutated c61 schema/tool/contract/c60 identity')


def path_summary(path):
    return dict(boundary_path_identity=c56.digest(path),boundary_transition_identities=[p['edge_identity'] for p in path],
        c59_record_identities=[p['c59_record_identity'] for p in path if p.get('proof_kind')=='c59_later_boundary'],
        c59_classifier_identities=[p['c59_classifier_identity'] for p in path if p.get('proof_kind')=='c59_later_boundary'])


class View(c60.View):
    def __init__(self,pool):
        super().__init__(pool)
        self.capture = None

    def edge(self,index,ins,kind,target,architectural,relations):
        try:
            return super().edge(index,ins,kind,target,architectural,relations)
        except c58.Refusal as error:
            if str(error)==c60.POST:
                tx,key = self.tx,self.key
                if tx is None or key is None or key[0]!=index:
                    raise c58.EvidenceError('terminal POST lacks exact source context')
                state,path = tx.contexts[key]
                if key[1]!=c58.state_id(state):raise c58.EvidenceError('wrong terminal incoming state')
                # Post reachability may have arrived after same-state dedup kept
                # an earlier first path, or local graph build may have already
                # preflighted a c59 edge without crossing it yet. Keep distinct.
                origin = tx.post_origins.get(key)
                local_ids = sorted({p['c59_record_identity'] for proofs in self.proofs_at.values()
                    for p in proofs.values() if p['source_entry_instance_identity']==c56.digest((self.seal,tx.root,key))})
                if key in tx.post_c59:
                    if origin is None:raise c58.EvidenceError('POST reachability missing diagnostic origin')
                    phase = 'post_c59_reachable_context' if any(p.get('proof_kind')=='c59_later_boundary' for p in path) else 'post_c59_reachable_dedup_stored_earlier_path'
                elif key in self.certified_contexts and local_ids:
                    phase = 'same_instance_after_c59_preflight_before_row_fixpoint'
                else:raise c58.EvidenceError('POST not justified by original c60 state')
                _,_,code,_ = self.row(index)
                capture = dict(root=tx.root,row=index,state_identity=key[1],path=path_summary(path),
                    entry_seed_fact_count=sum(f is not None for f in state),pc=ins.address,size=ins.size,
                    instruction_sha256=c56.sha(bytes(ins.bytes)),row_sha256=self.session.row_identity(index),
                    code_sha256=c56.sha(code),phase=phase,post_origin=deepcopy(origin),local_c59_record_identities=local_ids)
                if self.capture is not None and c56.encoded(self.capture)!=c56.encoded(capture):
                    raise c58.EvidenceError('multiple distinct POST events after first terminal refusal')
                self.capture = capture
            raise  # Preserve EXACT original outcome, including pre-c59 LATER.


class Transaction(c60.Transaction):
    """Shadow provenance only; no extra original work/cap/state mutation."""
    def __init__(self,view,root,engine,limits):
        super().__init__(view,root,engine,limits)
        self.post_origins = {}
        self.pending_arrival = self.pending_connect = None

    def instance(self,row,state,path):
        before = self.pending_arrival
        self.pending_arrival = ((row,c58.state_id(state)),path)
        try:return super().instance(row,state,path)
        finally:self.pending_arrival = before

    def connect(self,source,target):
        before = self.pending_connect
        self.pending_connect = (source,target)
        try:return super().connect(source,target)
        finally:self.pending_connect = before

    def mark_post(self,key):
        before = set(self.post_c59)
        super().mark_post(key)  # Invalidation-only POST rethrows, never invent a PC.
        if self.pending_arrival is not None and self.pending_arrival[0]==key:
            path = self.pending_arrival[1]
            origin = dict(kind='authenticated_c59_arrival',entry_context=key,
                arrival_path=path_summary(path),propagation_contexts=[key])
            if not origin['arrival_path']['c59_record_identities']:
                raise c58.EvidenceError('post arrival lacks authenticated c59 path')
        elif self.pending_connect is not None and self.pending_connect[1]==key:
            source,_ = self.pending_connect
            if source not in self.post_origins:raise c58.EvidenceError('post propagation lacks source origin')
            origin = deepcopy(self.post_origins[source])
            origin['propagation_contexts'].append(key)
        else:raise c58.EvidenceError('unexpected original post marking call path')
        if len(origin['propagation_contexts'])>self.limits.instances:
            raise c58.EvidenceError('diagnostic provenance path cap')
        # Original mark_post has already computed closure and charged its work.
        # Do not run another metered closure or affect original counters.
        for target in sorted(self.post_c59-before):self.post_origins[target] = deepcopy(origin)
        self.post_origins.setdefault(key,origin)


def record_for(baseline,tx,view):
    capture = view.capture
    if capture is None:
        # Accepted c60 misses=POST+pre-LATER excludes non-edge invalidation POST.
        # Such a different terminal shape cannot be relabeled as an instruction.
        raise c58.EvidenceError('terminal POST without source edge callback; diagnostic source unavailable')
    key = (capture['row'],capture['state_identity'])
    if capture['root']!=tx.root or key not in tx.contexts:
        raise c58.EvidenceError('wrong root/current instance')
    state,path = tx.contexts[key]
    if c58.state_id(state)!=capture['state_identity'] or path_summary(path)!=capture['path']:
        raise c58.EvidenceError('wrong POST incoming state/path')
    pool,s = baseline.pool,baseline.pool.baseline.evidence.session
    begin,end,code,_ = pool.baseline.evidence.row(capture['row'])
    if c56.sha(code)!=capture['code_sha256'] or s.row_identity(capture['row'])!=capture['row_sha256']:
        raise c58.EvidenceError('stale source row/body')
    pc,size = capture['pc'],capture['size']
    if not begin<=pc<end or not 1<=size<=15:raise c58.EvidenceError('invalid source instruction span')
    ins = next(cfg.old.decoder().disasm(code[pc-begin:pc-begin+size],pc,count=1),None)
    if ins is None or ins.size!=size or c56.sha(bytes(ins.bytes))!=capture['instruction_sha256']:
        raise c58.EvidenceError('stale source instruction bytes')
    allowed = lambda target:any(b<=target<min(e,b+n) and flags&0x60000000==0x60000000 and not flags&0x80000000
        for b,e,_,n,flags in s.sections)
    try:cfg.successors(ins,begin,end,s.size,allowed)
    except ValueError as error:
        if str(error)!='unsupported_runtime_boundary':raise c58.EvidenceError('POST source not original boundary oracle refusal') from error
    else:raise c58.EvidenceError('POST source has no outgoing boundary')
    edges = c59.classify_edges(s,begin,end,ins)
    if not 1<=len(edges)<=2:raise c58.EvidenceError('missing exact outgoing successor')
    snapshot = dict(contexts=[(k,c56.digest(p)) for k,(_,p) in sorted(tx.contexts.items())],
        graph=[(k,sorted(tx.forward[k])) for k in sorted(tx.contexts)],post_c59=sorted(tx.post_c59),
        path_tainted=sorted(tx.path_tainted),work=dict(tx.work),relations=dict(tx.relations),capture=capture)
    record = dict(classifier_identity=baseline.identity,
        accepted_c60_authentication_identity=pool.identity,accepted_c60_result_identity=baseline.header['accepted_c60_result_identity'],
        original_root_runtime_row=tx.root,original_root_row_sha256=s.row_identity(tx.root),current_entry_instance_row=key[0],
        incoming_state_identity=key[1],entry_seed_fact_count=capture['entry_seed_fact_count'],
        entry_instance_identity=c56.digest((pool.identity,tx.root,key,capture['path']['boundary_path_identity'])),
        **capture['path'],source_runtime_row_sha256=capture['row_sha256'],source_code_sha256=capture['code_sha256'],
        source_begin_rva=begin,source_end_rva=end,source_instruction_rva=pc,source_instruction_size=size,
        source_instruction_sha256=capture['instruction_sha256'],source_mnemonic=ins.mnemonic,
        first_refusal=c60.POST,terminal_phase=capture['phase'],post_c59_origin=capture['post_origin'],
        local_c59_record_identities=capture['local_c59_record_identities'],root_replay_identity=c56.digest(snapshot),edges=edges,
        state_scope='authenticated_entry_seed_not_source_instruction_must_state',
        new_post_c59_boundaries_admitted=0,new_candidates_emitted=0)
    record['record_identity'] = c56.digest(record)
    return record


class Audit:
    def __init__(self):
        self.old = c59.ReplayAudit()
        self.considered_instances,self.admitted_instances,self.considered_edges,self.admitted_edges = set(),set(),set(),set()
        self.preflight,self.extra = Counter(),Counter()
        self.refs = []

    def add(self,tx,view,reason,pool):
        self.old.add(tx,reason)
        self.considered_instances.update(view.considered_instances);self.considered_edges.update(view.considered_edges)
        self.preflight.update(view.preflight_refusals);self.extra['misses'] += view.lookup_misses
        if reason is None:
            self.admitted_instances.update(view.eligible_instances);self.admitted_edges.update(view.eligible_edges)
            self.extra['crossings'] += len(tx.later_crossings);self.extra['facts'] += sum(n for _,n in tx.later_crossings)
            for original in tx.references:
                ref = deepcopy(original);path = ref['boundary_path']
                ref.update(c60_authentication_identity=pool.identity,boundary_path_identity=c56.digest(path),
                    c60_entry_instance_identity=c56.digest((pool.identity,tx.root,ref['continuation_runtime_row'],ref['incoming_state_identity'],c56.digest(path))),
                    c59_later_record_identities=[p['c59_record_identity'] for p in path if p.get('proof_kind')=='c59_later_boundary'])
                if len(self.refs)<c58.Limits().retained_references:self.refs.append(ref)

    def finish(self,original):
        old,t = self.old,self.old.totals
        ops,count = old.ops,sum(old.ops.values())
        projection = dict(c59_boundary_instances_considered=len(self.considered_instances),
            c59_boundary_instances_admitted=len(self.admitted_instances),c59_boundary_instances_refused=len(self.considered_instances-self.admitted_instances),
            c59_exact_edges_considered=len(self.considered_edges),c59_exact_edges_admitted=len(self.admitted_edges),
            c59_exact_edges_refused=len(self.considered_edges-self.admitted_edges),c59_certified_crossings_with_facts=self.extra['crossings'],
            c59_address_facts_crossing_rows=self.extra['facts'],c59_certificate_lookup_misses=self.extra['misses'],
            c59_all_successor_preflight_refusal_relation_counts=dict(self.preflight),root_transactions_attempted=t['root_transactions_attempted'],
            root_transactions_completed=t['roots_completed'],roots_completed=t['roots_completed'],root_transactions_refused=sum(old.reasons.values()),
            root_refusal_reasons=dict(old.reasons),post_c59_uncertified_boundary_refusals=old.reasons[c60.POST],
            c59_predecessor_path_conflict_refusals=old.reasons['c59_predecessor_path_conflict'],pre_c59_unmatched_instance_refusals=old.reasons[c59.LATER],
            rows_reached_by_continuation=len(old.rows),remaining_boundary_refusal_relation_counts={k:old.relations[k] for k in c57.RELATIONS},
            reference_candidate_count=count,new_cross_row_candidate_count=count,reader_candidates=ops['READ'],writer_candidates=ops['WRITE'],
            rmw_candidates=ops['RMW'],address_only_candidates=ops['ADDRESS_ONLY'],reference_candidates=self.refs,candidate_records_not_retained=count-len(self.refs))
        for k in ('continuation_instances_created','continuation_instances_deduplicated','continuation_instances_refused','cross_row_cap_refusals',
            'cross_row_cycles_seen','address_facts_crossing_runtime_rows','call_edges_with_preserved_facts_crossing_rows','no_carried_fact_edges'):
            projection[k] = t[k]
        if any(c56.encoded(original.get(k))!=c56.encoded(v) for k,v in projection.items()):
            raise c58.EvidenceError('observer changed original c60 accounting/candidate bytes')


def replay_records(baseline,progress=None):
    baseline.assert_current()
    pool,e,audit = baseline.pool,baseline.pool.baseline.evidence,Audit()
    engine = shard = None
    count = 0
    for number,root in enumerate(e.proofs,1):
        current = root // e.session.profile.rows_per_shard
        if current!=shard:
            engine = c58.c55.Engine(e.session.domain,e.session.profile.summary);shard = current
        view = View(pool);tx = Transaction(view,root,engine,c58.Limits());reason = None
        try:tx.run()
        except c58.EvidenceError:raise
        except (c58.Refusal,ValueError) as error:reason = str(error)
        audit.add(tx,view,reason,pool)
        if reason==c60.POST:
            count += 1
            yield record_for(baseline,tx,view)
        elif view.capture is not None:raise c58.EvidenceError('POST capture does not match terminal root outcome')
        if progress and (number%1024==0 or number==len(e.proofs)):progress('c61 original c60 observer replay',number,len(e.proofs))
    audit.finish(baseline.result)
    if count!=baseline.header['post_c59_uncertified_boundary_refused_roots_expected']:
        raise c58.EvidenceError('missing accepted POST refusal root domain')
    baseline.assert_current()


class Aggregate:
    def __init__(self,baseline,example_limit=8):
        if type(example_limit)!=int or not 0<=example_limit<=8:raise ValueError('invalid example bound')
        self.baseline,self.limit,self.expected = baseline,example_limit,iter(replay_records(baseline))
        self.poisoned = False
        self.roots,self.instances,self.states,self.rows = set(),set(),set(),set()
        self.count = self.edges = self.model_only = 0
        self.kinds,self.relations,self.exact,self.interior,self.free,self.adjacent,self.phases = (Counter() for _ in range(7))
        self.examples_kind = {k:[] for k in c57.KINDS};self.examples_relation = {k:[] for k in c57.RELATIONS}
        self.records_sha = hashlib.sha256()

    def add(self,record):
        if self.poisoned:raise ValueError('POST aggregate already poisoned')
        try:
            expected = next(self.expected,None)
            if expected is None or type(record)!=dict or c56.encoded(record)!=c56.encoded(expected):
                raise c58.EvidenceError('missing/duplicate/out-of-order/stale/wrong root/state/path/source/record')
            self.count += 1;self.roots.add(record['original_root_runtime_row']);self.instances.add(record['record_identity'])
            self.states.add((record['original_root_runtime_row'],record['current_entry_instance_row'],record['incoming_state_identity'],record['boundary_path_identity']))
            self.rows.add(record['current_entry_instance_row']);self.phases[record['terminal_phase']] += 1
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
        except (ValueError,KeyError,TypeError,IndexError,RecursionError):
            self.poisoned = True
            raise

    def finish(self):
        try:
            b = self.baseline
            if self.poisoned or next(self.expected,None) is not None:
                raise c58.EvidenceError('poisoned/missing POST root or instance record')
            b.assert_current()
            if (len(self.roots)!=b.header['post_c59_uncertified_boundary_refused_roots_expected']
                or self.count!=len(self.instances) or self.count!=len(self.states)
                or sum(self.kinds.values())!=self.edges or sum(self.relations.values())!=self.edges):
                raise c58.EvidenceError('POST diagnostic count mismatch')
            return dict(repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
                evidence='OFFLINE_STATIC_DIAGNOSTIC_ONLY',image_authenticated=True,image_sha256=b.header['image_sha256'],
                classifier_header=b.header,classifier_identity=b.identity,
                **{k:b.header[k] for k in ('accepted_c57_boundary_identity','accepted_c58_proof_identity','accepted_c58_result_identity',
                    'accepted_c59_classifier_identity','accepted_c59_record_set_identity','accepted_c59_result_identity',
                    'accepted_c60_authentication_identity','accepted_c60_result_identity')},
                post_c59_uncertified_boundary_refused_roots_expected=b.header['post_c59_uncertified_boundary_refused_roots_expected'],
                post_c59_uncertified_boundary_refused_roots_classified=len(self.roots),post_c59_boundary_instances_total=self.count,
                unique_entry_instance_states=len(self.states),unique_source_rows=len(self.rows),post_c59_boundary_edges_total=self.edges,
                architectural_post_c59_boundary_edges_total=self.edges-self.model_only,model_only_post_c59_boundary_edges_total=self.model_only,
                edge_kind_counts={k:self.kinds[k] for k in c57.KINDS},target_metadata_relation_counts={k:self.relations[k] for k in c57.RELATIONS},
                exact_runtime_begin_edge_kind_counts={k:self.exact[k] for k in c57.KINDS},interior_target_edge_kind_counts={k:self.interior[k] for k in c57.KINDS},
                metadata_free_RX_edge_kind_counts={k:self.free[k] for k in c57.KINDS},exact_adjacency_counts={k:self.adjacent[k] for k in c57.KINDS},
                terminal_phase_counts=dict(sorted(self.phases.items())),authenticated_record_set_sha256=self.records_sha.hexdigest(),
                examples_per_class=self.limit,examples_by_edge_kind=self.examples_kind,examples_by_target_relation=self.examples_relation,
                classification_complete=True,new_post_c59_boundaries_admitted=0,cross_runtime_fact_propagation_unchanged=True,
                c60_candidate_output_unchanged=True,c60_candidate_output_sha256=c56.digest(b.result['reference_candidates']),
                historical_c60_candidate_counts={k:b.result[k] for k in ('reference_candidate_count','new_cross_row_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates')},
                new_cross_row_candidate_count=0,publication_source_kind='unproven',live_qword_reads=0,
                state_scope='entry_seed_not_instruction_must_state_first_stored_path_separate_from_post_origin',
                diagnostic_scope='unchanged_c60_first_terminal_POST_edge_callback_per_root_no_post_abort_exploration',
                post_c59_boundary_edges_scope='row_external_successors_of_terminal_instruction_unchanged_c57_vocabulary',
                c56_c57_c58_c59_c60_evidence_read_only=True,c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
                **{k:False for k in c56.FALSE_GATES})
        except (ValueError,KeyError,TypeError,IndexError,RecursionError):
            self.poisoned = True
            raise


def classify(baseline,progress=None):
    aggregate = Aggregate(baseline)
    for record in replay_records(baseline,progress):aggregate.add(record)
    return aggregate.finish()


def analyze(data,cache=None,c57_json=None,c58_json=None,c59_json=None,c60_json=None,progress=None):
    if len(data)>cfg.old.MAX_FILE or c56.sha(data)!=EXPECTED_SHA:raise c58.EvidenceError('exact executable SHA256 mismatch or input cap')
    s = c56.Session(data);old = c57.collect_baseline(s,cache,progress=progress)
    e = c58.Evidence(old,owner_result=c56.read_json(c57_json) if c57_json is not None else None,progress=progress)
    b = c59.Baseline(e,owner_result=c56.read_json(c58_json) if c58_json is not None else None,progress=progress)
    p = c60.ProofPool(b,owner_result=c56.read_json(c59_json) if c59_json is not None else None,progress=progress)
    original = Baseline(p,owner_result=c56.read_json(c60_json) if c60_json is not None else None,progress=progress)
    return classify(original,progress)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    for flag in ('c56-cache','c57-json','c58-json','c59-json','c60-json'):parser.add_argument('--'+flag,type=Path)
    a = parser.parse_args()
    try:
        output = a.output.resolve();inputs = [p for p in (a.exe,a.c57_json,a.c58_json,a.c59_json,a.c60_json) if p is not None]
        if any(output==p.resolve() for p in inputs) or (a.c56_cache and output.is_relative_to(a.c56_cache.resolve())):
            raise c58.EvidenceError('output overlaps immutable input evidence')
        if a.exe.stat().st_size>cfg.old.MAX_FILE:raise c58.EvidenceError('input file cap')
        result = analyze(a.exe.read_bytes(),a.c56_cache,a.c57_json,a.c58_json,a.c59_json,a.c60_json,
            lambda phase,n,total:print(f'{phase}: {n}/{total}',file=sys.stderr))
        c56.atomic_json(a.output,result)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(f'{BLOCKED}: {error}',file=sys.stderr);return 2
    print(f'{READY}: {a.output}');return 0


if __name__=='__main__':raise SystemExit(main())
