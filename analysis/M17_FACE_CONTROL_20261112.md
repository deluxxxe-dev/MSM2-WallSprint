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

## Task 2 — rolling game-down velocity
Replace four consecutive per-frame rises with overlap-weighted displacement/time
means over[now-0.1,now] and[now-0.2,now-0.1]. Both windows require complete native
coverage. Trigger current>3m/s and increase>1.5m/s, using actual QPC sample times;
invalid/gap/ineligible history resets. Log detect_delay_s from first full window
>1 in the current episode, and both means. If downward walking already exceeded1
for a long time, this literal episode age can include walking; it is not a
fabricated estimate of the physical fall onset. Steady<=6 does not trigger.

Launch carry is reconstructed from the newest ten qualified walking samples
BEFORE the first detection window, with a64-entry bounded history; insufficient
pre-window evidence gives zero, never the game's early fall velocity. The first
new noisy fixture exposed contamination by low-speed early-fall samples; the
pre-window selection fixes it and the same carry assertion passes. Formatting
warnings were fixed under Werror, without changing assertions/thresholds.
Owned tests cover noisy26m/s² onset/delay<=0.2, steady6m/s, gap refusal and
continued/deep/contact behavior. Production compile passes. Physical onset,
no false walking probes and the resulting contact classification remain owner
acceptance; probe safety is retained independently of detector noise.
