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

Step2: stop only extends the existing identity-pinned native XYZ axes writer
through landing+.4s, then projected normalised forward blend to current game
rows over.2s, up held at n and reorthogonalised with original handedness/lengths.
Takeoff uses actual Transform forward, not the WS travel command. Stop continuation
admits coherent orthogonal game rows whose up swung toward world up; ordinary
face-air admission remains unchanged. W lanes untouched,4000/hold cap/F8 retained.
Per-stop rotation summary counts writes and maximum incoming up/forward deviation.
Owned world-up/handedness/W/cap/blend tests and existing air-facing tests pass;
production Werror compile passes. Actual visual continuity remains UNKNOWN.

Step3: read-only first-native-call frame position tracker covers WS and the hold,
not only force-enabled calls (force starts after WS ends). Coherent atomic
publication lets the existing worker read last-frame velocity without racing
native tracker storage. Require same Transform and<=.1s freshness; unavailable
carry is zero and explicitly carry_valid0, never guessed from input. Project out
normal velocity. Normal hop arc unchanged; add exact integral of v0*(1-t/.35)
through the final truncated timestep, log v_plane0/valid and landing distance.
At5m/s, expected travel.875m; identity/gaps/integral tests and Werror compile pass.
No jump/zip/fall/mapping behavior change. Actual takeoff continuity is owner check.

Step4: native post-correction trace every second admitted model frame, first3
stop episodes only,<=60 lines each, through1.5s from last WS callback timestamp
(the existing WS-end convention). The hold begins after WS expiry, so initial
~.15-.25s has no fabricated stop trace. Logs plane distance, model planar speed
(labelled v_source=stop_model), corrected up/forward projections, raw game-step
normal/tangent magnitude and validity, writes and release block flag/count.
Unknown reads cannot manufacture pose evidence. Budget tests and Werror pass.
Decision: initial jerk -> b hop shape next; landing jerk -> c rotation; late jerk
-> d release guard. b is unchanged here. Owner: Update-WS -> Install mods -> Run;
WS-stop3x wait3s each, one video, latest versioned timestamped log. No idle wait.

## Cloud verification on final source tree

`build_windows.sh` PASS; `test.sh` **68 PASS**; `smoke_windows.sh` **7 PASS**;
`test_hooks_windows.sh` **907 PASS**,5 historical private-data SKIP and17
UNAVAILABLE (no owner runtime fixtures); `package.py` **2 PASS**. Final-source
hook attempts: 1. Exact per-attempt exits/log hashes/assertions are in the validation
JSON. No manual skips, fixture deadline extension or assertion relaxation.

Package: `source/output/MSM2-WS-v0.1c48/MSM2-WS-v0.1c48.script.disabled`.
CRC PASS; exactly info.json/WallSprint.ini/WallSprint.dll; packaged DLL equals
the new build and FACE_CAMERA_MAP/AIR_FACING/WALKOFF all1. DLL SHA256
`fc6e9510380411c761be66a0ce940018e2166b8ea044de9b84b1338f69a3872b`;
package SHA256 `65845dbea7777584e95aae4d061c718a5d0d30d7ad2eacd546b538d9f590b5a2`.
Artifact/log hashes identify cloud outputs; they do not replace real-game proof.

Final source tree `008123838be3cc594f5e364c64bb8af96a3a6c86`; task commits and QA persisted above.
Real-game owner acceptance remains UNKNOWN.
