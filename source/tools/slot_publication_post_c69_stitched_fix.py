"""Correction layer for c70 post-c69 provenance, observer lifetime, and c69 candidate projection.

C70 deliberately calls c68's full-replay instance/connect semantics for a newly
authorized c69 edge. c69's observer layer also tracks the exact post-c67 arrival
or connection that led to a context. Preserve those transient observer fields
while using c68's methods so c69 provenance remains exact instead of being
silently dropped.

After c70 has authenticated the first accepted c69 terminal, c69's inherited
first-terminal observer must no longer see later distinct edges in that root.
Those are the next post-c69 unknown and fail closed as c70 POST. Exact sibling
callbacks for the same accepted c69 source instruction still use the original
sibling-proof matcher so unknown extra siblings remain evidence failures.

C69 is diagnostic-only, so its candidate totals are preserved under
``historical_c68_candidate_counts`` instead of duplicated as c69 top-level
candidate keys. Present those historical counts through a non-serializing dict
view so c70 can report the unchanged inherited candidate totals without mutating
the accepted c69 result bytes or identity.
"""
from pathlib import Path

import slot_publication_post_c69_stitched as base

_OriginalView = base.View
_OriginalTransaction = base.Transaction
_OriginalProofPool = base.ProofPool
_original_sources = base.sources

_CANDIDATE_KEYS = (
    'reference_candidate_count','new_cross_row_candidate_count','reader_candidates',
    'writer_candidates','rmw_candidates','address_only_candidates')


def sources():
    return dict(_original_sources(),slot_publication_post_c69_stitched_fix_py=base.c56.sha(Path(__file__).read_bytes()))


class _C69ResultView(dict):
    """Read-only-compatible projection without changing serialized c69 bytes."""
    def __missing__(self,key):
        if key in _CANDIDATE_KEYS:
            historical = dict.get(self,'historical_c68_candidate_counts')
            if type(historical)==dict and key in historical:
                return historical[key]
        raise KeyError(key)


class ProofPool(_OriginalProofPool):
    def __init__(self,*args,**kwargs):
        super().__init__(*args,**kwargs)
        self.result = _C69ResultView(self.result)
        self.index_record_ids = frozenset(record['record_identity'] for record in self.records)


class View(_OriginalView):
    def edge(self,index,ins,kind,target,architectural,relations):
        # Before the first accepted c69 terminal, retain the exact original c69
        # first-terminal observer. Once c69_record is set, only callbacks for the
        # same authenticated source instruction/scan belong to that terminal's
        # sibling set. Every other later edge is the next c70 unknown and must not
        # be compared against c69's retained first-terminal capture.
        if self.c69_record is not None:
            tx,key = self.tx,self.key
            if tx is None or key is None:
                raise base.c58.EvidenceError('missing c70 post-c69 source context')
            same_authenticated_source = (
                key==self.c69_key
                and tx.scan_token==self.c69_scan_token
                and ins.address==self.c69_record['source_instruction_rva'])
            if not same_authenticated_source:
                return self._post69_boundary(index,ins,kind,target,architectural,relations)
        return super().edge(index,ins,kind,target,architectural,relations)


class Transaction(_OriginalTransaction):
    def instance(self,row,state,path):
        later69 = any(p.get('proof_kind')=='c69_post_boundary' for p in path)
        if not later69:
            return super().instance(row,state,path)
        old_entering = self._entering_post_c69
        old_arrival = self.post67_arrival
        self._entering_post_c69 = True
        self.post67_arrival = ((row,base.c58.state_id(state)),path)
        try:
            try:
                key = base.c68.Transaction.instance(self,row,state,path)
            except base.c58.Refusal as error:
                if str(error)==base.c68.POST:
                    raise base.c58.Refusal(base.POST) from error
                raise
        finally:
            self.post67_arrival = old_arrival
            self._entering_post_c69 = old_entering
        self.mark_post_c69(key,path)
        return key

    def connect(self,source,target):
        if source not in self.post_c69:
            return super().connect(source,target)
        old_entering = self._entering_post_c69
        old_connect = self.post67_connect
        self._entering_post_c69 = True
        self.post67_connect = (source,target)
        try:
            try:
                base.c68.Transaction.connect(self,source,target)
            except base.c58.Refusal as error:
                if str(error)==base.c68.POST:
                    raise base.c58.Refusal(base.POST) from error
                raise
        finally:
            self.post67_connect = old_connect
            self._entering_post_c69 = old_entering
        path = self.contexts.get(target,(None,()))[1]
        self.mark_post_c69(target,path)


base.sources = sources
base.ProofPool = ProofPool
base.View = View
base.Transaction = Transaction


def main():
    return base.main()


if __name__=='__main__':
    raise SystemExit(main())
