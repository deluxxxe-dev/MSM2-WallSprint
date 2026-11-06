# WallSprint feature roadmap

This file records deferred user-facing features that should be implemented only after the underlying movement/ownership foundation is safe enough. It does not authorize bypassing the current analysis gates.

## Design principle

Current near-term priority is completing the bounded orientation/reversal proof, then establishing safe wall-relative directional locomotion. A validated high-level motion route may replace the deferred low-level writer route; a roadmap is not permission to bypass ownership/lifecycle proof. User-facing controls and tuning follow the relevant demonstrated behavior.

## Agreed overall flow — reconciled 2026-10-31 JST

User's requested sequence recovered from prior project conversation (2026-10-22) is: movement -> wall-relative camera -> wall jump -> corners/ceilings/complex surfaces -> parkour -> swing -> combat -> stabilization.
The older order at the bottom of this document placed camera late; that order is superseded by the reconciled sequence below. Keep useful deferred feature details, not that conflicting global ordering.
Historical v0.x labels described feature milestones, not the current c/r diagnostic build numbers or completion percentages.

Final intent: vanilla WallRun transitions after a delay into a wall-as-ground locomotion stance with a normal ground-sprint-like posture; later wall-relative camera, wall-to-wall jump, parkour, swing, ceilings and wall combat. Finishers transition to ground. These are goals, not implemented claims. Current independent implementation/no third-party binary copying rules remain binding over older reference-use discussion.

Current state: r2 has actual reversal evidence, remaining visible delay; r3 duration-only120ms experiment is queued in CODEX_TURN_SPEED.md. Steering Probe retirement is the separate current tooling task. Existing movement is still supplied by vanilla; reversing WS orientation does NOT establish arbitrary wall-relative input/movement control. Five-second source budget remains diagnostic; continuous stance is not accepted.
R2 log: six completed candidate-to-completion intervals approximately302–322ms, not input latency. No animation owner has been identified.

### Immediate next feature after reversal acceptance

**Wall-relative directional movement and stopping on ONE flat wall.**
Expected play behavior: forward/back/left/right/diagonal directional requests on the same surface, heading following actual travel, stable slowdown/stop and ordinary detach without transform corruption.
First proof is only ONE additional sideways direction on the same flat wall; then its opposite and diagonals. Do not change multiple unknown engine quantities together.
Reuse vanilla contact/gravity where demonstrated sufficient. A bespoke adhesion force is conditional on an observed contact failure, not a mandatory implementation for its own sake.
Zero-speed idle/manual stance is a later substep after contact/lifecycle is proven. Stationary wall hold is not predeclared solved by heading hold.

Before writing movement, identify one authenticated current-build input/motion-request route and its lifecycle/ownership. Prefer a bounded higher-level investigation suggested by existing reference labels. If no route is proven, next task is read-only proof, not guessed Position/velocity patches.
Do not automatically resume c56–c72/c73 low-level publisher scans. Return there only for a concrete necessary unresolved question.

### Acceptance to leave the movement milestone

- Initial entry, straight motion, reversals, sideways and diagonals work on one flat wall.
- Cardinal and diagonal wall orientations are tested; forward/normal/axes remain consistent.
- Near-zero motion does not flicker or turn spuriously. Stop behavior and detach are explicit.
- Surface contact is maintained without unexplained drift/clipping; independent adhesion only if measured necessary.
- A lifecycle review separately resolves the current five-second diagnostic stop before any longer active window. Never merely remove the timeout.
- End/detach, stale identity, death/reload/fast travel restore ordinary behavior under bounded tests.
- Ground-sprint-like posture is visually verified. If required animation changes are unproven, mark that gap; do not label movement milestone complete.
- Only then begin the camera milestone. ModSettings exposes stable controls/values incrementally, not before their meanings exist.

## Polish backlog recorded 2026-11-05 (owner)

- Natural sprint turning on the wall: today direction and facing change instantly (sliding feel). Target: ground-sprint-like turns
  (smoothed heading toward the stick, turn/pivot animation, lean). Not blocking the current milestone.
- Fine facing, standoff distance from the wall, WR->WS transition jitter (M2b notes). Owner 2026-11-06: feet float off the wall, body pitched slightly up (deferred).

## Deferred feature: manual surface-stance hold

Goal: allow the player to remain oriented perpendicular to the current wall/surface even when not sprinting, so WallSprint is not limited to dash-only maintenance of the surface stance.

Desired interaction:

- reserve an otherwise-unused controller button / keyboard key;
- while the control is held or toggled, preserve the surface-relative standing state even at zero speed or walking speed;
- allow stopping on a wall without immediately falling back to ordinary world-up orientation;
- later support walking on the surface under the same maintained stance;
- exact input binding/toggle-vs-hold behavior is intentionally deferred until input integration is safe.

Implementation gate:

1. safe upstream surface-relative movement source established;
2. stationary/low-speed state can be distinguished without fighting vanilla ownership;
3. surface contact/adhesion persistence has a bounded state machine;
4. only then add the manual stance input and idle/walk behavior.

This feature must not be implemented by brute-force final Position writes merely to hold the actor against the wall.

## Deferred feature: surface-relative obstacle / protrusion detection

Goal: while WallSprint is active, detect geometry that protrudes approximately perpendicular to the currently traversed surface and stop or transition before the actor clips through or blindly runs over it.

Desired behavior:

- perform obstacle sensing in the current SurfaceFrame rather than world-up coordinates;
- distinguish the supporting wall/surface from a new face intruding into the actor's forward/tangential path;
- classify a sufficiently different encountered normal as an obstacle/protrusion candidate rather than immediately replacing the support surface;
- stop before the projected capsule/body path intersects a perpendicular ledge, trim, sill, beam, sign, facade detail, or similar geometry;
- retain the current wall-relative stance while stopped when adhesion/manual stance permits it;
- later allow explicit traversal decisions such as step/transition, corner transfer, jump, Web Zip, or detach instead of forcing one behavior for every obstruction.

Likely implementation shape after movement/contact foundations exist:

1. maintain the current support normal and SurfaceFrame;
2. cast/probe along the surface-relative movement direction using the player collision footprint rather than only a single point ray;
3. compare the encountered face normal with the current support normal and movement direction;
4. if the hit represents a sufficiently perpendicular/intruding face within stopping distance, classify it as a blocking protrusion;
5. reduce/cancel tangential movement before penetration and enter a bounded wall-relative stopped state;
6. separately handle support loss, convex/concave corners, traversable slopes and false positives.

Exact probe shape, normal-angle thresholds, stopping distance and transition policy must be derived from real geometry/runtime behavior rather than guessed now.

## Deferred feature: WallSprint Web Zip / Zip Jump integration

Goal: allow the game's existing Web Zip / Zip Jump traversal family to remain usable while the player is in a WallSprint/surface-relative stance.

Desired behavior:

- do not globally suppress vanilla Web Zip input merely because WallSprint is active;
- when a valid vanilla zip target is available, temporarily hand movement authority to the zip action instead of fighting it with WallSprint locomotion;
- preserve enough WallSprint surface context to decide whether the action should return to the previous/nearby surface, transfer to a new surface, or detach into ordinary world-gravity traversal;
- allow a zip endpoint on or near a valid wall surface to reacquire SurfaceFrame/adhesion cleanly;
- support a subsequent Zip Jump / point-launch style action without forcing the actor back to world-up too early;
- define jump impulse and gravity/orientation return relative to the active surface when appropriate, then blend back to ordinary gravity according to the normal detach/gravity-transition rules;
- respect obstacle/protrusion detection so a zip path or landing state does not drive the actor through perpendicular facade geometry.

Preferred integration strategy is state handoff, not simultaneous competing writers:

```text
Surface locomotion / stance
        -> vanilla zip acquisition / launch
        -> zip travel
        -> endpoint classification
              -> reacquire surface
              -> surface-relative Zip Jump
              -> ordinary detach / world gravity
```

The first implementation should reuse authenticated vanilla zip states/functions where practical instead of rebuilding the full web-zip trajectory from scratch. Exact native states and ownership boundaries are intentionally deferred until the movement foundation is complete.

## Deferred feature: adhesion / detach / gravity transition tuning

Expose the major traversal timings/thresholds as configurable parameters once their runtime semantics are stable. Candidate settings include:

- surface adhesion duration / persistence;
- delay before detaching when movement stops;
- manual stance hold behavior;
- detach grace period;
- jump impulse while surface-relative;
- time or condition before a wall-jump returns to ordinary world gravity;
- orientation blend time from surface frame back to world-up;
- transition smoothing / snap thresholds;
- obstacle/protrusion detection distance and angular tolerance once validated;
- Web Zip / Zip Jump WallSprint integration toggles and transition timing once validated;
- other later traversal parameters discovered during implementation.

Exact names, ranges, units and defaults must come from the final implementation rather than guessed values in advance.

## Deferred feature: ModSettings integration

Once the relevant parameters exist and have stable semantics, expose them through ModSettings instead of hard-coded constants.

Planned categories:

### Controls
- manual surface-stance button/key;
- hold vs toggle behavior if supported;
- optional enable/disable toggles for traversal subfeatures;
- optional WallSprint Web Zip / Zip Jump enable/disable controls if needed.

### Adhesion
- adhesion/persistence timing;
- detach grace timing;
- stationary hold behavior;
- related thresholds.

### Jump / gravity return
- 2026-11-06 owner: WS jump and charge jump, first only the ground-sprint-derived variants (sprint jump, sprint charge jump); M8a swaps the jump clip (analysis/M8_JUMP_PLAN_20261106.md). Open choice: keep vanilla vertical-WR web-climb (sling up) jump and use WS jump only on charge jump, or replace it.
- surface-jump parameters;
- delay/condition for restoring world gravity;
- orientation return/blend timing;
- surface-relative Zip Jump transition parameters once semantics are stable.

### Movement / feel
- surface-relative walk/run speed parameters once safe movement control exists;
- transition/interpolation parameters;
- validated obstacle/protrusion stopping thresholds;
- later camera/animation-related tuning only after those systems are implemented.

## Recommended implementation order

| Stage | Scope | Exit condition |
|---|---|---|
| Current | Probe retirement as separate tooling fix; r3 facing-delay experiment and actual-game acceptance | No unwanted probes; reproducible reversal/low-speed behavior |
| 1 Movement foundation | Authenticated safe input/motion route; one flat wall sideways proof; then four-way/diagonal, stop, contact and lifecycle/stance duration; required sprint posture | Movement acceptance checklist above |
| 2 Camera | Wall-relative up, input/view consistency, smooth entry/detach return | Playable camera without disrupting accepted movement |
| 3 Jump | Surface jump, wall-to-wall landing, controlled world-gravity return | Jump/landing/detach distinct and verified |
| 4 Surface changes | Corners, ceilings and complex/curved surfaces one at a time; obstacle/protrusion classification | No tunneling or unsupported surface transfer |
| 5 Parkour / zip | Existing parkour and Web Zip handoff/reacquisition; Zip Jump | Each vanilla action receives exclusive control and returns safely |
| 6 Wall Swing | Swing entry, release and surface reacquisition | No competing movement ownership |
| 7 Wall Combat | Basic wall combat/web attacks, then extensions; Finisher returns to ground | Combat/traversal ownership and recovery verified |
| 8 Stabilization | Regression, compatibility, settings and distribution | Explicit supported behavior/limitations and repeatable install |

Contact validation/basic safe stopping belongs to stage1. Sophisticated protrusion handling belongs to stage4 (bring forward only if an observed blocker makes it necessary). Manual surface hold/walk is a stage1 subgoal after safe zero-speed/contact proof; binding/toggle choice remains deferred. ModSettings is incremental for stable parameters, with final UI/polish late.
Camera is a dedicated stage2 feature; minimum posture correctness belongs to stage1 rather than deferring all animation until the end.

## Non-goals for current movement-ownership research

Do not let this roadmap expand the current bounded task. Until the movement foundation is authorized:

- no new Position writes;
- no input/movement writes;
- no speculative adhesion implementation;
- no speculative Web Zip / Zip Jump hook;
- no speculative obstacle collision rewrite;
- no ModSettings dependency added solely for future UI;
- no camera/animation work.

The roadmap is a reminder of the intended end-state, not permission to skip the current proof sequence.

## Owner requirement 2026-11-06: player-tunable presets (ModSettings)
- Camera distance/height/pivot/FOV, travel speed, turn rate, camera follow rate/delay and similar values must later be adjustable per player in ModSettings, with savable presets.
- Current work (M6) only searches for the DEFAULT preset values (ground-sprint parity). Record each chosen default with its measurement in analysis/M6_TURNING_20261105.md.
- Design note: today the camera values come from a config file baked into the Overstrike stage at Update-WS time. They cannot change while the game runs. A live ModSettings preset needs one of two routes: (a) the native side writes the loaded config values in memory (needs the loaded-asset location), or (b) the native side applies the offsets/speed itself. Native-side values (turn rate 400 deg/s, camera follow 90 deg/s / 10 deg / 1 s) are already single constants and are easy to expose.
- Speed: steering MoveSpeedFast is NOT the travel speed (M6i). The real speed control is still UNKNOWN.
- Camera auto-centering during WS: default OFF (owner), INI WALLSPRINT_CAMERA_AUTOCENTER now, ModSettings toggle later.
