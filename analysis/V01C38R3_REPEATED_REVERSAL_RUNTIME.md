# r3 repeated reversal runtime — 2026-10-31 JST
Source: owner-supplied MSM2-WS-v0.1c38r3-20261031-023446.log. Header v0.1c38r3/package0.1.59.
Video02-35-52.mp4 unavailable; do not claim visual synchronization.
Owner reports first reversal persists, further reversal releases WS; requests repeated reversal without release and earlier facing START, not faster rotation.

## VERIFIED-RUNTIME (bounded log evidence)
Session2 has THREE completed reversals within one native session:
candidate/accepted/complete active clock milliseconds:
437/484/609;2203/2250/2375;3937/3984/4109.
Each candidate->accept47ms, accept->completion125ms.
Exit17:36:09.622Z: bounded_test_window_5seconds, duration_since_source_entry5.000000.
Native lifecycle summary17:36:11.832Z: source_active_until_deinit0, wallrun_duration_ms7922; stale_suspend_count0.
Session4 TWO completed reversals:1187/1234/1359 and4281/4328/4453.
Exit17:36:30.703Z: same5.000000s bound; native summary wallrun_duration_ms7344; stale_suspend_count0.
Sessions1,3 also log same5second budget exits. Session1 had10 stale pairs/max1385ms; do not ignore this separate issue or remove stale gates.
Emitted UTC is not precise frame timing; source/native-clock fields above are internal observer timing.
No reversal-count failure is shown in sessions2/4. Current arbitrary budget is an established cause of override closure, but exact video release correspondence remains UNKNOWN.
No source change since duration-only r3; review commit was docs-only. Earlier input-to-START response improvement is NOT established.

## Decision
Next change isolates session lifetime: remove mandatory5second experimental expiry from normal WS while maintaining authenticated per-update/session validity and emergency closures. Do not reset budget on reversal as a workaround. Do not touch facing detector or duration in same experiment.
Earlier directional-intent evidence remains a separate blocked item; no new repeated owner test solely to remeasure old displacement signals.
