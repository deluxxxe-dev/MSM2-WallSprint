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

## Task 3 — bounded native-call air/fall facing experiment
New WALLSPRINT_WS_FACE_AIR_FACING: code default0, shipped1. Minimal drive-command/
telemetry/INI wiring; all writer logic stays in camera_ref. Targets use takeoff
planar direction (last walking direction when neutral), stop's existing WS travel
direction, fall's pre-signature walking velocity, or the current zip direction.
Unknown/degenerate directions refuse. At the first potential write, guarded
Transform0x00/0x10/0x20 reads must be unit-like, orthogonal and agree with up=n;
(right dot(up cross forward)) supplies the retained sign, never a guessed one.
Later inconsistent rows/sign refuse. Forward deviation>2deg permits xyz-only
12byte row stores, preserving every W bit in memory; no Position/scale/native
code changes. Compare current48bytes again inside the guarded callback, preflight
all written spans and recheck switch/F8/hold/generation/Hero/air-or-fall state.
Only the existing hero SetPosition detour may write (thread-local call scope),
including later calls in a frame; worker/mover paths cannot write axes.

4000 writes maximum per hold, retained across jump/zip retargets. Per-phase
WS_FACE_AIR_FACING summary gives committed writes, next-call overwrite count,
max deviation and terminal reason; zip changes target without resetting stats.
Landing repairs, when needed, happen before phase termination and facing readback;
after landing the writer is disabled and the game's rotation is left alone.
B/new WS/F8/normal cleanup end via atomic published counters. No restoration
write is added outside air/fall. A read failure drops overwrite continuity.
Owned ASan/UBSan fixtures cover both signs, W bits, deviation/overwrite counts,
4000-per-hold cap, phase/disabled/invalid-row/sign/direction refusal and required
INI defaults. An initial fixture indentation warning was fixed without weakening
assertions. Production cross-compile passes. Rendered orientation ownership and
actual next-call overwrites remain UNKNOWN; do not search another route if the
specified owner decision table rejects these rows.

## Task 4 — cache only a continuous steady walking window
A cached pair now requires>=0.3s of contiguous qualified samples, speed>2m/s
throughout, and pairwise circular span<10deg for BOTH stick and measured walking
direction across that whole window (including the boundary anchor). Gaps>0.1s,
slow or turning samples refuse; the prior stable pair remains. The64-sample ring
covers>=0.3s at the native>=5ms frame bound. Direct walking takeoff mapping remains
as accepted, but no longer overwrites the stable cache with a transient pair.
Non-walk takeoffs retain any-age stable pair, otherwise existing model fallback.
Frozen pair_steady_s is logged alongside pair_age_s; direct single-pair takeoffs
and regression explicitly carry0, not a fabricated stable duration.

Owned helpers cover initial0.3s coverage, turn refusal/retained prior pair, later
settling, slow refusal, stable any-age reuse and unchanged direct walking mapping.
The initial fixture reused an existing local name; renamed without changing
assertions. Production compile passes. Actual standing-jump/zip direction remains
an owner check; no camera search or regression retuning.

## Task 5 — separate actual A and H questions, with released-fall L
Independent comparisons: ground{F,W,G} vs A (each>=10), and ground vs H (H>=5,
ground labels>=10). J is reference-only, never pooled into the target or used as
a coverage requirement. New L=active released face fall is also reference-only.
Seven independent newest60 reservoirs retain420 process-local snapshots.
Each query has its own coverage/count and at most64 candidates, explicit
comparison/target_values. J/H/L examples are printed without affecting the selected
comparison's proof or partial-read/type validity; missing selected fields refuse.
The legacy air_values field is authoritative only in ground_vs_A; H query marks
it information_only and labels H_class=target_H, not a discovered air semantic.
Full selected sets remain checked (ground<=180,target<=60), no truncation.

WS_STATE_SUMMARY and two WS_STATE_COMPARISON lines distinguish insufficient
coverage from a covered empty list. L counts and values are explicit; no field is
named/written. Existing guarded50ms sampling, delayed J exclusion, B/60s/loss/
exit reports, W-vector correlations and Transform means stay bounded/read-only.
Owned tests cover independent A/H admission, H4 refusal/H5 admission, J overlap
with ground not blocking A, L reservoir/label and retained street coverage.
Initial new fixture indentation warning fixed without weakening assertions;
production compile and all affected actual-helper suites pass. Actual candidate
lists/engine ground-probe interpretation remain UNKNOWN pending owner log.
