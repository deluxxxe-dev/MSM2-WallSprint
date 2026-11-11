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
