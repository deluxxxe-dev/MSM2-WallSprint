# M12 face controls — 2026-11-11 JST

Evidence: M11_OWNER_RESULT_20261111.md and the referenced M11/M10z derived
reports. No raw log/video attachments were supplied in this cloud task. The
accepted M11 downward frame budget is unchanged. Runtime/game acceptance of M12
remains UNKNOWN until the specified owner run.

## Task 2 (implemented first): fixed plane and stop approach
VERIFIED-STATIC: the worker maintains an on-WS candidate point/source. At hold
start only, the last candidate is copied into gfP0 before publishing the new
force generation. It stays fixed through air/landing; new WS may prepare the next
candidate without overwriting an existing airborne hold's plane. Hold start and
every landing log point/source; a >0.05m change detected by force calls logs a
separate plane event.

Pinned non-faceAir state >0.15m from this plane for >0.1s clears the pin and
continues the existing wall-gravity integration/landing rule. Near-plane, airborne,
face-jump or invalid/gapped timer samples cannot carry the off-plane timer.
WS_GRAVITY_FORCE_REAPPROACH records the rest distance. No grounding/standing
animation claim follows from these source changes.

Stop-push output is projected to0.9m from the plane on first AND later calls,
including the last push frame and after impulses. This prevents reused same-frame
normal correction from adding another0.9m on each SetPosition call. Tangent
components are preserved. The accepted downward walking helper is byte-unchanged.

Owned tests compile actual pure helpers with ASan/UBSan: repeated/overshooting
pushes, tilted normal, preserved tangents, sustained reapproach threshold/reset,
face-air ineligibility and plane-change threshold PASS. Existing M11 actual-helper
tests PASS, and camera_ref.cpp passes production Windows cross-compile flags.
Final full-suite checks follow Task1; this intermediate commit is not a complete
M12 deployment/test handoff.
