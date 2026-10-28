"""c68 OFFLINE continuation using fully replayed exact-provenance c67 records.

Only the exact authenticated c67 terminal record may authorize its complete set of
architectural exact-runtime-begin successors. One bad arm aborts the whole root;
the next unknown row-external boundary remains fail-closed.
"""
import argparse
from collections import Counter
from copy import deepcopy
from dataclasses import asdict
import json
from pathlib import Path
import sys

# Import the correction first. It patches only c67's observer globals so replay of
# the accepted corrected c67 record stream remains exact while c56-c66 stay read-only.
import slot_publication_post_c65_boundary_fix as c67fix

c67 = c67fix.base
c66,c65,c64,c63,c62,c61,c60,c59,c58,c57,c56,cfg = (
    c67.c66,c67.c65,c67.c64,c67.c63,c67.c62,c67.c61,c67.c60,c67.c59,c67.c58,c67.c57,c67.c56,c67.cfg
)
VERSION = 'v0.1c68'
SCHEMA = 1
CONTRACT = 'exact_c67_full_prefix_terminal_reason_provenance_all_successor_continuation_v1'
READY = 'POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_READY'
BLOCKED = 'POST_C65_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED'
POST = 'uncertified_post_c67_runtime_boundary'
EXPECTED_SHA = c67.EXPECTED_SHA
EXPECTED_C67_FILE_SHA = '277cb83c3fbe36715876133079c99a1d5151634442ed842960a917b140ab8aee'
EXPECTED_C67_RESULT_IDENTITY = 'ac043225b8b4464e8fe7e52a2c44a9a81c72bf0c2fad6e30df173fdab83b113d'
ACCEPTED_C67 = dict(
    repository_result=c67.READY,tool_version=c67.VERSION,schema_version=c67.SCHEMA,contract=c67.CONTRACT,
    classifier_identity='a130b082512449cd008a912b94e2e3a2bcabf15140906c4780983bac07b99f20',
    authenticated_record_set_sha256='f0b2ce8c91868e572a83d59fc425c4e40d3fe4fe9faa7957e26a252b65e85ed6',
    post_c65_uncertified_boundary_refused_roots_expected=458,post_c65_uncertified_boundary_refused_roots_classified=458,
    post_c65_boundary_instances_total=458,unique_entry_instance_states=458,unique_source_rows=445,
    post_c65_boundary_edges_total=507,architectural_post_c65_boundary_edges_total=507,model_only_post_c65_boundary_edges_total=0,
    target_metadata_relation_counts={k:dict(exact_unique_runtime_begin=432,inside_unique_runtime_span_nonentry=74,
        executable_file_backed_outside_runtime_metadata=1).get(k,0) for k in c57.RELATIONS},
    terminal_phase_counts=dict(same_instance_after_c65_preflight_before_row_fixpoint=348,post_c65_reachable_context=34,
        post_c65_reachable_dedup_stored_earlier_path=3,prior_c65_authorization_converted_earlier_refusal=73),
    terminal_reason_counts=dict(used_c65_source_later_instruction=348,current_context_post_c65_reachable=37,
        prior_c65_authorization_converted_c64_post=73),
    underlying_refusal_counts={c64.POST:73},classification_complete=True,new_post_c65_boundaries_admitted=0,
    cross_runtime_fact_propagation_unchanged=True,c66_candidate_output_unchanged=True,
    c66_candidate_output_sha256='4f53cda18c2baa0c0354bb5f9a3ecbe5ed12ab4d8e11ba873c2f11161202b945',
    new_cross_row_candidate_count=0,c56_through_c66_evidence_read_only=True)


def sources():
    return dict(c67.sources(),slot_publication_post_c65_stitched_py=c56.sha(Path(__file__).read_bytes()))


def read_c67(path):
    raw = Path(path).read_bytes()
    if len(raw)>c56.MAX_JSON or c56.sha(raw)!=EXPECTED_C67_FILE_SHA:
        raise c58.EvidenceError('stale/wrong physical c67 owner JSON')
    try:return json.loads(raw)
    except (ValueError,TypeError) as error:raise c58.EvidenceError('invalid c67 owner JSON') from error


class ProofPool:
    def __init__(self,baseline,*,owner_result=None,accepted=None,progress=None):
        self.baseline = baseline
        aggregate = c67.Aggregate(baseline);records,index = [],{}
        for record in c67.replay_records(baseline,progress):
            aggregate.add(record)
            root = record['original_root_runtime_row']
            if root in index:raise c58.EvidenceError('multiple c67 terminal records for one accepted root')
            index[root] = len(records);records.append(c56.encoded(record))
        result = aggregate.finish();expected = ACCEPTED_C67 if accepted is None else accepted
        if any(c56.encoded(result.get(k))!=c56.encoded(v) for k,v in expected.items()):
            raise c58.EvidenceError('accepted c67 identity/count/phase/reason/record-set mismatch')
        if accepted is None and c56.digest(result)!=EXPECTED_C67_RESULT_IDENTITY:
            raise c58.EvidenceError('accepted c67 compact result identity mismatch')
        if owner_result is not None and c56.encoded(owner_result)!=c56.encoded(result):
            raise c58.EvidenceError('stale/wrong image/schema/tool/contract/phase/reason/provenance c67 JSON')
        if (result['contract']!=c67.CONTRACT or not result['classification_complete'] or any(result[k] for k in c56.FALSE_GATES)
            or len(records)!=result['post_c65_boundary_instances_total'] or not result['c56_through_c66_evidence_read_only']):
            raise c58.EvidenceError('wrong/incomplete c67 contract/domain')
        self.records,self.index,self.result = tuple(records),index,result
        self.header = dict(tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_sha256=result['image_sha256'],c56_manifest_id=baseline.header['c56_manifest_id'],
            **{k:v for k,v in baseline.header.items() if k.startswith('accepted_')},
            accepted_c67_classifier_identity=result['classifier_identity'],accepted_c67_result_identity=c56.digest(result),
            accepted_c67_record_set_identity=result['authenticated_record_set_sha256'],
            record_pool_sha256=c56.digest([c56.sha(r) for r in records]),index_sha256=c56.digest(sorted(index.items())),
            limits=asdict(c58.Limits()),tool_sources=sources())
        self.identity = c56.digest(self.header)

    def assert_current(self):
        self.baseline.assert_current()
        if (self.header.get('tool_sources')!=sources() or c56.digest(self.header)!=self.identity
            or c56.digest(self.result)!=self.header['accepted_c67_result_identity']
            or self.baseline.identity!=self.result['classifier_identity'] or self.baseline.header!=self.result['classifier_header']
            or c56.digest([c56.sha(r) for r in self.records])!=self.header['record_pool_sha256']
            or c56.digest(sorted(self.index.items()))!=self.header['index_sha256']):
            raise c58.EvidenceError('stale/mutated c56-c68 schema/tool/contract/record pool identity')

    def lookup(self,root):
        ordinal = self.index.get(root)
        return None if ordinal is None else json.loads(self.records[ordinal])


def sibling_prefix(tx,view):
    return c56.digest(dict(original_c66_prefix=c66.sibling_prefix(tx,view),post_c67=sorted(tx.post_c67),
        c67_source_keys=sorted(view.c67_source_keys),c67_record=c56.digest(view.c67_record),c67_key=view.c67_key,
        c67_scan_token=view.c67_scan_token,c67_prefix=view.c67_prefix,c67_cache_identity=view.c67_cache_identity,
        c67_proofs=c56.digest(sorted(view.c67_proofs.items())),c67_siblings_used=sorted(view.c67_siblings_used),
        c67_considered_before=sorted(view.c67_considered_before),c67_considered_allowed=sorted(view.c67_considered_allowed),
        c67_considered=sorted(view.c67_considered),c67_eligible=sorted(view.c67_eligible),
        c67_edges_considered=sorted(view.c67_edges_considered),c67_edges_eligible=sorted(view.c67_edges_eligible),
        c67_refusals=dict(view.c67_refusals),c67_mismatches=dict(view.c67_mismatches),
        post65_capture=c56.digest(view.post65_capture),crossings=tx.crossings,c61_crossings=tx.c61_crossings,
        c63_crossings=tx.c63_crossings,c65_crossings=tx.c65_crossings,c67_crossings=tx.c67_crossings))


class View(c67.View):
    def __init__(self,certificates):
        super().__init__(certificates.baseline.certificates)
        self.c68_certificates = certificates
        self.c67_considered,self.c67_eligible,self.c67_edges_considered,self.c67_edges_eligible = set(),set(),set(),set()
        self.c67_source_keys,self.c67_proofs = set(),{}
        self.c67_record,self.c67_scan_token,self.c67_key,self.c67_prefix = None,None,None,None
        self.c67_refusals,self.c67_mismatches = Counter(),Counter()
        self.c67_considered_before,self.c67_considered_allowed = set(),set()
        self.c67_siblings_used,self.c67_cache_identity,self.c67_capture = set(),None,None

    def edge(self,index,ins,kind,target,architectural,relations):
        tx,key = self.tx,self.key
        if tx is None or key is None or key[0]!=index:raise c58.EvidenceError('missing current c68 source context')
        if key in tx.post_c67 or (key in self.c67_source_keys and
            (ins.address!=self.c67_record['source_instruction_rva'] or tx.scan_token!=self.c67_scan_token)):
            begin,end,_ = self.session.rows[index]
            relations[c57.target_relation(self.session,begin,end,target)['target_metadata_relation']] += 1
            raise c58.Refusal(POST)
        if self.c67_record is not None and key==self.c67_key and tx.scan_token==self.c67_scan_token and ins.address==self.c67_record['source_instruction_rva']:
            if (c56.encoded(c67.capture_for(tx,self,index,ins))!=c56.encoded(self.c67_capture)
                or c56.digest((self.c67_record,sorted(self.c67_proofs.items())))!=self.c67_cache_identity
                or sibling_prefix(tx,self)!=self.c67_prefix
                or not self.c67_considered_before<=tx.considered<=self.c67_considered_allowed):
                raise c58.Refusal('c67_sibling_context_or_provenance_conflict')
            sibling = (kind,target,architectural)
            if sibling in self.c67_siblings_used:raise c58.Refusal('duplicate_c67_sibling_callback')
            self.c67_siblings_used.add(sibling)
            proof = self.c67_proofs.get(sibling)
            if proof is None:raise c58.EvidenceError('unmatched exact c67 sibling successor')
            return proof
        self.post65_capture = None
        try:return super().edge(index,ins,kind,target,architectural,relations)
        except c58.Refusal as error:
            if self.c67_record is not None and str(error) in (c66.POST,c64.POST,c62.POST,c60.POST,c59.LATER):raise c58.Refusal(POST) from error
            if str(error)!=c66.POST:raise
        expected = self.c68_certificates.lookup(tx.root)
        if expected is None:
            self.c67_mismatches['unrecorded_root'] += 1;raise c58.Refusal(c66.POST)
        rid = expected['record_identity'];self.c67_considered.add(rid)
        current = c67.record_for(self.c68_certificates.baseline,tx,self)
        if c56.encoded(current)!=c56.encoded(expected):
            self.c67_mismatches['full_prefix_context_phase_reason_or_provenance_mismatch'] += 1
            raise c58.Refusal(c66.POST)
        begin,end,code,_ = self.row(index)
        if c56.encoded(expected['edges'])!=c56.encoded(c59.classify_edges(self.session,begin,end,ins)):
            raise c58.EvidenceError('stale current c67 successors')
        proofs,reason = {},None
        for edge in expected['edges']:
            if edge['architectural_successor'] and edge['target_metadata_relation']=='exact_unique_runtime_begin':
                self.c67_edges_considered.add(c56.digest((rid,edge)))
        for edge in expected['edges']:
            relation = edge['target_metadata_relation']
            error = ('model_only_successor' if not edge['architectural_successor'] else relation if relation!='exact_unique_runtime_begin'
                else 'overlapping_source_target_spans' if edge['source_target_spans_overlap'] else None)
            if error is not None:self.c67_refusals[relation] += 1;reason = reason or error;continue
            dest = edge['target_runtime_row_index']
            try:low,high,raw,_ = self.row(dest)
            except c58.Refusal as error:
                self.c67_refusals['target_code_preflight_refused'] += 1;reason = reason or str(error);continue
            if (low,high)!=(edge['target_begin_rva'],edge['target_end_rva']) or low!=edge['edge_target_rva']:
                raise c58.EvidenceError('wrong exact c67 target metadata/bounds')
            proof = dict(image_sha256=self.c68_certificates.header['image_sha256'],
                accepted_c57_boundary_identity=self.c68_certificates.header['accepted_c57_boundary_identity'],
                source_runtime_row=index,source_row_sha256=expected['source_runtime_row_sha256'],source_code_sha256=c56.sha(code),
                source_instruction_rva=ins.address,source_instruction_size=ins.size,source_instruction_sha256=c56.sha(bytes(ins.bytes)),
                edge_kind=edge['edge_kind'],architectural_successor=True,target_metadata_relation=relation,
                target_runtime_row=dest,target_begin_rva=low,target_end_rva=high,target_row_sha256=self.session.row_identity(dest),target_code_sha256=c56.sha(raw),
                proof_kind='c67_post_boundary',c67_record_identity=rid,c67_classifier_identity=self.c68_certificates.result['classifier_identity'],
                c67_result_identity=self.c68_certificates.header['accepted_c67_result_identity'],c67_edge_identity=c56.digest((rid,edge)),
                c67_full_prefix_identity=expected['root_replay_identity'],current_c66_continuation_identity=expected['current_c66_continuation_identity'],
                source_entry_instance_identity=expected['entry_instance_identity'],source_incoming_state_identity=expected['incoming_state_identity'],
                source_boundary_path_identity=expected['boundary_path_identity'],terminal_phase=expected['terminal_phase'],terminal_reason=expected['terminal_reason'],
                terminal_provenance=deepcopy(expected['terminal_provenance']),post_c59_origin=deepcopy(expected['post_c59_origin']),
                post_c61_origin=deepcopy(expected['post_c61_origin']),post_c63_origin=deepcopy(expected['post_c63_origin']),
                post_c65_origin=deepcopy(expected['post_c65_origin']),c65_authorization=deepcopy(expected['c65_authorization']),
                c65_terminal_phase=expected['c65_terminal_phase'],c65_terminal_reason=expected['c65_terminal_reason'],
                local_c65_edge_identities=expected['local_c65_edge_identities'],
                accepted_c66_authentication_identity=expected['accepted_c66_authentication_identity'],accepted_c66_result_identity=expected['accepted_c66_result_identity'])
            proof['edge_identity'] = c56.digest(proof);proofs[(edge['edge_kind'],edge['edge_target_rva'],edge['architectural_successor'])] = proof
        if reason is not None:raise c58.Refusal(reason)
        if not proofs or len(proofs)!=len(expected['edges']):raise c58.EvidenceError('incomplete c67 all-successor preflight')
        self.c67_record,self.c67_key,self.c67_scan_token = expected,key,tx.scan_token
        self.c67_proofs = proofs;self.c67_source_keys.add(key);self.c67_capture = deepcopy(self.post65_capture)
        self.c67_cache_identity = c56.digest((expected,sorted(proofs.items())))
        self.c67_siblings_used.add((kind,target,architectural));self.c67_eligible.add(rid)
        self.c67_edges_eligible.update(p['c67_edge_identity'] for p in proofs.values())
        self.c67_prefix = sibling_prefix(tx,self);self.c67_considered_before = set(tx.considered)
        self.c67_considered_allowed = tx.considered | {(index,ins.address,e['edge_kind'],e['edge_target_rva']) for e in expected['edges']}
        proof = proofs.get((kind,target,architectural))
        if proof is None:raise c58.EvidenceError('unmatched first c67 successor')
        return proof


class Transaction(c67.Transaction):
    def __init__(self,view,root,engine,limits):
        super().__init__(view,root,engine,limits)
        self.post_c67,self.c67_crossings = set(),[]

    def taint(self,key):
        if self.e.c67_source_keys and self.closure(self.forward,key)&self.e.c67_source_keys:
            raise c58.Refusal('c67_predecessor_path_conflict')
        return super().taint(key)

    def mark_post(self,key):
        try:return super().mark_post(key)
        except c58.Refusal as error:
            if self.e.c67_source_keys and str(error)==c66.POST:raise c58.Refusal(POST) from error
            raise

    def mark_post_c61(self,key):
        try:return super().mark_post_c61(key)
        except c58.Refusal as error:
            if self.e.c67_source_keys and str(error)==c66.POST:raise c58.Refusal(POST) from error
            raise

    def mark_post_c63(self,key):
        try:return super().mark_post_c63(key)
        except c58.Refusal as error:
            if self.e.c67_source_keys and str(error)==c66.POST:raise c58.Refusal(POST) from error
            raise

    def mark_post_c65(self,key):
        try:return super().mark_post_c65(key)
        except c58.Refusal as error:
            if self.e.c67_source_keys and str(error)==c66.POST:raise c58.Refusal(POST) from error
            raise

    def mark_post_c67(self,key):
        descendants = self.closure(self.forward,key);self.post_c67.update(descendants)
        guarded = self.e.used_boundary_contexts|self.e.c61_source_keys|self.e.c63_source_keys|self.e.c65_source_keys|self.e.c67_source_keys
        if descendants&guarded:raise c58.Refusal(POST)

    def instance(self,row,state,path):
        later = any(p.get('proof_kind')=='c67_post_boundary' for p in path)
        try:key = super().instance(row,state,path)
        except c58.Refusal as error:
            lower = (c66.POST,c64.POST,c62.POST,c60.POST,'c59_predecessor_path_conflict','c61_predecessor_path_conflict',
                'c63_predecessor_path_conflict','c65_predecessor_path_conflict')
            if later and str(error) in lower:raise c58.Refusal(POST) from error
            raise
        if later:self.mark_post_c67(key)
        return key

    def connect(self,source,target):
        try:super().connect(source,target)
        except c58.Refusal as error:
            if source in self.post_c67 and str(error) in (c66.POST,c64.POST,c62.POST,c60.POST):raise c58.Refusal(POST) from error
            raise
        if source in self.post_c67:self.mark_post_c67(target)

    def scan_row(self,key):
        outgoing = super().scan_row(key)
        self.c67_crossings.extend((proof,sum(f is not None for f in state)) for proof,_,state in outgoing if proof.get('proof_kind')=='c67_post_boundary')
        return outgoing


def scan(certificates,limits=c58.Limits(),progress=None):
    if any(type(n)!=int or n<=0 for n in asdict(limits).values()):raise ValueError('all c68 caps must be positive integers')
    certificates.assert_current();e = c67.evidence(certificates.baseline.certificates);engine = shard = None
    audit = c59.ReplayAudit();considered,admitted,edges_considered,edges_admitted = set(),set(),set(),set()
    preflight,mismatches,extra,refs,by_id = Counter(),Counter(),Counter(),[],{}
    for number,root in enumerate(e.proofs,1):
        current = root // e.session.profile.rows_per_shard
        if current!=shard:engine = c58.c55.Engine(e.session.domain,e.session.profile.summary);shard = current
        view = View(certificates);tx = Transaction(view,root,engine,limits);reason = None
        try:tx.run()
        except c58.EvidenceError:raise
        except (c58.Refusal,ValueError) as error:reason = str(error)
        audit.add(tx,reason);considered.update(view.c67_considered);edges_considered.update(view.c67_edges_considered)
        preflight.update(view.c67_refusals);mismatches.update(view.c67_mismatches)
        record = certificates.lookup(root)
        if record is not None:by_id[record['record_identity']] = record
        if reason is None:
            admitted.update(view.c67_eligible);edges_admitted.update(view.c67_edges_eligible)
            extra['crossings'] += len(tx.c67_crossings);extra['facts'] += sum(n for _,n in tx.c67_crossings)
            for original in tx.references:
                ref = deepcopy(original);path = ref['boundary_path']
                if view.c67_eligible:
                    ref.update(c68_authentication_identity=certificates.identity,
                        c68_entry_instance_identity=c56.digest((certificates.identity,root,ref['continuation_runtime_row'],ref['incoming_state_identity'],c56.digest(path))),
                        path_c67_record_identities=[p['c67_record_identity'] for p in path if p.get('proof_kind')=='c67_post_boundary'],
                        root_c67_record_identities=sorted(view.c67_eligible))
                if len(refs)<limits.retained_references:refs.append(ref)
        if progress and (number%1024==0 or number==len(e.proofs)):progress('c68 exact c67 provenance continuation',number,len(e.proofs))
    certificates.assert_current();t,ops = audit.totals,audit.ops;count = sum(ops.values())
    grouped = lambda ids,field:dict(sorted(Counter(by_id[i][field] for i in ids).items()))
    result = dict(repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
        evidence='OFFLINE_STATIC_ENTRY_INSTANCE_CANDIDATES_ONLY',image_authenticated=True,image_sha256=certificates.header['image_sha256'],
        authentication_header=certificates.header,limits=asdict(limits),**{k:v for k,v in certificates.header.items() if k.startswith('accepted_')},
        c67_boundary_instances_available=certificates.result['post_c65_boundary_instances_total'],
        c67_architectural_edges_available=certificates.result['architectural_post_c65_boundary_edges_total'],
        c67_exact_edges_available=certificates.result['target_metadata_relation_counts']['exact_unique_runtime_begin'],
        c67_boundary_instances_considered=len(considered),c67_boundary_instances_admitted=len(admitted),c67_boundary_instances_refused=len(considered-admitted),
        c67_exact_edges_considered=len(edges_considered),c67_exact_edges_admitted=len(edges_admitted),c67_exact_edges_refused=len(edges_considered-edges_admitted),
        c67_terminal_phase_considered=grouped(considered,'terminal_phase'),c67_terminal_phase_admitted=grouped(admitted,'terminal_phase'),
        c67_terminal_phase_refused=grouped(considered-admitted,'terminal_phase'),c67_terminal_reason_considered=grouped(considered,'terminal_reason'),
        c67_terminal_reason_admitted=grouped(admitted,'terminal_reason'),c67_terminal_reason_refused=grouped(considered-admitted,'terminal_reason'),
        c67_all_successor_preflight_refusal_relation_counts=dict(sorted(preflight.items())),c67_certificate_mismatch_counts=dict(sorted(mismatches.items())),
        c67_certified_crossings_with_facts=extra['crossings'],c67_address_facts_crossing_rows=extra['facts'],
        admission_count_scope='unique_accepted_c67_root_record_checked_at_POST_admitted_only_all_successor_completed_root_including_factless',
        exact_edge_count_scope='exact_successors_considered_only_after_full_record_match_admitted_only_completed_root',
        phase_reason_count_scope='accepted_record_phase_and_reason_not_relabel_of_mismatching_current_attempt',
        root_transactions_attempted=t['root_transactions_attempted'],root_transactions_completed=t['roots_completed'],roots_completed=t['roots_completed'],
        root_transactions_refused=sum(audit.reasons.values()),root_refusal_reasons=dict(sorted(audit.reasons.items())),
        post_c67_uncertified_boundary_refusals=audit.reasons[POST],rows_reached_by_continuation=len(audit.rows),
        remaining_boundary_refusal_relation_counts={k:audit.relations[k] for k in c57.RELATIONS},
        remaining_relation_count_scope='aborted_root_edge_callbacks_separate_from_full_successor_preflight',
        reference_candidate_count=count,new_cross_row_candidate_count=count,reader_candidates=ops['READ'],writer_candidates=ops['WRITE'],rmw_candidates=ops['RMW'],
        address_only_candidates=ops['ADDRESS_ONLY'],reference_candidates=refs,candidate_records_not_retained=count-len(refs),
        candidate_count_scope='completed_c68_root_instance_operand_observations_not_unique_sites_or_difference_from_c66',
        historical_c66_candidate_counts={k:certificates.baseline.result[k] for k in ('reference_candidate_count','new_cross_row_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates')},
        historical_c66_candidate_output_sha256=c56.digest(certificates.baseline.result['reference_candidates']),
        external_anchors=e.result['external_anchors'],anchor_counted_as_local_candidate=False,c56_through_c67_evidence_read_only=True,
        publication_source_kind='unproven',live_qword_reads=0,c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
        summary_scope='fresh_c55_engine_per_original_root_shard_c68_work_not_interchangeable_with_c66',**{k:False for k in c56.FALSE_GATES})
    for k in ('continuation_instances_created','continuation_instances_deduplicated','continuation_instances_refused','cross_row_cap_refusals',
        'cross_row_cycles_seen','address_facts_crossing_runtime_rows','call_edges_with_preserved_facts_crossing_rows','no_carried_fact_edges'):
        result[k] = t[k]
    return result


def analyze(data,cache=None,c57_json=None,c58_json=None,c59_json=None,c60_json=None,c61_json=None,c62_json=None,
        c63_json=None,c64_json=None,c65_json=None,c66_json=None,c67_json=None,progress=None):
    if len(data)>cfg.old.MAX_FILE or c56.sha(data)!=EXPECTED_SHA:raise c58.EvidenceError('exact executable SHA256 mismatch or input cap')
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
    owner67 = read_c67(c67_json) if c67_json is not None else None
    return scan(ProofPool(base67,owner_result=owner67,progress=progress),progress=progress)


def main():
    parser = argparse.ArgumentParser(description=__doc__);parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    for flag in ('c56-cache','c57-json','c58-json','c59-json','c60-json','c61-json','c62-json','c63-json','c64-json','c65-json','c66-json','c67-json'):
        parser.add_argument('--'+flag,type=Path)
    a = parser.parse_args()
    try:
        output = a.output.resolve();inputs = [p for p in (a.exe,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,a.c62_json,a.c63_json,a.c64_json,a.c65_json,a.c66_json,a.c67_json) if p is not None]
        if any(output==p.resolve() for p in inputs) or (a.c56_cache and output.is_relative_to(a.c56_cache.resolve())):raise c58.EvidenceError('output overlaps immutable input evidence')
        if a.exe.stat().st_size>cfg.old.MAX_FILE:raise c58.EvidenceError('input file cap')
        result = analyze(a.exe.read_bytes(),a.c56_cache,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,a.c62_json,
            a.c63_json,a.c64_json,a.c65_json,a.c66_json,a.c67_json,lambda phase,n,total:print(f'{phase}: {n}/{total}',file=sys.stderr))
        c56.atomic_json(a.output,result)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(f'{BLOCKED}: {error}',file=sys.stderr);return 2
    print(f'{READY}: {a.output}');return 0


if __name__=='__main__':raise SystemExit(main())
