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

## Task 3 — neutral on the press read, sampled facing/arc
Raw WGI stick is published before the A edge counter. A physical face press
latches neutralization in that same read; A still passes to the game. Air and
post-landing100ms keep it neutral, with explicit no-hold/active-WS/F8/cancel
passthrough/cleanup. No rotation write or A injection is added.

WS_FACE_JUMP_FACING (40 records/process) reads existing Transform+0x20 forward
with guards, projected on face-up/side at WGI press, takeoff+>=0.1s, landing and
zip. The press's walking direction comes from bounded atomic publication of the
last same-hold frame direction; generation and read-valid flags distinguish
unavailable evidence. Later records carry that press direction. Peak delta_n
and its QPC age are sampled from actual pre-call Hero positions, not a claim of
continuous analytic peak. Logs name source=transform_0x20 and phase explicitly;
this is not independent rendered-body/animation facing proof.

Pure policy/peak tests and production cross-compile pass. Initial portable helper
compilation missed <cstdint> in its owned harness; the include was corrected,
then tests passed with assertions unchanged. Runtime decision remains: retained
walking-facing vs side turn independent of stick; the latter requires a later
explicitly authorized rotation task. No outcome is inferred from cloud fixtures.

## Task 4 — walking-pair takeoff mapping for zip
A source=walk takeoff with a non-neutral raw stick and a finite planar velocity
constructs theta_t directly from that one pair. h uses the current mirror at>=80%
agreement, otherwise the last mirror that met80% in this hold (initial+1).
Confidence history is updated during walking, not only at jump presses.
Source=none standing takeoffs reuse a valid pair younger than10s; otherwise the
current model remains the fallback. Neutral/zero-vector pairs cannot poison the
cache. Everything is copied into the jump's air snapshot, with mapping_source,
theta_takeoff_deg and h_used logs; facing is sampled at zip as well. No air speed,
arc or game zip/body/camera behavior is altered.
Owned helpers test same-direction reproduction, untrusted-current/last-trusted
mirror selection, initial+1, recent-pair expiry and model fallback; production
Windows cross-compile passes. Zip direction/game yaw acceptance is pending.

## Task 5 — read-only labeled state snapshots
Under existing exact-build/installed-hook admission, wsGravityTest INI and F8
gates, a250ms scheduled worker cadence captures Hero[0,0x1000) in256-byte guarded
blocks, stopping at the first unreadable block, plus Transform[0,0x100). Prefix
size is logged once per Hero/Transform identity. A fixed240-slot ring retains
only process-local bytes, clears on identity changes, and never dumps raw buffers.

F/W/J/H/G/A are authored labels from hold/pinned/raw-stick/air-age and positions,
not names for a verified game field. No-hold vertical gravity uses a guarded read
of the already-identified Hero gravity-up block; missing evidence leaves G/A
unlabeled. The existing exact SetPosition hook records current frame positions
with the5ms grouping even without force; its kinematics subtract WS-applied
corrections. G quiet-y and H pushed-zero require ten actual native frames, not
worker polls. Contention drops/invalidate diagnostics instead of blocking game
execution. Stale/identity/gap metrics refuse dependent labels. A y speed is from
worker position differences. F/J do not fabricate missing frame measurements.

B reports cached label counts and again normal camera_ref::stop reports at exit.
Force stops before report computation. Reports require>=4 each F/W/J/G/A before
emitting field candidates; missing coverage is explicit, distinct from a fully
covered empty result. All recorded ground{F,W,G}/air{J,A} values must be disjoint.
H values/class are separate. Candidate types are heuristic byte flag (u8 at each
4-byte offset), signed-small-int/finite-float (u32) or canonical-pattern pointer-
like (u64, no dereference). Pointer-like is not a pointer/provenance certificate.
Fields missing in any labeled snapshot refuse. Rank by fewer distinct values,
then offset; at most64 candidates/report. Display at most8 example values plus
exact distinct count, but separation checks the full retained sets. Finite J/W
Transform means and per-component counts occupy at most16 rows/report. No field
names/ground-state semantics, gameplay decisions, writes, watches, ABI or new
hooks are inferred or enabled by these candidates.

Owned actual-helper ASan/UBSan tests cover first-unreadable/overflow boundaries,
250ms cadence without duplicate catch-up copies, six labels/ten-frame counts,
own-correction subtraction, disjoint flag/float/pointer-like examples/H sets,
overlap/partial/coverage refusals, means and240 capacity. Two misleading-
indentation Werror findings were corrected before passing production compile;
no assertion was relaxed. All prior face/frame/landing helpers pass. Actual
label coverage and real candidate offsets remain UNKNOWN pending owner run.

The owner's simultaneous must-have clarification62163ef was merged from the
same development branch (merge880d5ea), preserving both seamless motion and
face-gravity falling until new WS/B. Current M14 still explicitly defers falling/
other-surface collision/landing/rotation writes until this evidence; those goals
are not claimed solved by snapshots or the retained infinite-plane behavior.
