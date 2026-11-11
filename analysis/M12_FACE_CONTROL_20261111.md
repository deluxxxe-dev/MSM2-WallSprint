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

## Task 1: learned face-walking mapping
VERIFIED-STATIC: camera manager matrix/position and the old sign vote are removed
from faceScreenAxes/faceAirDir/face zip. Basis is projected world-up plus
cross(normal,up). The64-entry ring stores raw stick angle, measured walking angle
and time; only pinned non-faceAir frames, raw stick magnitude>0.5 and planar
speed>1m/s train it. Frame anchors are the current Transform BEFORE the first
SetPosition call. All subsequent game calls contribute to the interval; our own
applied corrections are subtracted before fitting. Input belongs to that measured
interval. No single-call delta is divided by frame dt. Air/reapproach/landing and
>100ms observation gaps break the frame history.

Fit compares circular variances for mirror+1/-1 over <=20s samples only when
stick angles span>=60deg; ties/narrow spans retain the previous mirror (initial+1).
Rotation is the circular mean of <=2s residuals on each new walking sample. It
stays frozen without walking and expires after20s. Ring/fit are bounded; invalid
basis/input/step/dt and unrepresentable velocities refuse. No new camera reads,
searches, native hooks or input rewrite are introduced.

Takeoff uses measured walking velocity younger than0.3s, preserving its speed
and direction on the takeoff frame. Otherwise an eligible model supplies5m/s
along the current stick, otherwise zero planar launch. Air steering uses this
model; active-stick zip requires it. Neutral zip keeps the current planar
velocity (air velocity plus existing impulse), else adds no impulse; no camera or
arbitrary forward fallback remains. Neutral threshold remains0.2 from the old
air-control deadzone. A zip request count of0 is valid: the counter is snapshotted
at takeoff and the first subsequent request is processed, not discarded as an
initialization sentinel.

Each jump/zip logs stick,a_s radians,h,theta degrees,valid sample count/age,
model age,source walk/model/none and normalized e_up/e_side direction plus speed.
Neutral zip records velocity_fallback=1 and the carried velocity's walk/model
origin. camPos/heroPos fields and the per-process60-jump input log limit are
removed; these event logs emit once per action. No per-frame fit logging added.

Known limit (requested): camera turns while airborne or stationary cannot update
this mapping. It keeps the last walking mapping until fresh eligible walking;
no live-camera investigation is authorized here. Fit accuracy and actual game's
step pairing still need the owner video/log; owned fixtures are not game proof.

Owned actual-helper tests cover both mirrors and four cardinal directions,
60deg selection threshold/previous mirror, circular wrap/2s rotation/64-ring,
20s expiration, exact recent-walk speed vs5m/s model, no-trained-model straight
jump, raw-stick/speed/dt/basis refusals, later calls/own corrections/input pairing,
faceAir exclusion and neutral/model/unknown zip selection. ASan/UBSan PASS.
The accepted M11 budget helper and all of its threshold/aggregate tests remain
byte-unchanged (the obsolete camera-axis tests are replaced by the new model
fixtures). Pure Task2 tests remain in the portable suite.

Shared Wisdoms was consulted earlier in this session. No new reusable engine
fact is claimed from these cloud fixtures; no shared/SF/other repository edit.
