# Next task — face existing wall-plane travel, not add duplicate locomotion
Branch codex/v01c20-edi-value-provenance; no main merge.
Read AGENTS required docs, analysis/R4_DIRECTIONAL_BASELINE_20261031.md, current heading implementation and r4 native source paths.
Lock repair implemented; owner repair succeeded and subsequent game log supplied. Do not rerun old tooling/lifetime/input inventory.

## Purpose
Existing game already moves up/down/diagonally. Current heading only chooses +/-entry Forward. Implement the smallest orientation-only proof allowing facing to follow measured travel anywhere in ONE unchanged flat wall plane.
Earlier input-triggered START remains deferred: displacement following is not earlier-input response.
Do not claim to fix animation playback or descending speed.

## Implementation limits
Use ONLY currently authenticated positions, current valid wall normal, lifecycle and existing coherent A/B orientation publication. No new memory/API/hook or movement writer.
Replace binary forward/back heading selection with wall-tangent direction target; preserve proper basis and Up=wall normal. Reuse existing33ms observations,2-sample confirmation,speed>=2,max250ms gap, equal-time WAIT, reset/stale/identity behavior; changes necessary for angular direction confirmation must be explicit named experimental constants and tested, not engine facts.
Use angle-based candidate stability and deadband to avoid tiny lateral changes causing repeated turns; document chosen values in analysis. Keep turn interpolation duration120ms and entry0.25s unchanged. Continuous wall-plane angle with deterministic180deg tie, shortest rotation for other angles; no cumulative drift, no invalid basis, no mid-turn discontinuity. At zero/low speed hold last valid heading, do not infer desired direction.
Pending evidence must not combine different target directions; many-axis travel must not fall back to alternating binary entry signs.
Log bounded candidate/accepted/completed actual vectors/angle, session/native-update/clock and rejection categories where needed; update old binary event contract honestly.
No descending-speed/animation/input patch, stop/idle/lifecycle extension, camera/contact/adhesion/corners,4096 budgets or freshness/native/C52 gate changes.
If valid wall-plane target cannot safely pass existing A/B basis contract, report exact conflict rather than expanding gameplay permissions.

## Scope / verification
Allowed: pure heading math, minimal runtime adapter/log changes, corresponding tests, version/package conventions, analysis/PROJECT_STATE/CODEX_RESULT.
Meaningful cases: both vertical and four diagonal targets, original straight/reversal, no left-right oscillation for stable vertical motion, noise/deadband/low speed hold, unstable candidates cannot accumulate, shortest-angle/180 tie, arbitrary fixed wall orientation, repeated loops no drift, stale/WAIT/reset, Up and Position/W/metadata unchanged.
Run existing required focused/portable/build/smoke/native/hooks/package checks appropriate to runtime change. Preserve existing safety tests, revise only intentionally superseded binary-heading expectations.
No other MOD/shared tooling changes. Commit/push and verify; return STATUS/CHANGED/RESULT/NEED. Chat supplies owner test procedure.
