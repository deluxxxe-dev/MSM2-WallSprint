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
