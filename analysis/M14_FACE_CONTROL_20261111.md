# M14 face control/state diagnostic — 2026-11-11 JST

Current task: docs/CODEX_NEXT.md. Read derived M13/M12 owner results and M10z
history; no raw owner video/log attachments were supplied in this cloud task.
Accepted baseline: all-direction walking, no reapproach loop, jerk-free stop,
walking jump travel~2m. New M14 runtime acceptance remains UNKNOWN. Do not tune
air-model constants or add falling/other-surface landing/rotation writes.

## Task 1 — first-call frame clock
Hero SetPosition call gaps<5ms now remain in one frame. Frame dt is elapsed time
between first calls; later2.0-2.2ms calls do not rebase it. Force/fall/follow/walk
and sample dt use that same actual frame interval. The old50ms integration clamp
is removed so these consumers no longer disagree; existing>100ms gap rebase still
refuses integration across stalls. Worker fallback advances on5ms elapsed time
instead of waiting for gaps between its1ms timer polls (not native Hero calls).

Per-hold max_intra_frame_gap_ms and min_frame_dt_ms are atomically reported in
WS_GRAVITY_FORCE_END. Rebasing preserves observed hold stats; a new force
generation resets them. Owned actual-helper ASan/UBSan tests cover native multi-
call frames, first-call intervals, min/max/rebase and worker/native mode switch.
Production Windows cross-compile passes; actual walking-down hitch/undo absence
is an owner check, not a cloud fixture claim.

## Task 2 — stop hop through face-air landing
A near-face non-jump hold now starts the existing face-air model with nominal
height0.5m (v=sqrt(2*33*0.5)), zero planar velocity/impulse and immediate game-stick
neutralization through the existing air flag. No A press is synthesized. Steering
and zip impulses are excluded during this stop hop. The common plane landing
branch is bypassed while faceAir is active; only FACE_JUMP_LANDED completes it.
At that landing the stop's first-landing timestamp arms the retained0.3s outward
limit. STOP_HOP takeoff/landing events include height and measured QPC airtime.
Air flags also clear at hold cancellation and publish the completed frame state.

No normal gravity/arc/timing constants were tuned. Nominal launch height is not a
claim about measured peak or detected game air state. Pure launch tests verify
normal speed/zero planar motion; existing air-isolation/landing-limit fixtures
and production cross-compile pass. Standing/exactly-one-landing/no REAPPROACH
remain owner checks. Owner must-have no-hover/face-gravity fall remains binding;
current M14 explicitly defers that implementation until Task5 state evidence.
