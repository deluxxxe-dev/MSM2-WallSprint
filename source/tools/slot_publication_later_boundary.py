"""c59 OFFLINE diagnostics of c58's first uncertified later-boundary abort.

Uses unchanged c58 transactions and c53/c57 successor semantics. The observer
records an existing refusal and rethrows it. It never admits another edge, writes
old evidence, resumes an aborted transaction, or exports a new candidate.
"""
import argparse
from collections import Counter
from dataclasses import asdict, dataclass
import hashlib
from pathlib import Path
import sys

import slot_publication_stitched as c58

c57, c56, cfg = c58.c57, c58.c56, c58.cfg
VERSION = 'v0.1c59'
SCHEMA = 1
CONTRACT = 'exact_c58_first_terminal_later_boundary_entry_instance_diagnostic_v1'
READY = 'LATER_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_READY'
BLOCKED = 'LATER_RUNTIME_BOUNDARY_EDGE_CLASSIFIER_BLOCKED'
EXPECTED_SHA = c58.EXPECTED_SHA
LATER = 'uncertified_later_runtime_boundary'
ACCEPTED_C58 = dict(
    accepted_c57_boundary_identity='b990e0d13ed735418fda2669a8e54fec4f7e0b5647d846a4fbaf99eb80639158',
    exact_runtime_begin_edges_available=159734, exact_runtime_begin_edges_considered=147338,
    exact_runtime_begin_edges_admitted=1299, exact_runtime_begin_edges_refused=146039,
    continuation_instances_created=6808, continuation_instances_deduplicated=11,
    continuation_instances_refused=5499, cross_row_cap_refusals=34, cross_row_cycles_seen=0,
    rows_reached_by_continuation=482, address_facts_crossing_runtime_rows=1686,
    call_edges_with_preserved_facts_crossing_rows=0, root_transactions_attempted=162472,
    roots_completed=81896, root_transactions_refused=80576,
    root_refusal_reasons={LATER:33719, 'inside_unique_runtime_span_nonentry':29818,
        'executable_file_backed_outside_runtime_metadata':12526, 'indirect_call':4330,
        'indirect_jump':119, 'system_or_unsupported_transfer':27, 'summary_cap_refused':31,
        'decode_gap':3, 'instruction_cap':3},
    remaining_boundary_refusal_relation_counts={k:dict(exact_unique_runtime_begin=67515,
        inside_unique_runtime_span_nonentry=31384,
        executable_file_backed_outside_runtime_metadata=18680).get(k,0) for k in c57.RELATIONS},
    new_cross_row_candidate_count=0, reference_candidate_count=0, reader_candidates=0,
    writer_candidates=0, rmw_candidates=0, address_only_candidates=0)


def sources():
    return dict(c58.sources(), slot_publication_later_boundary_py=c56.sha(Path(__file__).read_bytes()))


class Baseline:
    """Authenticated original result, including its exact c58 proof/tool identity."""
    def __init__(self, evidence, *, owner_result=None, accepted=None, progress=None):
        self.evidence = evidence
        self.result = c58.scan(evidence, progress=progress)
        expected = ACCEPTED_C58 if accepted is None else accepted  # owned Python seam only
        if any(c56.encoded(self.result.get(k)) != c56.encoded(v) for k,v in expected.items()):
            raise c58.EvidenceError('accepted c58 owner result/count/identity mismatch')
        if owner_result is not None and c56.encoded(owner_result) != c56.encoded(self.result):
            raise c58.EvidenceError('stale/wrong image/tool/schema/contracts/state/proof c58 JSON')
        if (self.result['repository_result'] != c58.READY or self.result['tool_version'] != c58.VERSION
            or self.result['schema_version'] != c58.SCHEMA or self.result['contract'] != c58.CONTRACT
            or self.result['authentication_header'] != evidence.header
            or any(self.result[k] for k in c56.FALSE_GATES)):
            raise c58.EvidenceError('wrong c58 result/proof or completeness identity')
        self.header = dict(tool_version=VERSION, schema_version=SCHEMA, contract=CONTRACT,
            image_sha256=self.result['image_sha256'], c56_manifest_id=evidence.header['c56_manifest_id'],
            accepted_c57_boundary_identity=evidence.header['accepted_c57_boundary_identity'],
            accepted_c58_result_identity=c56.digest(self.result), accepted_c58_proof_identity=evidence.seal,
            c58_tool_version=c58.VERSION, c58_contract=c58.CONTRACT, c58_limits=self.result['limits'],
            later_boundary_refused_roots_expected=self.result['root_refusal_reasons'].get(LATER,0),
            tool_sources=sources())
        self.identity = c56.digest(self.header)

    def assert_current(self):
        self.evidence.assert_current()
        if (self.header.get('tool_sources') != sources() or c56.digest(self.header) != self.identity
            or c56.digest(self.result) != self.header['accepted_c58_result_identity']):
            raise c58.EvidenceError('stale/mutated c59 image/tool/schema/contract/c58 result identity')


@dataclass(frozen=True)
class Capture:
    root: int
    row: int
    state_identity: str
    path_identity: str
    transitions: tuple
    state_fact_count: int
    pc: int
    size: int
    instruction_sha256: str
    row_sha256: str
    code_sha256: str


class ObserverView:
    """No shared-module mutation: delegate every original admission/refusal."""
    def __init__(self, evidence):
        self.original = evidence
        self.session, self.proofs, self.header, self.seal = evidence.session, evidence.proofs, evidence.header, evidence.seal
        self.tx, self.key, self.capture = None, None, None
        self.identical_capture_duplicates = 0

    def row(self, index):
        return self.original.row(index)

    def remember(self, capture):
        if self.capture is None:
            self.capture = capture
        elif self.capture == capture:
            self.identical_capture_duplicates += 1
        else:
            raise c58.EvidenceError('multiple different events for one first terminal c58 abort')

    def edge(self, index, ins, kind, target, architectural, relations):
        try:
            return self.original.edge(index, ins, kind, target, architectural, relations)
        except c58.Refusal as error:
            if str(error) == LATER:
                tx, key = self.tx, self.key
                if tx is None or key is None or key[0] != index:
                    raise c58.EvidenceError('observer lacks exact current c58 context')
                state, path = tx.contexts[key]
                if key[1] != c58.state_id(state):
                    raise c58.EvidenceError('wrong incoming c58 state identity')
                _, _, code, _ = self.row(index)
                self.remember(Capture(tx.root, index, key[1], c56.digest(path),
                    tuple(p['edge_identity'] for p in path), sum(f is not None for f in state),
                    ins.address, ins.size, c56.sha(bytes(ins.bytes)), self.session.row_identity(index), c56.sha(code)))
            raise  # ALWAYS preserve the exact original terminal refusal


class ObservedTransaction(c58.Transaction):
    def scan_row(self, key):
        self.e.tx, self.e.key = self, key
        return super().scan_row(key)


def classify_edges(session, begin, end, ins):
    """Metadata only; c57 semantics, never c57 admission identity/target code."""
    return [dict(edge_kind=kind, edge_target_rva=target, architectural_successor=arch,
                 diagnostic_note=note, **c57.target_relation(session,begin,end,target))
        for kind,target,arch,note in c57.diagnostic_successors(ins) if not begin <= target < end]


def record_for(baseline, tx, view):
    capture = view.capture
    if capture is None:
        raise c58.EvidenceError('c58 later abort missing source instance capture')
    key = (capture.row, capture.state_identity)
    if key not in tx.contexts or tx.root != capture.root:
        raise c58.EvidenceError('wrong root/entry-instance identity')
    state, path = tx.contexts[key]
    if (c58.state_id(state) != capture.state_identity or c56.digest(path) != capture.path_identity
        or tuple(p['edge_identity'] for p in path) != capture.transitions):
        raise c58.EvidenceError('wrong c58 incoming-state/path identity')
    s = baseline.evidence.session
    begin,end,code,_ = baseline.evidence.row(capture.row)
    if s.row_identity(capture.row) != capture.row_sha256 or c56.sha(code) != capture.code_sha256:
        raise c58.EvidenceError('stale later source row/code bytes')
    if not begin <= capture.pc < end or not 1 <= capture.size <= 15:
        raise c58.EvidenceError('invalid bounded later instruction span')
    raw = code[capture.pc-begin:capture.pc-begin+capture.size]
    ins = next(cfg.old.decoder().disasm(raw,capture.pc,count=1),None)
    if ins is None or ins.size != capture.size or c56.sha(bytes(ins.bytes)) != capture.instruction_sha256:
        raise c58.EvidenceError('stale later source instruction bytes')
    allowed = lambda pc: any(b<=pc<min(e,b+n) and flags&0x60000000==0x60000000 and not flags&0x80000000
        for b,e,_,n,flags in s.sections)
    try:
        cfg.successors(ins,begin,end,s.size,allowed)
    except ValueError as error:
        if str(error) != 'unsupported_runtime_boundary':
            raise c58.EvidenceError('later source not original boundary refusal') from error
    else:
        raise c58.EvidenceError('later source has no refused boundary')
    cert = baseline.evidence.proofs.get(capture.row)
    if cert is not None and cert.pc == capture.pc:
        raise c58.EvidenceError('first c57 boundary incorrectly relabeled later')
    edges = classify_edges(s,begin,end,ins)
    if not 1 <= len(edges) <= 2:
        raise c58.EvidenceError('later boundary has no exact outgoing successors')
    contexts = [(row,identity,c56.digest(seed_path)) for (row,identity),(_,seed_path) in sorted(tx.contexts.items())]
    graph = [(key,sorted(tx.forward[key])) for key in sorted(tx.contexts)]
    proof = c56.digest(dict(root=tx.root, contexts=contexts, graph=graph, work=dict(tx.work),
        relations=dict(tx.relations), first_refusal=LATER, capture=asdict(capture)))
    result = dict(classifier_identity=baseline.identity,
        accepted_c58_result_identity=baseline.header['accepted_c58_result_identity'],
        accepted_c58_proof_identity=baseline.header['accepted_c58_proof_identity'],
        original_root_runtime_row=capture.root, original_root_row_sha256=s.row_identity(capture.root),
        current_entry_instance_row=capture.row,
        entry_instance_identity=c56.digest((baseline.evidence.seal,tx.root,key)),
        incoming_state_identity=capture.state_identity, incoming_state_fact_count=capture.state_fact_count,
        boundary_path_identity=capture.path_identity, boundary_transition_identities=capture.transitions,
        boundary_path_length=len(capture.transitions), source_runtime_row_sha256=capture.row_sha256,
        source_begin_rva=begin, source_end_rva=end, source_code_sha256=capture.code_sha256,
        source_instruction_rva=capture.pc, source_instruction_size=ins.size,
        source_instruction_sha256=capture.instruction_sha256, source_mnemonic=ins.mnemonic,
        first_refusal=LATER, root_replay_identity=proof, edges=edges,
        instruction_state_scope='entry_seed_identity_graph_build_abort_before_local_must_fixpoint',
        later_boundary_admitted=False, new_candidates_emitted=0)
    result['record_identity'] = c56.digest(result)
    return result


class ReplayAudit:
    """Observe original tx outputs; compare every dynamic c58 aggregate field."""
    def __init__(self):
        self.totals, self.reasons, self.relations, self.ops = Counter(), Counter(), Counter(), Counter()
        self.considered, self.admitted, self.rows, self.references = set(), set(), set(), []

    def add(self, tx, reason):
        t = self.totals
        if reason is not None:
            self.reasons[reason] += 1
            t['continuation_instances_refused'] += tx.created
            t['cross_row_cap_refusals'] += any(s in reason for s in ('cap','budget','exhausted'))
            self.relations.update(tx.relations)
        else:
            self.admitted.update(tx.admitted)
            for row,facts,kind in tx.crossings:
                self.rows.add(row)
                t['address_facts_crossing_runtime_rows'] += facts
                t['call_edges_with_preserved_facts_crossing_rows'] += kind=='call_fallthrough'
            self.ops.update(r['operation'] for r in tx.references)
            self.references.extend(tx.references[:max(0,c58.Limits().retained_references-len(self.references))])
            t['roots_completed'] += 1
        self.considered.update(tx.considered)
        t['continuation_instances_created'] += tx.created
        t['continuation_instances_deduplicated'] += tx.deduplicated
        t['cross_row_cycles_seen'] += tx.cycles
        t['no_carried_fact_edges'] += tx.work['no_carried_fact_edges']
        t['root_transactions_attempted'] += 1

    def finish(self, original):
        projection = dict(self.totals)
        for name in ('continuation_instances_created','continuation_instances_deduplicated','continuation_instances_refused',
            'cross_row_cycles_seen','cross_row_cap_refusals','address_facts_crossing_runtime_rows',
            'call_edges_with_preserved_facts_crossing_rows','roots_completed','no_carried_fact_edges','root_transactions_attempted'):
            projection[name] = self.totals[name]
        count = sum(self.ops.values())
        projection.update(root_refusal_reasons=dict(self.reasons),root_transactions_refused=sum(self.reasons.values()),
            exact_runtime_begin_edges_considered=len(self.considered),exact_runtime_begin_edges_admitted=len(self.admitted),
            exact_runtime_begin_edges_refused=len(self.considered-self.admitted),rows_reached_by_continuation=len(self.rows),
            remaining_boundary_refusal_relation_counts={k:self.relations[k] for k in c57.RELATIONS},
            reference_candidate_count=count,new_cross_row_candidate_count=count,
            reader_candidates=self.ops['READ'],writer_candidates=self.ops['WRITE'],rmw_candidates=self.ops['RMW'],
            address_only_candidates=self.ops['ADDRESS_ONLY'],reference_candidates=self.references,
            candidate_records_not_retained=count-len(self.references))
        if any(c56.encoded(original.get(k))!=c56.encoded(v) for k,v in projection.items()):
            raise c58.EvidenceError('observer changed original c58 output/refusal/candidate accounting')


def replay_records(baseline, progress=None):
    """Whole ordered replay, preserving shard-local summary budget/cache history.

    Current c58 aborts on its first failure: at most one captured later instance
    per refused root. Do not continue other rows/paths after this terminal abort.
    """
    baseline.assert_current()
    e, audit = baseline.evidence, ReplayAudit()
    engine, shard, classified = None, None, 0
    for number,root in enumerate(e.proofs,1):
        current = root // e.session.profile.rows_per_shard
        if current != shard:
            engine = c58.c55.Engine(e.session.domain,e.session.profile.summary)
            shard = current
        view = ObserverView(e)
        tx = ObservedTransaction(view,root,engine,c58.Limits())
        reason = None
        try:
            tx.run()
        except c58.EvidenceError:
            raise
        except (c58.Refusal,ValueError) as error:
            reason = str(error)
        audit.add(tx,reason)
        if reason == LATER:
            classified += 1
            yield record_for(baseline,tx,view)
        elif view.capture is not None:
            raise c58.EvidenceError('capture does not match root terminal refusal')
        if progress and (number%1024==0 or number==len(e.proofs)):
            progress('c59 original c58 observer replay',number,len(e.proofs))
    audit.finish(baseline.result)
    if classified != baseline.header['later_boundary_refused_roots_expected']:
        raise c58.EvidenceError('missing/mismatched accepted later refusal root set')
    baseline.assert_current()


class Aggregate:
    def __init__(self, baseline, example_limit=8):
        if type(example_limit)!=int or not 0<=example_limit<=8:
            raise ValueError('invalid example bound')
        self.baseline, self.limit, self.expected = baseline, example_limit, iter(replay_records(baseline))
        self.poisoned = False
        self.roots, self.instances, self.states, self.rows = set(), set(), set(), set()
        self.count = self.edges = self.model_only = self.empty = self.root_instances = 0
        self.kinds, self.relations = Counter(), Counter()
        self.exact, self.interior, self.free, self.adjacent = Counter(), Counter(), Counter(), Counter()
        self.examples_kind = {k:[] for k in c57.KINDS}
        self.examples_relation = {k:[] for k in c57.RELATIONS}
        self.record_digest = hashlib.sha256()

    def add(self, record):
        if self.poisoned:
            raise ValueError('later classifier aggregate already poisoned')
        try:
            expected = next(self.expected, None)
            if expected is None or type(record)!=dict or c56.encoded(record)!=c56.encoded(expected):
                raise c58.EvidenceError('missing/duplicate/out-of-order/wrong root/instance/state/path/bytes/classification record')
            self.roots.add(record['original_root_runtime_row'])
            self.instances.add(record['record_identity'])
            self.states.add((record['original_root_runtime_row'],record['current_entry_instance_row'],
                record['incoming_state_identity'],record['boundary_path_identity']))
            self.rows.add(record['current_entry_instance_row'])
            self.count += 1
            self.empty += record['incoming_state_fact_count']==0
            self.root_instances += record['original_root_runtime_row']==record['current_entry_instance_row']
            self.record_digest.update(c56.encoded(record)+b'\n')
            for edge in record['edges']:
                kind, relation = edge['edge_kind'],edge['target_metadata_relation']
                self.edges += 1; self.kinds[kind] += 1; self.relations[relation] += 1
                self.model_only += not edge['architectural_successor']
                if relation=='exact_unique_runtime_begin':self.exact[kind] += 1
                if relation=='inside_unique_runtime_span_nonentry':self.interior[kind] += 1
                if relation=='executable_file_backed_outside_runtime_metadata':self.free[kind] += 1
                if edge['exact_adjacency'] is True:self.adjacent[kind] += 1
                example = {k:v for k,v in record.items() if k!='edges'}
                example.update(edge)
                if len(self.examples_kind[kind])<self.limit:self.examples_kind[kind].append(example)
                if len(self.examples_relation[relation])<self.limit:self.examples_relation[relation].append(example)
        except (ValueError,KeyError,TypeError,IndexError,RecursionError):
            self.poisoned = True
            raise

    def finish(self):
        try:
            b = self.baseline
            if self.poisoned or next(self.expected,None) is not None:
                raise c58.EvidenceError('poisoned/missing later-boundary root or instance records')
            b.assert_current()
            if (len(self.roots)!=b.header['later_boundary_refused_roots_expected']
                or self.count!=len(self.instances) or self.count!=len(self.states)
                or sum(self.kinds.values())!=self.edges or sum(self.relations.values())!=self.edges):
                raise c58.EvidenceError('later-boundary count/identity mismatch')
            return dict(repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
                evidence='OFFLINE_STATIC_DIAGNOSTIC_ONLY',image_authenticated=True,image_sha256=b.header['image_sha256'],
                accepted_c57_boundary_identity=b.header['accepted_c57_boundary_identity'],
                accepted_c58_result_identity=b.header['accepted_c58_result_identity'],
                accepted_c58_proof_identity=b.header['accepted_c58_proof_identity'],
                classifier_header=b.header,classifier_identity=b.identity,
                later_boundary_refused_roots_expected=b.header['later_boundary_refused_roots_expected'],
                later_boundary_refused_roots_classified=len(self.roots),later_boundary_instances_total=self.count,
                later_boundary_edges_total=self.edges,architectural_later_boundary_edges_total=self.edges-self.model_only,
                model_only_later_boundary_edges_total=self.model_only,
                edge_kind_counts={k:self.kinds[k] for k in c57.KINDS},
                target_metadata_relation_counts={k:self.relations[k] for k in c57.RELATIONS},
                exact_runtime_begin_edge_kind_counts={k:self.exact[k] for k in c57.KINDS},
                interior_target_edge_kind_counts={k:self.interior[k] for k in c57.KINDS},
                metadata_free_RX_edge_kind_counts={k:self.free[k] for k in c57.KINDS},
                exact_adjacency_counts={k:self.adjacent[k] for k in c57.KINDS},
                unique_source_rows=len(self.rows),unique_entry_instance_states=len(self.states),
                unique_entry_instance_states_scope='original_root_row_incoming_state_and_first_authenticated_path',
                later_boundary_instances_with_empty_entry_seed=self.empty,
                later_boundary_instances_in_original_root=self.root_instances,
                instance_state_scope='entry_seed_not_uncomputed_instruction_must_state',
                diagnostic_scope='current_c58_first_terminal_abort_per_root_no_post_abort_exploration',
                authenticated_record_set_sha256=self.record_digest.hexdigest(),
                examples_per_class=self.limit,examples_by_edge_kind=self.examples_kind,examples_by_target_relation=self.examples_relation,
                classification_complete=True,cross_runtime_fact_propagation_unchanged=True,
                new_cross_row_candidate_count=0,new_later_boundaries_admitted=0,
                c58_candidate_output_unchanged=True,c58_candidate_output_sha256=c56.digest(b.result['reference_candidates']),
                historical_c58_candidate_counts={k:b.result[k] for k in ('reference_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates','new_cross_row_candidate_count')},
                c56_c57_c58_evidence_read_only=True,publication_source_kind='unproven',live_qword_reads=0,
                c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
                **{k:False for k in c56.FALSE_GATES})
        except (ValueError,KeyError,TypeError,IndexError,RecursionError):
            self.poisoned = True
            raise


def classify(baseline, progress=None):
    aggregate = Aggregate(baseline)
    for record in replay_records(baseline,progress):
        aggregate.add(record)
    return aggregate.finish()


def analyze(data, cache=None, c57_json=None, c58_json=None, progress=None):
    if len(data)>cfg.old.MAX_FILE or c56.sha(data)!=EXPECTED_SHA:
        raise c58.EvidenceError('exact executable SHA256 mismatch or input cap')
    session = c56.Session(data)
    original = c57.collect_baseline(session,cache,progress=progress)
    e = c58.Evidence(original,owner_result=c56.read_json(c57_json) if c57_json is not None else None,progress=progress)
    b = Baseline(e,owner_result=c56.read_json(c58_json) if c58_json is not None else None,progress=progress)
    return classify(b,progress)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--c56-cache',type=Path)
    parser.add_argument('--c57-json',type=Path)
    parser.add_argument('--c58-json',type=Path)
    args = parser.parse_args()
    try:
        output = args.output.resolve()
        inputs = [p for p in (args.exe,args.c57_json,args.c58_json) if p is not None]
        if any(output==p.resolve() for p in inputs) or (args.c56_cache and output.is_relative_to(args.c56_cache.resolve())):
            raise c58.EvidenceError('output overlaps immutable input evidence')
        if args.exe.stat().st_size>cfg.old.MAX_FILE:raise c58.EvidenceError('input file cap')
        result = analyze(args.exe.read_bytes(),args.c56_cache,args.c57_json,args.c58_json,
            lambda phase,n,total:print(f'{phase}: {n}/{total}',file=sys.stderr))
        c56.atomic_json(args.output,result)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(f'{BLOCKED}: {error}',file=sys.stderr)
        return 2
    print(f'{READY}: {args.output}')
    return 0


if __name__=='__main__':
    raise SystemExit(main())
