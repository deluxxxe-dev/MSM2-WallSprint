"""Lifecycle corrections for c72's c71 continuation.

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
"""
from pathlib import Path

import slot_publication_post_c71_stitched as base
from slot_publication_post_c71_stitched import *  # re-export c72 surface

c56,c58 = base.c56,base.c58
_original_sources = base.sources
_OriginalView = base.View
_OriginalTransaction = base.Transaction


def sources():
    return dict(_original_sources(),
        slot_publication_post_c71_stitched_fix_py=c56.sha(Path(__file__).read_bytes()))


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


# base.scan/main resolve these globals at call time.
base.sources = sources
base.View = View
base.Transaction = Transaction


def main():
    return base.main()


if __name__=='__main__':
    raise SystemExit(main())
