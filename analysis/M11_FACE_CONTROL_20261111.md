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
