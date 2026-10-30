# v0.1c38r1 — orientation-only forward/back reversal proof

2026-10-30 JST. Branch `codex/v01c20-edi-value-provenance`.
Runtime v0.1c38r1 / package0.1.57, distinct from historical c38 and not c73.

## Evidence and scope

Accepted runtime evidence is `WALLRUN_REVERSAL_RUNTIME_RESULT_20261030.md`:
active-source displacement reversed while sampled Transform Forward stayed -Z;
a later source-inactive comparison changed Forward in the same WallRun lifecycle.
Video/log frame synchronization, animation ownership and vanilla input intent
remain unproven. Incoming source Forward may already contain prior WS output.

This implements only the fixed CODEX_NEXT displacement-following orientation
experiment. No Position/input/velocity/animation writes, steering assets, adhesion,
camera, engine API, new hooks/watches or direct-CFG investigation.

## Implementation

- `source/include/Math/reversal_heading.hpp`: WS-owned pure `heading::Reversal`.
  Saved normalized entry target Forward f0 and wall normal n. Evaluates distinct
  fresh-update position differences >=100ms apart, dt<=250ms, after projection
  onto the original wall plane. Speed>=2 and dot(direction,f0)>=0.8 or <=-0.8
  identify forward/reverse. These are experimental thresholds, not engine facts.
- Two consecutive eligible observations of a new sign, >=100ms apart, are
  required. Same-sign/ambiguous/slow observations clear pending confirmation.
  Invalid data/time/update ordering or >250ms gaps clear/rebase observation;
  repeated updates cannot contribute another observation. Motion during turns
  is not accumulated or used for retargeting.
- Turn by a deterministic positive signed angle from the current accepted sign
  about the SAME n, from0 to pi in250ms. Forward is formed by axis-angle rotation,
  Right=normalize(cross(n,Forward)), Forward=normalize(cross(Right,n)), Up=n.
  Proper rotation validation remains mandatory. Final basis uses exact f0/-f0
  as input to re-orthogonalization, not cumulative rotations or opposite-vector
  linear interpolation. Completion clears history before another difference.
- `source/src/NativeHooks/native_source_override.cpp`: only private heading
  state, post-entry tick calculation, observation clearing on existing end/stale
  boundaries, and bounded logs. Shared `source_math::Transition` is unchanged;
  initial interpolation still runs to t=1 before heading observation starts.
- A private post-entry clock accumulates active worker time under the existing
  stale/pause boundaries; it freezes at the last published heading when suspended
  and resumes without charging the pause. This isolates post-entry progress from
  the entry clock's existing clamped t=1 stale-resume rebasing. It does not renew
  source permission or change the real five-second timeout.
- Only `Published::basis` changes. All identity/lifecycle/current-code/thread,
  freshness, collision, guard, F8, end, synchronous-close and five-second source
  budget logic remains. Session replacement resets private heading state;
  existing identity/normal/lifecycle failures end ownership and clear pending.
- Existing coherent AtomicPublication and A/B source paths are unchanged.
  Path A preserves Position/W; B preserves compact row lengths/metadata. No new
  store/hook/consumer or use of incoming source Forward as intent.

`REVERSAL_HEADING` events are capped at64/session and report session/native update,
wall/active-clock time, event, delta/speed estimate, candidate/accepted sign,
target and published Forward and bounded-displacement reason. Candidate,
acceptance, observation-reset and completion are distinct. Existing Actor axes
logs remain independent sampled Transform evidence, not animation telemetry.

## Validation

Focused command: `bash source/tools/test_reversal_heading.sh`.
Ten owned ASan/UBSan groups cover entry/Position preservation, straight travel,
two-sample confirmation, midpoint/proper rotation/Up and final sign, slow/noisy/
oblique hold, duplicate-update rejection, gaps, stale/clock freezing, invalid
input and session reset, repeated reversals without drift/retarget, and the
existing compact scale/metadata contract. `test.sh` runs this focused suite.

The existing Windows `native_source_override_smoke.cpp` fixture additionally
uses owned observation/clock seams plus actual MinHook A/B writers to check a
coherent midpoint/final heading, preserved Position/W/metadata, stale suspension/
resume and Deinit. The new reversal case runs in the standalone fixture; the same updated
production module also runs the retained real-position-watch regression (which
does not itself exercise a turn). This adds no watch machinery or game probe.
All earlier native lifecycle/identity/normal/stale/cleanup tests remain in place.

Exact commands, execution outcomes, the initial fixture-placement correction,
counts, package verification and publication are recorded in CODEX_RESULT.
Cloud fixture/Wine results are not native-Windows or real-game proof.

## Remaining proof and safety

Actual v0.1c38r1 game response remains UNKNOWN. This does not establish engine
input intent, physics velocity, animation/movement ownership, general steering,
corners, or an ownership/safe movement certificate. C52
`PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL` and existing proof/write gates
remain binding except for this explicitly authorized orientation-source behavior.
No SF/shared loader/launcher/other repository changes. No private assets/logs.
Owner update mechanism remains existing Update-WS.cmd after verified publication;
ChatGPT owns the human runtime-test interpretation. No local deployment performed.
