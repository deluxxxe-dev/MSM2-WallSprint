# r5 — world wall-plane displacement-facing experiment

2026-10-31 JST; baseline e1ad781, codex/v01c20-edi-value-provenance.
Scope: orientation-only. Existing owner up/down/diagonal TRANSLATION exists.
Analysis/R4_DIRECTIONAL_BASELINE_20261031.md supplies owner/static/runtime evidence;
compressed36.416667s video was inspected by Chat, not remeasured here. Camera roll
is not world/wall vertical. No input clock/descending speed/animation proof.

## Smallest admitted change

Authenticated position differences projected onto the existing authenticated fixed
wall normal supply a normalized tangent. Replace +/-entry Forward classification
with that continuous target. No new address, memory read, API, hook or writer.
Existing coherent A/B basis publication and reconstruction remain unchanged.
Right = unit(normal x forward), Forward = unit(Right x normal), Up = normal;
finite/proper-rotation admission unchanged. Position/W and B scales/metadata are
untouched. Output targets are normalized from current travel, not integrated
matrices, so repeated rotations do not accumulate orientation drift.

## Explicit experimental constants (NOT engine facts)

- Existing observation33ms /two distinct eligible samples; speed>=2;
  maximum gap250ms; equal native-update or wall timestamp WAIT without votes,
  baseline renewal, clock progress or freshness extension.
- NEW headingDeadbandRadians=12deg: travel already within12deg of last accepted
  heading holds it; no tiny-angle retarget churn.
- NEW candidateStabilityRadians=8deg: second direction must be within8deg of the
  FIRST pending candidate, never just latest sample or binary entry sign. Any
  incompatible observation replaces the anchor/count; low/invalid/deadband/reset
  observations clear pending. Accept second direction after required33ms.
- Existing turn120ms and entry0.25s unchanged. atan2(normal dot(from x to),
  from dot to) gives shortest signed planar angle. Numerical opposite tie
  abs(sin)<=1e-6 with negative dot chooses positive pi deterministically.
  Rodrigues evaluation starts from fixed accepted prior heading, no incremental
  multiplication. No retarget/evidence accumulation mid-turn, unchanged policy.
- Existing stale/identity/session/reset behavior and pause-aware clock retained.
  A zero/below2 speed observation HOLDS last valid facing; no desired input
  inference or silent descending-speed threshold reduction.

This is a bounded proof/tuning choice, not evidence that12/8deg suit all gameplay.
A new target immediately after turn completion needs a fresh position baseline
and two observations. This does not fix earlier INPUT START, descending speed,
animation playback or stop-to-vanilla-cling lifecycle.

## Diagnostic contract

WALL_PLANE_HEADING schema=2 replaces REVERSAL_HEADING binary signs. Logs candidate,
accepted target and published world vectors, candidate/turn angle radians, measured
tangent speed, delta, session/native update/wall/pause-aware clocks. Events include
candidate, candidate_unstable, accepted, turn_complete, low_speed_hold,
deadband_hold, observation_reset. No raw addresses/input/game bytes added.
Existing64 event/session bound remains; hold-category events share at most8 of
these64 so stationary samples cannot exhaust turn diagnostics. All diagnostics
remain independent of source permission. Existing4096 write budgets remain.

## Meaningful verification

Pure ASan/UBSan tests preserve WAIT/backward/native regression/gap/stale/reset/
Position/W/compact tests; supersede only binary-axis expectations. Add +/-vertical,
four diagonals/shallow20deg on three world-wall azimuths, shortest +/-pi seam,
positive180 tie, no mid-turn retarget,120 quarter-turn loops/no drift,
ascending/descending alternating tiny lateral noise without binary flips, genuine
full reversal admission, incompatible candidate nonaccumulation/deadband/low-speed
hold. MinHook production fixture uses actual wall clock for vertical/diagonal A/B
publication/commits plus retained31s/20-reversal lifecycle/quota/stale negatives.
Fixtures are owned synthetic/Wine evidence, not actual game/body acceptance.

No shared toolkit/SF/profile changes. Source orientation transition, ownership,
source lifecycle, native hook targets and C52 FAIL/watch-disabled gates unchanged.
Actual owner body facing and descending below-threshold intervals remain UNKNOWN.
