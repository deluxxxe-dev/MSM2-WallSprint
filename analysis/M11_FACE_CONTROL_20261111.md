# M11 face controls — 2026-11-11

Input evidence: M10Z_R19_RESULT_20261111.md (derived owner log/video evidence;
no raw attachments supplied to this cloud task). Current task: docs/CODEX_NEXT.md.

## Task 1: position-derived screen axes
VERIFIED-STATIC: camera manager+0xD54 and Hero Transform+0x30 are read through the
existing guarded reader. Screen forward is normalized face projection of hero minus
camera position. Projection below 0.2 m retains the last valid direction in this
hold/camera-manager state; no valid history or invalid reads refuse the axes.
Screen right is cross(forward, normal), using the configured handedness until
walking votes determine it. Only pinned, non-faceAir game steps (before correction)
with lateral stick magnitude >0.7 vote; >=20 votes and >=80% majority decide once.
Near-zero lateral steps abstain; an undecided window is bounded to1000 votes.
New hold or manager resets history/calibration. A decision logs once; jump input
logs add camPos/heroPos/s. No matrix-row axis semantics are assumed.

Owned C++ tests compile the actual pure helper from camera_ref.cpp; geometry,
camera turns, degenerate/invalid positions, both handedness signs, 20-vote/80%
threshold and stick abstention pass with ASan/UBSan. Game acceptance is UNKNOWN.
Shared Wisdoms WALLSPRINT_INTERNALS.md was read; no new runtime engine fact is
claimed and no cross-repository files were changed.

## Task 2: per-frame downward budget
VERIFIED-STATIC: a >=2ms gap between consecutive force calls starts a frame;
frame dt still uses elapsed frame time with the existing50ms integration cap.
All negative world-y steps, first and subsequent calls alike, consume the same
6m/s*dt allowance. Stick <=0.25 or landing age <=0.3s allows zero downward
motion. Upward steps do not replenish it. Landings/reinitialization reset it.
The original desired value is retained exactly when within budget, preserving
sp2Detour's no-extra-store path. WS_FACE_MOVE_SAMPLE now covers first and later
calls, with call and remaining-budget fields (unchanged200-record cap).
Owned actual-helper tests cover multiple later-call walking steps, total cap,
neutral input, landing guard, no replenishment, next frame and dt cap. ASan/UBSan
pass. Actual smooth walking/no neutral slide remains UNKNOWN pending owner run.

## Task 3: deferred decision, no implementation
No landing-path relaxation or A injection was added. First capture the updated
normal SETPOS=1 behavior: after ~20s freeroam, WS -> stop three times; walk the
face in four directions; face jump up/down/left/right twice each, including after
a camera turn; web zip in four directions. Then exit the game, preserve the loaded
WallSprint.ini bytes, set only WALLSPRINT_WS_GRAVITY_SETPOS=0 in that loaded INI
(startup log identifies it, not the source/package INI), restart without another
Install mods (which can rewrite it), wait ~20s freeroam and WS -> stop three times.
Return the newest complete logs and videos with English titles, plus whether
SETPOS=0 stands or still falls. Restore the preserved INI after exiting.
- Stands with0: next task may temporarily pass original SetPosition values after
  WALL_LANDED until five zero-step frames or0.5s. Not implemented here.
- Still falls: next task needs1s normal/wall-up/side step logging and a real
  approach/three-frame blocked-normal landing experiment. Not implemented here.
No cloud claim of Windows deployment, actual camera semantics, grounding fix or
runtime acceptance. Exact-build/pdata/hash, F8, default-OFF code gates, INI and
mover editing0 remain intact. Final full-suite results recorded in CODEX_RESULT.

## Final cloud validation
All five requested commands exit0; portable38 PASS plus4 selector cases, smoke7
PASS, hooks907 PASS/5 SKIP/17 UNAVAILABLE. No retries/timing failures/skipped
steps. Current source tree e19184f640e3f06e69f41add5ebd66a07b86d991 is listed in
CLOUD_VERIFIED. Package CRC/member set/exact built DLL equality pass. Exact
commands/counts/hashes in M11_CLOUD_VALIDATION_20261111.json; owner instructions
and publication record in docs/CODEX_RESULT.md. Actual-game result UNKNOWN.
