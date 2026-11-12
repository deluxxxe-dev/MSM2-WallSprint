# M18 stop transition (P1 only), 2026-11-12 JST
Baseline d5998ce; read M17_OWNER_RESULT in full, task and unchanged repository
rules. No raw attachments/game execution in cloud. P2-P6 deferred. Seamless face
motion and retained changed gravity remain mandatory. No stop-hop shape change:
apex0.8m/return0.35s retained, owner observed0.355-0.359s and standing.

Step1: stop episode begins at admitted stop-hop/WS-end hold, blocks release
through landing+1.5s, then while raw stick<.2 AND no A edge since stop. Mandatory
window cannot be bypassed by A/movement; afterwards either expires it permanently.
Existing fall guard still runs. Once per episode log blocked frame count after
mandatory window (later total appears in trace); reset on new hold generation.
Owned lifetime/boundary tests and production Werror compile pass.
Wisdoms canonical3817f5e consulted; no new engine fact established in cloud.
