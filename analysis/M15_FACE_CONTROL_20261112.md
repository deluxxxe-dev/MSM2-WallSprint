# M15 face control and diagnostic changes — 2026-11-12 JST

Task source: current CODEX_NEXT at ca47cb5; derived M14/M13 owner results read.
Wisdoms WALLSPRINT/WALLSPRINT_INTERNALS references consulted at f55e42d.
No raw owner logs/video or game execution in cloud. Runtime acceptance UNKNOWN.

## Task 1 — one gravity source
General integration is skipped in every frame scheduled to run face-air,
including the takeoff frame. Face-air integrates gravity and position through
one shared production helper, used by the owned ASan/UBSan numerical fixture.
Jump1.8m peaks around0.33s; hop0.5m around0.175s; normal return times are tested.
Stop hop height stays0.5m; no arc/animation timing tuning.
Other states audited: generic unpinned launch/reapproach/blend have one general
subtraction; pinned rest/walking discard general velocity and use game motion;
face jump and stop hop formerly got two and now get only face gravity; same-frame
callbacks integrate none; init/stall rebase integrate none. Air dash/zip add an
impulse, not another gravity source. Speed cap30 remains on general integration.
Initial new fixture compilation caught misleading indentation; formatting fixed,
no assertions weakened. Standing after stop remains owner acceptance.

## Task 2 — newest walking pair, retained through the hold
Every qualified model observation already requires stick>0.5 and planar game
speed>1m/s. Cache its own paired stick/walk angles immediately, with the existing
trusted mirror. Walk takeoffs retain their direct takeoff pair. Model/none
sources freeze the newest walking pair without age expiry; pair_age_s logs its
age. Regression is used only if no pair exists this hold. Pushed non-walk launch
velocity is reconstructed from the frozen pair too; neutral input invents none.
Owned helper tests cover stale model/none pairs, trust, cache replacement and
empty-history regression fallback. Production cross-compile passes.

## Task 3 — walking-speed acceleration is not a fall
The six-rising-frame rule additionally requires current downward speed>6m/s.
The >9m/s rule and same-frame cumulative undo are unchanged. Owned actual-helper
fixtures allow the measured3-4m/s run-start ramp and still reject a rising6.8m/s
sequence and the independent9m/s boundary. Production cross-compile passes.

## Task 4 — retained ground/air evidence
50ms cadence; newest60 snapshots independently per F/W/J/H/G/A (360 total).
No unlabeled raw buffer is retained. Three consecutive valid50ms samples with
world gravity/no hold/abs(vertical speed)>1.5m/s permit A in ascent or descent;
gaps/unknown gravity reset the run. J starts at actual takeoff+50ms; pending J
samples wait another50ms and are discarded if the recorded landing is too near,
so the last50ms of air is excluded without predicting a landing. Pending data
never enters counts/candidates/means. Older finalized J survives later jumps.
B,60s periodic, hero-loss and normal shutdown reports retain the M14 format.
Hero identity changes reset data; missing/unknown reads cannot fabricate labels.
Coverage requires ten samples for all six labels. Separate per-label reservoirs
preserve early street samples through later holds. Full ground union<=180/air<=120
fits the unchanged240-distinct-value sets; no silent truncation of separation.

Hero12byte vectors at4byte-aligned offsets are compared with each W snapshot's
native measured displacement direction. Require cosine>0.98 for>=90% of all W
snapshots, at least10; unreadable/nonfinite/zero vectors count as nonmatches.
Log<=16 candidate offsets with match/n; no field is interpreted or written.
Owned fixtures cover independent reservoirs, J trailing exclusion, A streaks,
vector correlation and existing separation/read/mean guards. An initial new
vector fixture assertion was placed after its own ring reset; moved before reset
without weakening the assertion. Production cross-compile and focused suites pass.
Engine semantics, actual coverage and move intent remain UNKNOWN.

## Task 5 — qualified walk-off release
New WALLSPRINT_WS_FACE_WALKOFF code default0/shipped INI1. Gameplay logic remains
in camera_ref; drive_command/telemetry/INI changes only wire the required setting.
FaceWalkFrame now exposes the completed raw game step even when unpinned, with
own corrections subtracted. The proxy uses native frames, not worker polls:
at zero-step onset retain the preceding ten-frame history with>=5 planar steps
>1m/s; require eight consecutive<1mm steps with stick>0.5. Stop hop/push and the
retained post-landing0.3s stop protection cannot arm it. Invalid/gap/neutral/air
history cannot arm; a new generation clears it. Carry the mean qualified planar
walking velocity, unpin and bypass the infinite-plane/stop/mover-sweep clamps.

General gravity integrates along unchanged substituted -u once; no world-y
launch term or desired game step is added to released motion. Total released
motion including the existing zip impulse is capped30m/s; dash decay is retained.
Same-frame callbacks freeze to current position rather than integrating again.
Three consecutive nonzero game frames (>=1mm) end release and pin a new plane
through the current game position with current u. This is a proxy, NOT proof of
another surface/contact/collision. B/new WS/F8/cleanup use existing endings;
release/land/end plus every-fifth-frame game u/tangent steps are logged,<=200
sample lines per release. Gravity substitution itself is not changed.

Owned actual-helper ASan/UBSan fixtures cover5-of10/8-zero trigger, neutral/
invalid/no-prior-walk refusal, three-step resume, planar carry, crossing the old
plane along -u, and total speed cap including zip. Existing model/landing tests
and production cross-compile pass. A stopped obstacle after qualified walking
can satisfy the same proxy: false-release behavior is UNKNOWN and explicitly an
owner test; no unsupported collision/contact discriminator is invented. H may
remain insufficient if release occurs before its ten-zero-frame diagnostic
threshold; label coverage is logged honestly. Other-surface landing stays later
scope, no collision guarantee or real-game success claim.

## Final QA — numerical and lifetime boundaries
The first numerical fixture used a1ms step. Strengthened it to actual grouped-
frame intervals5ms,60fps and30fps. Face-air position now uses the mean pre/post
normal velocity for constant gravity, avoiding semi-implicit height loss at
ordinary frame rates. Both1.8m and0.5m peaks/times pass the original tolerances;
launch/gravity/height constants are unchanged. This is numerical integration of
the single-gravity task, not new arc tuning. Released-air integration is unchanged.
Hero-loss reports also clear its identity so recovery at a recycled address resets
all reservoirs. A cancelled walk-off atomic prevents its local state continuing
through a new WS before generation reset. Required code-default0/shipped1 INI
wiring is asserted by the owned walk-off fixture. Published task commits remain
intact; final full suites certify the follow-up source tree.
