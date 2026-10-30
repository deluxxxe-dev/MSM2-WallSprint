# r1 reversal runtime — 2026-10-31 JST

ChatGPT inspected MSM2-WS-v0.1c38r1-20261031-005638.log and sampled accompanying video frames. Owner confirms reversal now works, but initially moonwalks for approximately one second. Treat that duration as owner perception, not an exact input-to-facing measurement; input timestamps and exact video/log synchronization are absent.

## Measured UTC events
|Session / turn|first logged candidate|accepted|turn complete|candidate to completion|
|---|---|---|---|---|
|1 reverse|15:58:05.383|15:58:05.613|15:58:05.857|474 ms|
|1 forward|15:58:07.732|15:58:07.837|15:58:08.090|358 ms|
|2 reverse|15:58:15.011|15:58:15.124|15:58:15.377|366 ms|
|2 forward|15:58:17.687|15:58:17.800|15:58:18.040|353 ms|

First candidate is not necessarily first reverse movement or input. At session1 15:58:05.017 sampled velocity Z=+0.32519 while Forward Z=-0.999999; 05.561 velocity Z=+6.22223 and Forward still negative; 06.107 velocity Z=+12.31566 and Forward Z=+0.999999. Runtime orientation now reverses; near-zero position differences can oscillate and are not proof of stable intended reversal.

## Unexpected reset
Session1 05.383 candidate: update160, ms595392421, speed3.25698137.
05.394 observation_reset: update161, SAME ms595392421.
05.499 candidate again, then05.613 accepted.
Current Reversal::sample treats wallMs<=observationMs as reset. Distinct native updates can share the coarse millisecond timestamp. Static rule plus actual events identifies a concrete unnecessary pending-history reset. It is not a lifecycle stale event; both sessions report stale suspend/resume0.
Candidate resets may extend confirmation; this single example does not account for the entire owner-perceived delay.

## Runtime gates
Two sessions verified_deinit=1; Up retention_pass=1; A/B counts352/351 and361/362. Expected five-second source stop in both, not continuous full-session override. Position writes0. No animation/input ownership conclusion, clean-process comparison or new native certificate.

## Decision
Separate a correctness fix (equal-timestamp observations wait without resetting pending history) from controlled latency tuning (observation/confirmation interval100 ->33ms). Preserve speed threshold2, direction dot0.8, two-sample requirement, turn250ms and all guards. Test each separately before combined r2. No anticipatory input claim; measured displacement inevitably follows motion.
