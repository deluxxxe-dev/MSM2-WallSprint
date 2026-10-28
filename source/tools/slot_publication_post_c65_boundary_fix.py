"""c67 correction for converted c64 POST lower-capture observation.

The accepted c66 implementation intentionally rethrows a c64 POST as c66 POST
before storing c65's diagnostic-only post63 capture.  C67 needs that exact lower
capture only to classify the already-observed c66 terminal.  This shim leaves
c66 and all accepted evidence read-only, snapshots the lower c65 edge-entry
context before the unchanged c66 edge call, and reconstructs the omitted lower
capture only after c66 actually reports POST caused by c64 POST.  The existing
c65 capture validator is then used unchanged and C67 still rethrows the original
refusal without traversal, admission, Fact propagation or candidate creation.
"""
from copy import deepcopy
from pathlib import Path

import slot_publication_post_c65_boundary as base

c66,c65,c64,c58,c56 = base.c66,base.c65,base.c64,base.c58,base.c56
_original_sources = base.sources


def sources():
    return dict(_original_sources(),
        slot_publication_post_c65_boundary_fix_py=c56.sha(Path(__file__).read_bytes()))


class View(c66.View):
    """C67 observer with exact reconstruction of c66's omitted c64 lower capture."""
    def __init__(self,certificates):
        super().__init__(certificates)
        self.post65_capture,self.post65_edge_before,self.post65_underlying_refusal = None,None,None

    def edge(self,index,ins,kind,target,architectural,relations):
        tx = self.tx
        before = base.edge_before(tx,self,index,ins) if tx is not None else None
        lower_before = c65.edge_before(tx,self,index,ins) if tx is not None else None
        try:
            return super().edge(index,ins,kind,target,architectural,relations)
        except c58.Refusal as error:
            if str(error)==c66.POST:
                if tx is None:
                    raise c58.EvidenceError('missing c66 terminal transaction')
                self.post65_edge_before = before
                cause = error.__cause__
                self.post65_underlying_refusal = str(cause) if cause is not None else None

                # c66 converts this exact lower refusal before its own diagnostic
                # post63_capture assignment.  Reconstruct only after observing the
                # real c66 POST+c64 POST chain, using the pre-call lower edge-entry
                # snapshot and the unchanged c65 validator.
                if self.post65_underlying_refusal==c64.POST:
                    if cause is None or lower_before is None:
                        raise c58.EvidenceError('converted c64 POST lacks reconstructable lower edge entry')
                    self.post63_edge_before = deepcopy(lower_before)
                    self.post63_underlying_refusal = str(cause.__cause__) if cause.__cause__ is not None else None
                    lower_capture = c65.capture_for(tx,self,index,ins)
                    if self.post63_capture is not None and c56.encoded(self.post63_capture)!=c56.encoded(lower_capture):
                        raise c58.EvidenceError('converted c64 POST lower capture conflicts with current exact source')
                    self.post63_capture = lower_capture

                capture = base.capture_for(tx,self,index,ins)
                if self.post65_capture is not None and c56.encoded(self.post65_capture)!=c56.encoded(capture):
                    raise c58.EvidenceError('multiple distinct first terminal c66 source events')
                self.post65_capture = capture
            raise


# Patch only the c67 observer globals.  c66 and all accepted c56-c66 modules and
# owner artifacts remain unchanged/read-only.
base.sources = sources
base.View = View


def main():
    return base.main()


if __name__=='__main__':
    raise SystemExit(main())
