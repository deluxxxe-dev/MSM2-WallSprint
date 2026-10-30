# After-facing handoff — flat-wall directional movement

Planning only, 2026-10-31 JST. This does not supersede active CODEX_NEXT (retire probes) or queued CODEX_TURN_SPEED (r3). Do not start this work until ChatGPT accepts the r3 runtime result and publishes a bounded implementation/research task into CODEX_NEXT.
Overall sequence/acceptance: docs/FEATURE_ROADMAP.md.

## Product decision
Next implemented feature is sideways travel on ONE existing flat wall as the first step toward forward/back/left/right/diagonal wall-relative locomotion and stopping. Camera follows the completed movement foundation. Do not mistake the existing orientation-only controller for a movement system.

## First bounded investigation to prepare
Use current runtime/source evidence and existing reference analysis. Identify how the intended game build receives directional input and submits motion while WallRun is active, and where current stance/orientation source overrides interact with that route.
Existing reference labels such as UpdateInput, UpdateSteering, RequestSimpleMotion, GravityOrientationOwner are search hints only. No copied offsets/signatures/ABI, guessed APIs or broad binary scan.
Desired result: one independently authenticated candidate route with exact inputs, coordinate meaning, WallRun selection, caller/lifetime and restoration boundaries; separate measured facts/static conclusions/unknowns.
Do not require a full ownership proof for every engine write if a verified high-level game API contract can meet the necessary conditions. Equally, a function name/signature match is not enough.
If unavailable, stop with one precise missing fact; do not automatically reopen c73.

## First subsequent runtime experiment, only after candidate admission
One lateral direction, one planar wall, existing contact behavior and fixed conservative speed policy; log requested direction, actual displacement, surface normal and facing. No camera/animation/adhesion/corner changes in the same proof.
Exact API/commands/thresholds must be designed from the investigation result, not invented now.
Then test opposite lateral direction and diagonals separately.
Five-second window remains until separate lifecycle acceptance. No brute-force Position/gravity/velocity writes.

## Remaining movement milestone
Stopping/contact, manual idle/walk stance if safe, ordinary detach/recovery, cardinal/diagonal wall verification and ground-sprint-like posture. Custom adhesion only if existing contact is demonstrably insufficient.
Do not jump to camera because simple reversal looks correct.

## Codex discipline
Only execute the later bounded CODEX_NEXT, not this whole roadmap. Build/tests from then-current docs; ChatGPT owns actual game instructions. This planning document authorizes no code changes by itself.
