# c4 result carried into c5

Authority: V01C4_RUNTIME_RESULT.md. Base: published6bf7646d2aebdfda9c285b9a3ca4108760d6baa1; implementationcfaae52304dfa757f2098eeb63c0168151c5b755.

VERIFIED REAL GAME: exact build and compact proof/admission passed. On a large unobstructed A-direction wall with N398 approximately(0,0,+1), both source paths wrote Surface Up. ORIENTATION_RETENTION_PASS=1 at329ms/20 distinct updates; the same sequence reached about55 updates/969ms. The user no longer saw the previous rapid flicker.

VERIFIED REAL GAME: process enumeration repaired, matches129/cap128/eligible127/installed126. One already-suspended thread was refused; all126 owned debug states restored with cleanup_pending=0. Coverage was incomplete because of cap/refusal. No third writer event/identity was captured; this does not prove absence.

VERIFIED REAL GAME: ownership exited around elapsed1.953s with native_lifecycle_inactive_or_stale, although verified WallRun continued to Deinit around3.900s. A/B writes stopped; Actor Up later returned to World Up. This establishes a continuity failure, not an unavoidable third-writer race.

UNKNOWN: the precise failing native sub-gate in that run. Its collapsed reason cannot distinguish publication return/phase/epoch/owner/update/time order/age. c5 must report these individually. c4 proves threshold retention, not lifecycle-wide ownership or permission for adhesion/movement/camera.
