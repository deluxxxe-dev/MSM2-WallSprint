# Next task — reversal START timing, not rotation speed
Branch: codex/v01c20-edi-value-provenance. Do not merge main.
Read AGENTS required documents, PROJECT_STATE, CODEX_RESULT, analysis/V01C38R3_TURN_DURATION.md and the r1/r2 facing implementation analyses.

## Goal / owner clarification (2026-10-31 JST)
Owner wants facing reversal to START approximately 1.1 seconds earlier relative to the current visible maneuver, NOT faster rotation or faster animation. 1.1 seconds is an owner-requested adjustment, not measured input latency. No new duration-only tuning. Current r3 reversal duration120ms is not accepted as the solution.
Owner also reports WS releases a few seconds after reversal, whereas straight traversal appears to remain active. Determine whether apparent persistence is native WallRun versus WS override; do not infer this from appearance alone.

## Supplied evidence / boundaries
Chat inspected supplied23.068633s video (02-10-47.mp4) and matching r3 log:
MSM2-WS-v0.1c38r3-20261031-020946.log.
Visible direction/facing mismatch is qualitative; no exact input timestamp or video/log-clock mapping exists.
Runtime log contains:
17:10:57.972Z NATIVE_SOURCE_EXIT session=1 reason=bounded_test_window_5seconds duration_since_source_entry=5.000000
17:11:00.241Z session1 lifecycle summary: source_exit_reason=bounded_test_window_5seconds; wallrun_duration_ms=7984.
17:11:17.830Z session2 same5.000000s bound.
Thus source override can stop while native WallRun continues. Whether either event maps to the supplied video release is UNKNOWN. Do not label reversal as timeout cause or claim straight override is unlimited.
Earlier r2 candidate-to-completion302–322ms measured only detector/turn stages, NOT input-to-facing latency. Current detector uses displacement after motion changes; earlier intent cannot be inferred from it without evidence.

## Task: narrow source review and instrument only missing boundaries
Trace only current runtime reversal observer, accepted heading publication and source exit path. Explain which existing conditions delay START (not turn completion), and whether existing read-only evidence exposes earlier directional intent. Identify actual callers and source locations; no speculative input/animation API or broad reverse engineering.
Record exact existing timer origin, whether reversal resets it, and distinction between source exit and native Deinit. Preserve the5s bound.
If existing diagnostics cannot distinguish these stages, add only bounded read-only diagnostics on already authenticated values:
- observation/candidate/accept/start/publication timestamps with session/update identity and rejection reason;
- longitudinal displacement/speed and existing heading signs;
- source-entry elapsed and source-exit reason / native Deinit.
Reuse current events where sufficient; no per-frame unbounded dump. No new hooks, pointer traversal, input capture API or generic framework.
Do NOT change gameplay behavior, turn duration, thresholds, confirmation count, entry delay, timers or animation. Do not implement a guessed minus1.1s offset.
Write a compact analysis with observed facts, static facts, unknowns, and ONE smallest proposed behavior change for Chat to judge, or exact missing evidence if no supported change exists.
Do not require raw full log re-analysis: provided excerpts establish only the timeout fact; finer measurements remain unknown.

## Allowed / forbidden
Allowed: targeted source reads, minimum diagnostics only if justified, focused regression tests, analysis document, CODEX_RESULT, PROJECT_STATE current-handoff correction.
Forbidden: movement/Position/velocity/input/animation writes; timer removal/extension/reset; further rotation speed changes; c73/new hooks; SF/other MOD/shared environment/profile edits; main merge.
Completed Steering Probe retirement is not a new task. Do not execute stale CODEX_TURN_SPEED.md again.
Preserve C52 and all A/B/native admission, stale and cleanup gates.

## Completion / validation
Record actual source paths/conditions and exact instrumentation changes. If runtime changes, follow existing version/package conventions, build and required focused/portable/native/smoke/release checks per current repository commands. For docs-only work do not rebuild unchanged binaries.
Commit/push and verify publication; update CODEX_RESULT with whether runtime bytes changed and exact log/version if changed. Automated checks do not prove owner game behavior.
Reply only STATUS / CHANGED / RESULT / NEED. Chat supplies owner update/test instructions.
