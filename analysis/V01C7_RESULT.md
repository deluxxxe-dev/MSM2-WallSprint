# c7 result — real-game early hard close

VERIFIED from analysis/V01C7_RUNTIME_RESULT.md: c7/package0.1.25 on the accepted exact executable started Path A at elapsed0.703/t0, wrote again at0.719/t.064, then ended as disabled_kill_or_patch_gate. The user saw effectively vanilla orientation, and verified WallRun continued after source close. Stale suspend/resume was not meaningfully exercised. Position stayed read-only.

The reset/orientation watch stopped with observation_guards_lost immediately after the first A commit. This is temporal evidence, not proof that the diagnostic stop caused the source kill. c7 has multiple independent hard-kill paths. Its generic exit cannot distinguish worker observation-plan/input loss from entry thread/caller/stack/identity/guard failure.

Static review VERIFIED a runtime dependency from a combined verifiedAvailable + diagnostic fieldPlanEnabled + four current hook patches to source/input kill and tick authentication. A diagnostic-only field-plan loss can reproduce that old worker decision, but the checked-in real result does not identify which sub-gate or call site actually failed. The exact real c7 first cause remains UNKNOWN until c8 evidence.

c8 provides immutable first-cause attribution and full side-effect-free observation status, and decouples only the field-plan diagnostic condition after owned-buffer tests prove continued independent A/B authentication. Actual code/patch/build/identity/thread/stack/epoch/rollback loss remains fail-closed. c5 is still the last real full-hold PASS; c8 real-game rotation/hold/first cause are PENDING. Do not advance Position ownership/adhesion.
