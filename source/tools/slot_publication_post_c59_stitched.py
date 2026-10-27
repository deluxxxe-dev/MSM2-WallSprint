"""c62 OFFLINE continuation from fully replayed, exact-phase c61 POST records.

First use matches the complete original c61 terminal record, including original
prefix graph/work, not just a row/site. Sibling successors share one all-edge
preflight. New post-c61 paths cannot inherit earlier certificates through dedup.
"""
import argparse
from collections import Counter
from copy import deepcopy
from dataclasses import asdict
import json
from pathlib import Path
import sys

import slot_publication_post_c59_boundary as c61

c60,c59,c58,c57,c56,cfg = c61.c60,c61.c59,c61.c58,c61.c57,c61.c56,c61.cfg
VERSION = 'v0.1c62'
SCHEMA = 1
CONTRACT = 'exact_c61_terminal_phase_origin_full_prefix_all_successor_continuation_v1'
READY = 'POST_C59_RUNTIME_BEGIN_STITCH_SCANNER_READY'
BLOCKED = 'POST_C59_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED'
POST = 'uncertified_post_c61_runtime_boundary'
EXPECTED_SHA = c61.EXPECTED_SHA
PHASES = ('same_instance_after_c59_preflight_before_row_fixpoint','post_c59_reachable_context',
    'post_c59_reachable_dedup_stored_earlier_path')
ACCEPTED_C61 = dict(repository_result=c61.READY,tool_version=c61.VERSION,schema_version=c61.SCHEMA,
    classifier_identity='a828ec479e0f3279ddaf117d76ff827652155c357a8415606cbe2924e479d349',
    authenticated_record_set_sha256='f5bc25085c62e7f9a32eab74ade7af97e076c9b03c99f796cc06d179ad5f724e',
    accepted_c60_authentication_identity='5a71f37023218abc6827dfd22274dad067837021e67ccb8f25d9070202360e0b',
    accepted_c60_result_identity='761ffda9b074dc05a8f4df389ded255f654a219ace0ea0364720b81bab50b21a',
    post_c59_uncertified_boundary_refused_roots_expected=6566,post_c59_uncertified_boundary_refused_roots_classified=6566,
    post_c59_boundary_instances_total=6566,unique_entry_instance_states=6566,unique_source_rows=6467,
    post_c59_boundary_edges_total=7206,architectural_post_c59_boundary_edges_total=7206,model_only_post_c59_boundary_edges_total=0,
    target_metadata_relation_counts={k:dict(exact_unique_runtime_begin=6946,inside_unique_runtime_span_nonentry=245,
        executable_file_backed_outside_runtime_metadata=15).get(k,0) for k in c57.RELATIONS},
    edge_kind_counts={k:dict(ordinary_fallthrough=2487,conditional_fallthrough=681,direct_conditional_taken=3492,
        direct_unconditional_branch=529,call_fallthrough=17).get(k,0) for k in c57.KINDS},
    exact_runtime_begin_edge_kind_counts={k:dict(ordinary_fallthrough=2487,conditional_fallthrough=681,direct_conditional_taken=3304,
        direct_unconditional_branch=457,call_fallthrough=17).get(k,0) for k in c57.KINDS},
    terminal_phase_counts=dict(zip(PHASES,(5759,805,2))),classification_complete=True,new_post_c59_boundaries_admitted=0,
    new_cross_row_candidate_count=0,cross_runtime_fact_propagation_unchanged=True,c60_candidate_output_unchanged=True)


def sources():
    return dict(c61.sources(),slot_publication_post_c59_stitched_py=c56.sha(Path(__file__).read_bytes()))


def semantic(record):
    # After first authorization only sibling callbacks in the SAME row scan may
    # reuse it. Original relation/lookup counters advance during that instruction.
    return {k:v for k,v in record.items() if k not in ('root_replay_identity','record_identity')}


class ProofPool:
    def __init__(self,baseline,*,owner_result=None,accepted=None,progress=None):
        self.baseline = baseline
        aggregate = c61.Aggregate(baseline)
        records,index = [],{}
        for record in c61.replay_records(baseline,progress):
            aggregate.add(record)  # independent ordered original c61 replay
            root = record['original_root_runtime_row']
            if root in index:raise c58.EvidenceError('multiple c61 terminal records for one accepted root')
            index[root] = len(records);records.append(c56.encoded(record))
        result = aggregate.finish()
        expected = ACCEPTED_C61 if accepted is None else accepted  # owned test seam, no CLI override
        if any(c56.encoded(result.get(k))!=c56.encoded(v) for k,v in expected.items()):
            raise c58.EvidenceError('accepted c61 identity/count/phase/record-set mismatch')
        if owner_result is not None and c56.encoded(owner_result)!=c56.encoded(result):
            raise c58.EvidenceError('stale/wrong image/schema/tool/contract/phase/record-set c61 JSON')
        if (result['contract']!=c61.CONTRACT or not result['classification_complete']
            or any(result[k] for k in c56.FALSE_GATES) or len(records)!=result['post_c59_boundary_instances_total']):
            raise c58.EvidenceError('wrong/incomplete c61 contract or domain')
        self.records,self.index,self.result = tuple(records),index,result
        self.header = dict(tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_sha256=result['image_sha256'],c56_manifest_id=baseline.header['c56_manifest_id'],
            **{k:result[k] for k in ('accepted_c57_boundary_identity','accepted_c58_proof_identity','accepted_c58_result_identity',
                'accepted_c59_classifier_identity','accepted_c59_record_set_identity','accepted_c59_result_identity',
                'accepted_c60_authentication_identity','accepted_c60_result_identity')},
            accepted_c61_classifier_identity=result['classifier_identity'],accepted_c61_result_identity=c56.digest(result),
            accepted_c61_record_set_identity=result['authenticated_record_set_sha256'],
            record_pool_sha256=c56.digest([c56.sha(r) for r in records]),index_sha256=c56.digest(sorted(index.items())),
            limits=asdict(c58.Limits()),tool_sources=sources())
        self.identity = c56.digest(self.header)

    def assert_current(self):
        self.baseline.assert_current()
        if (self.header.get('tool_sources')!=sources() or c56.digest(self.header)!=self.identity
            or c56.digest(self.result)!=self.header['accepted_c61_result_identity']
            or c56.digest([c56.sha(r) for r in self.records])!=self.header['record_pool_sha256']
            or c56.digest(sorted(self.index.items()))!=self.header['index_sha256']):
            raise c58.EvidenceError('stale/mutated c56-c62 schema/tool/contract/record pool identity')

    def lookup(self,root):
        ordinal = self.index.get(root)
        return None if ordinal is None else json.loads(self.records[ordinal])


class View(c61.View):
    def __init__(self,certificates):
        super().__init__(certificates.baseline.pool)  # original c60 proof namespace
        self.certificates = certificates
        self.c61_considered,self.c61_eligible,self.c61_edges_considered,self.c61_edges_eligible = set(),set(),set(),set()
        self.c61_source_keys,self.proofs_at_c61 = set(),{}
        self.c61_record,self.c61_scan_token = None,None
        self.c61_refusals,self.c61_mismatches = Counter(),Counter()

    def edge(self,index,ins,kind,target,architectural,relations):
        tx,key = self.tx,self.key
        if tx is None or key is None or key[0]!=index:raise c58.EvidenceError('missing current c62 source context')
        if key in tx.post_c61 or (key in self.c61_source_keys and self.c61_record is not None
            and (ins.address!=self.c61_record['source_instruction_rva'] or tx.scan_token!=self.c61_scan_token)):
            begin,end,_ = self.session.rows[index]
            relations[c57.target_relation(self.session,begin,end,target)['target_metadata_relation']] += 1
            raise c58.Refusal(POST)
        self.capture = None
        try:return super().edge(index,ins,kind,target,architectural,relations)
        except c58.Refusal as error:
            if str(error)!=c60.POST:raise
        expected = self.certificates.lookup(tx.root)
        if expected is None:
            self.c61_mismatches['unrecorded_root'] += 1
            raise c58.Refusal(POST if self.c61_record is not None else c60.POST)
        rid = expected['record_identity'];self.c61_considered.add(rid)
        current = c61.record_for(self.certificates.baseline,tx,self)
        if self.c61_record is None:
            # BEFORE the first new admission, prefix work/graph/phases/origins
            # must still be identical to original c60/c61. No extra c62 metered
            # work is performed before this comparison.
            match = c56.encoded(current)==c56.encoded(expected)
        else:
            match = (rid==self.c61_record['record_identity'] and tx.scan_token==self.c61_scan_token
                and c56.encoded(semantic(current))==c56.encoded(semantic(self.c61_record)))
        if not match:
            self.c61_mismatches['context_phase_origin_or_full_prefix_mismatch'] += 1
            raise c58.Refusal(POST if self.c61_record is not None else c60.POST)
        if self.c61_record is None:
            begin,end,code,_ = self.row(index)
            if c56.encoded(expected['edges'])!=c56.encoded(c59.classify_edges(self.session,begin,end,ins)):
                raise c58.EvidenceError('stale current c61 source successors')
            proofs,reason = {},None
            for edge in expected['edges']:
                if edge['architectural_successor'] and edge['target_metadata_relation']=='exact_unique_runtime_begin':
                    self.c61_edges_considered.add(c56.digest((rid,edge)))
            # All required successors before ANY edge is returned.
            for edge in expected['edges']:
                relation = edge['target_metadata_relation']
                error = ('model_only_successor' if not edge['architectural_successor'] else
                    relation if relation!='exact_unique_runtime_begin' else
                    'overlapping_source_target_spans' if edge['source_target_spans_overlap'] else None)
                if error is not None:
                    self.c61_refusals[relation] += 1;reason = reason or error;continue
                dest = edge['target_runtime_row_index']
                try:low,high,raw,_ = self.row(dest)
                except c58.Refusal as error:
                    self.c61_refusals['target_code_preflight_refused'] += 1;reason = reason or str(error);continue
                if (low,high)!=(edge['target_begin_rva'],edge['target_end_rva']) or low!=edge['edge_target_rva']:
                    raise c58.EvidenceError('wrong exact c61 target metadata/bounds')
                proof = dict(image_sha256=self.certificates.header['image_sha256'],
                    accepted_c57_boundary_identity=self.certificates.header['accepted_c57_boundary_identity'],
                    source_runtime_row=index,source_row_sha256=expected['source_runtime_row_sha256'],source_code_sha256=c56.sha(code),
                    source_instruction_rva=ins.address,source_instruction_size=ins.size,source_instruction_sha256=c56.sha(bytes(ins.bytes)),
                    edge_kind=edge['edge_kind'],architectural_successor=True,target_metadata_relation=relation,
                    target_runtime_row=dest,target_begin_rva=low,target_end_rva=high,
                    target_row_sha256=self.session.row_identity(dest),target_code_sha256=c56.sha(raw),
                    proof_kind='c61_post_boundary',c61_record_identity=rid,
                    c61_classifier_identity=self.certificates.result['classifier_identity'],
                    c61_result_identity=self.certificates.header['accepted_c61_result_identity'],
                    c61_edge_identity=c56.digest((rid,edge)),c61_full_prefix_identity=expected['root_replay_identity'],
                    source_entry_instance_identity=expected['entry_instance_identity'],
                    source_incoming_state_identity=expected['incoming_state_identity'],
                    source_boundary_path_identity=expected['boundary_path_identity'],
                    terminal_phase=expected['terminal_phase'],post_c59_origin=deepcopy(expected['post_c59_origin']),
                    local_c59_record_identities=expected['local_c59_record_identities'],
                    accepted_c60_authentication_identity=expected['accepted_c60_authentication_identity'],
                    accepted_c60_result_identity=expected['accepted_c60_result_identity'])
                proof['edge_identity'] = c56.digest(proof)
                proofs[(edge['edge_kind'],edge['edge_target_rva'],edge['architectural_successor'])] = proof
            if reason is not None:raise c58.Refusal(reason)
            if not proofs or len(proofs)!=len(expected['edges']):raise c58.EvidenceError('incomplete c61 successor preflight')
            self.proofs_at_c61 = proofs;self.c61_record = expected;self.c61_scan_token = tx.scan_token
            self.c61_eligible.add(rid);self.c61_edges_eligible.update(p['c61_edge_identity'] for p in proofs.values())
        proof = self.proofs_at_c61.get((kind,target,architectural))
        if proof is None:raise c58.EvidenceError('unmatched c61 sibling successor')
        self.c61_source_keys.add(key)
        return proof


class Transaction(c61.Transaction):
    def __init__(self,view,root,engine,limits):
        super().__init__(view,root,engine,limits)
        self.post_c61,self.c61_crossings = set(),[]
        self.scan_token = 0

    def taint(self,key):
        if self.e.c61_source_keys:
            descendants = self.closure(self.forward,key)
            if descendants & self.e.c61_source_keys:raise c58.Refusal('c61_predecessor_path_conflict')
        return super().taint(key)

    def mark_post(self,key):
        before = set(self.post_c59)
        try:super().mark_post(key)
        except c58.Refusal as error:
            if self.e.c61_source_keys and str(error)==c60.POST:raise c58.Refusal(POST) from error
            raise
        if (self.post_c59-before) & self.e.c61_source_keys:raise c58.Refusal('c61_origin_conflict')

    def mark_post_c61(self,key):
        descendants = self.closure(self.forward,key)
        self.post_c61.update(descendants)
        if descendants & (self.e.used_boundary_contexts|self.e.c61_source_keys):raise c58.Refusal(POST)

    def instance(self,row,state,path):
        later = any(p.get('proof_kind')=='c61_post_boundary' for p in path)
        try:key = super().instance(row,state,path)
        except c58.Refusal as error:
            if later and str(error) in (c60.POST,'c59_predecessor_path_conflict'):
                raise c58.Refusal(POST) from error
            raise
        if later:self.mark_post_c61(key)
        return key

    def connect(self,source,target):
        try:super().connect(source,target)
        except c58.Refusal as error:
            if source in self.post_c61 and str(error)==c60.POST:raise c58.Refusal(POST) from error
            raise
        if source in self.post_c61:self.mark_post_c61(target)

    def scan_row(self,key):
        self.scan_token += 1  # diagnostic cache lifetime, not original work
        outgoing = super().scan_row(key)
        self.c61_crossings.extend((proof,sum(f is not None for f in state)) for proof,_,state in outgoing
            if proof.get('proof_kind')=='c61_post_boundary')
        return outgoing


def scan(certificates,limits=c58.Limits(),progress=None):
    if any(type(n)!=int or n<=0 for n in asdict(limits).values()):raise ValueError('all c62 caps must be positive integers')
    certificates.assert_current()
    e = certificates.baseline.pool.baseline.evidence
    audit = c59.ReplayAudit();considered,admitted,edges_considered,edges_admitted = set(),set(),set(),set()
    preflight,mismatches,extra,refs,phase_by_id = Counter(),Counter(),Counter(),[],{}
    engine = shard = None
    for number,root in enumerate(e.proofs,1):
        current = root // e.session.profile.rows_per_shard
        if current!=shard:
            engine = c58.c55.Engine(e.session.domain,e.session.profile.summary);shard = current
        view = View(certificates);tx = Transaction(view,root,engine,limits);reason = None
        try:tx.run()
        except c58.EvidenceError:raise
        except (c58.Refusal,ValueError) as error:reason = str(error)
        audit.add(tx,reason);considered.update(view.c61_considered);edges_considered.update(view.c61_edges_considered)
        preflight.update(view.c61_refusals);mismatches.update(view.c61_mismatches)
        record = certificates.lookup(root)
        if record is not None:phase_by_id[record['record_identity']] = record['terminal_phase']
        if reason is None:
            admitted.update(view.c61_eligible);edges_admitted.update(view.c61_edges_eligible)
            extra['crossings'] += len(tx.c61_crossings);extra['facts'] += sum(n for _,n in tx.c61_crossings)
            for ref in tx.references:
                path = ref['boundary_path']
                ref.update(c62_authentication_identity=certificates.identity,boundary_path_identity=c56.digest(path),
                    c62_entry_instance_identity=c56.digest((certificates.identity,root,ref['continuation_runtime_row'],
                        ref['incoming_state_identity'],c56.digest(path))),
                    path_c59_record_identities=[p['c59_record_identity'] for p in path if p.get('proof_kind')=='c59_later_boundary'],
                    path_c61_record_identities=[p['c61_record_identity'] for p in path if p.get('proof_kind')=='c61_post_boundary'],
                    root_c61_record_identities=sorted(view.c61_eligible))
                if len(refs)<limits.retained_references:refs.append(ref)
        if progress and (number%1024==0 or number==len(e.proofs)):progress('c62 exact-phase POST continuation',number,len(e.proofs))
    certificates.assert_current()
    t,ops = audit.totals,audit.ops;count = sum(ops.values())
    phase_counts = lambda ids:{p:sum(phase_by_id[i]==p for i in ids) for p in PHASES}
    result = dict(repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
        evidence='OFFLINE_STATIC_ENTRY_INSTANCE_CANDIDATES_ONLY',image_authenticated=True,image_sha256=certificates.header['image_sha256'],
        authentication_header=certificates.header,limits=asdict(limits),
        **{k:v for k,v in certificates.header.items() if k.startswith('accepted_')},
        c61_post_boundary_instances_available=certificates.result['post_c59_boundary_instances_total'],
        c61_post_architectural_edges_available=certificates.result['architectural_post_c59_boundary_edges_total'],
        c61_post_exact_edges_available=certificates.result['target_metadata_relation_counts']['exact_unique_runtime_begin'],
        c61_boundary_instances_considered=len(considered),c61_boundary_instances_admitted=len(admitted),c61_boundary_instances_refused=len(considered-admitted),
        c61_exact_edges_considered=len(edges_considered),c61_exact_edges_admitted=len(edges_admitted),c61_exact_edges_refused=len(edges_considered-edges_admitted),
        c61_terminal_phase_considered=phase_counts(considered),c61_terminal_phase_admitted=phase_counts(admitted),
        c61_terminal_phase_refused=phase_counts(considered-admitted),
        c61_all_successor_preflight_refusal_relation_counts=dict(sorted(preflight.items())),c61_certificate_mismatch_counts=dict(sorted(mismatches.items())),
        c61_certified_crossings_with_facts=extra['crossings'],c61_address_facts_crossing_rows=extra['facts'],
        admission_count_scope='unique_accepted_root_record_checked_at_POST_admitted_only_full_prefix_all_successor_completed_root_including_factless',
        exact_edge_count_scope='exact_successors_checked_only_after_full_c61_record_match_admitted_only_in_completed_roots',
        phase_count_scope='accepted_record_phase_not_a_relabel_of_current_mismatching_attempt',
        root_transactions_attempted=t['root_transactions_attempted'],root_transactions_completed=t['roots_completed'],roots_completed=t['roots_completed'],
        root_transactions_refused=sum(audit.reasons.values()),root_refusal_reasons=dict(sorted(audit.reasons.items())),
        post_c61_uncertified_boundary_refusals=audit.reasons[POST],
        rows_reached_by_continuation=len(audit.rows),remaining_boundary_refusal_relation_counts={k:audit.relations[k] for k in c57.RELATIONS},
        remaining_relation_count_scope='edge_callbacks_in_aborted_c62_roots_separate_from_full_successor_preflight',
        reference_candidate_count=count,new_cross_row_candidate_count=count,reader_candidates=ops['READ'],writer_candidates=ops['WRITE'],
        rmw_candidates=ops['RMW'],address_only_candidates=ops['ADDRESS_ONLY'],reference_candidates=refs,candidate_records_not_retained=count-len(refs),
        candidate_count_scope='completed_c62_root_entry_instance_operand_observations_not_unique_sites_or_difference_from_c60',
        historical_c60_candidate_counts={k:certificates.baseline.result[k] for k in ('reference_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates')},
        external_anchors=e.result['external_anchors'],anchor_counted_as_local_candidate=False,c56_through_c61_evidence_read_only=True,
        publication_source_kind='unproven',live_qword_reads=0,c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
        summary_scope='fresh_c55_engine_per_original_root_shard_c62_work_not_interchangeable_with_c60_c61',
        **{k:False for k in c56.FALSE_GATES})
    for k in ('continuation_instances_created','continuation_instances_deduplicated','continuation_instances_refused','cross_row_cap_refusals',
        'cross_row_cycles_seen','address_facts_crossing_runtime_rows','call_edges_with_preserved_facts_crossing_rows','no_carried_fact_edges'):
        result[k] = t[k]
    return result


def analyze(data,cache=None,c57_json=None,c58_json=None,c59_json=None,c60_json=None,c61_json=None,progress=None):
    if len(data)>cfg.old.MAX_FILE or c56.sha(data)!=EXPECTED_SHA:raise c58.EvidenceError('exact executable SHA256 mismatch or input cap')
    s = c56.Session(data);old = c57.collect_baseline(s,cache,progress=progress)
    e = c58.Evidence(old,owner_result=c56.read_json(c57_json) if c57_json is not None else None,progress=progress)
    b = c59.Baseline(e,owner_result=c56.read_json(c58_json) if c58_json is not None else None,progress=progress)
    p = c60.ProofPool(b,owner_result=c56.read_json(c59_json) if c59_json is not None else None,progress=progress)
    original = c61.Baseline(p,owner_result=c56.read_json(c60_json) if c60_json is not None else None,progress=progress)
    certificates = ProofPool(original,owner_result=c56.read_json(c61_json) if c61_json is not None else None,progress=progress)
    return scan(certificates,progress=progress)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    for flag in ('c56-cache','c57-json','c58-json','c59-json','c60-json','c61-json'):parser.add_argument('--'+flag,type=Path)
    a = parser.parse_args()
    try:
        out = a.output.resolve();inputs = [p for p in (a.exe,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json) if p is not None]
        if any(out==p.resolve() for p in inputs) or (a.c56_cache and out.is_relative_to(a.c56_cache.resolve())):
            raise c58.EvidenceError('output overlaps immutable input evidence')
        if a.exe.stat().st_size>cfg.old.MAX_FILE:raise c58.EvidenceError('input file cap')
        result = analyze(a.exe.read_bytes(),a.c56_cache,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,
            lambda phase,n,total:print(f'{phase}: {n}/{total}',file=sys.stderr))
        c56.atomic_json(a.output,result)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(f'{BLOCKED}: {error}',file=sys.stderr);return 2
    print(f'{READY}: {a.output}');return 0


if __name__=='__main__':raise SystemExit(main())
