"""c58 OFFLINE entry-instance continuation on authenticated c57 exact-row edges.

Never reads slot values. Historical c53-c57 modules/cache evidence are immutable.
Candidates are conditional observations from a single carried entry instance,
not all-entry properties. Any incomplete root transaction emits no candidates.
"""
import argparse
from collections import Counter, deque
from dataclasses import asdict, dataclass
import heapq
from pathlib import Path
import sys

import slot_publication_boundary as c57

c56, cfg = c57.c56, c57.cfg
c55 = c56.c55
VERSION = 'v0.1c58'
SCHEMA = 1
CONTRACT = 'authenticated_first_boundary_exact_runtime_begin_entry_instances_v1'
READY = 'EXACT_RUNTIME_BEGIN_STITCH_SCANNER_READY'
BLOCKED = 'EXACT_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED'
EXPECTED_SHA = c57.EXPECTED_SHA


class EvidenceError(ValueError):
    """Fatal identity failure, never downgraded to a local unsupported row."""


class Refusal(ValueError):
    """Abort an entire root transaction (including all provisional candidates)."""


@dataclass(frozen=True)
class Limits:
    instances: int = 32
    transitions: int = 64
    instructions: int = 4096
    bytes: int = 262144
    edges: int = 8192
    states: int = 4096
    iterations: int = 32768
    blocks: int = 2048
    path_transitions: int = 16
    root_references: int = 64
    retained_references: int = 256


@dataclass(frozen=True)
class AcceptedEdges:
    total: int = 201788
    exact: int = 159734
    interior: int = 29560
    free_rx: int = 12494
    model_only: int = 0
    adjacent: int = 90430


@dataclass(frozen=True)
class RowProof:
    index: int
    row_sha256: str
    code_sha256: str
    pc: int
    size: int
    instruction_sha256: str
    # kind, target, architectural, relation, target index/begin/end, overlap
    edges: tuple

    @classmethod
    def from_record(cls, record):
        return cls(record['source_runtime_row_index'], record['source_runtime_row_sha256'],
            record['source_code_sha256'], record['source_instruction_rva'],
            record['source_instruction_size'], record['source_instruction_sha256'],
            tuple((e['edge_kind'], e['edge_target_rva'], e['architectural_successor'],
                e['target_metadata_relation'], e['target_runtime_row_index'],
                e['target_begin_rva'], e['target_end_rva'], e['source_target_spans_overlap']) for e in record['edges']))


def sources():
    return dict(c57.source_identity(), slot_publication_stitched_py=c56.sha(Path(__file__).read_bytes()))


class Evidence:
    def __init__(self, baseline, *, accepted=AcceptedEdges(), owner_result=None, progress=None):
        self.session = baseline.session
        classifier = c57.Classifier(baseline)
        aggregate = c57.Aggregate(classifier)
        proofs = {}
        for number, witness in enumerate(baseline.witnesses, 1):
            record = classifier.record(witness)
            aggregate.add(record)  # independent original c57 replay
            proofs[witness.index] = RowProof.from_record(record)
            if progress and (number % 1024 == 0 or number == len(baseline.witnesses)):
                progress('c57 authentication', number, len(baseline.witnesses))
        result = aggregate.finish()
        relations = result['target_metadata_relation_counts']
        observed = AcceptedEdges(result['boundary_edges_total'], relations['exact_unique_runtime_begin'],
            relations['inside_unique_runtime_span_nonentry'], relations['executable_file_backed_outside_runtime_metadata'],
            result['model_only_boundary_edges_total'], sum(result['exact_adjacency_edge_kind_counts'].values()))
        if observed != accepted or any(relations[k] for k in c57.INVALID):
            raise EvidenceError('accepted c57 distribution mismatch')
        if owner_result is not None and c56.encoded(owner_result) != c56.encoded(result):
            raise EvidenceError('stale/wrong image/schema/tool/contracts/counts c57 JSON')
        self.result, self.proofs = result, proofs
        self.header = dict(tool_version=VERSION, schema_version=SCHEMA, contract=CONTRACT,
            image_sha256=result['image_sha256'], accepted_c57_boundary_identity=classifier.identity,
            c56_manifest_id=self.session.manifest['manifest_id'], c57_result_sha256=c56.digest(result),
            proof_set_sha256=c56.digest([asdict(p) for p in proofs.values()]), tool_sources=sources())
        self.seal = c56.digest(self.header)

    def assert_current(self):
        self.session.assert_current_tool()
        if (self.header.get('tool_sources') != sources() or c56.digest(self.header) != self.seal
            or self.header['c57_result_sha256'] != c56.digest(self.result)
            or self.header['proof_set_sha256'] != c56.digest([asdict(p) for p in self.proofs.values()])):
            raise EvidenceError('stale/mutated c58/c57 identity/schema/tool/edge evidence')

    def row(self, index):
        if type(index) != int or not 0 <= index < len(self.session.rows):
            raise EvidenceError('wrong runtime row index')
        reason, code, section = self.session.preflight(index)
        if reason:
            raise Refusal(reason)
        begin, end, _ = self.session.rows[index]
        if self.session.domain.relation(begin) != 'exact_runtime_entry':
            raise Refusal('ambiguous_runtime_row')
        cert = self.proofs.get(index)
        if cert and (cert.index != index or cert.row_sha256 != self.session.row_identity(index)
                     or cert.code_sha256 != c56.sha(code)):
            raise EvidenceError('stale/wrong source row/code identity')
        return begin, end, code, section

    def edge(self, index, ins, kind, target, architectural, relations):
        begin, end, _ = self.session.rows[index]
        relation = c57.target_relation(self.session, begin, end, target)
        relations[relation['target_metadata_relation']] += 1
        cert = self.proofs.get(index)
        if cert is None or ins.address != cert.pc:
            raise Refusal('uncertified_later_runtime_boundary')
        if ins.size != cert.size or c56.sha(bytes(ins.bytes)) != cert.instruction_sha256:
            raise EvidenceError('wrong c57 boundary instruction identity')
        candidate = (kind, target, architectural, relation['target_metadata_relation'],
            relation['target_runtime_row_index'], relation['target_begin_rva'],
            relation['target_end_rva'], relation['source_target_spans_overlap'])
        if candidate not in cert.edges:
            raise EvidenceError('wrong c57 architectural edge/target identity')
        if not architectural:
            raise Refusal('model_only_successor')
        if relation['target_metadata_relation'] != 'exact_unique_runtime_begin':
            raise Refusal(relation['target_metadata_relation'])
        if relation['source_target_spans_overlap']:
            raise Refusal('overlapping_source_target_spans')
        dest = relation['target_runtime_row_index']
        low, high, raw, _ = self.row(dest)
        if (low, high) != (relation['target_begin_rva'], relation['target_end_rva']):
            raise EvidenceError('wrong target row bounds')
        proof = dict(accepted_c57_boundary_identity=self.header['accepted_c57_boundary_identity'],
            image_sha256=self.header['image_sha256'], source_runtime_row=index,
            source_row_sha256=cert.row_sha256, source_code_sha256=cert.code_sha256,
            source_instruction_rva=ins.address, source_instruction_size=ins.size,
            source_instruction_sha256=cert.instruction_sha256, edge_kind=kind,
            architectural_successor=True, target_metadata_relation=relation['target_metadata_relation'],
            target_runtime_row=dest, target_begin_rva=low, target_end_rva=high,
            target_row_sha256=self.session.row_identity(dest), target_code_sha256=c56.sha(raw))
        proof['edge_identity'] = c56.digest(proof)
        return proof


def state_id(state):
    return c56.digest([None if f is None else (f.rva, f.chain) for f in state])


class Transaction:
    def __init__(self, evidence, root, engine, limits):
        self.e, self.s, self.root, self.engine, self.limits = evidence, evidence.session, root, engine, limits
        self.work, self.relations = Counter(), Counter()
        self.considered, self.admitted = set(), set()
        self.contexts, self.forward, self.reverse = {}, {}, {}
        self.references, self.crossings = [], []
        self.created = self.deduplicated = self.cycles = 0

    def charge(self, key, count=1):
        self.work[key] += count
        if self.work[key] > getattr(self.limits, key):
            raise Refusal('cross_row_' + key + '_cap')

    def closure(self, graph, start):
        found, todo = set(), [start]
        while todo:
            self.charge('iterations')
            node = todo.pop()
            if node in found:
                continue
            found.add(node)
            todo.extend(sorted(graph[node] - found))
        return found

    def connect(self, source, target):
        # Check *all* old paths introduced by this edge, not just first ancestry.
        ancestors = self.closure(self.reverse, source)
        descendants = self.closure(self.forward, target)
        for a in sorted(ancestors):
            for b in sorted(descendants):
                self.charge('iterations')
                if a[0] == b[0] and a[1] != b[1]:
                    self.cycles += 1
                    raise Refusal('state_changing_cross_row_cycle')
        if ancestors & descendants:
            self.cycles += 1
        self.forward[source].add(target)
        self.reverse[target].add(source)

    def instance(self, row, state, path):
        key = (row, state_id(state))
        if key in self.contexts:
            self.deduplicated += 1
            return key
        if len(self.contexts) >= self.limits.instances:
            raise Refusal('cross_row_instances_cap')
        if len(path) > self.limits.path_transitions:
            raise Refusal('cross_row_path_transitions_cap')
        self.contexts[key] = (state, path)
        self.forward[key], self.reverse[key] = set(), set()
        if path:
            self.created += 1
        return key

    def scan_row(self, key):
        index, identity = key
        seed, path = self.contexts[key]
        begin, end, code, section = self.e.row(index)
        caps = self.s.profile.main
        self.charge('bytes', len(code))
        decoded, nodes, edges, crossing, masks, call_proofs = {}, {}, {}, {}, {}, {}
        md, cursor = cfg.old.decoder(), begin
        while cursor < end:
            if len(decoded) >= min(caps.instructions, caps.region_instructions):
                raise Refusal('instruction_cap')
            self.charge('instructions')
            ins = next(md.disasm(code[cursor-begin:cursor-begin+15], cursor, count=1), None)
            if ins is None:
                raise Refusal('decode_gap')
            decoded[cursor] = ins
            cursor += ins.size
        todo, queued = [begin], {begin}
        local_edges = 0
        call_allowed = lambda pc: any(b <= pc < min(e, b+n) and flags & 0x60000000 == 0x60000000
            and not flags & 0x80000000 for b,e,_,n,flags in self.s.sections)
        while todo:
            pc = heapq.heappop(todo)
            if pc in nodes:
                continue
            if pc not in decoded:
                raise Refusal('branch_into_middle_bytes')
            if len(nodes) >= min(caps.states, caps.region_states):
                raise Refusal('state_cap')
            self.charge('states')
            ins = nodes[pc] = decoded[pc]
            cfg.transfer(ins, cfg.EMPTY, self.s.base, self.s.size, caps.hops)
            try:
                following, _ = cfg.successors(ins, begin, end, self.s.size, call_allowed)
            except ValueError as error:
                if str(error) != 'unsupported_runtime_boundary':
                    raise Refusal(str(error)) from error
                following = []
                crossing[pc] = []
                for kind, target, arch, _ in c57.diagnostic_successors(ins):
                    if not arch:
                        raise Refusal('model_only_successor')
                    if begin <= target < end:
                        following.append((target, kind))
                    else:
                        # Count considered before target code preflight can refuse.
                        cert = self.e.proofs.get(index)
                        edge_key = (index, pc, kind, target)
                        if cert and any(e[:4] == (kind, target, True, 'exact_unique_runtime_begin') for e in cert.edges):
                            self.considered.add(edge_key)
                        proof = self.e.edge(index, ins, kind, target, arch, self.relations)
                        crossing[pc].append((proof, edge_key))
            edges[pc] = following
            for target, _ in following:
                local_edges += 1
                self.charge('edges')
                if local_edges > min(caps.edges, caps.region_edges):
                    raise Refusal('edge_cap')
                if target not in queued:
                    heapq.heappush(todo, target)
                    queued.add(target)
            for _ in crossing.get(pc, ()):
                local_edges += 1
                self.charge('edges')
                if local_edges > min(caps.edges, caps.region_edges):
                    raise Refusal('edge_cap')
        predecessors = {pc: set() for pc in nodes}
        for pc, following in edges.items():
            for target, _ in following:
                predecessors[target].add(pc)
        previous, blocks, nblocks = None, {}, 0
        for pc in sorted(nodes):
            if previous is None or predecessors[pc] != {previous} or edges[previous] != [(pc, 'fallthrough')]:
                nblocks += 1
                self.charge('blocks')
                if nblocks > min(caps.blocks, caps.region_blocks):
                    raise Refusal('block_cap')
                block = pc
            blocks[pc] = block
            previous = pc
        states = {begin: seed}
        reasons = {begin: tuple('exact_static_image_address' if f else 'register_base_without_exact_origin' for f in seed)}
        todo, queued, iterations = deque([begin]), {begin}, 0

        def preserve(pc):
            ins, state = nodes[pc], states[pc]
            if ins.group(cfg.cs.CS_GRP_CALL) and pc not in masks and any(f for i,f in enumerate(state) if i != 4):
                proof = self.engine.for_site(ins.operands[0].imm, dict(site_rva=pc, instruction_size=ins.size,
                    instruction_sha256=c56.sha(bytes(ins.bytes)), caller_entry_rva=begin, caller_end_rva=end,
                    caller_code_sha256=c56.sha(code), caller_contract='authenticated_runtime_caller_normal_CFG'))
                if any(s in (proof.get('refusal_reason') or '').lower() for s in ('cap','budget','exhausted')):
                    raise Refusal('summary_cap_refused')
                masks[pc] = tuple(i for i in proof.get('preserved_banks', ()) if type(i) == int and 0 <= i < 16 and i != 4) if proof.get('status') == 'ADMITTED' else ()
                call_proofs[pc] = proof
            return masks.get(pc, ())

        while todo:
            pc = todo.popleft()
            queued.remove(pc)
            iterations += 1
            self.charge('iterations')
            if iterations > min(caps.iterations, caps.region_iterations):
                raise Refusal('iteration_cap')
            preserved = preserve(pc)
            nxt = cfg.transfer(nodes[pc], states[pc], self.s.base, self.s.size, caps.hops, preserved)
            why = cfg.transfer_reasons(nodes[pc], states[pc], reasons[pc], nxt, preserved)
            for target, _ in edges[pc]:
                before = states.get(target)
                merged = nxt if before is None else tuple(a if a == b else None for a,b in zip(before,nxt))
                merged_why = why if before is None else tuple('exact_static_image_address' if f else
                    'cfg_merge_conflict' if before[i] != nxt[i] else reasons[target][i] if reasons[target][i] == why[i] else 'cfg_merge_unknown'
                    for i,f in enumerate(merged))
                if before != merged or reasons.get(target) != merged_why:
                    states[target], reasons[target] = merged, merged_why
                    if target not in queued:
                        todo.append(target)
                        queued.add(target)
        graph_hash = c56.digest([(pc, edges[pc], crossing.get(pc, ())) for pc in sorted(nodes)])
        outgoing = []
        for pc in sorted(nodes):
            ins, state = nodes[pc], states[pc]
            nxt = cfg.transfer(ins, state, self.s.base, self.s.size, caps.hops, masks.get(pc, ()))
            for proof, edge_key in crossing.get(pc, ()):
                if any(nxt):
                    outgoing.append((proof, edge_key, nxt))
                else:
                    self.work['no_carried_fact_edges'] += 1
            if not path:
                continue  # historical root candidates stay in c56 only
            for operand_index, op in enumerate(ins.operands):
                if op.type != cfg.x.X86_OP_MEM:
                    continue
                fact = state[cfg.old.PARENT[op.mem.base]] if op.mem.base in cfg.old.FULL else None
                # New records must actually depend on an incoming carried fact.
                if fact is None or not any(f and fact.chain[:len(f.chain)] == f.chain for f in seed):
                    continue
                extent = 1 if ins.id == cfg.x.X86_INS_LEA else op.size
                effective, _, error = cfg.address(ins, op, state, self.s.base, self.s.size, extent, reasons[pc])
                if error or not effective < cfg.SLOT+8 or not cfg.SLOT < effective+extent:
                    continue
                operation, access_proof = cfg.access_kind(ins, operand_index)
                if operation is None:
                    continue
                if len(self.references) >= self.limits.root_references:
                    raise Refusal('cross_row_root_references_cap')
                self.references.append(dict(original_root_runtime_row=self.root, continuation_runtime_row=index,
                    original_root_row_sha256=self.s.row_identity(self.root), target_row_sha256=self.s.row_identity(index),
                    target_code_sha256=c56.sha(code), incoming_state_identity=identity,
                    entry_instance_identity=c56.digest((self.e.seal,self.root,key)),
                    boundary_path=path, site_rva=pc, instruction_size=ins.size,
                    instruction_sha256=c56.sha(bytes(ins.bytes)), operand_index=operand_index,
                    operand_width=op.size, access_width=0 if operation == 'ADDRESS_ONLY' else op.size,
                    operation=operation, may_write=operation in ('WRITE','RMW'), access_proof=access_proof,
                    effective_rva=effective, overlap=dict(begin_rva=max(effective,cfg.SLOT),
                    end_rva=min(effective+extent,cfg.SLOT+8), address_point_only=operation=='ADDRESS_ONLY'),
                    address_proof_chain=fact.chain, cfg_graph_sha256=graph_hash,
                    call_summary_evidence_sha256=c56.digest(call_proofs), fixpoint_complete=True,
                    all_reachable_incoming_states_agree_within_instance=True,
                    scope='path_entry_instance_static_candidate_not_all_entry_invariant'))
        return outgoing

    def run(self):
        key = self.instance(self.root, cfg.EMPTY, ())
        todo, processed = deque([key]), set()
        while todo:
            key = todo.popleft()
            if key in processed:
                continue
            processed.add(key)
            outgoing = self.scan_row(key)
            for proof, edge_key, state in outgoing:
                self.charge('transitions')
                path = self.contexts[key][1] + (proof,)
                target = self.instance(proof['target_runtime_row'], state, path)
                self.connect(key, target)
                self.admitted.add(edge_key)
                self.crossings.append((proof['target_runtime_row'], sum(f is not None for f in state), proof['edge_kind']))
                if target not in processed:
                    todo.append(target)
        return self.references


def scan(evidence, limits=Limits(), progress=None):
    if any(type(n) != int or n <= 0 for n in asdict(limits).values()):
        raise ValueError('all c58 caps must be positive integers')
    evidence.assert_current()
    totals, root_refusals, relations, operations = Counter(), Counter(), Counter(), Counter()
    considered, admitted, reached, records = set(), set(), set(), []
    engine, shard = None, None
    for number, root in enumerate(evidence.proofs, 1):
        current = root // evidence.session.profile.rows_per_shard
        if current != shard:
            engine = c55.Engine(evidence.session.domain, evidence.session.profile.summary)
            shard = current
        tx = Transaction(evidence, root, engine, limits)
        try:
            refs = tx.run()
        except EvidenceError:
            raise
        except (Refusal, ValueError) as error:
            reason = str(error)
            root_refusals[reason] += 1
            totals['continuation_instances_refused'] += tx.created
            totals['cross_row_cap_refusals'] += any(s in reason for s in ('cap','budget','exhausted'))
            relations.update(tx.relations)
        else:
            admitted.update(tx.admitted)
            for row, facts, kind in tx.crossings:
                reached.add(row)
                totals['address_facts_crossing_runtime_rows'] += facts
                totals['call_edges_with_preserved_facts_crossing_rows'] += kind == 'call_fallthrough'
            operations.update(ref['operation'] for ref in refs)
            records.extend(refs[:max(0,limits.retained_references-len(records))])
            totals['roots_completed'] += 1
        totals['no_carried_fact_edges'] += tx.work['no_carried_fact_edges']
        considered.update(tx.considered)
        totals['continuation_instances_created'] += tx.created
        totals['continuation_instances_deduplicated'] += tx.deduplicated
        totals['cross_row_cycles_seen'] += tx.cycles
        if progress and (number % 1024 == 0 or number == len(evidence.proofs)):
            progress('c58 root transactions', number, len(evidence.proofs))
    evidence.assert_current()
    count = sum(operations.values())
    result = dict(repository_result=READY, tool_version=VERSION, schema_version=SCHEMA, contract=CONTRACT,
        evidence='OFFLINE_STATIC_ENTRY_INSTANCE_CANDIDATES_ONLY', image_authenticated=True,
        image_sha256=evidence.header['image_sha256'], accepted_c57_boundary_identity=evidence.header['accepted_c57_boundary_identity'],
        authentication_header=evidence.header, limits=asdict(limits),
        exact_runtime_begin_edges_available=evidence.result['target_metadata_relation_counts']['exact_unique_runtime_begin'],
        exact_runtime_begin_edges_considered=len(considered), exact_runtime_begin_edges_admitted=len(admitted),
        exact_runtime_begin_edges_refused=len(considered-admitted),
        edge_count_scope='unique_c57_architectural_exact_edges_considered_partition_admitted_in_completed_root_else_refused',
        root_transactions_attempted=len(evidence.proofs), root_transactions_refused=sum(root_refusals.values()),
        continuation_refusal_count_scope='created_nonroot_contexts_invalidated_by_aborted_root_transaction',
        rows_reached_by_continuation=len(reached), root_refusal_reasons=dict(sorted(root_refusals.items())),
        remaining_boundary_refusal_relation_counts={k:relations[k] for k in c57.RELATIONS},
        remaining_relation_count_scope='attempted_cross_edges_in_aborted_roots_not_complete_writer_domain',
        reference_candidate_count=count, new_cross_row_candidate_count=count,
        reader_candidates=operations['READ'], writer_candidates=operations['WRITE'], rmw_candidates=operations['RMW'],
        address_only_candidates=operations['ADDRESS_ONLY'], reference_candidates=records,
        candidate_count_scope='root_entry_instance_operand_observations_not_unique_global_sites_or_all_entry_invariants',
        candidate_records_not_retained=count-len(records),
        historical_c56_candidates={k:evidence.result[k] for k in ('reference_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates')},
        external_anchors=evidence.result['external_anchors'], anchor_counted_as_local_candidate=False,
        publication_source_kind='unproven', c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
        live_qword_reads=0, cross_runtime_fact_propagation=True,
        cross_runtime_fact_propagation_scope='offline_completed_entry_instances_only',
        c56_checkpoints_read_only=True, c57_JSON_read_only=True,
        summary_scope='fresh_c55_engine_per_original_c56_128row_root_shard_no_ABI_assumptions',
        **{k:False for k in c56.FALSE_GATES})
    for name in ('continuation_instances_created','continuation_instances_deduplicated','continuation_instances_refused',
                 'cross_row_cycles_seen','cross_row_cap_refusals','address_facts_crossing_runtime_rows',
                 'call_edges_with_preserved_facts_crossing_rows','roots_completed','no_carried_fact_edges'):
        result[name] = totals[name]
    return result


def analyze(data, cache=None, c57_json=None, progress=None):
    if len(data) > cfg.old.MAX_FILE or c56.sha(data) != EXPECTED_SHA:
        raise EvidenceError('exact executable SHA256 mismatch or input cap')
    session = c56.Session(data)
    baseline = c57.collect_baseline(session, cache, progress=progress)
    owner = c56.read_json(c57_json) if c57_json is not None else None
    return scan(Evidence(baseline, owner_result=owner, progress=progress), progress=progress)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe', type=Path)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--c56-cache', type=Path)
    parser.add_argument('--c57-json', type=Path)
    args = parser.parse_args()
    try:
        # Never overwrite the private input, old JSON, or any checkpoint path.
        output = args.output.resolve()
        inputs = [args.exe] + ([args.c57_json] if args.c57_json else [])
        if any(output == p.resolve() for p in inputs) or (args.c56_cache and output.is_relative_to(args.c56_cache.resolve())):
            raise EvidenceError('output overlaps immutable input evidence')
        if args.exe.stat().st_size > cfg.old.MAX_FILE:
            raise EvidenceError('input file cap')
        result = analyze(args.exe.read_bytes(), args.c56_cache, args.c57_json,
            lambda phase,n,total: print(f'{phase}: {n}/{total}', file=sys.stderr))
        c56.atomic_json(args.output, result)
    except (OSError, ValueError, KeyError, TypeError, RecursionError) as error:
        print(f'{BLOCKED}: {error}', file=sys.stderr)
        return 2
    print(f'{READY}: {args.output}')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
