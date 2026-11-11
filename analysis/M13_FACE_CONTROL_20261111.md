# M13 face controls — 2026-11-11 JST

Current task: docs/CODEX_NEXT.md. Accepted derived evidence is
M12_OWNER_RESULT_20261111.md plus M11_OWNER_RESULT/M10z history. No raw video/log
attachments were supplied to this cloud task. Owner-accepted four-direction
walking and standing after3/3 stops must be preserved; M13 game acceptance is
UNKNOWN. No camera/body-yaw/web-line/charge/velocity/mover/L3 work is authorized.

## Task 1 — replace downward budget with fall detection
First and later game calls accumulate raw world-y changes over a frame, using
uncapped actual frame dt (not per-call dt). Ordinary downward input steps pass
exactly: no6m/s allowance or landing-age exemption remains. At >9m/s downward
frame speed or six consecutive >0.2m/s frame-to-frame increases, the frame's
already-passed downward parts plus the current downward part are undone; later
calls stay blocked for that frame. Neutral stick still passes no downward step.
Input threshold remains0.25 from the previous guard. Landings clear motion/trend
history but preserve current frame dt; new hold/gap resets are fail-closed.

Every pushed-stick undo logs WS_FACE_FALL_UNDO with frame speed/reason, up to100
per process. Move samples expose speed/rises/blocked instead of a remaining
budget. Owned actual-helper ASan/UBSan tests cover ordinary multi-call7m/s steps,
long-frame7.5m/s walking, whole-frame restoration on >9, sixth acceleration,
neutral and trend reset. This is source/fixture validation, not game evidence.
