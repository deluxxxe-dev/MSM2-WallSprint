"""c70 OFFLINE full-replay continuation through accepted c69 exact boundaries.

The accepted c69 checkpoint authenticates the exact 191 first post-c67 terminal
records but is not an exact mutable-transaction resume snapshot.  C70 therefore
replays the historical transaction unchanged, matches the first c68 POST against
the accepted c69 record for that root, atomically authorizes only complete
all-successor exact-runtime-begin records, and stops at the first later boundary.

No slot value read, pointer chase, native/game probe, production integration or
gameplay write is performed.
"""
import argparse
from collections import Counter
from copy import deepcopy
from dataclasses import asdict
import json
from pathlib import Path
import sys

import slot_publication_post_c67_boundary_fix2 as c69fix

c69 = c69fix.base
c68,c67,c66,c65,c64,c63,c62,c61,c60,c59,c58,c57,c56,cfg = (
    c69.c68,c69.c67,c69.c66,c69.c65,c69.c64,c69.c63,c69.c62,c69.c61,
    c69.c60,c69.c59,c69.c58,c69.c57,c69.c56,c69.cfg
)
VERSION = 'v0.1c70'
SCHEMA = 1
CONTRACT = 'exact_c69_full_replay_all_successor_continuation_v1'
READY = 'POST_C69_RUNTIME_BEGIN_STITCH_SCANNER_READY'
BLOCKED = 'POST_C69_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED'
POST = 'uncertified_post_c69_runtime_boundary'
TRANSACTION_REFUSAL = 'c69_transaction_terminal_not_continuable'
EXPECTED_SHA = c69.EXPECTED_SHA
EXPECTED_C69_FILE_SHA = 'e57b7f39ea476e356397f944d8aadb14841b27874736a60c1a82b982eb86537f'
EXPECTED_C69_RESULT_IDENTITY = '08002e4c3aef2c7cfa037040b6f09f7d1b6e247d0b446daea1ffdb2569aa01fb'
EXPECTED_C69_CLASSIFIER_IDENTITY = '4f42b0d04346eb5c7b10c8fe0592edd3a3f19fcb463d66a0adf40be01fd25049'
EXPECTED_C69_RECORD_SET = '20a123c1e83dd6e777b47cb07e86a6a7203acb25e0cede2e86ff956efcfae8ed'
EXPECTED_C69_CHECKPOINT_FILE_SHA = 'd7b8dae21e9e0478e8ea5d8136c7f19156cd5afca2ff80f167dce48a2e61b6b5'
EXPECTED_C69_CHECKPOINT_IDENTITY = 'b734f8ab2f0a24e002c8b5c2ed1dbbfe75c9411cc773ec1394a1cf330c9b3a28'
EXPECTED_RECORDS = 191
EXPECTED_EDGE_BACKED = 190
EXPECTED_TRANSACTION = 1
EXPECTED_ALL_EXACT = 177
EXPECTED_CONTINUABLE_EXACT_EDGES = 191
EXPECTED_INTERIOR_INSTANCES = 13


def sources():
    return dict(c69.sources(),slot_publication_post_c69_stitched_py=c56.sha(Path(__file__).read_bytes()))


def read_c69(path):
    raw = Path(path).read_bytes()
    if len(raw)>c56.MAX_JSON or c56.sha(raw)!=EXPECTED_C69_FILE_SHA:
        raise c58.EvidenceError('stale/wrong physical c69 owner JSON')
    try:
        result = json.loads(raw)
    except (ValueError,TypeError) as error:
        raise c58.EvidenceError('invalid c69 owner JSON') from error
    if c56.digest(result)!=EXPECTED_C69_RESULT_IDENTITY:
        raise c58.EvidenceError('wrong semantic c69 owner result identity')
    return result


def read_c69_checkpoint(path):
    raw = Path(path).read_bytes()
    if len(raw)>c56.MAX_JSON or c56.sha(raw)!=EXPECTED_C69_CHECKPOINT_FILE_SHA:
        raise c58.EvidenceError('stale/wrong physical c69 checkpoint JSON')
    try:
        value = json.loads(raw)
    except (ValueError,TypeError) as error:
        raise c58.EvidenceError('invalid c69 checkpoint JSON') from error
    value = c69.validate_checkpoint(value)
    if value.get('checkpoint_identity')!=EXPECTED_C69_CHECKPOINT_IDENTITY:
        raise c58.EvidenceError('wrong accepted c69 checkpoint identity')
    return value


def _all_exact(record):
    edges = record.get('edges')
    return (record.get('terminal_event_edge_backed',True) is not False and type(edges)==list and bool(edges)
        and all(edge.get('architectural_successor') is True
            and edge.get('target_metadata_relation')=='exact_unique_runtime_begin'
            and edge.get('source_target_spans_overlap') is False for edge in edges))


class ProofPool:
    """Accepted c69 result/checkpoint records over a live full-replay c69 baseline."""
    def __init__(self,baseline,owner_result,checkpoint,*,production=True):
        baseline.assert_current()
        if type(owner_result)!=dict or type(checkpoint)!=dict:
            raise c58.EvidenceError('missing c69 owner result/checkpoint')
        if c56.encoded(checkpoint.get('result'))!=c56.encoded(owner_result):
            raise c58.EvidenceError('c69 result/checkpoint result mismatch')
        if (owner_result.get('repository_result')!=c69.READY or owner_result.get('tool_version')!=c69.VERSION
            or owner_result.get('schema_version')!=c69.SCHEMA or owner_result.get('contract')!=c69.CONTRACT
            or owner_result.get('classification_complete') is not True
            or owner_result.get('terminal_origin_classification_complete') is not True
            or owner_result.get('classifier_identity')!=baseline.identity
            or owner_result.get('classifier_header')!=baseline.header
            or any(owner_result.get(k) for k in c56.FALSE_GATES)
            or owner_result.get('c56_through_c68_evidence_read_only') is not True):
            raise c58.EvidenceError('wrong/incomplete c69 owner proof contract')
        header = checkpoint.get('header',{})
        if (header.get('c69_classifier_identity')!=baseline.identity
            or header.get('c69_result_identity')!=c56.digest(owner_result)
            or header.get('record_stream_sha256')!=owner_result.get('authenticated_record_set_sha256')):
            raise c58.EvidenceError('c69 checkpoint/header/result identity mismatch')
        if production and (c56.digest(owner_result)!=EXPECTED_C69_RESULT_IDENTITY
            or owner_result.get('classifier_identity')!=EXPECTED_C69_CLASSIFIER_IDENTITY
            or owner_result.get('authenticated_record_set_sha256')!=EXPECTED_C69_RECORD_SET
            or checkpoint.get('checkpoint_identity')!=EXPECTED_C69_CHECKPOINT_IDENTITY):
            raise c58.EvidenceError('wrong accepted production c69 identities')
        records = checkpoint.get('records')
        if type(records)!=list:
            raise c58.EvidenceError('missing c69 checkpoint records')
        index = {}
        for record in records:
            if type(record)!=dict or c56.digest({k:v for k,v in record.items() if k!='record_identity'})!=record.get('record_identity'):
                raise c58.EvidenceError('bad c69 checkpoint record identity')
            root = record.get('original_root_runtime_row')
            if type(root)!=int or root in index:
                raise c58.EvidenceError('duplicate/invalid c69 original root')
            index[root] = record
        edge_backed = sum(record.get('terminal_event_edge_backed',True) is not False for record in records)
        transaction = len(records)-edge_backed
        all_exact = [record for record in records if _all_exact(record)]
        continuable_edges = sum(len(record['edges']) for record in all_exact)
        interior_instances = sum(any(edge.get('target_metadata_relation')=='inside_unique_runtime_span_nonentry'
            for edge in record.get('edges',())) for record in records)
        if production and (len(records)!=EXPECTED_RECORDS or edge_backed!=EXPECTED_EDGE_BACKED
            or transaction!=EXPECTED_TRANSACTION or len(all_exact)!=EXPECTED_ALL_EXACT
            or continuable_edges!=EXPECTED_CONTINUABLE_EXACT_EDGES or interior_instances!=EXPECTED_INTERIOR_INSTANCES):
            raise c58.EvidenceError('accepted c69 continuation-domain count mismatch')
        self.baseline,self.result,self.checkpoint = baseline,deepcopy(owner_result),deepcopy(checkpoint)
        self.records,self.index = tuple(deepcopy(records)),index
        self.all_exact_record_ids = frozenset(record['record_identity'] for record in all_exact)
        self.continuable_edge_ids = frozenset(c56.digest((record['record_identity'],edge))
            for record in all_exact for edge in record['edges'])
        self.header = dict(tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_sha256=baseline.header['image_sha256'],c56_manifest_id=baseline.header['c56_manifest_id'],
            **{k:v for k,v in baseline.header.items() if k.startswith('accepted_')},
            accepted_c69_classifier_identity=owner_result['classifier_identity'],
            accepted_c69_result_identity=c56.digest(owner_result),
            accepted_c69_record_set_identity=owner_result['authenticated_record_set_sha256'],
            accepted_c69_checkpoint_identity=checkpoint['checkpoint_identity'],
            accepted_c69_checkpoint_record_stream=header['record_stream_sha256'],
            accepted_c69_physical_sha256=EXPECTED_C69_FILE_SHA if production else None,
            accepted_c69_checkpoint_physical_sha256=EXPECTED_C69_CHECKPOINT_FILE_SHA if production else None,
            c69_contract=c69.CONTRACT,c69_checkpoint_contract=header.get('checkpoint_contract'),
            limits=asdict(c58.Limits()),tool_sources=sources(),
            full_historical_replay_used=True,checkpoint_accelerated_resume_used=False)
        self.identity = c56.digest(self.header)

    def assert_current(self):
        self.baseline.assert_current()
        if (self.header.get('tool_sources')!=sources() or c56.digest(self.header)!=self.identity
            or self.baseline.identity!=self.result.get('classifier_identity')
            or self.baseline.header!=self.result.get('classifier_header')
            or c56.digest(self.result)!=self.header['accepted_c69_result_identity']
            or self.checkpoint.get('checkpoint_identity')!=self.header['accepted_c69_checkpoint_identity']
            or self.checkpoint.get('header',{}).get('record_stream_sha256')!=self.header['accepted_c69_checkpoint_record_stream']):
            raise c58.EvidenceError('stale/mutated c70 c69 proof pool')

    def lookup(self,root):
        return self.index.get(root)


def _path_c69_ids(path):
    return [p['c69_record_identity'] for p in path if p.get('proof_kind')=='c69_post_boundary']


class View(c69.View):
    def __init__(self,certificates):
        super().__init__(certificates.baseline.certificates)
        self.c70_certificates = certificates
        self.c69_considered,self.c69_preflight_eligible = set(),set()
        self.c69_edges_considered,self.c69_edges_preflight_eligible = set(),set()
        self.c69_source_keys,self.c69_proofs = set(),{}
        self.c69_record,self.c69_key,self.c69_scan_token,self.c69_prefix = None,None,None,None
        self.c69_siblings_used,self.c69_cache_identity = set(),None
        self.c69_refusals,self.c69_mismatches = Counter(),Counter()

    def _post69_boundary(self,index,ins,kind,target,architectural,relations):
        begin,end,_ = self.session.rows[index]
        relations[c57.target_relation(self.session,begin,end,target)['target_metadata_relation']] += 1
        raise c58.Refusal(POST)

    def edge(self,index,ins,kind,target,architectural,relations):
        tx,key = self.tx,self.key
        if tx is None or key is None or key[0]!=index:
            raise c58.EvidenceError('missing current c70 source context')
        if (self.c69_record is not None and key==self.c69_key and tx.scan_token==self.c69_scan_token
            and ins.address==self.c69_record['source_instruction_rva']):
            if c68.sibling_prefix(tx,self)!=self.c69_prefix or c56.digest((self.c69_record,sorted(self.c69_proofs.items())))!=self.c69_cache_identity:
                raise c58.Refusal('c69_sibling_context_or_provenance_conflict')
            sibling = (kind,target,architectural)
            if sibling in self.c69_siblings_used:
                raise c58.Refusal('duplicate_c69_sibling_callback')
            self.c69_siblings_used.add(sibling)
            proof = self.c69_proofs.get(sibling)
            if proof is None:
                raise c58.EvidenceError('unmatched exact c69 sibling successor')
            return proof
        if key in tx.post_c69 or (key in self.c69_source_keys and
            (self.c69_record is None or ins.address!=self.c69_record['source_instruction_rva'] or tx.scan_token!=self.c69_scan_token)):
            return self._post69_boundary(index,ins,kind,target,architectural,relations)
        try:
            return super().edge(index,ins,kind,target,architectural,relations)
        except c58.Refusal as error:
            if str(error)!=c68.POST:
                raise
        expected = self.c70_certificates.lookup(tx.root)
        if expected is None:
            self.c69_mismatches['unrecorded_c68_post_root'] += 1
            raise c58.EvidenceError('c68 POST root missing accepted c69 record')
        if expected.get('terminal_event_edge_backed',True) is False:
            raise c58.EvidenceError('accepted transaction c69 terminal appeared as edge callback')
        rid = expected['record_identity'];self.c69_considered.add(rid)
        current = c69.record_for(self.c70_certificates.baseline,tx,self)
        if c56.encoded(current)!=c56.encoded(expected):
            self.c69_mismatches['full_c69_record_mismatch'] += 1
            raise c58.EvidenceError('stale/wrong c69 root/state/path/source/phase/reason/provenance')
        edges = expected['edges']
        all_exact = _all_exact(expected)
        if not all_exact:
            for edge in edges:
                relation = edge.get('target_metadata_relation','invalid')
                if relation!='exact_unique_runtime_begin':
                    self.c69_refusals[relation] += 1
            raise c58.Refusal('c69_preflight_nonexact_successor')
        for edge in edges:
            self.c69_edges_considered.add(c56.digest((rid,edge)))
        begin,end,code,_ = self.row(index)
        proofs = {}
        for edge in edges:
            if edge['source_target_spans_overlap']:
                self.c69_refusals['overlapping_source_target_spans'] += 1
                raise c58.Refusal('c69_preflight_overlapping_source_target_spans')
            dest = edge['target_runtime_row_index']
            try:
                low,high,raw,_ = self.row(dest)
            except c58.Refusal as target_error:
                self.c69_refusals['target_code_preflight_refused'] += 1
                raise c58.Refusal('c69_preflight_target_code_refused') from target_error
            if (low,high)!=(edge['target_begin_rva'],edge['target_end_rva']) or low!=edge['edge_target_rva']:
                raise c58.EvidenceError('wrong exact c69 target metadata/bounds')
            proof = dict(image_sha256=self.c70_certificates.header['image_sha256'],
                accepted_c69_classifier_identity=self.c70_certificates.header['accepted_c69_classifier_identity'],
                accepted_c69_result_identity=self.c70_certificates.header['accepted_c69_result_identity'],
                accepted_c69_record_set_identity=self.c70_certificates.header['accepted_c69_record_set_identity'],
                accepted_c69_checkpoint_identity=self.c70_certificates.header['accepted_c69_checkpoint_identity'],
                source_runtime_row=index,source_row_sha256=expected['source_runtime_row_sha256'],
                source_code_sha256=c56.sha(code),source_instruction_rva=ins.address,
                source_instruction_size=ins.size,source_instruction_sha256=c56.sha(bytes(ins.bytes)),
                edge_kind=edge['edge_kind'],architectural_successor=True,
                target_metadata_relation='exact_unique_runtime_begin',target_runtime_row=dest,
                target_begin_rva=low,target_end_rva=high,target_row_sha256=self.session.row_identity(dest),
                target_code_sha256=c56.sha(raw),proof_kind='c69_post_boundary',
                c69_record_identity=rid,c69_edge_identity=c56.digest((rid,edge)),
                c69_classifier_identity=expected['classifier_identity'],
                c69_result_identity=self.c70_certificates.header['accepted_c69_result_identity'],
                c69_full_prefix_identity=expected['root_replay_identity'],
                source_entry_instance_identity=expected['entry_instance_identity'],
                source_incoming_state_identity=expected['incoming_state_identity'],
                source_boundary_path_identity=expected['boundary_path_identity'],
                terminal_phase=expected['terminal_phase'],terminal_reason=expected['terminal_reason'],
                terminal_provenance=deepcopy(expected['terminal_provenance']),
                post_c67_origin=deepcopy(expected.get('post_c67_origin')),
                c67_authorization=deepcopy(expected['c67_authorization']),
                c67_terminal_phase=expected['c67_terminal_phase'],c67_terminal_reason=expected['c67_terminal_reason'],
                local_c67_edge_identities=deepcopy(expected['local_c67_edge_identities']))
            proof['edge_identity'] = c56.digest(proof)
            proofs[(edge['edge_kind'],edge['edge_target_rva'],edge['architectural_successor'])] = proof
        if len(proofs)!=len(edges):
            raise c58.EvidenceError('incomplete c69 all-successor preflight')
        self.c69_record,self.c69_key,self.c69_scan_token = expected,key,tx.scan_token
        self.c69_proofs = proofs;self.c69_source_keys.add(key)
        self.c69_prefix = c68.sibling_prefix(tx,self)
        self.c69_cache_identity = c56.digest((expected,sorted(proofs.items())))
        self.c69_siblings_used.add((kind,target,architectural))
        self.c69_preflight_eligible.add(rid)
        self.c69_edges_preflight_eligible.update(p['c69_edge_identity'] for p in proofs.values())
        proof = proofs.get((kind,target,architectural))
        if proof is None:
            raise c58.EvidenceError('unmatched first c69 successor')
        return proof


class Transaction(c69.Transaction):
    def __init__(self,view,root,engine,limits):
        super().__init__(view,root,engine,limits)
        self.post_c69,self.post69_origins,self.c69_crossings = set(),{},[]
        self._entering_post_c69 = False

    def _transaction_terminal(self,error):
        expected = self.e.c70_certificates.lookup(self.root)
        if expected is None:
            raise c58.EvidenceError('transaction c68 POST root missing c69 record') from error
        if expected.get('terminal_event_edge_backed',True) is not False:
            raise c58.EvidenceError('edge-backed accepted c69 terminal appeared as transaction event') from error
        current = c69.record_for(self.e.c70_certificates.baseline,self,self.e)
        if c56.encoded(current)!=c56.encoded(expected):
            self.e.c69_mismatches['transaction_c69_record_mismatch'] += 1
            raise c58.EvidenceError('stale/wrong transaction c69 terminal provenance') from error
        self.e.c69_considered.add(expected['record_identity'])
        self.e.c69_refusals['transaction_terminal'] += 1
        raise c58.Refusal(TRANSACTION_REFUSAL) from error

    def mark_post_c69(self,key,path=None):
        descendants = self.closure(self.forward,key);before = set(self.post_c69);self.post_c69.update(descendants)
        guarded = (self.e.used_boundary_contexts|self.e.c61_source_keys|self.e.c63_source_keys|
            self.e.c65_source_keys|self.e.c67_source_keys|self.e.c69_source_keys)
        if descendants&guarded:
            raise c58.Refusal(POST)
        added = self.post_c69-before
        if not added and key in self.post69_origins:
            return
        if path is None:
            path = self.contexts.get(key,(None,()))[1]
        origin = dict(kind='authenticated_c69_arrival',entry_context=key,
            c69_record_identities=_path_c69_ids(path),propagation_contexts=[key])
        if not origin['c69_record_identities']:
            raise c58.EvidenceError('post-c69 arrival lacks c69 proof')
        for target in sorted(added):
            self.post69_origins[target] = deepcopy(origin)
        self.post69_origins.setdefault(key,origin)

    def instance(self,row,state,path):
        later69 = any(p.get('proof_kind')=='c69_post_boundary' for p in path)
        if later69:
            old = self._entering_post_c69;self._entering_post_c69 = True
            try:
                try:
                    key = c68.Transaction.instance(self,row,state,path)
                except c58.Refusal as error:
                    if str(error)==c68.POST:
                        raise c58.Refusal(POST) from error
                    raise
            finally:
                self._entering_post_c69 = old
            self.mark_post_c69(key,path);return key
        try:
            return super().instance(row,state,path)
        except c58.Refusal as error:
            if str(error)==c68.POST:
                return self._transaction_terminal(error)
            raise

    def connect(self,source,target):
        if source in self.post_c69:
            old = self._entering_post_c69;self._entering_post_c69 = True
            try:
                try:
                    c68.Transaction.connect(self,source,target)
                except c58.Refusal as error:
                    if str(error)==c68.POST:
                        raise c58.Refusal(POST) from error
                    raise
            finally:
                self._entering_post_c69 = old
            path = self.contexts.get(target,(None,()))[1];self.mark_post_c69(target,path);return
        try:
            return super().connect(source,target)
        except c58.Refusal as error:
            if str(error)==c68.POST:
                return self._transaction_terminal(error)
            raise

    def scan_row(self,key):
        try:
            outgoing = super().scan_row(key)
        except c58.Refusal as error:
            if str(error)==c68.POST and key in self.post_c69:
                raise c58.Refusal(POST) from error
            raise
        self.c69_crossings.extend((proof,sum(f is not None for f in state))
            for proof,_,state in outgoing if proof.get('proof_kind')=='c69_post_boundary')
        return outgoing


def scan(certificates,limits=c58.Limits(),progress=None):
    if any(type(n)!=int or n<=0 for n in asdict(limits).values()):
        raise ValueError('all c70 caps must be positive integers')
    certificates.assert_current();e = c69.evidence(certificates.baseline.certificates)
    engine = shard = None;audit = c59.ReplayAudit()
    considered,preflight,admitted = set(),set(),set()
    edges_considered,edges_preflight,edges_admitted = set(),set(),set()
    refusals,mismatches,refs = Counter(),Counter(),[]
    crossings = facts = 0
    by_id = {record['record_identity']:record for record in certificates.records}
    for number,root in enumerate(e.proofs,1):
        current = root // e.session.profile.rows_per_shard
        if current!=shard:
            engine = c58.c55.Engine(e.session.domain,e.session.profile.summary);shard = current
        view = View(certificates);tx = Transaction(view,root,engine,limits);reason = None
        try:
            tx.run()
        except c58.EvidenceError:
            raise
        except (c58.Refusal,ValueError) as error:
            reason = str(error)
        audit.add(tx,reason)
        considered.update(view.c69_considered);preflight.update(view.c69_preflight_eligible)
        edges_considered.update(view.c69_edges_considered);edges_preflight.update(view.c69_edges_preflight_eligible)
        refusals.update(view.c69_refusals);mismatches.update(view.c69_mismatches)
        if reason is None:
            admitted.update(view.c69_preflight_eligible);edges_admitted.update(view.c69_edges_preflight_eligible)
            crossings += len(tx.c69_crossings);facts += sum(n for _,n in tx.c69_crossings)
            for original in tx.references:
                ref = deepcopy(original);path = ref['boundary_path']
                if view.c69_preflight_eligible:
                    ref.update(c70_authentication_identity=certificates.identity,
                        c70_entry_instance_identity=c56.digest((certificates.identity,tx.root,
                            ref['continuation_runtime_row'],ref['incoming_state_identity'],c56.digest(path))),
                        path_c69_record_identities=_path_c69_ids(path),
                        root_c69_record_identities=sorted(view.c69_preflight_eligible))
                if len(refs)<limits.retained_references:
                    refs.append(ref)
        if progress and (number%1024==0 or number==len(e.proofs)):
            progress('c70 full-replay c69 continuation',number,len(e.proofs))
    certificates.assert_current()
    expected_ids = set(certificates.index_record_ids)
    if considered!=expected_ids:
        raise c58.EvidenceError('missing/unexpected accepted c69 terminal domain during c70 replay')
    if preflight!=set(certificates.all_exact_record_ids) or edges_considered!=set(certificates.continuable_edge_ids):
        raise c58.EvidenceError('c70 c69 all-successor preflight domain mismatch')
    if mismatches:
        raise c58.EvidenceError('c70 c69 record mismatch counts nonempty')
    t,ops = audit.totals,audit.ops;count = sum(ops.values())
    grouped = lambda ids,field:dict(sorted(Counter(by_id[i][field] for i in ids).items()))
    result = dict(repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
        evidence='OFFLINE_STATIC_FULL_REPLAY_ENTRY_INSTANCE_CANDIDATES_ONLY',image_authenticated=True,
        image_sha256=certificates.header['image_sha256'],authentication_header=certificates.header,
        **{k:v for k,v in certificates.header.items() if k.startswith('accepted_')},limits=asdict(limits),
        full_historical_replay_used=True,checkpoint_accelerated_resume_used=False,
        c69_boundary_instances_available=len(certificates.records),
        c69_all_successor_exact_instances_available=len(certificates.all_exact_record_ids),
        c69_boundary_instances_considered=len(considered),c69_boundary_instances_preflight_eligible=len(preflight),
        c69_boundary_instances_admitted=len(admitted),c69_boundary_instances_refused=len(considered-admitted),
        c69_continuable_exact_edges_available=len(certificates.continuable_edge_ids),
        c69_continuable_exact_edges_considered=len(edges_considered),
        c69_continuable_exact_edges_preflight_eligible=len(edges_preflight),
        c69_continuable_exact_edges_admitted=len(edges_admitted),
        c69_continuable_exact_edges_refused=len(edges_considered-edges_admitted),
        c69_preflight_refusal_counts=dict(sorted(refusals.items())),
        c69_record_mismatch_counts=dict(sorted(mismatches.items())),
        c69_terminal_phase_considered=grouped(considered,'terminal_phase'),
        c69_terminal_phase_admitted=grouped(admitted,'terminal_phase'),
        c69_terminal_reason_considered=grouped(considered,'terminal_reason'),
        c69_terminal_reason_admitted=grouped(admitted,'terminal_reason'),
        c69_certified_crossings_with_facts=crossings,c69_address_facts_crossing_rows=facts,
        root_transactions_attempted=t['root_transactions_attempted'],root_transactions_completed=t['roots_completed'],
        roots_completed=t['roots_completed'],root_transactions_refused=sum(audit.reasons.values()),
        root_refusal_reasons=dict(sorted(audit.reasons.items())),
        post_c69_uncertified_boundary_refusals=audit.reasons[POST],
        c69_transaction_terminal_refusals=audit.reasons[TRANSACTION_REFUSAL],
        rows_reached_by_continuation=len(audit.rows),
        remaining_boundary_refusal_relation_counts={k:audit.relations[k] for k in c57.RELATIONS},
        reference_candidate_count=count,new_cross_row_candidate_count=count,
        reader_candidates=ops['READ'],writer_candidates=ops['WRITE'],rmw_candidates=ops['RMW'],
        address_only_candidates=ops['ADDRESS_ONLY'],reference_candidates=refs,
        candidate_records_not_retained=count-len(refs),
        historical_c69_candidate_counts={k:certificates.result[k] for k in ('reference_candidate_count',
            'new_cross_row_candidate_count','reader_candidates','writer_candidates','rmw_candidates','address_only_candidates')},
        c56_through_c69_evidence_read_only=True,publication_source_kind='unproven',live_qword_reads=0,
        c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
        summary_scope='full_historical_root_transaction_preserved_c69_first_terminal_authorization_only',
        **{k:False for k in c56.FALSE_GATES})
    for k in ('continuation_instances_created','continuation_instances_deduplicated','continuation_instances_refused',
        'cross_row_cap_refusals','cross_row_cycles_seen','address_facts_crossing_runtime_rows',
        'call_edges_with_preserved_facts_crossing_rows','no_carried_fact_edges'):
        result[k] = t[k]
    return result


# Filled after ProofPool construction to avoid maintaining a second mutable mapping.
def _pool_record_ids(pool):
    return frozenset(record['record_identity'] for record in pool.records)


def analyze(data,cache=None,c57_json=None,c58_json=None,c59_json=None,c60_json=None,c61_json=None,c62_json=None,
        c63_json=None,c64_json=None,c65_json=None,c66_json=None,c67_json=None,c68_json=None,c69_json=None,
        c69_checkpoint=None,progress=None,production=True):
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
    owner68 = c69.read_c68(c68_json) if c68_json is not None else None
    baseline69 = c69.Baseline(cert68,owner_result=owner68,progress=progress)
    if c69_json is None or c69_checkpoint is None:
        raise c58.EvidenceError('c70 requires explicit c69 owner JSON and checkpoint')
    if production:
        owner69 = read_c69(c69_json);checkpoint = read_c69_checkpoint(c69_checkpoint)
    else:
        owner69 = c56.read_json(c69_json) if isinstance(c69_json,(str,Path)) else c69_json
        checkpoint = c69.read_checkpoint(c69_checkpoint) if isinstance(c69_checkpoint,(str,Path)) else c69.validate_checkpoint(c69_checkpoint)
    pool = ProofPool(baseline69,owner69,checkpoint,production=production)
    pool.index_record_ids = _pool_record_ids(pool)
    return scan(pool,progress=progress)


def main():
    parser = argparse.ArgumentParser(description=__doc__);parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    for flag in ('c56-cache','c57-json','c58-json','c59-json','c60-json','c61-json','c62-json','c63-json','c64-json','c65-json','c66-json','c67-json','c68-json','c69-json','c69-checkpoint'):
        parser.add_argument('--'+flag,type=Path)
    a = parser.parse_args()
    try:
        output = a.output.resolve();inputs = [p for p in (a.exe,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,
            a.c62_json,a.c63_json,a.c64_json,a.c65_json,a.c66_json,a.c67_json,a.c68_json,a.c69_json,a.c69_checkpoint) if p is not None]
        if any(output==p.resolve() for p in inputs) or (a.c56_cache and output.is_relative_to(a.c56_cache.resolve())):
            raise c58.EvidenceError('output overlaps immutable input evidence')
        if a.exe.stat().st_size>cfg.old.MAX_FILE:
            raise c58.EvidenceError('input file cap')
        result = analyze(a.exe.read_bytes(),a.c56_cache,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,
            a.c62_json,a.c63_json,a.c64_json,a.c65_json,a.c66_json,a.c67_json,a.c68_json,a.c69_json,a.c69_checkpoint,
            lambda phase,n,total:print(f'{phase}: {n}/{total}',file=sys.stderr),production=True)
        c56.atomic_json(a.output,result)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(f'{BLOCKED}: {error}',file=sys.stderr);return 2
    print(f'{READY}: {a.output}')
    return 0


if __name__=='__main__':
    raise SystemExit(main())
