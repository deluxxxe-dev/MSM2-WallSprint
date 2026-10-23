# c6 result — measured transient freshness regression

VERIFIED from analysis/V01C6_RUNTIME_RESULT.md: c6/package0.1.24 remained Position-read-only (proof0, corrections0, source Position writes0). The visible early release was reproduced in the private runtime evidence: session1/elapsed2.642367, coherent active phase3/update191, native update age250ms, owner/Hero/Transform matched, accepted/collision epochs0. c6 permanently ended ownership as native_lifecycle_inactive_or_stale. The real WallRun then continued ~1.4s before verified Deinit at4078ms. A132/B130, retention threshold PASS, max held1422ms/114 updates, until-Deinit0.

This is a real orientation lifecycle regression, despite c5 normal-cadence full hold3843ms/A219/B218/held2906ms having passed. Read-only planar diagnostics did not cause the release. Exact cause of the scheduling gap is UNKNOWN; a250ms update-age gap itself is measured. No full raw log or game function capture is needed to implement the supplied c7 task.

c7 changes time-only stale handling into suspension/recovery and retains all Position denials. Real c7 visual lifecycle-wide hold is PENDING. Position whole-path/W/downstream-exclusive-effect proof remains incomplete; no adhesion store is authorized.
