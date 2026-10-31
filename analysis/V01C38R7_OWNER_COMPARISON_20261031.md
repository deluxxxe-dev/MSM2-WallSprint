# R7 owner ON/OFF comparison — 2026-10-31 JST

## Evidence
Owner supplied ON.log and OFF.log from v0.1c38r7; ON video supplied, OFF video intentionally omitted because owner observed ordinary wallrun and no special behavior. This review numerically inspected logs; it does not claim a new frame-by-frame video review.
Owner reports ON down -> left/right oscillation, lower diagonals -> corresponding horizontal travel; OFF ordinary WR, down -> left/right motion and other directions similar. Vanilla WR cannot perform desired arbitrary downward WS; do not request that as a test prerequisite.

## VERIFIED-RUNTIME: treatment selection and observation
ON startup requested_override=1; comparison mode=ON observation_admitted=1 source_admitted=1.
OFF startup requested_override=0; mode=OFF observation_admitted=1 source_admitted=0.
ON retained89 packets across3sessions, nonzero active publications and A/B commits (packet maxima publications892,a280,b279; these are maxima, not final global totals).
OFF retained121 packets across4sessions; ALL packets publications=0,a_commits=0,b_commits=0,publication_active=0,source_admitted=0.
Therefore OFF truly suppresses WS orientation-source effects while retaining observations. Do not call this a completely unmodded executable/all-other-mods isolation.

## Derived motion, not raw input
For each retained packet calculate degrees(atan2(tangent_world.y, hypot(tangent_world.x,tangent_world.z))). This is world vertical slope, not camera/stick angle; zero-length vectors give0 by atan2 convention.
ON session1 n32 range[-19.72,0], session2 n31[-12,0.55], session3 n26[-12,0].
OFF session1 n30[0,0.55], session2 n22[-12,0], session3 n32[-12,71.83], session4 n37[-12,71.79].
No retained packet in either treatment has slope below-20 degrees. Buffers are capped/overwriting; these are retained samples, not exhaustive session extrema. OFF four sessions do not map uniquely to3reported maneuvers. Do not invent exact input timestamp alignment.
Repeated -12-degree lower bounds in both modes are consistent with a native downward-angle limit and the authenticated Standard TurnMaxDown=12 name/value, but causality/units/active asset selection remain HYPOTHESIS.

## Conclusion / stopping rule
Desired downward locomotion is missing with override OFF as well. Orientation override alone is not a sufficient explanation for failure to descend. It could still alter facing/oscillation and feedback; no universal non-interference claim.
A displacement-only heading observer cannot create a downward movement target when game displacement remains nearly horizontal; faster interpolation or lower observer thresholds will not establish requested intent or native movement capability.
ON/OFF data acquisition accepted. No repeat ON/OFF run, no missing OFF video demand, no new scalar capture or Fast/Slow serial probe.
Next task should identify independent mapped direction source AND native down constraint consumption before a narrow independently implemented remedy. WSU is a reference lead only.
