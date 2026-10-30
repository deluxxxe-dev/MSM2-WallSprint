# Reversal runtime result — 2026-10-30 JST

ChatGPT inspected owner log MSM2-WS-v0.1c38-20261030-230909.log and sampled frames from the accompanying 27.46-second video. Owner identifies first reversal as WS, second as vanilla WallRun. Do not equate video filename time with exact frame/log synchronization; no shared frame marker was supplied.

## VERIFIED-RUNTIME: logged movement versus Transform Forward
One WallRun session, same Hero/Transform identity in selected samples.
UTC timestamps (JST +9 h):
| Time | WS source state | Transform Forward | position-difference velocity |
|---|---|---|---|
|14:10:15.092|active|(0,0,-1)|(0,0,-15.47408)|
|14:10:16.752|active|(0,0,-0.999993)|(0,-0.04141,9.29982)|
|14:10:17.838|active|(0,0,-0.999993)|(0,-0.18137,19.39253)|
|14:10:18.392|active|(0,0,-0.999993)|(0,-0.09862,20.77267)|
|14:10:20.577|inactive|(0,0,-0.999993)|(0,0,-16.47893)|
|14:10:21.666|inactive|(0,0,0.999993)|(0,0,-0.89985)|
|14:10:22.220|inactive|(0,0,0.999993)|(0,0,5.31547)|
|14:10:23.313|inactive|(0,0,0.999993)|(0,0,17.27827)|

Active-source MOVEMENT_DELTA_SAMPLE independently changes delta_forward from positive before reversal to sustained negative afterward. Example 14:10:17.859 delta_world=(0,-0.0201568604,2.15512085), delta_forward=-2.15512133, valid=1, Position_writes=0.
Thus the active-source mismatch exists in numerical Transform observations, not merely visible animation. These worker snapshots do not prove every intermediate frame or animation ownership.

## Activity / stop
Guard admitted=1. Source overrides start14:10:14.669 and finish14:10:19.655.
Final summary14:10:23.679: duration9735ms; verified_deinit=1; A/B writes414/413; retention_pass=1 (Up only); stale suspend/resume0; source_exit_reason=bounded_test_window_5seconds; source_active_until_deinit=0; hard_kill_calls=1 with that expected bounded stop reason.
The later comparison is source-inactive within the SAME WallRun lifecycle, not a separately certified WS-unloaded/clean-process control.
The first and last Path A original_forward and target/applied_forward remain approximately negative Z. Incoming source can already contain prior orientation; do not assume it is an independent vanilla steering intention.

## Interpretation
Existing static review establishes entry-derived target retention without reversal retargeting. Runtime now establishes active-source displacement reversal with retained Transform Forward, followed by source-inactive forward reversal. Together these strongly implicate fixed orientation-source retention, but do not identify the engine's input/animation owner or prove that simply reusing incoming Forward fixes it.
No safe Position/movement write or new engine API is established. Runtime remains v0.1c38; C52 and all existing gates remain.

## Next
No repeat capture or serial config probe needed. ChatGPT must choose a bounded orientation-only correction and its reversal/low-speed behavior before implementation. Do not send Codex back through the same static review or c73.
