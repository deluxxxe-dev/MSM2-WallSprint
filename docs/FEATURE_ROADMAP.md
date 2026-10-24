# WallSprint feature roadmap

This file records deferred user-facing features that should be implemented only after the underlying movement/ownership foundation is safe enough. It does not authorize bypassing the current analysis gates.

## Design principle

Current priority remains engine ownership / movement-source proof. User-facing controls and tuning should be layered on top only after the relevant behavior exists safely and deterministically.

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

1. Current reverse-engineering and writer/effect proof.
2. Safe writable upstream movement source / ownership proof.
3. Basic surface-relative movement and stopping.
4. Adhesion/contact persistence state machine.
5. Surface-relative obstacle / protrusion sensing and safe stop behavior.
6. Manual surface-stance input for idle/walk/stop.
7. Basic surface jump and controlled return to world gravity.
8. Vanilla Web Zip handoff/reacquisition while WallSprint is active.
9. Surface-relative Zip Jump / point-launch transition and gravity handoff.
10. Corner/complex-surface traversal policies using the same contact classifier.
11. ModSettings exposure for stable parameters.
12. Animation/camera polish and additional traversal features.

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
