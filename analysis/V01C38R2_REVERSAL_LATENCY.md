# v0.1c38r2 — separate equal-clock correctness and33ms tuning

2026-10-31 JST. Runtime v0.1c38r2 / package0.1.58, not c73.
Evidence supplied through V01C38R1_REVERSAL_RUNTIME_RESULT_20261031.md.

## Accepted evidence, not an input-latency measurement

Owner confirms r1 turns but perceives initial moonwalk (~one second). First
logged candidate-to-completion intervals were474/358/366/353ms, not input-to-
facing latency. Source-active observations now show Forward reversing. The
unexpected first-session reset was update160 ->161 at identical ms595392421;
no stale episode was reported. This establishes an unnecessary pending reset,
not an explanation of the entire perceived delay. Two five-second source stops
remain expected; no animation/input ownership or continuous-session certificate.

## A — correctness independently verified at original100ms

Only helper time/order handling changes. Observation baseline consists of
position, observationMs and update; pending candidate/count/time are independent
of last-seen transport ordering metadata. Different native update at equal
observation timestamp WAITs with baseline and pending unchanged. Duplicate
update is non-evidence. No fabricated dt, added vote, acceptance/reset event,
lease renewal or turn progress is produced by duplicate timestamps. While
turning, equal render timestamp/update holds the last published-clock pose.

Separate last-seen wall/update metadata detects genuine regression even after
a WAIT; it grants no evidence or freshness. Native/wall regression, real >250ms
gap and existing invalid-data/stale/session/lifecycle boundaries still clear or
rebase as appropriate. A new time after an ignored same-update sample remains
eligible against the actual observation baseline; transport timestamps are not
silently promoted to observation points. A/B source permission checks are unchanged.

First A-only run with literal old100ms conditions passed11 focused groups before
tuning. Final correctness was rechecked in an owned temporary include tree with
the FINAL helper changed only from observationIntervalMs=33 to100; A/legacy test
cases retained, B-specific33ms static_assert/tuning case omitted for this phase.
No runtime configuration switch or alternate permission was added.

Exact final A command (owned fixture setup, not a game/source bypass):

```bash
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -I/tmp/ws-r2-A-final/include -Isource/include /tmp/ws-r2-A-final/reversal_heading_A.cpp -o /tmp/ws-r2-A-final/test_A
/tmp/ws-r2-A-final/test_A
```

Expected exit0,11 groups. Actual exit0,11 PASS. The temporary header is byte-
identical to the production header except the named interval value; command/setup
and phase SHA256s are in CODEX_RESULT. Production33ms runs of the permanent
100ms-timestamp reproduction are NOT mislabeled old-parameter execution.

## B — tuning / combined production33ms

One named experimental observationIntervalMs=33 controls both observation dt
minimum and pending confirmation span. Two consecutive eligible NEW-sign
observations remain required. Synthetic baseline0 -> candidate33 -> accepted66ms
passes; extra distinct-update calls at the same33ms cannot supply the second vote.
This is not a promise of66ms from a user's input.

Maximum dt250ms, tangent speed>=2, dot thresholds+/-0.8, deterministic positive
250ms turn, no mid-turn retarget, exact final f0/-f0, Up normal, entry transition,
Position/W, compact metadata/scales and five-second budget are unchanged.
MOVEMENT_DELTA_SAMPLE's separate100ms schedule and all native_source_override.cpp
code are byte-unchanged. No new input API or movement design.

## Validation and limits

Combined focused command `bash source/tools/test_reversal_heading.sh`:12 groups,
including exact same-ms/different-update history preservation, repeated duplicates,
no freshness extension/turn votes, regression after WAIT, backward wall/gap reset,
33/66ms confirmation, slow/oblique hold, fixed250ms midpoint/Up, repeated reversal,
stale/session reset and existing Position/W/compact invariants.

The old native fixture backdated observation clocks. That seam can collide with
the new real same-ms WAIT and initially failed a candidate assertion; corrected
only the fixture to wait actual34ms for the two samples, drive the turn with
fresh16ms native cadence, and test the existing120ms stale pause. No production
clock/time fabrication was added to make the fixture pass. Actual owned MinHook
A/B publication/store tests pass with the preserved assertions.

An intermediate ordering draft incorrectly gated a valid observation against
all last-seen timestamps; an old100ms ambiguity assertion failed. Ordering and
observation/render baselines were separated; the assertion was NOT removed.
Final A100 and combined33ms both pass. All attempts/results are in CODEX_RESULT.

Cloud/Wine checks do not prove owner r2 game response or animation/input intent.
C52 PRODUCTION_VCH_WITNESS_INTEGRATION_REVIEW_FAIL and all existing native proof/
write gates remain. No Position/input/velocity/animation/camera writes, new
hook/watch/VEH/VCH, reset_trace/producer/config assets, shared checkers/loaders,
SF or other MOD changes. No owner deployment or human game-test instructions.
