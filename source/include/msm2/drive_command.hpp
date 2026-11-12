#pragma once
#include <atomic>
#include <cstdint>
// M6b: the WS stick-driven travel direction, written on the game thread by the WallRun pre-original callback
// (actor_writer) and read by the orientation worker (native_source_override) so the facing follows the
// commanded travel instead of a lagging displacement estimate. C++17 inline variables: one object program-wide.
namespace ws::drive {
inline std::atomic<float> dir[3]{};
inline std::atomic<std::uint64_t> ms{0};
inline std::atomic<std::uintptr_t> owner{0};
// M6g: hero position published by the orientation worker (pivot for the WS camera follow experiment).
inline std::atomic<float> heroPos[3]{};
inline std::atomic<std::uint64_t> heroMs{0};
// M6o: WS camera auto-centering (yaw follow + aim correction). Default OFF like the owner's ground camera setting;
// WallSprint.ini WALLSPRINT_CAMERA_AUTOCENTER=1 turns it on (ModSettings later).
inline std::atomic<bool> cameraAutoCenter{false};
// M8c: WS-only in-memory jump config write (YBoost 0 during WS). WallSprint.ini WALLSPRINT_JUMP_CONFIG_WRITE, default 0.
inline std::atomic<bool> jumpConfigWrite{false};
// M8g: hide the XInput A button from the game while WS is active (input-route test). WALLSPRINT_JUMP_HOLDBACK_TEST, default 0.
inline std::atomic<bool> jumpHoldbackTest{false};
// M8j: WS charge jump (hold A during WS, release to jump; strength from hold time). WALLSPRINT_WS_CHARGE_JUMP, default 0.
inline std::atomic<bool> wsChargeJump{false};
// M9b: charge launch by writing the hero velocity candidates. WALLSPRINT_WS_VELOCITY_WRITE, default 0.
inline std::atomic<bool> wsVelocityWrite{false};
// M9f: bounded +4 m/s velocity probe of search candidates during falls. WALLSPRINT_VELOCITY_PROBE, default 0.
inline std::atomic<bool> velocityProbe{false};
// M10c: gravity toward the wall for 1.5 s after a WS jump (gravity block write). WALLSPRINT_WS_GRAVITY_TEST, default 0.
inline std::atomic<bool> wsGravityTest{false};
// M10f: write the gravity block for the whole WS (not only after the jump). WALLSPRINT_WS_GRAVITY_DURING_WS, default 0.
inline std::atomic<bool> wsGravityDuringWs{false};
// M10o: WallRun wall normal (owner+0x398), published every WallRun update by actor_writer.
inline std::atomic<float> wallN[3]{};
inline std::atomic<std::uint64_t> wallNMs{0};
// M10x: WallRun wall contact point (owner+0x338; the hero stands ~0.5 m off it during WR), published with wallN.
inline std::atomic<float> wallP[3]{};
inline std::atomic<bool> wallPOk{false};
// M10q: real wall gravity after a WS jump: per-frame Transform::SetPosition correction on the game thread (world-y
// gravity removed, gravity along -wall normal added). WALLSPRINT_WS_GRAVITY_FORCE, default 0.
inline std::atomic<bool> wsGravityForce{false};
// M15 bounded face walk-off proxy/released-air integration; code default OFF.
inline std::atomic<bool> wsFaceWalkoff{false};
// M16 face-air/fall-only bounded Transform row experiment, code default OFF.
inline std::atomic<bool> wsFaceAirFacing{false};
// M17 camera mapping is read-only until walking validation admits a live basis.
inline std::atomic<bool> wsFaceCameraMap{false};
// M10q: L3 stand: while set, the left stick reads neutral (WGI and the WS stick drive) until the stick is re-pressed.
inline std::atomic<bool> stickMask{false};
// M10z-r9: drive the wall gravity through the hero mover (RVA 0x3519E10) desired position. WALLSPRINT_WS_GRAVITY_MOVER, default 0.
inline std::atomic<bool> wsGravityMover{false};
// M10z-r15: compute the hero position inside Transform::SetPosition (RVA 0x2CFC900). WALLSPRINT_WS_GRAVITY_SETPOS, default 0.
inline std::atomic<bool> wsGravitySetPos{false};
}
