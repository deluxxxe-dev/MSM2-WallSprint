# Next task — one flat-wall lateral movement route
Branch codex/v01c20-edi-value-provenance; do not merge main.
Read AGENTS required docs, analysis/V01C38R4_OWNER_RUNTIME_20261031.md, docs/AFTER_TURN_PLAN.md, analysis/INPUT_START_ADAPTER_INVENTORY.md and analysis/WALLRUN_MOVEMENT_FACING_SOURCE_REVIEW_20261030.md. Consult relevant Wisdoms movement references.

## Purpose / product decision
Owner accepts r4 repeated-reversal persistence in supplied runtime and explicitly permits deferring earlier facing START. Do NOT repeat completed lifetime or input inventory tasks.
Prepare the first flat-wall ONE-direction lateral motion proof. This task is targeted route research only, not gameplay implementation.
Earlier facing START remains unresolved backlog; no turn duration/detection/input patch in this task.

## Narrow investigation
Using existing reference analyses, source definitions/callers and already available authenticated current-build captures, identify ONE best route by which original verified WallRun receives requested movement and submits wall-tangent motion.
Start at known WallRun_Update and existing indexed steering/motion references, not a new repository-wide/binary-wide search.
Labels UpdateInput/UpdateSteering/RequestSimpleMotion/GravityOrientationOwner are search hints, not authenticated APIs.
Provide actual available evidence for input type/coordinate meaning, caller/build identity, state/owner/lifetime, contact handling, and ordinary exit/restoration. Explain interaction with existing A/B orientation override.
A verified high-level game API can be preferable to low-level Position writes; do not demand unrelated whole-engine ownership proof, but names or copied offsets/ABI alone are insufficient.
Existing pure planar/controller helpers have fixture-only input producers; do not present them as runtime movement.
No action-mapped input reader exists in last inventory. Do not repeat inventory just to rediscover this. Investigate the one original movement consumer/route that could supply the missing link.
If exact function bytes/current-build data needed are absent, specify ONE precise missing capture/function/field and the smallest existing read-only acquisition method. Do not invent commands or start large automated RE.

## Output / limits
One compact analysis: verified runtime facts / static evidence / candidate / exact blocker; source paths and existing evidence references; recommendation for ONE next admitted experiment or ONE next bounded read-only acquisition.
No native/gameplay writes, new hooks, pointer chase, arbitrary offset probes, input/Position/velocity/gravity mutation, c73 or broad framework.
No camera/animation/adhesion/corner/swing/combat, other MOD/shared environment changes.
Do not remove independent4096 store/attempt budgets or stale/ownership/build gates.
Allowed: analysis and relevant PROJECT_STATE/CODEX_RESULT updates only unless a necessary existing offline inspection helper is narrowly justified; no new runtime version/build for docs-only change.
Validate docs/diff and existing relevant offline tooling only if changed. Commit/push and verify. Reply STATUS/CHANGED/RESULT/NEED only. Chat supplies owner test commands when an actual experiment is ready.
