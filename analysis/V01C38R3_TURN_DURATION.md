# v0.1c38r3 — post-entry reversal duration-only experiment

2026-10-31 JST. Runtime v0.1c38r3 / package0.1.59.
Baseline b0683a5af79a522d596937c842321ea420f1b8ac, retirement implementation
published and independently verified BEFORE runtime edits. No main merge.

## Supplied r2 runtime evidence / boundaries

ChatGPT inspected owner's v0.1c38r2-20261031-012913 log; six completed turns across
sessions2/3/4 had logged candidate-to-completion302/304/302/303/322/303ms.
Candidate-to-accepted41–54ms; accepted-to-completion248–270ms. Log timestamps
are not identical to internal active clock duration. Owner still perceives facing
lag. Supplied42.918s clip was reviewed by ChatGPT at overview/125ms frame samples;
this is qualitative delayed reorientation, not authenticated input/animation onset
or precise video/log clock alignment. We did not independently inspect private
raw log/video in cloud and do not redistribute them.

Sessions2/3/4 stop at expected five-second bound, verified Deinit/Up retention,
Position writes0. Session3 has ONE25ms stale episode; continuity is not universal.
Late accepted/candidate without completion near bound alone is not turn failure.
This bounded evidence supersedes r2-runtime-UNKNOWN notes, without new claims of
animation/input ownership, units, writability or general movement support.

## Exact change / preserved contracts

One named helper constant reversalTurnDurationMs=120 replaces divisor250 only
for post-entry reversal progress. This120ms is a chosen experiment, not a measured
match to vanilla animation. Total input latency still includes displacement
onset/threshold crossing and two eligible observations before accepted turn.

Keep observationIntervalMs33 for observation/confirmation; two consecutive
eligible new-sign samples, speed>=2, dot+/-0.8, maximum gap250, same-time WAIT,
regression/stale/session/identity handling. Positive180degree wall-normal rotation,
proper finite midpoint/Up, exact f0/-f0 endpoints, no drift/mid-turn retarget remain.
Entry transition (including0.25s), diagnostic sample100ms, native A/B source/store/
Position/W/metadata/scales, source timeout/five-second budget and C52/write gates
are byte-unchanged. No new input/animation API, INI, hook/watch/VEH/VCH, producer,
c73, assets, camera/adhesion/Position/velocity/input write or shared-tooling edit.
Tooling prerequisite is isolated in earlier published commits, not interleaved.

## Focused/native timing evidence

Retained acceptance test: baseline0, candidate33, duplicate33 no vote, acceptance66.
Midpoint at126 (=66+60) proper/finite with retained Up;185 (=66+119) still turning;
186 (=66+120) exact endpoint. Other fixtures use start200/mid260/end320. Twelve
reversals use120ms endpoint waits, preserving exact diagonal/metadata assertions.
Existing entry midpoint125 over250ms remains unchanged. Same-time duplicate loop,
zero/ambiguous holds, regression/gap/stale/session and pause/freezing remain.

Owned native fixture samples real worker cadence until at least50ms into120ms
turn (previously110 into250), before completion; unchanged A/B midpoint stores,
Position/W/compact metadata, real120ms stale pause, resume clock freeze and Deinit
assertions. No safety assertion removed, no production clock manipulation.

Commands/results/publication in CODEX_RESULT. Actual r3 game effect and animation
synchronization UNKNOWN; cloud tests cannot supply them. No owner game deployment
or human game-test prose produced; future movement roadmap is planning only.
