# Next task — r2 reversal latency: equal-timestamp correction and shorter observation interval

Branch codex/v01c20-edi-value-provenance. Do not merge main.
Read AGENTS required docs, current PROJECT_STATE/CODEX_RESULT, analysis/V01C38R1_REVERSAL_RUNTIME_RESULT_20261031.md and existing reversal helper/tests. Do not reopen broad investigation.

## Purpose
Owner confirms r1 reverses, but delayed facing causes initial moonwalk.
Implement this prescribed bounded change; no new input API or movement design.

## Changes, independently verified
A. Correct equal-clock handling in source/include/Math/reversal_heading.hpp:
- Native update regression, wall-clock regression (<), >250ms gap, stale/identity/session failure still invalidate/rebase as before.
- Different nativeUpdate with wallMs==observationMs must WAIT: preserve position/time/update baseline and pending candidate/count/time; no rebase/reset/event acceptance or fabricated elapsed time.
- Same nativeUpdate remains non-evidence. Distinct update and positive eligible elapsed interval required.
- Verify duplicate timestamp cannot extend freshness, bridge lifecycle gaps, accumulate candidate votes or advance turns on its own.
B. Change only heading observation minimum dt and pending confirmation minimum duration from100ms to33ms, preferably one shared named experimental constant.
- Still two consecutive eligible NEW-sign observations, >=33ms apart.
- Keep max dt250ms, tangent speed>=2, direction dot threshold +/-0.8, fixed250ms turn, no mid-turn retarget, entry transition, zero-speed hold, five-second budget and all A/B safety contracts unchanged.
- Do not change MOVEMENT_DELTA_SAMPLE's separate100ms diagnostic schedule.
Test A with old100ms parameters first, then B/combined. Preserve clearly separated correctness and tuning evidence.

## Scope
Allowed: reversal helper/tests, minimal required callsite adjustment, distinct v0.1c38r2/package metadata under existing conventions, bounded diagnostic refinement if needed, task/result/state docs and reproducible artifacts.
No runtime input/velocity/Position writes, animation/camera, native hooks/watches/VEH/VCH, reset_trace, steering assets, c73, producer, shared launcher/checker/loader changes, SF/other MOD changes. Existing C52 and all native write gates unchanged.

## Automated checks / completion
Regression tests: exact same-ms/different-update candidate reproduction; preserve candidate until next real-time sample; repeated same time cannot vote/accept; backward clock and real >250ms gap reset; stale/session identity reset; two consecutive33ms observations accept at66ms after baseline while one candidate cannot; low-speed/noisy/oblique hold; deterministic250ms proper turn/Up; repeated reversal and unchanged position/W/compact metadata.
Run repo-required focused/portable, Windows build/smoke and affected native A/B tests, plus required canonical hooks chain using exact documented commands. No bypass; unchanged skips/unavailable distinguished.
Update PROJECT_STATE/CODEX_RESULT with supplied runtime evidence and exact results; commit/push and independently verify. Keep distinct r2 version/artifacts so owner logs identify it.
Codex does not write user game test prose or deploy on owner's PC.

Reply:
STATUS: success / partial / blocked
CHANGED:
- paths
RESULT:
- separate A/B test evidence, build/canonical checks, published branch/commit/version
NEED:
- exact remaining blocker only
