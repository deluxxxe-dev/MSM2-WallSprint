# Input-triggered reversal START — queued after lifetime patch
Branch codex/v01c20-edi-value-provenance. Read AGENTS required docs and analysis/REVERSAL_START_TIMING_REVIEW.md.
Owner priority: begin facing reversal on reverse INPUT, approximately1second earlier than current perceived response. Do not speed up animation or existing rotation duration. The1s is a desired improvement, not measured latency or an offset constant.

## Scope / concrete next action
The completed review proved displacement-only trigger cannot know earlier intent. Do not repeat that review or finish solely by restating missing evidence.
Perform one focused inventory of existing repository input adapters, game-facing directional input definitions and current authenticated read paths; consult relevant Wisdoms input references. Distinguish test-only Controller helpers from runtime callers.
Identify ONE best existing read-only requested-direction candidate, with actual definition/callers, coordinate system, freshness/session/focus/menu conditions and evidence status.
Do not equate raw key state with game-accepted direction or infer direction from WS-overwritten orientation. Keyboard remapping/controller/camera-relative mapping must be explicit; do not silently hard-code WASD.
If already proven game-requested wall-relative input exists, implement a minimal trigger adapter using it in a separate commit AFTER lifetime patch, preserving existing turn duration and A/B orientation write gates. Distinguish repeated held input from new reverse intent and handle release/conflicting inputs/focus loss/session reset without flip loops.
If runtime semantic/order proof is missing, prepare the smallest bounded read-only diagnostic for the ONE supported candidate only when address/API/type and safe access are statically established; pair candidate direction, displacement sign, session/native update/monotonic clock. No unbounded polling/dumps or writes. Actual in-game evidence required before enabling trigger.
If no candidate exists within existing input adapters/definitions, stop with exact searched paths and one tightly specified next discovery target; do not launch broad binary scans/new hook frameworks or invent offsets/ABI.
No Position/velocity/input/animation writes, new movement framework, c73, shared environment/other mods or bypass of existing safety gates.
New hooks/unknown game API calls are not authorized by this bounded task.

## Validation / handoff
Use repository-required checks appropriate to actual changes. Meaningful tests for input adapter if implemented: requested reversal with old/zero displacement, held input one turn only, both signs/repeated reversals, focus/menu loss and invalid identity/stale source cannot trigger, existing rotation duration unchanged.
Synthetic tests are not game acceptance and do not prove1s gain.
Update PROJECT_STATE/CODEX_RESULT, document exact missing runtime test fields when needed, commit/push and verify.
Return STATUS/CHANGED/RESULT/NEED only; Chat supplies update/test procedure.
