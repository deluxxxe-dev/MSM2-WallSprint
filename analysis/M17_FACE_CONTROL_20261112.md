# M17 safe contact / prompt detection / stop / live camera / position mirrors — 2026-11-12 JST

Task baseline b992148. Read both M16 owner-result parts and M15 derived results;
repository/CLAUDE rules remain unchanged. No raw owner attachments/game execution
in cloud. Both seamless movement and retained face-gravity fall remain mandatory;
new runtime acceptance is UNKNOWN until the requested owner run.

## Task 1 — contact-bounded probe and safe landing
A release starts a0.20s probe with at most0.10m cumulative -u travel; planar
motion remains. Clean declaration also needs fresh native game feedback at/after
0.20s: worker time alone cannot certify absence of contact. Each hero SetPosition
call, including later calls in one frame, checks the game's desired-current +u
step. During probe, push>0.01m at original plane_d<0 aborts, projects back to the
ORIGINAL plane and ignores fall detection1s. During full fall, push>0.01m lands
at the saved position BEFORE the previous model step, not the already penetrated
current point. Original-plane-only mathematical landing is retired: an infinite
plane is not a surface probe. Normal face-jump plane landing remains unchanged.

The quiet-game-fall rule cannot re-place a plane deeper than original_d=-0.05m;
selected contact is handled first, providing the explicit contact exception.
Face-jump and stop-hop landings suppress new releases0.5s. B/new WS/F8 and axes
writer scope are preserved. Contact stops model velocity and returns the native
position argument; no new engine call/ABI/collision guess. Probe clean, abort,
contact and original distance/push/budget are logged. Before-step is the bounded
rollback point, not a verified engine collision position. Genuine free fall and
ledge contact still require owner evidence.

Owned actual-helper ASan/UBSan tests cover cumulative budget/planar preservation,
native feedback before clean, push thresholds, rollback classification and deep
quiet-landing refusal. The production Werror compile initially found the retired
plane-only helper unused; removed that obsolete helper/test with its retired
behavior. All retained/new tests and production compile pass.
