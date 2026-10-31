# Initial wall-as-ground intent review — 2026-10-31 JST

User requested project-chat history search before choosing WR-derived vs ground-Sprint locomotion architecture. Personal Context search retrieved the following dated conversation summaries; these are recovered historical statements, not a fresh raw-log/static-code certification or an exhaustive search of every project chat.

## Recovered history
- 2026-10-22 user specification: transition from vanilla WallRun to normal-Sprint-like WallSprint stance; wall as ground; gravity/contact/parkour/swing/combat were goals.
- v0.1a3 README_TEST historical content explicitly excluded normal Sprint animation switch, Actor posture, adhesion and wall-relative movement; earlier confirmed observations were Hero/position and markers/dumps.
- 2026-10-22 v0.1b discussion: posture-only build, not ground-Sprint motion replacement. Reports described successful immediate posture writes overwritten toward world-up within10–20ms. Ground Sprint animation remained unimplemented.
- v0.1b3 discussion: identify downstream orientation writer, compare WallRun with WallCrawl on the same wall. Adhesion/movement/Sprint animation/camera/gravity changes explicitly absent.
- 2026-10-24 retrospective: wall normal/SurfaceFrame/posture established; locomotion still native WR. Position-writer research was a route to movement, not proof that native ground Sprint could or could not work on a wall.
- Current FEATURE_ROADMAP already preserves wall-as-ground, ground-sprint-like final intent. Current r7 preserves native WR movement and changes orientation only.

## Correct interpretation
Wall-as-ground Sprint was the ORIGINAL intent, not a newly invented feature.
Evidence found does NOT show a completed actual ground-Sprint locomotion transplant or arbitrary-gravity/support-normal test. Do NOT label that architecture rejected, impossible, or already experimentally failed.
The posture-overwrite result only concerns those orientation writes. Actor Up alignment does not establish controller gravity/support direction or ground-state admission.
Historical explanation: posture restoration/ownership investigation became the immediate blocker; subsequent work continued to reuse WR movement. The search did not recover an explicit comparative decision proving this architecture superior to ground Sprint.

## Architecture recommendation
Before further native-WR-angle tuning, compare THREE concrete routes using current code/available evidence:
A native ground locomotion (not merely its animation) parameterized by wall support/down direction;
B WS-owned surface-relative movement request using native collision/contact controller, independent of WR directional limits;
C modifying native WR steering/constraints.
A is closest to original user mental model but feasibility UNKNOWN; B is a candidate if ground state assumes world-up; C must not be selected merely because existing WS uses WR.
Keep acquired surface basis/lifecycle/restore logic where reusable. No reset/rewrite of working orientation source and no production changes authorized by architecture research alone.
First discriminate whether existing ground controller actually consumes arbitrary support/up/gravity or hard-coded world vertical. Do not request repeated posture-only or unchanged ON/OFF owner runs as proof.
Current CODEX_NEXT design task is expanded to include this comparison, with one preferred route and concrete missing proof. No new package/game test for this documentation change.
