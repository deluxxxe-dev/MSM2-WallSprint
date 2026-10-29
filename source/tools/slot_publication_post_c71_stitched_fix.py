"""Lifecycle/provenance/order/representation corrections for c72's c71 continuation.

Accepted c71 records exactly one first c70 POST terminal per root. During c72,
a successful all-successor exact c71 authorization retires that first-terminal
observer for the root. Exact sibling callbacks from the same authenticated
source instruction still use the accepted sibling set, but any later distinct
c70 POST must be the next post-c71 unknown instead of being captured again as a
second c71 first terminal.

A c71-authorized arrival is also not automatically a current post-c69 arrival.
If the live continuation path contains an actual ``c69_post_boundary`` proof,
the historical c70 ``mark_post_c69`` behavior is preserved. If it contains a
``c71_post_boundary`` proof but no c69 proof, c72 must not invent c69
reachability merely to satisfy the older c70 bookkeeping layer; c71
``mark_post_c71`` remains the binding later-stage authorization.

The accepted c71 checkpoint also binds an order-sensitive serialized record
stream. Full c72 replay can encounter the same unique per-root records in a
different incidental traversal order. Each live record is already authenticated
byte-for-byte through ``c56.encoded`` against the accepted record for its
original root, duplicate roots are refused, and the complete 102-record domain
is checked before final result reproduction. Therefore final c71-result
reproduction uses accepted checkpoint root order and the final stream equality
uses the same canonical encoded-record identity as per-root authentication.
This avoids treating incidental Python tuple/list representation differences as
semantic record differences. Observed discovery order remains diagnostic only.
"""
from pathlib import Path

import slot_publication_post_c71_stitched as base
from slot_publication_post_c71_stitched import *  # re-export c72 surface

c56,c58 = base.c56,base.c58
_original_sources = base.sources
_OriginalView = base.View
_OriginalTransaction = base.Transaction
_OriginalProofPool = base.ProofPool
_original_scan = base.scan


def sources():
    return dict(_original_sources(),
        slot_publication_post_c71_stitched_fix_py=c56.sha(Path(__file__).read_bytes()))


class _CheckpointOrderedLiveRecords:
    """Store live arrival order, iterate in accepted checkpoint root order.

    Base ``authenticate_live`` has already proved each appended record has the
    same canonical encoded bytes as the accepted record for that root and rejects
    duplicate roots. This adapter does not weaken record authentication; it only
    separates incidental replay discovery/Python container representation from
    the accepted checkpoint serialization used to reproduce c71's result bytes.
    """
    def __init__(self,accepted_records):
        self._accepted = tuple(accepted_records)
        self._by_root = {}
        self.arrival_roots = []
        self.checkpoint_roots = tuple(
            record['original_root_runtime_row'] for record in self._accepted)

    def append(self,record):
        root = record.get('original_root_runtime_row')
        if root in self._by_root:
            raise c58.EvidenceError('duplicate live c71 terminal root in order adapter')
        self._by_root[root] = record
        self.arrival_roots.append(root)

    def __len__(self):
        return len(self._by_root)

    def __iter__(self):
        for expected in self._accepted:
            root = expected['original_root_runtime_row']
            if root in self._by_root:
                yield self._by_root[root]

    def __getitem__(self,item):
        return list(self)[item]

    def __eq__(self,other):
        try:
            other_records = list(other)
        except TypeError:
            return False
        return ([c56.encoded(record) for record in self]
            == [c56.encoded(record) for record in other_records])

    def discovery_order_matches_checkpoint(self):
        return tuple(self.arrival_roots)==self.checkpoint_roots

    def discovery_order_mismatch_positions(self):
        if len(self.arrival_roots)!=len(self.checkpoint_roots):
            return None
        return sum(a!=b for a,b in zip(self.arrival_roots,self.checkpoint_roots))


class ProofPool(_OriginalProofPool):
    def __init__(self,*args,**kwargs):
        super().__init__(*args,**kwargs)
        self.live_records = _CheckpointOrderedLiveRecords(self.records)


class View(_OriginalView):
    def __init__(self,certificates):
        super().__init__(certificates)
        self.c71_first_terminal_observer_retired = False

    def _post69_boundary(self,index,ins,kind,target,architectural,relations):
        tx,key = self.tx,self.key
        if tx is None or key is None or key[0]!=index:
            raise c58.EvidenceError('missing current c72 source context')

        same_authenticated_source = (
            self.c71_record is not None
            and key==self.c71_key
            and tx.scan_token==self.c71_scan_token
            and ins.address==self.c71_record.get('source_instruction_rva'))

        # Once the accepted first c71 terminal has been authenticated and its
        # continuation started, c71's first-terminal observer is retired. Keep
        # exact sibling callbacks on that same source, but a later distinct c70
        # POST is the next c72 unknown even if its current key is not in
        # tx.post_c71. Do not relabel current reachability.
        if self.c71_first_terminal_observer_retired and not same_authenticated_source:
            return self._post71_boundary(index,ins,kind,target,architectural,relations)

        result = super()._post69_boundary(index,ins,kind,target,architectural,relations)
        if self.c71_record is not None:
            self.c71_first_terminal_observer_retired = True
        return result


class Transaction(_OriginalTransaction):
    def mark_post_c69(self,key,path=None):
        """Do not promote c71-only arrival proof into current c69 reachability."""
        if path is None:
            path = self.contexts.get(key,(None,()))[1]
        has_c71 = any(p.get('proof_kind')=='c71_post_boundary' for p in path)
        has_c69 = any(p.get('proof_kind')=='c69_post_boundary' for p in path)
        if has_c71 and not has_c69:
            return
        return super().mark_post_c69(key,path)


def scan(certificates,limits=base.c58.Limits(),progress=None):
    result = _original_scan(certificates,limits=limits,progress=progress)
    live = certificates.live_records
    if isinstance(live,_CheckpointOrderedLiveRecords):
        result['c71_live_discovery_order_matches_checkpoint'] = live.discovery_order_matches_checkpoint()
        result['c71_live_discovery_order_mismatch_positions'] = live.discovery_order_mismatch_positions()
        result['c71_checkpoint_order_normalized_for_result_reproduction'] = True
        result['c71_final_stream_comparison_uses_canonical_encoding'] = True
    return result


# base.analyze/main resolve these globals at call time.
base.sources = sources
base.ProofPool = ProofPool
base.View = View
base.Transaction = Transaction
base.scan = scan


def main():
    return base.main()


if __name__=='__main__':
    raise SystemExit(main())
