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
- other later traversal parameters discovered during implementation.

Exact names, ranges, units and defaults must come from the final implementation rather than guessed values in advance.

## Deferred feature: ModSettings integration

Once the relevant parameters exist and have stable semantics, expose them through ModSettings instead of hard-coded constants.

Planned categories:

### Controls
- manual surface-stance button/key;
- hold vs toggle behavior if supported;
- optional enable/disable toggles for traversal subfeatures.

### Adhesion
- adhesion/persistence timing;
- detach grace timing;
- stationary hold behavior;
- related thresholds.

### Jump / gravity return
- surface-jump parameters;
- delay/condition for restoring world gravity;
- orientation return/blend timing.

### Movement / feel
- surface-relative walk/run speed parameters once safe movement control exists;
- transition/interpolation parameters;
- later camera/animation-related tuning only after those systems are implemented.

## Recommended implementation order

1. Current reverse-engineering and writer/effect proof.
2. Safe writable upstream movement source / ownership proof.
3. Basic surface-relative movement and stopping.
4. Adhesion/contact persistence state machine.
5. Manual surface-stance input for idle/walk/stop.
6. Surface jump and controlled return to world gravity.
7. ModSettings exposure for stable parameters.
8. Animation/camera polish and additional traversal features.

## Non-goals for current c21/c22 research

Do not let this roadmap expand the current bounded task. Until the movement foundation is authorized:

- no new Position writes;
- no input/movement writes;
- no speculative adhesion implementation;
- no ModSettings dependency added solely for future UI;
- no camera/animation work.

The roadmap is a reminder of the intended end-state, not permission to skip the current proof sequence.
