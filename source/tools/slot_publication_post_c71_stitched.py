"""c72 OFFLINE full-replay continuation through accepted c71 exact boundaries.

The accepted c71 checkpoint is diagnostic evidence, not a mutable transaction
snapshot. C72 therefore rebuilds the accepted c56-c70 history, authenticates the
accepted c71 checkpoint/result/record stream, reproduces each c71 terminal during
the unchanged root replay, and continues only records whose complete architectural
successor set is exact_unique_runtime_begin.

No slot qword read, pointer chase, native/game probe, production integration or
gameplay write is performed.
"""
import argparse
from collections import Counter
from copy import deepcopy
from dataclasses import asdict
import json
from pathlib import Path
import sys

import slot_publication_post_c69_boundary_fix as c71fix

c71 = c71fix.base
c70 = c71.c70
c69,c68,c67,c66,c65,c64,c63,c62,c61,c60,c59,c58,c57,c56,cfg = (
    c70.c69,c70.c68,c70.c67,c70.c66,c70.c65,c70.c64,c70.c63,c70.c62,
    c70.c61,c70.c60,c70.c59,c70.c58,c70.c57,c70.c56,c70.cfg
)

VERSION = 'v0.1c72'
SCHEMA = 1
CONTRACT = 'exact_c71_full_replay_all_successor_continuation_v1'
READY = 'POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_READY'
BLOCKED = 'POST_C71_RUNTIME_BEGIN_STITCH_SCANNER_BLOCKED'
POST = 'uncertified_post_c71_runtime_boundary'
TRANSACTION_REFUSAL = 'c71_transaction_terminal_not_continuable'

EXPECTED_SHA = c71.EXPECTED_SHA
EXPECTED_C71_CHECKPOINT_FILE_SHA = '25b4edccb0430bc2119c6ff5aa4aa408b92c806875c4a520ddd36c0ce69ee926'
EXPECTED_C71_CHECKPOINT_IDENTITY = '4769ecb7664e556f8771158218a7526134620c2774bfc19200f0e9b56ab54a9e'
EXPECTED_C71_RESULT_IDENTITY = '18d61be59032514a4c74c9b015f422c379e2c05050e2d5819c1025048a2276d7'
EXPECTED_C71_CLASSIFIER_IDENTITY = '18cba768c4261886b70496e7f73abe3fe5276b44aecadb0d929802ae3dae9fd5'
EXPECTED_C71_RECORD_SET = '6289d892477caa279ac9da0559ec9a3640ea501bb9850d0113538ccbb099c1b8'
EXPECTED_RECORDS = 102
EXPECTED_EDGE_BACKED = 102
EXPECTED_TRANSACTION = 0
EXPECTED_ALL_EXACT = 95
EXPECTED_CONTINUABLE_EXACT_EDGES = 99
EXPECTED_INTERIOR_INSTANCES = 6
EXPECTED_OTHER_RELATION_INSTANCES = 1


def sources():
    return dict(c71.sources(),slot_publication_post_c71_stitched_py=c56.sha(Path(__file__).read_bytes()))


def read_c71_checkpoint(path):
    path = Path(path)
    raw = path.read_bytes()
    if len(raw)>c56.MAX_JSON or c56.sha(raw)!=EXPECTED_C71_CHECKPOINT_FILE_SHA:
        raise c58.EvidenceError('stale/wrong physical c71 checkpoint JSON')
    value = c56.read_json(path)
    value = c71.validate_checkpoint(value)
    header = value.get('header',{})
    result = value.get('result',{})
    if (value.get('checkpoint_identity')!=EXPECTED_C71_CHECKPOINT_IDENTITY
        or header.get('c71_result_identity')!=EXPECTED_C71_RESULT_IDENTITY
        or header.get('c71_classifier_identity')!=EXPECTED_C71_CLASSIFIER_IDENTITY
        or header.get('record_stream_sha256')!=EXPECTED_C71_RECORD_SET
        or c56.digest(result)!=EXPECTED_C71_RESULT_IDENTITY):
        raise c58.EvidenceError('wrong accepted production c71 checkpoint identities')
    return value


def _all_exact(record):
    edges = record.get('edges')
    return (record.get('terminal_event_edge_backed') is True
        and type(edges)==list and bool(edges)
        and all(edge.get('architectural_successor') is True
            and edge.get('target_metadata_relation')=='exact_unique_runtime_begin'
            and edge.get('source_target_spans_overlap') is False
            for edge in edges))


def _path_c71_ids(path):
    return [p['c71_record_identity'] for p in path if p.get('proof_kind')=='c71_post_boundary']


class ProofPool:
    """Accepted c71 diagnostic records over the exact live c71 baseline."""
    def __init__(self,baseline,checkpoint,*,production=True):
        baseline.assert_current()
        checkpoint = c71.validate_checkpoint(deepcopy(checkpoint))
        header = checkpoint.get('header',{})
        result = checkpoint.get('result',{})
        records = checkpoint.get('records')
        if type(records)!=list:
            raise c58.EvidenceError('missing c71 checkpoint records')
        if (result.get('repository_result')!=c71.READY
            or result.get('tool_version')!=c71.VERSION
            or result.get('schema_version')!=c71.SCHEMA
            or result.get('contract')!=c71.CONTRACT
            or result.get('classification_complete') is not True
            or result.get('terminal_origin_classification_complete') is not True
            or result.get('full_historical_replay_used') is not True
            or result.get('checkpoint_accelerated_resume_used') is not False
            or result.get('new_post_c69_boundaries_admitted')!=0
            or result.get('new_cross_row_candidate_count')!=0
            or result.get('live_qword_reads')!=0
            or result.get('c56_through_c70_evidence_read_only') is not True
            or any(result.get(k) for k in c56.FALSE_GATES)):
            raise c58.EvidenceError('wrong/incomplete c71 owner proof contract')
        if (result.get('classifier_header')!=baseline.header
            or result.get('classifier_identity')!=baseline.identity
            or header.get('c71_classifier_identity')!=baseline.identity
            or header.get('c71_result_identity')!=c56.digest(result)
            or header.get('record_stream_sha256')!=result.get('authenticated_record_set_sha256')
            or header.get('record_count')!=len(records)):
            raise c58.EvidenceError('c71 checkpoint/live baseline identity mismatch')

        index = {}
        for record in records:
            if (type(record)!=dict
                or c56.digest({k:v for k,v in record.items() if k!='record_identity'})!=record.get('record_identity')):
                raise c58.EvidenceError('bad c71 checkpoint record identity')
            root = record.get('original_root_runtime_row')
            if type(root)!=int or root in index:
                raise c58.EvidenceError('duplicate/invalid c71 original root')
            index[root] = record

        edge_backed = sum(record.get('terminal_event_edge_backed') is True for record in records)
        transaction = len(records)-edge_backed
        all_exact = [record for record in records if _all_exact(record)]
        continuable_edges = sum(len(record['edges']) for record in all_exact)
        interior = sum(any(edge.get('target_metadata_relation')=='inside_unique_runtime_span_nonentry'
            for edge in record.get('edges',())) for record in records)
        other = sum(record.get('terminal_event_edge_backed') is True
            and not _all_exact(record)
            and not any(edge.get('target_metadata_relation')=='inside_unique_runtime_span_nonentry'
                for edge in record.get('edges',()))
            for record in records)
        if production and (
            checkpoint.get('checkpoint_identity')!=EXPECTED_C71_CHECKPOINT_IDENTITY
            or c56.digest(result)!=EXPECTED_C71_RESULT_IDENTITY
            or result.get('classifier_identity')!=EXPECTED_C71_CLASSIFIER_IDENTITY
            or result.get('authenticated_record_set_sha256')!=EXPECTED_C71_RECORD_SET
            or len(records)!=EXPECTED_RECORDS
            or edge_backed!=EXPECTED_EDGE_BACKED
            or transaction!=EXPECTED_TRANSACTION
            or len(all_exact)!=EXPECTED_ALL_EXACT
            or continuable_edges!=EXPECTED_CONTINUABLE_EXACT_EDGES
            or interior!=EXPECTED_INTERIOR_INSTANCES
            or other!=EXPECTED_OTHER_RELATION_INSTANCES):
            raise c58.EvidenceError('accepted c71 continuation-domain count/identity mismatch')

        self.baseline = baseline
        self.checkpoint = deepcopy(checkpoint)
        self.result = deepcopy(result)
        self.records = tuple(deepcopy(records))
        self.index = index
        self.index_record_ids = frozenset(record['record_identity'] for record in records)
        self.all_exact_record_ids = frozenset(record['record_identity'] for record in all_exact)
        self.continuable_edge_ids = frozenset(c56.digest((record['record_identity'],edge))
            for record in all_exact for edge in record['edges'])
        self.live_records = []
        self.live_roots = set()
        self.header = dict(
            tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
            image_sha256=baseline.header['image_sha256'],
            c56_manifest_id=baseline.header['c56_manifest_id'],
            **{k:v for k,v in baseline.header.items() if k.startswith('accepted_')},
            accepted_c71_checkpoint_identity=checkpoint['checkpoint_identity'],
            accepted_c71_checkpoint_physical_sha256=EXPECTED_C71_CHECKPOINT_FILE_SHA if production else None,
            accepted_c71_result_identity=c56.digest(result),
            accepted_c71_classifier_identity=result['classifier_identity'],
            accepted_c71_record_set_identity=result['authenticated_record_set_sha256'],
            c71_contract=c71.CONTRACT,
            c71_checkpoint_contract=header.get('checkpoint_contract'),
            limits=asdict(c58.Limits()),tool_sources=sources(),
            full_historical_replay_used=True,checkpoint_accelerated_resume_used=False)
        self.identity = c56.digest(self.header)

        # c71 record construction resolves this baseline through the c70 pool.
        baseline.certificates.c71_baseline = baseline
        baseline.certificates.c71_records = []

    def assert_current(self):
        self.baseline.assert_current()
        if (self.header.get('tool_sources')!=sources()
            or c56.digest(self.header)!=self.identity
            or self.checkpoint.get('checkpoint_identity')!=self.header['accepted_c71_checkpoint_identity']
            or c56.digest(self.result)!=self.header['accepted_c71_result_identity']
            or self.result.get('classifier_identity')!=self.header['accepted_c71_classifier_identity']
            or self.result.get('authenticated_record_set_sha256')!=self.header['accepted_c71_record_set_identity']
            or self.result.get('classifier_header')!=self.baseline.header):
            raise c58.EvidenceError('stale/mutated c72 c71 proof pool')

    def lookup(self,root):
        return self.index.get(root)

    def authenticate_live(self,record):
        root = record.get('original_root_runtime_row')
        expected = self.lookup(root)
        if expected is None:
            raise c58.EvidenceError('live c71 terminal root missing accepted checkpoint record')
        if root in self.live_roots:
            raise c58.EvidenceError('duplicate live c71 terminal root')
        if c56.encoded(record)!=c56.encoded(expected):
            raise c58.EvidenceError('stale/wrong c71 root/state/path/source/phase/reason/provenance')
        self.live_roots.add(root)
        self.live_records.append(deepcopy(record))
        return expected


class View(c71.View):
    def __init__(self,certificates):
        super().__init__(certificates.baseline.certificates)
        self.c72_certificates = certificates
        self.c71_considered,self.c71_preflight_eligible = set(),set()
        self.c71_edges_considered,self.c71_edges_preflight_eligible = set(),set()
        self.c71_source_keys,self.c71_proofs = set(),{}
        self.c71_record,self.c71_key,self.c71_scan_token,self.c71_prefix = None,None,None,None
        self.c71_siblings_used,self.c71_cache_identity = set(),None
        self.c71_refusals,self.c71_mismatches = Counter(),Counter()

    def _post71_boundary(self,index,ins,kind,target,architectural,relations):
        begin,end,_ = self.session.rows[index]
        relations[c57.target_relation(self.session,begin,end,target)['target_metadata_relation']] += 1
        raise c58.Refusal(POST)

    def _post69_boundary(self,index,ins,kind,target,architectural,relations):
        tx,key = self.tx,self.key
        if tx is None or key is None or key[0]!=index:
            raise c58.EvidenceError('missing current c72 source context')

        # Complete sibling set of the single authenticated c71 source instruction.
        if (self.c71_record is not None and key==self.c71_key and tx.scan_token==self.c71_scan_token
            and ins.address==self.c71_record['source_instruction_rva']):
            if (c68.sibling_prefix(tx,self)!=self.c71_prefix
                or c56.digest((self.c71_record,sorted(self.c71_proofs.items())))!=self.c71_cache_identity):
                raise c58.Refusal('c71_sibling_context_or_provenance_conflict')
            sibling=(kind,target,architectural)
            if sibling in self.c71_siblings_used:
                raise c58.Refusal('duplicate_c71_sibling_callback')
            self.c71_siblings_used.add(sibling)
            proof=self.c71_proofs.get(sibling)
            if proof is None:
                raise c58.EvidenceError('unmatched exact c71 sibling successor')
            return proof

        # After c71 authorization, any later distinct c70 POST is the next unknown.
        if key in getattr(tx,'post_c71',set()) or key in self.c71_source_keys:
            return self._post71_boundary(index,ins,kind,target,architectural,relations)

        # Reproduce the exact accepted c71 diagnostic record at this first terminal.
        self.c71_capture = None
        c71._capture_edge(tx,self,index,ins,kind,target,architectural)
        current = self.c71_capture
        if current is None:
            raise c58.EvidenceError('c72 failed to reproduce c71 edge terminal record')
        expected = self.c72_certificates.authenticate_live(current)
        if expected.get('terminal_event_edge_backed') is not True:
            raise c58.EvidenceError('accepted transaction c71 terminal appeared as edge callback')
        rid=expected['record_identity'];self.c71_considered.add(rid)
        edges=expected['edges']
        if not _all_exact(expected):
            for edge in edges:
                relation=edge.get('target_metadata_relation','invalid')
                if relation!='exact_unique_runtime_begin':
                    self.c71_refusals[relation]+=1
            raise c58.Refusal('c71_preflight_nonexact_successor')

        for edge in edges:
            self.c71_edges_considered.add(c56.digest((rid,edge)))
        begin,end,code,_ = self.row(index)
        proofs={}
        for edge in edges:
            if edge.get('source_target_spans_overlap'):
                self.c71_refusals['overlapping_source_target_spans']+=1
                raise c58.Refusal('c71_preflight_overlapping_source_target_spans')
            dest=edge.get('target_runtime_row_index')
            if type(dest)!=int:
                raise c58.EvidenceError('exact c71 edge lacks target runtime row')
            try:
                low,high,raw,_=self.row(dest)
            except c58.Refusal as target_error:
                self.c71_refusals['target_code_preflight_refused']+=1
                raise c58.Refusal('c71_preflight_target_code_refused') from target_error
            if ((low,high)!=(edge.get('target_begin_rva'),edge.get('target_end_rva'))
                or low!=edge.get('edge_target_rva')):
                raise c58.EvidenceError('wrong exact c71 target metadata/bounds')
            proof=dict(
                image_sha256=self.c72_certificates.header['image_sha256'],
                accepted_c71_checkpoint_identity=self.c72_certificates.header['accepted_c71_checkpoint_identity'],
                accepted_c71_result_identity=self.c72_certificates.header['accepted_c71_result_identity'],
                accepted_c71_classifier_identity=self.c72_certificates.header['accepted_c71_classifier_identity'],
                accepted_c71_record_set_identity=self.c72_certificates.header['accepted_c71_record_set_identity'],
                source_runtime_row=index,source_row_sha256=expected['source_runtime_row_sha256'],
                source_code_sha256=c56.sha(code),source_instruction_rva=ins.address,
                source_instruction_size=ins.size,source_instruction_sha256=c56.sha(bytes(ins.bytes)),
                edge_kind=edge['edge_kind'],architectural_successor=True,
                target_metadata_relation='exact_unique_runtime_begin',target_runtime_row=dest,
                target_begin_rva=low,target_end_rva=high,target_row_sha256=self.session.row_identity(dest),
                target_code_sha256=c56.sha(raw),proof_kind='c71_post_boundary',
                c71_record_identity=rid,c71_edge_identity=c56.digest((rid,edge)),
                c71_classifier_identity=expected['classifier_identity'],
                c71_result_identity=self.c72_certificates.header['accepted_c71_result_identity'],
                c71_full_prefix_identity=expected['root_replay_identity'],
                source_entry_instance_identity=expected['entry_instance_identity'],
                source_incoming_state_identity=expected['incoming_state_identity'],
                source_boundary_path_identity=expected['boundary_path_identity'],
                terminal_phase=expected['terminal_phase'],terminal_reason=expected['terminal_reason'],
                terminal_provenance=deepcopy(expected['terminal_provenance']),
                active_c69_authorization=deepcopy(expected.get('active_c69_authorization')),
                current_c70_continuation_identity=expected.get('current_c70_continuation_identity'))
            proof['edge_identity']=c56.digest(proof)
            proofs[(edge['edge_kind'],edge['edge_target_rva'],edge['architectural_successor'])]=proof
        if len(proofs)!=len(edges):
            raise c58.EvidenceError('incomplete c71 all-successor preflight')

        self.c71_record,self.c71_key,self.c71_scan_token=expected,key,tx.scan_token
        self.c71_proofs=proofs;self.c71_source_keys.add(key)
        self.c71_prefix=c68.sibling_prefix(tx,self)
        self.c71_cache_identity=c56.digest((expected,sorted(proofs.items())))
        self.c71_siblings_used.add((kind,target,architectural))
        self.c71_preflight_eligible.add(rid)
        self.c71_edges_preflight_eligible.update(p['c71_edge_identity'] for p in proofs.values())
        proof=proofs.get((kind,target,architectural))
        if proof is None:
            raise c58.EvidenceError('unmatched first c71 successor')
        return proof


class Transaction(c71.Transaction):
    def __init__(self,view,root,engine,limits):
        super().__init__(view,root,engine,limits)
        self.post_c71,self.post71_origins,self.c71_crossings=set(),{},[]
        self._entering_post_c71=False

    def _authenticate_transaction_terminal(self,error):
        capture=self.e.c71_capture
        if capture is None:
            raise c58.EvidenceError('c72 transaction c70 POST lacks c71 capture') from error
        expected=self.e.c72_certificates.authenticate_live(capture)
        if expected.get('terminal_event_edge_backed') is not False:
            raise c58.EvidenceError('edge-backed accepted c71 terminal appeared as transaction event') from error
        self.e.c71_considered.add(expected['record_identity'])
        self.e.c71_refusals['transaction_terminal']+=1
        raise c58.Refusal(TRANSACTION_REFUSAL) from error

    def mark_post_c71(self,key,path=None):
        descendants=self.closure(self.forward,key);before=set(self.post_c71);self.post_c71.update(descendants)
        guarded=(self.e.used_boundary_contexts|self.e.c61_source_keys|self.e.c63_source_keys|
            self.e.c65_source_keys|self.e.c67_source_keys|self.e.c69_source_keys|self.e.c71_source_keys)
        if descendants&guarded:
            raise c58.Refusal(POST)
        added=self.post_c71-before
        if not added and key in self.post71_origins:
            return
        if path is None:
            path=self.contexts.get(key,(None,()))[1]
        ids=_path_c71_ids(path)
        if not ids:
            raise c58.EvidenceError('post-c71 arrival lacks c71 proof')
        origin=dict(kind='authenticated_c71_arrival',entry_context=key,
            c71_record_identities=ids,propagation_contexts=[key])
        for target in sorted(added):
            self.post71_origins[target]=deepcopy(origin)
        self.post71_origins.setdefault(key,origin)

    def instance(self,row,state,path):
        later71=any(p.get('proof_kind')=='c71_post_boundary' for p in path)
        if later71:
            old_entering69=self._entering_post_c69
            old_entering71=self._entering_post_c71
            old_arrival=self.post67_arrival
            self._entering_post_c69=True;self._entering_post_c71=True
            self.post67_arrival=((row,c58.state_id(state)),path)
            try:
                try:
                    key=c68.Transaction.instance(self,row,state,path)
                except c58.Refusal as error:
                    if str(error)==c68.POST:
                        raise c58.Refusal(POST) from error
                    raise
            finally:
                self.post67_arrival=old_arrival
                self._entering_post_c69=old_entering69;self._entering_post_c71=old_entering71
            try:
                self.mark_post_c69(key,path)
            except c58.Refusal as error:
                if str(error)==c70.POST:
                    raise c58.Refusal(POST) from error
                raise
            self.mark_post_c71(key,path)
            return key
        try:
            return super().instance(row,state,path)
        except c58.Refusal as error:
            if str(error)==c70.POST and self.e.c71_capture is not None:
                return self._authenticate_transaction_terminal(error)
            raise

    def connect(self,source,target):
        if source in self.post_c71:
            old_entering69=self._entering_post_c69
            old_entering71=self._entering_post_c71
            old_connect=self.post67_connect
            self._entering_post_c69=True;self._entering_post_c71=True
            self.post67_connect=(source,target)
            try:
                try:
                    c68.Transaction.connect(self,source,target)
                except c58.Refusal as error:
                    if str(error)==c68.POST:
                        raise c58.Refusal(POST) from error
                    raise
            finally:
                self.post67_connect=old_connect
                self._entering_post_c69=old_entering69;self._entering_post_c71=old_entering71
            path=self.contexts.get(target,(None,()))[1]
            try:
                self.mark_post_c69(target,path)
            except c58.Refusal as error:
                if str(error)==c70.POST:
                    raise c58.Refusal(POST) from error
                raise
            self.mark_post_c71(target,path)
            return
        try:
            return super().connect(source,target)
        except c58.Refusal as error:
            if str(error)==c70.POST and self.e.c71_capture is not None:
                return self._authenticate_transaction_terminal(error)
            raise

    def scan_row(self,key):
        try:
            outgoing=super().scan_row(key)
        except c58.Refusal as error:
            if str(error)==c70.POST and key in self.post_c71:
                raise c58.Refusal(POST) from error
            if str(error)==c70.POST and self.e.c71_capture is not None:
                return self._authenticate_transaction_terminal(error)
            raise
        self.c71_crossings.extend((proof,sum(f is not None for f in state))
            for proof,_,state in outgoing if proof.get('proof_kind')=='c71_post_boundary')
        return outgoing


def scan(certificates,limits=c58.Limits(),progress=None):
    if any(type(n)!=int or n<=0 for n in asdict(limits).values()):
        raise ValueError('all c72 caps must be positive integers')
    certificates.assert_current()
    pool70=certificates.baseline.certificates
    e=c69.evidence(pool70.baseline.certificates)
    engine=shard=None;audit=c59.ReplayAudit()
    considered,preflight,admitted=set(),set(),set()
    edges_considered,edges_preflight,edges_admitted=set(),set(),set()
    refusals,mismatches=Counter(),Counter();refs=[]
    crossings=facts=0
    by_id={record['record_identity']:record for record in certificates.records}

    for number,root in enumerate(e.proofs,1):
        current=root//e.session.profile.rows_per_shard
        if current!=shard:
            engine=c58.c55.Engine(e.session.domain,e.session.profile.summary);shard=current
        view=View(certificates);tx=Transaction(view,root,engine,limits);reason=None
        try:
            tx.run()
        except c58.EvidenceError:
            raise
        except (c58.Refusal,ValueError) as error:
            reason=str(error)
        audit.add(tx,reason)
        considered.update(view.c71_considered);preflight.update(view.c71_preflight_eligible)
        edges_considered.update(view.c71_edges_considered);edges_preflight.update(view.c71_edges_preflight_eligible)
        refusals.update(view.c71_refusals);mismatches.update(view.c71_mismatches)
        if reason is None:
            admitted.update(view.c71_preflight_eligible);edges_admitted.update(view.c71_edges_preflight_eligible)
            crossings+=len(tx.c71_crossings);facts+=sum(n for _,n in tx.c71_crossings)
            for original in tx.references:
                ref=deepcopy(original);path=ref['boundary_path']
                if view.c71_preflight_eligible:
                    ref.update(c72_authentication_identity=certificates.identity,
                        c72_entry_instance_identity=c56.digest((certificates.identity,tx.root,
                            ref['continuation_runtime_row'],ref['incoming_state_identity'],c56.digest(path))),
                        path_c71_record_identities=_path_c71_ids(path),
                        root_c71_record_identities=sorted(view.c71_preflight_eligible))
                if len(refs)<limits.retained_references:
                    refs.append(ref)
        if progress and (number%1024==0 or number==len(e.proofs)):
            progress('c72 full-replay c71 continuation',number,len(e.proofs))

    certificates.assert_current()
    expected_ids=set(certificates.index_record_ids)
    if considered!=expected_ids:
        raise c58.EvidenceError('missing/unexpected accepted c71 terminal domain during c72 replay')
    if preflight!=set(certificates.all_exact_record_ids) or edges_considered!=set(certificates.continuable_edge_ids):
        raise c58.EvidenceError('c72 c71 all-successor preflight domain mismatch')
    if mismatches:
        raise c58.EvidenceError('c72 c71 record mismatch counts nonempty')
    if certificates.live_records!=list(certificates.records):
        raise c58.EvidenceError('c72 live c71 record stream/order differs from accepted checkpoint')
    reproduced=c71.Aggregate(certificates.baseline,certificates.live_records).finish()
    if c56.encoded(reproduced)!=c56.encoded(certificates.result):
        raise c58.EvidenceError('c72 live replay did not reproduce accepted c71 result')

    t,ops=audit.totals,audit.ops;count=sum(ops.values())
    grouped=lambda ids,field:dict(sorted(Counter(by_id[i][field] for i in ids).items()))
    result=dict(
        repository_result=READY,tool_version=VERSION,schema_version=SCHEMA,contract=CONTRACT,
        evidence='OFFLINE_STATIC_FULL_REPLAY_ENTRY_INSTANCE_CANDIDATES_ONLY',
        image_authenticated=True,image_sha256=certificates.header['image_sha256'],
        authentication_header=certificates.header,
        **{k:v for k,v in certificates.header.items() if k.startswith('accepted_')},
        limits=asdict(limits),full_historical_replay_used=True,checkpoint_accelerated_resume_used=False,
        c71_boundary_instances_available=len(certificates.records),
        c71_all_successor_exact_instances_available=len(certificates.all_exact_record_ids),
        c71_boundary_instances_considered=len(considered),
        c71_boundary_instances_preflight_eligible=len(preflight),
        c71_boundary_instances_admitted=len(admitted),
        c71_boundary_instances_refused=len(considered-admitted),
        c71_continuable_exact_edges_available=len(certificates.continuable_edge_ids),
        c71_continuable_exact_edges_considered=len(edges_considered),
        c71_continuable_exact_edges_preflight_eligible=len(edges_preflight),
        c71_continuable_exact_edges_admitted=len(edges_admitted),
        c71_continuable_exact_edges_refused=len(edges_considered-edges_admitted),
        c71_preflight_refusal_counts=dict(sorted(refusals.items())),
        c71_record_mismatch_counts=dict(sorted(mismatches.items())),
        c71_terminal_phase_considered=grouped(considered,'terminal_phase'),
        c71_terminal_phase_admitted=grouped(admitted,'terminal_phase'),
        c71_terminal_reason_considered=grouped(considered,'terminal_reason'),
        c71_terminal_reason_admitted=grouped(admitted,'terminal_reason'),
        c71_certified_crossings_with_facts=crossings,c71_address_facts_crossing_rows=facts,
        root_transactions_attempted=t['root_transactions_attempted'],
        root_transactions_completed=t['roots_completed'],roots_completed=t['roots_completed'],
        root_transactions_refused=sum(audit.reasons.values()),
        root_refusal_reasons=dict(sorted(audit.reasons.items())),
        post_c71_uncertified_boundary_refusals=audit.reasons[POST],
        c71_transaction_terminal_refusals=audit.reasons[TRANSACTION_REFUSAL],
        rows_reached_by_continuation=len(audit.rows),
        remaining_boundary_refusal_relation_counts={k:audit.relations[k] for k in c57.RELATIONS},
        reference_candidate_count=count,new_cross_row_candidate_count=count,
        reader_candidates=ops['READ'],writer_candidates=ops['WRITE'],rmw_candidates=ops['RMW'],
        address_only_candidates=ops['ADDRESS_ONLY'],reference_candidates=refs,
        candidate_records_not_retained=count-len(refs),
        historical_c71_candidate_counts=deepcopy(certificates.result['historical_c70_candidate_counts']),
        c56_through_c71_evidence_read_only=True,publication_source_kind='unproven',live_qword_reads=0,
        c52_production_watch_gate='PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL',
        summary_scope='full_historical_root_transaction_preserved_c71_first_terminal_authorization_only',
        **{k:False for k in c56.FALSE_GATES})
    for k in ('continuation_instances_created','continuation_instances_deduplicated','continuation_instances_refused',
        'cross_row_cap_refusals','cross_row_cycles_seen','address_facts_crossing_runtime_rows',
        'call_edges_with_preserved_facts_crossing_rows','no_carried_fact_edges'):
        result[k]=t[k]
    return result


def analyze(data,cache=None,c57_json=None,c58_json=None,c59_json=None,c60_json=None,c61_json=None,c62_json=None,
        c63_json=None,c64_json=None,c65_json=None,c66_json=None,c67_json=None,c68_json=None,c69_json=None,
        c69_checkpoint=None,c70_json=None,c71_checkpoint=None,progress=None,production=True):
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
    if c69_json is None or c69_checkpoint is None or c70_json is None or c71_checkpoint is None:
        raise c58.EvidenceError('c72 requires explicit c69 result/checkpoint, c70 result, and c71 checkpoint')
    if production:
        owner69=c70.read_c69(c69_json);checkpoint69=c70.read_c69_checkpoint(c69_checkpoint)
    else:
        owner69=c56.read_json(c69_json) if isinstance(c69_json,(str,Path)) else c69_json
        checkpoint69=c69.read_checkpoint(c69_checkpoint) if isinstance(c69_checkpoint,(str,Path)) else c69.validate_checkpoint(c69_checkpoint)
    pool70=c70.ProofPool(baseline69,owner69,checkpoint69,production=production)
    pool70.index_record_ids=frozenset(record['record_identity'] for record in pool70.records)
    owner70=c71.read_c70(c70_json) if production else (c56.read_json(c70_json) if isinstance(c70_json,(str,Path)) else c70_json)
    baseline71=c71.Baseline(pool70,owner70,production=production)
    checkpoint71=read_c71_checkpoint(c71_checkpoint) if production else (
        c56.read_json(c71_checkpoint) if isinstance(c71_checkpoint,(str,Path)) else c71.validate_checkpoint(c71_checkpoint))
    pool72=ProofPool(baseline71,checkpoint71,production=production)
    return scan(pool72,progress=progress)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    for flag in ('c56-cache','c57-json','c58-json','c59-json','c60-json','c61-json','c62-json','c63-json',
        'c64-json','c65-json','c66-json','c67-json','c68-json','c69-json','c69-checkpoint','c70-json','c71-checkpoint'):
        parser.add_argument('--'+flag,type=Path)
    a=parser.parse_args()
    try:
        output=a.output.resolve()
        inputs=[p for p in (a.exe,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,a.c62_json,
            a.c63_json,a.c64_json,a.c65_json,a.c66_json,a.c67_json,a.c68_json,a.c69_json,a.c69_checkpoint,
            a.c70_json,a.c71_checkpoint) if p is not None]
        if any(output==p.resolve() for p in inputs) or (a.c56_cache and output.is_relative_to(a.c56_cache.resolve())):
            raise c58.EvidenceError('output overlaps immutable input evidence')
        if a.exe.stat().st_size>cfg.old.MAX_FILE:
            raise c58.EvidenceError('input file cap')
        result=analyze(a.exe.read_bytes(),a.c56_cache,a.c57_json,a.c58_json,a.c59_json,a.c60_json,a.c61_json,
            a.c62_json,a.c63_json,a.c64_json,a.c65_json,a.c66_json,a.c67_json,a.c68_json,a.c69_json,
            a.c69_checkpoint,a.c70_json,a.c71_checkpoint,
            lambda phase,n,total:print(f'{phase}: {n}/{total}',file=sys.stderr),production=True)
        c56.atomic_json(a.output,result)
    except (OSError,ValueError,KeyError,TypeError,IndexError,RecursionError) as error:
        print(f'{BLOCKED}: {error}',file=sys.stderr);return 2
    print(f'{READY}: {a.output}')
    return 0


if __name__=='__main__':
    raise SystemExit(main())
