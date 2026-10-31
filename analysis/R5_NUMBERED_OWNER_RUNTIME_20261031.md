# r5 numbered owner runtime — 2026-10-31 JST
Inputs: MSM2-WS-v0.1c38r5-20261031-111152.log and available11-12-49.mp4 (42.958333s). Video numbered1–5 lower-left; inspected sampled full sequence. Edited black separators/camera motion; do not force one linear video-to-log mapping or claim stick direction measured.
Owner mapping:
1 left/right succeeds.
2 up only reached from horizontal WS; up animation visually fine, diagonal-up lateral travel weak.
3 down doesn't face down; periodic horizontal flips.
4 upper diagonals partially work, lower diagonals fail like3.
5 RT held/stick neutral -> vanilla cling; re-input reactivates WS.
Video supports visible uphill body orientation and problematic lower movement/facing plus later cling. Numeric lateral responsiveness and button timing remain owner report, not independently measured input.

## Log facts
Session3 accepted world-space targets near up:02:13:07.232 forward(-4.019e-9,0.997723699,0.067435399), speed13.9904; completed07.358. General-plane target path therefore DOES execute.
Session4 (downward-test correspondence inferred from sequence/content, exact frame alignment unverified):
13.816/13.858/13.909/13.952 low_speed holds .129/.413/1.050/1.578.
14.138 accepted delta(0,0,+0.220336914) speed4.78993 -> target approx(0,0,+1).
15.995 accepted delta(0,0,-0.161712646) speed3.44069 -> target approx(0,0,-1).
17.828 accepted delta(0,-0.204101562,-0.960220337) speed20.88665 -> target(approx0,-0.207912102,-0.978147507): some downward component DOES exist; not globally zero.
19.048 accepted delta(0,0,+0.170776367) speed3.63354 -> horizontal+1.
21.205 accepted delta(0,0,-0.225021362) speed4.78769 -> horizontal-1.
Thus some sampled flips follow substantial ACTUAL horizontal displacement with zero vertical component, not demonstrated tiny noise and not solely speed<2. Merely lower speed threshold, deadband or turn duration cannot establish requested downward intent.
Session5 accepted targets sweep up and later back toward horizontal; actual input vector absent. Do not infer upper diagonal input45deg should equal measured45deg or blame12deg deadband for lateral speed.
Existing observer bases heading on displacement, not requested input. A/B orientation may affect original steering/contact coordinates (HYPOTHESIS); renderer animation ownership/downward movement cause UNKNOWN.
Lifecycle exit native Deinit must not be bypassed just to keep WS after neutral: stationary wall stance requires separate verified cling/idle design.

## Decision
Owner r5 game log proves new runtime loaded/executed; old deployment-unknown/tooling-next-test wording superseded for this run. Basic launch/deploy usability accepted, not every concurrency test.
Do not label complete multidirectional success. Prioritize downward movement/facing interaction before diagonal lateral response, then stationary WS posture. Input-triggered earlier START remains deferred.
Next task isolate existing original WallRun motion vs orientation override interaction using authenticated existing fields and a bounded evidence review/necessary diagnostics. No arbitrary velocity/Position patch, new hook/c73 or old repeated process diagnostics.
