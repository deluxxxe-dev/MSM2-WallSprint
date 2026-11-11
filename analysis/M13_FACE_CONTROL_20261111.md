# M13 face controls — 2026-11-11 JST

Current task: docs/CODEX_NEXT.md. Accepted derived evidence is
M12_OWNER_RESULT_20261111.md plus M11_OWNER_RESULT/M10z history. No raw video/log
attachments were supplied to this cloud task. Owner-accepted four-direction
walking and standing after3/3 stops must be preserved; M13 game acceptance is
UNKNOWN. No camera/body-yaw/web-line/charge/velocity/mover/L3 work is authorized.

## Task 1 — replace downward budget with fall detection
First and later game calls accumulate raw world-y changes over a frame, using
uncapped actual frame dt (not per-call dt). Ordinary downward input steps pass
exactly: no6m/s allowance or landing-age exemption remains. At >9m/s downward
frame speed or six consecutive >0.2m/s frame-to-frame increases, the frame's
already-passed downward parts plus the current downward part are undone; later
calls stay blocked for that frame. Neutral stick still passes no downward step.
Input threshold remains0.25 from the previous guard. Landings clear motion/trend
history but preserve current frame dt; new hold/gap resets are fail-closed.

Every pushed-stick undo logs WS_FACE_FALL_UNDO with frame speed/reason, up to100
per process. Move samples expose speed/rises/blocked instead of a remaining
budget. Owned actual-helper ASan/UBSan tests cover ordinary multi-call7m/s steps,
long-frame7.5m/s walking, whole-frame restoration on >9, sixth acceleration,
neutral and trend reset. This is source/fixture validation, not game evidence.

## Task 2 — follow quiet local wall, bound reapproach
Completed frame displacement subtracts WS-applied corrections before testing the
game's normal step, even with neutral stick. Five eligible pinned/non-faceAir
frames with <=2mm absolute normal step permit plane follow (authored rest
tolerance). Each frame removes25% of the rest distance, capped at0.05m; no follow
while in air, after invalid/gapped samples or actual normal motion. Cumulative
point changes>0.05m log event=follow. The hero's actual pre-call current position,
not the game's uncommitted desired position, determines rest/reapproach distance.

Reapproach now needs >0.3m for>0.2s; absolute QPC time enforces>=1s between attempts
in a hold, including after a model-gap reset. Reason/cooldown are logged. Near
0.18m rest adapts without reapproach. Pure helpers test quiet count, cap/sign,
normal motion/air invalidation, convergence and cooldown. Windows production
cross-compile passes; actual uneven-wall rest and jump landing remain pending.

## Task 3A only — shorter single-landing stop experiment
Push is0.5m for0.12s, including same-frame/final-call clamps. No landing is
reported during a controlled push. Stop approach normal velocity is capped4m/s;
a shared-frame normal-distance floor also prevents multiple SetPosition calls
from multiplying the inward approach. At the first stop landing, pinned outputs
are limited to plane_d<=0.02m for0.3s (both first/later calls), leaving tangents
unchanged. The landing contact/grace clamp is distinct from the airborne approach
speed cap. The first timestamp is not renewed by later landings. Face jumps
leave this stop-only policy; their launch/arc is not capped by it.

Owned tests cover speed/tangent preservation, repeated-call floor, grace timeout,
repeat-landing accounting and faceAir isolation. Task3A is an experiment, not an
accepted jerk fix: owner must count WALL_LANDED and inspect the three stops. Task3B
(0.6s/240-line trace after failed A) is deliberately NOT implemented before that
result. Camera/standing/jerk outcome remains UNKNOWN; no camera change made.

## Task 4 — directional jump speed and takeoff-relative trace
The qualified walking ring now records speeds; takeoff uses their mean over the
last0.3s, keeping the current known walking direction. Stick>0.5 with a known
walking/model direction has a5m/s floor. No known direction still means zero
planar launch. Steering turns the planar velocity without decaying its speed;
neutral input does not change it. The normal launch expression sqrt(2*33*1.8),
existing normal arc and landing integration are unchanged; actual height/airtime
remain owner measurements, not a new cloud acceptance claim.

WS_FACE_JUMP_TRACE logs takeoff/landing (or explicit timeout) displacement in
wall-up/side/normal relative to the actual takeoff position, full takeoff velocity,
original/current stick and original source; cap60 records per process. Source
walk/model/none and existing input logs remain. Owned helpers test mean vs one
sample, pushed floor, constant-speed steering through turns, zero-origin and
three-axis landing projection. Body yaw/visible "into screen" outcome remain
UNKNOWN until captioned owner video, not inferred from source vectors.

## Task 5 — stable mirror gate, steady samples and frozen air mapping
A separate takeoff snapshot stores h/theta/validity and QPC takeoff time. Only
face jump takeoff writes it. All air steering and zip directions use that snapshot;
mutating/expiring the live walking fit cannot change an in-progress jump. Zip/input
logs expose frozen_h/frozen_theta_deg/takeoff_time_s/mapping_valid plus agreement
counts. No live camera search or camera/zip-animation replacement is added.

Rotation's2s mean now uses only steady-stick samples (<20deg change from the
previous eligible frame, <=100ms gap). Mirror fitting also uses these steady
samples. Lower circular variance alone cannot flip h: at least30 agreeing inliers,
>=80% of eligible fit samples and>=60deg span among those inliers are required.
Agreement is defined as wrapped residual within20deg of the candidate circular
mean (authored tolerance, not an engine fact). Ties/narrow/short/noisy evidence
retain the previous h. All64-ring/20s/neutral/stale/untrained refusals remain;
speed averaging uses qualified walking samples even when their stick is changing.

Owned tests cover exactly30 inliers,30/40 rejection then80% acceptance, direction
transition exclusion, both mirrors/cardinals with held inputs, wrap/2s/64-ring/
expiry, a deliberately mutated live fit after snapshot and unchanged frozen
steering/zip. Prior fall/plane/stop/mean/trace fixtures still pass. Each task's
production camera_ref.cpp cross-compile used the existing warnings-as-errors
flags. Full final pipeline follows the fifth commit; no game acceptance is
claimed from these independently owned mathematical/fixture inputs.
