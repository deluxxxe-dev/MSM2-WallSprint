# Next task — orientation-only reversal proof

Branch: `codex/v01c20-edi-value-provenance`. Do not merge main.
Implement the following fixed design, not another investigation/design cycle.

## Purpose / evidence
Read AGENTS and its required operational docs, then:
- analysis/WALLRUN_REVERSAL_RUNTIME_RESULT_20261030.md
- analysis/WALLRUN_MOVEMENT_FACING_SOURCE_REVIEW_20261030.md

Runtime: active-source displacement reversed while sampled Transform Forward stayed negative Z. After the existing source timeout, later reversal changed Forward. Current Transition retains an entry-derived target. This supports an orientation-only experiment, not an engine input/velocity/animation ownership claim.

## Exact minimum design
Only support forward/back reversal along the original wall-tangent heading in the current admitted planar-wall session. General steering/corners are deferred.
Keep initial entry transition unchanged. Once it reaches t=1:
1. Retain normalized entry target Forward f0 and surface normal n.
2. From existing validated Hero position snapshots and distinct fresh native updates, maintain a separate small heading-observation state (not the capped diagnostic sampler). Evaluate successive position differences at >=100ms, dt<=250ms. Project out n. Units/velocity remain position-difference estimates, not physics velocity.
3. Accept a direction candidate only when tangent speed>=2 world-units/s and normalized tangent direction dot f0>=0.8 (forward) or <=-0.8 (reverse). These are chosen experimental thresholds, not engine constants. Otherwise hold the current accepted heading and clear pending evidence.
4. Require two consecutive eligible observations of the same NEW sign, spanning >=100ms, before starting a turn. Invalid dt/data, identity/lifecycle/normal discontinuity, or stale suspension clears pending history and rebases position before another difference. Do not accumulate across pauses/sessions.
5. Turn Forward/Right by a deterministic positive signed angle about the SAME wall normal from current accepted sign to the opposite sign: 180 degrees over 0.25 seconds of the existing pause-aware transition clock. Rebuild Right=cross(n,Forward), then Forward=cross(Right,n), normalize/validate proper rotation. Up remains n. Do not linearly blend opposite Forward vectors through zero or slerp arbitrary whole poses.
6. Complete the accepted turn without retargeting mid-turn; rebase displacement/pending history on completion. Near-zero or ambiguous motion must not initiate another turn. Keep exact final f0/-f0, not cumulative drift.
7. Publish the resulting basis through the SAME existing Published/A/B source paths. Do not create a writer/hook/API. A/B must consume one coherent publication. Preserve Position/W/metadata/scales and every current admission/stale/cleanup/timeout guard.

This is deliberately a displacement-following facing proof; it does not promise anticipatory input-facing or full WallSprint movement. Do not use incoming source Forward as independent vanilla intent.

## Change scope
Allowed: native_source_override.cpp private session state/tick and bounded heading logs; a small WS-owned pure math helper and focused tests; existing version/build/package metadata and task/result/state docs.
Avoid altering shared Transition behavior unless strictly needed; isolate the post-entry heading computation.
Log bounded heading-candidate/accepted/turn-complete events with session/update/time, displacement estimate, speed, sign, target/published Forward and reason. Existing Actor axes logs supply independent sampled Forward.
No growth of watch/native diagnostics; no change to reset_trace/hardware/VEH/VCH/producer logic. No Position/source-position/input/velocity writes, adhesion, animation/camera, config asset edits, c73 or broad binary scans. Existing orientation-source replacement is the ONLY authorized gameplay-output change.
No shared launcher/loader/SF/other MOD changes.
C52 remains binding; no ownership or safe movement certificate follows. Keep five-second source budget and existing F8/end handling unchanged.

## Completion / automated validation
Use exact repository build/test commands from required docs. Focused tests must cover: unchanged entry; straight travel; clear reversal; slow/noisy/oblique hold; two-sample confirmation; 180-degree midpoint finite proper rotation/Up preservation; final sign; repeated reversal without drift; stale/gap/identity/session reset; unchanged Position/W and A/B invariants.
Run existing relevant math/source tests and Windows build/check, review diff. If blocked by an existing unrelated failure report it separately; do not widen work.
Use a distinct runtime/package version under repo conventions (e.g. v0.1c38r1 if supported); do not call it c73 or overwrite indistinguishably the c38 build.
Update CODEX_RESULT and PROJECT_STATE with exact changes/tests/remaining real-game proof. Commit/push and verify publication. Owner uses existing Update-WS.cmd after success; do not execute local deployment or write user test prose.
If this design cannot fit the existing safe orientation publication contract, return the exact blocker without substituting another movement design.

## Reply only
STATUS: success / partial / blocked
CHANGED:
- files
RESULT:
- implementation and automated checks; exact published branch/commit/version
NEED:
- Chat-side decision only if required
