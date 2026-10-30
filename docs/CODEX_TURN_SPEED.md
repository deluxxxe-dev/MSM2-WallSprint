# Queued task — r3 turn-duration-only experiment
Execute AFTER the current Steering Probe retirement task has been completed and published. Do not interleave its tooling changes with this runtime experiment. Read AGENTS required documents, current PROJECT_STATE, CODEX_NEXT and CODEX_RESULT first; this file explicitly authorizes only the runtime scope below, after that prerequisite. Do not merge main.

## Supplied r2 runtime evidence — 2026-10-31 JST
ChatGPT inspected MSM2-WS-v0.1c38r2-20261031-012913.log.
Six completed turns across sessions2/3/4: candidate-to-completion302/304/302/303/322/303ms by log line times. Candidate-to-accepted41–54ms; accepted-to-completion248–270ms. Internal clock duration can differ slightly from emitted log time.
Owner still observes facing lag relative to animation.
Video reattached successfully as 42.918s clip "Marvel's Spider-Man 2 v2.810.0.0 2026-10-31 01-30-14(1).mp4"; ChatGPT inspected overview and125ms frame samples around a turn. This supports qualitative visible delayed reorientation, NOT an authenticated animation-state/input onset measurement or exact shared video/log time.
Session2/3/4 end via expected five-second bound, verified Deinit and Up retention pass, Position writes0. Session3 had one25ms stale episode; do not claim all sessions perfectly continuous. Late candidate/accepted without completion near source timeout is NOT automatically a turn failure.
This measured result supersedes r2-runtime-UNKNOWN notes, within this limited scope.

## Fixed design
Change ONLY the existing post-entry reversal turn duration250ms ->120ms via one named constant in reversal_heading.hpp. This is a chosen experimental duration, not a measured match to vanilla animation.
Preserve33ms observation/confirmation, two-sample requirement, speed2, dot0.8, max gap250ms, equal-time WAIT, deterministic signed180degree rotation, exact endpoints/no drift/no mid-turn retarget, entry transition and all stale/session/identity/normal/A-B/Position/W/scale/metadata and five-second contracts.
Do not claim total input latency120ms: displacement detection and threshold crossing still precede the turn.
Use distinct v0.1c38r3/package metadata per existing version conventions. No new INI knobs/framework, input/animation APIs, assets, watches/hooks/VEH/VCH, producer, Position/velocity/input writes, c73, camera/adhesion or shared tooling changes.

## Files / checks
Allowed: helper constant, affected focused/native tests (timing expectations only; keep safety assertions), version/package metadata, small analysis and PROJECT_STATE/CODEX_RESULT updates and artifacts.
Verify pre120ms midpoint finite/proper/Up, exact endpoint at120ms and no early completion, repeated reversals, pause/freeze, zero/ambiguous hold, no changed acceptance thresholds. Run affected focused/portable, Windows build/smoke/native A-B and required canonical hooks checks per current docs. Keep skip/unavailable facts explicit. Review diff for accidental timing changes outside turn duration.
Do not remove regression assertions to fit a faster turn; adjust owned fixture timing to observe the new legitimate midpoint.
Commit/push and verify remote. No owner deployment, game-test prose or claim of animation synchronization.
Record supplied r2 evidence compactly in analysis and update state; raw logs/video stay private.
Return only STATUS / CHANGED / RESULT (checks, branch/commit/version) / NEED if blocked.
