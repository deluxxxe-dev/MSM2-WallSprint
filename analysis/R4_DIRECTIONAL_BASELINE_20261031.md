# r4 directional baseline — 2026-10-31 JST
Owner supplied043119 log;04-32-34 video unavailable, no visual/input clock proof.
OWNER-OBSERVED: left/right normal; up/down and diagonals already translate; animation/body remains horizontal; down and downward diagonals slower with unchanged animation rate; descending shows alternating left/right facing without apparent translation effect; stick release ends WS to vanilla wall cling.
Do not invent missing lateral locomotion: first goal is wall-plane facing, later downward speed/animation and idle stance.
VERIFIED-STATIC current source/include/Math/reversal_heading.hpp restricts targets to f0*sign (+/- saved entry Forward), dot thresholds+/-0.8, speed>=2, two33ms observations and120ms turn. Vertical/diagonal facing targets cannot be produced. This explains a design limitation, not full measured animation ownership.
VERIFIED-RUNTIME043119 session1 has accepted/completed sign changes with horizontal target_forward(approximately0,0,+/-1), even when delta has vertical components (19:32:42.110 accepted delta(0,0.416549683,-0.663391113)). Emitted time not exact input time.
Session1 summary:27.765s native wallrun,27.062s source; verified_native_Deinit,source_active_until_deinit1,retention_pass1,stale0,hard_kill0;1879 writes each A/B. No timeout failure.
No exact event->owner descending interval or stick-release timing mapping. Periodic flips and slow down-speed cause remain unproven. Do not assert pure vertical noise caused acceptance: logged accepted examples have substantial horizontal displacement.
Repair command earlier returned TOOLING_REPAIR_COMPLETE runtime_unchanged1; subsequent owner game run confirms launch usable at least once, not every concurrent lock scenario.

Decision: next minimal orientation-only wall-plane facing change; translation already exists. Preserve earlier INPUT->START issue as deferred. Stopping to vanilla cling is current lifecycle behavior, not failed stationary WS implementation. No speed/animation/lifecycle idle changes in same proof.
