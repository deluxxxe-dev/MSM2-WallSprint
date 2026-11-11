# M16 face fall / hop / facing / walking pairs / state evidence — 2026-11-12 JST

Task baseline e660a62. Read M15/M14 owner-derived analysis, current CODEX_NEXT,
CLAUDE and repository instructions. No raw owner attachments/game execution here.
Both seamless movement and retained face-gravity falling remain mandatory;
new implementation acceptance is UNKNOWN until the requested owner run.

## Task 1 — measured game fall signature
Replaces the zero-step release/nonzero-resume proxy. Completed native-frame game
steps remove all own position corrections. Four consecutive world-down speed
samples>6m/s rising>=0.25m/s each release from pinned/non-face-air/non-stop-hop;
invalid or ineligible frames reset the signature. Carry comes only from walking
samples before the signature (speed<=6), never the accelerating world-down fall.
Released calls ignore desired game position, integrate existing33m/s² along -u,
keep total velocity/zip cap30, bypass plane/fall guards, and freeze later calls
in a frame exactly as face air. No new gravity substitution or native signature.

Continued nonzero world-down motion cannot mean ground. Three valid game-down
speeds<2m/s end the fall with reason=game_fall_ended, pinning current position/u.
This is the specified ground-probe HYPOTHESIS, not established collision.
Original-plane landing additionally requires motion closing to that plane and
abs distance<=0.02m, so moving away below it cannot immediately re-pin. No other
surface rule is invented. B/new-WS/F8 cleanup is retained. WS_FACE_FALL reports
release/land/end time, up/planar speed, plane distance; every-fifth-frame raw game
y/up/planar step trace is capped300 lines per hold. Owned ASan/UBSan tests cover
four rising frames, steady/zero/invalid refusal, continuing world fall, slow-
three-frame ending, closing-plane guard and released physics/carry/30m/s cap.
The initial fixture counted the threshold-crossing frame twice; its explicit
four-frame sequence was corrected, without changing production thresholds.
Production cross-compile and unchanged normal jump physics tests pass.

M15 owner result corrects a historical broad claim: game-owned airborne motion
under substituted up can still fall in WORLD down; only the observed WS face-
jump path had zero game steps. Preserve this distinction in shared knowledge.

## Task 2 — solve a small stop hop from actual start distance
For h=0.15,T=0.35, let r=sqrt(2h)+sqrt(2(h+d0)); then g_hop=r²/T²,
v=sqrt(2h*g_hop). Use the measured near-face d0 (finite0..0.8m; out-of-domain
starts refuse this hop), not a fixed0.5m launch height. Stop hop alone uses this
gravity, exact last integration interval and plane landing at0.35s. Normal face
jumps keep33 and their existing landing condition. Initial/takeoff and landing
logs include d0,v,g_hop and actual airtime. Planar/dash remain zero; no injected A.
Owned helper tests cover d0=0,0.45,0.79, apex relative height and solved landing,
invalid/negative/far starts and preserved normal-jump physics. Production compile
passes. Real standing/no-visible-shift acceptance remains UNKNOWN; if crawl
returns, report it rather than tuning height (airtime is the next variable).
