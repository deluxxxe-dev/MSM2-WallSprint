"""c60 OFFLINE entry-instance continuation using fully replayed c59 records.

Only an exact current root/row/seed/path/instruction match can use a later-edge
record. All successors must authenticate. Old modules/evidence remain read-only;
unknown post-c59 boundaries and incomplete roots never emit candidates.
"""
import argparse
from collections import Counter
from dataclasses import asdict
import json
from pathlib import Path
import sys

import slot_publication_later_boundary as c59

c58, c57, c56, cfg = c59.c58, c59.c57, c59.c56, c59.cfg
VERSION = 'v0.1c60'
SCHEMA = 1
CONTRACT = 'exact_c59_instance_path_all_successor_later_runtime_begin_continuation_v1'
READY = 'LATER_RUNTIME_BEGIN_STITCH_SCANNER_READY'
BLOCKED = 'LATER_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED'
POST = 'uncertified_post_c59_runtime_boundary'
EXPECTED_SHA = c59.EXPECTED_SHA
ACCEPTED_C59 = dict(repository_result=c59.READY,tool_version=c59.VERSION,schema_version=c59.SCHEMA,
    accepted_c57_boundary_identity='b990e0d13ed735418fda2669a8e54fec4f7e0b5647d846a4fbaf99eb80639158',
    accepted_c58_proof_identity='9fbdd2552a4e42fe592ebc41ab2f5b9feb65d67349aee43742364140fda10d8f',
    accepted_c58_result_identity='718013be12205027c1e0ae777e70bd2a49602ad002bc6f26242f35fece148827',
    classifier_identity='d4c9d6ad7e9403444ba97d5d181be56fd687b6c58cf401bed88940a2af7bb2e6',
    later_boundary_refused_roots_expected=33719,later_boundary_refused_roots_classified=33719,
    later_boundary_instances_total=33719,unique_entry_instance_states=33719,unique_source_rows=33582,
    later_boundary_edges_total=35921,architectural_later_boundary_edges_total=35921,
    model_only_later_boundary_edges_total=0,later_boundary_instances_with_empty_entry_seed=33582,
    later_boundary_instances_in_original_root=33582,classification_complete=True,
    cross_runtime_fact_propagation_unchanged=True,new_cross_row_candidate_count=0,new_later_boundaries_admitted=0,
    c58_candidate_output_unchanged=True,
    target_metadata_relation_counts={k:dict(exact_unique_runtime_begin=28201,
        inside_unique_runtime_span_nonentry=1566,
        executable_file_backed_outside_runtime_metadata=6154).get(k,0) for k in c57.RELATIONS},
    exact_runtime_begin_edge_kind_counts={k:dict(ordinary_fallthrough=16975,conditional_fallthrough=2309,
        direct_conditional_taken=7425,direct_unconditional_branch=1471,call_fallthrough=21).get(k,0) for k in c57.KINDS})


def sources():
    return dict(c59.sources(),slot_publication_later_stitched_py=c56.sha(Path(__file__).read_bytes()))


def record_key(record):
    return (record['original_root_runtime_row'],record['current_entry_instance_row'],
        record['incoming_state_identity'],record['boundary_path_identity'],record['source_instruction_rva'],
        record['source_instruction_size'],record['source_instruction_sha256'])


def current_key(tx, key, ins):
    state,path = tx.contexts[key]
    return (tx.root,key[0],c58.state_id(state),c56.digest(path),ins.address,ins.size,c56.sha(bytes(ins.bytes)))


class ProofPool:
    """Compact c59 output is NOT a record pool: regenerate and independently validate."""
    def __init__(self, baseline, *, owner_result=None, accepted=None, progress=None):
        self.baseline = baseline
        aggregate = c59.Aggregate(baseline)
        records, index = [], {}
        for number,record in enumerate(c59.replay_records(baseline,progress),1):
            aggregate.add(record)  # independent complete ordered c59 replay
            key = record_key(record)
            if key in index:raise c58.EvidenceError('duplicate c59 context/record proof')
            index[key] = len(records)
            records.append(c56.encoded(record))
        result = aggregate.finish()
        expected = ACCEPTED_C59 if accepted is None else accepted  # owned Python seam, no CLI override
        if any(c56.encoded(result.get(k))!=c56.encoded(v) for k,v in expected.items()):
            raise c58.EvidenceError('accepted c59 result/identity/count/record-set mismatch')
        if sum(result['exact_adjacency_counts'].values()) != (20863 if accepted is None else sum(expected['exact_adjacency_counts'].values())):
            raise c58.EvidenceError('accepted c59 adjacency count mismatch')
        if owner_result is not None and c56.encoded(owner_result)!=c56.encoded(result):
            raise c58.EvidenceError('stale/wrong image/tool/schema/contract/count/record-set c59 JSON')
        if result['contract']!=c59.CONTRACT or not result['classification_complete'] or any(result[k] for k in c56.FALSE_GATES):
            raise c58.EvidenceError('incomplete/wrong c59 contract or gates')
        self.records, self.index, self.result = tuple(records), index, result
        self.header = dict(tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_sha256=result['image_sha256'],c56_manifest_id=baseline.header['c56_manifest_id'],
            accepted_c57_boundary_identity=result['accepted_c57_boundary_identity'],
            accepted_c58_proof_identity=result['accepted_c58_proof_identity'],
            accepted_c58_result_identity=result['accepted_c58_result_identity'],
            accepted_c59_classifier_identity=result['classifier_identity'],
            accepted_c59_result_identity=c56.digest(result),
            accepted_c59_record_set_identity=result['authenticated_record_set_sha256'],
            record_pool_sha256=c56.digest([c56.sha(r) for r in records]),
            index_sha256=c56.digest(sorted(index.items())),limits=asdict(c58.Limits()),tool_sources=sources())
        self.identity = c56.digest(self.header)

    def assert_current(self):
        self.baseline.assert_current()
        if (self.header.get('tool_sources')!=sources() or c56.digest(self.header)!=self.identity
            or c56.digest(self.result)!=self.header['accepted_c59_result_identity']
            or c56.digest([c56.sha(r) for r in self.records])!=self.header['record_pool_sha256']
            or c56.digest(sorted(self.index.items()))!=self.header['index_sha256']):
            raise c58.EvidenceError('stale/mutated c56/c57/c58/c59/c60 tool/schema/proof/record pool identity')

    def lookup(self, key):
        ordinal = self.index.get(key)
        return None if ordinal is None else json.loads(self.records[ordinal])


class View:
    def __init__(self, pool):
        self.pool, self.original = pool, pool.baseline.evidence
        self.session,self.proofs,self.header,self.seal = self.original.session,self.original.proofs,self.original.header,self.original.seal
        self.tx = self.key = None
        self.considered_instances, self.eligible_instances = set(), set()
        self.considered_edges, self.eligible_edges = set(), set()
        self.certified_contexts, self.used_boundary_contexts, self.proofs_at = set(), set(), {}
        self.preflight_refusals, self.lookup_misses = Counter(), 0

    def row(self, index):
        return self.original.row(index)

    def edge(self, index, ins, kind, target, architectural, relations):
        tx,key = self.tx,self.key
        if tx is None or key is None or key[0]!=index:
            raise c58.EvidenceError('missing current c60 entry-instance context')
        if key in tx.post_c59:
            begin,end,_ = self.session.rows[index]
            relations[c57.target_relation(self.session,begin,end,target)['target_metadata_relation']] += 1
            # A new path cannot borrow an old c57 row-global proof, nor the
            # stored first path's c59 record after same-state dedup.
            if not any(p.get('proof_kind')=='c59_later_boundary' for p in tx.contexts[key][1]):
                self.lookup_misses += 1
                raise c58.Refusal(POST)
        else:
            try:
                proof = self.original.edge(index,ins,kind,target,architectural,relations)
                self.used_boundary_contexts.add(key)
                return proof
            except c58.Refusal as error:
                if str(error)!=c59.LATER:raise
        record = self.pool.lookup(current_key(tx,key,ins))
        if record is None:
            self.lookup_misses += 1
            raise c58.Refusal(POST if key in tx.post_c59 or key in self.certified_contexts else c59.LATER)
        rid = record['record_identity']
        self.considered_instances.add(rid)
        edges = record['edges']
        for edge in edges:
            if edge['architectural_successor'] and edge['target_metadata_relation']=='exact_unique_runtime_begin':
                self.considered_edges.add(c56.digest((rid,edge)))
        if key in tx.path_tainted:
            raise c58.Refusal('c59_predecessor_path_conflict')
        if rid not in self.proofs_at:
            begin,end,code,_ = self.row(index)
            state,path = tx.contexts[key]
            if (record['classifier_identity']!=self.pool.result['classifier_identity']
                or record['accepted_c58_proof_identity']!=self.seal
                or record['accepted_c58_result_identity']!=self.pool.baseline.header['accepted_c58_result_identity']
                or record['entry_instance_identity']!=c56.digest((self.seal,tx.root,key))
                or record['source_runtime_row_sha256']!=self.session.row_identity(index)
                or record['original_root_row_sha256']!=self.session.row_identity(tx.root)
                or record['source_code_sha256']!=c56.sha(code)
                or (record['source_begin_rva'],record['source_end_rva'])!=(begin,end)
                or tuple(record['boundary_transition_identities'])!=tuple(p['edge_identity'] for p in path)
                or c56.encoded(edges)!=c56.encoded(c59.classify_edges(self.session,begin,end,ins))):
                raise c58.EvidenceError('stale/wrong exact c59 source/context/path/edge identity')
            proofs = {}
            # Validate ALL external successors before making ANY later edge usable.
            # Return the first exact refusal only after counting all failed classes.
            refusal = None
            for edge in edges:
                relation = edge['target_metadata_relation']
                reason = ('model_only_successor' if not edge['architectural_successor'] else
                    relation if relation!='exact_unique_runtime_begin' else
                    'overlapping_source_target_spans' if edge['source_target_spans_overlap'] else None)
                if reason is not None:
                    self.preflight_refusals[relation] += 1
                    refusal = refusal or reason
                    continue
                dest = edge['target_runtime_row_index']
                try:low,high,raw,_ = self.row(dest)
                except c58.Refusal as error:
                    refusal = refusal or str(error)
                    self.preflight_refusals['target_code_preflight_refused'] += 1
                    continue
                if (low,high)!=(edge['target_begin_rva'],edge['target_end_rva']) or low!=edge['edge_target_rva']:
                    raise c58.EvidenceError('wrong c59 exact target row bounds/order')
                edge_id = c56.digest((rid,edge))
                proof = dict(accepted_c57_boundary_identity=self.pool.header['accepted_c57_boundary_identity'],
                    image_sha256=self.pool.header['image_sha256'],source_runtime_row=index,
                    source_row_sha256=record['source_runtime_row_sha256'],source_code_sha256=record['source_code_sha256'],
                    source_instruction_rva=ins.address,source_instruction_size=ins.size,
                    source_instruction_sha256=c56.sha(bytes(ins.bytes)),edge_kind=edge['edge_kind'],
                    architectural_successor=True,target_metadata_relation=relation,target_runtime_row=dest,
                    target_begin_rva=low,target_end_rva=high,target_row_sha256=self.session.row_identity(dest),
                    target_code_sha256=c56.sha(raw),proof_kind='c59_later_boundary',
                    c59_record_identity=rid,c59_classifier_identity=self.pool.result['classifier_identity'],
                    c59_result_identity=self.pool.header['accepted_c59_result_identity'],c59_edge_identity=edge_id,
                    source_entry_instance_identity=record['entry_instance_identity'],
                    source_incoming_state_identity=record['incoming_state_identity'],
                    source_boundary_path_identity=record['boundary_path_identity'])
                proof['edge_identity'] = c56.digest(proof)
                proofs[(edge['edge_kind'],edge['edge_target_rva'],edge['architectural_successor'])] = proof
            if refusal is not None:raise c58.Refusal(refusal)
            if len(proofs)!=len(edges) or not proofs:raise c58.EvidenceError('missing c59 successor proof')
            self.proofs_at[rid] = proofs
            self.eligible_instances.add(rid)
            self.eligible_edges.update(p['c59_edge_identity'] for p in proofs.values())
        proof = self.proofs_at[rid].get((kind,target,architectural))
        if proof is None:raise c58.EvidenceError('unmatched c59 successor at exact source context')
        self.certified_contexts.add(key)
        self.used_boundary_contexts.add(key)
        return proof


class Transaction(c58.Transaction):
    def __init__(self, view, root, engine, limits):
        super().__init__(view,root,engine,limits)
        self.path_tainted, self.post_c59, self.later_crossings = set(), set(), []

    def taint(self, key):
        # Identical-state dedup may retain a different first path. It must not
        # lend one path's c59 certificate to an unauthenticated arrival or any
        # descendant. Abort if a certificate was already used before this edge.
        descendants = self.closure(self.forward,key)
        self.path_tainted.update(descendants)
        if descendants & self.e.certified_contexts:
            raise c58.Refusal('c59_predecessor_path_conflict')

    def mark_post(self, key):
        descendants = self.closure(self.forward,key)
        self.post_c59.update(descendants)
        if descendants & self.e.used_boundary_contexts:
            raise c58.Refusal(POST)

    def instance(self, row, state, path):
        key = (row,c58.state_id(state))
        if key in self.contexts and c56.digest(self.contexts[key][1])!=c56.digest(path):
            self.taint(key)
        key = super().instance(row,state,path)
        if any(p.get('proof_kind')=='c59_later_boundary' for p in path):self.mark_post(key)
        return key

    def connect(self, source, target):
        super().connect(source,target)
        if source in self.path_tainted:self.taint(target)
        if source in self.post_c59:self.mark_post(target)

    def scan_row(self, key):
        self.e.tx,self.e.key = self,key
        outgoing = super().scan_row(key)
        self.later_crossings.extend((proof,sum(f is not None for f in state)) for proof,_,state in outgoing
            if proof.get('proof_kind')=='c59_later_boundary')
        return outgoing


def scan(pool, limits=c58.Limits(), progress=None):
    if any(type(n)!=int or n<=0 for n in asdict(limits).values()):raise ValueError('all c60 caps must be positive integers')
    pool.assert_current()
    e,audit = pool.baseline.evidence,c59.ReplayAudit()
    considered_instances,admitted_instances,considered_edges,admitted_edges = set(),set(),set(),set()
    preflight_refusals,extra,refs = Counter(),Counter(),[]
    engine = shard = None
    for number,root in enumerate(e.proofs,1):
        current = root // e.session.profile.rows_per_shard
        if current!=shard:
            engine = c58.c55.Engine(e.session.domain,e.session.profile.summary)
            shard = current
        view = View(pool)
        tx = Transaction(view,root,engine,limits)
        reason = None
        try:tx.run()
        except c58.EvidenceError:raise
        except (c58.Refusal,ValueError) as error:reason = str(error)
        audit.add(tx,reason)
        considered_instances.update(view.considered_instances)
        considered_edges.update(view.considered_edges)
        preflight_refusals.update(view.preflight_refusals)
        extra['c59_certificate_lookup_misses'] += view.lookup_misses
        if reason is None:
            admitted_instances.update(view.eligible_instances)
            admitted_edges.update(view.eligible_edges)
            extra['c59_certified_crossings_with_facts'] += len(tx.later_crossings)
            extra['c59_address_facts_crossing_rows'] += sum(n for _,n in tx.later_crossings)
            for ref in tx.references:
                path = ref['boundary_path']
                ref.update(c60_authentication_identity=pool.identity,boundary_path_identity=c56.digest(path),
                    c60_entry_instance_identity=c56.digest((pool.identity,root,ref['continuation_runtime_row'],
                        ref['incoming_state_identity'],c56.digest(path))),
                    c59_later_record_identities=[p['c59_record_identity'] for p in path if p.get('proof_kind')=='c59_later_boundary'])
                if len(refs)<limits.retained_references:refs.append(ref)
        if progress and (number%1024==0 or number==len(e.proofs)):
            progress('c60 exact-instance later continuation',number,len(e.proofs))
    pool.assert_current()
    t,ops = audit.totals,audit.ops
    count = sum(ops.values())
    result = dict(repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
        evidence='OFFLINE_STATIC_ENTRY_INSTANCE_CANDIDATES_ONLY',image_authenticated=True,
        image_sha256=pool.header['image_sha256'],authentication_header=pool.header,limits=asdict(limits),
        accepted_c57_boundary_identity=pool.header['accepted_c57_boundary_identity'],
        accepted_c58_proof_identity=pool.header['accepted_c58_proof_identity'],
        accepted_c58_result_identity=pool.header['accepted_c58_result_identity'],
        accepted_c59_classifier_identity=pool.header['accepted_c59_classifier_identity'],
        accepted_c59_result_identity=pool.header['accepted_c59_result_identity'],
        accepted_c59_record_set_identity=pool.header['accepted_c59_record_set_identity'],
        c59_later_boundary_instances_available=pool.result['later_boundary_instances_total'],
        c59_later_architectural_edges_available=pool.result['architectural_later_boundary_edges_total'],
        c59_later_exact_edges_available=pool.result['target_metadata_relation_counts']['exact_unique_runtime_begin'],
        c59_boundary_instances_considered=len(considered_instances),c59_boundary_instances_admitted=len(admitted_instances),
        c59_boundary_instances_refused=len(considered_instances-admitted_instances),
        c59_exact_edges_considered=len(considered_edges),c59_exact_edges_admitted=len(admitted_edges),
        c59_exact_edges_refused=len(considered_edges-admitted_edges),
        c59_admission_count_scope='unique_authenticated_c59_record_or_exact_edge_all_successor_preflight_in_completed_root_including_factless_edges',
        c59_certified_crossings_with_facts=extra['c59_certified_crossings_with_facts'],
        c59_address_facts_crossing_rows=extra['c59_address_facts_crossing_rows'],
        c59_certificate_lookup_misses=extra['c59_certificate_lookup_misses'],
        c59_all_successor_preflight_refusal_relation_counts=dict(sorted(preflight_refusals.items())),
        root_transactions_attempted=t['root_transactions_attempted'],root_transactions_completed=t['roots_completed'],
        roots_completed=t['roots_completed'],root_transactions_refused=sum(audit.reasons.values()),
        root_refusal_reasons=dict(sorted(audit.reasons.items())),post_c59_uncertified_boundary_refusals=audit.reasons[POST],
        c59_predecessor_path_conflict_refusals=audit.reasons['c59_predecessor_path_conflict'],
        pre_c59_unmatched_instance_refusals=audit.reasons[c59.LATER],
        rows_reached_by_continuation=len(audit.rows),
        remaining_boundary_refusal_relation_counts={k:audit.relations[k] for k in c57.RELATIONS},
        remaining_relation_count_scope='original_edge_callback_attempts_in_aborted_c60_roots_separate_all_successor_preflight_counts',
        reference_candidate_count=count,new_cross_row_candidate_count=count,
        reader_candidates=ops['READ'],writer_candidates=ops['WRITE'],rmw_candidates=ops['RMW'],address_only_candidates=ops['ADDRESS_ONLY'],
        reference_candidates=refs,candidate_records_not_retained=count-len(refs),
        candidate_count_scope='completed_c60_root_entry_instance_operand_observations_not_unique_sites_or_difference_from_c58',
        historical_c58_candidate_counts={k:pool.baseline.result[k] for k in ('reference_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates')},
        external_anchors=e.result['external_anchors'],anchor_counted_as_local_candidate=False,
        c56_c57_c58_c59_evidence_read_only=True,publication_source_kind='unproven',live_qword_reads=0,
        c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
        summary_scope='fresh_c55_engine_per_original_root_shard_new_c60_work_not_comparable_to_c58_or_c59',
        path_conflict_policy='no_c59_certificate_borrowing_through_different_path_same_state_dedup_or_its_descendants',
        **{k:False for k in c56.FALSE_GATES})
    for key in ('continuation_instances_created','continuation_instances_deduplicated','continuation_instances_refused',
        'cross_row_cap_refusals','cross_row_cycles_seen','address_facts_crossing_runtime_rows',
        'call_edges_with_preserved_facts_crossing_rows','no_carried_fact_edges'):
        result[key] = t[key]
    return result


def analyze(data, cache=None, c57_json=None, c58_json=None, c59_json=None, progress=None):
    if len(data)>cfg.old.MAX_FILE or c56.sha(data)!=EXPECTED_SHA:
        raise c58.EvidenceError('exact executable SHA256 mismatch or input cap')
    s = c56.Session(data)
    original = c57.collect_baseline(s,cache,progress=progress)
    e = c58.Evidence(original,owner_result=c56.read_json(c57_json) if c57_json is not None else None,progress=progress)
    b = c59.Baseline(e,owner_result=c56.read_json(c58_json) if c58_json is not None else None,progress=progress)
    p = ProofPool(b,owner_result=c56.read_json(c59_json) if c59_json is not None else None,progress=progress)
    return scan(p,progress=progress)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--c56-cache',type=Path)
    parser.add_argument('--c57-json',type=Path)
    parser.add_argument('--c58-json',type=Path)
    parser.add_argument('--c59-json',type=Path)
    a = parser.parse_args()
    try:
        out = a.output.resolve()
        inputs = [p for p in (a.exe,a.c57_json,a.c58_json,a.c59_json) if p is not None]
        if any(out==p.resolve() for p in inputs) or (a.c56_cache and out.is_relative_to(a.c56_cache.resolve())):
            raise c58.EvidenceError('output overlaps immutable input evidence')
        if a.exe.stat().st_size>cfg.old.MAX_FILE:raise c58.EvidenceError('input file cap')
        result = analyze(a.exe.read_bytes(),a.c56_cache,a.c57_json,a.c58_json,a.c59_json,
            lambda phase,n,total:print(f'{phase}: {n}/{total}',file=sys.stderr))
        c56.atomic_json(a.output,result)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(f'{BLOCKED}: {error}',file=sys.stderr)
        return 2
    print(f'{READY}: {a.output}')
    return 0


if __name__=='__main__':
    raise SystemExit(main())
